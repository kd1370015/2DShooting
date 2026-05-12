#pragma once

// オーブの種類
enum class OrbType {
    Blue,   // Speed (弾速・連射)
    Red,    // Power (威力・サイズ)
    Yellow  // Multi (弾数)
};

class C_Orb {
public:
    C_Orb() {}
    ~C_Orb() {}

    // 初期化：位置と種類を指定
    void Init(Math::Vector2 pos, OrbType type);

    // 更新：吸い寄せ処理などを書く場所
    void Update(const Math::Vector2& playerPos);

    // 描画
    void Draw(KdTexture* tex);

    // ゲッター・セッター
    Math::Vector2 GetPos() const { return m_pos; }
    OrbType GetType() const { return m_type; }
    bool IsAlive() const { return m_alive; }
    void Kill() { m_alive = false; }

private:
    Math::Vector2 m_pos;
    OrbType m_type;
    bool m_alive = true;

    float m_speed = 3.0f;       // 吸い寄せスピード
    float m_targetDist = 200.0f; // 吸い寄せが始まる距離
};
