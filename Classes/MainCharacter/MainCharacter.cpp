#include "MainCharacter.h"
#include "tools/AnimateTool.h"
#include "Monster/ScrollMode/ScrollMonsterFactory.h"
#include "Map/ScrollMode/ScrollMap.h"
#include "Scene/ScrollMode/ScrollSceneFactory.h"
#include "UI/ScrollMode/HUDlayer.h"
#include "Monster/ScrollMode/MonsterAI.h"
#include "DefinitionScrollMode.h"
#include "Tools\JsonParser.h"
#include "Tools\AudioManagement.h"
#include "RPGTools\RPGTool.h"
#include "RPGTools\SkillMG.h"
#include "UI\DamageMG.h"
#include "Npc\NPC_AI.h"
#include "Npc\NPCFactory.h"
#include "Scene\FailScene.h"
USING_NS_CC;
NS_SCROLLMODE_BEGAN
using namespace MyTools;
namespace
{
	//old normal
	//const float const_NormalAttack1Timing = 0.67f;
	//const float const_NormalAttack2Timing = 0.63f;
	//const float const_NormalAttack3Timing = 1.0f;
	//const float const_NormalSkill1Timing = 0.92f;
	//const float const_NormalSkill2Timing = 0.8f;
	//const float const_NormalSkill3Timing = 1.0f;

	//old dragon
	/*const float const_DragonAttack1Timing = 0.67f;
	const float const_DragonAttack2Timing = 0.63f;
	const float const_DragonAttack3Timing = 1.0f;
	const float const_DragonSkill1Timing = 0.92f;
	const float const_DragonSkill2Timing = 1.2f;
	const float const_DragonSkill3Timing = 1.8f;*/

	//new normal
	const float const_NormalAttack1Timing = 0.45f;
	const float const_NormalAttack2Timing = 0.43f;
	const float const_NormalAttack3Timing = 0.67f;
	const float const_NormalSkill1Timing = 0.62f;
	const float const_NormalSkill2Timing = 0.6f;
	const float const_NormalSkill3Timing = 0.66f;

