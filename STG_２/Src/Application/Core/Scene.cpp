#include "main.h"
#include "Scene.h"

//Scene.cpp

void Scene::Draw2D()
{

}

void Scene::Update()
{

}

void Scene::Init()
{

	


}

// 弾を追加する処理
//void Scene::AddBullet(Math::Vector2 pos, float angle) {
//	C_Bullet* newBullet = new C_Bullet();
//	newBullet->Init(pos, angle);
//	m_bullets.push_back(newBullet);
//}
//
//void Scene::AddEnemyBullet(Math::Vector2 pos, float angle) {
//	C_Bullet* newBullet = new C_Bullet();
//	newBullet->Init(pos, angle);
//	// 敵の弾だとわかるように、色を変えたり速度を遅くしたりするとベスト
//	m_enemyBullets.push_back(newBullet);
//}
//
//void Scene::AddOrb(Math::Vector2 pos) {
//	C_Orb* newOrb = new C_Orb();
//
//	// ランダムで種類を決定
//	OrbType type = static_cast<OrbType>(rand() % 3);
//
//	newOrb->Init(pos, type);
//	m_orbs.push_back(newOrb);
//}
//
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



