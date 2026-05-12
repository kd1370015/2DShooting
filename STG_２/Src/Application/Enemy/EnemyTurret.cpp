

#include "EnemyTurret.h"
#include "Application/Scene/GameScene/GameScene.h"

void C_EnemyTurret::Init(Math::Vector2 pos) {
    m_pos = pos;
    m_alive = true;
    m_isStopped = false;
    m_shootTimer = 60;
}

void C_EnemyTurret::Update(const Math::Vector2& playerPos, const Math::Vector2& playerVel, GameScene* owner)
{
    if (!m_alive || !owner) return;

    // 左へ移動し、定位置で止まる
    if (!m_isStopped) {
        m_pos.x -= 2.0f;
        if (m_pos.x <= m_stopX) m_isStopped = true;
    }
    else
    {
        if (--m_shootTimer <= 0)
        {
            // ★偏差射撃のロジック★
            // 弾がプレイヤーに届くまでの時間を簡易計算
            float dist = Math::Vector2::Distance(m_pos, playerPos);
            float timeToHit = dist / m_bulletSpeed;

            // 未来位置 = 現在地 + (速度 * 時間)
            Math::Vector2 predictedPos = playerPos + (playerVel * timeToHit);

            // 未来位置への角度を計算
            Math::Vector2 dir = predictedPos - m_pos;
            float angle = atan2f(dir.y, dir.x);

            owner->AddEnemyBullet(m_pos, angle);
            m_shootTimer = 100;
        }

    }

}


//void C_EnemyTurret::Draw(KdTexture* tex) {
//    if (!m_alive || !tex) return;
//    // 砲台は少し赤らめるなどして区別しても良い（スクエニ風の演出）
//    Math::Matrix mat = Math::Matrix::CreateTranslation(m_pos.x, m_pos.y, 0);
//    SHADER.m_spriteShader.SetMatrix(mat);
//    SHADER.m_spriteShader.DrawTex(tex, Math::Rectangle(0, 0, 64, 64), 1.0f);
//}


void C_EnemyTurret::Draw(KdTexture* tex) {
    if (!m_alive || !tex) return;

    // 1. 行列の作成
    // 砲台の位置(m_pos)を行列に変換
    Math::Matrix mat = Math::Matrix::CreateTranslation(m_pos.x, m_pos.y, 0);

    // 2. 行列をセット
    SHADER.m_spriteShader.SetMatrix(mat);

    // 3. 矩形をポインタで渡す準備
    // (例: 画像が64x64ならそのサイズを指定)
    Math::Rectangle srcRect = { 0, 0, 64, 64 };

    // 4. 新しい DrawTex の定義に合わせて呼び出し
    // 引数: (テクスチャ, x, y, 矩形ポインタ)
    // 行列で位置を決めているので x=0, y=0 でOK
    SHADER.m_spriteShader.DrawTex(tex, 0, 0, &srcRect);
}