	//new dragon
	const float const_DragonAttack1Timing = 0.45f;
	const float const_DragonAttack2Timing = 0.42f;
	const float const_DragonAttack3Timing = 0.67f;
	const float const_DragonSkill1Timing = 0.62f;
	const float const_DragonSkill2Timing = 0.8f;
	const float const_DragonSkill3Timing = 1.2f;
}
EXCharacter::EXCharacter()
{
}
EXCharacter::~EXCharacter()
{
	_armatureDisplay->dispose();
}
dragonBones::CCArmatureDisplay * EXCharacter::init(MainCharacter* mainCharacter)
{
	_mainCharacter = mainCharacter;
	_dragonBonesData = _factory.loadDragonBonesData("CharacterAssert/character_ske.json");
	
	_factory.loadTextureAtlasData("CharacterAssert/character_tex.json");
	
	_armatureDisplay = _factory.buildArmatureDisplay("MainCharacter");
	_armature = _armatureDisplay->getArmature();
	_armatureDisplay->setScale(0.27f);
	_armatureDisplay->setPositionY(_armatureDisplay->getPositionY() - _mainCharacter->visible.height / 15);
	this->setAllEquipMentAccordingToSave();
	return _armatureDisplay;
}
void EXCharacter::updateWeapon()
{
	_WeaponIndex = JsonParser::getEquipmentFromSave((int)RPGTool::CharacterPart::Weapon, JsonParser::Equipment::GetID);
	if (_WeaponIndex ==(int)RPGTool::ItemID::Weapon_Fist)
	{
		_armature->getSlot("Weapon")->setDisplayIndex(-1);
		JsonParser::setSave("Infor","SkillType","1");
	}
	else if (_WeaponIndex == (int)RPGTool::ItemID::Weapon_Fist_Blue)
	{
		_armature->getSlot("Weapon")->setDisplayIndex(-1);
		JsonParser::setSave("Infor", "SkillType", "1");
	}
	else
	{
		this->setCloth(_WeaponIndex);
		JsonParser::setSave("Infor", "SkillType", "0");
	}
}
void EXCharacter::updateHair()
{
	_HairType = JsonParser::getEquipmentFromSave((int)RPGTool::CharacterPart::Hair, JsonParser::Equipment::GetID);
	this->setCloth(_HairType);
}
void EXCharacter::updateHead()
{
	_HeadType = JsonParser::getEquipmentFromSave((int)RPGTool::CharacterPart::Head, JsonParser::Equipment::GetID);
	this->setCloth(_HeadType);
}
void EXCharacter::play(const std::string & animationName, int playTimes, bool isAttackAnimation)
{
	_armatureDisplay->getAnimation()->play(animationName, playTimes);
	_armature->getAnimation()->timeScale = 1.5f;
	this->setAllEquipMentAccordingToSave();
	updateHair();
	updateHead();
	if (isAttackAnimation)
	{
		DelayTime* delaytime = DelayTime::create(_mainCharacter->getAttackTiming(animationName));
		CallFunc* callfunc = CallFunc::create([&, animationName]()
		{
			if (!_mainCharacter->status._isHurt && _mainCharacter->status._isAttack)
			{
				_mainCharacter->AccordingAttackPlaySound(animationName);
				_mainCharacter->AttackMethod();
			}
			_mainCharacter->lockNormalClick = false;
		});
		ActionInterval* Sequence = Sequence::create(delaytime, callfunc, NULL);
		this->_armatureDisplay->runAction(Sequence);
	}
}
void EXCharacter::setAllEquipMentAccordingToSave()
{
	_HairType = JsonParser::getEquipmentFromSave((int)RPGTool::CharacterPart::Hair, JsonParser::Equipment::GetID);
	_HeadType = JsonParser::getEquipmentFromSave((int)RPGTool::CharacterPart::Head, JsonParser::Equipment::GetID);
	_ClothType = JsonParser::getEquipmentFromSave((int)RPGTool::CharacterPart::Cloth, JsonParser::Equipment::GetID);
	_PantType = JsonParser::getEquipmentFromSave((int)RPGTool::CharacterPart::Pant, JsonParser::Equipment::GetID);
	this->setCloth(_HairType);
	this->setCloth(_HeadType);
	this->setCloth(_ClothType);
	this->setCloth(_PantType);
	updateWeapon();
}
void EXCharacter::setArmatureFacial(std::string newFacial)
{
	if (newFacial == "NM")
	{
		_armature->getSlot("Facial")->_setDisplayIndex(0);
	}
	else if (newFacial == "NT")
	{
		_armature->getSlot("Facial")->_setDisplayIndex(1);
	}
	else if (newFacial == "AG")
	{
		_armature->getSlot("Facial")->_setDisplayIndex(2);
	}
	else if (newFacial == "SD")
	{
		_armature->getSlot("Facial")->_setDisplayIndex(3);
	}
	else if (newFacial == "BL")
	{
		_armature->getSlot("Facial")->_setDisplayIndex(4);
	}
	else if (newFacial == "HP")
	{
		_armature->getSlot("Facial")->_setDisplayIndex(5);
	}
	else if (newFacial == "DO")
	{
		_armature->getSlot("Facial")->_setDisplayIndex(6);
	}
	else if (newFacial == "DT")
	{
		_armature->getSlot("Facial")->_setDisplayIndex(7);
	}
	else if (newFacial == "DB")
	{
		_armature->getSlot("Facial")->_setDisplayIndex(8);
	}
	else if (newFacial == "CF")
	{
		_armature->getSlot("Facial")->_setDisplayIndex(9);
	}
	else
	{
		throw "character no this facial condition please check!";
	}
}
void EXCharacter::setCloth(int itemID)
{
	int ClothIndex = -1;
	int pantIndex = -1;
	int weaponIndex = -1;
	switch (RPGTool::ItemID(itemID))
	{
	case RPGTool::ItemID::Cloth_ChiuanJen:
		ClothIndex = 0;
		break;
	case RPGTool::ItemID::Cloth_Farmer:
		ClothIndex = 1;
		break;
	case RPGTool::ItemID::Cloth_Naked:
		ClothIndex = 2;
		break;
	case RPGTool::ItemID::Cloth_BlueKnight:
		ClothIndex = 3;
		break;
	case RPGTool::ItemID::Cloth_Blody:
		ClothIndex = 4;
		break;
	case RPGTool::ItemID::Hair_Explore:
		_armature->getSlot("Hair")->setDisplayIndex(1);
		break;
	case RPGTool::ItemID::Hair_Tail:
		_armature->getSlot("Hair")->setDisplayIndex(0);
		break;
	case RPGTool::ItemID::Hair_Blade:
		_armature->getSlot("Hair")->setDisplayIndex(-1);
		break;
	case RPGTool::ItemID::Hair_Band:
		_armature->getSlot("Hair")->setDisplayIndex(2);
		break;
	case RPGTool::ItemID::Hair_Hat:
		_armature->getSlot("Hair")->setDisplayIndex(3);
		break;
	case RPGTool::ItemID::Head_Goat:
		_armature->getSlot("Head")->setDisplayIndex(1);
		break;
	case RPGTool::ItemID::Head_Normal:
		_armature->getSlot("Head")->setDisplayIndex(0);
		break;
	case RPGTool::ItemID::Pant_ChiuanJen:
		pantIndex = 0;
		break;
	case RPGTool::ItemID::Pant_Farmer:
		pantIndex = 1;
		break;
	case RPGTool::ItemID::Pant_Naked:
		pantIndex = 2;
		break;
	case RPGTool::ItemID::Pant_BlueKnight:
		pantIndex = 3;
		break;
	case RPGTool::ItemID::Pant_Blody:
		pantIndex = 4;
		break;
	case RPGTool::ItemID::Weapon_Blody:
		weaponIndex = 3;
		break;
	case RPGTool::ItemID::Weapon_Shadow:
		weaponIndex = 2;
		break;
	case RPGTool::ItemID::Weapon_Sword:
		weaponIndex = 1;
		break;
	case RPGTool::ItemID::Weapon_NoobSword:
		weaponIndex = 0;
		break;
	}
	//change slot
	if (ClothIndex != -1)
	{
		_armature->getSlot("UpBody")->setDisplayIndex(ClothIndex);
		_armature->getSlot("RightUpArm")->setDisplayIndex(ClothIndex);
		_armature->getSlot("RightDownArm")->setDisplayIndex(ClothIndex);
		_armature->getSlot("LeftUpArm")->setDisplayIndex(ClothIndex);
		_armature->getSlot("LeftDownArm")->setDisplayIndex(ClothIndex);
		_armature->getSlot("DownBody")->setDisplayIndex(ClothIndex);
	}
	if (pantIndex != -1)
	{
		_armature->getSlot("RightBigLeg")->setDisplayIndex(pantIndex);
		_armature->getSlot("LeftBigLeg")->setDisplayIndex(pantIndex);
		_armature->getSlot("RightCalf")->setDisplayIndex(pantIndex);
		_armature->getSlot("LeftCalf")->setDisplayIndex(pantIndex);
		_armature->getSlot("RightFoot")->setDisplayIndex(pantIndex);
		_armature->getSlot("LeftFoot")->setDisplayIndex(pantIndex);
	}
	if (weaponIndex != -1)
	{
		_armature->getSlot("Weapon")->setDisplayIndex(weaponIndex);
	}
}
void EXCharacter::resetAllEquipMent()
{
	JsonParser::setEquipmentFromSave((int)RPGTool::ItemID::Hair_Tail, 0);
	JsonParser::setEquipmentFromSave((int)RPGTool::ItemID::Head_Normal, 0);
	JsonParser::setEquipmentFromSave((int)RPGTool::ItemID::Cloth_Farmer, 0);
	JsonParser::setEquipmentFromSave((int)RPGTool::ItemID::Pant_Farmer, 0);
	JsonParser::setEquipmentFromSave((int)RPGTool::ItemID::Weapon_NoobSword, 0);
	this->setAllEquipMentAccordingToSave();
}
//@MainCharacter
MainCharacter* MainCharacter::static_mainCharacter = nullptr;
MainCharacter * MainCharacter::getMainCharacter()
{
	return static_mainCharacter;
}
MainCharacter* MainCharacter::create(float rebornPosX, float rebornPosY,bool isShowCase)
{
	MainCharacter* pRet = new(std::nothrow) MainCharacter();
	if (pRet && pRet->initMainCharacter(rebornPosX, rebornPosY, isShowCase))
	{
		pRet->autorelease();
		return pRet;
	}
	else
	{
		delete pRet;
		pRet = nullptr;
		return nullptr;
	}
}
MainCharacter::MainCharacter()
{
	lockNormalClick = false;
	_isReplaceScene = false;
	status._isIdle=false;
	status._isRun = false;
	status._isAttack = false;
	status._isHurt = false;
	status._isJump = false;
	status._isDying = false;
	status._faceDirection = false;
	status._isGodMode = false;
	_normalAttackCount = 0;
	struct_infor._level=0.0f;
	struct_infor._exp = 0.0f;
	struct_infor._hp = 0.0f;
	struct_infor._mp = 0.0f;
	struct_infor._maxexp = 0.0f;
	struct_infor._maxhp = 0.0f;
	struct_infor._maxmp = 0.0f;
	struct_infor._damage = 0.0f;
	struct_infor._moveSpeed = 0.0f;
}
MainCharacter::~MainCharacter()
{
	//*移除所有觀察
	NotificationCenter::getInstance()->removeAllObservers(this);
	if (!this->_isShowCase)
		static_mainCharacter = nullptr;
	//init函數中有new 所以解構時delete
	delete _exCharacter;
}
bool MainCharacter::initMainCharacter(float rebornPosX, float rebornPosY, bool isShowCase)
{
	if (!Sprite::init())
	{
		return false;
	}
	visible = Director::getInstance()->getVisibleSize();
	origin = Director::getInstance()->getVisibleOrigin();
	if(!isShowCase)
		static_mainCharacter = this;
	this->_isShowCase = isShowCase;

	//dragonBones init
	_exCharacter = new EXCharacter;
	this->addChild(_exCharacter->init(this));

	//bornPos
	this->setAnchorPoint(Vec2(0.5, 0));
	this->setPosition(rebornPosX, rebornPosY);
	this->_startPosY = rebornPosY;

	if (!_isShowCase)
	{   //set physics body
		_CharacterPhysicsBody = PhysicsBody::createCircle(20, PHYSICSBODY_MATERIAL_DEFAULT);
		_CharacterPhysicsBody->getShape(0)->setMass(0.f);
		_CharacterPhysicsBody->getShape(0)->setRestitution(0.f);
		_CharacterPhysicsBody->getShape(0)->setDensity(1.0f);
		_CharacterPhysicsBody->getShape(0)->setFriction(100.f);
		_CharacterPhysicsBody->setRotationEnable(false);
		_CharacterPhysicsBody->setGravityEnable(true);
		_CharacterPhysicsBody->setCategoryBitmask(0x01);
		_CharacterPhysicsBody->setCollisionBitmask(0x01);
		_CharacterPhysicsBody->setContactTestBitmask(0xFFFFFFFF);
		this->setPhysicsBody(_CharacterPhysicsBody);
		//更新一些基本數據
		this->refreshBaseProperty();
		//攻擊範圍
		_characterAttackRange.width = 100.f;
		_characterAttackRange.height = 100.f;
	}

	if (!_isShowCase)
	{
		//消息接送 執行函數
		NotificationCenter::getInstance()->addObserver(this, callfuncO_selector(MainCharacter::AttackAnimation), "NormalAttack", NULL);
		NotificationCenter::getInstance()->addObserver(this, callfuncO_selector(MainCharacter::AttackAnimation), "Skill1", NULL);
		NotificationCenter::getInstance()->addObserver(this, callfuncO_selector(MainCharacter::AttackAnimation), "Skill2", NULL);
		NotificationCenter::getInstance()->addObserver(this, callfuncO_selector(MainCharacter::AttackAnimation), "Skill3", NULL);
		NotificationCenter::getInstance()->addObserver(this, callfuncO_selector(MainCharacter::JumpAnimation), "Jump", NULL);
		NotificationCenter::getInstance()->addObserver(this, callfuncO_selector(MainCharacter::FallDownAnimation), "Fall", NULL);
		NotificationCenter::getInstance()->addObserver(this, callfuncO_selector(MainCharacter::RunAnimation), "Run", NULL);
		NotificationCenter::getInstance()->addObserver(this, callfuncO_selector(MainCharacter::StopRunningAnimation), "StopRun", NULL);
	}

	if (!_isShowCase)
	{
		this->schedule([this](float dt)
		{
			if (!_exCharacter->isAnimationPlaying())
			{//當沒有主角動畫在Run時，狀態都清空
				//包含普通攻擊案超過3次 和 普通攻擊只按一次
				if (status._isDying)
				{
					if (_isReplaceScene)
						return;
					_isReplaceScene = true;
					AudioManagement::getInstance()->clearAudioManagement();
					ScrollScene::getScrollScene()->_ClearAudioManagement = true;
					Director::getInstance()->replaceScene(TransitionFade::create(6.f,FailScene::createScene()));
				}
				else
				{
					_normalAttackCount = 0;
					resetAllStatus();
				}
			}
			this->updatePhysicsFriction();
			this->updateCharacterOnTheSkyState();
			this->checkIdleAnimation();
			if (status._isRun && !status._isHurt)
			{
				if (this->status._faceDirection)
					this->setPositionX(getPositionX() - struct_infor._moveSpeed);
				else
					this->setPositionX(getPositionX() + struct_infor._moveSpeed);
			}
		}, 0.02f, "MainCharacterUpdate");
		this->schedule([&](float dt)
		{
			if (!NPC_AI::getInstance()->_npcs.empty() && !_isShowCase)
			{
				this->checkToNPCDistance();
			}
		}, 0.1f, "CheckToNpcDistance");
		this->schedule([&,this](float dt)
		{
			if (this->status._isGodMode)
			{
				static bool isGray = false;
				if (isGray)
				{
					this->_exCharacter->getArmatureDisplay()->setColor(Color3B::WHITE);
				}
				else
				{
					this->_exCharacter->getArmatureDisplay()->setColor(Color3B::GRAY);
				}
				isGray = !isGray;
			}
			
		}, 0.15f, "GodModeUpdate");
	}
	return true;
}
void MainCharacter::refreshBaseProperty()
{
	//init base property
	struct_infor._level = atoi(JsonParser::getJsonString("Infor", "Level", JsonParser::JsonType::Save).c_str());
	struct_property._Blood = struct_infor._level * 5 + RPGTool::getEquipmentBaseProperty(RPGTool::BaseProperty::Blood);
	struct_property._Force = struct_infor._level * 3 + RPGTool::getEquipmentBaseProperty(RPGTool::BaseProperty::Force);
	struct_property._Magic = struct_infor._level * 3 + RPGTool::getEquipmentBaseProperty(RPGTool::BaseProperty::Magic);
	struct_property._Defend = struct_infor._level * 1 + RPGTool::getEquipmentBaseProperty(RPGTool::BaseProperty::Defend);
	struct_property._Dodge = struct_infor._level * 1 + RPGTool::getEquipmentBaseProperty(RPGTool::BaseProperty::Dodge);
	struct_property._Popular = atoi(JsonParser::getJsonString("Infor", "Popular", JsonParser::JsonType::Save).c_str());
	NotificationCenter::getInstance()->postNotification("ItemSceneSetNewCharacterInfor",nullptr);
	//init character infor
	struct_infor._moveSpeed = 2.7f;
	struct_infor._maxhp = struct_property._Blood * 10;
	struct_infor._maxmp = struct_property._Magic * 5;
	struct_infor._maxexp = (struct_infor._level + 1)*(struct_infor._level + 1)*(struct_infor._level + 1)*0.5;
	struct_infor._hp = atoi(JsonParser::getJsonString("Infor", "HP", JsonParser::JsonType::Save).c_str());
	if (struct_infor._hp > struct_infor._maxhp)
	{
		struct_infor._hp = struct_infor._maxhp;
		char hpStr[10];
		sprintf(hpStr, "%d", (int)struct_infor._hp);
		JsonParser::setSave("Infor", "HP", hpStr);
	}
	struct_infor._mp = atoi(JsonParser::getJsonString("Infor", "MP", JsonParser::JsonType::Save).c_str());
	if (struct_infor._mp > struct_infor._maxmp)
	{
		struct_infor._mp = struct_infor._maxmp;
		char mpStr[10];
		sprintf(mpStr, "%d", (int)struct_infor._mp);
		JsonParser::setSave("Infor", "MP", mpStr);
	}
	struct_infor._exp= atoi(JsonParser::getJsonString("Infor", "EXP", JsonParser::JsonType::Save).c_str());
	struct_infor._damage = struct_property._Force * 3+ struct_property._Magic;
	struct_infor._defend = struct_property._Defend * 2 + struct_property._Magic;
	struct_infor._dodge = struct_property._Dodge * 2;
	_currentJumpCount = 0;
	_MaxJumpCount = 2;
}
void MainCharacter::updatePhysicsFriction()
{
	if(status._isHurt || status._isAttack)
		this->_CharacterPhysicsBody->getShape(0)->setFriction(1.f);//降低磨擦力
	else
		this->_CharacterPhysicsBody->getShape(0)->setFriction(100.f);//正常磨擦力
}
void MainCharacter::resetAllStatus()
{
	status._isIdle = false;
	status._isRun = false;
	status._isAttack = false;
	status._isHurt = false;
	status._isJump = false;
}
void MainCharacter::AttackAnimation(Ref* ref)
{
	if (status._isJump || status._isHurt ||
		(status._isAttack && _normalAttackCount == 0))
	{//如果在空中、受傷或已經放技能時則不能按下普通攻擊，就不能攻擊，
		return;
	}
	else
	{
		if (strcmp((char*)ref, "NormalAttack") == 0)
		{//普通攻擊
			if (_normalAttackCount == 0)
			{//第一次攻擊直接播放第一招動畫
				_exCharacter->play(SkillMG::getInstance()->getCurrentSkillAnimationName(SkillMG::SkillType::Normal,"1"), 1, true);
				_normalAttackCount++;
			}
			else
			{
				float AttackTotalTime = this->_exCharacter->getLastAnimationTotalTime();
				float AttackCurrentTime = this->_exCharacter->getLastAnimationCurrentTime();
				if (AttackCurrentTime > AttackTotalTime / 2 && !lockNormalClick)
				{
					lockNormalClick = true;
					if (_normalAttackCount == 1)
					{
						_exCharacter->play(SkillMG::getInstance()->getCurrentSkillAnimationName(SkillMG::SkillType::Normal, "2"), 1, true);
					}
					else if (_normalAttackCount == 2)
					{
						_exCharacter->play(SkillMG::getInstance()->getCurrentSkillAnimationName(SkillMG::SkillType::Normal, "3"), 1, true);
					}
					_normalAttackCount++;
				}
				else
				{//在沒有能觸發下一招普通攻擊的時機時點擊。
					return;
				}
			}
		}
		else
		{//技能攻擊
			if (_normalAttackCount != 0)
			{//這是在普功連段狀態,不該放技能的直接return
				return;
			}
			if (strcmp((char*)ref, "Skill1") == 0)
			{
				std::string skillAnimationName = SkillMG::getInstance()->getCurrentSkillAnimationName(SkillMG::SkillType::Skill, "1");
				if (checkAndSetMP(skillAnimationName) == false)
				{//魔力不夠施放招式
					return;
				}
				_exCharacter->play(skillAnimationName, 1, true);
			}
			else if (strcmp((char*)ref, "Skill2") == 0)
			{
				std::string skillAnimationName = SkillMG::getInstance()->getCurrentSkillAnimationName(SkillMG::SkillType::Skill, "2");
				if (checkAndSetMP(skillAnimationName) == false)
				{//魔力不夠施放招式
					return;
				}
				_exCharacter->play(skillAnimationName, 1, true);
			}
			else if (strcmp((char*)ref, "Skill3") == 0)
			{
				std::string skillAnimationName = SkillMG::getInstance()->getCurrentSkillAnimationName(SkillMG::SkillType::Skill, "3");
				if (checkAndSetMP(skillAnimationName) == false)
				{//魔力不夠施放招式
					return;
				}
				_exCharacter->play(skillAnimationName, 1, true);
			}
		}
		this->resetAllStatus();
		status._isAttack = true;
	}
}
void MainCharacter::HurtAnimation(bool AttackDirection)
{
	if (status._isHurt)
	{
		return;
	}
	else
	{
		this->resetAllStatus();
		status._isHurt = true;
		status._isGodMode = true;
		AudioManagement::getInstance()->play("sound/getHurt.mp3", AudioManagement::AudioType::Sound, false);
		_exCharacter->play("Hurt", 1, false);
		ScrollScene::getScrollScene()->ShakeCamera(0.2f, 50.f);
		if(AttackDirection)
			this->_CharacterPhysicsBody->setVelocity(Vec2(-150, 50));
		else
			this->_CharacterPhysicsBody->setVelocity(Vec2(150, 50));
		this->scheduleOnce([this](float dt) 
		{//無敵時間
			status._isGodMode = false;
			this->_exCharacter->getArmatureDisplay()->setColor(Color3B::WHITE);
		},2.5f,"MainCharacterGodModeTime");
	}
}
void MainCharacter::AttackMethod()
{
	MonsterAI* monsterAI = ScrollMode::MonsterAI::getInstance();
	if (!monsterAI->_monsters.empty())
	{
		for (auto it = monsterAI->_monsters.begin();it != monsterAI->_monsters.end();it++)
		{
			bool monsterXpointMoreThenMainCharacter = (*it)->isXpointMoreThenMainCharacter();
			Vec2 monsterToCharacterDistance = (*it)->getDistanceToMainCharacter();
			if (monsterToCharacterDistance.x < _characterAttackRange.width &&
				monsterToCharacterDistance.y < _characterAttackRange.height &&
				(_AttackRangeType == 0 && status._faceDirection == !monsterXpointMoreThenMainCharacter) ||
				(_AttackRangeType == 1 && status._faceDirection == monsterXpointMoreThenMainCharacter) ||
				(_AttackRangeType == 2))
			{//鼠標移到_AttackRangeType變數上就會有註解
				int realDamage = this->struct_infor._damage + _SkillDamage;
				realDamage += rand() % 30;
				if (monsterXpointMoreThenMainCharacter)
				{//怪物在主角的右邊，但是被攻擊後 面向左邊，面向主角
					(*it)->setFaceDirection(true);
					ScrollScene::getScrollScene()->_damageMG->showDamage(realDamage,
						DamageMG::Direction::ToRight, (*it)->getPosition());
				}
				else
				{//怪物在主角的右邊，但是被攻擊後 面向右邊，面向主角
					(*it)->setFaceDirection(false);
					ScrollScene::getScrollScene()->_damageMG->showDamage(realDamage,
						DamageMG::Direction::ToLeft, (*it)->getPosition());
				}
				(*it)->doHurtAnimation((ScrollMonster::HurtStatus)_SendToMonsterHurtAnimation);
				(*it)->setHPByAttack(realDamage);//怪物血量 扣掉主角攻擊力
				(*it)->playHurtSound();
				_SkillDamage = 0;
				if (_AttackType == 0)
				{//普通攻擊只能攻擊一隻怪物直接返回
					return;
				}
			}
		}
	}
}
void MainCharacter::checkIdleAnimation()
{
	//do nothing run idle action
	if (!status._isIdle && !status._isAttack && !status._isHurt && !status._isRun && !status._isDying && !status._isJump)
	{
		status._isIdle = true;
		_exCharacter->play("Idle",-1,false);
	}
}

