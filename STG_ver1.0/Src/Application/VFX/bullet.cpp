#include "Bullet.h"
#include "Application/Player/player.h" // ここに定義があるか確認

void C_Bullet::Init(Math::Vector2 pos, float angle)
{
    m_pos = pos;
    float speed = 10.0f; // 弾の速さ

    // float bulletAngle = angle + DirectX::XM_PIDIV2;

    // 自機の画像補正で angle をいじっている場合、
    // ここで弾の飛ぶ方向も合わせる必要があります。
    // もし弾が変な方向に飛ぶなら、ここの angle にも 
    // 自機と同じオフセット（DirectX::XM_PIDIV2など）を足してください。
    m_move.x = cosf(angle) * speed;
    m_move.y = sinf(angle) * speed;

    m_alive = true;
}

void C_Bullet::Update()
{
    if (!m_alive) return;

    m_pos += m_move;

    // 画面外に出たら消す（画面サイズに合わせて調整してください）
    if (m_pos.x < -700 || m_pos.x > 400 ||
        m_pos.y < -700 || m_pos.y > 400)
    {
        m_alive = false;
    }
}

void C_Bullet::Draw(KdTexture* tex)
{
    if (!m_alive || !tex) return;

    // 弾の行列作成（自機と同様に描画位置を計算）
    Math::Matrix mat = Math::Matrix::CreateTranslation(m_pos.x, m_pos.y, 0);

    SHADER.m_spriteShader.SetMatrix(mat);
    // 弾のサイズに合わせて Rectangle(0, 0, 32, 32) などに調整
    SHADER.m_spriteShader.DrawTex(tex, Math::Rectangle(0, 0, 32, 16), 1.0f);
}