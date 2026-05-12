#pragma once

class C_Explosion {
public:
    C_Explosion() {}
    ~C_Explosion() {}

    // 初期化：場所を指定
    void Init(Math::Vector2 pos);

    // 更新：アニメーションのコマを進める
    void Update();

    // 描画：現在のコマを描画
    void Draw(KdTexture* tex);

    // 寿命が尽きた（アニメーションが終わった）か？
    bool IsExpired() const { return m_isExpired; }

private:
    Math::Vector2 m_pos;           // 位置
    int           m_animeTimer;    // アニメーション用カウンター
    int           m_currentFrame;  // 現在のコマ番号 (0～9)
    bool          m_isExpired;     // 終了フラグ

    // スプライトシートの設定（画像に合わせて調整）
    const int   ANIME_MAX = 9;    // 全10コマ
    const int   ANIME_SPEED = 3;   // 3フレームごとにコマを進める（小さいと速い）
    const float FRAME_W = 96.0f;  // 1コマの横幅（画像を全幅÷10で計算）
    const float FRAME_H = 96.0f;  // 1コマの縦幅（画像に合わせて調整）
};
