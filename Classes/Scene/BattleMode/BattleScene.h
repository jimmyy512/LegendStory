#ifndef __BattleScene_SCENE_H__
#define __BattleScene_SCENE_H__
#include "DefinitionBattleMode.h"
#include "cocos2d.h"
NS_BATTLEMODE_BEGAN
class BattleScene : public cocos2d::Layer
{
public:
	enum
	{
		BattleMapTag,
		label1,
		label2,
		label3,
		TARGET_FLAG
	};
public:
	static cocos2d::Scene* createScene(int our,int enemy);
	static BattleScene* getBattleScene();
	virtual bool init(int our,int enemy);
	BattleScene();
	~BattleScene();
	static BattleScene* create(int our,int enemy);
public:
	//*連點計數
	int _continuousClickCount;
private:
	void pauseBattleMapListenerCallBack(Ref* ref);
	void resumeBattleMapListenerCallBack(Ref* ref);
private:
	static BattleScene* static_BattleScene;
	cocos2d::Size visible;
	cocos2d::Vec2 origin;
	cocos2d::Vec2 mapCurrentPosition;
	cocos2d::Vec2 MapOriginPosition;
	long _prevClickTime=0;
	long _currentClickTime=0;
	float m_ScaleOfScene;
};
NS_BATTLEMODE_END
#endif // __GameScene_SCENE_H__
