#include "BattleSoldierFactory.h"
#include "Tools\AnimateTool.h"
#include "Monster\BattleMode\SoldierAI.h"
USING_NS_CC;
NS_BATTLEMODE_BEGAN
BattleSoldierFactory::BattleSoldierFactory() :
_hp(100.f)
, _morale(100.f)
, _attackDamage(20.f)
,_speedX(1.0f)
,_speedY(0.5f)
{
	_targetPositionOffest = (rand() % 2 == 1) ? rand()%7+1: -rand()%7+1;
	this->_standPointSprite = Sprite::create("ui/standPoint.png");
	this->_standPointSprite->setScale(0.4f);
	this->_standPointSprite->setOpacity(120.f);
	this->_standPointSprite->setAnchorPoint(Vec2(0.5, 0.5));
	this->_standPointSprite->setVisible(false);
	this->addChild(_standPointSprite, -1, STAND_POINT_TAG);//y軸越高 圖層越低 才有漸層感);
}
void BattleSoldierFactory::ActionNodeInit(char* fileName, const char* plistFile)
{
	Size actionSize = AnimateTool::getInstance()->getFirstAnimationFrameSize(fileName, plistFile);
	this->setContentSize(Size(actionSize.width, actionSize.height));
	this->getBoundingBox().setRect(0, 0, actionSize.width, actionSize.height);
	this->_actionNode = Sprite::createWithSpriteFrame(AnimateTool::getInstance()->getFirstSpriteFrame(fileName, plistFile));
	this->_actionNode->setAnchorPoint(Vec2(0.5,0.2));
	this->_actionNode->setPosition(Vec2(0, 0));
	this->addChild(_actionNode);
}
void BattleSoldierFactory::updateBattleSoldier()
{
	if (SoldierAI::getInstance()->_structAICommand.AttackCommand)
	{
		if (this->_target != nullptr &&
		   !this->_structSoldierState.isAttack &&
		   !this->_structSoldierState.isRun)
		{
			if (
				this->_soldierCamp==SoldierCamp::OurSoldier &&
             	fabs(this->_target->getPositionX() - this->getPositionX()) <= 60 &&//這邊素質要比MoveToTargetPosition裡的參數還要大
				fabs(this->_target->getPositionY() - this->getPositionY()) <= 20
				)//非攻擊、非跑步狀態時 發動攻擊
			{
				this->AttackMethod();
			}
			else if(
				this->_soldierCamp == SoldierCamp::EnemySoldier &&
				fabs(this->_target->getPositionX() - this->getPositionX()) <= 60 &&
				fabs(this->_target->getPositionY() - this->getPositionY()) <= 20)
			{
				this->AttackMethod();
			}
		}
	}
	CheckIdleState();
	this->setZOrder(1000 - this->getPositionY());//y軸越高 圖層越低 才有漸層感
	
}
void BattleSoldierFactory::CheckIdleState()
{
	if (!this->_structSoldierState.isAttack &&
		!this->_structSoldierState.isRun &&
		!this->_structSoldierState.isDeath &&
		!this->_structSoldierState.isIdle)
	{
		this->_structSoldierState.isIdle = true;
		if (this->_target != nullptr)
		{
			if (this->_target->getPositionX() < this->getPositionX())
				this->setFaceDirection(true);
			else
				this->setFaceDirection(false);
		}
		this->_actionNode->stopAllActions();
		this->_actionNode->runAction(this->IdleAnimation());
	}
}
void BattleSoldierFactory::checkGrid()
{
	const int const_MaxGridXIndex = SoldierAI::getInstance()->const_MaxGridXIndex;
	const int const_MaxGridYIndex = SoldierAI::getInstance()->const_MaxGridYIndex;
	const int const_GridLengthX = SoldierAI::getInstance()->const_GridLengthX;
	const int const_GridLengthY = SoldierAI::getInstance()->const_GridLengthY;
	if (this->getPosition().x < _gridXIndex*const_GridLengthX)
	{//左半邊
		_gridXIndex--;
		if (this->getPosition().y < _gridYIndex*const_GridLengthY)
		{//左下角
			this->_gridYIndex--;
		}
		else if (this->getPosition().y >= (_gridYIndex + 1)*const_GridLengthY)
		{//左上角
			this->_gridYIndex++;
		}
		else
		{//左邊
			
		}
	}
	else if (this->getPosition().x >= (_gridXIndex + 1)*const_GridLengthX)
	{//右半邊
		_gridXIndex++;
		if (this->getPosition().y < _gridYIndex*const_GridLengthY)
		{
			this->_gridYIndex--;
		}
		else if (this->getPosition().y >= (_gridYIndex + 1)*const_GridLengthY)
		{
			this->_gridYIndex++;
		}
		else
		{

		}
	}
	else
	{//中間
		if (this->getPosition().y < _gridYIndex*const_GridLengthY)
		{
			this->_gridYIndex--;
		}
		else if (this->getPosition().y >= (_gridYIndex + 1)*const_GridLengthY)
		{
			this->_gridYIndex++;
		}
		else
		{

		}
	}
	_gridIndex = _gridYIndex*const_MaxGridXIndex + _gridXIndex;
}
void BattleSoldierFactory::MoveToAssignPosition()
{
	if (!this->_structSoldierState.isRun)
	{
		RunMethod();
	}
	bool running = false;
	Vec2 targetPositionOffset;
	targetPositionOffset.x = this->_assignPosition.x;
	targetPositionOffset.y = this->_assignPosition.y;
	if (fabs(this->getPosition().x - targetPositionOffset.x) >= 2)
	{
		if (this->getPositionX() < targetPositionOffset.x)
		{
			this->setPositionX(this->getPositionX() + this->_speedX);
			this->setFaceDirection(false);
		}
		else
		{
			this->setPositionX(this->getPositionX() - this->_speedX);
			this->setFaceDirection(true);
		}
		running = true;
	}
	if (fabs(this->getPosition().y - targetPositionOffset.y) >= 2)
	{
		if (this->getPositionY() < targetPositionOffset.y)
			this->setPositionY(this->getPositionY() + this->_speedY);
		else
			this->setPositionY(this->getPositionY() - this->_speedY);
		running = true;
	}
	//到達指定地點後
	if (!running)
	{
		this->_structSoldierState.isArriveAssignPosition = true;
		this->_actionNode->stopActionByTag(BattleSoldierFactory::RUN_TAG);
		_structSoldierState.isRun = false;
	}
}
void BattleSoldierFactory::MoveToTargetPosition()
{
	if (this->_structSoldierState.isAttack)
	{	//攻擊狀態下 不跑步
		return;
	}
	if (!this->_structSoldierState.isRun)
	{
		RunMethod();
	}
	bool running = false;
	Vec2 targetPositionOffset;
	targetPositionOffset.x = _target->getPositionX() + _targetPositionOffest;
	targetPositionOffset.y = _target->getPositionY() + _targetPositionOffest;
	if (fabs(this->getPosition().x - targetPositionOffset.x) >= 40)
	{
		if (this->getPositionX() < targetPositionOffset.x)
		{
			this->setPositionX(this->getPositionX() + this->_speedX);
			this->setFaceDirection(false);
		}
		else
		{
			this->setPositionX(this->getPositionX() - this->_speedX);
			this->setFaceDirection(true);
		}
		running = true;
	}
	if (fabs(this->getPosition().y - targetPositionOffset.y) >= 10)
	{
		if (this->getPositionY() < targetPositionOffset.y)
			this->setPositionY(this->getPositionY() + this->_speedY);
		else
			this->setPositionY(this->getPositionY() - this->_speedY);
		running = true;
	}
	if (!running)
	{
		this->_actionNode->stopActionByTag(BattleSoldierFactory::RUN_TAG);
		_structSoldierState.isRun = false;
	}
}
void BattleSoldierFactory::RunMethod()
{
	resetAllSoldierState();
	this->_actionNode->stopAllActions();
	this->_actionNode->runAction(this->RunAnimation());
	_structSoldierState.isRun = true;
}
void BattleSoldierFactory::forwardPosition()
{
	if (!this->_structSoldierState.isRun)
	{
		RunMethod();
	}
	switch (this->_soldierCamp)
	{
	case SoldierCamp::OurSoldier:
		this->setPositionX(this->getPositionX() + this->_speedX);
		break;
	case SoldierCamp::EnemySoldier:
		this->setPositionX(this->getPositionX() - this->_speedX);
		break;
	default:
		CCASSERT(false, "impossible!!");
	}
}
void BattleSoldierFactory::stayYourPosition()
{
	if (this->_structSoldierState.isRun == false)
	{//已經停止跑步狀態下就return
		return;
	}
	else
	{//跑步狀態中的話  下達停止跑步命令
		this->_structSoldierState.isRun = false;
		this->_actionNode->stopActionByTag(BattleSoldierFactory::RUN_TAG);
	}
}
void BattleSoldierFactory::resetAllSoldierState()
{
	this->_structSoldierState.isRun = false;
	this->_structSoldierState.isIdle = false;
	this->_structSoldierState.isAttack = false;
	this->_structSoldierState.isDeath = false;
}
void BattleSoldierFactory::setFaceDirection(bool direction)
{
	if (this->_structSoldierState.faceDirection != direction)
	{
		this->_structSoldierState.faceDirection = direction;
		float scaleX=this->_actionNode->getScaleX();
		if (scaleX > 0)
			scaleX = 0 - scaleX;
		else
			scaleX = fabs(scaleX);
		this->_actionNode->setScaleX(scaleX);
	}
}
void BattleSoldierFactory::AttackMethod()
{
	this->_actionNode->stopAllActions();
	this->resetAllSoldierState();
	this->_structSoldierState.isAttack = true;
	if (this->_target->getPositionX() > this->getPositionX())
	{
		this->setFaceDirection(false);
	}
	else
		this->setFaceDirection(true);
	CallFunc* StartHurtcallFunc = CallFunc::create([&]()
	{
		if (this->_target != nullptr)
		{
			//如果攻擊動作做完，敵人還在攻擊範圍內，就給敵人造成傷害
			if (this->getPosition().distance(this->_target->getPosition()) <= 60)
			{
				this->_target->setHpByDamage(this->_attackDamage);
				//你也打我一拳,我也打你一拳
				this->_target->_target = this;
			}
		}
		this->_structSoldierState.isAttack = false;
	});
	ActionInterval* StartAttackSequence = Sequence::create(this->AttackAnimation(), StartHurtcallFunc, NULL);
	this->_actionNode->runAction(StartAttackSequence);
}

