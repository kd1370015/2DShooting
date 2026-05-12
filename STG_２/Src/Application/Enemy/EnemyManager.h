#pragma once
#include <vector>
#include <memory>
#include "Enemy.h"
#include "EnemyTurret.h"
#include   "FollowEnemy.h"

class C_EnemyManager {
public:
    C_EnemyManager() : m_leader(nullptr) {}
    ~C_EnemyManager() {}

    void Update(const Math::Vector2& pPos, const Math::Vector2& pVel, GameScene* owner);
    void Draw(KdTexture* enemyTex, KdTexture* turretTex);

    // 敵の生成（右端から出すイメージ）
    void SpawnWave(Math::Vector2 basePos);
    void AddTurret(Math::Vector2 pos);

    std::vector<std::shared_ptr<C_Enemy>>& GetEnemies() { return m_enemies; }
    std::vector<std::shared_ptr<C_EnemyTurret>>& GetTurrets() { return m_turrets; }

    // 現在のリーダーのポインタを返す関数を追加
    std::shared_ptr<C_Enemy> GetLeader() const { return m_leader; }

private:
    std::vector<std::shared_ptr<C_Enemy>>       m_enemies;
    std::vector<std::shared_ptr<C_EnemyTurret>> m_turrets;
    std::vector<std::shared_ptr<C_FollowEnemy>> m_followEnemies;

    std::shared_ptr<C_Enemy> m_leader;


    // 陣形を9体編成に拡張（V字を深くする）
    const std::vector<Math::Vector2> m_formationOffsets = {
        { 0,   0   }, // リーダー
        { 60,  60  }, { 60, -60  }, // 2列目
        { 120, 120 }, { 120, -120 }, // 3列目
        { 180, 180 }, { 180, -180 }, // 4列目
        { 240, 240 }, { 240, -240 }  // 5列目
    };

    void UpdateLeader(); // リーダーの選出・更新
};