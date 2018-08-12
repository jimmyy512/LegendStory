#include "ScrollMonsterFactory.h"
#include "tools/AnimateTool.h"
#include "UI/ScrollMode/HUDlayer.h"
#include "MainCharacter/MainCharacter.h"
#include "RPGTools\RPGTool.h"
#include "tools\JsonParser.h"
#include "tools\AudioManagement.h"
#include "Scene\ScrollMode\ScrollSceneFactory.h"
#include "TipsLabel.h"
USING_NS_CC;
NS_SCROLLMODE_BEGAN
using namespace MyTools;
ScrollMonster * ScrollMonsterFactory::createMonster(MonsterName monsterName,bool isShowCase)
{
	ScrollMonster* newMonster = nullptr;
	switch (monsterName)
	{
	case ScrollMonsterFactory::MonsterName::Zombie:
		newMonster = Zombie::create(isShowCase);
		break;
	case ScrollMonsterFactory::MonsterName::SmallThief:
		newMonster = SmallThief::create(isShowCase);
		break;
	default:
		CCAssert(false, "createScrollMonster fail");
	}
	return newMonster;
}

ScrollMonster::ScrollMonster()
{
	this->_lastPosition = Vec2(-9999, -9999);
	this->_thinkInterval = (rand() % 1001 + 500) / 100.f;
	this->_thinkInterval2 = (rand() % 1301 + 700) / 100.f;
}

ScrollMonster::~ScrollMonster()
{
	_armatureDisplay->dispose();
}

bool ScrollMonster::init(bool isShowCase)
{
	if (!Node::init())
	{
		return false;
	}
	this->setAnchorPoint(Vec2(0.5, 0));

	if (!isShowCase)
	{
		//physics body
		_physicsBody = PhysicsBody::createCircle(20, PHYSICSBODY_MATERIAL_DEFAULT);
		_physicsBody->getShape(0)->setMass(0.f);
		_physicsBody->getShape(0)->setRestitution(0.f);
		_physicsBody->getShape(0)->setDensity(1.0f);
		_physicsBody->getShape(0)->setFriction(100.f);
		_physicsBody->setRotationEnable(false);
		_physicsBody->setGravityEnable(true);
		_physicsBody->setCategoryBitmask(0x01);
		_physicsBody->setCollisionBitmask(0x02);
		_physicsBody->setContactTestBitmask(0xFFFFFFFF);
		this->setPhysicsBody(_physicsBody);

		//hp status
		auto hpBar = Sprite::create("ui/monsterHP_Bar.png");
		hpBar->setPosition(this->_hpBarOffset.x, this->_hpBarOffset.y);
		hpBar->setVisible(false);
		this->addChild(hpBar, 2, monsterHP_Bar);

		auto hp = ProgressTimer::create(Sprite::create("ui/monsterHp.png"));
		hp->setType(ProgressTimer::Type::BAR);
		hp->setMidpoint(Vec2(0, 0.5));
		hp->setBarChangeRate(Vec2(1, 0));
		hp->setPercentage(100);
		hp->setPosition(hpBar->getPosition().x, hpBar->getPosition().y);

		auto yellowHp = ProgressTimer::create(Sprite::create("ui/monsterYellowHp.png"));
		yellowHp->setType(ProgressTimer::Type::BAR);
		yellowHp->setMidpoint(Vec2(0, 0.5));
		yellowHp->setBarChangeRate(Vec2(1, 0));
		yellowHp->setPercentage(100);
		yellowHp->setPosition(hpBar->getPosition().x, hpBar->getPosition().y);

		hpBar->setScaleX(0.7);
		hpBar->setScaleY(0.5);
		hp->setScaleX(0.7);
		hp->setScaleY(0.5);
		yellowHp->setScaleX(0.7);
		yellowHp->setScaleY(0.7);
		hp->setVisible(false);
		yellowHp->setVisible(false);
		this->addChild(hp, 1, monsterHP);
		this->addChild(yellowHp, 0, monsterYellowHP);

		this->schedule(schedule_selector(ScrollMonster::update), 0.1f);
		//固定思考AI
		this->schedule(schedule_selector(ScrollMonster::ThinkAI), this->_thinkInterval);
		this->schedule(schedule_selector(ScrollMonster::ThinkAI2), this->_thinkInterval2);
	}
	else
	{
		this->setFaceDirection(true);
	}

	return true;
}

