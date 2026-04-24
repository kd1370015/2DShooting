#pragma once
#include <vector>

class C_Bullet;
class C_Enemy;
class C_Player; // 追加

class C_Hit {
public:
    static void CheckBulletToEnemy(std::vector<C_Bullet*>& bullets, std::vector<C_Enemy*>& enemies);

    // ★追加：プレイヤーと敵の判定
    static void CheckPlayerToEnemy(C_Player& player, std::vector<C_Enemy*>& enemies);

private:
    static bool IsHitCircle(float x1, float y1, float x2, float y2, float radius);
};