#pragma once

// 前方宣言や必要なヘッダー
#include "Application/Core/main.h" 

class C_Bullet {
public:
    C_Bullet() {}
    ~C_Bullet() {}

    void Init(Math::Vector2 pos, float angle);
    void Update();
    void Draw(KdTexture* tex);

    bool IsAlive() const { return m_alive; }

private:
    Math::Vector2 m_pos;
    Math::Vector2 m_move;
    bool m_alive = false;
};