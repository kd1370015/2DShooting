#include "EnemyManager.h"


#include <algorithm> // std::remove_if を使うために必要

//void C_EnemyManager::Update(const Math::Vector2& pPos, const Math::Vector2& pVel, GameScene* owner, float timeScale)
//{
//    //// --- 1. 普通の敵(Enemy)の更新と削除 ---
//    //for (auto& enemy : m_enemies) {
//    //    enemy->Update(pPos, owner); // 引数は現在のプロジェクトに合わせて調整してください
//    //}
//    //// 死んだ敵(m_alive == false)を配列から削除
//    //m_enemies.erase(std::remove_if(m_enemies.begin(), m_enemies.end(),
//    //    [](const std::shared_ptr<C_Enemy>& e) { return !e->IsAlive(); }), m_enemies.end());
//
//
//    // --- 2. 砲台(Turret)の更新と削除 ---
//    for (auto& turret : m_turrets) {
//        turret->Update(pPos, pVel, owner);
//    }
//    // 死んだ砲台を配列から削除
//    m_turrets.erase(std::remove_if(m_turrets.begin(), m_turrets.end(),
//        [](const std::shared_ptr<C_EnemyTurret>& t) { return !t->IsAlive(); }), m_turrets.end());
//}


void C_EnemyManager::Update(const Math::Vector2& pPos, const Math::Vector2& pVel, GameScene* owner, float timeScale)
{
    // --- 1. 追尾型エネミー(Enemy)の更新と削除 ---
    for (auto& enemy : m_enemies) {
        // 先ほど作成した「ゆっくり追尾するUpdate」を呼び出す
        // 引数にプレイヤーの座標とタイムスケールを渡す
        enemy->Update(pPos, timeScale);
    }

    // 死んだ敵(m_alive == false)を配列から削除
    m_enemies.erase(std::remove_if(m_enemies.begin(), m_enemies.end(),
        [](const std::shared_ptr<C_Enemy>& e) { return !e->IsAlive(); }), m_enemies.end());


    // --- 2. 砲台(Turret)の更新と削除 ---
    for (auto& turret : m_turrets) {
        // 砲台側にもスローを適用させたい場合は、引数に timeScale を追加して渡す
        turret->Update(pPos, pVel, owner);
    }

    // 死んだ砲台を配列から削除
    m_turrets.erase(std::remove_if(m_turrets.begin(), m_turrets.end(),
        [](const std::shared_ptr<C_EnemyTurret>& t) { return !t->IsAlive(); }), m_turrets.end());
}

void C_EnemyManager::Draw(KdTexture* enemyTex, KdTexture* turretTex)
{
    // 全ての敵を描画
    for (auto& enemy : m_enemies) {
        enemy->Draw(enemyTex);
    }

    // 全ての砲台を描画
    for (auto& turret : m_turrets) {
        turret->Draw(turretTex);
    }
}

void C_EnemyManager::SpawnEnemy(Math::Vector2 basePos)
{
    // 1. 出現位置を計算
    float spawnY = (rand() % 2 == 0) ? 280.0f : -280.0f;
    float spawnX = 850.0f;

    // 2. 敵のインスタンスを作成
    std::shared_ptr<C_Enemy> newEnemy = std::make_shared<C_Enemy>();

    // 3. 初期化（thisを渡して管理者を覚えさせる）
    newEnemy->Init(Math::Vector2(spawnX, spawnY), this);

    // 4. マネージャーが持っているリストに追加
    // ※リスト名が m_enemies だと仮定しています
    m_enemies.push_back(newEnemy);
}


void C_EnemyManager::AddTurret(Math::Vector2 pos, float stopX)
{
    // shared_ptrを作成してリストに追加
    auto newTurret = std::make_shared<C_EnemyTurret>();
    newTurret->Init(pos, stopX);
    m_turrets.push_back(newTurret);
}