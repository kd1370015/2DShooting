#pragma once
#include <vector>
#include "Application/VFX/bullet.h"
#include "Application/Item/Orb.h"  // これを追加！


class Scene; //前方宣言

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

	void MapHitX(float posX, float moveX);
	//void MapHitY(float posY, float moveY, bool Jump);

	void MapHitY(float newY, float velocityY, bool isBottomHit);


	void SetTex(KdTexture* tex) { m_tex = tex; }
	bool IsAlive() const { return m_alive; }

	void SetAlive(bool alive) { m_alive = alive; }
	void SetOwner(Scene* owner) { m_owner = owner; }

	float GetScrollX() { return m_scrollX; }

	Math::Vector2 GetPos() { return m_pos; }
	Math::Vector2 GetFuturePos() { return m_pos + m_move; }
	float GetRadius() { return 32.0f; }

	// 移動量を取得する関数を追加
	Math::Vector2 GetMove() const { return m_move; }

	std::vector<C_Bullet> m_bullets;
	KdTexture* m_bulletTex; // 弾のテクスチャ

	void Upgrade(OrbType type);


private:

	Scene* m_owner;

	float m_scaleX;		//横拡大行列
	float m_scaleY;		//縦拡大行列
	float m_scrollX;
	float m_scrollMin;
	float m_scrollMax;


	const float MovePow = 5.0f;				//移動力


	KdTexture* m_tex;
	Math::Vector2 m_pos;
	Math::Vector2 m_move;
	Math::Matrix m_scaleMat;		//拡大行列
	Math::Matrix m_transMat;		//移動行列
	Math::Matrix m_mat;				//合成行列


	bool m_alive;		//生存フラグ

	float m_angle; // ★これを追加：マウスへの角度を保存する用


	int   m_shotCount = 1;     // 一度に発射する弾の数（初期値1）
	float m_spreadAngle = 15.0f; // 弾と弾の間の角度（度数法）
	float m_bulletSpeed = 10.0f; // 初期弾速（Bulletに渡す用）

	int m_shootInterval = 10; // 初期値（小さいほど速い）
	int m_shootTimer = 0;    // タイマー管理用


	static const int ScrWidth = 1280;
	static const int ScrHeight = 720;





};