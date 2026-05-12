#include "Player.h"
#include <Application/Scene/GameScene/GameScene.h>
#include <Application/System/mouse.h>
#include <Application/Core/main.h>
#include <Application/Scene/SceneManager.h>
#include <Application/System/Score.h>

//player.cpp
void C_Player::Draw()
{
	if (!m_alive || !m_tex) return;

	// --- ここでその場で行列を作ってしまう（最強のデバッグ） ---
	// もし m_scaleX 等が不安なら直接 1.0f と書く
	Math::Matrix scale = Math::Matrix::CreateScale(1.0f, 1.0f, 1.0f);

	// drawAngle が Update で正しく計算されているか確認
	// 上向き画像なら：m_angle - DirectX::XM_PIDIV2
	Math::Matrix rot = Math::Matrix::CreateRotationZ(m_angle - DirectX::XM_PIDIV2);

	// screenPosX が不安なら直接 m_pos.x と書く
	Math::Matrix trans = Math::Matrix::CreateTranslation(m_pos.x, m_pos.y, 0);

	// 行列を合成して即座にセット
	Math::Matrix mat = scale * rot * trans;
	SHADER.m_spriteShader.SetMatrix(mat);

	// 描画 (行列に座標が入っているので 0, 0 指定)
	Math::Rectangle srcRect = { 0, 0, 64, 64 };
	SHADER.m_spriteShader.DrawTex(m_tex, 0, 0, &srcRect);

	// ★ 次の描画物のためにリセット（これを忘れるとエネミーや弾が消える原因になる）
	SHADER.m_spriteShader.SetMatrix(Math::Matrix::Identity);
}


//void C_Player::Draw()
//{
//	// 画像がない、または死んでいるなら何もしない
//	if (!m_alive || !m_tex) return;
//
//	// 行列を一度リセット（これまでの描画の影響を消す）
//	SHADER.m_spriteShader.SetMatrix(Math::Matrix::Identity);
//
//	// 矩形（画像のサイズ 64x64 と仮定）
//	Math::Rectangle srcRect = { 0, 0, 64, 64 };
//
//	// 第2, 第3引数に「現在の座標(m_pos)」を渡して描画
//	// &srcRect と & 忘れずに！
//	SHADER.m_spriteShader.DrawTex(m_tex, (int)m_pos.x, (int)m_pos.y, &srcRect);
//}



void C_Player::Update()
{
	if (!m_owner) return; // 安全策
	// 1. まず入力を受け取る（ここで m_move が決まる）
	Action();

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
	//Math::Vector2 mousePos = m_owner->GetMouse()->GetPos();


	// 自機の描画位置（画面上の相対位置）
	float screenPosX = m_pos.x - m_scrollX;
	float screenPosY = m_pos.y;


	// 3. その後、最新の m_pos を使って角度を計算する
	Math::Vector2 mousePos = m_owner->GetMouse()->GetPos();
	float diffX = mousePos.x - m_pos.x;
	float diffY = mousePos.y - m_pos.y;
	m_angle = atan2f(diffY, diffX);

	//// 自機の画像補正（上向き素材の場合の-90度補正）
	float drawAngle = m_angle - DirectX::XM_PIDIV2;


	// 行列の作成
	m_scaleMat = Math::Matrix::CreateScale(m_scaleX, m_scaleY, 1.0f);

	Math::Matrix rotMat = Math::Matrix::CreateRotationZ(drawAngle);
	m_transMat = Math::Matrix::CreateTranslation(screenPosX, screenPosY, 0);
	// 一旦、スクロールを無視して見た目と位置を一致させてみる

	// 合成
	m_mat = m_scaleMat * rotMat * m_transMat;

	// 強制的に「等倍・回転なし・画面中央」の行列にする
	m_mat = Math::Matrix::Identity;
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

	//// ★重要：計算した移動量を座標に反映する
	//m_pos += m_move;

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



void C_Player::DecreaseHp(int damage) {
	// すでに死んでいる場合は、何度もシーン遷移を呼ばないようにガード
	if (m_hp <= 0) return;

	// 無敵タイマー中ならダメージ処理を飛ばす
	if (m_invincibleTimer > 0) return;

	// HPを減らす
	m_hp -= damage;

	if (m_hp <= 0) {
		m_hp = 0;
		// 死亡した瞬間にシーン遷移を呼ぶ
		SceneManager::GetInstance().SetNextScene(SceneManager::SceneType::Result);
	}
	else {
		// 生きている時だけ無敵タイマーをセット
		m_invincibleTimer = 60;
	}

	//// PlayerのDecreaseHp内、またはGameSceneのUpdate内
	//if (m_hp <= 0) {
	//	// 現在のスコアをSceneManagerに保存
	//	SceneManager::GetInstance().SetFinalScore(m_score->GetScore());
	//	// リザルトへ
	//	SceneManager::GetInstance().SetNextScene(SceneManager::SceneType::Result);
	//}
}