void ScrollMonster::update(float dt)
{
	if (this->_armatureDisplay->getAnimation()->isCompleted() && !monsterStatus._isDeath)
	{//如果有動畫執行完畢
		if (monsterStatus._isHurt)
		{
			this->inVisibleMonsterHP_Bar();//隱藏血量條
			_physicsBody->getShape(0)->setFriction(100.f);//回復怪物摩擦力
		}
		this->resetAllMonsterState();
		if (monsterStatus._isDying)
		{
			monsterStatus._isDying = false;
			monsterStatus._isDeath = true;
		}
	}
	if (this->monsterStatus._isHurt)
	{
		this->updateYellowHP();
	}
	else if (this->monsterStatus._isDeath)
	{
		if (this->_armatureDisplay->getOpacity() > 10)
		{
			this->_armatureDisplay->setOpacity(this->_armatureDisplay->getOpacity() - 2);
		}
		else
		{
			this->removeFromParent();
		}
	}
}

void ScrollMonster::ThinkAI(float dt)
{
	if (this->monsterStatus._isIdle ||
		this->monsterStatus._isRun ||
		this->monsterStatus._isWalk)
	{
		//隨機動作
		auto randNum = rand() % 3;
		if (randNum == 0)
		{
			this->doIdleAnimation();
		}
		else if (randNum == 1)
		{
			this->doRunAnimation();
		}
		else
		{
			this->doWalkAnimation();
		}
	}
}
void ScrollMonster::ThinkAI2(float dt)
{
	if (this->monsterStatus._isIdle ||
		this->monsterStatus._isRun ||
		this->monsterStatus._isWalk)
	{
		//二分之一機率看會不會轉向
		auto randNum = rand() % 2;
		if (randNum)
		{
			this->setFaceDirection(true);
		}
		else
		{
			this->setFaceDirection(false);
		}
	}
}

void ScrollMonster::setFacialType(FacialType facialType)
{
	std::string tmpFacial;
	switch (facialType)
	{
	case FacialType::NM:
		tmpFacial = "NM";
		break;
	case FacialType::NT:
		tmpFacial = "NT";
		break;
	case FacialType::AG:
		tmpFacial = "AG";
		break;
	case FacialType::SD:
		tmpFacial = "SD";
		break;
	case FacialType::BL:
		tmpFacial = "BL";
		break;
	case FacialType::HP:
		tmpFacial = "HP";
		break;
	case FacialType::DO:
		tmpFacial = "DO";
		break;
	case FacialType::DT:
		tmpFacial = "DT";
		break;
	case FacialType::DB:
		tmpFacial = "DB";
		break;
	case FacialType::CF:
		tmpFacial = "CF";
		break;
	default:
		throw "NPC::setFacialType dont have this facial type";
	}
	if (this->_facial == tmpFacial)
		return;
	else
	{
		this->_facial = tmpFacial;
		this->updateArmatureFacial();
	}
}

void ScrollMonster::setFacialType(std::string facialNameStr)
{
	std::string tmpFacial;
	if (facialNameStr == std::string("NM"))
	{
		tmpFacial = "NM";
	}
	else if (facialNameStr == std::string("NT"))
	{
		tmpFacial = "NT";
	}
	else if (facialNameStr == std::string("AG"))
	{
		tmpFacial = "AG";
	}
	else if (facialNameStr == std::string("SD"))
	{
		tmpFacial = "SD";
	}
	else if (facialNameStr == std::string("BL"))
	{
		tmpFacial = "BL";
	}
	else if (facialNameStr == std::string("HP"))
	{
		tmpFacial = "HP";
	}
	else if (facialNameStr == std::string("DO"))
	{
		tmpFacial = "DO";
	}
	else if (facialNameStr == std::string("DT"))
	{
		tmpFacial = "DT";
	}
	else if (facialNameStr == std::string("DB"))
	{
		tmpFacial = "DB";
	}
	else if (facialNameStr == std::string("CF"))
	{
		tmpFacial = "CF";
	}
	else
	{
		throw "NPC::setFacialType dont have this facial type.";
	}
	if (this->_facial == tmpFacial)
		return;
	else
	{
		this->_facial = tmpFacial;
		this->updateArmatureFacial();
	}
}

