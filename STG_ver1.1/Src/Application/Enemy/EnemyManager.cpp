#include "EnemyManager.h"


#include <algorithm> // std::remove_if を使うために必要


void C_EnemyManager::Update(const Math::Vector2& pPos, const Math::Vector2& pVel, GameScene* owner, float timeScale)
{
    // --- 1. 追尾型エネミー(Enemy)の更新と削除 ---
    for (auto& enemy : m_enemies) {
        // 先ほど作成した「ゆっくり追尾するUpdate」を呼び出す
        // 引数にプレイヤーの座標とタイムスケールを渡す
        enemy->Update(pPos, timeScale);
    }

    // 死んだ敵(m_alive == false)を配列から削除
    m_enemies.erase(std::remove_if(m_enemies.begin(), m_enemies.end(),
        [](const std::shared_ptr<C_Enemy>& e) { return !e->IsAlive(); }), m_enemies.end());


    // --- 2. 砲台(Turret)の更新と削除 ---
    for (auto& turret : m_turrets) {
        // 砲台側にもスローを適用させたい場合は、引数に timeScale を追加して渡す
        turret->Update(pPos, pVel, owner);
    }

    // 死んだ砲台を配列から削除
    m_turrets.erase(std::remove_if(m_turrets.begin(), m_turrets.end(),
        [](const std::shared_ptr<C_EnemyTurret>& t) { return !t->IsAlive(); }), m_turrets.end());

    // --- 3. ボス(Boss)の更新と削除 --- ★追加
    if (m_boss) {
        m_boss->Update(pPos, pVel, owner);

        // ボスが倒されたらポインタをクリアする
        if (!m_boss->IsAlive()) {
            m_boss = nullptr;
        }
    }
}

void C_EnemyManager::Draw(KdTexture* enemyTex, KdTexture* turretTex)
{
    // 全ての敵を描画
    for (auto& enemy : m_enemies) {
        enemy->Draw(enemyTex);
    }

    // 全ての砲台を描画
    for (auto& turret : m_turrets) {
        turret->Draw(turretTex);
    }

    // ボスの描画 ★追加
    if (m_boss) {
        // ボスは砲台(Turret)をベースにしているので、turretTex を使用
        m_boss->Draw(turretTex);
    }
}

void C_EnemyManager::SpawnEnemy(Math::Vector2 basePos)
{
    // 1. 出現位置を計算
    float spawnY = (rand() % 2 == 0) ? 250.0f : -250.0f;
    float spawnX = 850.0f;

    // 2. 敵のインスタンスを作成
    std::shared_ptr<C_Enemy> newEnemy = std::make_shared<C_Enemy>();

    // 3. 初期化（thisを渡して管理者を覚えさせる）
    newEnemy->Init(Math::Vector2(spawnX, spawnY), this);

    // 4. マネージャーが持っているリストに追加
    // ※リスト名が m_enemies だと仮定しています
    m_enemies.push_back(newEnemy);
}


// C_EnemyManager.cpp 内（再掲・微調整）
void C_EnemyManager::SpawnBowlingFormationTurret(Math::Vector2 basePos)
{
    int formation[] = { 7, 5, 3, 2, 1 };
    float offsetX = 80.0f; // 列同士の間隔
    float offsetY = 70.0f;

    for (int col = 0; col < 5; col++)
    {
        int enemyCount = formation[col];
        float startY = (enemyCount - 1) * offsetY * 0.5f;

        for (int row = 0; row < enemyCount; row++)
        {
            Math::Vector2 spawnPos;
            spawnPos.x = basePos.x + (col * offsetX);
            spawnPos.y = basePos.y + (startY - (row * offsetY));

            auto newTurret = std::make_shared<C_EnemyTurret>();

            // ★ ここが重要！
            // 1列目(col=0)は 200 で止まり、2列目(col=1)は 280 で止まる...という計算です。
            float finalStopX = 200.0f + (col * offsetX);

            newTurret->Init(spawnPos, finalStopX); // 計算した停止位置を渡す
            m_turrets.push_back(newTurret);
        }
    }
}

