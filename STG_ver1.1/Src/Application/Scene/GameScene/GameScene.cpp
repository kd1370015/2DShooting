#include "GameScene.h"
#include "Application/System/hit.h"
#include "Application/VFX/Effect.h" // パスは自分のフォルダ構成に合わせて
#include "Application/VFX/Charge.h" // パスは環境に合わせて
#include "Application/VFX/levelup.h"  // ★これを追加！


void GameScene::Init()
{
	m_back.Init(0);
	m_mouse.Init();
	m_player.SetOwner(this); // ※後述の注意点参照

	m_playerTex.Load("Texture/Player/SF01.png");
	m_dodgeTex.Load("Texture/Player/SF01_kaihi.png");
	// ★追加：パリィ用アニメーションテクスチャのロード
	m_parryAnimTex.Load("Texture/Player/SF01_Parry.png");
	m_turboTex.Load("Texture/Player/turbo.png");

	m_player.Init();
	m_player.SetTex(&m_playerTex);
	// ★追加：プレイヤーに回避テクスチャを渡す
	m_player.SetDodgeTex(&m_dodgeTex);
	// ★追加：プレイヤーにパリィテクスチャを渡す
	m_player.SetParryAnimTex(&m_parryAnimTex);
	// ★追加：プレイヤーにターボテクスチャを渡す
	m_player.SetTurboTex(&m_turboTex);

	m_hpTex.Load("Texture/VFX/hp.png"); // パスは環境に合わせてください

	// 敵の「見た目」だけ先にロードしておく
	m_enemyTex.Load("Texture/Enemy/E_Ship2.png");
	// 砲台の画像をロード（ファイル名は用意したものに変えてください）
	m_turretTex.Load("Texture/Enemy/E_Ship1.png");

	// スポーンタイマー初期化
	m_spawnTimer = 60;

	// 弾のテクスチャ読み込み（ファイル名は用意したものに変えてください）
	m_bulletTex.Load("Texture/Bullet/11.png");
	// ★敵の弾（ファイル名は用意したものに変えてください）
	m_enemyBulletTex.Load("Texture/Bullet/14.png");

	m_texOrbBlue.Load("Texture/Item/blue.png"); // など
	m_texOrbRed.Load("Texture/Item/red.png");
	m_texOrbYellow.Load("Texture/Item/yellow.png");

	m_parryTex.Load("Texture/Player/circle.png"); // 円の画像をロード

	m_barrierTex.Load("Texture/VFX/parryeffect.png");

	// ★追加：チャージ用テクスチャのロード（ファイル名は合わせたものに）
	m_chargeTex.Load("Texture/VFX/charge2.png");
	// 1. 枠（frame）
	m_chargeFrameTex.Load("Texture/VFX/charge_barframe.png");
	// 2. ゲージの中身（ゲージ）
	m_chargeGaugeTex.Load("Texture/VFX/charge_bar.png");
	m_chargeLowTex.Load("Texture/VFX/charge_barlow.png"); // ゲージが少ないときのテクスチャ

	m_chargeIconTex.Load("Texture/VFX/chargeIcon3.png"); // パスは環境に合わせて
	m_levelUpTex.Load("Texture/VFX/levelup.png");

	static KdTexture tex;
	tex.Load("Texture/VFX/expl.png");
	m_texExplosion = &tex;

	// ★追加：チャージクラスの生成
	// コンストラクタには「枠」のテクスチャとプレイヤーを渡す
	//m_chargeEffect = std::make_shared<C_Charge>(&m_chargeFrameTex, &m_player);
	// 2. その後でクラスを生成し、ロードしたテクスチャを渡す
	m_chargeEffect = std::make_shared<C_Charge>(&m_chargeFrameTex, &m_chargeLowTex, &m_player);

	m_totalScrollX = 0.0f; // リセット

	// スコアの初期化
	m_score.Init();

}

