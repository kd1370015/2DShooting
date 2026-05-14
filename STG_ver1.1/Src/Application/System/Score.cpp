#include "Score.h"


void Score::Init()
{
	m_tex.Load("Texture/VFX/n3.png");
}

void Score::Update()
{
	if (GetAsyncKeyState(VK_UP) & 0x8000)
	{
		m_score += 100;
		if (m_score > 9999999)
		{
			m_score = 9999999;
		}
	}


	//配列に各桁数の数値を格納
	unsigned long tmp = m_score;
	for (int i = maxDigits - 1; i >= 0; --i)
	{
		//下位の桁から抽出し、配列に格納
		m_digits[i] = tmp % 10;
		tmp /= 10;
	}

}


void Score::DrawSprite()
{
    // --- 1. 行列をリセットし、スコア用の位置を設定 ---
    // posX, posY は表示したい画面上の座標に調整してください
    float posX = 400.0f;
    float posY = 320.0f;

    // スコア専用の行列を作成（以前の描画の影響を断ち切る）
    Math::Matrix mat = Math::Matrix::CreateTranslation(posX, posY, 0);
    KdShaderManager::GetInstance().m_spriteShader.SetMatrix(mat);

    // --- 2. スコアの各桁を描画 ---
    for (int i = 0; i < maxDigits; ++i)
    {
        // 数字1つ分の切り抜き（画像に合わせて調整してください）
        Math::Rectangle rc = { 7 * m_digits[i], 0, 7, 7 };

        // DrawTexの引数を整理（posX + i * 35 は「行列」ではなくここでずらす）
        // 第2引数、第3引数は「セットした行列(posX, posY)からの相対座標」になります
        KdShaderManager::GetInstance().m_spriteShader.DrawTex(&m_tex, i * 35, 0, 35, 35, &rc);
    }

    // --- 3. 最後にリセット（お作法） ---
    KdShaderManager::GetInstance().m_spriteShader.SetMatrix(Math::Matrix::Identity);
}

void Score::Release()
{
}
