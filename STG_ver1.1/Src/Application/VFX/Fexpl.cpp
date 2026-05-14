#include "Fexpl.h"

void F_Explosion::Init(Math::Vector2 pos) {
    m_pos = pos;
    m_animeTimer = 0;
    m_currentFrame = 0;
    m_isExpired = false;
}

void F_Explosion::Update() {
    if (m_isExpired) return;

    // アニメーションタイマー加算
    m_animeTimer++;

    // 一定時間（ANIME_SPEED）経ったら次のコマへ
    if (m_animeTimer >= ANIME_SPEED) {
        m_animeTimer = 0;
        m_currentFrame++;

        // 全てのコマを表示し終えたら終了
        if (m_currentFrame >= ANIME_MAX) {
            m_isExpired = true;
        }
    }
}

void F_Explosion::Draw(KdTexture* tex) {
    if (m_isExpired || !tex) return;

    // 切り抜き範囲（スプライトシートが横並びであることを想定）
    Math::Rectangle srcRect = {
        (int)(m_currentFrame * FRAME_W),
        0,
        (int)FRAME_W,
        (int)FRAME_H
    };

    // 配置・サイズ行列
    Math::Matrix scaleMat = Math::Matrix::CreateScale(1.0f, 1.0f, 1.0f);
    Math::Matrix transMat = Math::Matrix::CreateTranslation(m_pos.x, m_pos.y, 0);

    SHADER.m_spriteShader.SetMatrix(scaleMat * transMat);

    // 爆発を綺麗に見せるための「加算合成」
    D3D.SetBlendState(BlendMode::Add);

    SHADER.m_spriteShader.DrawTex(tex, 0, 0, &srcRect);

    // 後片付け（通常のブレンドモードに戻す）
    D3D.SetBlendState(BlendMode::Alpha);
    SHADER.m_spriteShader.SetMatrix(Math::Matrix::Identity);
}