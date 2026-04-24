
#pragma once
#include "Application/Player/player.h"
#include <vector>
#include "Application/VFX/bullet.h"
#include "Application/VFX/Back.h" // パスは環境に合わせて調整してください
#include "Application/System/mouse.h"
#include "Application/Enemy/Enemy.h"
#include "Application/Enemy/EnemyTurret.h"
#include "Application/System/hit.h"
#include "Application/Item/Orb.h"




//scene.h

class Scene
{
private:


	KdTexture m_playerTex;
	C_Player m_player;

	std::vector<C_Enemy*> m_enemies; // 敵のリスト
	KdTexture m_enemyTex;           // 敵の見た目

	std::vector<C_EnemyTurret*> m_turrets; // 砲台リスト
	KdTexture m_turretTex; // 砲台用のテクスチャ
	

	std::vector<C_Bullet*> m_bullets; // 管理しやすいようポインタの動的配列に
	KdTexture m_bulletTex;           // 弾のテクスチャ（実体）

	std::vector<C_Bullet*> m_enemyBullets; // 敵の弾リスト
	KdTexture m_enemyBulletTex; // ★敵の弾専用に追加

	KdTexture m_crosshairTex; // ★クロスヘア用のテクスチャを追加

	std::vector<C_Orb*> m_orbs;    // オーブのリスト

	KdTexture m_texOrbBlue;
	KdTexture m_texOrbRed;
	KdTexture m_texOrbYellow;

	Math::Vector2 m_mousePos; // ★マウス位置を保持する変数

	C_Mouse m_mouse; // 追加

	C_Back m_back; // 背景クラスの実体
	float m_totalScrollX = 0.0f; // 累積スクロール量

	//KdTexture m_enemyTex;
	//C_Enemy m_enemy;

	//KdTexture m_mapTex;
	//C_Map m_map;

	//C_Hit m_hit;

	int m_spawnTimer = 0;           // 出現間隔用

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
	void AddEnemyBullet(Math::Vector2 pos, float angle);


	// GUI処理
	void ImGuiUpdate();

	C_Player* GetPlayer() { return &m_player; }
	//C_Enemy* GetEnemy() { return &m_enemy; }
	//C_Map* GetMap() { return &m_map; }

	// オーブを追加するための関数
	void AddOrb(Math::Vector2 pos);

	// オーブのリストを取得（判定用）
	std::vector<C_Orb*>& GetOrbs() { return m_orbs; }

	// ... 他の関数 ...
	C_Mouse* GetMouse() { return &m_mouse; } // これが必要

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

