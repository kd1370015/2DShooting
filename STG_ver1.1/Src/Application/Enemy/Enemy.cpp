

#include "Enemy.h"
#include "EnemyManager.h" // ★これを追加

// Enemy.cpp 内
void C_Enemy::Init(Math::Vector2 pos, C_EnemyManager* manager) {
    m_pos = pos;
    m_pParentManager = manager; // 管理者をセット
    m_alive = true;

    // --- ボスと同じ感じでロード ---
    for (int i = 0; i < 4; i++) {
        std::string fileName = "Texture/Enemy/E_T_" + std::to_string(i + 1) + ".png";
        m_turboTex[i].Load(fileName);
    }
    m_turboFrame = 0.0f;
}





void C_Enemy::Draw(KdTexture* tex) {
    if (!m_alive || !tex) return;

    // ターボエフェクトの描画

    int frameIdx = (int)m_turboFrame;

    // --- 1. ターボの描画（ボスの後ろと同じロジック） ---
    // 敵の向き(m_angle)に合わせて背後にずらす
    Math::Matrix turboScale = Math::Matrix::CreateScale(-0.85f, 0.85f, 1.0f);
    Math::Matrix turboRot = Math::Matrix::CreateRotationZ(m_angle);

    // 背後方向に30pxほどずらす
    Math::Vector2 offset = { -30.0f, 5.0f };
    Math::Vector2 rotatedOffset = Math::Vector2::Transform(offset, turboRot);

    Math::Matrix turboTrans = Math::Matrix::CreateTranslation(
        m_pos.x + rotatedOffset.x,
        m_pos.y + rotatedOffset.y,
        0.05f // Z値を少し奥へ
    );

    SHADER.m_spriteShader.SetMatrix(turboScale * turboRot * turboTrans);
    Math::Rectangle turboRect = { 0, 0, 64, 64 };
    SHADER.m_spriteShader.DrawTex(&m_turboTex[frameIdx], 0, 0, &turboRect);

    Math::Color color = { 1.0f, 1.0f, 1.0f, 1.0f };

    // 行列の作成：拡大率 * 回転 * 平行移動 の順番が鉄則です！
    Math::Matrix mat = Math::Matrix::CreateScale(-1.0f) * Math::Matrix::CreateRotationZ(m_angle) * Math::Matrix::CreateTranslation(m_pos.x, m_pos.y, 0);

    SHADER.m_spriteShader.SetMatrix(mat);
    Math::Rectangle srcRect = { 0, 0, 64, 64 };
    SHADER.m_spriteShader.DrawTex(tex, 0, 0, &srcRect, &color);
}




void C_Enemy::Update(const Math::Vector2& targetPos, float timeScale) {
    if (!m_alive) return;

    Math::Vector2 toTarget = targetPos - m_pos;
    float dist = toTarget.Length();

    // --- 追尾状態の切り替えロジック ---
    if (!m_isChasing) {
        // 1. まだ追尾していない場合：索敵範囲に入ったら開始
        if (dist < m_detectRange) {
            m_isChasing = true;
        }
    }
    else {
        // 2. 追尾中の場合：以下のいずれかで追尾を諦める
        // ・距離が離れすぎた (450px)
        // ・プレイヤーを追い越した (自分のXがプレイヤーのXより左になった)
        if (dist > 450.0f || m_pos.x < targetPos.x - 50.0f) {
            m_isChasing = false;
        }
    }

    Math::Vector2 targetVelocity;

    if (m_isChasing) {
        // 【追尾中】プレイヤーに向かう
        toTarget.Normalize();
        targetVelocity = toTarget * m_speed;
        m_angle = atan2f(toTarget.y, toTarget.x);
    }
    else {
        // 【通常・諦め】左へ飛び去る
        targetVelocity = Math::Vector2(-m_speed, 0);

        // 角度をなめらかに左向き(PI)に戻す
        float targetAngle = DirectX::XM_PI;
        float angleDiff = targetAngle - m_angle;
        // 角度の差を -PI ~ PI の範囲に補正（回転の最短ルートを通るため）
        if (angleDiff > DirectX::XM_PI) angleDiff -= DirectX::XM_2PI;
        if (angleDiff < -DirectX::XM_PI) angleDiff += DirectX::XM_2PI;

        m_angle += angleDiff * 0.05f;
    }

    // 慣性を適用して移動
    m_velocity = Math::Vector2::Lerp(m_velocity, targetVelocity, 0.05f);
    m_pos += m_velocity * timeScale;


    // --- ターボアニメーションの更新 ---
    m_turboFrame += 0.2f * timeScale;
    if (m_turboFrame >= 4.0f) {
        m_turboFrame = 0.0f;
    }

    // 画面外判定
    if (m_pos.x < -750) m_alive = false;
}
