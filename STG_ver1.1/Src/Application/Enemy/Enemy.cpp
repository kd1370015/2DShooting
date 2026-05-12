

#include "Enemy.h"
#include "EnemyManager.h" // ★これを追加

// Enemy.cpp 内
void C_Enemy::Init(Math::Vector2 pos, C_EnemyManager* manager) {
    m_pos = pos;
    m_pParentManager = manager; // 管理者をセット
    m_alive = true;
}




//void C_Enemy::Draw(KdTexture* tex) {
//    if (!m_alive || !tex) return;
//
//    Math::Color color = { 1.0f, 1.0f, 1.0f, 1.0f };
//    float scale = -1.0f;
//
//    // 1. 行列の作成（拡大率と座標をセット）
//    Math::Matrix mat = Math::Matrix::CreateScale(scale) * Math::Matrix::CreateTranslation(m_pos.x, m_pos.y, 0);
//
//    // 2. 行列をシェーダーにセット
//    SHADER.m_spriteShader.SetMatrix(mat);
//
//    // 3. 矩形をポインタで渡す準備
//    Math::Rectangle srcRect = { 0, 0, 64, 64 };
//
//    // 4. 新しい DrawTex の定義に合わせて呼び出し
//    // 引数: (テクスチャ, x, y, 矩形ポインタ, 色ポインタ)
//    // 行列で座標を決めているので x=0, y=0 で描画します
//    SHADER.m_spriteShader.DrawTex(tex, 0, 0, &srcRect, &color);
//}

void C_Enemy::Draw(KdTexture* tex) {
    if (!m_alive || !tex) return;

    Math::Color color = { 1.0f, 1.0f, 1.0f, 1.0f };

    // 行列の作成：拡大率 * 回転 * 平行移動 の順番が鉄則です！
    Math::Matrix mat = Math::Matrix::CreateScale(-1.0f) * Math::Matrix::CreateRotationZ(m_angle) * Math::Matrix::CreateTranslation(m_pos.x, m_pos.y, 0);

    SHADER.m_spriteShader.SetMatrix(mat);
    Math::Rectangle srcRect = { 0, 0, 64, 64 };
    SHADER.m_spriteShader.DrawTex(tex, 0, 0, &srcRect, &color);
}



//void C_Enemy::Update(const Math::Vector2& targetPos, float timeScale) {
//    if (!m_alive) return;
//
//    Math::Vector2 toTarget = targetPos - m_pos;
//    float dist = toTarget.Length();
//
//    Math::Vector2 targetVelocity;
//
//    // 指定した距離（250px）より近い場合だけ追尾
//    if (dist < m_detectRange) {
//        m_isChasing = true; // 一度見つけたら追尾開始
//        toTarget.Normalize();
//        targetVelocity = toTarget * m_speed;
//
//        // ★角度の計算：プレイヤーの方を向く
//        // atan2f(y, x) でラジアン（角度）が求まります
//        // 元の画像が「右向き」ならそのまま、そうでない場合はオフセットを加算します
//        m_angle = atan2f(toTarget.y, toTarget.x);
//    }
//    else {
//        // 通常は左へ移動
//        targetVelocity = Math::Vector2(-m_speed, 0);
//        // 通常移動時は左向き（180度 = PI）
//        m_angle = DirectX::XM_PI;
//    }
//
//    // 慣性移動（ゆっくり向きを変える）
//    m_velocity = Math::Vector2::Lerp(m_velocity, targetVelocity, 0.05f);
//    m_pos += m_velocity * timeScale;
//
//    // 画面外判定
//    if (m_pos.x < -750) m_alive = false;
//}

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

    // 画面外判定
    if (m_pos.x < -750) m_alive = false;
}
