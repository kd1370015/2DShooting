#include "Boss.h"
#include "Application/Scene/GameScene/GameScene.h"

void C_Boss::Init(Math::Vector2 pos, float stopX) {
    C_EnemyTurret::Init(pos, stopX);
    m_baseY = pos.y;      // 初期位置をセンターとして保存
    m_floatTimer = 0.0f;
    m_hp = 10; // ボスHP
    m_maxHp = m_hp;

    // ★ ボス専用テクスチャをロード
    m_bossTex.Load("Texture/Enemy/boss.png");

    // --- 1～4の画像をロード ---
    for (int i = 0; i < 4; i++) {
        // i=0のとき"1"、i=3のとき"4"になるようにする
        std::string fileName = "Texture/Enemy/boss_turbo0" + std::to_string(i + 1) + ".png";
        m_turboTex[i].Load(fileName);
    }

    m_turboFrame = 0.0f;
}

void C_Boss::Update(const Math::Vector2& playerPos, const Math::Vector2& playerVel, GameScene* owner) {
    if (!m_alive) return;

    // 1. 移動は親クラスのロジック（左へ進んで止まる）をそのまま使う
    C_EnemyTurret::Update(playerPos, playerVel, owner);

    // 0.2f ずつ足すと、5フレームごとに画像が切り替わります
    m_turboFrame += 0.2f;
    if (m_turboFrame >= 4.0f) {
        m_turboFrame = 0.0f;
    }

    // 2. パターンの切り替えタイマー
    m_patternTimer++;
    if (m_patternTimer > 180) {
        m_pattern = rand() % 2;
        m_patternTimer = 0;
    }

    // 3. ボス専用タイマーによる攻撃判定
    if (m_isStopped) {
        // ★親の m_shootTimer ではなく、ボス専用のタイマーを減らす
        m_bossShootTimer--;

        if (m_bossShootTimer <= 0) {
            // パターンに応じた発射処理
            if (m_pattern == 0) {
                // 扇状弾のところ
                for (int i = -2; i <= 2; ++i) {
                    owner->AddBossBullet(m_pos, DirectX::XM_PI + (i * 0.2f)); // ここを修正
                }
            }
            else {
                // プレイヤーを狙う偏差射撃
                float dist = Math::Vector2::Distance(m_pos, playerPos);
                float timeToHit = dist / m_bulletSpeed;
                Math::Vector2 predictedPos = playerPos + (playerVel * timeToHit);
                Math::Vector2 dir = predictedPos - m_pos;
                owner->AddEnemyBullet(m_pos, atan2f(dir.y, dir.x));
            }

            // ★発射が終わったらボス専用タイマーをリセット
            m_bossShootTimer = 80;
        }
    }
}


void C_Boss::Draw(KdTexture* tex) {
    if (!m_alive || !tex) return;

    // --- ここから追加：上下の揺れ ---
    if (m_isStopped) {
        m_floatTimer += 0.05f; // 数値を大きくすると揺れが速くなります

        // sinf(m_floatTimer) は -1.0 ～ 1.0 の間を動く
        // それに揺れ幅（例：50.0f）を掛ける
        float offset = sinf(m_floatTimer) * 50.0f;

        // 基準位置からオフセット分ずらす
        m_pos.y = m_baseY + offset;
    }


    // ターボエフェクトの描画
	
    {
        int frameIdx = (int)m_turboFrame;

        // チェックなしで直接描画へ
        Math::Matrix turboScale = Math::Matrix::CreateScale(-1.5f, 1.5f, 1.0f);
        Math::Matrix turboTrans = Math::Matrix::CreateTranslation(m_pos.x + 120.0f, m_pos.y, 0);
        Math::Matrix turboMat = turboScale * turboTrans;

        SHADER.m_spriteShader.SetMatrix(turboMat);

        Math::Rectangle turboRect = { 0, 0, 64, 64 };
        SHADER.m_spriteShader.DrawTex(&m_turboTex[frameIdx], 0, 0, &turboRect);
    }


    // 1. 行列の作成（3倍のサイズで反転）
    Math::Matrix scaleMat = Math::Matrix::CreateScale(-2.0f, 2.0f, 1.0f);
    Math::Matrix transMat = Math::Matrix::CreateTranslation(m_pos.x, m_pos.y, 0);
    Math::Matrix mat = scaleMat * transMat;

    // 2. 点滅色の設定
    Math::Color color = { 1.0f, 1.0f, 1.0f, 1.0f };
    if (m_hitFlashTimer > 0) {
        if (m_hitFlashTimer % 2 == 0) {
            color = { 1.0f, 0.0f, 0.0f, 1.0f };
        }
    }

    // 3. 描画
    SHADER.m_spriteShader.SetMatrix(mat);

    // 画像のサイズに合わせて srcRect を調整してください（例: 128x128 など）
    Math::Rectangle srcRect = { 0, 0, 128, 128 };

    // ★ 第1引数を m_bossTex に変更
    SHADER.m_spriteShader.DrawTex(&m_bossTex, 0, 0, &srcRect, &color);

    SHADER.m_spriteShader.SetMatrix(Math::Matrix::Identity);
}