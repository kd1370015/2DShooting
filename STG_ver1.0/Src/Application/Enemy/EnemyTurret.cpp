#include "EnemyTurret.h"
#include "Application/Core/Scene.h"

void C_EnemyTurret::Init(Math::Vector2 pos) {
    m_pos = pos;
    m_alive = true;
    m_isStopped = false;
    m_shootTimer = 60;
}

void C_EnemyTurret::Update(const Math::Vector2& playerPos) {
    if (!m_alive) return;

    // 1. 移動：止まるまでは左に進む
    if (!m_isStopped) {
        m_pos.x -= m_speed;
        if (m_pos.x <= m_stopX) {
            m_pos.x = m_stopX;
            m_isStopped = true;
        }
    }
    // 2. 攻撃：止まったらプレイヤーを狙う
    else {
        if (--m_shootTimer <= 0) {
            // 自機へのベクトルを計算
            Math::Vector2 dir = playerPos - m_pos;
            float angle = atan2f(dir.y, dir.x);

            // Sceneに実装した敵弾追加関数を呼ぶ
            SCENE.AddEnemyBullet(m_pos, angle);

            m_shootTimer = 90; // 次の射撃まで1.5秒（60fps想定）
        }
    }
}

void C_EnemyTurret::Draw(KdTexture* tex) {
    if (!m_alive || !tex) return;

    Math::Matrix mat = Math::Matrix::CreateTranslation(m_pos.x, m_pos.y, 0);
    SHADER.m_spriteShader.SetMatrix(mat);

    // 用意した画像のサイズに合わせて Rectangle を調整してください
    // (例: 画像が64x64なら Rectangle(0, 0, 64, 64))
    Math::Rectangle srcRect = Math::Rectangle(0, 0, 32, 32);

    SHADER.m_spriteShader.DrawTex(tex, Math::Rectangle(0, 0, 32, 32), 1.0f);
}