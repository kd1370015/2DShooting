#include "hit.h"
#include "../VFX/Bullet.h"
#include "../Enemy/Enemy.h"
#include "../Enemy/EnemyTurret.h" // 追加
#include "../Enemy/boss.h" 
#include "../Enemy/BossBullet.h"
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

                scene->AddFExplosion(bullet->GetPos());

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

            if (IsHitCircle(bullet->GetPos().x, bullet->GetPos().y, turret->GetPos().x, turret->GetPos().y, 32.0f)) {

                bullet->SetAlive(false);

                 //--- ダメージ計算の修正 ---
                int damage = 1;

                // 1. パリィ反射弾、またはプレイヤーがフルチャージ状態なら大ダメージ
                // ※ m_player.IsFullCharge() などのフラグ名は、ご自身のコードに合わせてください
                if (bullet->IsReflected() || scene->GetPlayer().IsUsingPower()) {
                    damage = 3;

                    // 強化弾が当たったときも爆風を出したいならここに追加
                    ExplosionRadiusDamage(bullet->GetPos(), 100.0f, turrets, scene);
                    scene->AddExplosion(bullet->GetPos());
                }


                turret->DecreaseHp(damage);

                // ★ここ！当たったので赤く光らせる
                turret->OnHit();


                // 撃破判定
                if (turret->GetHp() <= 0) {
                    turret->SetAlive(false);
                    scene->AddExplosion(turret->GetPos());

                    // ★追加：プレイヤーの状態によって加算スコアを変える
                    if (scene->GetPlayer().IsUsingPower()) {
                        // 強化状態（エネルギー消費中）なら高いスコア
                        scene->GetScore().AddScore(300);
                    }
                    else {
                        // 通常状態なら通常のスコア
                        scene->GetScore().AddScore(100);
                    }
                }

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

void C_Hit::CheckBulletToBoss(std::vector<C_Bullet*>& bullets, std::shared_ptr<C_Boss> boss, GameScene* scene) {
    // ボスがいない、または既に死んでいるなら何もしない
    if (!boss || !boss->IsAlive()) return;

    for (auto& bullet : bullets) {
        if (!bullet->IsAlive()) continue;

        // ボスは大きいので、当たり判定半径を大きめ（例：64.0f）に設定
        if (IsHitCircle(bullet->GetPos().x, bullet->GetPos().y, boss->GetPos().x, boss->GetPos().y, 64.0f)) {

            // 1. 弾を消す
            bullet->SetAlive(false);

            // 2. ボスのHPを減らす（EnemyTurretにm_hpがある前提）
            // もし DecreaseHp 関数がない場合は boss->SetHp(boss->GetHp() - 1); 等
            boss->DecreaseHp(1);

            // ★ここ！当たったので赤く光らせる
            boss->OnHit();


            // 3. 当たった時のエフェクト（火花など）
            //scene->AddExplosion(bullet->GetPos());

            // 4. HPが0以下になったら死亡処理
            if (boss->GetHp() <= 0) {
                boss->SetAlive(false);
                scene->AddExplosion(boss->GetPos()); // 撃破時の大爆発
                scene->GetScore().AddScore(5000);    // ボス撃破ボーナス
            }

            // 弾は消滅したので次の弾のループへ
        }
    }
}


//void C_Hit::ExplosionRadiusDamage(Math::Vector2 center, float radius, std::vector<std::shared_ptr<C_EnemyTurret>>& turrets, GameScene* scene) {
//    for (auto& turret : turrets) {
//        if (!turret->IsAlive()) continue;
//
//        // 着弾点と敵の距離を計算
//        float dist = Math::Vector2::Distance(center, turret->GetPos());
//
//        // 爆風の範囲内（radius）にいたら倒す
//        if (dist <= radius) {
//            turret->SetAlive(false);
//            scene->AddExplosion(turret->GetPos()); // 巻き込まれた敵も爆発
//            scene->GetScore().AddScore(50);        // 巻き込みスコアは少し控えめになど
//        }
//    }
//}


void C_Hit::ExplosionRadiusDamage(Math::Vector2 center, float radius, std::vector<std::shared_ptr<C_EnemyTurret>>& turrets, GameScene* scene) {
    // ★ 範囲を 70% 程度に絞る（例：radius が 100 なら 70 になる）
    float adjustedRadius = radius * 0.7f;

    for (auto& turret : turrets) {
        if (!turret->IsAlive()) continue;

        float dist = Math::Vector2::Distance(center, turret->GetPos());

        // 補正した範囲（adjustedRadius）で判定
        if (dist <= adjustedRadius) {
            turret->SetAlive(false);
            scene->AddExplosion(turret->GetPos());
            scene->GetScore().AddScore(50);
        }
    }
}

void C_Hit::CheckPlayerToBossBullet(C_Player& player, std::vector<C_BossBullet*>& bossBullets) {
    if (!player.IsAlive()) return;

    // 回避中は無敵（プレイヤーの仕様に合わせて）
    if (player.IsDodging()) return;

    auto pPos = player.GetPos();
    // プレイヤーの判定半径（中心付近に絞ると避けやすくて「避けた感」が出ます）
    float pRadius = player.GetRadius() * 0.6f;

    for (auto& bb : bossBullets) {
        if (!bb->IsAlive()) continue;

        auto bPos = bb->GetPos();

        // 判定：プレイヤー半径 + ボス弾の半径（12.0f 程度）
        // 扇状に広がっていても、個々の弾の座標 (bPos) で判定すればOKです
        if (IsHitCircle(pPos.x, pPos.y, bPos.x, bPos.y, pRadius + 12.0f)) {

            // 当たった時の処理
            player.DecreaseHp(1); // ボスの弾なのでダメージを2にするのもアリ
            bb->SetAlive(false);   // 弾を消す

            // 死亡確認
            if (player.GetHp() <= 0) {
                return;
            }
        }
    }
}


bool C_Hit::IsHitCircle(float x1, float y1, float x2, float y2, float radius) {
    float dx = x1 - x2;
    float dy = y1 - y2;
    return (dx * dx + dy * dy) <= (radius * radius);
}