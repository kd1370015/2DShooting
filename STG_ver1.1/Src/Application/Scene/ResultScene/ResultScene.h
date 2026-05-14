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


    // ResultScene.h に追加
    void SetScore(int score) { m_finalScore = score; }

    // もしくは SetStarResults を拡張
    void SetStarResults(bool iron, bool noDamage, bool time, int score) {
        m_isIronComplete = iron;
        m_isNoDamage = noDamage;
        m_isTimeClear = time;
        m_finalScore = score;
    }

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
    KdTexture m_rankTex; // rank2.png 用
    KdTexture m_resultTex;

    // ボタン用
    KdTexture m_retryButtonTex;
    KdTexture m_titleButtonTex;
    KdTexture m_selectButtonTex;
    ClearButton m_buttons[3];
    // 修正箇所：初期値を -1 から 0 に変更（常にどれかが選択されている状態にするため）
    int m_hoverIdx = 0;

    int m_finalScore = 0;           // 最終スコア保存用
    KdTexture m_numberTex;          // 数字フォント画像（0-9が並んだもの）

    // スコアを少しずつ増やす演出用（任意）
    float m_displayScore = 0.0f;

    // private メンバ変数に追加
    bool m_prevEnter = false;
    bool m_prevSpace = false;

    // privateメンバ変数
    int m_inputWaitTimer = 20; // 遷移直後の20フレームは入力を受け付けない
};