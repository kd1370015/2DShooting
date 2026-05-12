#pragma once

//すべてのシーンの親クラス


class BaseScene
{
public:
	BaseScene() {}
	virtual ~BaseScene(){}

	//virtual・・・仮想関数・・・上書きを前提とする関数
	virtual void Init();
	virtual void Update();
	virtual void DrawSprite();
	virtual void Release();

private:


};