void ScrollMonster::updateArmatureFacial()
{
	if (this->_facial == "NM")
	{
		_armature->getSlot("Facial")->_setDisplayIndex(0);
	}
	else if (this->_facial == "NT")
	{
		_armature->getSlot("Facial")->_setDisplayIndex(1);
	}
	else if (this->_facial == "AG")
	{
		_armature->getSlot("Facial")->_setDisplayIndex(2);
	}
	else if (this->_facial == "SD")
	{
		_armature->getSlot("Facial")->_setDisplayIndex(3);
	}
	else if (this->_facial == "BL")
	{
		_armature->getSlot("Facial")->_setDisplayIndex(4);
	}
	else if (this->_facial == "HP")
	{
		_armature->getSlot("Facial")->_setDisplayIndex(5);
	}
	else if (this->_facial == "DO")
	{
		_armature->getSlot("Facial")->_setDisplayIndex(6);
	}
	else if (this->_facial == "DT")
	{
		_armature->getSlot("Facial")->_setDisplayIndex(7);
	}
	else if (this->_facial == "DB")
	{
		_armature->getSlot("Facial")->_setDisplayIndex(8);
	}
	else if (this->_facial == "CF")
	{
		_armature->getSlot("Facial")->_setDisplayIndex(9);
	}
	else
	{
		throw "no this facial condition please check!";
	}
}

void ScrollMonster::setFaceDirection(bool newFaceDirection)
{
	if (this->monsterStatus._faceDirection == newFaceDirection)
		return;
	else
	{
		this->monsterStatus._faceDirection = newFaceDirection;
		this->_armatureDisplay->setScaleX(-this->_armatureDisplay->getScaleX());
	}
}
void ScrollMonster::updateYellowHP()
{
	ProgressTimer* yellowhpBar = static_cast<ProgressTimer*>(this->getChildByTag(monsterYellowHP));
	yellowhpBar->setPercentage(yellowhpBar->getPercentage() - 2.f);
}

void ScrollMonster::updatePosition()
{
	if (this->monsterStatus._faceDirection)
	{
		if (this->monsterStatus._isWalk)
			this->setPositionX(this->getPositionX() - this->_walkSpeed);
		else
			this->setPositionX(this->getPositionX() - this->_walkSpeed*1.7);
	}
	else
	{
		if (this->monsterStatus._isWalk)
			this->setPositionX(this->getPositionX() + this->_walkSpeed);
		else
			this->setPositionX(this->getPositionX() + this->_walkSpeed*1.7);
	}
	if (fabs(this->getPosition().x - this->_prevPos.x) < 0.1f)
	{//撞牆了，轉換方向
		this->setFaceDirection(!this->monsterStatus._faceDirection);
	}
	this->_prevPos = this->getPosition();
}

