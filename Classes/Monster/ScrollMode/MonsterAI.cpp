#include "MonsterAI.h"
#include "RPGTools\RPGTool.h"
#include "tools\JsonParser.h"
#include "tools\AudioManagement.h"
#include "Scene\ScrollMode\ScrollSceneFactory.h"
#include "TipsLabel.h"
#include "ui\ScrollMode\HUDlayer.h"
#include "Scene\ScrollMode\DropItem.h"
#include "MissionMG.h"
USING_NS_CC;
using namespace MyTools;
NS_SCROLLMODE_BEGAN
int MonsterAI::monsterAmount = 0;
MonsterAI* MonsterAI::static_monsterAi = nullptr;
MonsterAI* MonsterAI::getInstance()
{
	if (static_monsterAi == nullptr)
	{
		static_monsterAi = new MonsterAI;
	}
	return static_monsterAi;
}
MonsterAI::~MonsterAI()
{
	CC_SAFE_DELETE(static_monsterAi);
}
void MonsterAI::clearData()
{
	this->_monsters.clear();
}
void MonsterAI::updateMonsterAI()
{
	if (!this->_monsters.empty())
	{
		monsterAmount = 0;
		int totalEarnExp = 0;
		for (auto it = this->_monsters.begin();it != this->_monsters.end();)
		{
			if((*it)->_currentHP<=0 && !(*it)->monsterStatus._isDeath)
			{
				(*it)->playHurtSound();
				if ((*it)->_missionState == MissionMG::getInstance()->getCurrentMissionAmountAddState())
				{
					int currentTargetAmount=atoi(JsonParser::getJsonString("Infor", "MissionTargetMount", JsonParser::JsonType::Save).c_str());
					if (++currentTargetAmount >= MissionMG::getInstance()->getCurrentMissionTargetAmount())
					{
						MissionMG::getInstance()->NextMission();
						JsonParser::setSave("Infor", "MissionTargetMount", "0");
					}
					else
					{
						char newStr[10];
						sprintf(newStr, "%d", currentTargetAmount);
						JsonParser::setSave("Infor", "MissionTargetMount", newStr);
					}
				}
				(*it)->monsterStatus._isDeath = true;
				(*it)->doDyingAnimation();
				totalEarnExp += (*it)->struct_Reward.exp;
				for (auto RewardItem : (*it)->struct_Reward.items)
				{
					int earnItemChance = rand() % 100 + 1;
					if (earnItemChance < RewardItem.second)
					{
						DropItem::dropItemOnMap(RewardItem.first,(*it)->getPosition());
						if (MyTools::JsonParser::AddItemToSave(RewardItem.first, 1) == -1)
						{
							AudioManagement::getInstance()->play("sound/warning.mp3", AudioManagement::AudioType::Sound, false);
							TipsLabel::showTip(MyTools::JsonParser::getJsonString("Normal","BagFull",MyTools::JsonParser::JsonType::UserInterFace), HUDlayer::getHUDlayer(),30,false);
						}
					}
				}
				(*it)->inVisibleMonsterHP_Bar();
				it = _monsters.erase(it);
				continue;
			}
			else
			{
				if ((*it)->isMainCharacterInAttackRange() && !MainCharacter::getMainCharacter()->status._isDying)
				{
					if (!(*it)->monsterStatus._isAttack && !(*it)->monsterStatus._isHurt)
						(*it)->doAttackAnimation();
				}
				else if (!(*it)->monsterStatus._isWalk &&
					!(*it)->monsterStatus._isHurt &&
					!(*it)->monsterStatus._isAttack &&
					!(*it)->monsterStatus._isRun &&
					!(*it)->monsterStatus._isIdle)//非跑步 非受傷 非攻擊 非待機 非走路 才能繼續走
				{
					(*it)->doRunAnimation();
				}
				(*it)->checkIdleAnimation();
				++it;
			}
			monsterAmount++;
		}
		if(totalEarnExp!=0)
			ScrollScene::getScrollScene()->_maincharacter->addExp(totalEarnExp);
	}
}
void MonsterAI::updateMonsterPosition()
{
	if (!this->_monsters.empty())
	{
		for (auto it = this->_monsters.begin();it != this->_monsters.end();it++)
		{
			//怪物在跑步狀態下 才去更新座標
			if((*it)->monsterStatus._isWalk || (*it)->monsterStatus._isRun)
				(*it)->updatePosition();
		}
	}
}
void MonsterAI::addToMonsters(ScrollMonster* monster)
{
	this->_monsters.push_back(monster);
}
NS_SCROLLMODE_END