#include "Map\BattleMode\BattleMap.h"
#include "tools\MyBodyParser.h"
#include "Monster\BattleMode\SoldierAI.h"
#include "DebugLayer.h"
#include "AudioEngine.h"
USING_NS_CC;
using namespace experimental;
NS_BATTLEMODE_BEGAN
BattleMap* BattleMap::static_battleMap = nullptr;
BattleMap* BattleMap::create(std::string firstMapName)
{
	BattleMap* pRet = new (std::nothrow)BattleMap;
	if (pRet && (pRet->initBattleMap(firstMapName)))
	{
		pRet->autorelease();
		return pRet;
	}
	else
	{
		delete pRet;
		pRet = nullptr;
		return nullptr;
	}
}
BattleMap * BattleMap::getBattleMap()
{
	if (static_battleMap == nullptr)
	{
		CCASSERT(false, "warning");
	}
	return static_battleMap;
}
bool BattleMap::initBattleMap(std::string firstMapName)
{
	if (!Sprite::initWithFile(firstMapName))
	{
		return false;
	}
	BattleMap::static_battleMap = this;
    visible = Director::getInstance()->getVisibleSize();
    origin = Director::getInstance()->getVisibleOrigin();
	this->setAnchorPoint(Vec2(0, 0));
	
	//點擊後目標地點
	_targetFlag = Sprite::create("flag.png");
	_targetFlag->setAnchorPoint(Vec2(0.5, 0));
	_targetFlag->setVisible(false);
	this->addChild(_targetFlag, TARGET_FLAG, TARGET_FLAG);

	

	return true;

}

NS_BATTLEMODE_END