

#include "TitleScene.h"
#include "../SceneManager.h"
#include "Application/Core/main.h"

void TitleScene::Init()
{

    //while (ShowCursor(TRUE) < 0);

    m_tex.Load("Texture/Scene/title_1.png");
    m_enterTex.Load("Texture/Scene/Play.png");
    m_optionTex.Load("Texture/Scene/Option_button.png"); // ★Option用の画像をロード
    m_exitTex.Load("Texture/Scene/Exit_button.png");       // 96px想定

    float buttonY = -250.0f;
    // 中央のPlayを基準に左右に配置
    m_playButton.pos = { 0, buttonY };
    m_optionBtn.pos = { -300, buttonY };
    m_btnExit.pos = { 300, buttonY };

    m_selectIdx = 1; // 最初は中央のPlayを選択状態にする
}






void TitleScene::Update()
{

    if (m_option) {
        m_option->Update();

        // Optionを閉じるフラグが立ったら
        if (m_option->ShouldClose()) {
            m_option.reset();
            // ★重要：閉じた瞬間、キー状態を「押しっぱなし」に上書きして暴発を防ぐ
            m_oldSpaceKey = true;
            m_oldEnterKey = true;
        }
        return; // Optionが開いている間はタイトルの更新はしない
    }

    //m_timer += 0.08f;
    //m_alpha = (sin(m_timer) * 0.3f) + 0.7f;

    // --- 左右キーでの選択切り替え ---
    static bool s_isPushed = false;
    bool isKeyLeft = (GetAsyncKeyState('A') & 0x8000) || (GetAsyncKeyState(VK_LEFT) & 0x8000);
    bool isKeyRight = (GetAsyncKeyState('D') & 0x8000) || (GetAsyncKeyState(VK_RIGHT) & 0x8000);

    if (isKeyLeft || isKeyRight) {
        if (!s_isPushed) {
            if (isKeyRight) {
                // 右を押したらインデックスを増やす (0 -> 1 -> 2)
                m_selectIdx++;
                if (m_selectIdx > 2) m_selectIdx = 0; // 右端まで行ったら左端へ
            }
            if (isKeyLeft) {
                // 左を押したらインデックスを減らす (2 -> 1 -> 0)
                m_selectIdx--;
                if (m_selectIdx < 0) m_selectIdx = 2; // 左端まで行ったら右端へ
            }
            s_isPushed = true;
        }
    }
    else {
        s_isPushed = false;
    }

    // 現在のキー状態を取得
    bool nowSpace = (GetAsyncKeyState(VK_SPACE) & 0x8000) != 0;
    bool nowEnter = (GetAsyncKeyState(VK_RETURN) & 0x8000) != 0;
    if ((nowSpace && !m_oldSpaceKey) || (nowEnter && !m_oldEnterKey))
    {
        switch (m_selectIdx) {
        case 0: // Option
            m_option = std::make_unique<Option>();
            m_option->Init();

            // ★ここでOption側の「前回の状態」を無理やり true にする！
            // これにより、Option側の最初のUpdateで「!m_oldSpaceKey」が false になり、
            // 1回指を離すまで閉じなくなります。
            m_option->SetOldKeys(true);
        break;
        case 1: // Play
            SceneManager::GetInstance().SetNextScene(SceneManager::SceneType::Game);
            break;
        case 2: // Exit
            APP.End();
            break;
        }
    }

    // 次のフレームのために状態を保存
    m_oldSpaceKey = nowSpace;
    m_oldEnterKey = nowEnter;

}


//void TitleScene::DrawSprite()
//{
//    SHADER.m_spriteShader.DrawTex(&m_tex, 0, 0);
//
//    // ボタン情報を配列風に回すとスッキリ書けます
//    TitleButton* btns[] = { &m_optionBtn, &m_playButton, &m_btnExit };
//    KdTexture* texs[] = { &m_optionTex, &m_enterTex, &m_exitTex };
//
//    for (int i = 0; i < 3; ++i) {
//        bool isHover = (m_selectIdx == i);
//        float scale = isHover ? 1.2f : 1.0f;
//
//        // Playボタン(i=1)は元々大きいので、さらに強調
//        if (i == 1 && !isHover) scale = 1.1f;
//
//        Math::Color color = isHover ? Math::Color{ 1, 1, 1, m_alpha } : Math::Color{ 0.6f, 0.6f, 0.6f, 1.0f };
//
//        Math::Matrix mat = Math::Matrix::CreateScale(scale) * Math::Matrix::CreateTranslation(btns[i]->pos.x, btns[i]->pos.y, 0);
//        SHADER.m_spriteShader.SetMatrix(mat);
//        SHADER.m_spriteShader.DrawTex(texs[i], 0, 0, nullptr, &color);
//    }
//
//    SHADER.m_spriteShader.SetMatrix(Math::Matrix::Identity);
//
//    if (m_option) m_option->Draw();
//}

void TitleScene::DrawSprite()
{
    // 背景描画
    KdShaderManager::GetInstance().m_spriteShader.DrawTex(&m_tex, 0, 0);

    // ボタンの情報を配列で管理
    TitleButton* btns[] = { &m_optionBtn, &m_playButton, &m_btnExit };
    KdTexture* texs[] = { &m_optionTex, &m_enterTex, &m_exitTex };

    for (int i = 0; i < 3; ++i)
    {
        bool isSelected = (m_selectIdx == i);

        // --- 1. 色と明るさの設定 ---
        Math::Color color;
        if (isSelected) {
            // 選択中：真っ白（1.0）で表示。
            // もし「もっと光らせたい」場合は、加算合成を使うか、
            // もともとの画像より明るい色（2.0fなど）を指定します。
            color = { 1.5f, 1.5f, 1.5f, 1.0f };
        }
        else {
            // 非選択：半分くらいの明るさ（グレー）にして沈ませる
            color = { 0.4f, 0.4f, 0.4f, 1.0f };
        }

        // --- 2. 行列の設定 ---
        float scale = isSelected ? 1.2f : 1.0f;
        Math::Matrix mat = Math::Matrix::CreateScale(scale) * Math::Matrix::CreateTranslation(btns[i]->pos.x, btns[i]->pos.y, 0);

        KdShaderManager::GetInstance().m_spriteShader.SetMatrix(mat);

        // --- 3. 描画（加算合成で光らせる演出） ---
        if (isSelected) {
            // 選択中だけ「加算合成」に切り替えると、背後の色と混ざって「発光」して見えます
            D3D.SetBlendState(BlendMode::Add);
        }

        KdShaderManager::GetInstance().m_spriteShader.DrawTex(texs[i], 0, 0, nullptr, &color);

        // 常にアルファブレンド（通常）に戻しておく
        D3D.SetBlendState(BlendMode::Alpha);
    }

    // 行列リセット
    KdShaderManager::GetInstance().m_spriteShader.SetMatrix(Math::Matrix::Identity);

    if (m_option) m_option->Draw();
}

void TitleScene::Release()
{
    // 次のシーン（ゲーム本編など）でカーソルが邪魔にならないよう消しておきます
    while (ShowCursor(FALSE) >= 0);

}
