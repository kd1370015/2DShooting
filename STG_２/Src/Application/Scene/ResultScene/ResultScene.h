#pragma once

//クラスを継承するときのみインクルード許可！
#include "../BaseScene/BaseScene.h"

class ResultScene : public BaseScene
{
public:
	ResultScene() { Init(); }
	~ResultScene(){}

	void Init()			override;
	void Update()		override;
	void DrawSprite()	override;
	void Release()		override;

private:
	KdTexture m_tex;
	KdTexture m_enterTex;

	float m_alpha = 0.0f;
	float m_timer = 0.0f;

};