void BattleSoldierFactory::KillYourSelf()
{
	this->_actionNode->stopAllActions();
	CallFunc* StartDiecallFunc = CallFunc::create([&]()
	{
		this->removeFromParent();
	});
	ActionInterval* StartHurtSequence = Sequence::create(this->DeathAnimation(), StartDiecallFunc, NULL);
	this->_actionNode->runAction(StartHurtSequence);
}
void BattleSoldierFactory::setHpByDamage(float damage)
{
	this->_hp =_hp-damage;
}

//@ShieldSoldier
bool ShieldSoldier::init()
{
	ActionNodeInit("ShieldSoldierIdle", "animate/BattleMonsterAction/ShieldSoldierAction/ShieldSoldierIdle.plist");
	this->_soliderType = SoldierType::ShieldSoldier;
	this->_attackRange = this->_actionNode->getBoundingBox().size.width / 2;
	this->_speedX += (rand() % 2 == 1) ? rand_0_1() / 30 : -rand_0_1() / 30;
	this->_speedY += (rand() % 2 == 1) ? rand_0_1() / 10 : -rand_0_1() / 10;
	this->_structAnimationInterval.attackInterval = 
		0.15f + ((rand() % 2 == 1) ? rand_0_1() / 30 : -rand_0_1() / 30);
	this->_structAnimationInterval.IdleInterval = 
		0.15f+((rand() % 2 == 1)? rand_0_1() / 30 : -rand_0_1() / 30);
	this->_structAnimationInterval.RunInterval =
		0.15f + ((rand() % 2 == 1) ? rand_0_1() / 30 : -rand_0_1() / 30);
	this->_structAnimationInterval.DeathInterval = 0.2f;
	this->_attackDamage= 20 + (rand() % 2 == 1 ? rand() % 9 + 1 : -rand() % 9 + 1);
	return true;
}
Animate* ShieldSoldier::AttackAnimation()
{
	return AnimateTool::getInstance()->setAnimation("ShieldSoldierAttack"
		, this->_structAnimationInterval.attackInterval,1,ATTACK_TAG,false
		,"animate/BattleMonsterAction/ShieldSoldierAction/ShieldSoldierAttack.plist");
}
Animate* ShieldSoldier::RunAnimation()
{
	return AnimateTool::getInstance()->setAnimation("ShieldSoldierRun"
		, this->_structAnimationInterval.RunInterval, -1, RUN_TAG,true
		, "animate/BattleMonsterAction/ShieldSoldierAction/ShieldSoldierRun.plist");
}
Animate* ShieldSoldier::IdleAnimation()
{
	return AnimateTool::getInstance()->setAnimation("ShieldSoldierIdle"
		, this->_structAnimationInterval.IdleInterval, -1, IDLE_TAG,true
		, "animate/BattleMonsterAction/ShieldSoldierAction/ShieldSoldierIdle.plist");
}
Animate* ShieldSoldier::DeathAnimation()
{
	return AnimateTool::getInstance()->setAnimation("ShieldSoldierDeath"
		, this->_structAnimationInterval.DeathInterval, 1, DEATH_TAG,true
		, "animate/BattleMonsterAction/ShieldSoldierAction/ShieldSoldierDeath.plist");
}
float ShieldSoldier::getUnitOnAttackAnimation()
{
	return this->_structAnimationInterval.attackInterval * 8;
}
//@MusketSoldier
bool MusketSoldier::init()
{
	ActionNodeInit("soldierIdle", "animate/soldierAction/soldierIdle.plist");
	//this->_speedX+= (rand() % 2==1)?rand_0_1()/10: -rand_0_1() / 10;
	//this->_speedY += (rand() % 2 == 1) ? rand_0_1() / 5 : -rand_0_1() / 5;
	this->_actionNode->setScaleX(0.5);
	this->_actionNode->setScaleY(0.4);
	this->_actionNode->setPosition(0, 20);
	this->_attackRange = this->_actionNode->getBoundingBox().size.width / 2;
	this->_soliderType = SoldierType::Musket;
	this->_structAnimationInterval.attackInterval = 
		0.1f+ ((rand() % 2 == 1) ? rand_0_1() / 30 : -rand_0_1() / 30);
	this->_structAnimationInterval.IdleInterval = 
		0.1f+ ((rand() % 2 == 1) ? rand_0_1() / 30 : -rand_0_1() / 30);
	this->_structAnimationInterval.RunInterval = 
		0.1f + ((rand() % 2 == 1) ? rand_0_1() / 30 : -rand_0_1() / 30);
	this->_structAnimationInterval.DeathInterval = 0.1f;
	this->_structExtensionsAnimationInterval.ReadyToShootInterval = 
		0.15f + ((rand() % 2 == 1) ? rand_0_1() / 30 : -rand_0_1() / 30);
	this->_structExtensionsAnimationInterval.ReloadInterval = 
		0.15f + ((rand() % 2 == 1) ? rand_0_1() / 30 : -rand_0_1() / 30);
	this->_structExtensionsAnimationInterval.ShootInterval =
		0.15f + ((rand() % 2 == 1) ? rand_0_1() / 30 : -rand_0_1() / 30);
	this->_attackDamage = 20 + (rand() % 2 == 1 ? rand() % 9 + 1 : -rand() % 9 + 1);
	this->_shootDamage = 50 + (rand() % 2 == 1 ? rand() % 9 + 1 : -rand() % 9 + 1);
	//一開始士兵槍枝都已填充完畢
	this->_structExtensionsSoldierState.HaveAmmo = true;
	return true;
}
Animate* MusketSoldier::AttackAnimation()
{
	return AnimateTool::getInstance()->setAnimation("soldierAttack"
		, this->_structAnimationInterval.attackInterval, 1, ATTACK_TAG, false
		, "animate/soldierAction/soldierAttack.plist");
}
Animate* MusketSoldier::RunAnimation()
{
	return AnimateTool::getInstance()->setAnimation("soldierWalk"
		, this->_structAnimationInterval.RunInterval, -1, RUN_TAG, true
		, "animate/soldierAction/soldierWalk.plist");
}
Animate* MusketSoldier::IdleAnimation()
{
	return AnimateTool::getInstance()->setAnimation("soldierIdle"
		, this->_structAnimationInterval.IdleInterval, -1, IDLE_TAG, true
		, "animate/soldierAction/soldierIdle.plist");
}
Animate* MusketSoldier::DeathAnimation()
{
	return AnimateTool::getInstance()->setAnimation("soldierDead"
		, this->_structAnimationInterval.DeathInterval, 1, DEATH_TAG, true
		, "animate/soldierAction/soldierDead.plist");
}
cocos2d::Animate * MusketSoldier::soldierReadyToShoot()
{
	return AnimateTool::getInstance()->setAnimation("soldierReadyToShoot"
		, _structExtensionsAnimationInterval.ReadyToShootInterval,-1, "animate/soldierAction/soldierReadyToShoot.plist");
}
cocos2d::Animate * MusketSoldier::soldierShoot()
{
	return AnimateTool::getInstance()->setAnimation("soldierShoot"
		, _structExtensionsAnimationInterval.ShootInterval, 1, "animate/soldierAction/soldierShoot.plist");
}
cocos2d::Animate * MusketSoldier::soldierReload()
{
	return AnimateTool::getInstance()->setAnimation("soldierReload"
		, _structExtensionsAnimationInterval.ReloadInterval, 3, "animate/soldierAction/soldierReload.plist");
}
float MusketSoldier::getUnitOnAttackAnimation()
{
	return this->_structAnimationInterval.attackInterval * 8;
}
void  MusketSoldier::CheckIdleState()
{
	if (!this->_structSoldierState.isAttack &&
		!this->_structSoldierState.isRun &&
		!this->_structSoldierState.isDeath &&
		!this->_structSoldierState.isIdle &&
		!this->_structExtensionsSoldierState.isReadyToShoot &&
		!this->_structExtensionsSoldierState.isReload &&
		!this->_structExtensionsSoldierState.isShoot)
	{
		this->_structSoldierState.isIdle = true;
		if (this->_target != nullptr)
		{
			if (this->_target->getPositionX() < this->getPositionX())
				this->setFaceDirection(true);
			else
				this->setFaceDirection(false);
		}
		this->_actionNode->stopAllActions();
		//沒子彈時，待機情況下，做裝子彈的動作
		if (!this->_structExtensionsSoldierState.HaveAmmo)
		{
			CallFunc* callFunc = CallFunc::create([&]()
			{
				//裝完子彈後 執行待機動作
				this->_structExtensionsSoldierState.HaveAmmo = true;
				this->_actionNode->runAction(this->IdleAnimation());
			});
			ActionInterval* Sequence = Sequence::create(this->soldierReload(), callFunc, NULL);
			this->_actionNode->runAction(Sequence);
		}
		else
			this->_actionNode->runAction(this->IdleAnimation());
	}
}
void MusketSoldier::resetAllSoldierState()
{
	this->_structSoldierState.isRun = false;
	this->_structSoldierState.isIdle = false;
	this->_structSoldierState.isAttack = false;
	this->_structSoldierState.isDeath = false;
	this->_structExtensionsSoldierState.isReadyToShoot = false;
	this->_structExtensionsSoldierState.isReload = false;
	this->_structExtensionsSoldierState.isShoot = false;
}
void MusketSoldier::ReadyToShootMethod()
{
	this->_actionNode->stopAllActions();
	this->_actionNode->runAction(this->soldierReadyToShoot());
}

