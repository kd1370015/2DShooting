

#include "EnemyTurret.h"
#include "Application/Scene/GameScene/GameScene.h"

void C_EnemyTurret::Init(Math::Vector2 pos, float stopX) {
    m_pos = pos;
    m_alive = true;
    m_isStopped = false;
    m_shootTimer = 60;
    // 300 ～ 600 の間でランダムに止まるようにする
    m_stopX = (float)(300 + (rand() % 300));
}

//void C_EnemyTurret::Update(const Math::Vector2& playerPos, const Math::Vector2& playerVel, GameScene* owner)
//{
//    if (!m_alive || !owner) return;
//
//    // 左へ移動し、定位置で止まる
//    if (!m_isStopped) {
//        m_pos.x -= 2.0f;
//        if (m_pos.x <= m_stopX) m_isStopped = true;
//    }
//    else
//    {
//        if (--m_shootTimer <= 0)
//        {
//            // ★偏差射撃のロジック★
//            // 弾がプレイヤーに届くまでの時間を簡易計算
//            float dist = Math::Vector2::Distance(m_pos, playerPos);
//            float timeToHit = dist / m_bulletSpeed;
//
//            // 未来位置 = 現在地 + (速度 * 時間)
//            Math::Vector2 predictedPos = playerPos + (playerVel * timeToHit);
//
//            // 未来位置への角度を計算
//            Math::Vector2 dir = predictedPos - m_pos;
//            float angle = atan2f(dir.y, dir.x);
//
//            owner->AddEnemyBullet(m_pos, angle);
//            m_shootTimer = 100;
//        }
//
//    }
//
//}

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
            // ★修正：プレイヤーを狙わず、真左（180度）に撃つ
            // DirectX(Math)において、真右が0、真上がPI/2、真左がPI、真下が-PI/2です。
            float angle = DirectX::XM_PI; // 180度 (真左)

            // もし少し斜めにバラけさせたい場合は、少しランダム値を足すと「砲撃」っぽくなります
            // float randomOffset = (((rand() % 100) / 100.0f) - 0.5f) * 0.1f; // ±約3度
            // angle += randomOffset;

            owner->AddEnemyBullet(m_pos, angle);
            m_shootTimer = 100; // 次の弾までの間隔
        }
    }
}


void C_EnemyTurret::Draw(KdTexture* tex) {
    if (!m_alive || !tex) return;

    // --- ★修正箇所：行列の作成方法を変更 ---

    // 1. まず、水平方向（X軸）に -1 倍のスケールをかける（これで画像が反転する）
    Math::Matrix scaleMat = Math::Matrix::CreateScale(-1.0f, 1.0f, 1.0f);

    // 2. 位置(m_pos)の移動行列を作成
    Math::Matrix transMat = Math::Matrix::CreateTranslation(m_pos.x, m_pos.y, 0);

    // 3. 行列を合成 (反転 -> 移動)
    // ※順番が重要です。移動してから反転すると位置がズレます。
    Math::Matrix mat = scaleMat * transMat;

    // --- 以下の描画処理はそのまま ---

    // 行列をセット
    SHADER.m_spriteShader.SetMatrix(mat);

    // 画像の矩形 (64x64と仮定)
    Math::Rectangle srcRect = { 0, 0, 64, 64 };

    // 描画
    SHADER.m_spriteShader.DrawTex(tex, 0, 0, &srcRect);

    // 次の描画のために行列をリセット（お作法）
    SHADER.m_spriteShader.SetMatrix(Math::Matrix::Identity);
}