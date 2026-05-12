#include "Option.h"
#include <algorithm>

void Option::Init() {
    m_bgTex.Load("Texture/Scene/123.png");
    m_keyJTex.Load("Texture/Scene/J.png");
	m_keyKTex.Load("Texture/Scene/K.png");
    m_bubbleTex.Load("Texture/Effect/levelup.png");
    m_parryTex.Load("Texture/VFX/parryeffect.png"); // バリア画像

	m_moveFontTex.Load("Texture/Scene/Move.png");
	m_parryFontTex.Load("Texture/Scene/Reflect.png");
	m_dodgeFontTex.Load("Texture/Scene/Dodge.png");

    m_keyWTex.Load("Texture/Scene/W.png");
    m_keyATex.Load("Texture/Scene/A.png");
    m_keySTex.Load("Texture/Scene/S.png");
    m_keyDTex.Load("Texture/Scene/D.png");
    m_playerTex.Load("Texture/Player/SF01.png"); // プレイヤー画像
	m_playerParryTex.Load("Texture/Player/SF01_Parry.png"); // プレイヤーのパリィ中画像)
    m_playerDodgeTex.Load("Texture/Player/SF01_kaihi.png");




    // Xを -300 -> -200（右へ）、Yを -100 -> 0（上へ）
    //m_demoPlayerPos = { -200.0f, 0.0f };
    m_demoPlayerPos = { -275.0f, -30.0f };
    // 配置（Kキーのデモなので、パリィの下あたりに配置）
    m_dodgePlayerPos = { 250, 50 }; // お好みで調整してください
    m_parryTimer = 0;
    m_shouldClose = false;

    // Initの最後に現在の状態を覚えさせてしまう
    m_oldSpaceKey = (GetAsyncKeyState(VK_SPACE) & 0x8000) != 0;
    m_oldEscapeKey = (GetAsyncKeyState(VK_RETURN) & 0x8000) != 0;

}



void Option::Update() {
    m_timer += 0.15f;
    m_jFrame = (static_cast<int>(m_timer) % 3);

    // --- 1. WASD自動デモ ---
    static float s_demoTimer = 0;
    s_demoTimer += 0.15f;
    if (s_demoTimer > 6.0f) {
        m_demoKeyIdx = (m_demoKeyIdx + 1) % 4;
        s_demoTimer = 0;
    }

    // --- 1. 移動速度をアップ (1.0f -> 1.5f) ---
    float moveSpeed = 1.5f;
    switch (m_demoKeyIdx) {
    case 0: m_demoPlayerPos.y += moveSpeed; break; // W
    case 1: m_demoPlayerPos.x -= moveSpeed; break; // A
    case 2: m_demoPlayerPos.y -= moveSpeed; break; // S
    case 3: m_demoPlayerPos.x += moveSpeed; break; // D
    }

    // --- 2. 移動制限（クランプ）の幅を大きく広げる ---

    // 横幅を 100px から 200px の範囲に拡張
    // 中心を -260 とし、左右に 100 ずつ動けるように設定
    if (m_demoPlayerPos.x < -360.0f) m_demoPlayerPos.x = -360.0f;
    if (m_demoPlayerPos.x > -160.0f) m_demoPlayerPos.x = -160.0f;

    // 縦幅も広げて、よりダイナミックに上下させる
    // WASDキー(-250)の上で、大きくゆらゆら動かします
    if (m_demoPlayerPos.y < -130.0f) m_demoPlayerPos.y = -130.0f;
    if (m_demoPlayerPos.y > 70.0f)  m_demoPlayerPos.y = 70.0f;



    if (!m_isParrying) {
        m_parryWaitTimer--;
        if (m_parryWaitTimer <= 0) {
            m_isParrying = true;
            m_playerAnimTimer = 0.0f;
            m_effectAnimTimer = 0.0f;
        }
    }
    else {
        // プレイヤーの再生速度（お好みの速度に調整可能）
        m_playerAnimTimer += 1.75f;

        // エフェクトの再生速度（実機合わせ）
        m_effectAnimTimer += 0.4f;

        // ★ プレイヤーの全60コマが終わったら「再生終了」とする
        if (m_playerAnimTimer >= 60.0f) {
            m_isParrying = false;
            m_parryWaitTimer = 80; // 次のデモまでの待ち時間（間を置く）
        }
    }

    // --- 3. 回避アニメーションの更新 ---
    if (!m_isDodging) {
        m_dodgeWaitTimer--;
        if (m_dodgeWaitTimer <= 0) {
            m_isDodging = true;
            m_dodgeAnimTimer = 0.0f;
        }
    }
    else {
        // 実機が 30フレームで60コマ進む設定なので、
        // 1フレームにつき 2.0 加算すると実機と同じ速度になります。
        m_dodgeAnimTimer += 2.0f;

        if (m_dodgeAnimTimer >= 60.0f) {
            m_isDodging = false;
            m_dodgeWaitTimer = 90; // 回避後の待ち時間
        }
    }

    // 現在のキー入力状態を取得
    bool nowSpace = (GetAsyncKeyState(VK_SPACE) & 0x8000) != 0;
    bool nowEscape = (GetAsyncKeyState(VK_RETURN) & 0x8000) != 0;

    // ★重要：【今押されている】かつ【前回は押されていなかった】場合のみ実行
    if ((nowSpace && !m_oldSpaceKey) || (nowEscape && !m_oldEscapeKey)) {
        m_shouldClose = true;
    }

    // 次のフレームのために今の状態を保存
    m_oldSpaceKey = nowSpace;
    m_oldEscapeKey = nowEscape;


}




