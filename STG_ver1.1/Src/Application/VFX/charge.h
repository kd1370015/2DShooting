#pragma once

class C_Player;

class C_Charge {
public:
    // コンストラクタで「枠」のテクスチャを保持
    C_Charge(KdTexture* tex, KdTexture* lowTex, C_Player* player);
    ~C_Charge() {}

    void Update();

    // 描画時に「中身」と「アイコン」も受け取るように変更
    void Draw(KdTexture* gaugeTex, KdTexture* iconTex);
private:

    C_Player* m_player = nullptr;
    KdTexture* m_tex = nullptr; // 枠用 (charge_barframe.png)
    KdTexture* m_gaugeLowTex = nullptr; // 赤いゲージ用 (charge_barlow.png)

    Math::Vector2 m_pos;
    int m_currentX = 0;

    // 画像のサイズ設定（アップロードされた画像サイズに合わせる）
    // もし画像サイズが異なる場合はここを調整してください
    const int GAUGE_FULL_W = 272;
    const int GAUGE_H = 21;

};
