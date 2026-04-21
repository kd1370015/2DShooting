//#include "main.h"
//#include "Scene.h"
//
//void Scene::Draw2D()
//{
//	// 文字列表示
//	SHADER.m_spriteShader.DrawString(0, 0, "Hello World", Math::Vector4(1, 1, 0, 1));
//}
//
//void Scene::Update()
//{
//	
//}
//
//void Scene::Init()
//{
//	// 画像の読み込み処理
//	charaTex.Load("player.png");
//}
//
//void Scene::Release()
//{
//	// 画像の解放処理
//	charaTex.Release();
//}
//
//void Scene::ImGuiUpdate()
//{
//	return;
//
//	ImGui::SetNextWindowPos(ImVec2(20, 20), ImGuiSetCond_Once);
//	ImGui::SetNextWindowSize(ImVec2(200, 100), ImGuiSetCond_Once);
//
//	// デバッグウィンドウ
//	if (ImGui::Begin("Debug Window"))
//	{
//		ImGui::Text("FPS : %d", APP.m_fps);
//	}
//	ImGui::End();
//}

#include "main.h"
#include "Scene.h"

//Scene.cpp

void Scene::Draw2D()
{
	//m_map.Draw();
	m_player.Draw();
	//m_enemy.Draw();

	// 弾の描画
	for (auto b : m_bullets) {
		b->Draw(&m_bulletTex);
	}


}

void Scene::Update()
{
	m_player.Action();
	//m_enemy.Action();

	//m_hit.MapPlayer();
	//m_hit.MapEnemy();
	//m_hit.EnemyPlayer();

	m_player.Update();

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

	//m_enemy.Update(m_player.GetScrollX());
	//m_map.Update(m_player.GetScrollX());

}

void Scene::Init()
{

	//m_hit.SetOwner(this);
	m_player.SetOwner(this);

	//m_map.LoadMap();


	//m_mapTex.Load("Texture/Map/MapChip.png");
	//m_map.Init();
	//m_map.SetTex(&m_mapTex);

	m_playerTex.Load("Texture/Player/Player.png");
	m_player.Init();
	m_player.SetTex(&m_playerTex);

	//m_enemyTex.Load("Texture/Player/Chara.png");
	//m_enemy.Init();
	//m_enemy.SetTex(&m_enemyTex);

	// 弾のテクスチャ読み込み（ファイル名は用意したものに変えてください）
	m_bulletTex.Load("Texture/Bullet/bullet.png");


}

// 弾を追加する処理
void Scene::AddBullet(Math::Vector2 pos, float angle) {
	C_Bullet* newBullet = new C_Bullet();
	newBullet->Init(pos, angle);
	m_bullets.push_back(newBullet);
}

void Scene::Release()
{
	m_playerTex.Release();
	//m_enemyTex.Release();
	//m_mapTex.Release();

	// 残っている弾を掃除
	for (auto b : m_bullets) delete b;
	m_bullets.clear();
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



