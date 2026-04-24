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
    // ★これを追加：生存状態を外から変更できるようにする
    void SetAlive(bool alive) { m_alive = alive; }
    // ★これを追加（座標を取得できるようにする）
    const Math::Vector2& GetPos() const { return m_pos; }

private:
    Math::Vector2 m_pos;
    Math::Vector2 m_move;
    bool m_alive = false;
};