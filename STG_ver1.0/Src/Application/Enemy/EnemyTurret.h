#pragma once
#include "Application/Enemy/Enemy.h" // 共通の構造が必要な場合

class C_EnemyTurret {
public:
    C_EnemyTurret() {}
    ~C_EnemyTurret() {}

    void Init(Math::Vector2 pos);
    void Update(const Math::Vector2& playerPos);
    void Draw(KdTexture* tex);

    bool IsAlive() const { return m_alive; }
    void SetAlive(bool alive) { m_alive = alive; }
    Math::Vector2 GetPos() const { return m_pos; }

private:
    Math::Vector2 m_pos;
    bool m_alive = true;

    // 砲台専用のパラメータ
    float m_speed = 2.0f;        // 登場時の移動速度
    float m_stopX = 400.0f;      // 止まる位置（画面右側）
    bool  m_isStopped = false;   // 停止フラグ
    int   m_shootTimer = 60;     // 発射間隔
};