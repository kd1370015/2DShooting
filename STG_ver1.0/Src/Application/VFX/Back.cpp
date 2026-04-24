#include "Back.h"

// ステージごとの背景設定データ構造
struct StageBackConfig {
    int layerCount;
    const char* texPaths[MAX_BACK_LAYERS];
    float ratios[MAX_BACK_LAYERS];
};

static const StageBackConfig kStageConfigs[] = {
    {
        4, // ★ 6から4に変更（実際に使う数に合わせる）
        {

            "Texture/Back/Sky/4.png",
            "Texture/Back/Sky/3.png",
            "Texture/Back/Sky/2.png",
            "Texture/Back/Sky/1.png",
            nullptr, nullptr, nullptr, nullptr, nullptr // 残りはnullptrで埋める
        },
    // 視差係数も4つにする。残りは0で埋める
    { 1.0f, 0.75f, 0.5f, 0.25f, 0, 0, 0, 0, 0 }
},
};
void C_Back::Init(int stageIdx)
{
    // 安全策：範囲外なら0番目を参照
    if (stageIdx < 0 || stageIdx >= 6) stageIdx = 0;

    const auto& config = kStageConfigs[stageIdx];
    m_activeLayerCount = config.layerCount;

    for (int i = 0; i < MAX_BACK_LAYERS; i++)
    {
        m_pos[i] = { 0, 0 };

        if (i < m_activeLayerCount)
        {
            m_parallaxRatio[i] = config.ratios[i];

            // テクスチャのロード
            if (config.texPaths[i] != nullptr)
            {
                m_tex[i] = new KdTexture();
                m_tex[i]->Load(config.texPaths[i]);
            }
        }
        else
        {
            m_tex[i] = nullptr;
        }
    }
}

void C_Back::Update(float totalScrollOffset)
{
    for (int i = 0; i < m_activeLayerCount; ++i)
    {
        // 累積移動量に係数をかける
        float backgroundOffset = -(totalScrollOffset * m_parallaxRatio[i]);

        // fmodf で 0 ～ -1280 (ScreenWidth) の間に収める
        m_pos[i].x = fmodf(backgroundOffset, (float)ScreenWidth);

        // 1枚目：現在の位置
        m_mat[i][0] = Math::Matrix::CreateTranslation(m_pos[i].x, 0, 0);

        // 2枚目：1枚目の後ろ（右側）にピッタリくっつける
        // 座標がマイナスに流れていくので、+ScreenWidth するだけでOK
        m_mat[i][1] = Math::Matrix::CreateTranslation(m_pos[i].x + ScreenWidth, 0, 0);
    }
}

void C_Back::Draw()
{
    // 重要：背景なので奥のレイヤーから順番に描画する (降順ループ)
    for (int i = m_activeLayerCount - 1; i >= 0; --i)
    {
        if (!m_tex[i]) continue;

        for (int j = 0; j < BackTextureNum; ++j)
        {
            SHADER.m_spriteShader.SetMatrix(m_mat[i][j]);
            SHADER.m_spriteShader.DrawTex(m_tex[i], { 0, 0, ScreenWidth, ScreenHeight }, 1.0f);
        }
    }
}