#include "SceneManager.h"
#include "BaseScene/BaseScene.h"	
#include "TitleScene/TitleScene.h"
#include "GameScene/GameScene.h"
#include "ResultScene/ResultScene.h"

void SceneManager::PreUpdate()
{
	if (m_currentSceneType != m_nextSceneType)
	{
		ChangeScene(m_nextSceneType);
	}
}

void SceneManager::Update()
{
	if (m_currentScene)
	{
		m_currentScene->Update();
	}
}

void SceneManager::Draw()
{
	if (m_currentScene)
	{
		m_currentScene->DrawSprite();
	}
}

void SceneManager::Init()
{
	ChangeScene(m_currentSceneType);
}

void SceneManager::Release()
{
	m_currentScene.reset();
}

void SceneManager::ChangeScene(SceneType _sceneType)
{
	//①次のシーンを作成し、②シーン管理フラグ更新
	// 現在のシーンを解放
	if (m_currentScene)
	{
		m_currentScene->Release();
		m_currentScene.reset();
	}

	switch (_sceneType)
	{
	case SceneType::Title:
		m_currentScene = std::make_shared<TitleScene>();
		break;
	case SceneType::Game:
		m_currentScene = std::make_shared<GameScene>();
		break;
	case SceneType::Result:
		m_currentScene = std::make_shared<ResultScene>();
		break;

	}

	// 初期化
	if (m_currentScene)
	{
		m_currentScene->Init();
	}

	//Init関数を書くならここ！
	//m_currentScene->Init();

// 現在のシーン情報を更新
	m_currentSceneType = _sceneType;
	m_nextSceneType = _sceneType;
}
