#include "ResultScene.h"
#include "../SceneManager.h"

void ResultScene::Init()
{

    while (ShowCursor(TRUE) < 0);

    // 1. テクスチャ読み込み
    m_tex.Load("Texture/Scene/Result_back.png"); // 背景
    m_starTex.Load("Texture/Scene/Star.png");
    m_emptyStarTex.Load("Texture/Scene/NoStar.png");
    m_retryButtonTex.Load("Texture/Scene/Retry.png");
    m_titleButtonTex.Load("Texture/Scene/Home.png");
    m_selectButtonTex.Load("Texture/UI/Button_Select.png");

    // 2. ボタンの配置設定
    m_buttons[0] = { { -400, -250 }, { 128, 128 }, &m_retryButtonTex };
    m_buttons[1] = { {    0, -250 }, { 128, 128 }, &m_titleButtonTex };
    m_buttons[2] = { {  400, -250 }, { 128, 128 }, &m_selectButtonTex };

    // 各種変数リセット
    m_starAnimeTimer = 0.0f;
    for (int i = 0; i < 3; i++) { m_starScales[i] = 0.0f; m_isStarSoundPlayed[i] = false; }
}



void ResultScene::Update() {
    // --- ADキー または 左右キー で選択切り替え ---
    static bool s_isPushed = false;
    bool isKeyLeft = (GetAsyncKeyState('A') & 0x8000) || (GetAsyncKeyState(VK_LEFT) & 0x8000);
    bool isKeyRight = (GetAsyncKeyState('D') & 0x8000) || (GetAsyncKeyState(VK_RIGHT) & 0x8000);

    if (isKeyLeft || isKeyRight) {
        if (!s_isPushed) {
            if (isKeyLeft)  m_hoverIdx--;
            if (isKeyRight) m_hoverIdx++;

            // インデックスが 0～2 の範囲に収まるようにループ・制限させる
            if (m_hoverIdx < 0) m_hoverIdx = 2;
            if (m_hoverIdx > 2) m_hoverIdx = 0;

            // ここでカーソル移動SEを鳴らすとGOOD
            // SCENE.GetSound()->CursorSE(); 

            s_isPushed = true;
        }
    }
    else {
        s_isPushed = false;
    }

    // --- 決定実行 (ENTER または SPACE) ---
    if ((GetAsyncKeyState(VK_RETURN) & 0x8000) || (GetAsyncKeyState(VK_SPACE) & 0x8000)) {
        // SCENE.GetSound()->SelectSE(); // 決定音
        switch (m_hoverIdx) {
        case 0: SceneManager::GetInstance().SetNextScene(SceneManager::SceneType::Game); break;
        case 1: SceneManager::GetInstance().SetNextScene(SceneManager::SceneType::Title); break;
        case 2: /* ステージセレクト等があれば */ break;
        }
    }

    // 星のアニメーション更新（ここはそのまま）
    m_starAnimeTimer += 1.0f;
    // ...
}



void ResultScene::DrawSprite() {
    // 1. 背景
    KdShaderManager::GetInstance().m_spriteShader.DrawTex(&m_tex, 0, 0);

    // 2. 星の描画（ここは変更なし）
    // ...
     // 2. 星の描画
    bool results[] = { m_isIronComplete, m_isNoDamage, m_isTimeClear };
    for (int i = 0; i < 3; i++) {
        float s = m_starScales[i];
        Math::Matrix starMat = Math::Matrix::CreateScale(s, s, 1.0f) * Math::Matrix::CreateTranslation(-200.0f + (i * 200.0f), 50.0f, 0);

        KdShaderManager::GetInstance().m_spriteShader.SetMatrix(starMat);
        KdTexture* pTex = results[i] ? &m_starTex : &m_emptyStarTex;
        KdShaderManager::GetInstance().m_spriteShader.DrawTex(pTex, 0, 0, 96, 96);
    }


    // 3. ボタンの描画
    for (int i = 0; i < 3; i++) {
        // 選択中（m_hoverIdx == i）なら少し大きく
        float scale = (m_hoverIdx == i) ? 1.2f : 1.0f;

        // 選択中なら白くハッキリ、非選択なら少し暗く（または半透明）すると分かりやすい
        Math::Color color = (m_hoverIdx == i) ? Math::Color{ 1, 1, 1, 1 } : Math::Color{ 0.6f, 0.6f, 0.6f, 1.0f };

        Math::Matrix btnMat = Math::Matrix::CreateScale(scale, scale, 1.0f) * Math::Matrix::CreateTranslation(m_buttons[i].pos.x, m_buttons[i].pos.y, 0);

        KdShaderManager::GetInstance().m_spriteShader.SetMatrix(btnMat);

        // 重要：行列で座標指定しているので、DrawTexの引数は 0, 0 にする
        KdShaderManager::GetInstance().m_spriteShader.DrawTex(m_buttons[i].normalTex, 0, 0, 128, 128, nullptr, &color);
    }

    // 行列をリセット
    KdShaderManager::GetInstance().m_spriteShader.SetMatrix(Math::Matrix::Identity);
}

void ResultScene::Release()
{
    // 次のシーン（ゲーム本編など）でカーソルが邪魔にならないよう消しておきます
    while (ShowCursor(FALSE) >= 0);
}
