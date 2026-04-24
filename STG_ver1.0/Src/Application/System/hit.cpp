#include "hit.h"
#include "../VFX/Bullet.h"
#include "../Enemy/Enemy.h"
#include "../Player/player.h" // 追加
#include "../Core/Scene.h"  // 追加


//void C_Hit::CheckBulletToEnemy(std::vector<C_Bullet*>& bullets, std::vector<C_Enemy*>& enemies) {
//    for (auto& bullet : bullets) {
//         すでに消えている弾は無視
//        if (!bullet->IsAlive()) continue;
//
//        for (auto& enemy : enemies) {
//             すでに消えている敵は無視
//            if (!enemy->IsAlive()) continue;
//
//             弾と敵の座標を取得
//            auto bPos = bullet->GetPos();
//            auto ePos = enemy->GetPos();
//
//             当たり判定（半径の合計。サイズに合わせて調整してください）
//             弾(8px) + 敵(16px) ＝ 24px くらいが目安
//            if (IsHitCircle(bPos.x, bPos.y, ePos.x, ePos.y, 24.0f)) {
//                bullet->SetAlive(false); // 弾消滅
//                enemy->SetAlive(false);  // 敵消滅
//
//                 弾が1つ消えたら、その弾の敵ループは終了
//                break;
//            }
//
//        }
//    }
//}

void C_Hit::CheckBulletToEnemy(std::vector<C_Bullet*>& bullets, std::vector<C_Enemy*>& enemies) {
    for (auto& bullet : bullets) {
        if (!bullet->IsAlive()) continue;

        for (auto& enemy : enemies) {
            if (!enemy->IsAlive()) continue;

            auto bPos = bullet->GetPos();
            auto ePos = enemy->GetPos();

            if (IsHitCircle(bPos.x, bPos.y, ePos.x, ePos.y, 24.0f)) {
                bullet->SetAlive(false);
                enemy->SetAlive(false);

                // 50%の確率でドロップさせる場合
                if (rand() % 100 < 10) {
                    SCENE.AddOrb(ePos);
                }                // ---------------------------------

                break;
            }
        }
    }
}

void C_Hit::CheckPlayerToEnemy(C_Player& player, std::vector<C_Enemy*>& enemies) {
    if (!player.IsAlive()) return; // ドット(.)でアクセス

    for (auto& enemy : enemies) {
        if (!enemy->IsAlive()) continue;

        auto pPos = player.GetPos(); // ドット(.)でアクセス
        auto ePos = enemy->GetPos();

        float hitDistance = player.GetRadius() + 16.0f;

        if (IsHitCircle(pPos.x, pPos.y, ePos.x, ePos.y, hitDistance)) {
            //player.SetAlive(false);     //死亡判定
        }
    }
}


bool C_Hit::IsHitCircle(float x1, float y1, float x2, float y2, float radius) {
    float dx = x1 - x2;
    float dy = y1 - y2;
    // 距離の2乗 ≦ 半径の2乗
    return (dx * dx + dy * dy) <= (radius * radius);
}