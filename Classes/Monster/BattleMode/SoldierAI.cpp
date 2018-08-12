#include "SoldierAI.h"
#include "Monster\BattleMode\BattleSoldierFactory.h"
#include "Map\BattleMode\BattleMap.h"
#include "UI\BattleMode\BattleHUDlayer.h"
#include "AudioEngine.h"
#include "ExtensionsAlgorithm.h"
#include "DebugLayer.h"
USING_NS_CC;
using namespace experimental;
NS_BATTLEMODE_BEGAN
enum class SoldierType;
enum class SoldierCamp;
SoldierAI* SoldierAI::static_monsterAi_ptr = nullptr;
namespace
{
	//我方軍團陣型行數
	const int const_ourLegionFormationRow=6;
	//我方軍團陣型列數
	const int const_ourLegionFormationColumn=5;
	//敵方軍團陣型行數
	const int const_enemyLegionFormationRow = 6;
	//敵方軍團陣型列數
	const int const_enemyLegionFormationColumn = 5;
	//我方陣型起始點
	Vec2 FormationStartVec2;
	//我方陣行間的距離
	Size const_OurLegionFormationSize = Size(12, 25);
	//敵方陣行間的距離
	Size const_EnemyLegionFormationSize = Size(12, 25);
}
SoldierAI::SoldierAI()
{
	visible = Director::getInstance()->getVisibleSize();
	origin = Director::getInstance()->getVisibleOrigin();
	this->_structAICommand.AttackCommand = true;
	this->_structAICommand.StartBattleCommand = false;
	this->_structAICommand.FindEnemyCommand= true;
	this->_structAICommand.AssignMoveCommand = false;
}
SoldierAI::~SoldierAI()
{
	CC_SAFE_DELETE(static_monsterAi_ptr);
}
void SoldierAI::clearData()
{
	_soldiers.clear();
	_ourSideSoldiers.clear();
	_enemySideSoldiers.clear();
	_ourLegions.clear();
	_enemyLegions.clear();
}
SoldierAI* SoldierAI::getInstance()
{
	if (static_monsterAi_ptr == nullptr)
	{
		static_monsterAi_ptr = new SoldierAI;
	}
	return static_monsterAi_ptr;
}
void SoldierAI::updateBattleSoldier()
{
	if (_soldiers.size() != 0)
	{
		for (auto it = this->_soldiers.begin();it != this->_soldiers.end();)
		{
			if ((*it)->getSoldierHp()<=0 && (*it)->_structSoldierState.isDeath==false)
			{//如果血量小於0
				(*it)->_structSoldierState.isDeath = true;
				(*it)->KillYourSelf();
				BattleSoldierFactory* soldier = static_cast<BattleSoldierFactory*>(*it);
				//如果當前死亡的單位是軍團將軍
				if (soldier == soldier->_belongLegion->_general)
				{//讓將軍底下士兵取消選取狀態
					this->unselectedLegionSoldier(soldier->_belongLegion);
				}
				this->removeFromCampVector((*it)->_soldierCamp, *it);
				this->removeFromSideVector((*it)->_soldierCamp, *it);
				it = this->_soldiers.erase(it);
			}
			else
			{
				(*it)->updateBattleSoldier();
				it++;
			}
		}
	}
}
void SoldierAI::searchTarget()
{
	std::multimap<int, BattleSoldierFactory*> oursoldierTree;
	std::multimap<int, BattleSoldierFactory*> enemysoldierTree;
	for (auto it = _soldiers.begin();it != _soldiers.end();it++)
	{
		if ((*it)->_soldierCamp == SoldierCamp::OurSoldier)
		{
			oursoldierTree.insert(std::pair<int, BattleSoldierFactory*>((*it)->_gridIndex, *it));
		}
		else
		{
			enemysoldierTree.insert(std::pair<int, BattleSoldierFactory*>((*it)->_gridIndex, *it));
		}
	}
	for (auto it = _soldiers.begin();it != _soldiers.end();)
	{
		if (this->_structAICommand.FindEnemyCommand)
		{
			std::vector<int> wantFindGrids;
			wantFindGrids.push_back((*it)->_gridIndex);//中
			wantFindGrids.push_back((*it)->_gridIndex - 1);//左
			wantFindGrids.push_back((*it)->_gridIndex - 2);//左
			wantFindGrids.push_back((*it)->_gridIndex - 3);//左
			wantFindGrids.push_back((*it)->_gridIndex - 4);//左
			wantFindGrids.push_back((*it)->_gridIndex - 5);//左
			wantFindGrids.push_back((*it)->_gridIndex - 1 + const_MaxGridXIndex);//左上
			wantFindGrids.push_back((*it)->_gridIndex - 1 - const_MaxGridXIndex);//左下
			wantFindGrids.push_back((*it)->_gridIndex + 1);//右
			wantFindGrids.push_back((*it)->_gridIndex + 2);//右
			wantFindGrids.push_back((*it)->_gridIndex + 3);//右
			wantFindGrids.push_back((*it)->_gridIndex + 4);//右
			wantFindGrids.push_back((*it)->_gridIndex + 5);//右
			wantFindGrids.push_back((*it)->_gridIndex + 1 + const_MaxGridXIndex);//右上
			wantFindGrids.push_back((*it)->_gridIndex + 1 - const_MaxGridXIndex);//右下
			wantFindGrids.push_back((*it)->_gridIndex + const_MaxGridXIndex);//中上
			wantFindGrids.push_back((*it)->_gridIndex - const_MaxGridXIndex);//中下
			Vec2 pretendCloserDistanceVec2 = Vec2(9999, 9999);
			BattleSoldierFactory* closerEnemy = nullptr;
			int otherSideSoldierCount = 0;
			for (auto grid : wantFindGrids)
			{
				std::multimap<int, BattleSoldierFactory*>::iterator begin;
				std::multimap<int, BattleSoldierFactory*>::iterator end;
				if ((*it)->_soldierCamp == SoldierCamp::OurSoldier)
				{//如果當前士兵是我方士兵，那只搜尋敵方敵人
					begin = enemysoldierTree.lower_bound(grid);
					end = enemysoldierTree.upper_bound(grid);
				}
				else
				{//如果當前士兵是我方士兵，那只搜尋敵方敵人
					begin = oursoldierTree.lower_bound(grid);
					end = oursoldierTree.upper_bound(grid);
				}
				while (begin != end)
				{
					float distance = (*it)->getPosition().distance(begin->second->getPosition());
					float oldVec2distance = (*it)->getPosition().distance(pretendCloserDistanceVec2);
					//傳進來的敵人位子距離  比假定的最小的還要更小 就由它成為假定最小值
					//每位士兵最多只能被兩個敵人鎖定
					if (distance < oldVec2distance && begin->second->_beingEnemyTagetCount < 2)
					{
						closerEnemy = begin->second;
						pretendCloserDistanceVec2 = begin->second->getPosition();
						otherSideSoldierCount++;
					}
					begin++;
				}
			}
			//當場上找不到士兵時
			if (otherSideSoldierCount == 0)
				(*it)->_target = nullptr;
			else
			{//當士兵找到目標時 存方目標的指針 並把目標的"被敵人設為目標"的計數+1
				if (closerEnemy != nullptr)
				{
					(*it)->_target = closerEnemy;
					closerEnemy->_beingEnemyTagetCount++;
				}
			}
		}
		it++;
	}
	for (auto soldier = this->_soldiers.begin();soldier != this->_soldiers.end();soldier++)
	{
		(*soldier)->_beingEnemyTagetCount = 0;
	}
}
void SoldierAI::updateSoldierAI()
{
	if (!this->_soldiers.empty())
	{
		this->searchTarget();
		//_t1=std::thread(&SoldierAI::searchTarget, this);
		//_t1.detach();
	}
}
void SoldierAI::updateSoldierPosition()
{
	if (this->_structAICommand.StartBattleCommand)
	{
		if (!this->_soldiers.empty())
		{
			for (auto it = this->_soldiers.begin();it != this->_soldiers.end();	it++)
			{
				if ((*it)->_soldierCamp == SoldierCamp::OurSoldier)
				{//我方軍隊AI
					if ((*it)->_structSoldierState.isAttack == false &&
						(*it)->_target != nullptr &&
						(*it)->getPosition().distance((*it)->_target->getPosition()) < 100.f)
					{//靠近敵人時，士兵會自己去攻擊敵人
						(*it)->MoveToTargetPosition();
					}
					else if (!(*it)->_structSoldierState.isArriveAssignPosition && 
						!(*it)->_structSoldierState.isAttack &&
						_structAICommand.AssignMoveCommand)
					{//前往指定地點
						(*it)->MoveToAssignPosition();
					}
					else
					{//沒收到任何命令 原地待機
						(*it)->stayYourPosition();
					}
				}
				else
				{//敵人AI
					//靠近敵人一定的距離會變成攻擊狀態 所以要判斷非攻擊狀態時 才能移動
					if ((*it)->_target != nullptr)
					{
						(*it)->MoveToTargetPosition();
					}
					else
					{//找不到敵人繼續前進
						(*it)->forwardPosition();
					}
				}
				(*it)->checkGrid();
			}
		}
	}
}
void SoldierAI::generateOneLegion(int LegionIndex, int SoldierAmount, SoldierType SoldierType, SoldierCamp SoldierCamp, BattleMap* battleMap , Vec2 generatePos)
{
	Legion* legion = new Legion(LegionIndex, SoldierCamp);
	for (int i = 0;i < SoldierAmount;i++)
	{
		BattleSoldierFactory* Soldier=nullptr;
		switch (SoldierType)
		{
		case BattleMode::SoldierType::Assassin:
			Soldier = Assassin::create();
			break;
		case BattleMode::SoldierType::Musket:
			Soldier = MusketSoldier::create();
			break;
		case BattleMode::SoldierType::ShieldSoldier:
			Soldier = ShieldSoldier::create();
			break;
		default:
			CCASSERT(false, "warning");
		}
		switch (SoldierCamp)
		{
		case BattleMode::SoldierCamp::OurSoldier:
			Soldier->_soldierCamp = BattleMode::SoldierCamp::OurSoldier;
			this->_ourSideSoldiers.push_back(Soldier);
			break;
		case BattleMode::SoldierCamp::EnemySoldier:
			Soldier->_soldierCamp = BattleMode::SoldierCamp::EnemySoldier;
			Soldier->setFaceDirection(true);
			this->_enemySideSoldiers.push_back(Soldier);
			break;
		default:
			CCASSERT(false, "impossible");
		}
		legion->addToLegion(Soldier);//將士兵加入軍團管理
		Soldier->_belongLegion = legion;//每個士兵需要知道自己的所屬軍團
		_soldiers.push_back(Soldier);//將士兵加入戰場上所有士兵管理
		battleMap->addChild(Soldier, 3);
	}
	//陣營管理
	switch (SoldierCamp)
	{
	case BattleMode::SoldierCamp::OurSoldier:
		legion->_general = Lyonar::create();//產生一個我方將軍
		legion->_general->_soldierCamp = BattleMode::SoldierCamp::OurSoldier;//將軍所屬陣營
		legion->_general->_belongLegion = legion;//將軍需要知道所屬軍團
		_ourLegions.push_back(legion);//把當前軍團加到我方陣營中
		_ourSideSoldiers.push_back(legion->_general);//把當前將軍加入到我們所有單位管理
		break;
	case BattleMode::SoldierCamp::EnemySoldier:
		legion->_general = ShieldSoldier::create();//產生一個敵方將軍
		legion->_general->_soldierCamp = BattleMode::SoldierCamp::EnemySoldier;//將軍所屬陣營
		legion->_general->_belongLegion = legion;//將軍需要知道所屬軍團
		legion->_general->setFaceDirection(true);//敵人反轉方向
		_enemyLegions.push_back(legion);//把當前軍團加到敵方陣營中
		_enemySideSoldiers.push_back(legion->_general);
		break;
	default:
		CCASSERT(false, "impossible");
	}
	legion->addToLegion(legion->_general);//把將軍加入軍團中
	_soldiers.push_back(legion->_general);//把將軍加入到戰場上所有士兵管理中
	battleMap->addChild(legion->_general, 3);
	//軍團排陣型
	legion->startFormation(generatePos);
	//切割地圖成格子，並且告訴戰場上的人，自己的格子索引。
	splitMapToGrid();
}

