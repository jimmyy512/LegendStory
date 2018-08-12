#include "NPCFactory.h"
#include "tools\AnimateTool.h"
#include "tools\JsonParser.h"
#include "tools\MajitooTool.h"
#include "RPGTools\RPGTool.h"
#include "MainCharacter\MainCharacter.h"
#include "Npc\NPC_AI.h"
#include "UI\ScrollMode\Dialog.h"
#include "UI\ScrollMode\HUDlayer.h"
#include "MissionMG.h"
USING_NS_CC;
using namespace ScrollMode;
using namespace MyTools;
NPC * NPCFactory::create(NPCName npcName,bool isShowCase ,Vec2 bornPos)
{
	NPC* npc = nullptr;
	switch (npcName)
	{
	case NPCFactory::NPCName::ZhangQing:
		npc = ZhangQing::create(isShowCase);
		break;
	case NPCFactory::NPCName::ZhangYin:
		npc = ZhangYin::create(isShowCase);
		break;
	case NPCFactory::NPCName::ZhangZhen:
		npc = ZhangZhen::create(isShowCase);
		break;
	case NPCFactory::NPCName::ZhangHen:
		npc = ZhangHen::create(isShowCase);
		break;
	case NPCFactory::NPCName::ZhangFong:
		npc = ZhangFong::create(isShowCase);
		break;
	case NPCFactory::NPCName::ZhangWo:
		npc = ZhangWo::create(isShowCase);
		break;
	default:
		CCAssert(false,"IN NPCFactory create no this NPC Name.");
	}
	npc->setPosition(bornPos);
	if(!npc->_isShowCase)
		NPC_AI::getInstance()->addToAI(npc);
	return npc;
}
NPC::NPC()
{
	this->setAnchorPoint(Vec2(0, 0));
	this->_isNpcCanMove = false;
	this->_facial = "NM";
	this->_moveSpeed = 1.5f;
	this->_thinkInterval = (rand() %1201 + 700)/100.f;
	this->_thinkInterval2 = (rand() % 1301 + 500) / 100.f;
}
NPC::~NPC()
{
	_armatureDisplay->dispose();
}
bool NPC::init(bool isShowCase)
{
	if (!Node::init())
		return false;
	this->_isShowCase = isShowCase;
	if (!this->_isShowCase)
	{
		//驚嘆號
		_exclam = Sprite::create("ui/exclamation.png");
		_exclam->setPositionY(110);
		_exclam->setVisible(false);
		this->addChild(_exclam,100);

		this->_moveSpeed += rand()%2==0?(rand() % 75) / 1000.f: -((rand() % 75) / 1000.f);
		if (rand() % 2 == 0)
			this->setFaceDirection(true);
		else
			this->setFaceDirection(false);
		_chatSprite->setVisible(false);
		_chatSprite->setAnchorPoint(Vec2(0, 0));
		//對話框高度自己指定
		_chatSprite->setPosition(_dialogXY);
		this->addChild(_chatSprite, 1);
		_chatLabel->setMaxLineWidth(_chatSprite->getBoundingBox().size.width*0.9);
		_chatLabel->setColor(Color3B::BLACK);
		_chatLabel->setPosition(_chatSprite->getBoundingBox().size.width / 2, _chatSprite->getBoundingBox().size.height / 2);
		_chatSprite->addChild(_chatLabel, 2);

		//初始化NPC會出現在交易選單上的物品
		this->judgmentSellItem();
		//每個NPC至少都會有小紅、小藍水能做交易
		_sellItems.push_back(std::make_tuple((int)RPGTool::ItemID::SmallRedPotion, rand()%50+1, 100));
		_sellItems.push_back(std::make_tuple((int)RPGTool::ItemID::SmallBluePotion, rand() % 50 + 1, 100));
		_sellItems.push_back(std::make_tuple((int)RPGTool::ItemID::MiddleBluePotion, rand() % 35 + 1, 70));
		_sellItems.push_back(std::make_tuple((int)RPGTool::ItemID::MiddleRedPotion, rand() % 35 + 1, 70));
		_sellItems.push_back(std::make_tuple((int)RPGTool::ItemID::LargeBluePotion, rand() % 20 + 1, 20));
		_sellItems.push_back(std::make_tuple((int)RPGTool::ItemID::LargeRedPotion, rand() % 20 + 1, 20));

		//固定思考AI
		this->schedule(schedule_selector(NPC::ThinkAI),this->_thinkInterval);
		this->schedule(schedule_selector(NPC::ThinkAI2), this->_thinkInterval2);
		

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

	}
	return true;
}
void NPC::update()
{
	if (this->_missionState == MissionMG::getInstance()->getCurrentMissionCompleteState())
	{
		this->_exclam->setVisible(true);
	}
	else
	{
		this->_exclam->setVisible(false);
	}
	if (this->_armatureDisplay->getAnimation()->isCompleted())
	{
		if (status._isDying)
			this->status._isDeath = true;
		else
			this->resetAllStatus();
	}
	if (!status._isIdle && !status._isRun && !status._isWalk
		&& !status._isIdle2 && !status._isDeath && !status._isDying)
	{
		this->doIdleAnimation();
		if (this->_isNpcCanMove)
		{
			this->scheduleOnce([&](float dt)
			{
				this->doWalkAnimation();
			}, 0.1f,"NPC become to Move");
		}
	}
}
void NPC::ThinkAI(float dt)
{
	//隨機動作
	auto randNum = rand() % 3;
	if (randNum==0)
	{
		this->doIdleAnimation();
	}
	else if (randNum == 1)
	{
		this->doIdle2Animation();
	}
	else
	{
		this->doWalkAnimation();
	}
}
void NPC::ThinkAI2(float dt)
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
void NPC::resetAllStatus()
{
	this->status._isIdle = false;
	this->status._isIdle2 = false;
	this->status._isRun = false;
	this->status._isWalk = false;
	this->status._isDying = false;
	this->status._isDeath= false;
}
void NPC::updateFaceDirectionWithCharacter()
{
	if (this->getPositionX() - MainCharacter::getMainCharacter()->getPositionX() > 0)
	{
		if (this->status._faceDirection == true)
			this->_armatureDisplay->setScaleX(this->_armatureDisplay->getScaleX());
		else
		{
			this->_armatureDisplay->setScaleX(-this->_armatureDisplay->getScaleX());
			this->status._faceDirection = !this->status._faceDirection;
		}
	}
	else
	{
		if (this->status._faceDirection == true)
		{
			this->_armatureDisplay->setScaleX(-this->_armatureDisplay->getScaleX());
			this->status._faceDirection = !this->status._faceDirection;
		}
		else
			this->_armatureDisplay->setScaleX(this->_armatureDisplay->getScaleX());
	}
}
void NPC::changeChatLabelAndFacialState()
{
	int randNum=0;
	std::string nullStr = "NoStr";
	do 
	{
		randNum = rand() % 5;
		
	} while (_prevRandNum == randNum);
	_prevRandNum = randNum;
	
	std::string newChatStr = MajitooTool::SplitStr(
		JsonParser::getNPCJsonFile(_name.c_str(), "Talk", randNum),
		MajitooTool::SplitType::MarkFront);
	_chatLabel->setString(newChatStr);
	this->_facial = MajitooTool::SplitStr(JsonParser::getNPCJsonFile(_name.c_str(), "Talk", randNum),
		MajitooTool::SplitType::wantFacial);
}