// 上または下に1列（5体など）並べる関数
void C_EnemyManager::SpawnVerticalLine(float spawnY)
{
    float spawnX = 850.0f;
    float gapY = 55.0f;
    int count = 5;
    float startY = spawnY + ((count - 1) * gapY * 0.5f);

    for (int i = 0; i < count; i++)
    {
        auto newTurret = std::make_shared<C_EnemyTurret>();
        Math::Vector2 pos = { spawnX, startY - (i * gapY) };

        // 350px地点で止まるように設定
        newTurret->Init(pos, 350.0f);
        m_turrets.push_back(newTurret);
    }
}

void C_EnemyManager::SpawnSandwichFormation()
{
    int countPerLine = 4;    // 片側4体ずつ
    float offsetX = 80.0f;   // 横の間隔
    float startX = 850.0f;   // 出現位置（右端）
    float stopX_Base = 450.0f; // 停止位置

    // 上の列と下の列を同時に生成
    for (int i = 0; i < countPerLine; i++)
    {
        float currentX = startX + (i * offsetX);
        float currentStopX = stopX_Base + (i * offsetX);

        // --- 上の列 (Y = 180) ---
        auto upperTurret = std::make_shared<C_EnemyTurret>();
        upperTurret->Init(Math::Vector2(currentX, 180.0f), currentStopX);
        m_turrets.push_back(upperTurret);

        // --- 下の列 (Y = -180) ---
        auto lowerTurret = std::make_shared<C_EnemyTurret>();
        lowerTurret->Init(Math::Vector2(currentX, -180.0f), currentStopX);
        m_turrets.push_back(lowerTurret);
    }
}

void C_EnemyManager::SpawnReverseVFormation()
{
    // 各列の敵の数（1体 → 3体 → 5体）
    int formation[] = { 1, 3, 5 };

    float offsetX = 80.0f; // 列同士の横の間隔
    float offsetY = 70.0f; // 敵同士の縦の間隔
    float startX = 850.0f; // 出現位置

    for (int col = 0; col < 3; col++)
    {
        int enemyCount = formation[col];

        // その列を上下中央揃えにするための開始Y地点を計算
        float startY = (enemyCount - 1) * offsetY * 0.5f;

        for (int row = 0; row < enemyCount; row++)
        {
            auto newTurret = std::make_shared<C_EnemyTurret>();

            // 座標計算
            Math::Vector2 spawnPos;
            spawnPos.x = startX + (col * offsetX);
            spawnPos.y = 0.0f + (startY - (row * offsetY));

            // 停止位置の計算（1列目が200、2列目が280...とずらして整列させる）
            float stopX = 200.0f + (col * offsetX);

            newTurret->Init(spawnPos, stopX);
            m_turrets.push_back(newTurret);
        }
    }
}

void C_EnemyManager::SpawnDiamondFormation()
{
    // 各列の敵の数（1 -> 3 -> 5 -> 3 -> 1）
    int formation[] = { 1, 3, 5, 3, 1 };

    float offsetX = 70.0f; // 列同士の横の間隔
    float offsetY = 64.0f; // 敵同士の縦の間隔
    float startX = 850.0f; // 出現位置（右端）

    for (int col = 0; col < 5; col++)
    {
        int enemyCount = formation[col];

        // その列を上下中央揃えにするための開始Y地点を計算
        float startY = (enemyCount - 1) * offsetY * 0.5f;

        for (int row = 0; row < enemyCount; row++)
        {
            auto newTurret = std::make_shared<C_EnemyTurret>();

            // 座標計算
            Math::Vector2 spawnPos;
            spawnPos.x = startX + (col * offsetX);
            spawnPos.y = 0.0f + (startY - (row * offsetY));

            // 停止位置の計算
            // 1列目(col=0)を200地点として、陣形の形を保って整列
            float stopX = 200.0f + (col * offsetX);

            newTurret->Init(spawnPos, stopX);
            m_turrets.push_back(newTurret);
        }
    }
}

void C_EnemyManager::ClearAllEnemies() {
    m_turrets.clear();
    // 他の敵リストがあればそれもclear
}

void C_EnemyManager::AddTurret(Math::Vector2 pos, float stopX)
{
    // shared_ptrを作成してリストに追加
    auto newTurret = std::make_shared<C_EnemyTurret>();
    newTurret->Init(pos, stopX);
    m_turrets.push_back(newTurret);
}