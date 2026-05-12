#include "Bullet.h"
#include "Application/Player/player.h" // ここに定義があるか確認

void C_Bullet::Init(Math::Vector2 pos, float angle)
{
    m_pos = pos;
    float speed = 10.0f; // 弾の速さ

    // float bulletAngle = angle + DirectX::XM_PIDIV2;

    // 自機の画像補正で angle をいじっている場合、
    // ここで弾の飛ぶ方向も合わせる必要があります。
    // もし弾が変な方向に飛ぶなら、ここの angle にも 
    // 自機と同じオフセット（DirectX::XM_PIDIV2など）を足してください。
    m_move.x = cosf(angle) * speed;
    m_move.y = sinf(angle) * speed;

    m_alive = true;
    // ... 既存の初期化 ...
    m_isDodged = false; // 最初は必ず false

    // ★ 生成した瞬間の向き（右向きなら0度、左向きなら180度）をセット
    m_angle = atan2f(m_move.y, m_move.x);
}

//void C_Bullet::Update()
//{
//    if (!m_alive) return;
//
//    bool isReflected = false;
//
//    // 1. 移動を反映
//    m_pos += m_move;
//
//    // --- ★ここから：上下の壁での反射処理 ---
//    // 縦の範囲を少し抑える（例：±300）
//    float limitY = 275.0f;
//
//    if (m_pos.y > limitY) {
//        m_pos.y = limitY;    // 埋まらないように位置を固定
//        m_move.y *= -1.0f;   // Y方向の移動量を反転！
//        isReflected = true;
//    }
//    else if (m_pos.y < -limitY) {
//        m_pos.y = -limitY;   // 埋まらないように位置を固定
//        m_move.y *= -1.0f;   // Y方向の移動量を反転！
//        isReflected = true;
//    }
//
//    // ★反射した場合の処理
//    if (isReflected) {
//        AddReflectCount(); // 反射回数を＋1
//
//        // 「2回反射した後、次に壁に当たったら消える」＝ 3回目の接触で消去
//        if (GetReflectCount() >= 3) {
//            SetAlive(false);
//        }
//    }
//
//    // ----------------------------------------
//
//    // 2. 左右の画面外に出たら消す（壁は上下だけ、左右は消滅させるのが一般的）
//    if (m_pos.x < -700 || m_pos.x > 700)
//    {
//        m_alive = false;
//    }
//
//
//}

void C_Bullet::Update(float timeScale)
{
    if (!m_alive) return;

    bool isReflected = false;

    // 1. 移動を反映
// 移動量にタイムスケールを掛ける（スローなら 0.3 倍になる）
    m_pos += m_move * timeScale;
    // --- 上下の壁での反射処理 ---
    float limitY = 275.0f;

    if (m_pos.y > limitY) {
        m_pos.y = limitY;
        m_move.y *= -1.0f;
        isReflected = true;
    }
    else if (m_pos.y < -limitY) {
        m_pos.y = -limitY;
        m_move.y *= -1.0f;
        isReflected = true;
    }

    // ★反射した場合の追加処理
    if (isReflected) {
        AddReflectCount();

        // --- 角度を今の移動量(m_move)に合わせて再計算 ---
        // これで、跳ね返った瞬間に画像がグイッと進行方向を向きます
        m_angle = atan2f(m_move.y, m_move.x);

        if (GetReflectCount() >= 3) {
            SetAlive(false);
        }
    }

    // 2. 左右の画面外に出たら消す
    if (m_pos.x < -700 || m_pos.x > 700)
    {
        m_alive = false;
    }
}


void C_Bullet::Draw(KdTexture* tex)
{
    if (!m_alive || !tex) return;

    // --- 1. 行列の組み立て ---
    // scaleX = -1.0f などの判定は不要です！ 
    // m_angle（回転）がすべて解決してくれます。

    // 回転行列（現在の進行方向を向く）
    Math::Matrix rotMat = Math::Matrix::CreateRotationZ(m_angle);

    // 移動行列
    Math::Matrix transMat = Math::Matrix::CreateTranslation(m_pos.x, m_pos.y, 0);

    // 合成：回転してから移動
    Math::Matrix mat = rotMat * transMat;

    // --- 2. 実行 ---
    SHADER.m_spriteShader.SetMatrix(mat);

    Math::Rectangle srcRect = { 0, 0, 64, 36 };
    SHADER.m_spriteShader.DrawTex(tex, 0, 0, &srcRect);

    // 後片付け
    SHADER.m_spriteShader.SetMatrix(Math::Matrix::Identity);
}