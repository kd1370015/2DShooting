#pragma once

class Option {
public:
    Option() {}
    void Init();
    void Update();
    void Draw();

    // 閉じるボタンが押されたか
    bool ShouldClose() const { return m_shouldClose; }

    void SetOldKeys(bool val) {
        m_oldSpaceKey = val;
        m_oldEscapeKey = val; // ESC(またはReturn)も同様に
    }

private:

	KdTexture m_moveFontTex;      // WASDの移動デモ用テクスチャ
    KdTexture m_parryFontTex;
	KdTexture m_dodgeFontTex;



    // テクスチャ
    KdTexture m_bgTex;        // option.jpg
    KdTexture m_keyJTex;      // J.png
	KdTexture m_keyKTex;      // W.png
    KdTexture m_bubbleTex;    // levelup.png (回避)


    KdTexture m_parryTex;   // バリア画像
    Math::Vector2 m_parryPlayerPos = { 150, -10 }; // パリィ用プレイヤーの固定位置
    float m_parryTimer = 0;     // パリィアニメ用の独立したタイマー
    int   m_parryFrame = 0;     // SF01_Parryの現在のコマ(0-39)

    float m_playerAnimTimer = 0; // プレイヤー用タイマー
    float m_effectAnimTimer = 0; // エフェクト用タイマー

    bool  m_isParrying = false;  // アニメーション中かどうかのフラグ
    int   m_parryWaitTimer = 0;  // 次のパリィまでの待ち時間

    KdTexture m_keyWTex;
    KdTexture m_keyATex;
    KdTexture m_keySTex;
    KdTexture m_keyDTex;
    KdTexture m_playerTex;    // デモ用のプレイヤーアイコン
	KdTexture m_playerParryTex; // デモ用のプレイヤーのパリィ中画像
    KdTexture m_playerDodgeTex; // 回避用テクスチャ

    float m_timer = 0;
    int   m_jFrame = 2;      // Jキーのアニメーションコマ
    bool  m_shouldClose = false;

    // ★追加：WASDデモ用
    int   m_demoKeyIdx = 0;        // 現在デモで押されているキー (0:W, 1:A, 2:S, 3:D)
    Math::Vector2 m_demoPlayerPos; // デモプレイヤーの座標

    // 各アクションのデモ用変数
    Math::Vector2 m_movePos = { 150, 50 }; // WASD移動のデモ座標
    float m_barrierAlpha = 0.0f;           // バリアの透明度
    float m_dodgeScale = 1.0f;             // 回避中の膨らみ


    // --- 回避デモ用変数 ---
    float m_dodgeAnimTimer = 0;
    bool  m_isDodging = false;
    int   m_dodgeWaitTimer = 0; // 次の回避デモまでの待ち時間

    Math::Vector2 m_dodgePlayerPos; // デモプレイヤーの座標

    bool m_oldSpaceKey = false; // 前回のスペースキーの状態
    bool m_oldEscapeKey = false; // 前回のESCキーの状態

    // 定数
    const int J_WIDTH = 19;
    const int J_HEIGHT = 21;

    const int WASD_WIDTH = 19;
    const int WASD_HEIGHT = 21;

    const int PARRY_PLAYER_ANM_COUNT = 60;
    const int PARRY_EFFECT_COL = 5;
    const int PARRY_EFFECT_ROW = 3;

    // 標準的なスプライトサイズ (例: 1コマ 64x64)
    const int PARRY_PLAYER_W = 80;
    const int PARRY_PLAYER_H = 80;
    const int PARRY_EFFECT_W = 80;
    const int PARRY_EFFECT_H = 80;
};