void SoldierAI::removeFromCampVector(SoldierCamp soldierCamp,BattleSoldierFactory* RemoveSoldierPtr)
{
	std::vector<Legion*>* Legions;
	switch (soldierCamp)
	{
	case SoldierCamp::OurSoldier:
		Legions = &_ourLegions;
		break;
	case SoldierCamp::EnemySoldier:
		Legions = &_enemyLegions;
		break;
	}
	for (auto Legion = (*Legions).begin();Legion != (*Legions).end();)
	{
		if ((*Legion)->_legionSoldiers.size() != 0)
		{
			//軍隊裡的軍團的每個士兵
			for (auto LegionSoldier =(*Legion)->_legionSoldiers.begin();LegionSoldier != (*Legion)->_legionSoldiers.end();LegionSoldier++)
			{
				if (*LegionSoldier == RemoveSoldierPtr)
				{
					(*Legion)->_legionSoldiers.erase(LegionSoldier);
					break;
				}
			}
			Legion++;
		}
		//士兵數0 被滅團。並且把被滅團士兵，移出陣營軍團
		else
		{
			Legion=(*Legions).erase(Legion);
		}
	}
}
void SoldierAI::removeFromSideVector(SoldierCamp soldierCamp, BattleSoldierFactory * RemoveSoldierPtr)
{
	std::vector<BattleSoldierFactory*>* SideSoldiers;
	switch (soldierCamp)
	{
	case SoldierCamp::OurSoldier:
		SideSoldiers = &_ourSideSoldiers;
		break;
	case SoldierCamp::EnemySoldier:
		SideSoldiers = &_enemySideSoldiers;
		break;
	}
	for (auto sideSoldier = (*SideSoldiers).begin();sideSoldier != (*SideSoldiers).end();sideSoldier++)
	{
		if (*sideSoldier == RemoveSoldierPtr)
		{
			SideSoldiers->erase(sideSoldier);
			break;
		}
	}
}

