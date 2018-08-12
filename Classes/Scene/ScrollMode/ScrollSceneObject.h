#ifndef __ScrollSceneObject_H__
#define __ScrollSceneObject_H__
#include "cocos2d.h"
#include "DefinitionScrollMode.h"
#include "Scene\SplashSceneFactory.h"
NS_SCROLLMODE_BEGAN
class Cloud : public cocos2d::Sprite
{
public:
	static Cloud* create(char* cloudPath);
private:
	float _moveSpeed;
private:
	virtual bool init(char * cloudPath);
	virtual void update(float dt);
};

class TransferPoint :public cocos2d::Sprite
{
public:
	static TransferPoint* create(SplashSceneFactory::preLoadScene NextSceneName);
	cocos2d::Scene* getNextScene();
private:
	virtual bool init(SplashSceneFactory::preLoadScene NextSceneName);
private:
	SplashSceneFactory::preLoadScene _nextSceneNameEnum;
};

class MapNameTip :public cocos2d::Sprite
{
public:
	static MapNameTip* create(std::string ShowText);
private:
	virtual bool init(std::string ShowText);
	std::string _showText;
	cocos2d::Label* _label;
	virtual void update(float dt);
};
NS_SCROLLMODE_END

#endif
