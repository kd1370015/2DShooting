#include "ResultScene.h"
#include "../SceneManager.h"

void ResultScene::Init()
{
	m_tex.Load("Texture/title.png");
	m_enterTex.Load("Texture/start.png"); 

}

void ResultScene::Update()
{

	if (GetAsyncKeyState(VK_RETURN) & 0x8000)
	{
		SceneManager::GetInstance().SetNextScene(SceneManager::SceneType::Title);
	}
}

void ResultScene::DrawSprite()
{

	KdShaderManager::GetInstance().m_spriteShader.DrawTex(&m_tex, 0, 0);
	Math::Color color = { 1,1,1, m_alpha };
	KdShaderManager::GetInstance().m_spriteShader.DrawTex(&m_enterTex, 0, -250, nullptr, &color);

}

void ResultScene::Release()
{}
