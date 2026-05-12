#pragma once

// 前方宣言や必要なヘッダー
#include "Application/Core/main.h" 

class C_Bullet {
public:
    C_Bullet() {}
    ~C_Bullet() {}

    void Init(Math::Vector2 pos, float angle);
    void Update(float timeScale = 1.0f);
    void Draw(KdTexture* tex);

    bool IsAlive() const { return m_alive; }
    // ★これを追加：生存状態を外から変更できるようにする
    void SetAlive(bool alive) { m_alive = alive; }

    // ★追加：弾の向きを無理やり書き換える関数
    void Reflect(float angle, float speed = 10.0f) {
        m_move.x = cosf(angle) * speed;
        m_move.y = sinf(angle) * speed;
    }

    // ★追加：今の角度を取得する（必要であれば）
    float GetAngle() const { return atan2f(m_move.y, m_move.x); }

    // ★追加：回避済みフラグの取得と設定
    bool IsDodged() const { return m_isDodged; }
    void SetDodged(bool flag) { m_isDodged = flag; }

    void AddReflectCount() { m_reflectCount++; }
    int GetReflectCount() const { return m_reflectCount; }

    // 敵の弾かどうかを設定する関数
    void SetIsEnemy(bool isEnemy) { m_isEnemy = isEnemy; }

    // ★これを追加（座標を取得できるようにする）
    const Math::Vector2& GetPos() const { return m_pos; }

private:
    Math::Vector2 m_pos;
    Math::Vector2 m_move;
    bool m_alive = false;

    // ★追加：回避済みかどうかを管理するフラグ
    bool m_isDodged = false;
    int m_reflectCount = 0; // ★追加：現在の反射回数

    bool m_isEnemy = false; // デフォルトは自機の弾(false)
    float m_angle = 0.0f;   // 角度も持っておくと便利
};