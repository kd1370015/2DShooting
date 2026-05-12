#include "LevelUp.h"
#include "Application/Player/player.h" // 座標取得のため

void LevelUp::Start(const Math::Vector2& pos) {
    m_pos = pos;
    m_active = true;
    m_frameIdx = 0; // 開始位置にリセット
}

//void LevelUp::Update() {
//    if (!m_active) return;
//
//    // 時間経過でコマを進める
//    m_frameIdx += m_anmSpeed;
//
//    // 最後のコマを超えたら消滅
//    if (m_frameIdx >= m_maxFrame) {
//        m_active = false;
//    }
//}




void LevelUp::Init(KdTexture* tex, C_Player* player) {
    m_pTex = tex;
    m_player = player;
    m_active = false;
}

void LevelUp::Update() {
    if (!m_active) return;

    // アニメーション更新
    m_frameIdx += m_anmSpeed;

    if (m_frameIdx >= m_maxFrame) {
        if (m_isLoop) {
            m_frameIdx = 0; // ★ループ：最後まで行ったら最初に戻る
        }
        else {
            m_active = false; // ループしない場合は終了
        }
    }
}

void LevelUp::DrawSprite() {
    if (!m_active || !m_pTex || !m_player) return;

    // --- 最新のプレイヤー座標を取得して描画 ---
    Math::Vector2 pos = m_player->GetPos();

    D3D.SetBlendState(BlendMode::Add);

    int srcX = (int)m_frameIdx * m_pieceSize;
    Math::Rectangle srcRect = { srcX, 0, m_pieceSize, m_pieceSize };

    // 2.0倍程度がおすすめ
    Math::Matrix mat = Math::Matrix::CreateScale(2.0f) * Math::Matrix::CreateTranslation(pos.x, pos.y, 0);

    SHADER.m_spriteShader.SetMatrix(mat);
    Math::Color color = { 1.0f, 1.0f, 1.0f, 1.0f };
    SHADER.m_spriteShader.DrawTex(m_pTex, 0, 0, &srcRect, &color);

    D3D.SetBlendState(BlendMode::Alpha);
    SHADER.m_spriteShader.SetMatrix(Math::Matrix::Identity);
}