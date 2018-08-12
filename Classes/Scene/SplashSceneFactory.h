#ifndef __SplashSceneFactory_SCENE_H__
#define __SplashSceneFactory_SCENE_H__
#include "cocos2d.h"
#include "cocos-ext.h"
class SplashSceneFactory 
{
public:
	enum class preLoadScene
	{
		preLoadForestField,
		preLoadLuoYangCity,
		preLoadChiuanJenMount,
		preLoadChiuanJen,
		preLoadBattleTrainingField,
		preLoadCave,
		preLoadTestMap
	};
	static cocos2d::Scene* createScene(preLoadScene preLoadsceneName);
};
class SplashScene :public cocos2d::Layer
{
public:
	cocos2d::Size visible;
	cocos2d::Vec2 origin;
protected:
	void addLoadingSpriteWitchSpark();
	void addWordJumpEffect(char* jumpWordString);
	virtual bool init();
public:
	virtual void nextScene(float dt)=0;
	virtual void loadTextureCallBack(cocos2d::Texture2D* texture) = 0;
};

//@ForestField
class PreloadForestField :public SplashScene
{
public:
    static cocos2d::Scene* createScene();
    virtual bool init();
    CREATE_FUNC(PreloadForestField);
private:
	virtual void nextScene(float dt)override;
	void loadTextureCallBack(cocos2d::Texture2D* texture)override;
};

//@LuoYang
class PreloadLuoYang :public PreloadForestField
{
public:
	static cocos2d::Scene* createScene();
	CREATE_FUNC(PreloadLuoYang);
private:
	virtual void nextScene(float dt)override;
};

//@ChiuanJenMount
class PreloadChiuanJenMount :public PreloadForestField
{
public:
	static cocos2d::Scene* createScene();
	CREATE_FUNC(PreloadChiuanJenMount);
private:
	virtual void nextScene(float dt)override;
};

//@PreloadCave
class PreloadCave :public PreloadForestField
{
public:
	static cocos2d::Scene* createScene();
	CREATE_FUNC(PreloadCave);
private:
	virtual void nextScene(float dt)override;
};

//@ChiuanJen
class PreloadChiuanJen :public PreloadForestField
{
public:
	static cocos2d::Scene* createScene();
	CREATE_FUNC(PreloadChiuanJen);
private:
	virtual void nextScene(float dt)override;
};

///////////////////////////////BattleMode//////////////////////////////////////////
//@PreloadBattleTraining
class PreloadBattleTraining:public SplashScene
{
public:
	static cocos2d::Scene* createScene();
	virtual bool init();
	CREATE_FUNC(PreloadBattleTraining);
	virtual void nextScene(float dt)override;
	void loadTextureCallBack(cocos2d::Texture2D* texture)override;
};

class PreloadTestMap :public SplashScene
{
public:
	static cocos2d::Scene* createScene();
	virtual bool init();
	CREATE_FUNC(PreloadTestMap);
	void loadTextureCallBack(cocos2d::Texture2D * texture)override;
	virtual void nextScene(float dt)override;
};

#endif
