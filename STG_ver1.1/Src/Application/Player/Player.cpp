#include "Player.h"
#include <Application/Scene/GameScene/GameScene.h>
#include <Application/System/mouse.h>
#include <Application/Core/main.h>
#include <Application/Scene/SceneManager.h>
#include <Application/System/Score.h>






void C_Player::Draw()
{
	if (!m_alive || !m_tex) return;


	// 現在のアニメーション枠（0, 1, 2, 3）を取得
	int frameIdx = (int)m_turboFrame;

	// --- 1. ターボの描画（本体の後ろ） ---
	// 雑魚敵の進行方向（左）に合わせて、右側にターボを表示
	// スケールは 1.0f（または敵のサイズに合わせて調整）
	Math::Matrix turboScale = Math::Matrix::CreateScale(1.0f, 1.0f, 1.0f);
	// 右に 30px ほどずらして配置
	Math::Matrix turboTrans = Math::Matrix::CreateTranslation(m_pos.x - 25.0f, m_pos.y, 0.05f); // 0.05fで少し奥へ

	SHADER.m_spriteShader.SetMatrix(turboScale * turboTrans);

	Math::Rectangle turboRect = { 0, 0, 64, 64 };
	// 配列の画像を描画
	SHADER.m_spriteShader.DrawTex(&m_turboTex[frameIdx], 0, 0, &turboRect);


	Math::Color drawColor = { 1.0f, 1.0f, 1.0f, 1.0f };

	// --- ★点滅処理の追加 ---
	if (m_invincibleTimer > 0) {
		// タイマーを10で割った余りを見ることで、10フレームごとに表示/非表示を切り替える
		// 数値を小さくする（例: % 4）と点滅が速くなります
		if ((m_invincibleTimer / 5) % 2 == 0) {
			drawColor.A(0.3f); // 半透明（または 0.0f で完全に消す）
		}
	}

	// 自機のベース行列（位置・回転）
	Math::Matrix rot = Math::Matrix::CreateRotationZ(m_angle - DirectX::XM_PIDIV2);
	Math::Matrix trans = Math::Matrix::CreateTranslation(m_pos.x, m_pos.y, 0);
	Math::Matrix mat = rot * trans;



	// =========================================================
	// 2. プレイヤー本体（自機・回避・パリィ）の描画
	// =========================================================
	SHADER.m_spriteShader.SetMatrix(mat);

		// ★追加：パリィアニメーションの描画
		if (IsParrying() && m_parryAnimTex) {
			// PARRY_DURATION (20) に対して 60コマを割り当てる
			// (20 - 残り時間) / 20.0f で 0.0～1.0 の進捗率を出し、59を掛けてコマ番号にする
			float progress = (float)(PARRY_DURATION - m_parryTimer) / (float)PARRY_DURATION;
			int currentStep = (int)(progress * (PARRY_STEP_MAX - 1));
	
			// 念のための範囲ガード
			if (currentStep < 0) currentStep = 0;
			if (currentStep >= PARRY_STEP_MAX) currentStep = PARRY_STEP_MAX - 1;
	
			int x = currentStep * PARRY_CHIP_W;
			Math::Rectangle srcRectParryAnim = { x, 0, PARRY_CHIP_W, PARRY_CHIP_H };
	
			SHADER.m_spriteShader.DrawTex(m_parryAnimTex, 0, 0, &srcRectParryAnim, &drawColor);
		}
		else if (IsDodging() && m_dodgeTex) {
			// --- 回避アニメーションの描画 ---
			int currentStep = (30 - m_dodgeTimer) * 2;
			if (currentStep >= DODGE_STEP_MAX) currentStep = DODGE_STEP_MAX - 1;
	
			int x = currentStep * DODGE_CHIP_W;
			Math::Rectangle srcRectDodge = { x, 0, DODGE_CHIP_W, DODGE_CHIP_H };
	
			SHADER.m_spriteShader.DrawTex(m_dodgeTex, 0, 0, &srcRectDodge, &drawColor);
		}

		else if (IsDodging() && m_dodgeTex) {
			// 現在のステップ計算
			int currentStep = (30 - m_dodgeTimer) * 2;
			if (currentStep >= DODGE_STEP_MAX) currentStep = DODGE_STEP_MAX - 1;

			// --- 残像の描画 (3つほど過去のポーズを出す) ---
			for (int i = 1; i <= 3; ++i) {
				int ghostStep = currentStep - (i * 2); // 少し前のコマ
				if (ghostStep < 0) continue;

				int gx = ghostStep * DODGE_CHIP_W;
				Math::Rectangle srcRectGhost = { gx, 0, DODGE_CHIP_W, DODGE_CHIP_H };

				Math::Color ghostColor = Math::Color(1.0f, 1.0f, 1.0f, 0.5f); // 半透明の白
				
				// 位置を少し後ろにずらすと、より「速さ」が出ます
				// 自機の移動方向（ベクトルの逆）にずらすのが理想です
				float offset = i * -5.0f;
				SHADER.m_spriteShader.DrawTex(m_dodgeTex, offset, 0, &srcRectGhost, &ghostColor);
			}

			// --- 本体の描画 ---
			int x = currentStep * DODGE_CHIP_W;
			Math::Rectangle srcRectDodge = { x, 0, DODGE_CHIP_W, DODGE_CHIP_H };
			SHADER.m_spriteShader.DrawTex(m_dodgeTex, 0, 0, &srcRectDodge, &drawColor);
		}

		else
		{
			// 通常時
			if (m_isPoweredUp) drawColor = { 1.0f, 1.0f, 1.0f, 1.0f };
			Math::Rectangle srcRectPlayer = { 0, 0, 80, 80 };
			SHADER.m_spriteShader.DrawTex(m_tex, 0, 0, &srcRectPlayer, &drawColor);
		}

	// 行列リセット
	SHADER.m_spriteShader.SetMatrix(Math::Matrix::Identity);
}