void NPC::setFaceDirection(bool newFaceDirection)
{
	if (this->status._faceDirection == newFaceDirection)
		return;
	else
	{
		this->status._faceDirection = newFaceDirection;
		this->_armatureDisplay->setScaleX(-this->_armatureDisplay->getScaleX());
	}
}

void NPC::setFacialType(FacialType facialType)
{
	std::string tmpFacial;
	switch (facialType)
	{
	case FacialType::NM:
		tmpFacial ="NM";
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

void NPC::setFacialType(std::string facialNameStr)
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

void NPC::updateArmatureFacial()
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

void NPC::doIdleAnimation()
{
	if (status._isIdle)
		return;
	this->resetAllStatus();
	this->status._isIdle = true;
	this->_armatureDisplay->getAnimation()->play("Idle", -1);
}

void NPC::doIdle2Animation()
{
	if (status._isIdle2)
		return;
	this->resetAllStatus();
	this->status._isIdle2 = true;
	this->_armatureDisplay->getAnimation()->play("Idle2", -1);
}

void NPC::doWalkAnimation()
{
	if (status._isWalk)
		return;
	this->resetAllStatus();
	this->status._isWalk = true;
	this->_armatureDisplay->getAnimation()->play("Walk", -1);
}

void NPC::doRunAnimation()
{
	if (status._isRun)
		return;
	this->resetAllStatus();
	this->status._isRun = true;
	this->_armatureDisplay->getAnimation()->play("Run", -1);
}

void NPC::doDyingAnimation()
{
	this->resetAllStatus();
	this->status._isDying = true;
	this->_armatureDisplay->getAnimation()->play("Dying", 1);
}

void NPC::judgmentSellItem()
{
	for (auto item=_sellItems.begin();item!=_sellItems.end();)
	{
		int sellChance = rand() % 100 + 1;
		if (sellChance > std::get<2>(*item))
		{
			item = _sellItems.erase(item);
		}
		else
		{
			item++;
		}
	}
}

std::string NPC::getDialogCode()
{
	if ((int)MissionMG::getInstance()->getCurrentMissionTag() > _unlockDialogCodeStage)
	{
		return _DialogCode;
	}
	else
	{
		return "WhoAreYou";
	}
}

//sprite sheet架構
////@WeaponTrader
//WeaponTrader * WeaponTrader::create(bool isShowCase)
//{
//	WeaponTrader* ptr = new(std::nothrow)WeaponTrader;
//	if (ptr&& ptr->init(isShowCase))
//	{
//		ptr->autorelease();
//	}
//	else
//	{
//		CC_SAFE_DELETE(ptr);
//	}
//	return ptr;
//}
//WeaponTrader::WeaponTrader()
//{
//	this->setScale(1.1f);
//	_name = "WeaponTrader";
//	_chineseName = JsonParser::getJsonString("Code_Table", "LWT", JsonParser::JsonType::TableCode);
//	_DialogCode = "LuoYang_WeaponTrader";
//	status._faceDirection = true;
//	this->setContentSize(AnimateTool::getInstance()->getFirstAnimationFrameSize("weaponTraderIdle", "animate/npcs/weaponTrader/weaponTraderIdle.plist"));
//	_actionNode = Sprite::createWithSpriteFrame(AnimateTool::getInstance()->getFirstSpriteFrame("weaponTraderIdle", "animate/npcs/weaponTrader/weaponTraderIdle.plist"));
//	_actionNode->setScale(1.1f);
//	_chatSprite = Sprite::create("sprite/chatSprite.png");
//	_chatLabel = Label::createWithTTF(JsonParser::getJsonString("WeaponTrader", "Talk1", JsonParser::JsonType::NPC),
//		JsonParser::fontType(), JsonParser::fontSize(16));
//}
//Animate * WeaponTrader::IdleAnimation()
//{
//	return AnimateTool::getInstance()->setAnimation("weaponTraderIdle",0.2f,
//		-1, "animate/npcs/weaponTrader/weaponTraderIdle.plist");
//}
//Animate * WeaponTrader::TalkAnimation()
//{
//	return AnimateTool::getInstance()->setAnimation("weaponTraderTalk", 0.15f,
//		2, "animate/npcs/weaponTrader/weaponTraderTalk.plist");
//}



//@ZhangQing
ZhangQing * ZhangQing::create(bool isShowCase)
{
	ZhangQing* ptr = new(std::nothrow)ZhangQing;
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
ZhangQing::ZhangQing()
{
	_missionState = MissionMG::State::TalkZhangQing;
	_unlockDialogCodeStage = (int)MissionMG::MainName::FirstMeetZhangQing;
	this->setAnchorPoint(Vec2(0.5, 0));
	this->setContentSize(Size(0, 0));
	this->_isNpcCanMove = true;
	status._faceDirection = true;

	_dialogXY.x = 35;
	_dialogXY.y = 75;
	_visibleRange = 150;
	_moveSpeed = 0.25f;
	_nameCode = "ZQ";
	_name = "ZhangQing";
	_chineseName = JsonParser::getJsonString("Code_Table", "ZQ", JsonParser::JsonType::TableCode);
	_DialogCode = "ZhangQing";
	_chatSprite = Sprite::create("sprite/chatSprite.png");
	_chatLabel =Label::createWithTTF(
		MajitooTool::SplitStr(JsonParser::getNPCJsonFile("ZhangQing", "Talk"),MajitooTool::SplitType::MarkFront),
		JsonParser::fontType(), JsonParser::fontSize(15));
	//dragon bone
	_dragonBonesData = _factory.loadDragonBonesData("npc/ZhangQing_ske.json");
	_factory.loadTextureAtlasData("npc/ZhangQing_tex.json");
	_armatureDisplay = _factory.buildArmatureDisplay("Armature");
	_armature = _armatureDisplay->getArmature();
	_armatureDisplay->setScale(0.4f);
	_armatureDisplay->setPositionY(-20);
	this->addChild(_armatureDisplay);

	_sellItems.push_back(std::make_tuple((int)RPGTool::ItemID::Cloth_Farmer, RPGTool::getRandMaxItemValue(), 80));
	_sellItems.push_back(std::make_tuple((int)RPGTool::ItemID::Pant_Farmer, RPGTool::getRandMaxItemValue(), 80));
	_sellItems.push_back(std::make_tuple((int)RPGTool::ItemID::Cloth_ChiuanJen, RPGTool::getRandMaxItemValue(), 100));
	_sellItems.push_back(std::make_tuple((int)RPGTool::ItemID::Pant_ChiuanJen, RPGTool::getRandMaxItemValue(), 100));
	_sellItems.push_back(std::make_tuple((int)RPGTool::ItemID::Weapon_NoobSword, RPGTool::getRandMaxItemValue(), 100));
	_sellItems.push_back(std::make_tuple((int)RPGTool::ItemID::Weapon_Fist, RPGTool::getRandMaxItemValue(), 100));
	_sellItems.push_back(std::make_tuple((int)RPGTool::ItemID::Weapon_Fist_Blue, RPGTool::getRandMaxItemValue(), 10));
	_sellItems.push_back(std::make_tuple((int)RPGTool::ItemID::Cloth_BlueKnight, RPGTool::getRandMaxItemValue(), 10));
	_sellItems.push_back(std::make_tuple((int)RPGTool::ItemID::Pant_BlueKnight, RPGTool::getRandMaxItemValue(), 10));
	
}

//@ZhangYin
ZhangYin * ZhangYin::create(bool isShowCase)
{
	ZhangYin* ptr = new(std::nothrow)ZhangYin;
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
ZhangYin::ZhangYin()
{
	_missionState = MissionMG::State::TalkZhangYin;
	_unlockDialogCodeStage = (int)MissionMG::MainName::FirstMeetZhangYin;
	this->setAnchorPoint(Vec2(0.5, 0));
	this->setContentSize(Size(0, 0));
	this->_isNpcCanMove = true;
	status._faceDirection = true;

	_dialogXY.x = 35;
	_dialogXY.y = 75;
	_visibleRange = 150;
	_moveSpeed = 0.25f;
	_nameCode = "ZY";
	_name = "ZhangYin";
	_chineseName = JsonParser::getJsonString("Code_Table", "ZY", JsonParser::JsonType::TableCode);
	_DialogCode = "ZhangYin";
	_chatSprite = Sprite::create("sprite/chatSprite.png");
	_chatLabel = Label::createWithTTF(
		MajitooTool::SplitStr(JsonParser::getNPCJsonFile("ZhangYin", "Talk"), MajitooTool::SplitType::MarkFront),
		JsonParser::fontType(), JsonParser::fontSize(15));
	//dragon bone
	_dragonBonesData = _factory.loadDragonBonesData("npc/ZhangYin_ske.json");
	_factory.loadTextureAtlasData("npc/ZhangYin_tex.json");
	_armatureDisplay = _factory.buildArmatureDisplay("Armature");
	_armature = _armatureDisplay->getArmature();
	_armatureDisplay->setScale(0.4f);
	_armatureDisplay->setPositionY(-20);
	this->addChild(_armatureDisplay);

	_sellItems.push_back(std::make_tuple((int)RPGTool::ItemID::Cloth_Farmer, RPGTool::getRandMaxItemValue(), 70));
	_sellItems.push_back(std::make_tuple((int)RPGTool::ItemID::Pant_Farmer, RPGTool::getRandMaxItemValue(), 70));
	_sellItems.push_back(std::make_tuple((int)RPGTool::ItemID::Weapon_NoobSword, RPGTool::getRandMaxItemValue(), 80));
	_sellItems.push_back(std::make_tuple((int)RPGTool::ItemID::Cloth_BlueKnight, RPGTool::getRandMaxItemValue(), 10));
	_sellItems.push_back(std::make_tuple((int)RPGTool::ItemID::Pant_BlueKnight, RPGTool::getRandMaxItemValue(), 10));
	_sellItems.push_back(std::make_tuple((int)RPGTool::ItemID::Cloth_ChiuanJen, RPGTool::getRandMaxItemValue(), 100));
	_sellItems.push_back(std::make_tuple((int)RPGTool::ItemID::Pant_ChiuanJen, RPGTool::getRandMaxItemValue(), 100));
}

//@ZhangZhen
ZhangZhen * ZhangZhen::create(bool isShowCase)
{
	ZhangZhen* ptr = new(std::nothrow)ZhangZhen;
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
ZhangZhen::ZhangZhen()
{
	_missionState = MissionMG::State::TalkZhangZhen;
	_unlockDialogCodeStage = (int)MissionMG::MainName::FirstInChiuanJen;
	this->setAnchorPoint(Vec2(0.5, 0));
	this->setContentSize(Size(0, 0));
	this->_isNpcCanMove = true;
	status._faceDirection = true;

	_dialogXY.x = 35;
	_dialogXY.y = 75;
	_visibleRange = 150;
	_moveSpeed = 0.25f;
	_nameCode = "ZZ";
	_name = "ZhangZhen";
	_chineseName = JsonParser::getJsonString("Code_Table", "ZZ", JsonParser::JsonType::TableCode);
	_DialogCode = "ZhangZhen";
	_chatSprite = Sprite::create("sprite/chatSprite.png");
	_chatLabel = Label::createWithTTF(
		MajitooTool::SplitStr(JsonParser::getNPCJsonFile("ZhangZhen", "Talk"), MajitooTool::SplitType::MarkFront),
		JsonParser::fontType(), JsonParser::fontSize(15));
	//dragon bone
	_dragonBonesData = _factory.loadDragonBonesData("npc/ZhangZhen_ske.json");
	_factory.loadTextureAtlasData("npc/ZhangZhen_tex.json");
	_armatureDisplay = _factory.buildArmatureDisplay("Armature");
	_armature = _armatureDisplay->getArmature();
	_armatureDisplay->setScale(0.4f);
	_armatureDisplay->setPositionY(-20);
	this->addChild(_armatureDisplay);

	_sellItems.push_back(std::make_tuple((int)RPGTool::ItemID::Cloth_Farmer, RPGTool::getRandMaxItemValue(), 70));
	_sellItems.push_back(std::make_tuple((int)RPGTool::ItemID::Pant_Farmer, RPGTool::getRandMaxItemValue(), 70));
	_sellItems.push_back(std::make_tuple((int)RPGTool::ItemID::Weapon_NoobSword, RPGTool::getRandMaxItemValue(), 80));
	_sellItems.push_back(std::make_tuple((int)RPGTool::ItemID::Weapon_Shadow, RPGTool::getRandMaxItemValue(), 10));
	_sellItems.push_back(std::make_tuple((int)RPGTool::ItemID::Weapon_Blody, RPGTool::getRandMaxItemValue(), 5));
	_sellItems.push_back(std::make_tuple((int)RPGTool::ItemID::Weapon_NoobSword, RPGTool::getRandMaxItemValue(), 50));
	_sellItems.push_back(std::make_tuple((int)RPGTool::ItemID::Cloth_BlueKnight, RPGTool::getRandMaxItemValue(), 10));
	_sellItems.push_back(std::make_tuple((int)RPGTool::ItemID::Pant_BlueKnight, RPGTool::getRandMaxItemValue(), 10));
	_sellItems.push_back(std::make_tuple((int)RPGTool::ItemID::Cloth_Blody, RPGTool::getRandMaxItemValue(), 5));
	_sellItems.push_back(std::make_tuple((int)RPGTool::ItemID::Pant_Blody, RPGTool::getRandMaxItemValue(), 5));
	_sellItems.push_back(std::make_tuple((int)RPGTool::ItemID::Cloth_ChiuanJen, RPGTool::getRandMaxItemValue(), 100));
	_sellItems.push_back(std::make_tuple((int)RPGTool::ItemID::Pant_ChiuanJen, RPGTool::getRandMaxItemValue(), 100));
}

//@ZhangHen
ZhangHen * ZhangHen::create(bool isShowCase)
{
	ZhangHen* ptr = new(std::nothrow)ZhangHen;
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
ZhangHen::ZhangHen()
{
	_missionState = MissionMG::State::TalkZhangHen;
	_unlockDialogCodeStage = (int)MissionMG::MainName::FirstMeetZhangHen;
	this->setAnchorPoint(Vec2(0.5, 0));
	this->setContentSize(Size(0, 0));
	this->_isNpcCanMove = true;
	status._faceDirection = true;

	_dialogXY.x = 35;
	_dialogXY.y = 75;
	_visibleRange = 150;
	_moveSpeed = 0.25f;
	_nameCode = "ZH";
	_name = "ZhangHen";
	_chineseName = JsonParser::getJsonString("Code_Table", "ZH", JsonParser::JsonType::TableCode);
	_DialogCode = "ZhangHen";
	_chatSprite = Sprite::create("sprite/chatSprite.png");
	_chatLabel = Label::createWithTTF(
		MajitooTool::SplitStr(JsonParser::getNPCJsonFile("ZhangHen", "Talk"), MajitooTool::SplitType::MarkFront),
		JsonParser::fontType(), JsonParser::fontSize(15));
	//dragon bone
	_dragonBonesData = _factory.loadDragonBonesData("npc/ZhangHen_ske.json");
	_factory.loadTextureAtlasData("npc/ZhangHen_tex.json");
	_armatureDisplay = _factory.buildArmatureDisplay("Armature");
	_armature = _armatureDisplay->getArmature();
	_armatureDisplay->setScale(0.4f);
	_armatureDisplay->setPositionY(-20);
	this->addChild(_armatureDisplay);

	_sellItems.push_back(std::make_tuple((int)RPGTool::ItemID::Cloth_Farmer, RPGTool::getRandMaxItemValue(), 80));
	_sellItems.push_back(std::make_tuple((int)RPGTool::ItemID::Pant_Farmer, RPGTool::getRandMaxItemValue(), 80));
	_sellItems.push_back(std::make_tuple((int)RPGTool::ItemID::Weapon_NoobSword, RPGTool::getRandMaxItemValue(), 70));
	_sellItems.push_back(std::make_tuple((int)RPGTool::ItemID::Cloth_BlueKnight, RPGTool::getRandMaxItemValue(), 10));
	_sellItems.push_back(std::make_tuple((int)RPGTool::ItemID::Pant_BlueKnight, RPGTool::getRandMaxItemValue(), 10));
	_sellItems.push_back(std::make_tuple((int)RPGTool::ItemID::Cloth_Blody, RPGTool::getRandMaxItemValue(), 10));
	_sellItems.push_back(std::make_tuple((int)RPGTool::ItemID::Pant_Blody, RPGTool::getRandMaxItemValue(), 10));
	_sellItems.push_back(std::make_tuple((int)RPGTool::ItemID::Cloth_ChiuanJen, RPGTool::getRandMaxItemValue(), 100));
	_sellItems.push_back(std::make_tuple((int)RPGTool::ItemID::Pant_ChiuanJen, RPGTool::getRandMaxItemValue(), 100));
}

//@ZhangFong
ZhangFong * ZhangFong::create(bool isShowCase)
{
	ZhangFong* ptr = new(std::nothrow)ZhangFong;
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
ZhangFong::ZhangFong()
{
	_missionState = MissionMG::State::TalkZhangFong;
	_unlockDialogCodeStage = (int)MissionMG::MainName::FirstMeetZhangFong;
	this->setAnchorPoint(Vec2(0.5, 0));
	this->setContentSize(Size(0, 0));
	this->_isNpcCanMove = true;
	status._faceDirection = true;

	_dialogXY.x = 35;
	_dialogXY.y = 75;
	_visibleRange = 150;
	_moveSpeed = 0.25f;
	_nameCode = "ZF";
	_name = "ZhangFong";
	_chineseName = JsonParser::getJsonString("Code_Table", "ZF", JsonParser::JsonType::TableCode);
	_DialogCode = "ZhangFong";
	_chatSprite = Sprite::create("sprite/chatSprite.png");
	_chatLabel = Label::createWithTTF(
		MajitooTool::SplitStr(JsonParser::getNPCJsonFile("ZhangFong", "Talk"), MajitooTool::SplitType::MarkFront),
		JsonParser::fontType(), JsonParser::fontSize(15));
	//dragon bone
	_dragonBonesData = _factory.loadDragonBonesData("npc/ZhangFong_ske.json");
	_factory.loadTextureAtlasData("npc/ZhangFong_tex.json");
	_armatureDisplay = _factory.buildArmatureDisplay("Armature");
	_armature = _armatureDisplay->getArmature();
	_armatureDisplay->setScale(0.4f);
	_armatureDisplay->setPositionY(-20);
	this->addChild(_armatureDisplay);

	_sellItems.push_back(std::make_tuple((int)RPGTool::ItemID::Cloth_Farmer, RPGTool::getRandMaxItemValue(), 70));
	_sellItems.push_back(std::make_tuple((int)RPGTool::ItemID::Pant_Farmer, RPGTool::getRandMaxItemValue(), 70));
	_sellItems.push_back(std::make_tuple((int)RPGTool::ItemID::Weapon_NoobSword, RPGTool::getRandMaxItemValue(), 100));
	_sellItems.push_back(std::make_tuple((int)RPGTool::ItemID::Cloth_BlueKnight, RPGTool::getRandMaxItemValue(), 10));
	_sellItems.push_back(std::make_tuple((int)RPGTool::ItemID::Pant_BlueKnight, RPGTool::getRandMaxItemValue(), 10));
	_sellItems.push_back(std::make_tuple((int)RPGTool::ItemID::Cloth_ChiuanJen, RPGTool::getRandMaxItemValue(), 100));
	_sellItems.push_back(std::make_tuple((int)RPGTool::ItemID::Pant_ChiuanJen, RPGTool::getRandMaxItemValue(), 100));
	_sellItems.push_back(std::make_tuple((int)RPGTool::ItemID::Weapon_Shadow, RPGTool::getRandMaxItemValue(), 1));
	_sellItems.push_back(std::make_tuple((int)RPGTool::ItemID::Weapon_Blody, RPGTool::getRandMaxItemValue(), 1));
}

//@ZhangWo
ZhangWo * ZhangWo::create(bool isShowCase)
{
	ZhangWo* ptr = new(std::nothrow)ZhangWo;
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
ZhangWo::ZhangWo()
{
	_missionState = MissionMG::State::TalkZhangWo;
	_unlockDialogCodeStage = (int)MissionMG::MainName::FirstMeetZhangWo;
	this->setAnchorPoint(Vec2(0.5, 0));
	this->setContentSize(Size(0, 0));
	this->_isNpcCanMove = true;
	status._faceDirection = true;

	_dialogXY.x = 35;
	_dialogXY.y = 75;
	_visibleRange = 150;
	_moveSpeed = 0.25f;
	_nameCode = "ZW";
	_name = "ZhangWo";
	_chineseName = JsonParser::getJsonString("Code_Table", "ZW", JsonParser::JsonType::TableCode);
	_DialogCode = "ZhangWo";
	_chatSprite = Sprite::create("sprite/chatSprite.png");
	_chatLabel = Label::createWithTTF(
		MajitooTool::SplitStr(JsonParser::getNPCJsonFile("ZhangWo", "Talk"), MajitooTool::SplitType::MarkFront),
		JsonParser::fontType(), JsonParser::fontSize(15));
	//dragon bone
	_dragonBonesData = _factory.loadDragonBonesData("npc/ZhangWo_ske.json");
	_factory.loadTextureAtlasData("npc/ZhangWo_tex.json");
	_armatureDisplay = _factory.buildArmatureDisplay("Armature");
	_armature = _armatureDisplay->getArmature();
	_armatureDisplay->setScale(0.4f);
	_armatureDisplay->setPositionY(-20);
	this->addChild(_armatureDisplay);

	_sellItems.push_back(std::make_tuple((int)RPGTool::ItemID::Cloth_Farmer, RPGTool::getRandMaxItemValue(), 80));
	_sellItems.push_back(std::make_tuple((int)RPGTool::ItemID::Pant_Farmer, RPGTool::getRandMaxItemValue(), 80));
	_sellItems.push_back(std::make_tuple((int)RPGTool::ItemID::Weapon_NoobSword, RPGTool::getRandMaxItemValue(), 90));
	_sellItems.push_back(std::make_tuple((int)RPGTool::ItemID::Cloth_BlueKnight, RPGTool::getRandMaxItemValue(), 10));
	_sellItems.push_back(std::make_tuple((int)RPGTool::ItemID::Pant_BlueKnight, RPGTool::getRandMaxItemValue(), 10));
	_sellItems.push_back(std::make_tuple((int)RPGTool::ItemID::Weapon_Shadow, RPGTool::getRandMaxItemValue(), 3));
	_sellItems.push_back(std::make_tuple((int)RPGTool::ItemID::Weapon_Blody, RPGTool::getRandMaxItemValue(), 3));
	_sellItems.push_back(std::make_tuple((int)RPGTool::ItemID::Cloth_ChiuanJen, RPGTool::getRandMaxItemValue(), 50));
	_sellItems.push_back(std::make_tuple((int)RPGTool::ItemID::Pant_ChiuanJen, RPGTool::getRandMaxItemValue(), 50));
}