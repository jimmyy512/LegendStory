#ifndef __DeveloperMenuScene_SCENE_H__
#define __DeveloperMenuScene_SCENE_H__

#include "cocos2d.h"
#include "ui\CocosGUI.h"
#include "UI\MyButton.h"
class DeveloperMenuScene : public cocos2d::Layer
{
public:
    static cocos2d::Scene* createScene();
    virtual bool init();
    CREATE_FUNC(DeveloperMenuScene);
private:
	void ScrollModeMenuItemCallBack(Ref* sender);
	void BattleModeMenuItemCallBack(Ref* sender);
	void TestMapMenuItemCallBack(Ref* sender);
	void CreateCharacterSceneItemCallBack(Ref* sender);
	void LuoYangSceneCallBack(Ref* sender);
	void BackMountSceneCallBack(Ref* sender);
	void ChiuanJenSceneCallBack(Ref* sender);
	void CaveSceneCallBack(Ref* sender);
	void checkBoxCallback(cocos2d::Ref * ref, cocos2d::ui::CheckBox::EventType type);
	cocos2d::Size visible;
	cocos2d::Vec2 origin;
	cocos2d::Menu* _menu;
	cocos2d::Node* _page2Node;
	cocos2d::Label* _MissionName;
	cocos2d::Label* _setLevelLabel;
	bool _pageTurn;
	ShakeButton* _pageBtn;
};

#endif // __DeveloperMenuScene_SCENE_H__
