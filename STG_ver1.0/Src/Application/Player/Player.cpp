#include "Player.h"
#include "Application/Core/Scene.h"
#include <Application/Core/main.h>

//player.cpp

void C_Player::Draw()
{
	if (!m_alive) return;

	SHADER.m_spriteShader.SetMatrix(m_mat);
	SHADER.m_spriteShader.DrawTex(m_tex, Math::Rectangle(0, 0, 64, 64), 1.0f);


}

//void C_Player::MapHitX(float posX, float moveX)
//{
//	m_pos.x = posX;
//	m_move.x = moveX;
//}
//
//
//void C_Player::MapHitY(float posY, float moveY, bool Jump)
//{
//	m_pos.y = posY;
//	m_move.y = moveY;
//	m_jump = Jump;
//}




void C_Player::Update()
{
	// 座標確定処理
	m_pos += m_move;

	// --- マウスの方向を向く処理 (C_Mouseから座標をもらう) ---
	// ※Sceneクラスに GetMouse() が実装されている前提です
	Math::Vector2 mousePos = SCENE.GetMouse()->GetPos();

	// 自機の描画位置（画面上の相対位置）
	float screenPosX = m_pos.x - m_scrollX;
	float screenPosY = m_pos.y;

	// 差分計算：【マウス座標 - 自機の画面内座標】
	float diffX = mousePos.x - screenPosX;
	float diffY = mousePos.y - screenPosY;

	// 角度を算出
	m_angle = atan2f(diffY, diffX);

	// 自機の画像補正（上向き素材の場合の-90度補正）
	float drawAngle = m_angle - DirectX::XM_PIDIV2;

	// 行列の作成
	m_scaleMat = Math::Matrix::CreateScale(m_scaleX, m_scaleY, 1.0f);
	Math::Matrix rotMat = Math::Matrix::CreateRotationZ(drawAngle);
	m_transMat = Math::Matrix::CreateTranslation(screenPosX, screenPosY, 0);

	// 合成
	m_mat = m_scaleMat * rotMat * m_transMat;
}

void C_Player::Init()
{
	m_pos = { 0,0 };
	m_move = { 0,0 };
	m_alive = true;
	m_scaleX = 1.0f;
	m_scaleY = 1.0f;



	//C_Map* map = m_owner->GetMap();
	//m_scrollX = 0.0f;
	//m_scrollMin = map->GetPos(0, 0).x + 640;
	//m_scrollMax = map->GetPos(0, map->GetWidth() - 1).x - 640;

}

void C_Player::Action()
{
	if (!m_alive) return;

	//bDashing = (GetAsyncKeyState(VK_SHIFT) & 0x8000);



	m_move.y = 0;

	m_move.x = 0;


	// Dキー（右移動）
	if (GetAsyncKeyState('D') & 0x8000)
	{
		// Shiftキーが押されていればダッシュ倍率をかける
		m_move.x += MovePow;
	}

	// Aキー（左移動）
	if (GetAsyncKeyState('A') & 0x8000)
	{
		// Shiftキーが押されていればダッシュ倍率をかける
		m_move.x -= MovePow;
	}

	if (GetAsyncKeyState('W') & 0x8000)
	{
		// Shiftキーが押されていればダッシュ倍率をかける
		m_move.y += MovePow;
	}

	if (GetAsyncKeyState('S') & 0x8000)
	{
		// Shiftキーが押されていればダッシュ倍率をかける
		m_move.y -= MovePow;
	}



	// Player.cpp の Action() 内
	if (GetAsyncKeyState(VK_LBUTTON) & 0x8000) {
		// 連続で出過ぎないよう、簡単なタイマーを入れるのがおすすめ
		static int timer = 0;
		if (--timer <= 0) {
			// SCENEマクロ経由で弾を追加
			// m_angleはUpdateで計算したマウスへの角度
			SCENE.AddBullet(m_pos, m_angle);
			timer = 10; // 次に撃てるまでのフレーム数
		}
	}


	//if (GetAsyncKeyState(VK_SPACE) & 0x8000)
	//{
	//	if (!m_jump)
	//	{
	//		m_move.y = JumpPow;
	//		m_jump = true;
	//	}
	//}

}