void Option::Draw() {
    // --- 1. 背景の描画 ---
    Math::Matrix bgMat = Math::Matrix::CreateScale(1.0f) * Math::Matrix::CreateTranslation(-10, 0, 0);
    SHADER.m_spriteShader.SetMatrix(bgMat);
    SHADER.m_spriteShader.DrawTex(&m_bgTex, 0, 0);

    SHADER.m_spriteShader.SetMatrix(Math::Matrix::Identity);

    // --- 2. WASDキーの十字配置描画 (既存のまま) ---
    Math::Vector2 wasdBasePos = { -300, -250 };
    float keySpan = 60.0f;
    KdTexture* wasdTexs[] = { &m_keyWTex, &m_keyATex, &m_keySTex, &m_keyDTex };
    Math::Vector2 keyOffsets[] = { {0, keySpan}, {-keySpan, 0}, {0, 0}, {keySpan, 0} };
    for (int i = 0; i < 4; ++i) {
        int frame = (m_demoKeyIdx == i) ? 1 : 0;
        Math::Rectangle srcRect = { frame * WASD_WIDTH, 0, WASD_WIDTH, WASD_HEIGHT };
        Math::Matrix kMat = Math::Matrix::CreateScale(3.0f) * Math::Matrix::CreateTranslation(wasdBasePos.x + keyOffsets[i].x, wasdBasePos.y + keyOffsets[i].y, 0);
        SHADER.m_spriteShader.SetMatrix(kMat);
        SHADER.m_spriteShader.DrawTex(wasdTexs[i], 0, 0, &srcRect);
    }

    // --- 3. WASDで【移動する】プレイヤーの描画 (既存のまま) ---
    {
        Math::Matrix pMat = Math::Matrix::CreateScale(1.5f) * Math::Matrix::CreateRotationZ(-DirectX::XM_PIDIV2) * Math::Matrix::CreateTranslation(m_demoPlayerPos.x, m_demoPlayerPos.y, 0);
        SHADER.m_spriteShader.SetMatrix(pMat);
        SHADER.m_spriteShader.DrawTex(&m_playerTex, 0, 0);
    }




    // --- 4. 固定位置で【パリィする】プレイヤー ---
    {
        const int P_W = 80;
        const int P_H = 80;

        int pFrame = 0; // 基本は0番目（立ちポーズ）

        if (m_isParrying) {
            pFrame = static_cast<int>(m_playerAnimTimer);

            // ★重要：画像の外を参照しないようにガード
            if (pFrame >= 60) pFrame = 59;
        }

        Math::Rectangle pRect = { pFrame * P_W, 0, P_W, P_H };
        Math::Matrix pMat = Math::Matrix::CreateScale(1.5f) * Math::Matrix::CreateRotationZ(-DirectX::XM_PIDIV2) * Math::Matrix::CreateTranslation(m_parryPlayerPos.x, m_parryPlayerPos.y, 0);

        SHADER.m_spriteShader.SetMatrix(pMat);
        // ★ if(m_isParrying) の外に出すことで、常に表示されるようになります
        SHADER.m_spriteShader.DrawTex(&m_playerParryTex, 0, 0, &pRect);
    }

    // --- 5. パリィエフェクト (加算合成) ---
    // エフェクトは「アニメーション中」かつ「15コマ以内」の時だけ出す
    if (m_isParrying && m_effectAnimTimer < 15.0f) {
        D3D.SetBlendState(BlendMode::Add);

        const int CHIP_SIZE = 80;
        const int COLS = 5;

        int eFrame = static_cast<int>(m_effectAnimTimer);

        int x = (eFrame % COLS) * CHIP_SIZE;
        int y = (eFrame / COLS) * CHIP_SIZE;
        Math::Rectangle srcRect = { x, y, CHIP_SIZE, CHIP_SIZE };

        Math::Matrix eMat = Math::Matrix::CreateScale(2.5f) * Math::Matrix::CreateTranslation(m_parryPlayerPos.x, m_parryPlayerPos.y, 0);

        SHADER.m_spriteShader.SetMatrix(eMat);
        SHADER.m_spriteShader.DrawTex(&m_parryTex, 0, 0, &srcRect);

        D3D.SetBlendState(BlendMode::Alpha);
    }
    // --- Jキーアイコン ---
    // プレイヤーが動き出してから10コマ分だけ「押し」状態にする
    int jDisplayFrame = (m_isParrying && m_playerAnimTimer < 10) ? 1 : 0;
    
    Math::Rectangle jRect = { jDisplayFrame * J_WIDTH, 0, J_WIDTH, J_HEIGHT };
    Math::Matrix jMat = Math::Matrix::CreateScale(3.0f) * Math::Matrix::CreateTranslation(150, -250, 0);
    SHADER.m_spriteShader.SetMatrix(jMat);
    SHADER.m_spriteShader.DrawTex(&m_keyJTex, 0, 0, &jRect);
    
    SHADER.m_spriteShader.SetMatrix(Math::Matrix::Identity);
    
    // --- 7. 固定位置で【回避する】プレイヤーの描画 ---
    {
        const int D_W = 80;
        const int D_H = 80;
    
        // 非回避時は 0コマ目（待機）を表示
        int dFrame = 0;
        if (m_isDodging) {
            dFrame = static_cast<int>(m_dodgeAnimTimer);
            if (dFrame >= 60) dFrame = 59; // ガード
        }
    
        Math::Rectangle dRect = { dFrame * D_W, 0, D_W, D_H };
    
        // パリィの下らへんに配置（座標は適宜微調整してください）
        Math::Matrix dMat = Math::Matrix::CreateScale(1.5f)
            * Math::Matrix::CreateRotationZ(-DirectX::XM_PIDIV2)
            * Math::Matrix::CreateTranslation(400, -10, 0);
    
        SHADER.m_spriteShader.SetMatrix(dMat);
        SHADER.m_spriteShader.DrawTex(&m_playerDodgeTex, 0, 0, &dRect);
    }

    // --- 8. Kキーアイコンの描画 (Jキーの横や下など) ---
    // 回避開始時の一瞬だけ「押しっぱなし」コマ(1)にする
    int kDisplayFrame = (m_isDodging && m_dodgeAnimTimer < 6) ? 1 : 0;
    
    Math::Rectangle kRect = { kDisplayFrame * WASD_WIDTH, 0, WASD_WIDTH, WASD_HEIGHT };
    Math::Matrix kKeyMat = Math::Matrix::CreateScale(3.0f) * Math::Matrix::CreateTranslation(400, -250, 0);
    
    SHADER.m_spriteShader.SetMatrix(kKeyMat);
    SHADER.m_spriteShader.DrawTex(&m_keyKTex, 0, 0, &kRect); // ※回避用キー画像があれば差し替え
    
    SHADER.m_spriteShader.SetMatrix(Math::Matrix::Identity);

    // --- 共通設定：文字を浮かせる高さ ---
    float fontOffsetY = 100.0f;

    // --- A. MOVE (WASDの上) ---
    {
        // WASDのベース位置は -300, -250 なので、その少し上に配置
        Math::Matrix tMat = Math::Matrix::CreateScale(0.45f)
            * Math::Matrix::CreateTranslation(wasdBasePos.x, wasdBasePos.y + fontOffsetY + 25, 0);
        SHADER.m_spriteShader.SetMatrix(tMat);
        SHADER.m_spriteShader.DrawTex(&m_moveFontTex, 0, 0);
    }

    // --- B. REFLECT (Jキーの上) ---
    {
        // Jキーの座標 (150, -250) に合わせる
        Math::Matrix tMat = Math::Matrix::CreateScale(0.45f)
            * Math::Matrix::CreateTranslation(150, -250 + fontOffsetY, 0);
        SHADER.m_spriteShader.SetMatrix(tMat);
        SHADER.m_spriteShader.DrawTex(&m_parryFontTex, 0, 0);
    }

    // --- C. DODGE (Kキーの上) ---
    {
        // Kキーの座標 (400, -250) に合わせる
        Math::Matrix tMat = Math::Matrix::CreateScale(0.45f)
            * Math::Matrix::CreateTranslation(400, -250 + fontOffsetY, 0);
        SHADER.m_spriteShader.SetMatrix(tMat);
        SHADER.m_spriteShader.DrawTex(&m_dodgeFontTex, 0, 0);
    }

    // 最後にマトリックスをリセット
    SHADER.m_spriteShader.SetMatrix(Math::Matrix::Identity);

}