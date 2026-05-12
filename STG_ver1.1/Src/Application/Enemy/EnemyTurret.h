


#pragma once
#include "Application/Core/main.h"


class GameScene; // 前方宣言

class C_EnemyTurret {
public:
    C_EnemyTurret() {}
    void Init(Math::Vector2 pos, float stopX);

    // プレイヤーの速度(playerVel)を受け取るように拡張
    void Update(const Math::Vector2& playerPos, const Math::Vector2& playerVel, GameScene* owner); // 引数にownerを追加
    void Draw(KdTexture* tex);

    bool IsAlive() const { return m_alive; }
    void SetAlive(bool alive) { m_alive = alive; }
    Math::Vector2 GetPos() const { return m_pos; }
private:
    Math::Vector2 m_pos;
    bool m_alive = true;
    bool m_isStopped = false;
    int  m_shootTimer = 60;


	float m_stopX; // 停止位置をメンバ変数に変更

    //const float m_stopX = 450.0f;
    const float m_bulletSpeed = 8.0f; // 偏差計算に必要
};