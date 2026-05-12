// --- FollowEnemy.h ---
#pragma once
#include "Enemy.h"

//class C_FollowEnemy : public C_Enemy {
//public:
//    C_FollowEnemy(Math::Vector2 pos, C_EnemyManager* manager);
//
//    // overrideを付けて、親のUpdateとは違う動きをすることを明示
//    void Update(const Math::Vector2& targetPos) override;
//};

class C_FollowEnemy : public C_Enemy {
public:
    // コンストラクタ
    C_FollowEnemy(Math::Vector2 pos, C_EnemyManager* manager) {
        // 親のInitを呼ぶか、protectedになった変数に代入する
        this->Init(pos, manager);
        m_angle = 0.0f;
        SetAlive(true); // 生存フラグを忘れずに
    }

    void Update(const Math::Vector2& targetPos) {
        // これで m_pos にアクセスできるようになります
        float easing = 0.03f;
        m_pos.x += (targetPos.x - m_pos.x) * easing;
        m_pos.y += (targetPos.y - m_pos.y) * easing;

        // m_angle もアクセス可能
        m_angle += 2.0f;
    }
};
