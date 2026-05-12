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

    // どちらのUpdateを呼ぶかで挙動を変えられるようにする
    void Update(const Math::Vector2& targetPos, float timeScale = 1.0f);
    void Draw(KdTexture* tex);

    bool IsAlive() const { return m_alive; }
    void SetAlive(bool alive) { m_alive = alive; }
    const Math::Vector2& GetPos() const { return m_pos; }

private:
    Math::Vector2 m_pos;
    Math::Vector2 m_velocity; // 現在の移動ベクトルを保持
    bool m_alive = false;
    float m_angle = 0.0f;          // ★追加：描画に使う角度
    C_EnemyManager* m_pParentManager = nullptr;

    // --- 追加設定 ---
    bool m_isChasing = false;
    const float m_detectRange = 250.0f; // 索敵範囲
    const float m_speed = 2.5f;        // 移動スピード
};