void GameScene::Update()
{

	// --- スロー状態の管理 ---
	if (m_slowTimer > 0) {
		m_slowTimer--;
		m_timeScale = 0.3f; // 30%の速度
	}
	else {
		m_timeScale = 1.0f; // 通常速度
	}


	// 1. スクロール（スロー中はスクロールも遅くする）
	m_totalScrollX += 2.0f * m_timeScale;
	// 1. スクロール・プレイヤー更新
	//m_totalScrollX += 2.0f;
	m_back.Update(m_totalScrollX);



	//m_mouse.Update();
	m_player.Update();

	// ★追加：チャージエフェクトの更新
	if (m_chargeEffect) {
		m_chargeEffect->Update();
	}

	// ★追加：エフェクトの更新と削除（これがないとエフェクトが動きません）
	for (auto it = m_effects.begin(); it != m_effects.end(); ) {
		(*it)->Update();

		if (!(*it)->IsAlive()) {
			it = m_effects.erase(it); // 寿命が来たらリストから削除
		}
		else {
			++it;
		}
	}

	// =========================================================
	// ★追加：LevelUp（バブルエフェクト）の管理
	// =========================================================

	// 1. パワーアップ状態の取得
	bool isPoweredUp = m_player.IsUsingPower();

	// 2. チャージ完了の瞬間判定（前回の値と比較して発生させる）
	float currentCharge = m_player.GetCharge();
	if (currentCharge >= 100.0f && m_prevCharge < 100.0f)
	{
		// プレイヤーの座標でエフェクトを新規生成
		AddLevelUpEffect(m_player.GetPos());
	}
	m_prevCharge = currentCharge; // 次のフレーム用に保存

	// 3. 全てのエフェクトをプレイヤーに追従させ、状態を更新する
	auto itLvl = m_levelUpEffects.begin();
	while (itLvl != m_levelUpEffects.end())
	{
		// パワーアップ中ならループ、終わったら1回再生して終了
		(*itLvl)->SetLoop(isPoweredUp);

		// 更新（内部でプレイヤーの最新座標に追従する）
		(*itLvl)->Update();

		if (!(*itLvl)->IsActive()) {
			itLvl = m_levelUpEffects.erase(itLvl);
		}
		else {
			++itLvl;
		}
	}







	// 例：一定時間ごとに敵を出す
	// --- A. タイマー更新の共通フラグ ---
// スロー中は3フレームに1回しかタイマーが進まないようにする
	bool isUpdateFrame = (m_slowTimer <= 0 || (m_slowTimer % 3 == 0));

	if (isUpdateFrame)
	{
		// --- 1. 砲撃型（艦隊）の出現管理 ---
		if (--m_spawnTimer <= 0)
		{
			// 画面内の砲台の数をチェック
			if (m_enemyManager.GetTurretCount() < 10)
			{
				int numEnemies = 4;
				float gapY = 70.0f;
				float startY = ((float)numEnemies - 1.0f) * gapY / 2.0f;
				float areaOffset = m_spawnUpper ? 120.0f : -120.0f;
				m_spawnUpper = !m_spawnUpper;

				for (int i = 0; i < numEnemies; ++i) {
					float microNoise = (float)(rand() % 40 - 20);
					float spawnY = (startY + areaOffset + microNoise) - (i * gapY);
					float stopX = 550.0f - (i * 60.0f);
					m_enemyManager.AddTurret(Math::Vector2(850.0f, spawnY), stopX);
				}
				m_spawnTimer = 200; // 次の艦隊まで
			}
			else {
				m_spawnTimer = 60;  // 敵がいっぱいなら少し待つ
			}
		}

		// --- 2. 追尾型の出現管理 ---
		if (--m_followerSpawnTimer <= 0)
		{
			// Managerで作った、上下端から出る関数を呼ぶ
			m_enemyManager.SpawnEnemy(Math::Vector2(850.0f, 0));

			m_followerSpawnTimer = FOLLOWER_INTERVAL;
		}
	}


	// 2. EnemyManagerの一括更新
	// プレイヤーの座標だけでなく、偏差射撃用に速度(GetMove等)も渡す
	//m_enemyManager.Update(m_player.GetPos(), m_player.GetMove(), this);

	m_enemyManager.Update(m_player.GetPos(), m_player.GetMove(), this, m_timeScale);

	// 当たり判定実行
// 第3引数に this を渡すことで、Hit側が「自分(GameScene)」にアクセスできるようになる
	C_Hit::CheckBulletToEnemy(m_bullets, m_enemyManager.GetEnemies(), this);
	C_Hit::CheckBulletToTurret(m_bullets, m_enemyManager.GetTurrets(), this);

	C_Hit::CheckPlayerToEnemy(m_player, m_enemyManager.GetEnemies());
	C_Hit::CheckPlayerToEnemyBullet(m_player, m_enemyBullets);


	for (auto it = m_bullets.begin(); it != m_bullets.end(); )
	{
		//(*it)->Update(); // スローに対応させるなら Bullet::Update 内で m_timeScale を使う

		(*it)->Update(m_timeScale); // ★ここで渡す！
		if (!(*it)->IsAlive()) { delete (*it); it = m_bullets.erase(it); }
		else { ++it; }
	}

	// --- 追尾型エネミー本体との当たり判定ループ ---
// --- 追尾型エネミー本体との判定ループ ---
	auto& enemies = m_enemyManager.GetEnemies();
	for (auto it = enemies.begin(); it != enemies.end(); ) {
		auto& e = *it;
		float dist = Math::Vector2::Distance(m_player.GetPos(), e->GetPos());

		// --- 1. パリィ判定（最優先） ---
		// 追尾エネミーもパリィ範囲内なら、自分の弾にして弾き返す！
		if (m_player.IsParrying() && dist < m_player.GetParryRadius()) {

			// A. 反射弾の生成
			if (m_player.CanShot()) {
				Math::Vector2 spawnPos = e->GetPos();
				float baseAngle = m_player.GetAngle();
				int count = m_player.GetShotCount();

				for (int i = -(count / 2); i <= (count / 2); ++i) {
					float offset = DirectX::XMConvertToRadians(i * 15.0f);
					AddBullet(spawnPos, baseAngle + offset);
				}
				m_player.SetShotDone();
			}

			// B. 回復処理
			if (m_player.CanHeal()) {
				m_player.Heal(1);
				m_player.SetHealDone();
			}

			// C. 敵を消去
			e->SetAlive(false);
			it = enemies.erase(it);
			continue; // 次の敵の判定へ
		}

		// --- 2. スロー（かすり）判定 ---
		if (dist < 30.0f && m_player.IsDodging()) {
			if (m_slowTimer <= 0) {
				m_slowTimer = 30;
				AddEffect(m_player.GetPos(), 2.0f);
			}
		}

		// --- 3. 強制ダメージ判定（回避不可） ---
		if (dist < 15.0f) {
			// 回避中であっても、パリィしていなければダメージ
			m_player.DecreaseHp(1);

			e->SetAlive(false);
			it = enemies.erase(it);
			continue;
		}

		++it;
	}

 ////--- ★追加：敵の弾の更新 ＆ パリィ・回避スロー・ダメージ判定 ---
for (auto it = m_enemyBullets.begin(); it != m_enemyBullets.end(); ) {
	C_Bullet* eb = (*it);

	// 1. 弾の移動（計算済みの m_timeScale を渡してスロー対応）
	eb->Update(m_timeScale);

	// 2. プレイヤーと弾の距離を計算
	float dist = Math::Vector2::Distance(m_player.GetPos(), eb->GetPos());

	// --- A. パリィ判定（最優先：弾を跳ね返す） ---
	if (m_player.IsParrying() && dist < m_player.GetParryRadius()) {
		// 反射弾の生成（1回のみ）
		if (m_player.CanShot()) {
			Math::Vector2 bulletPos = eb->GetPos();
			float baseAngle = m_player.GetAngle();
			int count = m_player.GetShotCount();

			for (int i = -(count / 2); i <= (count / 2); ++i) {
				float offset = DirectX::XMConvertToRadians(i * 15.0f);
				AddBullet(bulletPos, baseAngle + offset);
			}
			m_player.SetShotDone();
		}

		// 回復処理
		if (m_player.CanHeal()) {
			m_player.Heal(1);
			m_player.SetHealDone();
		}

		// 弾を消去して次の弾へ
		delete eb;
		it = m_enemyBullets.erase(it);
		continue;
	}


//	// --- B. 回避（チャージ ＆ スロー）判定 ---
//// 「ギリギリ」を狙うため、判定を 15.0f 程度に絞る
//	if (m_player.IsDodging() && dist < 15.0f) {
//		if (!eb->IsDodged()) {
//			// この弾で初めて「ギリギリ回避」に成功した瞬間
//
//			// 1. エネルギーチャージ
//			if (m_player.CanDodgeCharge()) {
//				m_player.ChargeEnergy(20.0f);
//				m_player.SetDodgeChargeDone();
//			}
//
//			// 2. スロー発動（チャージと同時に判定するので、必ずセットで動く）
//			if (m_slowTimer <= 0) {
//				m_slowTimer = 30;
//				AddEffect(m_player.GetPos(), 2.0f);
//			}
//
//			eb->SetDodged(true);
//		}
//	}
//
//
//	// --- C. スロー発動 ＆ ダメージ判定（至近距離 10.0f） ---
//	if (dist < 10.0f) {
//		if (m_player.IsDodging()) {
//			// 回避成功！スローが動いていなければ発動
//			if (m_slowTimer <= 0) {
//				m_slowTimer = 30; // 0.5秒スロー
//				AddEffect(m_player.GetPos(), 2.0f); // 成功エフェクト
//			}
//			// 弾は消さずに通り抜ける（無敵状態）
//		}
//		else if (dist < 5.0f) {
//			// 回避していない、かつ至近距離ならダメージ
//			m_player.DecreaseHp(1);
//
//			delete eb;
//			it = m_enemyBullets.erase(it);
//			continue;
//		}
//	}

	// --- B. 回避（チャージ ＆ スロー）判定 ---
// 「ギリギリ」かすった瞬間のご褒美処理
	if (m_player.IsDodging() && dist < 15.0f) {
		if (!eb->IsDodged()) {
			// エネルギーチャージ
			if (m_player.CanDodgeCharge()) {
				m_player.ChargeEnergy(25.0f);
				m_player.SetDodgeChargeDone();
			}

			// スロー発動
			if (m_slowTimer <= 0) {
				m_slowTimer = 30;
				AddEffect(m_player.GetPos(), 2.0f);
			}

			eb->SetDodged(true); // この弾での回避成功を記録
		}
	}

	// --- C. 被弾（HP減少）または 無敵スルー 判定 ---
	if (dist < 5.0f) { // ダメージを受ける至近距離
		if (m_player.IsDodging()) {
			// 回避中なら、体当たりしてもダメージを受けずに通り抜ける
			// (ここでは何もしない = 弾を消さずにループを継続)
		}
		else {
			// ★ HPが減る処理はここ！
			// 回避していない状態で、至近距離まで弾が来たら被弾
			m_player.DecreaseHp(1);

			delete eb;
			it = m_enemyBullets.erase(it);
			continue; // 弾が消えたので次の弾へ
		}
	}

	// 4. 寿命または画面外判定
	if (!eb->IsAlive()) {
		delete eb;
		it = m_enemyBullets.erase(it);
	}
	else {
		++it;
	}
}


	// -------------------------------------------------------
	// 爆発アニメーションの更新
	// -------------------------------------------------------
	for (auto& exp : m_explosions)
	{
		exp->Update();
	}

	// 終わった爆発をリストから削除（これがないとメモリが溜まり続けます）
	m_explosions.erase(std::remove_if(m_explosions.begin(), m_explosions.end(),
		[](const std::shared_ptr<C_Explosion>& e) { return e->IsExpired(); }), m_explosions.end());
	
	
	// 3. プレイヤーとオーブの当たり判定 ★追加
	for (auto orb : m_orbs) {
		if (!orb->IsAlive()) continue;

		float dist = Math::Vector2::Distance(m_player.GetPos(), orb->GetPos());

		// 判定半径（自機とオーブが触れたら）
		if (dist < 40.0f) {
			// --- 強化を適用 ---
			m_player.Upgrade(orb->GetType());

			// --- オーブを死んだことにする ---
			orb->Kill();
		}
	}

	// Scene.cpp の Update 内（すでに記述があるはずですが、念のため確認）
	for (auto it = m_orbs.begin(); it != m_orbs.end(); ) {
		(*it)->Update(m_player.GetPos());

		if (!(*it)->IsAlive()) {
			delete* it;            // メモリを解放
			it = m_orbs.erase(it); // リストから除外
		}
		else {
			++it;
		}
	}

	// スコアの更新（キー入力や桁数計算が行われる）
	m_score.Update();



	if (GetAsyncKeyState('M') & 0x8000) {
		SceneManager::GetInstance().SetNextScene(SceneManager::SceneType::Result);
	}


}

