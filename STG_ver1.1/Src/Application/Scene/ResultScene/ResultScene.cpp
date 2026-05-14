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
	m_numberTex.Load("Texture/VFX/n3.png"); // 数字フォントテクスチャ（0-9が横に並んでいるもの）
    m_rankTex.Load("Texture/Scene/rank3.png");
	m_resultTex.Load("Texture/Scene/Result.png");


    // 2. ボタンの配置設定
    m_buttons[0] = { { -250, -250 }, { 128, 128 }, &m_retryButtonTex };
    m_buttons[1] = { { 250, -250 }, { 128, 128 }, &m_titleButtonTex };
    //m_buttons[2] = { {   }, { 128, 128 }, &m_selectButtonTex };

    // SceneManagerに預けておいたスコアを受け取る
    m_finalScore = SceneManager::GetInstance().GetFinalScore();

    // 演出用の変数もリセット
    m_displayScore = 0.0f;

    // 各種変数リセット
    m_starAnimeTimer = 0.0f;
    for (int i = 0; i < 3; i++) { m_starScales[i] = 0.0f; m_isStarSoundPlayed[i] = false; }

    // ★ここを追加！ シーンが始まる瞬間に 20フレーム（約0.3秒）の猶予を作る
    m_inputWaitTimer = 20;

    // 前回フレームの入力状態もリセットしておく
    m_prevEnter = false;
    m_prevSpace = false;
}



void ResultScene::Update() {

    // 1. ガードタイマー：シーン遷移直後の誤爆を物理的に防ぐ
    if (m_inputWaitTimer > 0) {
        m_inputWaitTimer--;

        // タイマー動作中は、現在の入力状態を「押されていた」ことにして保存し、
        // タイマー明けの瞬間に「押しっぱなし」と判定されないようにする
        m_prevEnter = (GetAsyncKeyState(VK_RETURN) & 0x8000);
        m_prevSpace = (GetAsyncKeyState(VK_SPACE) & 0x8000);
        return;
    }

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

    // 2. 決定判定：現在の入力を取得
    bool currentEnter = (GetAsyncKeyState(VK_RETURN) & 0x8000);
    bool currentSpace = (GetAsyncKeyState(VK_SPACE) & 0x8000);

    // 3. 決定実行：「前回は離れていて、今は押されている」瞬間だけ反応
    if ((currentEnter && !m_prevEnter) || (currentSpace && !m_prevSpace)) {
        switch (m_hoverIdx) {
        case 0: SceneManager::GetInstance().SetNextScene(SceneManager::SceneType::Game);  break;
        case 1: SceneManager::GetInstance().SetNextScene(SceneManager::SceneType::Title); break;
        case 2: /* ステージセレクト等 */ break;
        }
    }

    // 4. 次のフレームのために現在の状態を保存
    m_prevEnter = currentEnter;
    m_prevSpace = currentSpace;


    // ...
}



void ResultScene::DrawSprite() {
    // 1. 背景
    KdShaderManager::GetInstance().m_spriteShader.DrawTex(&m_tex, 0, 0);

    // 2. 星の描画（ここは変更なし）
    // ...
    // ロゴを少し上に配置（y = 200 あたり）
    Math::Matrix logoMat = Math::Matrix::CreateScale(1.0f, 1.0f, 1.0f) *
        Math::Matrix::CreateTranslation(0, 200.0f, 0);

    KdShaderManager::GetInstance().m_spriteShader.SetMatrix(logoMat);
    // ロゴ全体を表示するので srcRect は nullptr でOK
    KdShaderManager::GetInstance().m_spriteShader.DrawTex(&m_resultTex, 0, 0, nullptr);

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



    // ResultScene.cpp / DrawSprite内
    if (m_displayScore < m_finalScore) {
        float diff = (float)m_finalScore - m_displayScore;

        // fmaxf を使って、差分の15%か、最低でも10.0f増えるようにする
        m_displayScore += fmaxf(diff * 0.15f, 10.0f);

        if (m_displayScore > m_finalScore) m_displayScore = (float)m_finalScore;
    }


    std::string scoreStr = std::to_string((int)m_displayScore);
    int scoreLen = (int)scoreStr.length();


    // 1. スケールに合わせた1文字分の幅を定義
    float fontSize = 56.0f; // 7px * 8.0f = 56px

    // 2. 描画位置の基準を新しい幅で再計算（中央揃え）
    float startX = -((scoreLen * fontSize) / 2.0f) + (fontSize / 2.0f);
    float yPos = -90.0f;

    for (int i = 0; i < scoreLen; i++) {
        int num = scoreStr[i] - '0';

        // 切り抜き範囲は元の画像サイズ（7px）のまま
        Math::Rectangle srcRect = { num * 7, 0, 7, 7 };

        // 行列の設定
        // スケールを 8.0f に、ずらし幅を fontSize (56.0f) に合わせる
        Math::Matrix scoreMat = Math::Matrix::CreateScale(8.0f, 8.0f, 1.0f) *
            Math::Matrix::CreateTranslation(startX + (i * fontSize), yPos, 0);

        KdShaderManager::GetInstance().m_spriteShader.SetMatrix(scoreMat);

        // 描画
        KdShaderManager::GetInstance().m_spriteShader.DrawTex(&m_numberTex, 0, 0, &srcRect);
    }

    {
        // スコア判定（ここは共通）
        int rankIdx = 5;
        if (m_finalScore >= 10000) rankIdx = 0; // S
        else if (m_finalScore >= 8000)  rankIdx = 1; // A
        else if (m_finalScore >= 6000)  rankIdx = 2; // B
        else if (m_finalScore >= 4000)  rankIdx = 3; // C
        else if (m_finalScore >= 2000)  rankIdx = 4; // D

        // --- ここでサイズを直接指定 ---
        // 画像を開いてプロパティを確認し、その数値をここに入れてください
        float texW = 225.0f; // 仮の横幅
        float texH = 42.0f;  // 仮の縦幅

        int chipW = (int)texW / 6;
        int chipH = (int)texH;
        Math::Rectangle srcRect = { rankIdx * chipW, 0, chipW, chipH };

        // 表示位置の設定
        Math::Matrix rankMat = Math::Matrix::CreateScale(2.5f, 2.5f, 1.0f) *
            Math::Matrix::CreateTranslation(0, 65.0f, 0);

        KdShaderManager::GetInstance().m_spriteShader.SetMatrix(rankMat);
        KdShaderManager::GetInstance().m_spriteShader.DrawTex(&m_rankTex, 0, 0, &srcRect);
        KdShaderManager::GetInstance().m_spriteShader.SetMatrix(Math::Matrix::Identity);
    }



    // 行列をリセット
    KdShaderManager::GetInstance().m_spriteShader.SetMatrix(Math::Matrix::Identity);
}

void ResultScene::Release()
{
    // 次のシーン（ゲーム本編など）でカーソルが邪魔にならないよう消しておきます
    while (ShowCursor(FALSE) >= 0);
}