void MainCharacter::RunAnimation(Ref* ref)
{
	if (status._isRun || status._isAttack || status._isHurt)
		return;
	this->resetAllStatus();
	status._isRun = true;
	_exCharacter->play("Run", -1,false);
}

void MainCharacter::StopRunningAnimation(Ref* ref)
{
	status._isRun = false;

}

void MainCharacter::JumpAnimation(Ref* ref)
{
	if (status._isHurt)
		return;
	_currentJumpCount++;
	this->status._isIdle = false;
	if (status._isRun != true)//如果正在跑步狀態，就不要執行跳躍動畫
		_exCharacter->play("Jump", -1, false);
	status._isJump = true;
	this->_CharacterPhysicsBody->setVelocity(Vec2(0, 200));//jump velocity
}

void MainCharacter::doDyingAnimation()
{
	if (status._isDying)
		return;
	AudioManagement::getInstance()->play("sound/getHurt.mp3", AudioManagement::AudioType::Sound, false);
	this->resetAllStatus();
	status._isDying = true;
	_exCharacter->play("Dying", 1, false);
}
void MainCharacter::FallDownAnimation(Ref* ref)
{
	if (status._isHurt)
		return;
	_currentJumpCount++;
	this->status._isIdle = false;
	if (status._isRun != true)//如果正在跑步狀態，就不要執行跳躍動畫
		_exCharacter->play("Jump", -1, false);
	status._isJump = true;
	this->_CharacterPhysicsBody->setVelocity(Vec2(0,-10));//jump velocity
}

