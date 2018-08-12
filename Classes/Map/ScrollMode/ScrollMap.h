#ifndef __ScrollMap__H__
#define __ScrollMap__H__
#include "cocos2d.h"
#include "Tools/MyBodyParser.h"
#include "MainCharacter/MainCharacter.h"
#include "DefinitionScrollMode.h"
NS_SCROLLMODE_BEGAN
class ScrollMap;
class TrainingFieldMap;
class LuoYangCityMap;

class ScrollMapFactory
{
public:
	enum class MAP_NAME
	{
		LuoYangCity,
		ChiuanJenMount,
		ChiuanJen,
		ForestField,
		Cave,
		TestMap
	};
public:
	static ScrollMap* createMap(MAP_NAME mapName);
};
class ScrollMap :public cocos2d::Node
{//抽象類，不能直接實例化
public:
	cocos2d::Size visible;
	cocos2d::Vec2 origin;
	cocos2d::Sprite* _firstMap;
	cocos2d::Sprite* _secondMap;
	cocos2d::Sprite* _thirdMap;
	cocos2d::Sprite* _fourthMap;
	cocos2d::ParallaxNode* _parallaxNode;
	cocos2d::PhysicsBody* _physicsBody;
	cocos2d::PhysicsBody* _physicsBodyStandPlatform;
	cocos2d::Node* _standPlatformNode;
	std::string _mapName;
public:
	//*子類建構子完，CREATE_FUNC會呼叫init()函數
	//*不要去重載他，除非你明白你在幹甚麼。
	bool init();
	ScrollMap();
	virtual Rect getMapRect() const=0;
};
class ForestFieldMap :public ScrollMap
{
public:
	CREATE_FUNC(ForestFieldMap);
	ForestFieldMap();
	inline virtual Rect getMapRect() const { return _firstMap->getBoundingBox(); };
};
class LuoYangCityMap :public ScrollMap
{
public:
	CREATE_FUNC(LuoYangCityMap);
	LuoYangCityMap();
	inline virtual Rect getMapRect() const { return _firstMap->getBoundingBox(); };
};
class ChiuanJenMountMap :public ScrollMap
{
public:
	CREATE_FUNC(ChiuanJenMountMap);
	ChiuanJenMountMap();
	inline virtual Rect getMapRect() const { return _firstMap->getBoundingBox(); };
};
class ChiuanJenMap :public ScrollMap
{
public:
	CREATE_FUNC(ChiuanJenMap);
	ChiuanJenMap();
	inline virtual Rect getMapRect() const { return _firstMap->getBoundingBox(); };
};
class CaveMap :public ScrollMap
{
public:
	CREATE_FUNC(CaveMap);
	CaveMap();
	inline virtual Rect getMapRect() const { return _firstMap->getBoundingBox(); };
};
class TestMap :public ScrollMap
{
public:
	CREATE_FUNC(TestMap);
	TestMap();
	inline virtual Rect getMapRect() const { return _firstMap->getBoundingBox(); };
};
NS_SCROLLMODE_END
#endif
