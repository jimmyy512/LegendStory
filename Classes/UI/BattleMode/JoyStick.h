#ifndef __JoyStick_SCENE_H__
#define __JoyStick_SCENE_H__

#include "cocos2d.h"
#include "DefinitionBattleMode.h"
NS_BATTLEMODE_BEGAN
class JoyStick : public cocos2d::Layer
{
	enum
	{
		JOYSTICKBACKGROUND,
		JOYSTICKBALL
	};
public:
    static cocos2d::Scene* createScene();
    virtual bool init();
    CREATE_FUNC(JoyStick);
private:
	void showJoyStick();
	void hideJoyStick();
private:
	cocos2d::Size visible;
	cocos2d::Vec2 origin;
};
NS_BATTLEMODE_END
#endif // __GameScene_SCENE_H__
