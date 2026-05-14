

#include "EnemyTurret.h"
#include "Application/Scene/GameScene/GameScene.h"



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

    // ★追加：点滅タイマーをカウントダウン
    if (m_hitFlashTimer > 0) {
        m_hitFlashTimer--;
    }

    // --- ターボアニメーションの更新 ---
    m_turboFrame += 0.2f;
    if (m_turboFrame >= 4.0f) {
        m_turboFrame = 0.0f;
    }
}

void C_EnemyTurret::Draw(KdTexture* tex) {
    if (!m_alive || !tex) return;

        // 現在のアニメーション枠（0, 1, 2, 3）を取得
     int frameIdx = (int)m_turboFrame;

    // --- 1. ターボの描画（本体の後ろ） ---
    // 雑魚敵の進行方向（左）に合わせて、右側にターボを表示
    // スケールは 1.0f（または敵のサイズに合わせて調整）
    Math::Matrix turboScale = Math::Matrix::CreateScale(-1.0f, 1.0f, 1.0f);
    // 右に 30px ほどずらして配置
    Math::Matrix turboTrans = Math::Matrix::CreateTranslation(m_pos.x + 30.0f, m_pos.y, 0.05f); // 0.05fで少し奥へ

    SHADER.m_spriteShader.SetMatrix(turboScale * turboTrans);

    Math::Rectangle turboRect = { 0, 0, 64, 64 };
    // 配列の画像を描画
    SHADER.m_spriteShader.DrawTex(&m_turboTex[frameIdx], 0, 0, &turboRect);

    // --- 1. 行列の作成 (反転 & 移動) ---
    Math::Matrix scaleMat = Math::Matrix::CreateScale(-1.0f, 1.0f, 1.0f);
    Math::Matrix transMat = Math::Matrix::CreateTranslation(m_pos.x, m_pos.y, 0);
    Math::Matrix mat = scaleMat * transMat;

    // --- 2. 点滅色の設定 ---
    // 通常は白（1, 1, 1, 1）
    Math::Color color = { 1.0f, 1.0f, 1.0f, 1.0f };


    if (m_hitFlashTimer > 0) {
        // 2フレーム（約0.03秒）ごとに赤と白を切り替える
        // この数字を 1 にすると最速になります
        if ((m_hitFlashTimer / 2) % 2 == 0) {
            color = { 1.0f, 0.0f, 0.0f, 1.0f }; // 赤
        }
        else {
            color = { 1.0f, 1.0f, 1.0f, 1.0f }; // 通常色
        }
    }


    // --- 3. 描画処理 ---
    // 行列をセット
    SHADER.m_spriteShader.SetMatrix(mat);

    // 画像の矩形 (64x64)
    Math::Rectangle srcRect = { 0, 0, 64, 64 };

    // 描画関数の引数に &color を追加
    // 第2、第3引数は行列(mat)で位置が決まるので 0 でOKです
    SHADER.m_spriteShader.DrawTex(tex, 0, 0, &srcRect, &color);

    // 行列をリセット
    SHADER.m_spriteShader.SetMatrix(Math::Matrix::Identity);
}


