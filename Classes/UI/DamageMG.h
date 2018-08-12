#ifndef __DamageMG__H__
#define __DamageMG__H__

#include "cocos2d.h"

class DamageMG : public cocos2d::Layer
{
public:
	enum class Direction
	{
		ToLeft,
		ToRight
	};
public:
	CREATE_FUNC(DamageMG);
	//*顯示傷害 1:顯示數字 2:數字移動方向 3:傳入怪物座標
	void showDamage(int damage,Direction direction, cocos2d::Vec2 monsterPos);
private:
	virtual bool init();
	cocos2d::Size visible;
	cocos2d::Vec2 origin;
	void CallFunc(cocos2d::Label* damageLabel);
};

#endif