void SoldierAI::unselectedLegionSoldier(Legion* selectedLegion)
{
	selectedLegion->_isSelected = false;
	for (auto LegionSoldier : selectedLegion->_legionSoldiers)
	{
		static_cast<BattleSoldierFactory*>(LegionSoldier)->_standPointSprite->setVisible(false);
	}
}

void SoldierAI::splitMapToGrid()
{
	std::vector<BattleSoldierFactory*> _TotalSoldier = _soldiers;
	int GridIndex = 0;
	//第幾格X格子
	int gridXIndex = 0;
	//第幾格Y格子
	int gridYIndex = 0;
	//200*600大小地圖 被切成100*100的小格子 共有120格子
	for (int i=0;i<const_MaxGridXIndex*const_MaxGridYIndex;i++)
	{
		if (GridIndex % const_MaxGridXIndex == 0 && GridIndex != 0)
		{//換列時
			gridXIndex = 0;
			gridYIndex++;
		}
		for (auto soldier = _TotalSoldier.begin();soldier != _TotalSoldier.end();)
		{
			if (((*soldier)->getPosition().x >= gridXIndex * const_GridLengthX &&
				(*soldier)->getPosition().x < (gridXIndex + 1) * const_GridLengthX)&&
				((*soldier)->getPosition().y >= gridYIndex * const_GridLengthY &&
				(*soldier)->getPosition().y < (gridYIndex + 1) * const_GridLengthY))
			{
				(*soldier)->_gridIndex = GridIndex;
				(*soldier)->_gridXIndex = gridXIndex;
				(*soldier)->_gridYIndex = gridYIndex;
				soldier = _TotalSoldier.erase(soldier);
			}
			else
			{
				soldier++;
			}
		}
		gridXIndex++;
		GridIndex++;
	}
}