void MainCharacter::reviseDirection(bool faceDirection)
{
	if (status._faceDirection != faceDirection)
	{
		status._faceDirection = faceDirection;
		if (faceDirection)
		{
			_exCharacter->setScaleX(-0.27f);
		}
		else
			_exCharacter->setScaleX(0.27f);
	}
}
void MainCharacter::updateCharacterOnTheSkyState()
{
	if (this->_CharacterPhysicsBody->getVelocity().y <=  0.1f &&
		this->_CharacterPhysicsBody->getVelocity().y >= -0.1f)
	{
		if (this->status._isJump)
		{//當主角物理剛體速度為0時，並且還是在空中的狀態，代表主角剛落地。
			this->status._isJump = false;
			_currentJumpCount = 0;//當前以跳躍次數重計為0
		}
		else//平常走路狀態
			return;
	}
	else
	{//主角物理剛體有速度，代表是在空中的狀態
		status._isJump = true;
	}
}
float MainCharacter::getAttackTiming(const std::string & animationName)
{
	if (animationName == "NormalAttack1")
	{
		_characterAttackRange.width = 80.f;
		_characterAttackRange.height = 100.f;
		_SendToMonsterHurtAnimation = (int)ScrollMonster::HurtStatus::ShortBackWard;
		_AttackRangeType = 0;
		_SkillDamage = 0;
		_AttackType = 0;
		//if(status._faceDirection==false)//面向右
		//	this->_CharacterPhysicsBody->setVelocity(Vec2(30, 0));
		//else                            //面向左
		//	this->_CharacterPhysicsBody->setVelocity(Vec2(-30, 0));
		return const_NormalAttack1Timing;
	}
	else if (animationName == "NormalAttack2")
	{
		_characterAttackRange.width = 85.f;
		_characterAttackRange.height = 100.f;
		_SendToMonsterHurtAnimation = (int)ScrollMonster::HurtStatus::ShortBackWard;
		_AttackRangeType = 0;
		_SkillDamage = 5;
		_AttackType = 0;
		if (status._faceDirection == false)//面向右
			this->_CharacterPhysicsBody->setVelocity(Vec2(80, 0));
		else                            //面向左
			this->_CharacterPhysicsBody->setVelocity(Vec2(-80, 0));
		return const_NormalAttack2Timing;
	}
	else if (animationName == "NormalAttack3")
	{
		_characterAttackRange.width = 85.f;
		_characterAttackRange.height = 100.f;
		_SendToMonsterHurtAnimation = (int)ScrollMonster::HurtStatus::HighBackWard;
		_AttackRangeType = 0;
		_SkillDamage = 10;
		_AttackType = 0;
		if (status._faceDirection == false)//面向右
			this->_CharacterPhysicsBody->setVelocity(Vec2(50, 0));
		else                            //面向左
			this->_CharacterPhysicsBody->setVelocity(Vec2(-50, 0));
		return const_NormalAttack3Timing;
	}
	else if (animationName == "NormalSkill1")
	{
		_characterAttackRange.width = 200.f;
		_characterAttackRange.height = 100.f;
		_SendToMonsterHurtAnimation = (int)ScrollMonster::HurtStatus::ShortBackWard;
		_AttackRangeType = 0;
		_SkillDamage = 25;
		_AttackType = 1;
		return const_NormalSkill1Timing;
	}
	else if (animationName == "NormalSkill2")
	{
		AudioManagement::getInstance()->play("sound/normalSkill2.mp3", AudioManagement::AudioType::Sound, false);
		_characterAttackRange.width = 150.f;
		_characterAttackRange.height = 100.f;
		_SendToMonsterHurtAnimation = (int)ScrollMonster::HurtStatus::StrongBackWard;
		_AttackRangeType = 0;
		_SkillDamage = 50;
		_AttackType = 2;
		return const_NormalSkill2Timing;
	}
	else if (animationName == "NormalSkill3")
	{
		_characterAttackRange.width = 250.f;
		_characterAttackRange.height = 100.f;
		_SendToMonsterHurtAnimation = (int)ScrollMonster::HurtStatus::HighBackWard;
		_AttackRangeType = 2;
		_SkillDamage = 100;
		_AttackType = 3;
		return const_NormalSkill3Timing;
	}
	//dragon
	else if (animationName == "DragonAttack1")
	{
		AudioManagement::getInstance()->play("sound/dragonAttack1.mp3", AudioManagement::AudioType::Sound, false);
		_characterAttackRange.width = 85.f;
		_characterAttackRange.height = 100.f;
		_SendToMonsterHurtAnimation = (int)ScrollMonster::HurtStatus::ShortBackWard;
		_AttackRangeType = 0;
		_SkillDamage = 0;
		_AttackType = 0;
		return const_DragonAttack1Timing;
	}
	else if (animationName == "DragonAttack2")
	{
		AudioManagement::getInstance()->play("sound/dragonAttack2.mp3", AudioManagement::AudioType::Sound, false);
		_characterAttackRange.width = 95.f;
		_characterAttackRange.height = 100.f;
		_SendToMonsterHurtAnimation = (int)ScrollMonster::HurtStatus::ShortBackWard;
		_AttackRangeType = 0;
		_SkillDamage = 5;
		_AttackType = 0;
		return const_DragonAttack2Timing;
	}
	else if (animationName == "DragonAttack3")
	{
		AudioManagement::getInstance()->play("sound/dragonAttack3.mp3", AudioManagement::AudioType::Sound, false);
		_characterAttackRange.width = 140.f;
		_characterAttackRange.height = 100.f;
		_SendToMonsterHurtAnimation = (int)ScrollMonster::HurtStatus::HighBackWard;
		_AttackRangeType = 0;
		_SkillDamage = 10;
		_AttackType = 0;
		return const_DragonAttack3Timing;
	}
	
	else if (animationName == "DragonSkill1")
	{
		AudioManagement::getInstance()->play("sound/dragonSkill.mp3", AudioManagement::AudioType::Sound, false);
		_characterAttackRange.width = 260.f;
		_characterAttackRange.height = 100.f;
		_SendToMonsterHurtAnimation = (int)ScrollMonster::HurtStatus::HighBackWard;
		_AttackRangeType = 0;
		_SkillDamage = 20;
		_AttackType = 1;
		return const_DragonSkill1Timing;
	}
	
	else if (animationName == "DragonSkill2")
	{
		AudioManagement::getInstance()->play("sound/dragonSkill2.mp3", AudioManagement::AudioType::Sound, false);
		_characterAttackRange.width = 240.f;
		_characterAttackRange.height = 100.f;
		_SendToMonsterHurtAnimation = (int)ScrollMonster::HurtStatus::StrongBackWard;
		_AttackRangeType = 0;
		_SkillDamage = 60;
		_AttackType = 2;
		return const_DragonSkill2Timing;
	}
	else if (animationName == "DragonSkill3")
	{
		AudioManagement::getInstance()->play("sound/dragonSkill3.mp3", AudioManagement::AudioType::Sound, false);
		_characterAttackRange.width = 250.f;
		_characterAttackRange.height = 100.f;
		_SendToMonsterHurtAnimation = (int)ScrollMonster::HurtStatus::HighBackWard;
		_AttackRangeType = 2;
		_SkillDamage = 120;
		_AttackType = 3;
		return const_DragonSkill3Timing;
	}
	else
	{
		CCAssert(false, "no this condition name.");
	}
}
bool MainCharacter::checkAndSetMP(const std::string & animationName)
{
	int mpCost = 0;
	if (animationName == "NormalAttack1")
	{
		mpCost = 1;
	}
	else if (animationName == "NormalAttack2")
	{
		mpCost = 1;
	}
	else if (animationName == "NormalAttack3")
	{
		mpCost = 2;
	}
	else if (animationName == "NormalSkill1")
	{
		mpCost = -5;
	}
	else if (animationName == "NormalSkill2")
	{
		mpCost = -10;
	}
	else if (animationName == "NormalSkill3")
	{
		mpCost = -15;
	}
	//dragon
	else if (animationName == "DragonAttack1")
	{
		mpCost = 1;
	}
	else if (animationName == "DragonAttack2")
	{
		mpCost = 1;
	}
	else if (animationName == "DragonAttack3")
	{
		mpCost = 2;
	}
	else if (animationName == "DragonSkill1")
	{
		mpCost = -5;
	}
	else if (animationName == "DragonSkill2")
	{
		mpCost = -10;
	}
	else if (animationName == "DragonSkill3")
	{
		mpCost = -15;
	}
	else
	{
		CCAssert(false, "no this animation name.");
	}
	if (this->struct_infor._mp + mpCost >= 0)
	{//當前魔力足夠消耗掉並放技能
		this->struct_infor._mp = this->struct_infor._mp + mpCost;
		if (this->struct_infor._mp > this->struct_infor._maxmp)
			this->struct_infor._mp = this->struct_infor._maxmp;
		char newMPStr[10];
		sprintf(newMPStr, "%d", (int)this->struct_infor._mp);
		JsonParser::setSave("Infor", "MP", newMPStr);
		HUDlayer::getHUDlayer()->refreshMPBar();
		return true;
	}
	else
	{
		AudioManagement::getInstance()->play("sound/noMana.mp3", AudioManagement::AudioType::Sound, false);
		return false;
	}
}
void MainCharacter::AccordingAttackPlaySound(const std::string& animationName)
{
	if (animationName == "NormalAttack1")
	{
		AudioManagement::getInstance()->play("sound/normalAttack1.mp3", AudioManagement::AudioType::Sound, false);
	}
	else if (animationName == "NormalAttack2")
	{
		AudioManagement::getInstance()->play("sound/normalAttack2.mp3", AudioManagement::AudioType::Sound, false);
	}
	else if (animationName == "NormalAttack3")
	{
		AudioManagement::getInstance()->play("sound/normalAttack3.mp3", AudioManagement::AudioType::Sound, false);
		ScrollScene::getScrollScene()->ShakeCamera(0.1f, 30.f);
	}
	else if (animationName == "NormalSkill1")
	{
		AudioManagement::getInstance()->play("sound/normalSkill.mp3", AudioManagement::AudioType::Sound, false);
		ScrollScene::getScrollScene()->ShakeCamera(0.1f, 20.f);
	}
	else if (animationName == "NormalSkill2")
	{
		ScrollScene::getScrollScene()->ShakeCamera(0.1f, 20.f);
	}
	else if (animationName == "NormalSkill3")
	{
		AudioManagement::getInstance()->play("sound/normalSkill3.mp3", AudioManagement::AudioType::Sound, false);
		ScrollScene::getScrollScene()->ShakeCamera(0.2f, 30.f);
	}
	//dragon
	else if (animationName == "DragonAttack1")
	{
		
	}
	else if (animationName == "DragonAttack2")
	{
		
	}
	else if (animationName == "DragonAttack3")
	{

	}
	else if (animationName == "DragonSkill1")
	{
		ScrollScene::getScrollScene()->ShakeCamera(0.1f, 20.f);
	}
	else if (animationName == "DragonSkill2")
	{

		ScrollScene::getScrollScene()->ShakeCamera(0.1f, 20.f);
	}
	else if (animationName == "DragonSkill3")
	{
	
		ScrollScene::getScrollScene()->ShakeCamera(0.2f, 30.f);
	}
	else
	{
		CCAssert(false, "no this condition.");
	}
}
void MainCharacter::checkToNPCDistance()
{
	bool isCharacterInNPCvisibleRange=false;
	bool isNPCinVisible = false;
	NPC* pretendCloseToMainCharacter = nullptr;
	for (auto npc = NPC_AI::getInstance()->_npcs.begin();npc != NPC_AI::getInstance()->_npcs.end();npc++)
	{
		auto NpcToCharacterDis = (*npc)->getPosition().distance(this->getPosition());
		if (pretendCloseToMainCharacter == nullptr)
		{
			pretendCloseToMainCharacter = *npc;
		}
		else
		{
			if (NpcToCharacterDis <= pretendCloseToMainCharacter->getPosition().distance(this->getPosition()))
			{
				pretendCloseToMainCharacter = *npc;
			}
			else
			{
				(*npc)->setZOrder(ScrollScene::NPC_ZORDER);
			}
		}
		pretendCloseToMainCharacter->setZOrder(ScrollScene::NPC2_ZORDER);
		if (NpcToCharacterDis < (*npc)->_visibleRange)
		{
			isNPCinVisible = true;
			if ((*npc)->status._isTalk)
			{//NPC保持不變繼續說話
				
			}
			else
			{
				(*npc)->_chatSprite->setVisible(true);
				isCharacterInNPCvisibleRange = true;
				(*npc)->changeChatLabelAndFacialState();
				(*npc)->updateArmatureFacial();
				(*npc)->status._isTalk = true;
			}
		}
		else
		{
			(*npc)->_chatSprite->setVisible(false);
			(*npc)->setFacialType(NPC::FacialType::NM);
			(*npc)->status._isTalk = false;
		}
	}
	if (isNPCinVisible)
	{
		HUDlayer::getHUDlayer()->_normalSkillState.Talk = true;
	}
	else
	{
		HUDlayer::getHUDlayer()->_normalSkillState.Talk = false;
	}
	HUDlayer::getHUDlayer()->_closerNPC = pretendCloseToMainCharacter;
}
void MainCharacter::addExp(float exp)
{
	bool isLevelUp = false;
	this->struct_infor._exp += exp;
	if (this->struct_infor._exp >= this->struct_infor._maxexp)
	{//升等
		isLevelUp = true;
		//超過的經驗留到下一等
		this->struct_infor._exp = this->struct_infor._exp - this->struct_infor._maxexp;
		this->struct_infor._level++;
		char newLevelStr[10];
		char newExpStr[10];
		sprintf(newLevelStr, "%d", this->struct_infor._level);
		sprintf(newExpStr, "%d",(int)this->struct_infor._exp);
		JsonParser::setSave("Infor", "Level", newLevelStr);
		JsonParser::setSave("Infor", "EXP", newExpStr);
	}
	else
	{//還沒升等
		char newExpStr[10];
		sprintf(newExpStr, "%d", (int)this->struct_infor._exp);
		JsonParser::setSave("Infor", "EXP", newExpStr);
	}
	this->refreshBaseProperty();
	if (isLevelUp)
	{
		this->addHPMPToFull();
		AudioManagement::getInstance()->play("sound/lvup.mp3", AudioManagement::AudioType::Sound, false);
	}
	HUDlayer::getHUDlayer()->refreshHUDlayer();
}
void MainCharacter::addHPMPToFull()
{
	this->struct_infor._hp = this->struct_infor._maxhp;
	this->struct_infor._mp = this->struct_infor._maxmp;
	char newHPStr[10];
	char newMPStr[10];
	sprintf(newHPStr, "%d", (int)this->struct_infor._hp);
	sprintf(newMPStr, "%d", (int)this->struct_infor._mp);
	JsonParser::setSave("Infor", "HP", newHPStr);
	JsonParser::setSave("Infor", "MP", newMPStr);
	HUDlayer::getHUDlayer()->refreshHUDlayer();
}
NS_SCROLLMODE_END