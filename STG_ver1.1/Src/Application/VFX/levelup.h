#pragma once

class C_Player; // 前方宣言

class LevelUp {
public:
    LevelUp() {}
    ~LevelUp() {}

    // 初期化：テクスチャのポインタを受け取る
    void Init(KdTexture* tex, C_Player* player); // プレイヤーを登録

    // 更新：コマを進める
    void Update();

    // 描画：現在のコマを切り取って表示
    void DrawSprite();

    // エフェクト開始
    //void Start(const Math::Vector2& pos);

    void Start();
    // ★追加：外部からエフェクトを止めるための関数
    //void Stop() { m_active = false; }
   

    void SetLoop(bool loop) { m_isLoop = loop; } // ループ切り替え
    void SetActive(bool active) { m_active = active; }

    // 生存確認（終わったらfalseを返す）
    bool IsActive() const { return m_active; }
    void Stop() { m_shouldStop = true; } // 止めたいフラグを立てる

private:
    KdTexture* m_pTex = nullptr; // テクスチャへのポインタ
    KdTexture* m_tex = nullptr;
    KdTexture* m_pEffectTex = nullptr; // 新しいエフェクト用のみ
    C_Player* m_player = nullptr; // 追従対象
    Math::Vector2 m_pos;         // 出現位置
    bool m_alive = true;


    bool  m_active = false;      // 再生中か
    bool  m_isLoop = false;      // ★ループフラグ
    float m_frameIdx = 0;        // 現在のコマ番号（floatで持つと再生速度を細かく設定可能）

    // アニメーション用
    int m_animeTimer = 0;
    int m_currentFrame = 0;
    bool m_shouldStop = false; // ★追加


    // --- 画像に合わせた設定値 ---
    const int   m_maxFrame = 17;      // 画像の総コマ数（levelup.pngは40コマ前後）
    const float m_anmSpeed = 0.5f;    // 0.5なら2フレームに1コマ進む（秒間30コマ再生）
    const int   m_pieceSize = 80;    // 1コマのサイズ（画像幅が4000なら 4000/40 = 100）

    // 画像シートの設定（今回の画像に合わせる）
    const int FRAME_MAX = 15;      // 全〇コマ
    const int COLS = 5;            // 横に5枚
    const int CHIP_SIZE = 80;     // 1枚のサイズ（画像名から推測 192x192）
    const int ANIME_SPEED = 2;     // 何フレームごとにコマを進めるか

};