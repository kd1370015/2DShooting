#pragma once

// 背景レイヤーの最大数
static const int MAX_BACK_LAYERS = 9;
static const int BackTextureNum = 2; // 無限ループ用に2枚交互に並べる

class C_Back
{
public:
    C_Back() {}
    ~C_Back() {}

    // 初期化：ステージ番号を受け取り、設定データからテクスチャをロード
    void Init(int stageIdx);

    // 更新：累積スクロール量を受け取り、背景の表示位置（行列）を計算
    void Update(float totalScrollOffset);

    // 描画：有効なレイヤーを奥から順に描画
    void Draw();

private:
    static const int ScreenHeight = 720;
    static const int ScreenWidth = 1280;

    int m_activeLayerCount = 0;

    // 各レイヤーのデータ
    KdTexture* m_tex[MAX_BACK_LAYERS];
    Math::Vector2 m_pos[MAX_BACK_LAYERS];
    Math::Matrix  m_mat[MAX_BACK_LAYERS][BackTextureNum];
    float         m_parallaxRatio[MAX_BACK_LAYERS];
};


