#include "TitleScene.h"
#include "../SceneManager.h"

void TitleScene::Init()
{
	m_tex.Load("Texture/Scene/title.png");
	m_enterTex.Load("Texture/Scene/start.png"); 

}

void TitleScene::Update()
{
	m_timer += 0.01f;
	m_alpha = (sin(m_timer) * 1.0f) * 0.5f;

	if (GetAsyncKeyState(VK_RETURN) & 0x8000)
	{
		SceneManager::GetInstance().SetNextScene(SceneManager::SceneType::Game);
	}
}

void TitleScene::DrawSprite()
{

	KdShaderManager::GetInstance().m_spriteShader.DrawTex(&m_tex, 0, 0);
	Math::Color color = { 1,1,1, m_alpha };
	KdShaderManager::GetInstance().m_spriteShader.DrawTex(&m_enterTex, 0, -250, nullptr, &color);

}

void TitleScene::Release()
{}
