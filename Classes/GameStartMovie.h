#ifndef __GameStartMovie_SCENE_H__
#define __GameStartMovie_SCENE_H__

#include "cocos2d.h"
#include "UI\CocosGUI.h"
#include "ui\UIVideoPlayer.h"
class GameStartMovie : public cocos2d::Layer
{
public:
	~GameStartMovie();
    static cocos2d::Scene* createScene();
    virtual bool init();
    CREATE_FUNC(GameStartMovie);
	static bool isAlive();
private:
	void videoPlayOverCallback();
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID || CC_TARGET_PLATFORM == CC_PLATFORM_IOS || CC_TARGET_PLATFORM == CC_PLATFORM_TIZEN) && !defined(CC_PLATFORM_OS_TVOS)
	void videoEventCallback(Ref* sender, cocos2d::experimental::ui::VideoPlayer::EventType eventType);
#endif
	cocos2d::Size visible;
	cocos2d::Vec2 origin;
	cocos2d::Label* _label;
	int _touchCount;
	static GameStartMovie* static_instance;
};

#endif
