#pragma once

// 爆発エフェクト1つ分を管理するクラス
class F_Explosion {
public:
    F_Explosion() {}
    ~F_Explosion() {}

    // 初期化：発生場所を指定
    void Init(Math::Vector2 pos);

    // 更新：アニメーションの進行管理
    void Update();

    // 描画：現在のコマを描画
    void Draw(KdTexture* tex);

    // 寿命チェック：アニメーションが終わったらtrue
    bool IsExpired() const { return m_isExpired; }

private:
    Math::Vector2 m_pos;            // 発生位置
    int           m_animeTimer;     // コマ送りのタイマー
    int           m_currentFrame;   // 現在の表示コマ (0～)
    bool          m_isExpired;      // 終了フラグ

    // スプライトシートの設定
    const int   ANIME_MAX = 8;     // 最大コマ数
    const int   ANIME_SPEED = 3;   // 何フレームで次のコマへ進むか
    const float FRAME_W = 64.0f;   // 1コマの幅
    const float FRAME_H = 64.0f;   // 1コマの高さ
};