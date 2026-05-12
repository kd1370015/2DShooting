#include "Explosion.h"

void C_Explosion::Init(Math::Vector2 pos) {
    m_pos = pos;
    m_animeTimer = 0;
    m_currentFrame = 0;
    m_isExpired = false;
}

void C_Explosion::Update() {
    if (m_isExpired) return;

    // タイマーを進める
    m_animeTimer++;

    // 一定時間ごとにコマを進める
    if (m_animeTimer >= ANIME_SPEED) {
        m_animeTimer = 0;
        m_currentFrame++;

        // 最後のコマを超えたら終了
        if (m_currentFrame >= ANIME_MAX) {
            m_isExpired = true;
        }
    }
}

void C_Explosion::Draw(KdTexture* tex) {
    if (m_isExpired || !tex) return;

    // --- 現在のコマに応じた矩形（切り抜き範囲）を計算 ---
    // 横一列のスプライトシートなので、X座標だけずらす
    float srcX = m_currentFrame * FRAME_W;
    Math::Rectangle srcRect = { (int)srcX, 0, (int)FRAME_W, (int)FRAME_H };

    // --- 行列で位置を指定 ---
    // 爆発は少し大きく見せても良い (CreateScale)
    Math::Matrix scaleMat = Math::Matrix::CreateScale(1.5f, 1.5f, 1.0f);
    Math::Matrix transMat = Math::Matrix::CreateTranslation(m_pos.x, m_pos.y, 0);

    SHADER.m_spriteShader.SetMatrix(scaleMat * transMat);

    // --- 描画 ---
    // 爆発は光らせたいので「加算合成」にするのがお薦め
    D3D.SetBlendState(BlendMode::Add);

    SHADER.m_spriteShader.DrawTex(tex, 0, 0, &srcRect);

    // お作法：加算合成を元に戻す
    D3D.SetBlendState(BlendMode::Alpha);
    SHADER.m_spriteShader.SetMatrix(Math::Matrix::Identity);
}