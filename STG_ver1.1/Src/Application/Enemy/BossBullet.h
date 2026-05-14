#pragma once
#include "Application/Core/main.h"

class C_BossBullet {
public:
    C_BossBullet() {}
    ~C_BossBullet() {}

    // 初期化（座標、角度、弾速などを指定）
    void Init(Math::Vector2 pos, float angle, float speed = 8.0f);
    void Update();
    void Draw(KdTexture* tex);

    // ゲッター・セッター
    bool IsAlive() const { return m_alive; }
    void SetAlive(bool alive) { m_alive = alive; }
    const Math::Vector2& GetPos() const { return m_pos; }

    // ★追加：回避済みフラグのゲッター・セッター
    bool IsDodged() const { return m_isDodged; }
    void SetDodged(bool flag) { m_isDodged = flag; }

private:
    Math::Vector2 m_pos;
    Math::Vector2 m_move;
    float m_angle = 0.0f;
    bool  m_alive = false;

    // ★追加：この弾で回避（ジャスト回避など）が成立済みか
    bool  m_isDodged = false;
};
