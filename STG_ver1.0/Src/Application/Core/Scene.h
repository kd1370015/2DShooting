
#pragma once
#include "Application/Player/player.h"
#include <vector>
#include "Application/VFX/bullet.h"



//scene.h

class Scene
{
private:


	KdTexture m_playerTex;
	C_Player m_player;

	std::vector<C_Bullet*> m_bullets; // 管理しやすいようポインタの動的配列に
	KdTexture m_bulletTex;           // 弾のテクスチャ（実体）

	//KdTexture m_enemyTex;
	//C_Enemy m_enemy;

	//KdTexture m_mapTex;
	//C_Map m_map;

	//C_Hit m_hit;

public:

	// 初期設定
	void Init();

	// 解放
	void Release();

	// 更新処理
	void Update();

	// 描画処理
	void Draw2D();

	// 弾を追加するための関数
	void AddBullet(Math::Vector2 pos, float angle);

	// GUI処理
	void ImGuiUpdate();

	C_Player* GetPlayer() { return &m_player; }
	//C_Enemy* GetEnemy() { return &m_enemy; }
	//C_Map* GetMap() { return &m_map; }

private:

	Scene() {}

public:
	static Scene& GetInstance()
	{
		static Scene instance;
		return instance;
	}
};

#define SCENE Scene::GetInstance()

