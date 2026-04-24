#include "Enemy.h"
#include "../Core/Scene.h" // シーンへのアクセスのため

void C_Enemy::Init(Math::Vector2 pos) {
    m_pos = pos;
    m_alive = true;
}

void C_Enemy::Update(const Math::Vector2& playerPos) {
    if (!m_alive) return;

    // プレイヤーへの方向ベクトルを計算
    Math::Vector2 dir = playerPos - m_pos;
    dir.Normalize(); // 長さを1にする

    // プレイヤーに向かって移動
    m_move = dir * m_speed;
    m_pos += m_move;
}

void C_Enemy::Draw(KdTexture* tex) {
    if (!m_alive || !tex) return;

    Math::Matrix mat = Math::Matrix::CreateTranslation(m_pos.x, m_pos.y, 0);
    SHADER.m_spriteShader.SetMatrix(mat);
    SHADER.m_spriteShader.DrawTex(tex, Math::Rectangle(0, 0, 32, 32), 1.0f);
}