


#pragma once
#include "Application/Core/main.h"


class GameScene; // 前方宣言


class C_EnemyTurret {
public:
    C_EnemyTurret() {}
    // 子クラスで上書きできるように virtual をつける
    virtual ~C_EnemyTurret() {}
    //virtual void Init(Math::Vector2 pos, float stopX);

    // 初期化時にHPをセット
    virtual void Init(Math::Vector2 pos, float stopX) {
        m_pos = pos;
        m_alive = true;
        m_isStopped = false;
        m_shootTimer = 60;
        //m_stopX = (float)(300 + (rand() % 300));
        m_stopX = stopX;

        m_hp = 3; // ★ ここでHPを 3 に設定

        m_hitFlashTimer = 0; // ★初期化を追加

        // --- ボスと同じ感じでロード ---
        for (int i = 0; i < 4; i++) {
            std::string fileName = "Texture/Enemy/EF_T_" + std::to_string(i + 1) + ".png";
            m_turboTex[i].Load(fileName);
        }
        m_turboFrame = 0.0f;

    }
    virtual void Update(const Math::Vector2& playerPos, const Math::Vector2& playerVel, GameScene* owner);
    virtual void Draw(KdTexture* tex);

    bool IsAlive() const { return m_alive; }
    void SetAlive(bool alive) { m_alive = alive; }
    Math::Vector2 GetPos() const { return m_pos; }

    // HPを取得・減少させる関数
    int  GetHp() const { return m_hp; }
    void DecreaseHp(int amount) { m_hp -= amount; }

    // ★追加：当たった瞬間にタイマーをセットする
    void OnHit() { m_hitFlashTimer = 30; } // 8フレーム（約0.13秒）赤くする

protected: // ★ Bossクラスからも触れるように protected に変更
    Math::Vector2 m_pos;
    bool m_alive = true;
    bool m_isStopped = false;
    int  m_shootTimer = 60;
    int  m_hp = 1; // ★ ここにHPを追加（雑魚は1）

    int  m_hitFlashTimer = 0; // ★追加：点滅用タイマー

    // --- ターボアニメーション用 ---
    float m_turboFrame = 0.0f; // 0.0～4.0の間で動かす
    KdTexture m_turboTex[4]; // 4枚分のテクスチャ配列

    float m_stopX;
    const float m_bulletSpeed = 8.0f;
};