void ScrollMonster::showMonsterHP_Bar()
{
	auto hpBar = this->getChildByTag(monsterHP_Bar);
	auto hp = this->getChildByTag(monsterHP);
	auto yellowhp = this->getChildByTag(monsterYellowHP);
	hpBar->setVisible(true);
	hp->setVisible(true);
	yellowhp->setVisible(true);
}
bool ScrollMonster::isXpointMoreThenMainCharacter()
{
	Vec2 mainCharacterPos = MainCharacter::getMainCharacter()->getPosition();
	if ( this->getPositionX() > mainCharacterPos.x)
		return true;
	else
		return false;
}
cocos2d::Vec2 ScrollMonster::getDistanceToMainCharacter()
{
	Vec2 vec2;
	vec2.x= fabs(MainCharacter::getMainCharacter()->getPosition().x - this->getPosition().x);
	vec2.y = fabs(MainCharacter::getMainCharacter()->getPosition().y - this->getPosition().y);
	return vec2;
}
void ScrollMonster::setHPByAttack(float attackDamage)
{
	ProgressTimer* hpBar = static_cast<ProgressTimer*>(this->getChildByTag(monsterHP));
	ProgressTimer* yellowhpBar = static_cast<ProgressTimer*>(this->getChildByTag(monsterYellowHP));
	this->_yellowHP = _currentHP;
	this->_currentHP -= attackDamage;
	float hpPercent = (this->_currentHP / this->_maxHP) * 100;
	float yellowhpPercent = (this->_yellowHP / this->_maxHP) * 100;
	hpBar->setPercentage(hpPercent);// 當前血量/總血量*100 =血量百分比
	yellowhpBar->setPercentage(yellowhpPercent);
}
void ScrollMonster::doAttackAnimation()
{
	this->resetAllMonsterState();
	//動畫速度
	_armature->getAnimation()->timeScale = 1.5f;
	this->monsterStatus._isAttack = true;
	this->_armatureDisplay->getAnimation()->play("Attack", 1);
	DelayTime* delaytime = DelayTime::create(this->getAttackTiming());
	CallFunc* callfunc = CallFunc::create([&]()
	{
		if (!this->monsterStatus._isHurt)
		{//發動攻擊途中如果仍然沒有被攻擊,就發出傷害訊息
			this->playAttackSound();
			this->AttackMethod();
		}
	});
	ActionInterval* Sequence = Sequence::create(delaytime, callfunc, NULL);
	this->_armatureDisplay->runAction(Sequence);
}
void ScrollMonster::AttackMethod()
{
	if (!this->monsterStatus._isHurt && 
		!ScrollScene::getScrollScene()->_maincharacter->status._isGodMode &&
		!ScrollScene::getScrollScene()->_maincharacter->status._isDying &&
		!this->monsterStatus._isDying)
	//如果自己怪物受傷，代表主角已經搶先攻擊怪物，就不發送主角受傷消息。
	//如果主角是無敵狀態也不發送主角受傷消息
	{
		if (this->isMainCharacterInAttackRange())
		{
			ScrollScene::getScrollScene()->_maincharacter->struct_infor._hp -= this->_damage;
			char newHPStr[10];
			if (ScrollScene::getScrollScene()->_maincharacter->struct_infor._hp <= 0)
			{//主角死亡
				ScrollScene::getScrollScene()->_maincharacter->doDyingAnimation();
				sprintf(newHPStr, "%d", 0);
				JsonParser::setSave("Infor", "HP", newHPStr);
			}
			else
			{//主角受傷
				sprintf(newHPStr, "%d", (int)ScrollScene::getScrollScene()->_maincharacter->struct_infor._hp);
				JsonParser::setSave("Infor", "HP", newHPStr);
				ScrollScene::getScrollScene()->_maincharacter->HurtAnimation(this->monsterStatus._faceDirection);
			}
			ScrollScene::getScrollScene()->_maincharacter->reviseDirection(!this->monsterStatus._faceDirection);
			HUDlayer::getHUDlayer()->refreshHUDlayer();
		}
	}
}

void ScrollMonster::checkIdleAnimation()
{
	if (!monsterStatus._isIdle && !monsterStatus._isWalk && !monsterStatus._isAttack &&
		!monsterStatus._isHurt && !monsterStatus._isDeath && !monsterStatus._isDying &&
		!monsterStatus._isRun)
	{
		this->doIdleAnimation();
	}
}

void ScrollMonster::doIdleAnimation()
{
	if (this->monsterStatus._isIdle)
		return;
	this->resetAllMonsterState();
	this->monsterStatus._isIdle = true;
	this->_armatureDisplay->getAnimation()->play("Idle", -1);
}

void ScrollMonster::doWalkAnimation()
{
	if (this->monsterStatus._isWalk)
		return;
	this->resetAllMonsterState();
	this->monsterStatus._isWalk = true;
	this->_armatureDisplay->getAnimation()->play("Walk", -1);
}

void ScrollMonster::doRunAnimation()
{
	if (this->monsterStatus._isRun)
		return;
	this->resetAllMonsterState();
	this->monsterStatus._isRun = true;
	this->_armatureDisplay->getAnimation()->play("Run", -1);
}

void ScrollMonster::doHurtAnimation(HurtStatus hurtState)
{
	this->resetAllMonsterState();
	this->showMonsterHP_Bar();
	monsterStatus._isHurt = true;
	this->_armatureDisplay->getAnimation()->play("Hurt", 1);
	repelWithPhysicsBody(hurtState);
}

void ScrollMonster::doDyingAnimation()
{
	this->resetAllMonsterState();
	this->inVisibleMonsterHP_Bar();
	monsterStatus._isDying = true;
	this->_armatureDisplay->getAnimation()->play("Dying", 1);
}

