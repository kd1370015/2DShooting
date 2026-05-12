#pragma once

//クラスを継承するときのみインクルード許可！
#include "../BaseScene/BaseScene.h"

// ボタン管理用構造体
struct ClearButton {
	Math::Vector2 pos;
	Math::Vector2 size = { 96.0f, 96.0f };
	KdTexture* normalTex = nullptr;
	// 必要に応じて hoverTex なども追加
};



class ResultScene : public BaseScene
{
public:
	ResultScene() { Init(); }
	~ResultScene(){}

	void Init()			override;
	void Update()		override;
	void DrawSprite()	override;
	void Release()		override;

	// 外部（ゲーム本編）から結果を受け取るための関数
	void SetStarResults(bool iron, bool noDamage, bool time);

private:
	//KdTexture m_tex;
	//KdTexture m_enterTex;

	//float m_alpha = 0.0f;
	//float m_timer = 0.0f;

    // 背景・演出用
    KdTexture m_tex;
    Math::Matrix m_mat;
    float m_alpha = 0.0f;
    float m_timer = 0.0f;

    // 条件達成フラグ
    bool m_isIronComplete = false;
    bool m_isNoDamage = false;
    bool m_isTimeClear = false;

    // 星のアニメーション用
    float m_starAnimeTimer = 0.0f;
    float m_starScales[3] = { 0, 0, 0 };
    bool  m_isStarSoundPlayed[3] = { false, false, false };

    KdTexture m_starTex;
    KdTexture m_emptyStarTex;

    // ボタン用
    KdTexture m_retryButtonTex;
    KdTexture m_titleButtonTex;
    KdTexture m_selectButtonTex;
    ClearButton m_buttons[3];
    // 修正箇所：初期値を -1 から 0 に変更（常にどれかが選択されている状態にするため）
    int m_hoverIdx = 0;
};