#ifndef __HUDlayer_SCENE_H__
#define __HUDlayer_SCENE_H__
#include "cocos2d.h"
#include "DefinitionScrollMode.h"
#include "cocos-ext.h"
#include "cocostudio\CocoStudio.h"
#include "ui\UILoadingBar.h"
#include "Npc\NPCFactory.h"
#include "Scene\ScrollMode\ScrollSceneObject.h"
NS_SCROLLMODE_BEGAN
class MainCharacter;
class HUDlayer : public cocos2d::Layer
{
public:
	enum class BtnType
	{
		NormalBtn,
		SkillBtn,
		PotionBtn
	};
	enum SkillType
	{
		Normal,
		Taji
	};
	//*普通攻擊按鈕狀態
	struct NormalSkillState
	{
		bool Transfer = false;
		bool Attack = false;
		bool Talk = false;
	};
	enum
	{
		LeftMove,
		RightMove,
		NormalAttack,
		JUMP,
		SKILL1,
		SKILL2,
		SKILL3,
		HPPotion,
		MPPotion
	};
public:
	HUDlayer();
	virtual ~HUDlayer();
	static HUDlayer* getHUDlayer();
	static HUDlayer* create(MainCharacter* mainCharacter);
	virtual bool init(MainCharacter* mainCharacter);
	void setHPBarPercent(float hp);
	void setMPBarPercent(float mp);
	void setEXPBarPercent(float exp);
	float getHPBarPercent();
	float getMPBarPercent();
	float getEXPBarPercent();
	//*切換說話技能圖標
	void SwitchTalkBtn();
	//*切換普通攻擊技能圖標
	void SwitchAttackBtn();
	//*切換傳送技能圖標
	void SwitchTransferBtn();
	//*更新HUD layer 跟著Scene pos 去移動
	void updateLayerWithScenePos(float x, float y);
	//重新刷新介面數據 像是bar上面文字 藥水圖標等等
	void refreshHUDlayer();
	void refreshMPBar();
	//重新刷新技能圖標
	void refreshSkillSprite();
	void setItemTip(bool wantShow);//傳入true或是false決定是否要顯示提示
	void setMissionTip(bool wantShow);//傳入true或是false決定是否要顯示提示
public:
	NPC* _closerNPC;
	TransferPoint* _transferPoint;
	//*如果是在切換場景狀態
	bool _isReplaceingScene;
	NormalSkillState _normalSkillState;
private:
	const float const_NormalBtn = 0.54f;
	const float const_SkillBtn = 0.41f;
	const float const_PotionBtn = 1.0f;
	static HUDlayer* static_hudlayer;
	MainCharacter* _mainCharacter;
	cocos2d::Node* _sceneCSB;
	cocos2d::Label* _HPDetailLabel;
	cocos2d::Label* _MPDetailLabel;
	cocos2d::Label* _EXPDetailLabel;
	cocos2d::Label* _levelLabel;
	cocos2d::Sprite* _leftMoveClick;
	cocos2d::Sprite* _leftMoveUnClick;
	cocos2d::Sprite* _rightMoveClick;
	cocos2d::Sprite* _rightMoveUnClick;
	cocos2d::Sprite* _normalBtn;
	cocos2d::Sprite* _jumpBtn;
	cocos2d::Sprite* _skill1Btn;
	cocos2d::Sprite* _skill2Btn;
	cocos2d::Sprite* _skill3Btn;
	cocos2d::Sprite* _HPPotionBtn;
	cocos2d::Sprite* _MPPotionBtn;
	cocos2d::Sprite* _missionTipBall;
	cocos2d::Sprite* _itemTipBall;
	//*SkillBG
	cocos2d::Sprite* _normalBG;
	cocos2d::Sprite* _jumpBG;
	cocos2d::Sprite* _skill1BG;
	cocos2d::Sprite* _skill2BG;
	cocos2d::Sprite* _skill3BG;
	//*按下移動按鈕時為true 否則為false
	bool _isTouchMove;
	std::vector<cocos2d::Sprite*> _moveBtns;
	std::vector<cocos2d::Sprite*> _skillBtns;
private:
	//重新刷新藥水按鈕區
	void refreshPotionBG();
	//重新刷新bar上面的數字
	void refreshBarNumberLabel();
	//重新刷新等級label
	void refreshLevelLabel();
	void NormalAttackCallBack();
	void Skill1CallBack();
	void Skill2CallBack();
	void Skill3CallBack();
	void hpPotionCallBack();
	void mpPotionCallBack();
	void JumpCallBack();
	void MoveListenerSendNotifiToCharacter(cocos2d::Sprite* moveSprite);
	void ShakeBtn(cocos2d::Node* wantToShakeNode, BtnType btnType);
	virtual void onExit();
private:
	cocos2d::Size visible;
	cocos2d::Vec2 origin;
	//*0傳送
	//*1說話
	//*2普通攻擊
	int _intNormalState;

};
NS_SCROLLMODE_END
#endif
