#include "GameScene.h"
#include "Application/System/hit.h"

void GameScene::Init()
{
	m_back.Init(0);
	m_mouse.Init();
	m_player.SetOwner(this); // ※後述の注意点参照

	m_playerTex.Load("Texture/Player/Player.png");
	m_player.Init();
	m_player.SetTex(&m_playerTex);

	m_hpTex.Load("Texture/VFX/hp.png"); // パスは環境に合わせてください

	// 敵の「見た目」だけ先にロードしておく
	m_enemyTex.Load("Texture/Enemy/Hamu.png");
	// 砲台の画像をロード（ファイル名は用意したものに変えてください）
	m_turretTex.Load("Texture/Enemy/turret.png");

	// スポーンタイマー初期化
	m_spawnTimer = 60;

	// 弾のテクスチャ読み込み（ファイル名は用意したものに変えてください）
	m_bulletTex.Load("Texture/Bullet/bullet.png");
	// ★敵の弾（ファイル名は用意したものに変えてください）
	m_enemyBulletTex.Load("Texture/Enemy/turret.png");

	m_texOrbBlue.Load("Texture/Item/blue.png"); // など
	m_texOrbRed.Load("Texture/Item/red.png");
	m_texOrbYellow.Load("Texture/Item/yellow.png");

	m_totalScrollX = 0.0f; // リセット

	// スコアの初期化
	m_score.Init();

}

void GameScene::Update()
{
	// 1. スクロール・プレイヤー更新
	m_totalScrollX += 2.0f;
	m_back.Update(m_totalScrollX);
	m_mouse.Update();
	m_player.Action();
	m_player.Update();

	// --- 敵の自動生成 ---
	if (--m_spawnTimer <= 0)
	{
		// ★ ここに書く！
		const float kSpawnPositionX = 800.0f;
		const float kScreenRangeY = 300.0f;

		// Y座標を -300 ～ 300 の間でランダムに決める計算
		float spawnY = (float)(rand() % ((int)kScreenRangeY * 2) - (int)kScreenRangeY);

		if (rand() % 2 == 0) {
			// 砲台を生成
			m_enemyManager.AddTurret(Math::Vector2(kSpawnPositionX, spawnY));
		}
		else {
			// 通常の敵を「群れ」で生成
			m_enemyManager.SpawnWave(Math::Vector2(kSpawnPositionX, spawnY));
		}

		m_spawnTimer = 300; // 次の出現まで約2秒待機
	}

	// 2. EnemyManagerの一括更新
	// プレイヤーの座標だけでなく、偏差射撃用に速度(GetMove等)も渡す
	m_enemyManager.Update(m_player.GetPos(), m_player.GetMove(), this);

	// 当たり判定実行
// 第3引数に this を渡すことで、Hit側が「自分(GameScene)」にアクセスできるようになる
	C_Hit::CheckBulletToEnemy(m_bullets, m_enemyManager.GetEnemies(), this);
	C_Hit::CheckBulletToTurret(m_bullets, m_enemyManager.GetTurrets(), this);

	C_Hit::CheckPlayerToEnemy(m_player, m_enemyManager.GetEnemies());
	C_Hit::CheckPlayerToEnemyBullet(m_player, m_enemyBullets);

	// 弾の更新
	for (auto it = m_bullets.begin(); it != m_bullets.end(); ) {
		(*it)->Update();

		if (!(*it)->IsAlive()) {
			delete (*it);             // メモリ解放
			it = m_bullets.erase(it); // リストから削除
		}
		else {
			++it;
		}
	}

	// --- ★追加：敵の弾の更新 ---
	for (auto it = m_enemyBullets.begin(); it != m_enemyBullets.end(); ) {
		(*it)->Update();

		// 画面外に出た、またはプレイヤーに当たった弾を削除
		if (!(*it)->IsAlive()) {
			delete (*it);
			it = m_enemyBullets.erase(it);
		}
		else {
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




}

void GameScene::DrawSprite()
{

	m_back.Draw();
	//m_map.Draw();


	SHADER.m_spriteShader.SetMatrix(Math::Matrix::Identity);
	m_player.Draw();
	//m_enemy.Draw();

	// Managerにテクスチャを渡して一括描画
	m_enemyManager.Draw(&m_enemyTex, &m_turretTex);


	// 弾の描画
	for (auto b : m_bullets) {
		b->Draw(&m_bulletTex);
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




	// --- HPバーを横長に大きくする ---
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
	m_score.DrawSprite();


	m_mouse.Draw(); // クロスヘアを一番手前に描画

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
	// 敵の弾だとわかるように、色を変えたり速度を遅くしたりするとベスト
	m_enemyBullets.push_back(newBullet);
}

void GameScene::AddOrb(Math::Vector2 pos) {
	C_Orb* newOrb = new C_Orb();

	// ランダムで種類を決定
	OrbType type = static_cast<OrbType>(rand() % 3);

	newOrb->Init(pos, type);
	m_orbs.push_back(newOrb);
}


void GameScene::Release()
{}
