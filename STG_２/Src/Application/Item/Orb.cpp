#include "Orb.h"

void C_Orb::Init(Math::Vector2 pos, OrbType type) {
    m_pos = pos;
    m_type = type;
    m_alive = true;
}

void C_Orb::Update(const Math::Vector2& playerPos) {
    if (!m_alive) return;

    // プレイヤーとの距離を計算
    float dist = Math::Vector2::Distance(m_pos, playerPos);

    // 一定距離内に入ったら吸い寄せ
    if (dist < m_targetDist) {
        Math::Vector2 dir = playerPos - m_pos;
        dir.Normalize();
        m_pos += dir * m_speed;

        // 吸い寄せ中は少しずつ加速すると気持ちいい
        m_speed += 0.2f;
    }
}

//void C_Orb::Draw(KdTexture* tex) 
//{
//    if (!m_alive || !tex) return;
//
//    // 画像に合わせて Rectangle のサイズを調整してください
//    // (例: 用意した画像が32x32ならRectangle(0,0,32,32))
//    Math::Rectangle srcRect = Math::Rectangle(0, 0, 32, 32);
//
//    Math::Matrix mat = Math::Matrix::CreateTranslation(m_pos.x, m_pos.y, 0);
//    SHADER.m_spriteShader.SetMatrix(mat);
//
//    // 色は画像そのままで描画
//    SHADER.m_spriteShader.DrawTex(tex, srcRect, 1.0f);
//}

void C_Orb::Draw(KdTexture* tex) {
    if (!m_alive || !tex) return;

    // 位置を行列に変換
    Math::Matrix mat = Math::Matrix::CreateTranslation(m_pos.x, m_pos.y, 0);
    SHADER.m_spriteShader.SetMatrix(mat);

    // 矩形（例：16x16の画像）
    Math::Rectangle srcRect = { 0, 0, 16, 16 };

    // x=0, y=0 で描画（行列で位置が決まっているため）
    SHADER.m_spriteShader.DrawTex(tex, 0, 0, &srcRect);
}