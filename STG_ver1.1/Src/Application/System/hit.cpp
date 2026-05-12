#include "hit.h"
#include "../VFX/Bullet.h"
#include "../Enemy/Enemy.h"
#include "../Enemy/EnemyTurret.h" // 追加
#include "../Player/player.h"
#include "../Core/Scene.h"
#include <Application/Scene/GameScene/GameScene.h>

// 弾と通常敵の判定
// 弾と通常敵の判定
void C_Hit::CheckBulletToEnemy(std::vector<C_Bullet*>& bullets, std::vector<std::shared_ptr<C_Enemy>>& enemies, GameScene* scene) {
    if (!scene) return;

    for (auto& bullet : bullets) {
        if (!bullet->IsAlive()) continue;

        for (auto& enemy : enemies) {
            if (!enemy->IsAlive()) continue;

            if (IsHitCircle(bullet->GetPos().x, bullet->GetPos().y, enemy->GetPos().x, enemy->GetPos().y, 24.0f)) {
                bullet->SetAlive(false);
                enemy->SetAlive(false);

                // ★ SCENE マクロを引数の scene に書き換え
                scene->GetScore().AddScore(100);

                break;
            }
        }
    }
}



void C_Hit::CheckBulletToTurret(std::vector<C_Bullet*>& bullets, std::vector<std::shared_ptr<C_EnemyTurret>>& turrets, GameScene* scene) {
    if (!scene) return;

    for (auto& bullet : bullets) {
        if (!bullet->IsAlive()) continue;

        for (auto& turret : turrets) {
            if (!turret->IsAlive()) continue;

            // 当たり判定
            if (IsHitCircle(bullet->GetPos().x, bullet->GetPos().y, turret->GetPos().x, turret->GetPos().y, 32.0f)) {
                bullet->SetAlive(false);
                turret->SetAlive(false);

                // --- ★ここから追加・修正 ---

                // 1. 爆発を生成（GameSceneに新設した関数を呼ぶ）
                scene->AddExplosion(turret->GetPos());

                // 2. スコア加算
                scene->GetScore().AddScore(100);


                // 弾は1つの敵に当たったら消えるのでループを抜ける
                break;
            }
        }
    }
}

// hit.cpp
void C_Hit::CheckPlayerToEnemyBullet(C_Player& player, std::vector<C_Bullet*>& enemyBullets) {
    if (!player.IsAlive()) return;

    // ★追加：プレイヤーが回避アクション中なら、このダメージ判定自体をスキップする
    if (player.IsDodging()) return;

    auto pPos = player.GetPos();
    // プレイヤーの当たり判定半径（少し小さめにすると避けやすくて気持ちいいです）
    float pRadius = player.GetRadius() * 0.7f;

    for (auto& eb : enemyBullets) {
        if (!eb->IsAlive()) continue;

        auto bPos = eb->GetPos();

        // 距離判定（弾の半径を 8px と想定）
        if (IsHitCircle(pPos.x, pPos.y, bPos.x, bPos.y, pRadius + 8.0f)) {
            // 弾が当たった！
            player.DecreaseHp(1); // HPを減らす
            eb->SetAlive(false);   // 弾を消す
            // プレイヤーが死んだら、このフレームの他の当たり判定や敵の処理を止める
            if (player.GetHp() <= 0) {
                return; // もしくは break;
            }
        }
    }
}

void C_Hit::CheckPlayerToEnemy(C_Player& player, std::vector<std::shared_ptr<C_Enemy>>& enemies) {
    if (!player.IsAlive()) return;

    for (auto& enemy : enemies) {
        if (!enemy->IsAlive()) continue;

        auto pPos = player.GetPos();
        auto ePos = enemy->GetPos();

        float hitDistance = player.GetRadius() + 16.0f;

        if (IsHitCircle(pPos.x, pPos.y, ePos.x, ePos.y, hitDistance)) {
            player.DecreaseHp(1);
            enemy->SetAlive(false); // ぶつかった敵も消すのが一般的
            // プレイヤーが死んだら、このフレームの他の当たり判定や敵の処理を止める
            if (player.GetHp() <= 0) {
                return; // もしくは break;
            }
        }
    }
}

bool C_Hit::IsHitCircle(float x1, float y1, float x2, float y2, float radius) {
    float dx = x1 - x2;
    float dy = y1 - y2;
    return (dx * dx + dy * dy) <= (radius * radius);
}