#include "Mouse.h"

// もしどこにも定義がないなら、ここで定義してしまう
static const float ScrWidth = 1280.0f;
static const float ScrHeight = 720.0f;


void C_Mouse::Init() {
    m_tex.Load("Texture/VFX/crosshair.png"); // クロスヘア画像のパス
    m_pos = { 0, 0 };

    // Windowsのカーソルを非表示にする（お好みで）
    ShowCursor(FALSE); 
}

void C_Mouse::Update() {
    POINT pt;
    GetCursorPos(&pt);
    ScreenToClient(APP.m_window.GetWndHandle(), &pt);


    m_pos.x = (float)pt.x - (ScrWidth / 2.0f);
    m_pos.y = (float)pt.y - (ScrHeight / 2.0f);
    m_pos.y *= -1; // 上をプラスにする
}

void C_Mouse::Draw() {
    Math::Matrix mat = Math::Matrix::CreateTranslation(m_pos.x, m_pos.y, 0);
    SHADER.m_spriteShader.SetMatrix(mat);

    // 画像サイズに合わせて調整（例：64x64）
    SHADER.m_spriteShader.DrawTex(&m_tex, Math::Rectangle(0, 0, 64, 64), 1.0f);
}