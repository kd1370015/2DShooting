#include "Player.h"
#include <Application/Scene/GameScene/GameScene.h>
#include <Application/System/mouse.h>
#include <Application/Core/main.h>

//player.cpp




void C_Player::Draw()
{
	// 画像がない、または死んでいるなら何もしない
	if (!m_alive || !m_tex) return;

	// 行列を一度リセット（これまでの描画の影響を消す）
	SHADER.m_spriteShader.SetMatrix(Math::Matrix::Identity);

	// 矩形（画像のサイズ 64x64 と仮定）
	Math::Rectangle srcRect = { 0, 0, 64, 64 };

	// 第2, 第3引数に「現在の座標(m_pos)」を渡して描画
	// &srcRect と & 忘れずに！
	SHADER.m_spriteShader.DrawTex(m_tex, (int)m_pos.x, (int)m_pos.y, &srcRect);
}

void C_Player::Update()
{
	//if (!m_owner) return; // 安全策


	// 座標確定処理
	m_pos += m_move;

	// Update内
// --- 移動制限の追加 (1280x720 画面中央0,0想定) ---
	float limitX = (1280.0f / 2.0f) - 32.0f; // 画面端 640 から自機の半径 32 を引く
	float limitY = (720.0f / 2.0f) - 32.0f;  // 画面端 360 から自機の半径 32 を引く

	// X方向の制限
	if (m_pos.x > limitX) m_pos.x = limitX;
	if (m_pos.x < -limitX) m_pos.x = -limitX;

	// Y方向の制限
	if (m_pos.y > limitY) m_pos.y = limitY;
	if (m_pos.y < -limitY) m_pos.y = -limitY;

	if (m_invincibleTimer > 0) m_invincibleTimer--; // タイマーを減らす

	// --- マウスの方向を向く処理 (C_Mouseから座標をもらう) ---
	// ※Sceneクラスに GetMouse() が実装されている前提です
// m_owner（GameScene*）経由でマウス座標を取得
	Math::Vector2 mousePos = m_owner->GetMouse()->GetPos();


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




}

void C_Player::Action()
{
	if (!m_alive) return;




	m_move.y = 0;

	m_move.x = 0;


	// Dキー（右移動）
	if (GetAsyncKeyState('D') & 0x8000)
	{
		m_move.x += MovePow;
	}

	// Aキー（左移動）
	if (GetAsyncKeyState('A') & 0x8000)
	{
		m_move.x -= MovePow;
	}

	if (GetAsyncKeyState('W') & 0x8000)
	{
		m_move.y += MovePow;
	}

	if (GetAsyncKeyState('S') & 0x8000)
	{
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
				m_owner->AddBullet(m_pos, m_angle + offset);
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

	// ★重要：計算した移動量を座標に反映する
	m_pos += m_move;

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


// Player.cpp
void C_Player::DecreaseHp(int damage) {
	// 1. 無敵タイマーが動いている間は、ダメージ処理を完全にスルーする
	if (m_invincibleTimer > 0) return;

	// 2. HPを減らす
	m_hp -= damage;
	if (m_hp < 0) m_hp = 0;

	// 3. 被弾した瞬間にタイマーをセット
	m_invincibleTimer = 60; // 1秒間
}