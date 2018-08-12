#ifndef __BattleMode_HUDlayer_SCENE_H__
#define __BattleMode_HUDlayer_SCENE_H__
#include "cocos2d.h"
#include "cocos-ext.h"
#include "cocostudio\CocoStudio.h"
#include "DefinitionBattleMode.h"
NS_BATTLEMODE_BEGAN
class BattleHUDlayer : public cocos2d::Layer
{
public:
	enum
	{
		//@sort with zorder
		FIRST_LAYER,
		BATTLE_SCROLL_BAR,
		SECOND_LAYER,
		JOYSTICK,
		TRIANGLE_SWITCH,
		//@sort end
		SHOOT,
		BAYONET
		
	};
public:
	static BattleHUDlayer* create();
	BattleHUDlayer();
	~BattleHUDlayer();
	virtual bool init();
	static BattleHUDlayer* getBattleHUDlayer();
public:
	//*如果是在切換場景狀態
	bool _isReplaceingScene;
private:
	void ShootCallBack();
	void ReadyToShootCallBack();
	void ClickSwitchBtnCallBack(float dt);
	void ShakeBtn(int _enum);
	virtual void onEnter();
	virtual void onExit();
private:
	static BattleHUDlayer* static_BattleHUDlayer;
	cocos2d::Size visible;
	cocos2d::Vec2 origin;
	cocos2d::Layer* _firstLayer;
	cocos2d::Layer* _secondLayer;
	cocos2d::EventListenerTouchOneByOne* _triangleSwitchListener;
	std::vector<cocos2d::Sprite*> _battleScrollSkills;
	bool _isFirstLayerDowing;
	int _SwitchLayerTimeCount = 0;
};
NS_BATTLEMODE_END
#endif // __HUDlayer_SCENE_H__
