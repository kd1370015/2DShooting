#pragma once

//クラスを継承するときのみインクルード許可！
#include <vector>
#include <memory> // std::shared_ptr を使うために必要
#include "../BaseScene/BaseScene.h"
#include "Application/Player/player.h"
#include "Application/Enemy/EnemyManager.h"
#include "Application/Enemy/BossBullet.h"
#include "Application/Enemy\boss.h"
#include "Application/VFX/Back.h"
#include "Application/VFX/explosion.h"
#include "Application/VFX/Fexpl.h"
#include "Application/System/mouse.h"
#include "Application/System/Score.h"
#include "Application/Scene/SceneManager.h"

// 前方宣言（Effect.hをここでインクルードせず、存在だけ教える）
class C_Effect;
class C_Charge; // 追加
class LevelUp; // ★追加：LevelUpエフェクト用のクラスも前方宣言しておく

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
	// ボスの弾を追加するための関数
	void AddBossBullet(Math::Vector2 pos, float angle);
	//void AddEnemyBullet(Math::Vector2 pos, float angle, bool isBossBullet);
	void AddOrb(Math::Vector2 pos);
	void AddEffect(Math::Vector2 pos, float scale); // ★追加：エフェクトを追加する関数
	// 他の Add... 関数の並びに追加
	void AddExplosion(Math::Vector2 pos);
	void AddFExplosion(Math::Vector2 pos);
	// ... 既存の関数 ...

	void AddLevelUpEffect(); // 引数なしに変更

	// ボスのHPバー描画などで使用
	float GetBossHpRate();

	// ★追加：敵の弾リストを外部（Hitクラス等）から参照できるようにする
	std::vector<C_Bullet*>& GetEnemyBullets() { return m_enemyBullets; }

	// ★追加：自機の情報を取得する（当たり判定で使用するため）
	C_Player& GetPlayer() { return m_player; }
	// パリィ用テクスチャをPlayerに渡すための関数
	KdTexture* GetParryTex() { return &m_parryTex; }
	// ★追加：エフェクト用のテクスチャ取得関数
	KdTexture* GetBarrierTex() { return &m_barrierTex; }

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
	// GameScene.h
	std::vector<C_BossBullet*> m_bossBullets;
	std::vector<C_Orb*> m_orbs;
	// ★追加：エフェクトのリスト
	std::vector<std::shared_ptr<C_Effect>> m_effects;

	KdTexture m_playerTex, m_hpTex, m_enemyTex, m_turretTex;
	KdTexture m_bulletTex, m_enemyBulletTex, m_eFollowBulletTex;

	KdTexture m_hpGaugeTex;    // HPの中身（緑や赤のバー）
	KdTexture m_hpFrameTex;    // HPの外枠

	std::shared_ptr<C_Boss> m_boss = nullptr; // ポインタ管理の場合

	KdTexture m_bosshpTex;
	KdTexture m_bosshpFrameTex;
	KdTexture m_bossTex;


	const int HP_GAUGE_FULL_W = 272; // 画像の本来の横幅
	const int HP_GAUGE_H = 21; // 画像の本来の縦幅	

	KdTexture m_playerIconTex; // プレイヤーの顔アイコン

	KdTexture m_bossBulletTex;   // ★ボスの弾用に追加！
	KdTexture m_texOrbBlue, m_texOrbRed, m_texOrbYellow;
	KdTexture m_parryTex; // パリィ用の円画像
	KdTexture m_dodgeTex; // ★追加
	KdTexture m_parryAnimTex; // ★追加
	KdTexture m_turboTex; // ★追加


	// ★追加：バリアアニメーション用のテクスチャ
	KdTexture m_barrierTex;

	// ★追加：チャージエフェクトの実体（ポインタで持つのが楽です）
	std::shared_ptr<C_Charge> m_chargeEffect;

	// ★追加：チャージ用のテクスチャ
	KdTexture m_chargeTex;
	KdTexture m_chargeIconTex; // アイコン用のテクスチャを追加
	KdTexture m_chargeFrameTex; // チャージゲージの枠用テクスチャ
	KdTexture m_chargeGaugeTex; // チャージゲージの中身用テクスチャ
	KdTexture m_chargeLowTex;


	// 爆発管理用リスト
	std::vector<std::shared_ptr<C_Explosion>> m_explosions;
	// GameScene.h 内での定義
	std::list<std::shared_ptr<F_Explosion>> m_explList;
	// 爆発テクスチャ
	KdTexture* m_texExplosion;
	KdTexture* m_texFExplosion;


	// --- private 内に追加 ---
	bool m_bossSpawned = false;    // ボスが既に出現したか
	bool m_isBossBattle = false;  // 現在ボス戦中か

	float m_totalScrollX = 0.0f;
	int m_spawnTimer = 0;

	bool m_spawnUpper = true; // クラスのメンバ変数として追加

	float m_timeScale = 1.0f;     // 時間の倍率（1.0が通常、0.2がスロー）
	int   m_slowTimer = 0;        // スローが続く時間

	// 砲撃型(Turret)用
	int m_turretSpawnTimer = 100;
	const int TURRET_INTERVAL = 200; // 砲撃型はゆっくり

	// 追尾型(Follower)用
	int m_followerSpawnTimer = 50;
	const int FOLLOWER_INTERVAL = 120; // 追尾型は頻繁に

	// ★追加：前フレームのチャージ量を保存する変数
	float m_prevCharge = 0.0f;

	// ★追加：LevelUpエフェクトの管理用
	std::vector<std::shared_ptr<LevelUp>> m_levelUpEffects;
	KdTexture m_levelUpTex;

	// チャージマックスを1回だけ検知するためのフラグ
	bool m_wasChargeMax = false;

	int  m_waveStep = 0;          // 0:上列, 1:下列, 2:ボウリング, 3:ボス
	bool m_isBossSpawned = false; // ボスが出たかどうか

};