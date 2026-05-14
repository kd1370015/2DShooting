#pragma once
#include <vector>
#include <memory>
#include "Enemy.h"
#include "EnemyTurret.h"
#include "boss.h"

class C_EnemyManager {
public:
    C_EnemyManager(){}
    ~C_EnemyManager() {}

    void Update(const Math::Vector2& pPos, const Math::Vector2& pVel, GameScene* owner, float timeScale);
    void Draw(KdTexture* enemyTex, KdTexture* turretTex);

    // 敵の生成（右端から出すイメージ）
    void SpawnEnemy(Math::Vector2 basePos);
    // ★追加：ボウリング陣形（V字型）で敵を生成する関数
    //void SpawnBowlingFormation();
    // 引数として、陣形の中心となる座標（basePos）を受け取れるようにします

    void SpawnBowlingFormationTurret(Math::Vector2 basePos = Math::Vector2(850.0f, 0.0f));
    void SpawnVerticalLine(float spawnY);
    void SpawnSandwichFormation();
    void SpawnReverseVFormation();
    void SpawnDiamondFormation();

    void ClearAllEnemies();


    void AddTurret(Math::Vector2 pos, float stopX);
    // public に追加
    void SpawnBoss(Math::Vector2 pos, float stopX) {
        m_boss = std::make_shared<C_Boss>();
        m_boss->Init(pos, stopX);
    }
    std::shared_ptr<C_Boss> GetBoss() { return m_boss; }

    // 生きている敵の数を返す関数
    size_t GetTurretCount() const { return m_turrets.size(); }


    std::vector<std::shared_ptr<C_Enemy>>& GetEnemies() { return m_enemies; }
    std::vector<std::shared_ptr<C_EnemyTurret>>& GetTurrets() { return m_turrets; }


private:
    std::vector<std::shared_ptr<C_Enemy>>       m_enemies;
    // 砲台(C_EnemyTurret)のポインタを格納する可変長配列
    std::vector<std::shared_ptr<C_EnemyTurret>> m_turrets;

    // private に追加
    std::shared_ptr<C_Boss> m_boss = nullptr;




};