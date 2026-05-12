#pragma once

//クラスを継承するときのみインクルード許可！
#include "../BaseScene/BaseScene.h"
#include "Application/Player/player.h"
#include "Application/Enemy/EnemyManager.h"
#include "Application/VFX/Back.h"
#include "Application/System/mouse.h"
#include "Application/System/Score.h"

class GameScene : public BaseScene
{
public:
	GameScene() { Init(); }
	~GameScene(){}

	void Init()			override;
	void Update()		override;
	void DrawSprite()	override;
	void Release()		override;

	// 外部から呼ばれる追加関数（弾の追加など）
	void AddBullet(Math::Vector2 pos, float angle);
	void AddEnemyBullet(Math::Vector2 pos, float angle);
	void AddOrb(Math::Vector2 pos);

	// マウス取得用の関数（これがないとエラーになります）
	C_Mouse* GetMouse() { return &m_mouse; }
	Score& GetScore() { return m_score; }

private:

	// --- Scene.h から引越してきた変数たち ---
	C_Player m_player;
	C_EnemyManager m_enemyManager;
	C_Back m_back;
	C_Mouse m_mouse;
	Score m_score; // Scoreクラスとして持っておくのが綺麗です

	std::vector<C_Bullet*> m_bullets;
	std::vector<C_Bullet*> m_enemyBullets;
	std::vector<C_Orb*> m_orbs;

	KdTexture m_playerTex, m_hpTex, m_enemyTex, m_turretTex;
	KdTexture m_bulletTex, m_enemyBulletTex;
	KdTexture m_texOrbBlue, m_texOrbRed, m_texOrbYellow;

	float m_totalScrollX = 0.0f;
	int m_spawnTimer = 0;


};