#include "FollowEnemy.h"

//// コンストラクタの実装
//C_FollowEnemy::C_FollowEnemy(Math::Vector2 pos, C_EnemyManager* manager) {
//    this->Init(pos, manager);
//    m_angle = 0.0f;
//    SetAlive(true);
//}
//
//// Updateの実装
//void C_FollowEnemy::Update(const Math::Vector2& targetPos) {
//    // 滑らかな追従
//    float easing = 0.03f;
//    m_pos.x += (targetPos.x - m_pos.x) * easing;
//    m_pos.y += (targetPos.y - m_pos.y) * easing;
//
//    // 回転
//    m_angle += 2.0f;
//}