void MusketSoldier::ShootMethod()
{
	this->_actionNode->stopAllActions();
	CallFunc* callFunc = CallFunc::create([&]()
	{
		//如果攻擊動作做完，敵人還在攻擊範圍內，就給敵人造成傷害
		if (this->_target != nullptr)
		{
			//if (this->getPosition().distance(this->_target->getPosition()) <= 50)
			{
				this->_target->setHpByDamage(_shootDamage);
			}
		}
		this->_structExtensionsSoldierState.isShoot = false;
		this->_structExtensionsSoldierState.HaveAmmo = false;
		
	});
	ActionInterval* Sequence = Sequence::create(this->soldierShoot(), callFunc, NULL);
	this->_actionNode->runAction(Sequence);
}

//@Lyonar
bool Lyonar::init()
{
	ActionNodeInit("lyonarBreath", "animate/lyonarAction/lyonarBreath.plist");
	this->_soliderType = SoldierType::Lyonar;
	this->_actionNode->setScale(0.9);
	this->_attackRange = this->_actionNode->getBoundingBox().size.width / 2;
	this->_structAnimationInterval.attackInterval =
		0.1 + ((rand() % 2 == 1) ? rand_0_1() / 30 : -rand_0_1() / 30);
	this->_structAnimationInterval.IdleInterval =
		0.15 + ((rand() % 2 == 1) ? rand_0_1() / 30 : -rand_0_1() / 30);
	this->_structAnimationInterval.RunInterval =
		0.15 + ((rand() % 2 == 1) ? rand_0_1() / 30 : -rand_0_1() / 30);
	this->_structAnimationInterval.DeathInterval = 0.15f;
	this->_attackDamage = 20 + (rand() % 2 == 1 ? rand() % 9 + 1 : -rand() % 9 + 1);
	return true;
}
Animate* Lyonar::AttackAnimation()
{
	return AnimateTool::getInstance()->setAnimation("lyonarSwingSword"
		, this->_structAnimationInterval.attackInterval, 1, ATTACK_TAG, false
		, "animate/lyonarAction/lyonarSwingSword.plist");
}
Animate* Lyonar::RunAnimation()
{
	return AnimateTool::getInstance()->setAnimation("lyonarRun"
		, this->_structAnimationInterval.RunInterval, -1, RUN_TAG, true
		, "animate/lyonarAction/lyonarRun.plist");
}
Animate* Lyonar::IdleAnimation()
{
	return AnimateTool::getInstance()->setAnimation("lyonarBreath"
		, this->_structAnimationInterval.IdleInterval, -1, IDLE_TAG, true
		, "animate/lyonarAction/lyonarBreath.plist");
}
Animate* Lyonar::DeathAnimation()
{
	return AnimateTool::getInstance()->setAnimation("lyonarDeath"
		, this->_structAnimationInterval.DeathInterval, 1, DEATH_TAG, true
		, "animate/lyonarAction/lyonarDeath.plist");
}
float Lyonar::getUnitOnAttackAnimation()
{
	return this->_structAnimationInterval.attackInterval * 10;
}