void ScrollMonster::repelWithPhysicsBody(HurtStatus hurtState)
{
	_physicsBody->getShape(0)->setFriction(1.f);//先把怪物的摩擦力降低，讓怪物受傷時能飛滑動
	int VelocityX;
	int VelocityY;
	switch (hurtState)
	{
	case HurtStatus::ShortBackWard:
		if (this->isXpointMoreThenMainCharacter())
		{
			VelocityX = rand() % 5 + 15;
		}
		else
		{
			VelocityX = -(rand() % 5 + 15);
		}
		VelocityY = rand() % 5 + 10;//10~15
		break;
	case HurtStatus::MiddleBackWard:
		if (this->isXpointMoreThenMainCharacter())
		{
			VelocityX = rand() % 20 + 50;
		}
		else
		{
			VelocityX = -(rand() % 20 + 50);
		}
		VelocityY = rand() % 20 + 150;
		break;
	case HurtStatus::StrongBackWard:
		if (this->isXpointMoreThenMainCharacter())
		{
			VelocityX = rand() % 20 + 80;//80~100
		}
		else
		{
			VelocityX = -(rand() % 20 + 80);
		}
		VelocityY = rand() % 20 + 20;
		break;
	case HurtStatus::HighBackWard:
		if (this->isXpointMoreThenMainCharacter())
		{
			VelocityX = rand() % 40 + 80;//80~120
		}
		else
		{
			VelocityX = -(rand() % 40 + 80);//80~120
		}
		VelocityY = rand() % 40 + 100;
		break;
	default:
		throw "error no this condition.";
	}
	this->getPhysicsBody()->setVelocity(Vec2(VelocityX, VelocityY));
}

bool ScrollMonster::isMainCharacterInAttackRange()
{
	bool monsterXpointMoreThenMainCharacter = this->isXpointMoreThenMainCharacter();
	Vec2 monsterToCharacterDistance = this->getDistanceToMainCharacter();
	if ((monsterToCharacterDistance.x < _AttackRange.width &&
		monsterToCharacterDistance.y < _AttackRange.height) &&
		((this->monsterStatus._faceDirection == false && monsterXpointMoreThenMainCharacter == false) ||
		(this->monsterStatus._faceDirection == true && monsterXpointMoreThenMainCharacter == true)))
	{//主角在怪物的範圍內，並且主角在怪物的眼前
		return true;
	}
	else
		return false;
}
void ScrollMonster::resetAllMonsterState()
{
	monsterStatus._isAttack = false;
	monsterStatus._isIdle = false;
	monsterStatus._isWalk = false;
	monsterStatus._isHurt = false;
	monsterStatus._isDeath = false;
	monsterStatus._isRun = false;
	//動畫速度回復
	_armature->getAnimation()->timeScale = 1.0f;
}

void ScrollMonster::inVisibleMonsterHP_Bar()
{
	auto hpBar = this->getChildByTag(monsterHP_Bar);
	auto hp = this->getChildByTag(monsterHP);
	auto yellowhp = this->getChildByTag(monsterYellowHP);
	yellowhp->setVisible(false);
	hpBar->setVisible(false);
	hp->setVisible(false);
}