//@Legion
Legion::Legion(int legionIndex, SoldierCamp soldierCamp):
_LegionIndex(legionIndex),
_SoldierCamp(soldierCamp)
{
	
}
void Legion::showSoldierStandSprite()
{
	for (auto& it : this->_legionSoldiers)
	{
		it->getChildByTag(BattleSoldierFactory::STAND_POINT_TAG)->setVisible(true);
	}
}
void Legion::hideSoldierStandSprite()
{
	for (auto& it : _legionSoldiers)
	{
		it->getChildByTag(BattleSoldierFactory::STAND_POINT_TAG)->setVisible(false);
	}
}
void Legion::addToLegion(BattleSoldierFactory* battleSoldier)
{
	_legionSoldiers.push_back(battleSoldier);
}
void Legion::startFormation(Vec2 generatePos)
{
	int currentRow=0;
	int currentColumn=1;
	Vec2 currentVec2=generatePos;
	FormationStartVec2 = currentVec2;
	for (auto it: _legionSoldiers)
	{
		if (it == this->_general)//軍團將軍
		{
			switch (this->_SoldierCamp)
			{
			case SoldierCamp::OurSoldier:
				it->setPosition(Vec2(currentVec2.x-const_OurLegionFormationSize.width,
					FormationStartVec2.y - (const_OurLegionFormationSize.height*const_ourLegionFormationRow)/2));
				//我方軍團增加觸控事件
				this->addGeneralTouchListener(it);
				break;
			case SoldierCamp::EnemySoldier:
				it->setPosition(Vec2(currentVec2.x+const_EnemyLegionFormationSize.width*4,
					FormationStartVec2.y - (const_EnemyLegionFormationSize.height*const_enemyLegionFormationRow) / 2.5));
				break;
			}			
		}
		else//士兵
		{
			it->setPosition(currentVec2);
			switch (this->_SoldierCamp)
			{
			case SoldierCamp::OurSoldier:
				if (currentRow < const_ourLegionFormationRow-1)
				{
					currentVec2.x = currentVec2.x + const_OurLegionFormationSize.width;
					currentVec2.y = currentVec2.y - const_OurLegionFormationSize.height;
					currentRow++;
				}
				else
				{
					currentVec2.x = FormationStartVec2.x - currentColumn*const_OurLegionFormationSize.width * 2;
					currentVec2.y = FormationStartVec2.y;
					currentRow = 0;
					currentColumn++;
				}
				break;
			case SoldierCamp::EnemySoldier:
				
				if (currentRow < const_enemyLegionFormationRow - 1)
				{
					currentVec2.x = currentVec2.x + const_EnemyLegionFormationSize.width;
					currentVec2.y = currentVec2.y - const_EnemyLegionFormationSize.height;
					currentRow++;
				}
				else
				{
					currentVec2.x = FormationStartVec2.x + currentColumn*const_EnemyLegionFormationSize.width * 2;
					currentVec2.y = FormationStartVec2.y;
					currentRow = 0;
					currentColumn++;
				}
				break;
			}
		}
		//假定指定位子，就是當前位子。用來後面指定位子的算法。主要的目的是讓他知道陣型的樣子
		it->_assignPosition = it->getPosition();
	}
}
void Legion::addGeneralTouchListener(BattleSoldierFactory* general)
{
	//@touch
	//觸控軍團將軍事件
	auto generalTouchEvent = EventListenerTouchOneByOne::create();
	generalTouchEvent->onTouchBegan = [this](Touch* touch, Event* event)
	{
		//由於放大縮小 會讓茅點變更，預設點擊位子是依照Vec2(0,0)茅點去判斷
		//經過觸控點加上茅點*地圖大小，就是變更茅點後新的偏移量
		Vec2 battleMapAnchorPoint = BattleMap::getBattleMap()->getAnchorPoint();
		Size battleMapSize = BattleMap::getBattleMap()->getBoundingBox().size;
		battleMapSize.width = battleMapSize.width*battleMapAnchorPoint.x;
		battleMapSize.height = battleMapSize.height*battleMapAnchorPoint.y;
		//觸控點加上地圖偏移量=點擊在地圖上的Vec2
		Vec2 touchOnBattleMap = touch->getLocation() + (-BattleMap::getBattleMap()->getPosition());
		//縮放後的地圖大小
		touchOnBattleMap.x = touchOnBattleMap.x + battleMapSize.width;
		touchOnBattleMap.y = touchOnBattleMap.y + battleMapSize.height;
		//點擊將軍判斷
		if (ExtensionsAlgorithm::ContainsPoint(
			touchOnBattleMap, _general->getPosition()*BattleMap::getBattleMap()->getScale(), _general->getBoundingBox().size.width, _general->getBoundingBox().size.width))
		{
			//點擊將軍時，不算觸控場景
			
			//*從將軍那得知所屬軍團，並通知軍團執行selevtLegionSoldier
			Legion* selectedLegion = ((BattleSoldierFactory*)event->getCurrentTarget())->_belongLegion;
			if (!selectedLegion->_isSelected)
			{
				AudioEngine::play2d("sound/selected.mp3", false);
				selectedLegion->showSoldierStandSprite();
			}
			else
			{
				AudioEngine::play2d("sound/waitingCommand.mp3", false);
				selectedLegion->hideSoldierStandSprite();
			}
			selectedLegion->_isSelected = !selectedLegion->_isSelected;
		}
		return false;
	};
	Director::getInstance()->getEventDispatcher()->addEventListenerWithSceneGraphPriority(generalTouchEvent, this->_general);
	//Director::getInstance()->getEventDispatcher()->addEventListenerWithFixedPriority(generalTouchEvent, 3);
}
int SoldierAI::getOurSoldierSelectedTotal()
{
	int selectedSum = 0;
	for (auto& it : _ourLegions)
	{
		if (it->_isSelected)
		{
			selectedSum++;
		}
	}
	return selectedSum;
}
int SoldierAI::countHowManyLegionGetSelected()
{
	int count = 0;
	for (auto& it : _ourLegions)
	{
		if (it->_isSelected)
		{
			count++;
		}
	}
	return count;
}
void SoldierAI::updateSelectedLegionFlagTargetPosition(Vec2 FlagPos)
{
	int HowManyOurLegionGetSelected = this->countHowManyLegionGetSelected();
	int index = 0;
	Vec2 lastGeneralAssignPos;
	for (auto& it : _ourLegions)
	{
		if (it->_isSelected)
		{
			//下完指令後取消選擇狀態
			it->_isSelected = false;
			it->hideSoldierStandSprite();
			it->_targetFlagPosition = FlagPos;
			Vec2 GeneralToFlagPosVec2= it->_targetFlagPosition - it->_general->_assignPosition;
			if (HowManyOurLegionGetSelected > 1 && index == 0)
			{//當選擇軍團數超過1個以上時 把當前將軍指定位子存起來，留給下個軍團指定時的相對位子，以維持個軍隊間的陣型
				lastGeneralAssignPos = it->_general->_assignPosition;
			}
			for (auto& legionSoldier : it->_legionSoldiers)
			{
				//如果選擇的軍隊 大於一隊以上 Y軸不變
				if (HowManyOurLegionGetSelected > 1)
				{
					legionSoldier->_assignPosition.x += GeneralToFlagPosVec2.x- (lastGeneralAssignPos.x-it->_general->_assignPosition.x);
					legionSoldier->_assignPosition.y += 0;
				}
				else//只有選擇一隊軍隊 正常指定地點
					legionSoldier->_assignPosition+=GeneralToFlagPosVec2;
				legionSoldier->_structSoldierState.isArriveAssignPosition = false;
			}
			index++;
		}
	}
}
void SoldierAI::updateMusketSoldierReadyToShootState()
{
	for (auto& it : _ourLegions)
	{
		if (it->_isSelected)
		{
			for (auto& legionSoldier : it->_legionSoldiers)
			{
				if (legionSoldier->_soliderType == SoldierType::Musket )
				{//轉型成musketSoldier子類  才能使用musketSoldier獨有的功能
					MusketSoldier* musketSoldier = static_cast<MusketSoldier*>(legionSoldier);
					//非走路 非攻擊狀態 並且身上有彈藥 才能舉槍
					if (musketSoldier->_structExtensionsSoldierState.HaveAmmo && 
						!musketSoldier->_structSoldierState.isAttack &&
						!musketSoldier->_structSoldierState.isRun)
					{
						musketSoldier->resetAllSoldierState();
						musketSoldier->_structExtensionsSoldierState.isReadyToShoot = true;
						musketSoldier->ReadyToShootMethod();
					}
				}
			}
		}
	}
}
void SoldierAI::updateMusketSoldierShoot()
{
	for (auto& it : _ourLegions)
	{
		if (it->_isSelected)
		{
			for (auto& legionSoldier : it->_legionSoldiers)
			{
				if (legionSoldier->_soliderType == SoldierType::Musket && 
					!legionSoldier->_structSoldierState.isRun)
				{//轉型成musketSoldier子類  才能使用musketSoldier獨有的功能
					MusketSoldier* musketSoldier = static_cast<MusketSoldier*>(legionSoldier);
					//需要再舉槍動作下 才能開槍
					if (musketSoldier->_structExtensionsSoldierState.isReadyToShoot)
					{
						musketSoldier->resetAllSoldierState();
						musketSoldier->_structExtensionsSoldierState.isShoot = true;
						musketSoldier->ShootMethod();
					}
				}
			}
		}
	}
}
NS_BATTLEMODE_END