#ifndef __LoginScene_SCENE_H__
#define __LoginScene_SCENE_H__

#include "cocos2d.h"
#include "DefinitionBattleMode.h"
#include "cocostudio\CocoStudio.h"
#include "ui/CocosGUI.h"
class LoginScene : public cocos2d::Layer
{
public:
    static cocos2d::Scene* createScene();
    virtual bool init();
    CREATE_FUNC(LoginScene);
private:
	cocos2d::Size visible;
	cocos2d::Vec2 origin;
	cocos2d::Label* resaultLabel;
	cocos2d::ui::TextField* userName;
	cocos2d::ui::TextField* userPass;
	cocos2d::ui::Button* loginButton;
	std::string recvbuf;
	void curlConnect();
};
#endif // __GameScene_SCENE_H__
