#pragma once
#include <vector>
#include <memory> // shared_ptrのために必要

class C_Bullet;
class C_Enemy;
class C_EnemyTurret; // 追加
class C_Boss; // 前方宣言
class C_BossBullet; // 前方宣言を追加
class C_Player;
class Score;
class GameScene; // 追加

class C_Hit {
public:
    static void CheckBulletToEnemy(std::vector<C_Bullet*>& bullets, std::vector<std::shared_ptr<C_Enemy>>& enemies, GameScene* scene);
    static void CheckBulletToTurret(std::vector<C_Bullet*>& bullets, std::vector<std::shared_ptr<C_EnemyTurret>>& turrets, GameScene* scene);
    static void CheckPlayerToEnemyBullet(C_Player& player, std::vector<C_Bullet*>& enemyBullets);
    static void CheckPlayerToEnemy(C_Player& player, std::vector<std::shared_ptr<C_Enemy>>& enemies);
    static void CheckBulletToBoss(std::vector<C_Bullet*>& bullets, std::shared_ptr<C_Boss> boss, GameScene* scene);
    // ★追加：プレイヤーとボスの弾の判定
    static void CheckPlayerToBossBullet(C_Player& player, std::vector<C_BossBullet*>& bossBullets);

    static void ExplosionRadiusDamage(Math::Vector2 center, float radius, std::vector<std::shared_ptr<C_EnemyTurret>>& turrets, GameScene* scene);
    static bool IsHitCircle(float x1, float y1, float x2, float y2, float radius);
private:

};