#ifndef __MainScene_SCENE_H__
#define __MainScene_SCENE_H__

#include "cocos2d.h"
#include "cocostudio\CocoStudio.h"
#include "cocos-ext.h"
class MainScene : public cocos2d::Layer
{
public:
    static cocos2d::Scene* createScene();
    virtual bool init();
    CREATE_FUNC(MainScene);
private:
	cocos2d::Size visible;
	cocos2d::Vec2 origin;
	cocos2d::Node* _sceneNode;
	bool _isReplaceScene;
};

#endif
