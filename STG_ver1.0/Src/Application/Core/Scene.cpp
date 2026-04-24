#include "main.h"
#include "Scene.h"

//Scene.cpp

void Scene::Draw2D()
{
	m_back.Draw();
	//m_map.Draw();
	m_player.Draw();
	//m_enemy.Draw();


	// 敵の描画
	for (auto e : m_enemies) {
		e->Draw(&m_enemyTex);
	}

	// 弾の描画
	for (auto b : m_bullets) {
		b->Draw(&m_bulletTex);
	}


	// --- 砲台の描画 ---
	for (auto t : m_turrets) {
		// 既存の m_enemyTex ではなく、専用の m_turretTex を渡す
		t->Draw(&m_turretTex);
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


	m_mouse.Draw(); // クロスヘアを一番手前に描画

}

void Scene::Update()
{
	// 1. 自動スクロールの速度を決める (例: 1フレームに2ピクセル進む)
	float autoScrollSpeed = 2.0f;

	// 2. 累積スクロール量に加算し続ける
	m_totalScrollX += autoScrollSpeed;

	// 3. 背景の更新に渡す
	m_back.Update(m_totalScrollX);

	m_mouse.Update(); // マウスの更新
	m_player.Action();
	//m_enemy.Action();

	//m_hit.MapPlayer();
	//m_hit.MapEnemy();
	//m_hit.EnemyPlayer();

	m_player.Update();

	// --- 1. 敵の自動生成 ---
	if (--m_spawnTimer <= 0) {
		// 画面の右端などのランダムな位置に出現させる
		float spawnX = 800.0f;
		float spawnY = (float)(rand() % 800 - 400); // Y座標をランダムに

		C_Enemy* newEnemy = new C_Enemy();
		newEnemy->Init(Math::Vector2(spawnX, spawnY));
		m_enemies.push_back(newEnemy);

		m_spawnTimer = 60; // 60フレーム（約1秒）に1体
	}

	// --- 2. 敵の更新 ---
	Math::Vector2 pPos = m_player.GetPos();
	for (auto it = m_enemies.begin(); it != m_enemies.end(); ) {
		(*it)->Update(pPos);

		if (!(*it)->IsAlive()) {
			delete (*it);
			it = m_enemies.erase(it);
		}
		else {
			++it;
		}
	}


	// 当たり判定実行
	C_Hit::CheckBulletToEnemy(m_bullets, m_enemies);

	// 2. プレイヤーと敵の当たり判定 ★追加
	C_Hit::CheckPlayerToEnemy(m_player, m_enemies);

	// Scene.cpp の Update() 内
	if (--m_spawnTimer <= 0) {
		// 50%の確率で砲台、50%で通常の敵を出す例
		if (rand() % 2 == 0) {
			C_EnemyTurret* newTurret = new C_EnemyTurret();
			// 画面右端の少し外から登場させる
			newTurret->Init(Math::Vector2(800.0f, (float)(rand() % 600 - 300)));
			m_turrets.push_back(newTurret);
		}
		else {
			// 通常の敵の生成（既存の処理）
			C_Enemy* newEnemy = new C_Enemy();
			newEnemy->Init(Math::Vector2(800.0f, (float)(rand() % 800 - 400)));
			m_enemies.push_back(newEnemy);
		}
		m_spawnTimer = 60;
	}


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

	// Scene.cpp の Update 内
	for (auto it = m_turrets.begin(); it != m_turrets.end(); ) {
		(*it)->Update(m_player.GetPos());
		if (!(*it)->IsAlive()) {
			delete* it;
			it = m_turrets.erase(it);
		}
		else {
			++it;
		}
	}

	//m_enemy.Update(m_player.GetScrollX());
	//m_map.Update(m_player.GetScrollX());

}

void Scene::Init()
{
	m_back.Init(0);
	m_mouse.Init();

	//m_hit.SetOwner(this);
	m_player.SetOwner(this);

	//m_map.LoadMap();


	//m_mapTex.Load("Texture/Map/MapChip.png");
	//m_map.Init();
	//m_map.SetTex(&m_mapTex);

	m_playerTex.Load("Texture/Player/Player.png");
	m_player.Init();
	m_player.SetTex(&m_playerTex);

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

	m_texOrbBlue.	Load("Texture/Item/blue.png"); // など
	m_texOrbRed.	Load("Texture/Item/red.png");
	m_texOrbYellow. Load("Texture/Item/yellow.png");

	m_totalScrollX = 0.0f; // リセット

	


}

// 弾を追加する処理
void Scene::AddBullet(Math::Vector2 pos, float angle) {
	C_Bullet* newBullet = new C_Bullet();
	newBullet->Init(pos, angle);
	m_bullets.push_back(newBullet);
}

void Scene::AddEnemyBullet(Math::Vector2 pos, float angle) {
	C_Bullet* newBullet = new C_Bullet();
	newBullet->Init(pos, angle);
	// 敵の弾だとわかるように、色を変えたり速度を遅くしたりするとベスト
	m_enemyBullets.push_back(newBullet);
}

void Scene::AddOrb(Math::Vector2 pos) {
	C_Orb* newOrb = new C_Orb();

	// ランダムで種類を決定
	OrbType type = static_cast<OrbType>(rand() % 3);

	newOrb->Init(pos, type);
	m_orbs.push_back(newOrb);
}

void Scene::Release()
{
	m_playerTex.Release();
	//m_enemyTex.Release();
	//m_mapTex.Release();

	// 残っている弾を掃除
	for (auto b : m_bullets) delete b;
	m_bullets.clear();

	m_enemies.erase(std::remove_if(m_enemies.begin(), m_enemies.end(),
		[](C_Enemy* e) { return !e->IsAlive(); }), m_enemies.end());
}

void Scene::ImGuiUpdate()
{
	return;

	ImGui::SetNextWindowPos(ImVec2(20, 20), ImGuiSetCond_Once);
	ImGui::SetNextWindowSize(ImVec2(200, 100), ImGuiSetCond_Once);

	// デバッグウィンドウ
	if (ImGui::Begin("Debug Window"))
	{
		ImGui::Text("FPS : %d", APP.m_fps);
	}
	ImGui::End();
}