//@assassin
bool Assassin::init()
{
	ActionNodeInit("assassinIdle", "animate/monsterAction/assassin/assassinIdle.plist");
	this->_soliderType = SoldierType::Assassin;
	this->_attackRange = this->_actionNode->getBoundingBox().size.width / 2;
	this->_structAnimationInterval.attackInterval =
		0.15 + ((rand() % 2 == 1) ? rand_0_1() / 30 : -rand_0_1() / 30);
	this->_structAnimationInterval.IdleInterval =
		0.15 + ((rand() % 2 == 1) ? rand_0_1() / 30 : -rand_0_1() / 30);
	this->_structAnimationInterval.RunInterval =
		0.15 + ((rand() % 2 == 1) ? rand_0_1() / 100 : -rand_0_1() / 100);
	this->_structAnimationInterval.DeathInterval = 
		0.15 + ((rand() % 2 == 1) ? rand_0_1() / 50 : -rand_0_1() / 50);
	this->_attackDamage = 20 + (rand() % 2 == 1 ? rand() % 9 + 1 : -rand() % 9 + 1);
	return true;
}
Animate* Assassin::AttackAnimation()
{
	return AnimateTool::getInstance()->setAnimation("assassinAttack"
		, this->_structAnimationInterval.attackInterval, 1, ATTACK_TAG, false
		, "animate/monsterAction/assassin/assassinAttack.plist");
}
Animate* Assassin::RunAnimation()
{
	return AnimateTool::getInstance()->setAnimation("assassinRun"
		, this->_structAnimationInterval.RunInterval, -1, RUN_TAG, true
		, "animate/monsterAction/assassin/assassinRun.plist");
}
Animate* Assassin::IdleAnimation()
{
	return AnimateTool::getInstance()->setAnimation("assassinIdle"
		, this->_structAnimationInterval.IdleInterval, -1, IDLE_TAG, true
		, "animate/monsterAction/assassin/assassinIdle.plist");
}
Animate* Assassin::DeathAnimation()
{
	return AnimateTool::getInstance()->setAnimation("assassinDeath"
		, this->_structAnimationInterval.DeathInterval, 1, DEATH_TAG, true
		, "animate/monsterAction/assassin/assassinDeath.plist");
}
float Assassin::getUnitOnAttackAnimation()
{
	return this->_structAnimationInterval.attackInterval * 10;
}
NS_BATTLEMODE_END