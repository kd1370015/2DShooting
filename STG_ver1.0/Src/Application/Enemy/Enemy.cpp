

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
//    float scale = 1.0f;
//
//    if (m_pParentManager && m_pParentManager->GetLeader().get() == this) {
//        // リーダーは赤く、かつ少し大きく
//        color = { 1.0f, 0.2f, 0.2f, 1.0f };
//        scale = 1.3f; // 30%大きく
//
//        // ★さらに明滅させる（チカチカ）
//        color.A(0.7f + 0.3f * sinf(APP.m_fps * 0.1f));
//    }
//
//    Math::Matrix mat = Math::Matrix::CreateScale(scale) * Math::Matrix::CreateTranslation(m_pos.x, m_pos.y, 0);
//
//    SHADER.m_spriteShader.SetMatrix(mat);
//    // DrawTexに色とアルファを反映（環境に合わせて引数を調整してください）
//    SHADER.m_spriteShader.DrawTex(tex, Math::Rectangle(0, 0, 32, 32), color.A());
//}


void C_Enemy::Draw(KdTexture* tex) {
    if (!m_alive || !tex) return;

    Math::Color color = { 1.0f, 1.0f, 1.0f, 1.0f };
    float scale = 1.0f;

    // リーダー判定の演出設定
    if (m_pParentManager && m_pParentManager->GetLeader().get() == this) {
        color = { 1.0f, 0.2f, 0.2f, 1.0f }; // 赤っぽく
        scale = 1.3f; // 30%大きく

        // 明滅（チカチカ）演出
        // ※APP.m_fpsだと固定値なので、経過時間やカウンタを使うのが一般的ですが、
        //   今のコードを活かすならそのままにします
        color.A(0.7f + 0.3f * sinf(GetTickCount() * 0.01f));
    }

    // 1. 行列の作成（拡大率と座標をセット）
    Math::Matrix mat = Math::Matrix::CreateScale(scale) * Math::Matrix::CreateTranslation(m_pos.x, m_pos.y, 0);

    // 2. 行列をシェーダーにセット
    SHADER.m_spriteShader.SetMatrix(mat);

    // 3. 矩形をポインタで渡す準備
    Math::Rectangle srcRect = { 0, 0, 32, 32 };

    // 4. 新しい DrawTex の定義に合わせて呼び出し
    // 引数: (テクスチャ, x, y, 矩形ポインタ, 色ポインタ)
    // 行列で座標を決めているので x=0, y=0 で描画します
    SHADER.m_spriteShader.DrawTex(tex, 0, 0, &srcRect, &color);
}


void C_Enemy::Update(const Math::Vector2& targetPos) {
    if (!m_alive) return;

    // 1. 目標へのベクトルを計算
    Math::Vector2 move = (targetPos - m_pos) * 0.05f;

    // 2. ★リミッター：1フレームの移動量が大きすぎたら制限する
    float maxSpeed = 3.0f; // ここを調整して「最高速度」を決める
    float len = move.Length();
    if (len > maxSpeed) {
        move.Normalize();
        move *= maxSpeed;
    }
    m_pos += move;

    // 3. 上下の揺らぎ（ふわふわ）
    m_pos.y += sinf(m_pos.x * 0.02f) * 0.5f;

    // 画面左端（横画面）に消えたら死亡扱い
    if (m_pos.x < -700) m_alive = false;
}


