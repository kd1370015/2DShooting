#include "Charge.h"
#include "Application/Player/player.h"


C_Charge::C_Charge(KdTexture* tex, KdTexture* lowTex, C_Player* player) {
    m_tex = tex;
    m_gaugeLowTex = lowTex; // ここでしっかり受け取ります
    m_player = player;
}



void C_Charge::Update() {
    if (!m_player) return;

    m_pos = m_player->GetPos();
    float energy = m_player->GetEnergy();

    // 4段階チャージ（25%刻み）の計算
    int step = (int)(energy / 25.0f);
    m_currentX = (GAUGE_FULL_W / 4) * step;

    if (energy >= 100.0f) m_currentX = GAUGE_FULL_W;

    if (m_currentX < 0) m_currentX = 0;
    if (m_currentX > GAUGE_FULL_W) m_currentX = GAUGE_FULL_W;
}

void C_Charge::Draw(KdTexture* gaugeTex, KdTexture* iconTex) {
    // 必要なテクスチャが揃っていない場合は描画をスキップ
    if (!m_tex || !gaugeTex || !iconTex) return;

    // --- 0. 影の描画 (最背面) ---
    {
        // 影はゲージ画像全体を表示（空の状態でも枠があるように見せるため）
        Math::Rectangle srcRectShadow = { 0, 0, GAUGE_FULL_W, GAUGE_H };

        // 基準位置（-500, -300）から少し右下にずらす
        Math::Matrix shadowMat = Math::Matrix::CreateTranslation(-389, 305, 0);
        SHADER.m_spriteShader.SetMatrix(shadowMat);

        // 半透明の黒で描画
        Math::Color shadowColor = { 0, 0, 0, 1.0f };
        SHADER.m_spriteShader.DrawTex(gaugeTex, 0, 0, &srcRectShadow, &shadowColor);
    }


    // --- 1. ゲージ本体の描画 ---
    {
        Math::Rectangle srcRectGauge = { 0, 0, m_currentX, GAUGE_H };

        // 左端を固定するための位置計算 (-350を基準とする場合)
        float offsetX = (m_currentX - GAUGE_FULL_W) * 0.5f;
        Math::Matrix mat = Math::Matrix::CreateTranslation(-389 + offsetX, 305, 0);
        SHADER.m_spriteShader.SetMatrix(mat);

        // テクスチャの選択
        KdTexture* drawTex = gaugeTex;
        Math::Color finalColor = { 1.0f, 1.0f, 1.0f, 1.0f };

        if (m_player->IsUsingPower() && m_player->GetEnergy() < 100.0f) {
            float energy = m_player->GetEnergy();

            if (energy <= 25.0f) {
                // 保存しておいた m_gaugeLowTex (赤い画像) を使う
                if (m_gaugeLowTex) {
                    drawTex = m_gaugeLowTex;
                }
                else {
                    finalColor = Math::Color(1.0f, 0.4f, 0.4f, 1.0f);
                }
            }
            else {
                // 通常時は不透明のまま少しずつ赤くする
                float redRatio = 1.0f - (energy / 100.0f);
                float colorVar = 1.0f - (redRatio * 0.5f);
                finalColor = Math::Color(1.0f, colorVar, colorVar, 1.0f);
            }
        }

        SHADER.m_spriteShader.DrawTex(drawTex, 0, 0, &srcRectGauge, &finalColor);
    }


    // --- 2. 枠画像の描画 (ゲージの前面) ---
    {
        Math::Matrix mat = Math::Matrix::CreateTranslation(-389, 305, 0);
        SHADER.m_spriteShader.SetMatrix(mat);

        Math::Color color = { 1.0f, 1.0f, 1.0f, 1.0f };
        // 枠は画像全体を描画するので、第4引数はnullptr（または全体指定）
        SHADER.m_spriteShader.DrawTex(m_tex, 0, 0, nullptr, &color);
    }

    // --- 3. アイコンの描画 (最前面) ---
    {
        // ゲージの左端に合わせて配置（座標は微調整してください）
        Math::Matrix iconMat = Math::Matrix::CreateTranslation(-475 - 60.0f, -290 + 0.0f, 0);
        SHADER.m_spriteShader.SetMatrix(iconMat);

        SHADER.m_spriteShader.DrawTex(iconTex, 0, 0);
    }

    // 行列をリセット
    SHADER.m_spriteShader.SetMatrix(Math::Matrix::Identity);
}