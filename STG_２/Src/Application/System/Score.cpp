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

	//スコア表示
	for (int i = 0; i < maxDigits; ++i)
	{
		Math::Rectangle rc = { 7 * m_digits[i], 0, 7, 7 };

		float posX = 475;

		//桁別に表示(サイズを10倍にしています)
		KdShaderManager::GetInstance().m_spriteShader.DrawTex(&m_tex, posX + i * 35, 0, 35, 35, &rc);
	}

}

void Score::Release()
{
}