void GameScene::DrawSprite()
{

	m_back.Draw();
	//m_map.Draw();

	// ★ ここを修正：Identityをセットしてからプレイヤーを呼ぶのはOK
	SHADER.m_spriteShader.SetMatrix(Math::Matrix::Identity);
	m_player.Draw();

	// ★追加：チャージエフェクトの描画
	if (m_chargeEffect) {
		// ゲージの中身とアイコンを渡して描画
		m_chargeEffect->Draw(&m_chargeGaugeTex, &m_chargeIconTex);
	}

	// ★追加：エフェクトの描画
	for (auto& effect : m_effects) {
		effect->Draw();
	}

	// Managerにテクスチャを渡して一括描画
	m_enemyManager.Draw(&m_enemyTex, &m_turretTex);


	// 弾の描画
	for (auto b : m_bullets) {
		b->Draw(&m_bulletTex);
	}

	// 爆発を描画
	for (auto& exp : m_explosions) {
		exp->Draw(m_texExplosion);
	}


	// エフェクトを描画（リストの中身を全部描く）
	for (auto& effect : m_levelUpEffects) {
		effect->DrawSprite();
	}



	// --- ★ここを修正：専用テクスチャを渡す ---
	for (auto eb : m_enemyBullets) {
		eb->Draw(&m_enemyBulletTex);
	}

	// --- Scene.cpp の Draw2D 内 ---
	for (auto orb : m_orbs) {
		if (!orb->IsAlive()) continue;

		// 種類に合わせて渡すテクスチャを変える
		KdTexture* targetTex = nullptr;
		switch (orb->GetType()) {
		case OrbType::Blue:   targetTex = &m_texOrbBlue;   break;
		case OrbType::Red:    targetTex = &m_texOrbRed;    break;
		case OrbType::Yellow: targetTex = &m_texOrbYellow; break;
		}

		// Orb::Drawには、決定したテクスチャ1枚だけを渡す
		orb->Draw(targetTex);
	}




	//HPバーの描画
	{// --- HPバーを横長に大きくする ---
		int currentHp = m_player.GetHp();
		int frameWidth = 294;
		int frameHeight = 125;
		int hpIndex = 5 - currentHp;

		if (hpIndex < 0) hpIndex = 0;
		if (hpIndex > 4) hpIndex = 4;
		Math::Rectangle srcRect = { hpIndex * frameWidth, 0, frameWidth, frameHeight };

		// ★ ここを調整！
		// 第1引数(X)を大きくすると横に伸び、第2引数(Y)で高さを調整します
		float scaleX = 1.5f; // 横に1.5倍
		float scaleY = 0.8f; // 縦は少し抑える（お好みで1.0fなど）
		Math::Matrix hpScale = Math::Matrix::CreateScale(scaleX, scaleY, 1.0f);

		// 位置も横長になった分、左に寄せます
		Math::Matrix hpTrans = Math::Matrix::CreateTranslation(-420, 320, 0);

		SHADER.m_spriteShader.SetMatrix(hpScale * hpTrans);
		//SHADER.m_spriteShader.DrawTex(&m_hpTex, &srcRect, 1.0f);
		SHADER.m_spriteShader.DrawTex(&m_hpTex, 0, 0, &srcRect);
	}

	// スコアの描画
	m_score.DrawSprite();


	//m_mouse.Draw(); // クロスヘアを一番手前に描画

}

