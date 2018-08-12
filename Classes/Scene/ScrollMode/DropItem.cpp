#include "DropItem.h"
#include "tools\JsonParser.h"
#include "RPGTools\RPGTool.h"
#include "Map\ScrollMode\ScrollMap.h"
#include "Scene\ScrollMode\ScrollSceneFactory.h"
USING_NS_CC;
using namespace MyTools;
using namespace ScrollMode;
void DropItem::dropItemOnMap(int ItemID, cocos2d::Vec2 pos)
{
	DropItem* dropItem = DropItem::create();
	dropItem->initWithFile(RPGTool::accordingItemIDToData(ItemID).SpritePath);
	dropItem->setAnchorPoint(Vec2(0.5, 0.5));
	//圖片 anchor 0.5 0.5 回有一半的圖片 在地平線下面，所以加上這一半圖片的高度
	pos.y += dropItem->getBoundingBox().size.height / 2;
	dropItem->_groundPos = pos;
	dropItem->setPosition(pos.x, pos.y);
	//action
	const float actionTime = 1.5f;
	pos.x += rand() % 2? (rand() % 61 + 20) : -(rand() % 61 + 20);
	dropItem->runAction(Spawn::create(
		RotateBy::create(actionTime, 360), 
		JumpTo::create(actionTime,pos,120,1),
		NULL));
	ScrollScene::getScrollScene()->_gameMap->addChild(dropItem,ScrollScene::DROP_ITEM_ZORDER);
}
bool DropItem::init()
{
	if (!Sprite::init())
	{
		return false;
	}
	this->schedule(schedule_selector(DropItem::update), 0.1f);
	return true;
}
void DropItem::update(float dt)
{
	if (this->getOpacity() > 10)
	{
		this->setOpacity(this->getOpacity() - 2);
	}
	else
	{
		this->unschedule(schedule_selector(DropItem::update));
		this->removeFromParent();
	}
}
