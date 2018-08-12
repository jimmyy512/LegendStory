#include "ScrollMap.h"
#include "Tools\JsonParser.h"
#include "Scene\ScrollMode\ScrollSceneFactory.h"
USING_NS_CC;
NS_SCROLLMODE_BEGAN
ScrollMap* ScrollMapFactory::createMap(MAP_NAME mapName)
{
	ScrollMap* map = nullptr;
	switch (mapName)
	{
	case MAP_NAME::ForestField:
		map = ForestFieldMap::create();
		break;
	case MAP_NAME::LuoYangCity:
		map = LuoYangCityMap::create();
		break;
	case MAP_NAME::ChiuanJenMount:
		map = ChiuanJenMountMap::create();
		break;
	case MAP_NAME::ChiuanJen:
		map = ChiuanJenMap::create();
		break;
	case MAP_NAME::Cave:
		map = CaveMap::create();
		break;
	case MAP_NAME::TestMap:
		map = TestMap::create();
		break;
	default:
		CCAssert(false, "MapName not found");
	}
	return map;
}
ScrollMap::ScrollMap():
_firstMap(nullptr),
_secondMap(nullptr),
_thirdMap(nullptr),
_fourthMap(nullptr),
_parallaxNode(nullptr)
{
	visible = Director::getInstance()->getVisibleSize();
	origin = Director::getInstance()->getVisibleOrigin();
	_parallaxNode = ParallaxNode::create();
	_physicsBodyStandPlatform = nullptr;
}

bool ScrollMap::init()
{//子類建構子完，CREATE_FUNC會呼叫init()函數
	if (!Node::init())
	{
		return false;
	}
	_physicsBody->getShape(0)->setMass(0.1f);
	_physicsBody->getShape(0)->setRestitution(0.f);
	_physicsBody->getShape(0)->setFriction(100.f);
	_physicsBody->getShape(0)->setDensity(1.0f);
	_physicsBody->setDynamic(false);
	if (_physicsBodyStandPlatform != nullptr)
	{
		_physicsBodyStandPlatform->getShape(0)->setMass(0.1f);
		_physicsBodyStandPlatform->getShape(0)->setRestitution(0.f);
		_physicsBodyStandPlatform->getShape(0)->setFriction(10.f);
		_physicsBodyStandPlatform->getShape(0)->setDensity(1.0f);
		_physicsBodyStandPlatform->setDynamic(false);

		_standPlatformNode = Node::create();
		_standPlatformNode->setPosition(Vec2::ZERO);
		_standPlatformNode->setAnchorPoint(Vec2::ZERO);
		_standPlatformNode->setPhysicsBody(_physicsBodyStandPlatform);
		this->addChild(_standPlatformNode);

		auto contactListener = EventListenerPhysicsContact::create();
		contactListener->onContactBegin = [](PhysicsContact& contact)
		{
			if(ScrollScene::getScrollScene()->_canCharacterJumpDown)
				return false;
			else
				return contact.getContactData()->normal.y < 0;
		};
		contactListener->onContactPreSolve = [](PhysicsContact& contact, PhysicsContactPreSolve& solve)
		{
			if (ScrollScene::getScrollScene()->_canCharacterJumpDown)
				return false;
			else
				return contact.getContactData()->normal.y < 0;
		};
		_eventDispatcher->addEventListenerWithSceneGraphPriority(contactListener, this);
	}
	this->setPhysicsBody(_physicsBody);
	this->addChild(_parallaxNode, 1);
	
	return true;
}
//@ForestFieldMap
ForestFieldMap::ForestFieldMap()
{
	_mapName = "ForestField";
	_firstMap = Sprite::create("map/ScrollMode/ForestField_1.png");
	_secondMap = Sprite::create("map/ScrollMode/ForestField_2.png");
	_thirdMap = Sprite::create("map/ScrollMode/ForestField_3.png");
	_fourthMap = Sprite::create("map/ScrollMode/ForestField_4.png");
	_firstMap->setAnchorPoint(Vec2(0, 0));
	_secondMap->setAnchorPoint(Vec2(0, 0));
	_thirdMap->setAnchorPoint(Vec2(0, 0));
	_fourthMap->setAnchorPoint(Vec2(0, 0));

	_firstMap->setPosition(0, 0);
	_secondMap->setPosition(0, 0);
	_thirdMap->setPosition(0, 0);
	_fourthMap->setPosition(0, 0);
	this->addChild(_firstMap, 2);
	//視差節點
	_parallaxNode->addChild(_secondMap, 3, Vec2(0.7, 0.7), Vec2(0,-60));
	_parallaxNode->addChild(_thirdMap, 2,Vec2(0.6, 0.6), Vec2(0,-100));
	_parallaxNode->addChild(_fourthMap, 1, Vec2(0.5, 0.5), Vec2(0, -100));
	
	MyBodyParser::getInstance()->parseJsonFile("map/ScrollMode/ForestField.json");
	_physicsBody = MyBodyParser::getInstance()->bodyFormJson(_firstMap, "ForestField", PHYSICSBODY_MATERIAL_DEFAULT);
	
}

