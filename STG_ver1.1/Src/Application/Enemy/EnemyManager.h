#pragma once
#include <vector>
#include <memory>
#include "Enemy.h"
#include "EnemyTurret.h"

class C_EnemyManager {
public:
    C_EnemyManager(){}
    ~C_EnemyManager() {}

    void Update(const Math::Vector2& pPos, const Math::Vector2& pVel, GameScene* owner, float timeScale);
    void Draw(KdTexture* enemyTex, KdTexture* turretTex);

    // 敵の生成（右端から出すイメージ）
    void SpawnEnemy(Math::Vector2 basePos);
    void AddTurret(Math::Vector2 pos, float stopX);

    // 生きている敵の数を返す関数
    size_t GetTurretCount() const { return m_turrets.size(); }


    std::vector<std::shared_ptr<C_Enemy>>& GetEnemies() { return m_enemies; }
    std::vector<std::shared_ptr<C_EnemyTurret>>& GetTurrets() { return m_turrets; }


private:
    std::vector<std::shared_ptr<C_Enemy>>       m_enemies;
    // 砲台(C_EnemyTurret)のポインタを格納する可変長配列
    std::vector<std::shared_ptr<C_EnemyTurret>> m_turrets;




};