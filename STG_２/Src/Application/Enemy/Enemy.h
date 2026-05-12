//#pragma once
//#include "Application/Core/main.h"
//
//class C_Enemy {
//public:
//    C_Enemy() {}
//    ~C_Enemy() {}
//
//    void Init(Math::Vector2 pos);
//    void Update(const Math::Vector2& playerPos); // プレイヤーの位置を渡す
//    void Draw(KdTexture* tex);
//
//    bool IsAlive() const { return m_alive; }
//    void SetAlive(bool alive) { m_alive = alive; }
//    const Math::Vector2& GetPos() const { return m_pos; }
//
//private:
//    Math::Vector2 m_pos;
//    Math::Vector2 m_move;
//    bool m_alive = false;
//    const float m_speed = 2.0f; // 敵の移動速度
//
//};

#pragma once
#include "Application/Core/main.h"

class C_EnemyManager; // 前方宣言

class C_Enemy {
public:
    C_Enemy() {}
    // 引数に Manager を追加
    void Init(Math::Vector2 pos, C_EnemyManager* manager);

    // --- 追加：生存フラグを取得する関数 ---
    bool GetAlive() const { return m_alive; }

    // どちらのUpdateを呼ぶかで挙動を変えられるようにする
    //void Update(const Math::Vector2& targetPos);

    virtual void Update(const Math::Vector2& targetPos); 

    void Draw(KdTexture* tex);

    bool IsAlive() const { return m_alive; }
    void SetAlive(bool alive) { m_alive = alive; }
    const Math::Vector2& GetPos() const { return m_pos; }

protected: // 子クラスからのアクセスを許可する
    Math::Vector2 m_pos;
    float m_angle = 0.0f; // 追加：回転角度
    bool m_alive = false;
    C_EnemyManager* m_pParentManager = nullptr;
    const float m_speed = 3.0f;
};
