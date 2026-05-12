#include "Charge.h"
#include "Application/Player/player.h"

// コンストラクタ：枠画像（charge_barframe.png）とプレイヤーへのポインタを受け取る
//C_Charge::C_Charge(KdTexture* tex, C_Player* player) {
//    m_tex = tex;
//    m_player = player;
//}

C_Charge::C_Charge(KdTexture* tex, KdTexture* lowTex, C_Player* player) {
    m_tex = tex;
    m_gaugeLowTex = lowTex; // ここでしっかり受け取ります
    m_player = player;
}


//void C_Charge::Update() {
//    if (!m_player) return;
//
//    m_pos = m_player->GetPos();
//    float energy = m_player->GetEnergy(); // 0 ～ 100
//
//    // --- 修正：4回チャージ（25単位）の計算 ---
//    // 100を4分割するので、25ごとにステップを分ける
//    // energyが 0〜24.9ならstep0, 25〜49.9ならstep1...
//    int step = (int)(energy / 25.0f);
//
//    // ステップ数に応じて、描画する横幅を「カチッ」と決める
//    // 例：GAUGE_FULL_Wが272なら、1ステップごとに68ピクセルずつ増える
//    m_currentX = (GAUGE_FULL_W / 4) * step;
//
//    // もしエネルギーが100ピッタリなら、確実に全表示にする
//    if (energy >= 100.0f) {
//        m_currentX = GAUGE_FULL_W;
//    }
//
//    // 安全策
//    if (m_currentX < 0) m_currentX = 0;
//    if (m_currentX > GAUGE_FULL_W) m_currentX = GAUGE_FULL_W;
//}

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
        Math::Matrix shadowMat = Math::Matrix::CreateTranslation(-350, -325, 0);
        SHADER.m_spriteShader.SetMatrix(shadowMat);

        // 半透明の黒で描画
        Math::Color shadowColor = { 0, 0, 0, 1.0f };
        SHADER.m_spriteShader.DrawTex(gaugeTex, 0, 0, &srcRectShadow, &shadowColor);
    }

    //// --- 1. ゲージ本体の描画 (中層) ---
    //{
    //    // Updateで計算したm_currentXまでを切り出す
    //    Math::Rectangle srcRectGauge = { 0, 0, m_currentX, GAUGE_H };

    //    // --- ★修正ポイント：左端を固定する計算 ---
    //    // 画像の中央を基準にする性質を打ち消すため、右にずらします
    //    float offsetX = (m_currentX - GAUGE_FULL_W) * 0.5f;

    //    // 元々の基準位置 -350 に、計算したオフセットを加える
    //    Math::Matrix mat = Math::Matrix::CreateTranslation(-350 + offsetX, -300, 0);
    //    SHADER.m_spriteShader.SetMatrix(mat);

    //    //Math::Color color = { 1.0f, 1.0f, 1.0f, 1.0f };

    //    //Math::Color finalColor = { 1.0f, 1.0f, 1.0f, 1.0f }; // デフォルトは白

    //    //// --- 強化消費中（カウントダウン中）の点滅演出 ---
    //    //if (m_player->IsUsingPower() && m_player->GetEnergy() < 100.0f) {
    //    //    float energy = m_player->GetEnergy();
    //    //    float speed = (105.0f - energy) * 0.01f;
    //    //    float s = sinf(static_cast<float>(GetTickCount()) * 0.01f * speed);
    //    //    float redRatio = 1.0f - (energy / 100.0f);
    //    //    float alpha = 0.8f + s * 0.2f;

    //    //    color = { 1.0f, 1.0f - redRatio, 1.0f - redRatio, alpha };
    //    //}

    //    // 描画に使うテクスチャのポインタ。基本は普通のゲージ
    //    KdTexture* drawTex = gaugeTex;
    //    Math::Color finalColor = { 1.0f, 1.0f, 1.0f, 1.0f }; // 色も基本はそのまま

    //    if (m_player->IsUsingPower() && m_player->GetEnergy() < 100.0f) {
    //        float energy = m_player->GetEnergy();

    //        if (energy <= 25.0f) {
    //            // ★【ここがポイント】：25%以下の時は、渡された「赤い画像」に切り替える
    //            // gaugeLowTex は GameScene から渡す charge_barlow.png です
    //            if (m_gaugeLowTex) {
    //                drawTex = m_gaugeLowTex;
    //            }
    //            else {
    //                // 画像がない場合のバックアップとして色を赤くする
    //                finalColor = Math::Color(1.0f, 0.4f, 0.4f, 1.0f);
    //            }
    //        }
    //        else {
    //            // 通常時は、白から少しずつ赤みを帯びさせる（不透明のまま）
    //            float redRatio = 1.0f - (energy / 100.0f);
    //            float colorVar = 1.0f - (redRatio * 0.5f);
    //            finalColor = Math::Color(1.0f, colorVar, colorVar, 1.0f);
    //        }
    //    }

    //    // drawTex（通常 or 赤）を使って描画
    //    SHADER.m_spriteShader.DrawTex(drawTex, 0, 0, &srcRectGauge, &finalColor);

    //    // ★DrawTexの第2・第3引数は 0, 0 でOK（行列で位置を決めているため）
    //    //SHADER.m_spriteShader.DrawTex(gaugeTex, 0, 0, &srcRectGauge, &color);
    //}

    // --- 1. ゲージ本体の描画 ---
    {
        Math::Rectangle srcRectGauge = { 0, 0, m_currentX, GAUGE_H };

        // 左端を固定するための位置計算 (-350を基準とする場合)
        float offsetX = (m_currentX - GAUGE_FULL_W) * 0.5f;
        Math::Matrix mat = Math::Matrix::CreateTranslation(-350 + offsetX, -325, 0);
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
        Math::Matrix mat = Math::Matrix::CreateTranslation(-350, -325, 0);
        SHADER.m_spriteShader.SetMatrix(mat);

        Math::Color color = { 1.0f, 1.0f, 1.0f, 1.0f };
        // 枠は画像全体を描画するので、第4引数はnullptr（または全体指定）
        SHADER.m_spriteShader.DrawTex(m_tex, 0, 0, nullptr, &color);
    }

    // --- 3. アイコンの描画 (最前面) ---
    {
        // ゲージの左端に合わせて配置（座標は微調整してください）
        Math::Matrix iconMat = Math::Matrix::CreateTranslation(-500 - 60.0f, -300 + 0.0f, 0);
        SHADER.m_spriteShader.SetMatrix(iconMat);

        SHADER.m_spriteShader.DrawTex(iconTex, 0, 0);
    }

    // 行列をリセット
    SHADER.m_spriteShader.SetMatrix(Math::Matrix::Identity);
}