void C_Player::Update()
{
	if (!m_owner) return; // 安全策
	// 1. まず入力を受け取る（ここで m_move が決まる）
	Action();

	// 座標確定処理
	m_pos += m_move;


	// --- 移動制限の修正 ---
	float limitX = (1280.0f / 2.0f) - 32.0f; // 左側の限界（画面端）

	// ★ 右側の限界を「真ん中の少し右（例：100）」に設定
	float limitXPlus = 100.0f;

	float limitY = 275.0f - 32.0f;

	// Y方向の制限
	if (m_pos.y > limitY) m_pos.y = limitY;
	if (m_pos.y < -limitY) m_pos.y = -limitY;

	// --- X方向の制限を個別に設定 ---
	if (m_pos.x > limitXPlus) m_pos.x = limitXPlus; // 右は limitXPlus まで
	if (m_pos.x < -limitX)    m_pos.x = -limitX;    // 左は 画面端 まで

	if (m_invincibleTimer > 0) m_invincibleTimer--; // タイマーを減らす




	// 自機の描画位置（画面上の相対位置）
	float screenPosX = m_pos.x - m_scrollX;
	float screenPosY = m_pos.y;



	m_angle = 0.0f;


	// 行列の作成
	m_scaleMat = Math::Matrix::CreateScale(m_scaleX, m_scaleY, 1.0f);

	Math::Matrix rotMat = Math::Matrix::CreateRotationZ(m_angle);
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

	// ... 既存の初期化 ...
	m_dodgeTex = nullptr; // 必要に応じてGameSceneからセットするか、ここでLoadする

	// --- 1～4の画像をロード ---
	for (int i = 0; i < 4; i++) {
		// i=0のとき"1"、i=3のとき"4"になるようにする
		std::string fileName = "Texture/Player/P_T_" + std::to_string(i + 1) + ".png";
		m_turboTex[i].Load(fileName);
	}

	m_turboFrame = 0.0f;


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

	// 移動ベクトルの長さを計算
	float length = sqrtf(m_move.x * m_move.x + m_move.y * m_move.y);

	if (length > 0)
	{
		// 各成分を長さで割ることで、ベクトルの長さを1にする
		m_move.x /= length;
		m_move.y /= length;

		// 最後に本来の移動速度（MovePow）を掛ける
		m_move.x *= MovePow;
		m_move.y *= MovePow;
	}





	// --- 回避 (Kキー) ---
	bool nowKKey = (GetAsyncKeyState('K') & 0x8000) != 0;

	// 【修正点】パリィ中でない（m_parryTimer <= 0）時だけ回避できる
	if (nowKKey && !m_oldDodgeKey && m_parryTimer <= 0) {
		if (m_dodgeCooldownTimer <= 0) {
			m_dodgeTimer = 30;
			m_dodgeCooldownTimer = 50;
			m_canDodgeCharge = true;

			// もし回避した瞬間にパリィの判定が残っていたら消す（念のため）
			m_parryTimer = 0;
		}
	}
	m_oldDodgeKey = nowKKey;

	//// タイマーの更新
	if (m_dodgeTimer > 0) m_dodgeTimer--;
	if (m_dodgeCooldownTimer > 0) m_dodgeCooldownTimer--;


	// --- パリィ (Jキー) ---
	bool nowJKey = (GetAsyncKeyState('J') & 0x8000) != 0;

	// 【修正点】回避中でない（m_dodgeTimer <= 0）時だけパリィできる
	if (nowJKey && !m_oldParryKey && m_dodgeTimer <= 0) {
		if (m_parryCooldownTimer <= 0 && m_parryTimer <= 0) {
			m_parryTimer = PARRY_DURATION;
			m_parryCooldownTimer = PARRY_COOLDOWN;

			m_canHeal = true;
			m_canShot = true;

			m_owner->AddEffect(m_pos, 2.0f);
		}
	}
	m_oldParryKey = nowJKey;

	if (m_parryCooldownTimer > 0) m_parryCooldownTimer--;
	if (m_parryTimer > 0) m_parryTimer--;

	// Player.cpp の Update内
	if (m_isUsingPower) {
		m_consumeTimer++;

		// 1秒(60F)ごとに 20% (1メモリ) 減らす
		if (m_consumeTimer >= 60) {
			m_energy -= 25.0f;
			m_consumeTimer = 0;

			if (m_energy <= 0.0f) {
				m_energy = 0.0f;
				m_isUsingPower = false;
				m_isPoweredUp = false;
				m_shotCount -= 2; // ★忘れずにショット数を元に戻す！
			}
		}
	}

	// 0.2f ずつ足すと、5フレームごとに画像が切り替わります
	m_turboFrame += 0.2f;
	if (m_turboFrame >= 4.0f) {
		m_turboFrame = 0.0f;
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


void C_Player::ChargeEnergy(float amount) {
	// 強化中（エネルギー消費中）は溜まらない
	if (m_isUsingPower) return;

	m_energy += amount;

	if (m_energy >= MAX_ENERGY) {
		m_energy = MAX_ENERGY;    // ★ 0にせず100(満タン)で止める
		m_isUsingPower = true;    // 消費モード開始
		m_isPoweredUp = true;     // 強化フラグON
		m_consumeTimer = 0;       // 減少用タイマーリセット

		m_shotCount += 2;         // 拡散数アップなどの強化
	}
}



//void C_Player::DecreaseHp(int damage) {
//	// すでに死んでいる場合は、何度もシーン遷移を呼ばないようにガード
//	if (m_hp <= 0) return;
//
//	// 無敵タイマー中ならダメージ処理を飛ばす
//	if (m_invincibleTimer > 0) return;
//
//	// HPを減らす
//	m_hp -= damage;
//
//	if (m_hp <= 0) {
//		m_hp = 0;
//		// 死亡した瞬間にシーン遷移を呼ぶ
//		SceneManager::GetInstance().SetNextScene(SceneManager::SceneType::Result);
//	}
//	else {
//		// 生きている時だけ無敵タイマーをセット
//		m_invincibleTimer = 60;
//	}
//
//}

void C_Player::DecreaseHp(int damage) {
	// すでに死んでいる場合は、何度もシーン遷移を呼ばないようにガード
	if (m_hp <= 0) return;

	// 無敵タイマー中ならダメージ処理を飛ばす
	if (m_invincibleTimer > 0) return;

	// HPを減らす
	m_hp -= damage;

	if (m_hp <= 0) {
		m_hp = 0;

		// ★ここを修正：GetScoreManager() ではなく GetScore() を使う
		if (m_owner) {
			// GameSceneの m_score から現在のスコア数値を取得
			// ※Scoreクラスに GetScore() という数値を返す関数がある前提です
			int currentScore = m_owner->GetScore().GetScore();
			SceneManager::GetInstance().SetFinalScore(currentScore);
		}

		// 死亡した瞬間にシーン遷移を呼ぶ
		SceneManager::GetInstance().SetNextScene(SceneManager::SceneType::Result);
	}
	else {
		// 生きている時だけ無敵タイマーをセット
		m_invincibleTimer = 60;
	}
}