// 弾を追加する処理
void GameScene::AddBullet(Math::Vector2 pos, float angle) {
	C_Bullet* newBullet = new C_Bullet();
	newBullet->Init(pos, angle);
	m_bullets.push_back(newBullet);
}

//void GameScene::AddEnemyBullet(Math::Vector2 pos, float angle) {
//	C_Bullet* newBullet = new C_Bullet();
//	newBullet->Init(pos, angle);
//	// 敵の弾だとわかるように、色を変えたり速度を遅くしたりするとベスト
//	m_enemyBullets.push_back(newBullet);
//}

void GameScene::AddEnemyBullet(Math::Vector2 pos, float angle) {
	C_Bullet* newBullet = new C_Bullet();
	newBullet->Init(pos, angle);

	// ★ ここで「お前は敵の弾だ！」と設定する
	newBullet->SetIsEnemy(true);

	m_enemyBullets.push_back(newBullet);
}


void GameScene::AddOrb(Math::Vector2 pos) {
	C_Orb* newOrb = new C_Orb();

	// ランダムで種類を決定
	OrbType type = static_cast<OrbType>(rand() % 3);

	newOrb->Init(pos, type);
	m_orbs.push_back(newOrb);
}

void GameScene::AddEffect(Math::Vector2 pos, float scale) {
	auto newEffect = std::make_shared<C_Effect>(&m_barrierTex, &m_player, scale);
	m_effects.push_back(newEffect);
}

void GameScene::AddExplosion(Math::Vector2 pos) {
	auto newExp = std::make_shared<C_Explosion>();
	newExp->Init(pos);
	m_explosions.push_back(newExp);
}

//void GameScene::AddLevelUpEffect(Math::Vector2 pos) {
//	auto newEffect = std::make_shared<LevelUp>();
//	newEffect->Init(&m_levelUpTex); // 読み込んでおいたテクスチャのポインタを渡す
//	newEffect->Start(pos);
//	m_levelUpEffects.push_back(newEffect);
//}

void GameScene::AddLevelUpEffect(Math::Vector2 pos) {
	auto newEffect = std::make_shared<LevelUp>();

	// Initにテクスチャ(&m_levelUpTex)と、プレイヤーのポインタ(&m_player)の両方を渡す
	newEffect->Init(&m_levelUpTex, &m_player);

	newEffect->Start(pos);
	m_levelUpEffects.push_back(newEffect);
}

void GameScene::Release()
{}
