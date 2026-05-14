#pragma once


class Score
{
public:
	Score() { Init(); }
	~Score() {}

	void Init()			;
	void Update()		;
	void DrawSprite()	;
	void Release()		;

	// スコアを加算する関数を追加
	void AddScore(int amount)
	{
		m_score += amount;

		// カンスト（最大値）チェック
		if (m_score > 9999999)
		{
			m_score = 9999999;
		}
	}

	// 現在のスコアを取得する関数を追加
	unsigned long GetScore() const { return m_score; }


private:
	static const int maxDigits = 7; // 最大桁数

	KdTexture m_tex;
	unsigned long m_score = 0;
	int m_digits[maxDigits] = {}; // 各桁の値を格納する配列

};