#ifndef __DropItem_H__
#define __DropItem_H__

#include "cocos2d.h"
#include "DefinitionScrollMode.h"
class DropItem : public cocos2d::Sprite
{
public:
	static void dropItemOnMap(int ItemID,cocos2d::Vec2 pos);
private:
	CREATE_FUNC(DropItem);
	virtual bool init();
	virtual void update(float dt);
private:
	cocos2d::Vec2 _groundPos;
};

#endif
