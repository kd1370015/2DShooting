#pragma once

#include "../BaseScene/BaseScene.h"
#include "Option.h"

class TitleScene : public BaseScene
{
public:
    TitleScene() { Init(); }
    ~TitleScene() {}

    void Init()         override;
    void Update()       override;
    void DrawSprite()   override;
    void Release()      override;

private:
    KdTexture m_tex;
    KdTexture m_enterTex;
    KdTexture m_optionTex; // ★追加：Optionボタン画像用
    KdTexture m_exitTex;    // Exit用 (★追加)
	KdTexture m_titleTex;   // タイトルロゴ用 (★追加)
    KdTexture m_pressTex;



    std::unique_ptr<Option> m_option;



    float m_alpha = 0.0f;
    float m_timer = 0.0f;

    float m_bgPosX = 0.0f;

    // --- 追加：マウス操作用 ---
    struct TitleButton {
        Math::Vector2 pos;
        Math::Vector2 size;
        float width;
    };


    TitleButton m_playButton;
    TitleButton m_optionBtn; // ★追加：Optionボタンの座標
    TitleButton m_btnExit;


    int m_selectIdx = 1;   // 0: Play, (1: Exit...)
    bool m_isHover; // マウスが乗っているか
    bool m_isOptionOpen = false; // ★追加：Option画面が開いているか

    bool m_oldSpaceKey = false;
    bool m_oldEnterKey = false;

};