//@LuoYangCityMap
LuoYangCityMap::LuoYangCityMap()
{
	_mapName = "LuoYangCity";
	_firstMap = Sprite::create("map/ScrollMode/LuoYangCity.png");
	_secondMap = Sprite::create("map/ScrollMode/LuoYangCityBG1.png");
	_thirdMap=Sprite::create("map/ScrollMode/LuoYangCityBG2.png");
	_thirdMap->setScale(1.2);
	_firstMap->setAnchorPoint(Vec2(0, 0));
	_secondMap->setAnchorPoint(Vec2(0, 0));
	_thirdMap->setAnchorPoint(Vec2(0, 0));
	_firstMap->setPosition(0, 0);
	_secondMap->setPosition(0, 0);
	_thirdMap->setPosition(0, 0);
	this->addChild(_firstMap, 2);
	//視差節點
	//樹
	_parallaxNode->addChild(_secondMap, 2, Vec2(0.5, 0.8), Vec2(200, 40));
	//山景
	_parallaxNode->addChild(_thirdMap, 1, Vec2(0.3, 0.6), Vec2(0, 0));
	//地圖邊界
	MyBodyParser::getInstance()->parseJsonFile("map/ScrollMode/LuoYangCity.json");
	_physicsBody = MyBodyParser::getInstance()->bodyFormJson(_firstMap, "LuoYangCity", PHYSICSBODY_MATERIAL_DEFAULT);
	MyBodyParser::getInstance()->parseJsonFile("map/ScrollMode/LuoYangCitySP.json");
	_physicsBodyStandPlatform= MyBodyParser::getInstance()->bodyFormJson(_firstMap, "LuoYangCitySP", PHYSICSBODY_MATERIAL_DEFAULT);
	_physicsBodyStandPlatform->setContactTestBitmask(0xFFFFFFFF);
}

//@ChiuanJenMountMap
ChiuanJenMountMap::ChiuanJenMountMap()
{
	_mapName = "ChiuanJenMount";
	_firstMap = Sprite::create("map/ScrollMode/ChiuanJenMount_1.png");
	_secondMap = Sprite::create("map/ScrollMode/ChiuanJenMount_2.png");
	_thirdMap = Sprite::create("map/ScrollMode/ChiuanJenMount_3.png");
	auto fourthMap= Sprite::create("map/ScrollMode/ChiuanJenMount_4.png");
	_firstMap->setAnchorPoint(Vec2(0, 0));
	_secondMap->setAnchorPoint(Vec2(0, 0));
	_thirdMap->setAnchorPoint(Vec2(0, 0));
	fourthMap->setAnchorPoint(Vec2(0, 0));

	_firstMap->setPosition(0, 0);
	_secondMap->setPosition(0, 0);
	_thirdMap->setPosition(0, 0);
	fourthMap->setPosition(0, 0);
	this->addChild(_firstMap, 2);
	this->addChild(fourthMap, 9999);
	//視差節點
	_parallaxNode->addChild(_secondMap, 2, Vec2(0.7, 0.8), Vec2(0,0));
	_parallaxNode->addChild(_thirdMap, 1, Vec2(0.1, 0.1), Vec2(0, 0));
	//地圖邊界
	MyBodyParser::getInstance()->parseJsonFile("map/ScrollMode/ChiuanJenMount.json");
	_physicsBody = MyBodyParser::getInstance()->bodyFormJson(_firstMap, "ChiuanJenMount", PHYSICSBODY_MATERIAL_DEFAULT);
	MyBodyParser::getInstance()->parseJsonFile("map/ScrollMode/ChiuanJenMountSP.json");
	_physicsBodyStandPlatform = MyBodyParser::getInstance()->bodyFormJson(_firstMap, "ChiuanJenMountSP", PHYSICSBODY_MATERIAL_DEFAULT);
	_physicsBodyStandPlatform->setContactTestBitmask(0xFFFFFFFF);
}

