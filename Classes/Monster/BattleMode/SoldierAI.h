#ifndef __SoldierAI__H__
#define __SoldierAI__H__
#include "DefinitionBattleMode.h"
#include "cocos2d.h"
NS_BATTLEMODE_BEGAN
class Legion;
class BattleSoldierFactory;
class BattleMap;
enum class SoldierType
{
	ShieldSoldier,
	Assassin,
	Musket,
	Lyonar
};
enum class SoldierCamp//陣營
{
	OurSoldier,
	EnemySoldier
};
class SoldierAI
{
public:
	struct AICommand
	{
		bool AttackCommand;
		bool StartBattleCommand;
		bool FindEnemyCommand;
		bool AssignMoveCommand;
	};
public:
	SoldierAI();
	~SoldierAI();
	static SoldierAI* getInstance();
	void clearData();
	void generateOneLegion(int LegionIndex, int SoldierAmount, SoldierType SoldierType, SoldierCamp SoldierCamp, BattleMap* battlemMap,cocos2d::Vec2 generatePos);
	void updateSoldierAI();
	void updateBattleSoldier();
	void updateSoldierPosition();
	//*according class Legion _isSelected is true
	//*update class Legion _targetFlagPosition;
	void updateSelectedLegionFlagTargetPosition(cocos2d::Vec2 FlagPos);
	void updateMusketSoldierReadyToShootState();
	void updateMusketSoldierShoot();
	int countHowManyLegionGetSelected();
	int getOurSoldierSelectedTotal();
	void searchTarget();
public:
	static SoldierAI* static_monsterAi_ptr;
	//*Ai thread
	std::thread _t1;
	//*戰場上所有單位
	std::vector<BattleSoldierFactory*> _soldiers;
	//*我們陣營所有單位
	std::vector<BattleSoldierFactory*> _ourSideSoldiers;
	//*敵方陣營所有單位
	std::vector<BattleSoldierFactory*> _enemySideSoldiers;
	//*我們所有軍團
	std::vector<Legion*> _ourLegions;
	//*敵方所有軍團
	std::vector<Legion*> _enemyLegions;
	const int const_MaxGridXIndex = 20;
	const int const_MaxGridYIndex = 12;
	const int const_GridLengthX = 100;
	const int const_GridLengthY = 50;
	AICommand _structAICommand;
private:
	//*當士兵需要從戰場上移除時，從各自所屬的軍團vector移除
	void removeFromCampVector(SoldierCamp soldierCamp, BattleSoldierFactory* RemoveSoldierPtr);
	//*當士兵需要從戰場上移除時，從各自所屬的陣營vector移除
	void removeFromSideVector(SoldierCamp soldierCamp, BattleSoldierFactory* RemoveSoldierPtr);
	void unselectedLegionSoldier(Legion* selectedLegion);
	//切割地圖成格子，並且告訴戰場上的人，自己的格子索引。
	void splitMapToGrid();
private:
	cocos2d::Size visible;
	cocos2d::Vec2 origin;
	std::mutex _mu;
};

class Legion
{
	friend SoldierAI;
public:
	Legion(int legionIndex, SoldierCamp soldierCamp);
	//*let under soldier _standPointSprite getting visible
	void showSoldierStandSprite();
	//*let under soldier _standPointSprite getting invisible
	void hideSoldierStandSprite();
	void addToLegion(BattleSoldierFactory* battleSoldier);
private:
	//*when create Legion,formation all soldiers.
	void startFormation(cocos2d::Vec2 generatePos);
	//*general touch listener
	void addGeneralTouchListener(BattleSoldierFactory* generalPtr);
public:
	int _LegionIndex;
	//*SoldierAI::SoldierCamp enum to determine whether legion belongs
	BattleMode::SoldierCamp _SoldierCamp;
	bool _isSelected = false;
	cocos2d::Vec2 _targetFlagPosition;
	std::vector<BattleSoldierFactory*> _legionSoldiers;
	BattleSoldierFactory* _general;
};
NS_BATTLEMODE_END

#endif 

