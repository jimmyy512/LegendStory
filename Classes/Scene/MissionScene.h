#ifndef __Mission_SCENE_H__
#define __Mission_SCENE_H__
#include "cocos2d.h"
#include "cocostudio\CocoStudio.h"
#include "ui\CocosGUI.h"
class ShakeButton;
class MissionScene : public cocos2d::Layer
{
public:
    static cocos2d::Scene* createScene(cocos2d::RenderTexture* capture);
    virtual bool init();
    CREATE_FUNC(MissionScene);
private:
	cocos2d::Size visible;
	cocos2d::Vec2 origin;
	cocos2d::Node* _sceneCSB;
	cocos2d::Label* _missionTitle;
	cocos2d::Label* _missionContent;
	cocos2d::Label* _missionTip;
	ShakeButton* _returnBtn;
	virtual void onEnterTransitionDidFinish();
};

#endif