//@ChiuanJenMap
ChiuanJenMap::ChiuanJenMap()
{
	_mapName = "ChiuanJen";
	_firstMap = Sprite::create("map/ScrollMode/ChiuanJen_1.png");
	_secondMap = Sprite::create("map/ScrollMode/ChiuanJen_2.png");

	_firstMap->setAnchorPoint(Vec2(0, 0));
	_secondMap->setAnchorPoint(Vec2(0, 0));

	_firstMap->setPosition(0, 0);
	_secondMap->setPosition(0, 0);

	this->addChild(_firstMap, 2);
	//視差節點
	//樹
	_parallaxNode->addChild(_secondMap, 2, Vec2(0.6, 0.6), Vec2(0, 0));
	
	//地圖邊界
	MyBodyParser::getInstance()->parseJsonFile("map/ScrollMode/ChiuanJen.json");
	_physicsBody = MyBodyParser::getInstance()->bodyFormJson(_firstMap, "ChiuanJen", PHYSICSBODY_MATERIAL_DEFAULT);

	MyBodyParser::getInstance()->parseJsonFile("map/ScrollMode/ChiuanJenSP.json");
	_physicsBodyStandPlatform = MyBodyParser::getInstance()->bodyFormJson(_firstMap, "ChiuanJenSP", PHYSICSBODY_MATERIAL_DEFAULT);
	_physicsBodyStandPlatform->setContactTestBitmask(0xFFFFFFFF);
}

//@CaveMap
CaveMap::CaveMap()
{
	_mapName = "Cave";
	_firstMap = Sprite::create("map/ScrollMode/Cave_1.png");
	_secondMap = Sprite::create("map/ScrollMode/Cave_2.png");
	_thirdMap = Sprite::create("map/ScrollMode/Cave_3.png");
	_fourthMap = Sprite::create("map/ScrollMode/Cave_4.png");
	_firstMap->setAnchorPoint(Vec2(0, 0));
	_secondMap->setAnchorPoint(Vec2(0, 0));
	_thirdMap->setAnchorPoint(Vec2(0, 0));
	_fourthMap->setAnchorPoint(Vec2(0, 0));

	_firstMap->setPosition(0, 0);
	_secondMap->setPosition(0, 0);
	_thirdMap->setPosition(0, 0);
	_fourthMap->setPosition(0, 0);
	this->addChild(_firstMap, 2);
	//視差節點
	_parallaxNode->addChild(_secondMap, 3, Vec2(0.7, 0.7), Vec2(0, 0));
	_parallaxNode->addChild(_thirdMap, 2, Vec2(0.6, 0.6), Vec2(0, 0));
	_parallaxNode->addChild(_fourthMap, 1, Vec2(0.5, 0.5), Vec2(0, 0));

	MyBodyParser::getInstance()->parseJsonFile("map/ScrollMode/Cave.json");
	_physicsBody = MyBodyParser::getInstance()->bodyFormJson(_firstMap, "Cave", PHYSICSBODY_MATERIAL_DEFAULT);

}

//@TestMap
TestMap::TestMap()
{
	_mapName = "TestMap";
	_firstMap = Sprite::create("map/ScrollMode/TestMap.png");
	/*_secondMap = Sprite::create("map/ScrollMode/ChiuanJenMount_2.png");
	_thirdMap = Sprite::create("map/ScrollMode/ChiuanJenMount_3.png");
	auto fourthMap = Sprite::create("map/ScrollMode/ChiuanJenMount_4.png");*/
	_firstMap->setAnchorPoint(Vec2(0, 0));
	/*_secondMap->setAnchorPoint(Vec2(0, 0));
	_thirdMap->setAnchorPoint(Vec2(0, 0));
	fourthMap->setAnchorPoint(Vec2(0, 0));*/

	_firstMap->setPosition(0, 0);
	/*_secondMap->setPosition(0, 0);
	_thirdMap->setPosition(0, 0);
	fourthMap->setPosition(0, 0);*/
	this->addChild(_firstMap, 2);
	//this->addChild(fourthMap, 9999);
	//視差節點
	/*_parallaxNode->addChild(_secondMap, 2, Vec2(0.7, 0.8), Vec2(0, 0));
	_parallaxNode->addChild(_thirdMap, 1, Vec2(0.1, 0.1), Vec2(0, 0));*/
	//地圖邊界
	MyBodyParser::getInstance()->parseJsonFile("map/ScrollMode/TestMap.json");
	_physicsBody = MyBodyParser::getInstance()->bodyFormJson(_firstMap, "TestMap", PHYSICSBODY_MATERIAL_DEFAULT);

}



NS_SCROLLMODE_END


