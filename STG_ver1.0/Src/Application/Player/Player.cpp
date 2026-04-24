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

	// テスト用：Gキーを押すと弾数が増える
	if (GetAsyncKeyState('G') & 0x0001) { // 押した瞬間だけ
		m_shotCount += 2; // 1 -> 3 -> 5 と増えていく
		if (m_shotCount > 11) m_shotCount = 1; // 増えすぎたら戻る
	}


	// Player.cpp の Action() 内
	if (GetAsyncKeyState(VK_LBUTTON) & 0x8000) {
		if (--m_shootTimer <= 0) { // クラスのタイマーを使用
			float spreadRad = DirectX::XMConvertToRadians(15.0f);

			for (int i = 0; i < m_shotCount; ++i) {
				float offset = 0;
				if (m_shotCount > 1) {
					offset = (i - (m_shotCount - 1) * 0.5f) * spreadRad;
				}
				SCENE.AddBullet(m_pos, m_angle + offset);
			}

			// 10固定ではなく、変数にする
			m_shootTimer = m_shootInterval;
		}
	}
	else {
		// ボタンを離している間もタイマーを減らしておくと、
		// 次に押した瞬間にすぐ撃てるので操作感が良くなります
		if (m_shootTimer > 0) m_shootTimer--;
	}

}

void C_Player::Upgrade(OrbType type) {
	switch (type) {
	case OrbType::Blue:
		// 青：連射速度アップ（間隔を短くする）
		m_shootInterval -= 2; // 1回拾うごとに2フレーム短縮
		m_bulletSpeed += 1.5f; // 弾も速くして、より「レーザー」っぽくする
		// 最速でも1フレームに1発（または2〜3フレーム）に制限しないと
		// 弾が出過ぎて処理が止まるので注意！
		if (m_shootInterval < 3) m_shootInterval = 3;
		break;

	case OrbType::Red:
		// 赤の強化（サイズアップなど）
		break;

	case OrbType::Yellow:
		m_shotCount += 2;
		if (m_shotCount > 15) m_shotCount = 15;
		break;
	}
}