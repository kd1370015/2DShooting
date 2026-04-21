#pragma once
#include "Application/Core/main.h"




class C_Mouse {
public:
    C_Mouse() {}
    ~C_Mouse() {}

    void Init();
    void Update();
    void Draw();

    // 他のクラスからマウス座標を使いたい時に呼ぶ
    const Math::Vector2& GetPos() const { return m_pos; }

private:
    KdTexture m_tex;
    Math::Vector2 m_pos; // 補正後のゲーム内座標
};
