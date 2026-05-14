#pragma once
#include "EnemyTurret.h"

class C_Boss : public C_EnemyTurret {
public:
    C_Boss() : m_maxHp(10), m_pattern(0), m_patternTimer(0) {}

    // 親の関数を上書きすることを override で明示
    void Init(Math::Vector2 pos, float stopX) override;
    void Update(const Math::Vector2& playerPos, const Math::Vector2& playerVel, GameScene* owner) override;
    void Draw(KdTexture* tex) override;

    float GetHpRate() const { return (float)m_hp / (float)m_maxHp; }

    // 関数名を GetBossHpRate に変更
    float GetBossHpRate() const {
        if (m_maxHp <= 0) return 0.0f; // 念のためゼロ除算対策
        return (float)m_hp / (float)m_maxHp;
    }

    // EnemyTurret.h または Boss.h
    void DecreaseHp(int amount) { m_hp -= amount; }
    int GetHp() const { return m_hp; }

private:
    KdTexture m_bossTex; // ボス専用テクスチャ
    KdTexture m_turboTex[4]; // 4枚分のテクスチャ配列
    float     m_turboFrame;  // アニメーション用フレーム

    int m_maxHp;
    int m_pattern;
    int m_patternTimer;

    int m_bossShootTimer = 60; // 追加

    float m_baseY;        // 出現時のY座標を保持
    float m_floatTimer;   // 上下運動用のカウンター
};