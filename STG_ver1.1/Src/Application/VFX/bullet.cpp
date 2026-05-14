#include "Bullet.h"
#include "Application/Player/player.h" // ここに定義があるか確認


void C_Bullet::Init(Math::Vector2 pos, float angle, bool isReflected)
{
    m_pos = pos;
    m_isReflected = isReflected; // フラグを保存

    // ★ スピードの切り分け
    float speed = 10.0f; // 通常の自機弾の速さ
    if (m_isReflected) {
        speed = 20.0f;   // パリィ反射弾は2倍速く！
    }

    m_move.x = cosf(angle) * speed;
    m_move.y = sinf(angle) * speed;

    m_alive = true;
    m_isDodged = false;
    m_angle = atan2f(m_move.y, m_move.x);
}


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
    // 回転（進行方向）
    Math::Matrix rotMat = Math::Matrix::CreateRotationZ(m_angle);
    // 位置
    Math::Matrix transMat = Math::Matrix::CreateTranslation(m_pos.x, m_pos.y, 0);

    // 合成
    Math::Matrix mat = rotMat * transMat;

    // --- 2. 実行 ---
    SHADER.m_spriteShader.SetMatrix(mat);

    // --- 3. サイズ（切り抜き範囲）の決定 ---
    Math::Rectangle srcRect;

    if (m_isReflected) {
        // ★反射弾（23.png）のピクセルサイズに合わせて数値を書き換えてください
        // 例: 画像が横48px、縦48pxの場合
        srcRect = { 0, 0, 80, 33 };
    }
    else {
        // 通常の自機弾（11.png）のサイズ
        srcRect = { 0, 0, 64, 36 };
    }

    // 第2引数、第3引数を 0 にすることで、画像の中心で回転・描画されます
    SHADER.m_spriteShader.DrawTex(tex, 0, 0, &srcRect);

    // 後片付け
    SHADER.m_spriteShader.SetMatrix(Math::Matrix::Identity);
}