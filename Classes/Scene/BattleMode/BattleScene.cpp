#include "scene/BattleMode/BattleScene.h"
#include <sstream>
#include "cocos-ext.h"
#include "spine\spine-cocos2dx.h"
#include "spine\spine.h"
#include "Tools\AudioManagement.h"
#include "UI\BattleMode\BattleHUDlayer.h"
#include "Map\BattleMode\BattleMap.h"
#include "Monster\BattleMode\BattleSoldierFactory.h"
#include "Monster\BattleMode\SoldierAI.h"
#include "Scene\DeveloperMenuScene.h"
#include "ExtensionsAlgorithm.h"
#include "DebugLayer.h"
USING_NS_CC;
USING_NS_CC_EXT;
using namespace spine;
using namespace cocos2d::experimental;
NS_BATTLEMODE_BEGAN
BattleScene* BattleScene::static_BattleScene = nullptr;
cocos2d::Scene * BattleScene::createScene(int our, int enemy)
{
	auto scene = Scene::createWithPhysics();
	auto layer = BattleScene::create(our,enemy);
	scene->addChild(layer);
	return scene;
}
BattleScene* BattleScene::getBattleScene()
{
	if (static_BattleScene == nullptr)
		return nullptr;
	else
		return static_BattleScene;
}
BattleScene::BattleScene()
{
	this->_continuousClickCount=0;
}
BattleScene::~BattleScene()
{
	NotificationCenter::getInstance()->removeAllObservers(this);
	AudioManagement::getInstance()->clearAudioManagement();
	SoldierAI::getInstance()->clearData();
	static_BattleScene = nullptr;
}
BattleScene* BattleScene::create(int our, int enemy)
{
	BattleScene* _ptr = new (std::nothrow)BattleScene;
	if (_ptr && _ptr->init(our,enemy))
	{
		_ptr->autorelease();
	}
	else
	{
		CC_SAFE_DELETE(_ptr);
	}
	return _ptr;
}
bool BattleScene::init(int our, int enemy)
{
	if (!Layer::init())
	{
		return false;
	}
	static_BattleScene = this;
	visible = Director::getInstance()->getVisibleSize();
	origin = Director::getInstance()->getVisibleOrigin();
	NotificationCenter::getInstance()->addObserver(this,
		callfuncO_selector(BattleScene::pauseBattleMapListenerCallBack), "PauseBattleMapListenerCallBack", NULL);
	NotificationCenter::getInstance()->addObserver(this,
		callfuncO_selector(BattleScene::resumeBattleMapListenerCallBack), "ResumeBattleMapListenerCallBack", NULL);

	AudioManagement::getInstance()->play("music/battleMusic3.mp3", AudioManagement::AudioType::Music, true);
	//this->addChild(DebugLayer::getInstance(), 9999);

	auto battleMap = BattleMap::create("map/BattleMode/battleMap.png");
	battleMap->setScale(0.8f);
	battleMap->setAnchorPoint(cocos2d::Vec2(0, 0));
	battleMap->setPosition(cocos2d::Vec2(0, 0));
	this->addChild(battleMap, 0, BattleMapTag);

	auto hudlayer = BattleMode::BattleHUDlayer::create();
	this->addChild(hudlayer,1);
	//init
	this->m_ScaleOfScene = 1.0f;
	
	this->schedule([this](float dt)
	{
		SoldierAI::getInstance()->updateSoldierAI();
	}, 0.7f, "SoldierAIUpdate");
	this->schedule([this](float dt)
	{
		SoldierAI::getInstance()->updateBattleSoldier();
	}, 0.2f, "BattleSoldierUpdate");
	this->schedule([this](float dt)
	{
		SoldierAI::getInstance()->updateSoldierPosition();
	}, 0.04f, "SoldierPositionupdate");


	//點擊後目標地點
	auto multiTouchListener = EventListenerTouchAllAtOnce::create();
	multiTouchListener->onTouchesBegan = [&](std::vector<Touch*> touch, Event* event)
	{
		if (SoldierAI::getInstance()->getOurSoldierSelectedTotal() >= 1)
		{
			this->_continuousClickCount++;
			this->_currentClickTime = ExtensionsAlgorithm::getCurrentTime();
			if (this->_currentClickTime -this->_prevClickTime < 10)
			{
				this->_continuousClickCount = 0;
			}
			if (this->_continuousClickCount >= 2 && 
				this->_currentClickTime - this->_prevClickTime < 300 &&
				touch.size()==1)
			{
				////由於放大縮小 會讓茅點變更，預設點擊位子是依照Vec2(0,0)茅點去判斷
				////經過觸控點加上茅點*地圖大小，就是變更茅點後新的偏移量
				Vec2 battleMapAnchorPoint = BattleMap::getBattleMap()->getAnchorPoint();
				Size battleMapSize = BattleMap::getBattleMap()->getBoundingBox().size;
				battleMapSize.width = battleMapSize.width*battleMapAnchorPoint.x;
				battleMapSize.height = battleMapSize.height*battleMapAnchorPoint.y;
				////觸控點加上地圖偏移量=點擊在地圖上的Vec2
				Vec2 touchOnBattleMap = touch[0]->getLocation() + (-BattleMap::getBattleMap()->getPosition());
				////點擊pos+縮放後的地圖和茅點位子所產生的偏移量
				touchOnBattleMap.x = touchOnBattleMap.x + battleMapSize.width;
				touchOnBattleMap.y = touchOnBattleMap.y + battleMapSize.height;
				SoldierAI::getInstance()->_structAICommand.AssignMoveCommand = true;
				AudioManagement::getInstance()->play("sound/assignPos.mp3", AudioManagement::AudioType::Sound, false);
				Sprite* targetFlag = (Sprite*)BattleMap::getBattleMap()->getChildByTag(BattleMap::TARGET_FLAG);
				targetFlag->setVisible(true);
				//雖然地圖父節點放大縮小，子節點也跟著放大縮小
				//但是子節點pos仍然不變，touchOnBattleMap是點擊地圖縮放後的Pos
				//touchOnBattleMap*(1/地圖縮放係數) = 相對於地圖未縮放時的地圖點擊pos
				targetFlag->setPosition(touchOnBattleMap*(1 / BattleMap::getBattleMap()->getScale()));
				SoldierAI::getInstance()->updateSelectedLegionFlagTargetPosition(targetFlag->getPosition());
				//點擊後 點擊次數歸0s
				this->_continuousClickCount = 0;
			}
			//點擊時間
			//CCLOG("%lu", this->_currentClickTime - this->_prevClickTime);
			this->_prevClickTime = this->_currentClickTime;
		}
	};
	multiTouchListener->onTouchesMoved = [this](std::vector<Touch*> touches, Event* event)
	{
		BattleMap* battleMap = (BattleMap*)getChildByTag(BattleMapTag);
		if (touches.size() >= 2)
		{
			auto point1 = touches[0]->getLocation();
			auto point2 = touches[1]->getLocation();
			auto currDistance = point1.distance(point2);
			auto prevDistance = touches[0]->getPreviousLocation().distance(touches[1]->getPreviousLocation());
			//兩個觸摸點和圖片原點的差距
			cocos2d::Vec2 touchWithOrginDef1 = point1 - mapCurrentPosition;
			cocos2d::Vec2 touchWithOrginDef2 = point2 - mapCurrentPosition;
			//兩觸摸點的中間
			float touchWithOrginMidX = (touchWithOrginDef1.x + touchWithOrginDef2.x) / 2;
			float touchWithOrginMidY = (touchWithOrginDef1.y + touchWithOrginDef2.y) / 2;
			//用比率計算 地圖新的矛點
			auto newAnchorX = touchWithOrginMidX / battleMap->getBoundingBox().size.width;
			auto newAnchorY = touchWithOrginMidY / battleMap->getBoundingBox().size.height;
			battleMap->setAnchorPoint(cocos2d::Vec2(newAnchorX, newAnchorY));
			//地圖新的位置
			cocos2d::Vec2 mapNewPosition;
			mapNewPosition.x = (point2.x + point1.x) / 2;
			mapNewPosition.y = (point2.y + point1.y) / 2;
			if (currDistance > prevDistance)
			{
				m_ScaleOfScene += 0.01f;
				if (m_ScaleOfScene > 1.8f)//最大的放大倍率
					m_ScaleOfScene = 1.8f;
				battleMap->setScale(m_ScaleOfScene);
			}
			else if (currDistance < prevDistance)
			{
				m_ScaleOfScene -= 0.01f;
				if (m_ScaleOfScene < 0.8f)//預設倍率就是最小的
					m_ScaleOfScene = 0.8f;
				battleMap->setScale(m_ScaleOfScene);
			}
			//縮放後 圖片大小
			Size battleMapOirginSize = battleMap->getBoundingBox().size;
			//縮放時 避免空白處顯示出來
			if (mapCurrentPosition.x > 0)
			{
				mapNewPosition.x -= mapCurrentPosition.x;
			}
			if (mapCurrentPosition.x < (-battleMapOirginSize.width) + visible.width)
			{
				mapNewPosition.x += (-battleMapOirginSize.width) + visible.width - mapCurrentPosition.x;
			}
			if (mapCurrentPosition.y > 0)
			{
				mapNewPosition.y -= mapCurrentPosition.y;
			}
			if (mapCurrentPosition.y < (-battleMapOirginSize.height) + visible.height)
			{
				mapNewPosition.y += (-battleMapOirginSize.height) + visible.height - mapCurrentPosition.y;
			}
			if (mapNewPosition != battleMap->getPosition())
			{
				this->_continuousClickCount = 0;
			}
			battleMap->setPosition(mapNewPosition);
			
			// 更新原点位置  
			mapCurrentPosition = cocos2d::Vec2(mapNewPosition.x, mapNewPosition.y)- cocos2d::Vec2(battleMapOirginSize.width * newAnchorX, battleMapOirginSize.height * newAnchorY);
		}
		else
		{
			Size battleMapOirginSize = battleMap->getBoundingBox().size;
			auto touch = touches[0];
			//滑動距離
			auto currentPointToPreviousPointDistance = touch->getDelta();
			//滑動後的地圖 應該所在的位子
			auto pos = battleMap->getPosition() + currentPointToPreviousPointDistance;
			//地圖最小、最大的X，Y座標
			auto MaxMapPosX = battleMapOirginSize.width * battleMap->getAnchorPoint().x+origin.x;
			auto MaxMapPosY = battleMapOirginSize.height * battleMap->getAnchorPoint().y+origin.y;
			auto MinMapPosX = -battleMapOirginSize.width + visible.width + battleMapOirginSize.width * battleMap->getAnchorPoint().x+origin.x;
			auto MinMapPosY = -battleMapOirginSize.height + visible.height + battleMapOirginSize.height * battleMap->getAnchorPoint().y+origin.y;
			//邊界控制
			pos.x = MIN(pos.x, MaxMapPosX);
			pos.x = MAX(pos.x, MinMapPosX);
			pos.y = MIN(pos.y, MaxMapPosY);
			pos.y = MAX(pos.y, MinMapPosY);
			//重新校正地圖位置
			if (pos != battleMap->getPosition())
			{
				this->_continuousClickCount = 0;
			}
			battleMap->setPosition(pos.x,pos.y);
			//如果到達邊界偏移量要歸0
			if (pos.x >= MaxMapPosX || pos.x <= MinMapPosX)
			{
				currentPointToPreviousPointDistance.x = 0;
			}
			if (pos.y >= MaxMapPosY || pos.y <= MinMapPosY)
			{
				currentPointToPreviousPointDistance.y = 0;
			}
			mapCurrentPosition += currentPointToPreviousPointDistance;
		}
	};
	multiTouchListener->onTouchesEnded = [this](std::vector<Touch*> touches, Event* event)
	{
	};
	Director::getInstance()->getEventDispatcher()->addEventListenerWithSceneGraphPriority(multiTouchListener, this);
	
	/*SoldierAI::getInstance()->generateOneLegion(1, our, SoldierType::Musket, SoldierCamp::OurSoldier, battleMap, cocos2d::Vec2(250, 400));
	SoldierAI::getInstance()->generateOneLegion(1, enemy, SoldierType::Assassin, SoldierCamp::EnemySoldier, battleMap, cocos2d::Vec2(400, 400));*/


	SoldierAI::getInstance()->generateOneLegion(1, 48, SoldierType::Musket, SoldierCamp::OurSoldier, battleMap, cocos2d::Vec2(250, 400));
	SoldierAI::getInstance()->generateOneLegion(2, 48, SoldierType::ShieldSoldier, SoldierCamp::EnemySoldier, battleMap, cocos2d::Vec2(1500, 400));

	SoldierAI::getInstance()->generateOneLegion(1, 48, SoldierType::Musket, SoldierCamp::OurSoldier, battleMap, cocos2d::Vec2(250, 200));
	SoldierAI::getInstance()->generateOneLegion(2, 48, SoldierType::ShieldSoldier, SoldierCamp::EnemySoldier, battleMap, cocos2d::Vec2(1500, 200));
	/*SoldierAI::getInstance()->generateOneLegion(1, 48, SoldierType::Musket, SoldierCamp::OurSoldier, battleMap, cocos2d::Vec2(250,400));
	SoldierAI::getInstance()->generateOneLegion(2, 48, SoldierType::Musket, SoldierCamp::OurSoldier, battleMap, cocos2d::Vec2(250, 200));
	SoldierAI::getInstance()->generateOneLegion(1, 48, SoldierType::Assassin,SoldierCamp::EnemySoldier, battleMap, cocos2d::Vec2(1000,400));
	SoldierAI::getInstance()->generateOneLegion(2, 48, SoldierType::Assassin, SoldierCamp::EnemySoldier, battleMap, cocos2d::Vec2(1000, 200));*/
	

	/*SoldierAI::getInstance()->generateOneLegion(1, 30, SoldierType::Musket, SoldierCamp::OurSoldier, battleMap,Vec2(1000,350));
	SoldierAI::getInstance()->generateOneLegion(2, 30, SoldierType::Musket, SoldierCamp::OurSoldier, battleMap,Vec2(1000,170));
	SoldierAI::getInstance()->generateOneLegion(3, 30, SoldierType::Musket, SoldierCamp::OurSoldier, battleMap, Vec2(1000,530));
	SoldierAI::getInstance()->generateOneLegion(1, 30, SoldierType::Assassin,SoldierCamp::EnemySoldier, battleMap, Vec2(1500,350));
	SoldierAI::getInstance()->generateOneLegion(2, 30, SoldierType::Assassin,SoldierCamp::EnemySoldier, battleMap, Vec2(1580,170));
	SoldierAI::getInstance()->generateOneLegion(3, 30, SoldierType::Assassin,SoldierCamp::EnemySoldier, battleMap, Vec2(1420,530));*/
	return true;
}
void BattleScene::pauseBattleMapListenerCallBack(Ref* ref)
{
	auto battleMap = getChildByTag(BattleMapTag);
	Director::getInstance()->getEventDispatcher()->pauseEventListenersForTarget(battleMap, true);
}
void BattleScene::resumeBattleMapListenerCallBack(Ref* ref)
{
	auto battleMap = getChildByTag(BattleMapTag);
	Director::getInstance()->getEventDispatcher()->resumeEventListenersForTarget(battleMap, true);
}


NS_BATTLEMODE_END