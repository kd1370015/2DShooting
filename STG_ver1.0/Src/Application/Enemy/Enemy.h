#pragma once
#include "Application/Core/main.h"

class C_Enemy {
public:
    C_Enemy() {}
    ~C_Enemy() {}

    void Init(Math::Vector2 pos);
    void Update(const Math::Vector2& playerPos); // プレイヤーの位置を渡す
    void Draw(KdTexture* tex);

    bool IsAlive() const { return m_alive; }
    void SetAlive(bool alive) { m_alive = alive; }
    const Math::Vector2& GetPos() const { return m_pos; }

private:
    Math::Vector2 m_pos;
    Math::Vector2 m_move;
    bool m_alive = false;
    const float m_speed = 2.0f; // 敵の移動速度

};
