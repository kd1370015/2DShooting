#include "Effect.h"
#include "Application/Player/player.h" // プレイヤーの座標を得るために必要

C_Effect::C_Effect(KdTexture* tex, C_Player* player, float scale) {
    m_tex = tex;
    m_player = player;
    m_scale = scale;
    if (m_player) {
        m_pos = m_player->GetPos();
    }
}

void C_Effect::Update() {
    if (!m_alive) return;

    // ★重要：プレイヤーが生きていれば、毎フレーム位置を同期させる
    if (m_player) {
        m_pos = m_player->GetPos();
    }

    m_animeTimer++;
    if (m_animeTimer >= ANIME_SPEED) {
        m_animeTimer = 0;
        m_currentFrame++;
        if (m_currentFrame >= FRAME_MAX) {
            m_alive = false;
        }
    }
}


void C_Effect::Draw() {
    if (!m_alive || !m_tex) return;

    // --- 1. 加算合成に設定 ---
    // これにより、エフェクトの黒い部分が透け、光る部分が重なってプレイヤーが見えるようになります
    D3D.SetBlendState(BlendMode::Add);

    // コマ数から切り出し位置を計算
    int x = (m_currentFrame % COLS) * CHIP_SIZE;
    int y = (m_currentFrame / COLS) * CHIP_SIZE;
    Math::Rectangle srcRect = { x, y, CHIP_SIZE, CHIP_SIZE };

    // 行列作成
    Math::Matrix mat = Math::Matrix::CreateScale(m_scale) * Math::Matrix::CreateTranslation(m_pos.x, m_pos.y, 0);
    SHADER.m_spriteShader.SetMatrix(mat);

    // 描画（色は白、透明度は1.0でも加算合成なら綺麗に透けます）
    Math::Color color = { 1.0f, 1.0f, 1.0f, 1.0f };
    SHADER.m_spriteShader.DrawTex(m_tex, 0, 0, &srcRect, &color);

    // --- 2. ブレンドモードを通常（アルファブレンド）に戻す ---
    // これを忘れると、この後に描画されるUIやクロスヘアまで光ってしまいます
    D3D.SetBlendState(BlendMode::Alpha);

    // 行列リセット
    SHADER.m_spriteShader.SetMatrix(Math::Matrix::Identity);
}