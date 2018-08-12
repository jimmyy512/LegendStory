#ifndef __FailScene_SCENE_H__
#define __FailScene_SCENE_H__

#include "cocos2d.h"
#include "cocos-ext.h"
#include "cocostudio\CocoStudio.h"
class FailScene : public cocos2d::Layer
{
public:
    static cocos2d::Scene* createScene();
    virtual bool init();
    CREATE_FUNC(FailScene);
private:
	cocos2d::Size visible;
	cocos2d::Vec2 origin;
	cocos2d::Node* _sceneNode;
	virtual void onExit();
	bool _isReplaceScene;
};

#endif
