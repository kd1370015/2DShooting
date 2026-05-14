

#include "LevelUp.h"
#include "Application/Player/player.h"

void LevelUp::Init(KdTexture* tex, C_Player* player) {
    m_pTex = tex;
    m_player = player;
    m_active = false;
}

void LevelUp::Start() {
    m_active = true;
    m_currentFrame = 0;
    m_animeTimer = 0;

    if (m_player) {
        m_pos = m_player->GetPos();
    }
}



void LevelUp::Update() {
    if (!m_active) return;

    // プレイヤー追従
    if (m_player) m_pos = m_player->GetPos();

    m_animeTimer++;
    if (m_animeTimer >= ANIME_SPEED) {
        m_animeTimer = 0;
        m_currentFrame++;

        if (m_currentFrame >= FRAME_MAX) {
            // ★ここがポイント
            if (m_shouldStop) {
                // ストップ指示が出ていれば、ループせずに終了
                m_active = false;
            }
            else {
                // 指示がなければ 0 に戻してループ
                m_currentFrame = 0;
            }
        }
    }
}

void LevelUp::DrawSprite() {
    if (!m_active || !m_pTex || !m_player) return;

    // 1. 加算合成（光る演出）に設定
    D3D.SetBlendState(BlendMode::Add);

    // コマ数から切り出し位置(srcRect)を計算
    int x = (m_currentFrame % COLS) * CHIP_SIZE;
    int y = (m_currentFrame / COLS) * CHIP_SIZE;
    Math::Rectangle srcRect = { x, y, CHIP_SIZE, CHIP_SIZE };

    // 行列作成 (Scaleは新しい画像の見栄えに合わせて 1.0f ～ 3.0f で調整してください)
    Math::Matrix mat = Math::Matrix::CreateScale(2.5f) *
        Math::Matrix::CreateTranslation(m_pos.x, m_pos.y, 0);

    SHADER.m_spriteShader.SetMatrix(mat);

    // 描画
    Math::Color color = { 1.0f, 1.0f, 1.0f, 1.0f };
    SHADER.m_spriteShader.DrawTex(m_pTex, 0, 0, &srcRect, &color);

    // 2. ブレンドモードを通常に戻す
    D3D.SetBlendState(BlendMode::Alpha);
    SHADER.m_spriteShader.SetMatrix(Math::Matrix::Identity);
}