#include "EnemyManager.h"

void C_EnemyManager::Update(const Math::Vector2& pPos, const Math::Vector2& pVel, GameScene* owner) {
    // 1. 生存チェックとリーダーの更新
    UpdateLeader();

    // 2. リーダーが存在すれば、フォーメーションの位置を各敵に伝える
    if (m_leader) {
        int index = 0;
        for (auto& enemy : m_enemies) {
            if (!enemy->IsAlive()) continue;

            if (enemy == m_leader) {
                // リーダーは自律してプレイヤーを追う
                enemy->Update(pPos);
            }
            else {
                // 子機はリーダーの相対位置（未来位置を少し予測するとスムーズ）を目指す
                Math::Vector2 target = m_leader->GetPos() + m_formationOffsets[index % m_formationOffsets.size()];
                enemy->Update(target); // ※Enemy::Updateを「目標地点へ向かう」ように使う
                index++;
            }
        }
    }

    // 3. 砲台の更新（偏差射撃のために playerVelocity を追加して渡す）
    for (auto& turret : m_turrets) {
        if (turret->IsAlive()) {
            // playerPos だけでなく playerVelocity も渡すように修正
            turret->Update(pPos, pVel, owner);
        }
    }

    // 4. 死んだ敵をリストから削除（メモリ管理）
    m_enemies.erase(std::remove_if(m_enemies.begin(), m_enemies.end(),
        [](auto& e) { return !e->IsAlive(); }), m_enemies.end());
}

void C_EnemyManager::UpdateLeader() {
    // リーダーがいない、もしくは死んでいたら次のリーダーを探す
    if (!m_leader || !m_leader->IsAlive()) {
        m_leader = nullptr;
        for (auto& enemy : m_enemies) {
            if (enemy->IsAlive()) {
                m_leader = enemy;
                break;
            }
        }
    }
}

void C_EnemyManager::SpawnWave(Math::Vector2 basePos) {
    // 陣形の数だけループを回して生成
    for (int i = 0; i < (int)m_formationOffsets.size(); ++i) {
        auto newEnemy = std::make_shared<C_Enemy>();
        // 最初は重ならないように少しずらしてInit
        newEnemy->Init(basePos + m_formationOffsets[i], this);
        m_enemies.push_back(newEnemy);
    }
}

void C_EnemyManager::AddTurret(Math::Vector2 pos)
{
    auto newTurret = std::make_shared<C_EnemyTurret>();
    newTurret->Init(pos);
    m_turrets.push_back(newTurret);
}

void C_EnemyManager::Draw(KdTexture* enemyTex, KdTexture* turretTex) {
    for (auto& enemy : m_enemies)  enemy->Draw(enemyTex);
    for (auto& turret : m_turrets) turret->Draw(turretTex);
}