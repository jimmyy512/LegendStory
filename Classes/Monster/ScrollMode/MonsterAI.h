#ifndef __MonsterAI__H__
#define __MonsterAI__H__
#include "Monster/ScrollMode/ScrollMonsterFactory.h"
#include "MainCharacter/MainCharacter.h"
#include "cocos2d.h"
NS_SCROLLMODE_BEGAN
class MonsterAI
{
public:
	static MonsterAI* getInstance();
	virtual ~MonsterAI();
	void clearData();
	void addToMonsters(ScrollMonster* monster);
	void updateMonsterAI();
	void updateMonsterPosition();
public:
	std::vector<ScrollMonster*> _monsters;
	static int monsterAmount;
private:
	MainCharacter* _mainCharacter;
	static MonsterAI* static_monsterAi;

};
NS_SCROLLMODE_END

#endif
