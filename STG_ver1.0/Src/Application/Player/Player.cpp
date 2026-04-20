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

	// --- マウスの方向を向く処理 ---
	POINT mousePos;
	GetCursorPos(&mousePos);
	ScreenToClient(APP.m_window.GetWndHandle(), &mousePos);

	// マウスの座標系を実行画面の座標系(中心が0,0)に補正
	float mouseX = (float)mousePos.x - (ScrWidth / 2.0f);
	float mouseY = (float)mousePos.y - (ScrHeight / 2.0f);
	mouseY *= -1; // Y軸をゲーム座標系（上がプラス）に合わせる

	// 自機の描画位置
	float screenPosX = m_pos.x - m_scrollX;
	float screenPosY = m_pos.y;

	// 差分計算：【目標 - 自分】にする
	float diffX = mouseX - screenPosX;
	float diffY = mouseY - screenPosY; // 修正ポイント：ここを (マウス - 自機) に

	// 角度を算出
	float angle = atan2f(diffY, diffX);


	 // ★計算した角度をメンバ変数に保存しておく
	 m_angle = angle;

	 // 自機の画像補正（上向き素材の場合）
	 float drawAngle = m_angle - DirectX::XM_PIDIV2;



	// 行列の作成
	m_scaleMat = Math::Matrix::CreateScale(m_scaleX, m_scaleY, 1.0f);
	// 行列作成時は描画用の角度（drawAngle）を使う
	Math::Matrix rotMat = Math::Matrix::CreateRotationZ(drawAngle);
	m_transMat = Math::Matrix::CreateTranslation(m_pos.x - m_scrollX, m_pos.y, 0);

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
