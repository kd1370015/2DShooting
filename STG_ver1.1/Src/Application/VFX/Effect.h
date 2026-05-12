#pragma once

// 循環参照を防ぐための前方宣言
// ★ 1. プレイヤーの定義を読み込む（前方宣言だけでは不十分な場合があります）
#include "Application/Player/player.h"

class C_Effect {
public:
    C_Effect(KdTexture* tex, C_Player* player, float scale);
    ~C_Effect() {}

    void Update();
    void Draw();

    bool IsAlive() const { return m_alive; }

private:
    KdTexture* m_tex = nullptr;
    C_Player* m_player = nullptr; // プレイヤーへの参照

    Math::Vector2 m_pos;
    float m_scale;
    bool m_alive = true;

    // アニメーション用
    int m_animeTimer = 0;
    int m_currentFrame = 0;

    // 画像シートの設定（今回の画像に合わせる）
    const int FRAME_MAX = 15;      // 全〇コマ
    const int COLS = 5;            // 横に5枚
    const int CHIP_SIZE = 80;     // 1枚のサイズ（画像名から推測 192x192）
    const int ANIME_SPEED = 2;     // 何フレームごとにコマを進めるか
};
