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

	//m_hpTex.Load("Texture/VFX/hp.png"); // パスは環境に合わせてください

	// HPバー用テクスチャの読み込み
	// ファイル名は実際の画像名に合わせてください
	m_hpGaugeTex.Load("Texture/VFX/Hpbar.png");
	m_hpFrameTex.Load("Texture/VFX/Hpbar_frametrans.png");

	m_bosshpTex.Load("Texture/VFX/charge_barlow.png");
	m_bosshpFrameTex.Load("Texture/VFX/Hpbar_frametrans.png");

	m_playerIconTex.Load("Texture/VFX/icon4.png");

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
	m_eFollowBulletTex.Load("Texture/Bullet/23.png");
	// ※ファイルパスは自分のプロジェクトのフォルダ構成に合わせてください
	m_bossBulletTex.Load("Texture/Bullet/boss_bullet.png");


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

	//m_chargeIconTex.Load("Texture/VFX/chargeIcon3.png"); // パスは環境に合わせて
	m_levelUpTex.Load("Texture/VFX/lvlup.png");

	static KdTexture tex;
	tex.Load("Texture/VFX/expl.png");
	m_texExplosion = &tex;

	static KdTexture ftex;
	ftex.Load("Texture/VFX/f_expl.png");
	m_texFExplosion = &ftex;

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

	// 2. チャージ完了の瞬間判定
	float currentCharge = m_player.GetCharge();
	if (currentCharge >= 100.0f && m_prevCharge < 100.0f)
	{
		AddLevelUpEffect(); // 引数は内部で同期するので不要になりました
	}
	m_prevCharge = currentCharge;

	// 3. 全てのエフェクトの更新と停止管理
	auto itLvl = m_levelUpEffects.begin();
	while (itLvl != m_levelUpEffects.end())
	{
		// ★修正ポイント
		if (isPoweredUp) {
			// パワーアップ中はループを維持（必要なら SetLoop(true) など）
		}
		else {
			// パワーアップが終わったら Stop() を呼んでアニメーションを終了させる
			(*itLvl)->Stop();
		}

		// 更新（内部でプレイヤーの最新座標に追従する）
		(*itLvl)->Update();

		// Stop() が呼ばれ、アニメーションが最後まで再生されたらリストから削除
		if (!(*itLvl)->IsActive()) {
			itLvl = m_levelUpEffects.erase(itLvl);
		}
		else {
			++itLvl;
		}
	}

	// '1'～'6'キーで各ステップへジャンプ
	if (GetAsyncKeyState('1') & 0x8000) m_waveStep = 0;
	if (GetAsyncKeyState('2') & 0x8000) m_waveStep = 1;
	if (GetAsyncKeyState('3') & 0x8000) m_waveStep = 2;
	if (GetAsyncKeyState('4') & 0x8000) m_waveStep = 3;
	if (GetAsyncKeyState('5') & 0x8000) m_waveStep = 4;
	if (GetAsyncKeyState('6') & 0x8000) m_waveStep = 5; // ボス直前

	// 'B'キーで一気にボスを出す（全滅させてから進める）
	if (GetAsyncKeyState('B') & 0x8000) {
		m_enemyManager.ClearAllEnemies(); // 全ての敵を消す関数（未実装なら追加）
		m_waveStep = 5;
	}







	// 例：一定時間ごとに敵を出す
	// --- A. タイマー更新の共通フラグ ---
	// スロー中は3フレームに1回しかタイマーが進まないようにする
	bool isUpdateFrame = (m_slowTimer <= 0 || (m_slowTimer % 3 == 0));

	if (isUpdateFrame)
	{

		if (m_enemyManager.GetTurretCount() == 0 && !m_isBossSpawned && !m_isBossBattle)
		{

			switch (m_waveStep)
			{
			case 0: // 第1ウェーブ
				m_enemyManager.SpawnVerticalLine(100.0f);
				m_waveStep++;
				break;

			case 1: // 第2ウェーブ
				m_enemyManager.SpawnVerticalLine(-150.0f);
				m_waveStep++;
				break;

			case 2: // 追加：斜め陣形など
				//m_enemyManager.SpawnSandwichFormation();
				m_enemyManager.SpawnReverseVFormation();
				m_waveStep++;
				break;

			case 3: // 追加：V字陣形（逆向き）など
				//m_enemyManager.SpawnReverseVFormation();
				m_enemyManager.SpawnDiamondFormation();
				m_waveStep++;
				break;

			case 4: // 第5ウェーブ：ボウリング陣形
				m_enemyManager.SpawnBowlingFormationTurret(Math::Vector2(850.0f, 0.0f));
				m_waveStep++;
				break;

			case 5: // 最後にボス出現
				m_enemyManager.SpawnBoss(Math::Vector2(900.0f, 0.0f), 400.0f);
				m_isBossSpawned = true;
				m_isBossBattle = true;
				break;
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



	m_enemyManager.Update(m_player.GetPos(), m_player.GetMove(), this, m_timeScale);





	// --- GameScene.cpp (クリア判定) ---
	//if (m_bossSpawned && m_isBossBattle) 
		if (m_isBossSpawned && m_isBossBattle)

		{	
		if (m_enemyManager.GetBoss() == nullptr) {
			static int clearTimer = 120;
			if (--clearTimer <= 0) {
				// 1. SceneManagerにスコアを預ける
				SceneManager::GetInstance().SetFinalScore(m_score.GetScore());

				// 2. シーンを切り替える
				SceneManager::GetInstance().SetNextScene(SceneManager::SceneType::Result);
			}
			}
		}

		// プレイヤーが死んだ（HPが0以下になった）時の判定
		if (m_player.GetHp() <= 0)
		{
			// 1. その時点までのスコアを SceneManager に預ける
			// これにより、リザルト画面で「死ぬ直前までのスコア」が表示されます
			SceneManager::GetInstance().SetFinalScore(m_score.GetScore());

			// 2. リザルトシーンへ遷移
			SceneManager::GetInstance().SetNextScene(SceneManager::SceneType::Result);
		}

	// 当たり判定実行
	// 第3引数に this を渡すことで、Hit側が「自分(GameScene)」にアクセスできるようになる
	C_Hit::CheckBulletToEnemy(m_bullets, m_enemyManager.GetEnemies(), this);
	C_Hit::CheckBulletToTurret(m_bullets, m_enemyManager.GetTurrets(), this);

	C_Hit::CheckPlayerToEnemy(m_player, m_enemyManager.GetEnemies());
	C_Hit::CheckPlayerToEnemyBullet(m_player, m_enemyBullets);
	// ★ボスの弾とプレイヤーの当たり判定を実行
	C_Hit::CheckPlayerToBossBullet(m_player, m_bossBullets);

	// ★追加：ボスがいる時だけ判定を行う
	if (m_enemyManager.GetBoss()) {
		C_Hit::CheckBulletToBoss(m_bullets, m_enemyManager.GetBoss(), this);
	}


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

			// A. 反射弾の生成（真っ直ぐ1発のみ）
			if (m_player.CanShot()) {
				Math::Vector2 spawnPos = e->GetPos();  // 敵の位置
				float finalAngle = m_player.GetAngle(); // プレイヤーが狙っている角度

				// ★ ループを削除し、1発だけ生成する
				C_Bullet* newBullet = new C_Bullet();

				// Initの第3引数を true にして高速化を適用
				newBullet->Init(spawnPos, finalAngle, true);

				m_bullets.push_back(newBullet);

				// 1回のパリィで1発にするためのフラグ管理
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
				/*AddEffect(m_player.GetPos(), 2.0f);*/
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
				//AddEffect(m_player.GetPos(), 2.0f);
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

////--- ★追加：ボスの弾の更新 ＆ パリィ・回避スロー・ダメージ判定 ---
for (auto it = m_bossBullets.begin(); it != m_bossBullets.end(); ) {
	C_BossBullet* bb = (*it);

	// 1. 弾の移動（ボス弾クラスのUpdateにスロー対応があれば m_timeScale を渡す）
	// もし引数なしのUpdateなら bb->Update(); 
	bb->Update();

	// 2. プレイヤーと弾の距離を計算
	float dist = Math::Vector2::Distance(m_player.GetPos(), bb->GetPos());

	// --- A. パリィ判定（ボスの弾を跳ね返す！） ---
	// ボス弾は大きいので判定距離を少し広め(例: +10.0f)に設定
	if (m_player.IsParrying() && dist < (m_player.GetParryRadius() + 10.0f)) {
		if (m_player.CanShot()) {
			Math::Vector2 bulletPos = bb->GetPos();
			float baseAngle = m_player.GetAngle();
			int count = m_player.GetShotCount();

			// プレイヤーの反射弾として生成
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
		delete bb;
		it = m_bossBullets.erase(it);
		continue;
	}

	// --- B. 回避（チャージ ＆ スロー）判定 ---
	if (m_player.IsDodging() && dist < 25.0f) {
		// クラスに追加したフラグをチェック
		if (!bb->IsDodged()) {
			// エネルギーチャージ
			if (m_player.CanDodgeCharge()) {
				m_player.ChargeEnergy(25.0f);
				m_player.SetDodgeChargeDone();
			}

			// スロー発動
			if (m_slowTimer <= 0) {
				m_slowTimer = 30;
			}

			// ★この弾は判定済み！とする
			bb->SetDodged(true);
		}
	}


	// --- C. 被弾（HP減少）判定 ---
	// ボス弾は大きいので当たり判定も少し大きく(例: 12.0f)
	if (dist < 12.0f) {
		if (m_player.IsDodging()) {
			// 回避中ならスルー
		}
		else {
			m_player.DecreaseHp(1); // ボス弾なので 2 ダメージにするのもアリ
			delete bb;
			it = m_bossBullets.erase(it);
			continue;
		}
	}

	// 4. 寿命または画面外判定
	if (!bb->IsAlive()) {
		delete bb;
		it = m_bossBullets.erase(it);
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
	

	// --- 新しい爆発（m_explList）の更新と削除 ---
	// イテレータを使って、アニメーションが終わったものをリストから取り除きます
	for (auto it = m_explList.begin(); it != m_explList.end(); ) {

		(*it)->Update(); // 1コマ進める

		if ((*it)->IsExpired()) {
			// アニメーション終了ならリストから削除し、次の要素へ
			it = m_explList.erase(it);
		}
		else {
			// まだ続くなら次の要素へ
			++it;
		}
	}


	
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
		// ★重要：遷移する前に現在のスコアを SceneManager に保存する
		SceneManager::GetInstance().SetFinalScore(m_score.GetScore());

		// リザルト画面へ遷移
		SceneManager::GetInstance().SetNextScene(SceneManager::SceneType::Result);
	}


	if (GetAsyncKeyState('P') & 0x8000) {
		// 押しっぱなしで一気に増えすぎないよう注意が必要ですが、
		// 1万にするだけならこれで十分です。
		m_score.AddScore(10000);
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


	//// 弾の描画
	//for (auto b : m_bullets) {
	//	b->Draw(&m_bulletTex);
	//}

	for (auto& bullet : m_bullets) {
		if (!bullet->IsAlive()) continue;

		// ★ フラグによって渡すテクスチャを変える
		if (bullet->IsReflected()) {
			// パリィで生まれた弾は 23.png (m_eFollowBulletTex)
			bullet->Draw(&m_eFollowBulletTex);
		}
		else {
			// 自機が普通に撃った弾は 11.png (m_bulletTex)
			bullet->Draw(&m_bulletTex);
		}
	}

	// ボスの弾をすべて描画
	for (auto& bullet : m_bossBullets) {
		if (bullet->IsAlive()) {
			// ここでボス専用のテクスチャを渡す！
			bullet->Draw(&m_bossBulletTex);
		}
	}

	// 爆発を描画
	for (auto& exp : m_explosions) {
		exp->Draw(m_texExplosion);
	}

	// 全ての爆発を描画
	for (auto& expl : m_explList) {
		expl->Draw(m_texFExplosion);
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






	// --- HPバーの描画 (チャージゲージ方式) ---
	{
		// 1. HPの割合を計算
		int currentHp = m_player.GetHp();
		int maxHp = 5; // 最大HPが4の場合
		float hpRate = (float)currentHp / (float)maxHp;

		// 2. 描画する横幅を決定
		int drawW = (int)(HP_GAUGE_FULL_W * hpRate);
		if (drawW < 0) drawW = 0;

		// 3. 切り抜き範囲の設定 (左端から drawW 分だけ)
		Math::Rectangle srcRect = { 0, 0, drawW, HP_GAUGE_H };

		// 4. 行列の作成
		float scaleX = 1.0f;
		float scaleY = 1.0f;

		// 左端を固定するための計算 (チャージゲージと同じ理屈)
		// 基準位置を (-420, 320) とした場合
		float basePosX = -389.0f;
		float basePosY = 328.0f;

			// スケールがかかっている分、ズレもスケール倍する
			float offsetX = (drawW - HP_GAUGE_FULL_W) * 0.5f * scaleX;

		Math::Matrix mat = Math::Matrix::CreateScale(scaleX, scaleY, 1.0f) *
			Math::Matrix::CreateTranslation(basePosX + offsetX, basePosY, 0);

		// 5. 描画
		SHADER.m_spriteShader.SetMatrix(mat);

		// 中身の描画
		SHADER.m_spriteShader.DrawTex(&m_hpGaugeTex, 0, 0, &srcRect);

		// 枠の描画 (枠は常に全表示なので srcRect は nullptr)
		// 枠の位置を合わせるため offsetX なしの行列を再セット
		Math::Matrix frameMat = Math::Matrix::CreateScale(scaleX, scaleY, 1.0f) *
			Math::Matrix::CreateTranslation(basePosX, basePosY, 0);
		SHADER.m_spriteShader.SetMatrix(frameMat);
		SHADER.m_spriteShader.DrawTex(&m_hpFrameTex, 0, 0, nullptr);

		SHADER.m_spriteShader.SetMatrix(Math::Matrix::Identity);
	}

	// --- プレイヤーアイコンの描画 ---
	{
		// HPバーの基準位置 (basePosX = -420) の少し左に配置
		float posX = -570.0f;
		float posY = 300.0f;

		float scale = 0.9f; // アイコンのサイズ調整
		Math::Matrix iconMat = Math::Matrix::CreateScale(scale, scale, 1.0f) *
			Math::Matrix::CreateTranslation(posX, posY, 0);

		SHADER.m_spriteShader.SetMatrix(iconMat);
		SHADER.m_spriteShader.DrawTex(&m_playerIconTex, 0, 0, nullptr);

		// 次の描画に影響が出ないようリセット
		SHADER.m_spriteShader.SetMatrix(Math::Matrix::Identity);
	}

	// マネージャーからボスのインスタンスを一時的に取得する
	m_boss = m_enemyManager.GetBoss();

	if (m_boss) // ボスが存在する場合のみ描画
	{
		// 1. HPの割合を取得
		float bossHpRate = m_boss->GetBossHpRate();

		// 2. 描画する横幅を決定
		// HP_GAUGE_FULL_W や HP_GAUGE_H はプレイヤーの時と同じ定数を使用
		int drawW = (int)(HP_GAUGE_FULL_W * bossHpRate);
		if (drawW < 0) drawW = 0;

		// 3. 切り抜き範囲の設定 (左端から drawW 分だけ)
		Math::Rectangle srcRect = { 0, 0, drawW, HP_GAUGE_H };

		// 4. 行列の作成
		float scaleX = 2.0f; // ボスなのでプレイヤーより少し大きく(2倍など)
		float scaleY = 1.0f;

		// 画面上部の中央を基準にする
		float basePosX = 70.0f;
		float basePosY = 300.0f;

		// 左端を固定するための計算（プレイヤーの時と同じ理屈）
		float offsetX = (drawW - HP_GAUGE_FULL_W) * 0.5f * scaleX;

		Math::Matrix mat = Math::Matrix::CreateScale(scaleX, scaleY, 1.0f) *
			Math::Matrix::CreateTranslation(basePosX + offsetX, basePosY, 0);

		// 5. 描画
		SHADER.m_spriteShader.SetMatrix(mat);

		SHADER.m_spriteShader.DrawTex(&m_bosshpTex, 0, 0, &srcRect);

		// 枠の描画
		Math::Matrix frameMat = Math::Matrix::CreateScale(scaleX, scaleY, 1.0f) *
			Math::Matrix::CreateTranslation(basePosX, basePosY, 0);
		SHADER.m_spriteShader.SetMatrix(frameMat);
		SHADER.m_spriteShader.DrawTex(&m_bosshpFrameTex, 0, 0, nullptr);

		SHADER.m_spriteShader.SetMatrix(Math::Matrix::Identity);
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


void GameScene::AddEnemyBullet(Math::Vector2 pos, float angle) {
	C_Bullet* newBullet = new C_Bullet();
	newBullet->Init(pos, angle);

	// ★ ここで「お前は敵の弾だ！」と設定する
	newBullet->SetIsEnemy(true);

	m_enemyBullets.push_back(newBullet);
}

void GameScene::AddBossBullet(Math::Vector2 pos, float angle) {
	C_BossBullet* newBullet = new C_BossBullet();
	newBullet->Init(pos, angle, 8.0f); // 弾速はお好みで
	m_bossBullets.push_back(newBullet);
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

void GameScene::AddFExplosion(Math::Vector2 pos) {
	// 1. F_Explosion クラスのインスタンスを生成
	auto newExpl = std::make_shared<F_Explosion>();

	// 2. 指定された座標で初期化
	newExpl->Init(pos);

	// 3. 管理リストに追加
	m_explList.push_back(newExpl);
}


void GameScene::AddLevelUpEffect() {
	auto newEffect = std::make_shared<LevelUp>();

	newEffect->Init(&m_levelUpTex, &m_player);

	newEffect->Start(); // 引数なしのStartを呼ぶ
	m_levelUpEffects.push_back(newEffect);
}

void GameScene::Release()
{}
