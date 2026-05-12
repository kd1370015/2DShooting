#pragma once
#include <vector>
#include <memory> // shared_ptr‚Ì‚½‚ß‚É•K—v

class C_Bullet;
class C_Enemy;
class C_EnemyTurret; // ’Ç‰Á
class C_Player;
class Score;
class GameScene; // ’Ç‰Á

class C_Hit {
public:
    static void CheckBulletToEnemy(std::vector<C_Bullet*>& bullets, std::vector<std::shared_ptr<C_Enemy>>& enemies, GameScene* scene);
    static void CheckBulletToTurret(std::vector<C_Bullet*>& bullets, std::vector<std::shared_ptr<C_EnemyTurret>>& turrets, GameScene* scene);
    static void CheckPlayerToEnemyBullet(C_Player& player, std::vector<C_Bullet*>& enemyBullets);
    static void CheckPlayerToEnemy(C_Player& player, std::vector<std::shared_ptr<C_Enemy>>& enemies);

    static bool IsHitCircle(float x1, float y1, float x2, float y2, float radius);
private:

};