//Zombie
Zombie * Zombie::create(bool isShowCase)
{
	Zombie* ptr = new(std::nothrow)Zombie;
	if (ptr&& ptr->init(isShowCase))
	{
		ptr->autorelease();
	}
	else
	{
		CC_SAFE_DELETE(ptr);
	}
	return ptr;
}
Zombie::Zombie()
{
	//data init
	_nameCode = "ZB";
	_name = "Zombie";
	_chineseName = JsonParser::getJsonString("Code_Table", "ZB", JsonParser::JsonType::TableCode);
	this->_DialogCode = "ZombieTalk";


	_missionState = MissionMG::State::KillZombie;
	monsterStatus._faceDirection = true;//初始面對方向
	_maxHP = _currentHP = _yellowHP = 1500;
	_damage = 200;
	_AttackRange.width = 70.f;
	_AttackRange.height = 70.f;
	_hpBarOffset.x = 0.0f;
	_hpBarOffset.y = 120.0f;
	_walkSpeed = 0.3f;
	struct_Reward.exp = 16;
	struct_Reward.items.push_back(std::make_pair((int)RPGTool::ItemID::Weapon_Shadow, 1));
	struct_Reward.items.push_back(std::make_pair((int)RPGTool::ItemID::Weapon_NoobSword, 15));
	struct_Reward.items.push_back(std::make_pair((int)RPGTool::ItemID::Weapon_Fist_Blue, 5));
	struct_Reward.items.push_back(std::make_pair((int)RPGTool::ItemID::Cloth_BlueKnight, 5));
	struct_Reward.items.push_back(std::make_pair((int)RPGTool::ItemID::Pant_BlueKnight, 5));
	struct_Reward.items.push_back(std::make_pair((int)RPGTool::ItemID::Cloth_ChiuanJen, 20));
	struct_Reward.items.push_back(std::make_pair((int)RPGTool::ItemID::Pant_ChiuanJen, 20));
	struct_Reward.items.push_back(std::make_pair((int)RPGTool::ItemID::Cloth_Farmer, 10));
	struct_Reward.items.push_back(std::make_pair((int)RPGTool::ItemID::Pant_Farmer, 10));
	struct_Reward.items.push_back(std::make_pair((int)RPGTool::ItemID::Weapon_Sword, 10));
	struct_Reward.items.push_back(std::make_pair((int)RPGTool::ItemID::SmallRedPotion, 50));
	struct_Reward.items.push_back(std::make_pair((int)RPGTool::ItemID::SmallBluePotion, 50));
	
	
	//dragon bone
	_dragonBonesData = _factory.loadDragonBonesData("animate/monsters/zombie_ske.json");
	_factory.loadTextureAtlasData("animate/monsters/zombie_tex.json");
	_armatureDisplay = _factory.buildArmatureDisplay("Armature");
	_armature = _armatureDisplay->getArmature();
	_armatureDisplay->setScaleY(0.4f);
	_armatureDisplay->setScaleX(0.4f);
	_armatureDisplay->setPositionY(-22);
	this->addChild(_armatureDisplay);

	this->setFaceDirection(false);
}
float Zombie::getAttackTiming()
{
	return _armatureDisplay->getAnimation()->getLastAnimationState()->getTotalTime()/2*0.5;
}
void Zombie::playAttackSound()
{
	AudioManagement::getInstance()->play("sound/ZombieAttack.mp3", AudioManagement::AudioType::Sound, false);
}
void Zombie::playHurtSound()
{
	AudioManagement::getInstance()->play("sound/ZombieHurt.mp3", AudioManagement::AudioType::Sound, false);
}


//SmallThief
SmallThief * SmallThief::create(bool isShowCase)
{
	SmallThief* ptr = new(std::nothrow)SmallThief;
	if (ptr&& ptr->init(isShowCase))
	{
		ptr->autorelease();
	}
	else
	{
		CC_SAFE_DELETE(ptr);
	}
	return ptr;
}

