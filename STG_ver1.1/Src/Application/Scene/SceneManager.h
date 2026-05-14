#pragma once

class BaseScene;

class SceneManager
{
public:

	//シーン情報
	//enum列挙型・・・int型の羅列
	enum SceneType
	{
		Title,
		Game,
		Result,
		Option,
	};

	void PreUpdate();
	void Update();
	void Draw();

	//次のシーンを予約する
	void SetNextScene(SceneType _nextScene)
	{
		m_nextSceneType = _nextScene;
	}

	//現在のシーンを取得する
	SceneType GetCurrentSceneType() const
	{
		return m_currentSceneType;
	}

	// SceneManagerに現在のシーンを返す関数を作る
	// 現在のシーンを取得する関数を追加
	std::shared_ptr<BaseScene> GetCurrentScene() const { return m_currentScene; }
	
	// スコアをセットする関数
	void SetFinalScore(int _score) { m_finalScore = _score; }
	// スコアを取得する関数
	int GetFinalScore() const { return m_finalScore; }

private:

	//保守性を高めるために、他で呼べないようにする
	void Init();
	void Release();
	void ChangeScene(SceneType _sceneType);

	//変数
	std::shared_ptr<BaseScene>m_currentScene;		//源氏のシーンを管理しているポインタ

	SceneType m_currentSceneType = SceneType::Title;		//現在のシーンの種類

	SceneType m_nextSceneType = m_currentSceneType; 		//現在のシーンの種類

	int m_finalScore = 0; // スコア保存用

	//シングルトンパターン
	//実態が１つしかないことを保障するデザインパターン
private:
	SceneManager() { Init(); }
	~SceneManager() { Release(); }
public:
	static SceneManager& GetInstance()
	{
		static SceneManager instance;
		return instance;
	}


};