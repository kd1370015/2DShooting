#pragma once
#include <vector>
#include "Application/VFX/bullet.h"
#include "Application/Item/Orb.h"  // これを追加！


class GameScene;

class C_Player
{
public:
	C_Player() {}
	~C_Player() {}


	void Init();

	void Action();



	// 更新処理
	void Update();

	// 描画処理
	void Draw();


	void SetTex(KdTexture* tex) { m_tex = tex; }
	bool IsAlive() const { return m_alive; }

	void SetAlive(bool alive) { m_alive = alive; }
	void SetOwner(GameScene* owner) { m_owner = owner; }

	float GetScrollX() { return m_scrollX; }

	Math::Vector2 GetPos() { return m_pos; }
	Math::Vector2 GetFuturePos() { return m_pos + m_move; }
	float GetRadius() { return 32.0f; }

	// 移動量を取得する関数を追加
	Math::Vector2 GetMove() const { return m_move; }

	std::vector<C_Bullet> m_bullets;
	KdTexture* m_bulletTex; // 弾のテクスチャ

	void Upgrade(OrbType type);

	// 宣言だけに書き換える
	void DecreaseHp(int damage);

	int GetHp() const { return m_hp; }
	void SetHp(int hp) { m_hp = hp; }

	// ★追加：パリィの状態を取得する
	bool IsParrying() const { return m_parryTimer > 0; }
	float GetParryRadius() const { return m_parryRadius; }
	// ★追加：計算済みのマウス角度を返す関数
	float GetAngle() const { return m_angle; }

	// ★追加：エネルギーをチャージする（外部の弾などから呼ばれる想定）
	void ChargeEnergy(float amount);

	void Heal(int amount) {
		m_hp += amount;
		if (m_hp > 5) m_hp = 5; // 最大HPを超えないように固定
	}

	bool CanHeal() const { return m_canHeal; }
	void SetHealDone() { m_canHeal = false; }
	int GetShotCount() { return m_shotCount; }

	bool IsDodging() const { return m_dodgeTimer > 0; }
	// Player.h 内に追加
	bool CanShot() const { return m_canShot; }
	void SetShotDone() { m_canShot = false; }

	bool CanDodgeCharge() const { return m_canDodgeCharge; }
	void SetDodgeChargeDone() { m_canDodgeCharge = false; }

	// ★追加：現在のエネルギー量を返す関数
	float GetEnergy() const { return m_energy; }
	// チャージ量（エネルギー）を取得する関数
	float GetCharge() const { return m_energy; }

	// ★追加：現在エネルギーを消費中（カウントダウン中）かどうかを返す
	bool IsUsingPower() const { return m_isUsingPower; }

	// 必要であれば、消費モードを開始する関数
	void StartConsume() {
		if (m_energy >= 100.0f) {
			m_isUsingPower = true;
			m_consumeTimer = 0;
		}
	}

	// ★これを追加：回避用テクスチャをセットする
	void SetDodgeTex(KdTexture* tex) { m_dodgeTex = tex; }
	// パリィ用テクスチャをセットする関数
	void SetParryAnimTex(KdTexture* tex) { m_parryAnimTex = tex; }


private:

	GameScene* m_owner;

	float m_scaleX;		//横拡大行列
	float m_scaleY;		//縦拡大行列
	float m_scrollX;
	float m_scrollMin;
	float m_scrollMax;

	int m_hp = 5;       // 現在のHP
	int m_maxHp = 5;    // 最大HP
	int m_invincibleTimer = 0; // 無敵タイマー

	float INVINCIBLE_TIME = 60.0f;

	const float MovePow = 3.0f;				//移動力


	KdTexture* m_tex;
	Math::Vector2 m_pos;
	Math::Vector2 m_move;
	Math::Matrix m_scaleMat;		//拡大行列
	Math::Matrix m_transMat;		//移動行列
	Math::Matrix m_mat;				//合成行列

	KdTexture* m_dodgeTex = nullptr; // ★追加：回避用テクスチャ(SF01a_strip60.png用)
	// ... 既存の変数 ...
	KdTexture* m_parryAnimTex = nullptr; // ★追加：SF01b_strip60.png用


	KdTexture m_turboTex[4]; // 4枚分のテクスチャ配列
	float     m_turboFrame;  // アニメーション用フレーム

	// ★追加：ターボ（炎）アニメーション用変数


	bool m_alive;		//生存フラグ

	float m_angle; // ★これを追加：マウスへの角度を保存する用


	int   m_shotCount = 3;     // 一度に発射する弾の数（初期値1）
	float m_spreadAngle = 15.0f; // 弾と弾の間の角度（度数法）
	float m_bulletSpeed = 1.0f; // 初期弾速（Bulletに渡す用）

	int m_shootInterval = 50; // 初期値（小さいほど速い）
	int m_shootTimer = 0;    // タイマー管理用


	static const int ScrWidth = 1280;
	static const int ScrHeight = 720;


	// ★追加：パリィ（リフレクト）用
	int   m_parryTimer = 0;       // パリィの持続時間
	float m_parryRadius = 96.0f; // パリィ判定の円の半径
	const int PARRY_DURATION = 20; // 1回押した時の有効フレーム数(約0.3秒)
	const int PARRY_COOLDOWN = 60; // 次に撃てるまでの待ち時間
	int   m_parryCooldownTimer = 0;

	// ★追加：エネルギー・強化システム用
	bool  m_isUsingPower = false; // 消費中フラグ
	int   m_consumeTimer = 0;     // 減らすタイミングを測るタイマー
	float m_energy = 0.0f;        // 現在の蓄積エネルギー

	const float MAX_ENERGY = 100.0f;
	bool  m_isPoweredUp = false;  // 強化状態フラグ
	int   m_powerUpTimer = 0;     // 強化持続時間

	int m_dodgeTimer = 0;         // 回避中の残り時間
	int m_dodgeCooldownTimer = 0; // 次に回避できるまでの待ち時間
	bool m_oldDodgeKey = false;    // 前フレームの左クリック状態
	bool m_oldParryKey = false; // 旧 m_oldRButton

	// かすり判定（エネルギー吸収範囲）
	float m_grazeRadius = 80.0f;


	bool m_oldRButton = false; // 前回の右クリック状態

	bool m_canHeal = false; // ★追加：このパリィで回復が可能か
	bool m_canShot = false; // 今回のパリィで反射弾を撃ったか
	bool m_canDodgeCharge = false; // 今回の回避でチャージが可能か

	// ★回避アニメーション用の定数
	const int  DODGE_CHIP_W = 80;   // 1コマの幅（画像に合わせて調整してください）
	const int  DODGE_CHIP_H = 80;   // 1コマの高さ
	const int  DODGE_STEP_MAX = 60;   // 全コマ数（ファイル名がstrip60なので60と想定）

	// パリィアニメーション定数
	const int PARRY_CHIP_W = 80;   // 1コマの幅
	const int PARRY_CHIP_H = 80;   // 1コマの高さ
	const int PARRY_STEP_MAX = 60; // 全コマ数

	// ★追加：ターボ画像の定数 (★画像の実際の1コマ分を測って入力)
	const int  TURBO_CHIP_W = 128;    // 1コマの横幅
	const int  TURBO_CHIP_H = 64;    // 1コマの縦幅
	const int  TURBO_STEP_MAX = 7;     // 全コマ数
	const int  TURBO_ANIM_SPEED = 2;   // アニメーション速度（小さいほど速い）


};