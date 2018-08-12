#ifndef __GameMap__H__
#define __GameMap__H__
#include "cocos2d.h"
#include "DefinitionBattleMode.h"
NS_BATTLEMODE_BEGAN
class BattleMap : public cocos2d::Sprite
{
public:
	enum
	{
		//flag Zorder and tag
		TARGET_FLAG = 10000
	};
	static BattleMap* create(std::string firstMapName);
	static BattleMap* getBattleMap();
private:
	bool initBattleMap(std::string firstMapName);
private:
	static BattleMap* static_battleMap;
	cocos2d::Sprite* _targetFlag;
	cocos2d::Size visible;
	cocos2d::Vec2 origin;
};
NS_BATTLEMODE_END
#endif // __GameMap__H__