SmallThief::SmallThief()
{
	//data init
	_nameCode = "ST";
	_name = "SmallThief";
	_chineseName = JsonParser::getJsonString("Code_Table", "ST", JsonParser::JsonType::TableCode);
	this->_DialogCode = "SmallThiefTalk";

	_missionState = MissionMG::State::KillSmallThief;
	monsterStatus._faceDirection = true;//初始面對方向
	_maxHP = _currentHP = _yellowHP = 300;
	_damage = 50;
	_AttackRange.width = 80.f;
	_AttackRange.height = 80.f;
	_hpBarOffset.x = 0.0f;
	_hpBarOffset.y = 120.0f;
	_walkSpeed = 0.3f;
	struct_Reward.exp = 4;
	struct_Reward.items.push_back(std::make_pair((int)RPGTool::ItemID::Weapon_NoobSword, 10));
	struct_Reward.items.push_back(std::make_pair((int)RPGTool::ItemID::Weapon_Fist_Blue, 3));
	struct_Reward.items.push_back(std::make_pair((int)RPGTool::ItemID::Weapon_Shadow, 1));
	struct_Reward.items.push_back(std::make_pair((int)RPGTool::ItemID::Cloth_BlueKnight, 5));
	struct_Reward.items.push_back(std::make_pair((int)RPGTool::ItemID::Pant_BlueKnight, 5));
	struct_Reward.items.push_back(std::make_pair((int)RPGTool::ItemID::Cloth_Farmer, 10));
	struct_Reward.items.push_back(std::make_pair((int)RPGTool::ItemID::Pant_Farmer, 10));
	struct_Reward.items.push_back(std::make_pair((int)RPGTool::ItemID::Weapon_Sword, 10));
	struct_Reward.items.push_back(std::make_pair((int)RPGTool::ItemID::Hair_Band, 15));
	struct_Reward.items.push_back(std::make_pair((int)RPGTool::ItemID::Hair_Explore, 15));
	struct_Reward.items.push_back(std::make_pair((int)RPGTool::ItemID::Hair_Tail, 15));
	struct_Reward.items.push_back(std::make_pair((int)RPGTool::ItemID::Hair_Hat, 15));
	struct_Reward.items.push_back(std::make_pair((int)RPGTool::ItemID::SmallRedPotion, 50));
	struct_Reward.items.push_back(std::make_pair((int)RPGTool::ItemID::SmallBluePotion, 50));

	//dragon bone
	_dragonBonesData = _factory.loadDragonBonesData("animate/monsters/SmallThief_ske.json");
	_factory.loadTextureAtlasData("animate/monsters/SmallThief_tex.json");
	_armatureDisplay = _factory.buildArmatureDisplay("Armature");
	_armature = _armatureDisplay->getArmature();
	_armatureDisplay->setScaleY(0.4f);
	_armatureDisplay->setScaleX(0.4f);
	_armatureDisplay->setPositionY(-18);
	this->addChild(_armatureDisplay);

	this->setFaceDirection(false);
}
float SmallThief::getAttackTiming()
{
	return _armatureDisplay->getAnimation()->getLastAnimationState()->getTotalTime() / 2*0.5;
}
void SmallThief::playAttackSound()
{
	AudioManagement::getInstance()->play("sound/SmallThiefAttack.mp3", AudioManagement::AudioType::Sound, false);
}
void SmallThief::playHurtSound()
{
	AudioManagement::getInstance()->play("sound/SmallThiefDeath.mp3", AudioManagement::AudioType::Sound, false);
}
////*assassin
//Assassin::Assassin()
//{
//	this->_actionNode->setScale(2.0f);
//	this->_actionNode->setAnchorPoint(Vec2(0.5f, 0.2f));
//	//設定一下動畫偏移量，將會再父類加上
//	_speed = (rand() % 89 + 60) / 100.f;
//	_maxHP = _currentHP = _yellowHP = 100;
//	_damage = 20;
//	_AttackRange.width = (rand() % 21 + 60);
//	_AttackRange.height = 100.f;
//	monsterStatus._faceDirection = false;
//	this->_physicsBodySize = Size(Size(100.f, 80.f));
//	struct_Reward.exp = 3;
//	struct_Reward.items.push_back(std::make_pair((int)RPGTool::ItemID::Weapon_NoobSword, 20));
//	struct_Reward.items.push_back(std::make_pair((int)RPGTool::ItemID::Cloth_BlueKnight, 5));
//	struct_Reward.items.push_back(std::make_pair((int)RPGTool::ItemID::Pant_BlueKnight, 5));
//	struct_Reward.items.push_back(std::make_pair((int)RPGTool::ItemID::Cloth_Farmer, 10));
//	struct_Reward.items.push_back(std::make_pair((int)RPGTool::ItemID::Pant_Farmer, 10));
//	struct_Reward.items.push_back(std::make_pair((int)RPGTool::ItemID::Weapon_Sword, 40));
//	struct_Reward.items.push_back(std::make_pair((int)RPGTool::ItemID::SmallRedPotion, 50));
//	struct_Reward.items.push_back(std::make_pair((int)RPGTool::ItemID::SmallBluePotion, 50));
//}
//Animate* Assassin::initAttackAnimation()
//{
//	return AnimateTool::getInstance()->setAnimation("assassinAttack", const_AssAssin_Attack, 1,ATTACK_TAG);
//}
//Animate* Assassin::initHurtAnimation()
//{
//	return AnimateTool::getInstance()->setAnimation("assassinHurt", const_AssAssin_Hurt, 1, HURT_TAG);
//}
//Animate* Assassin::initIdleAnimation()
//{
//	return AnimateTool::getInstance()->setAnimation("assassinIdle", const_AssAssin_Idle, -1, IDLE_TAG);
//}
//Animate* Assassin::initRunAnimation()
//{
//	return AnimateTool::getInstance()->setAnimation("assassinRun", const_AssAssin_Run, -1, RUN_TAG);
//}
//Animate* Assassin::initDeadAnimation()
//{
//	return AnimateTool::getInstance()->setAnimation("assassinDeath", const_AssAssin_Death, 1);
//}
//
//float Assassin::getUnitOnAttackAnimation()
//{
//	return const_AssAssin_Attack * 7;//第7張畫面攻擊
//}
NS_SCROLLMODE_END