#include "BossBullet.h"

void C_BossBullet::Init(Math::Vector2 pos, float angle, float speed) {
    m_pos = pos;
    m_angle = angle;
    m_move.x = cosf(angle) * speed;
    m_move.y = sinf(angle) * speed;
    m_alive = true;

    m_isDodged = false; // ★忘れずにリセット
}

void C_BossBullet::Update() {
    if (!m_alive) return;

    // 移動
    m_pos += m_move;

    // 画面外に出たら消す（画面サイズに合わせて調整）
    if (m_pos.x < -700 || m_pos.x > 700 || m_pos.y < -400 || m_pos.y > 400) {
        m_alive = false;
    }
}

void C_BossBullet::Draw(KdTexture* tex) {
    if (!m_alive || !tex) return;

    // 行列の作成
    // 弾を 2.5倍にし、進行方向に回転させる
    Math::Matrix scaleMat = Math::Matrix::CreateScale(2.5f);
    Math::Matrix rotMat = Math::Matrix::CreateRotationZ(m_angle);
    Math::Matrix transMat = Math::Matrix::CreateTranslation(m_pos.x, m_pos.y, 0);

    Math::Matrix mat = scaleMat * rotMat * transMat;

    SHADER.m_spriteShader.SetMatrix(mat);
    SHADER.m_spriteShader.DrawTex(tex, 0, 0);
    SHADER.m_spriteShader.SetMatrix(Math::Matrix::Identity);
}