#include "Dialog.h"
#include "tools\JsonParser.h"
#include "tools\MajitooTool.h"
#include "UI\ScrollMode\HUDlayer.h"
#include "ui\MyButton.h"
#include "Scene\StoreScene.h"
#include "MissionMG.h"
#include "Monster\ScrollMode\ScrollMonsterFactory.h"
#include "TipsLabel.h"
USING_NS_CC;
using namespace MyTools;

void Dialog::onEnterTransitionDidFinish()
{
	Layer::onEnterTransitionDidFinish();
	ScrollMode::HUDlayer::getHUDlayer()->_isReplaceingScene = false;
}

Dialog::Dialog()
{
	_isRunLabelAnimation = false;
	_isMCTalking = false;
	_dramaAnimationCount = 1;
	_dramaIndex = 0;
	_dramaLabel = nullptr;
	_talkingNpc = nullptr;
}

Dialog::~Dialog()
{

}


Scene* Dialog::createScene(cocos2d::RenderTexture* capture, TalkCharacter* talkCharacter, std::string dialogCode)
{
	auto scene = Scene::create();
	auto layer = Dialog::create(talkCharacter, dialogCode);
	scene->addChild(layer, 1);
	if (capture != nullptr)
	{
		Size visible = Director::getInstance()->getVisibleSize();
		Vec2 origin = Director::getInstance()->getVisibleOrigin();
		auto pause_background = Sprite::createWithTexture(capture->getSprite()->getTexture());
		pause_background->setFlipY(true);
		pause_background->setColor(Color3B::GRAY);
		pause_background->setPosition(Vec2(visible.width / 2 + origin.x, visible.height / 2 + origin.y));
		layer->addChild(pause_background, 0);
	}
	return scene;
}

Dialog * Dialog::create(TalkCharacter* talkCharacter,std::string dialogCode)
{
	Dialog* dialog_ptr = new (std::nothrow)Dialog;
	if (dialog_ptr && dialog_ptr->init(talkCharacter, dialogCode))
	{
		dialog_ptr->autorelease();
	}
	else
	{
		CC_SAFE_DELETE(dialog_ptr);
	}
	return dialog_ptr;
}

bool Dialog::init(TalkCharacter* talkCharacter,std::string dialogCode)
{
	if(!Layer::init())
		return false;
	_isReplacing = false;
	this->_talkCharacter = talkCharacter;
	
	_sceneNode= CSLoader::createNode("dialogScene/dialogScene.csb");
	_sceneNode->setVisible(true);
	this->addChild(_sceneNode,1);

	//mainCharacter
	this->_maincharacter = ScrollMode::MainCharacter::create(0.f, 0.f, true);
	this->_maincharacter->_exCharacter->setScale(0.3f);
	this->_maincharacter->_exCharacter->play("Idle", -1, false);
	_sceneNode->getChildByName("MainCharacterPos")->addChild(_maincharacter);

	//option dialog
	_optionDialog = this->_sceneNode->getChildByName("DialogOptionBG");
	_optionDialog->setPosition(_sceneNode->getChildByName("VisiblePos")->getPosition());
	//option title
	Label* titleLabel = Label::createWithTTF(JsonParser::getJsonString("DialogScene", "Title", JsonParser::JsonType::UserInterFace),
		JsonParser::fontType(), JsonParser::fontSize(20));
	titleLabel->setColor(Color3B::WHITE);
	titleLabel->setAnchorPoint(Vec2(0.5f, 0.5f));
	titleLabel->enableGlow(Color4B::BLACK);
	titleLabel->setPosition(_optionDialog->getChildByName("DialogOptionTitlePos")->getPosition());
	_optionDialog->addChild(titleLabel);
	//first option //開始對話
	auto opt1= ShakeButton::create(JsonParser::getJsonString("DialogScene", "Option1", JsonParser::JsonType::UserInterFace),
		JsonParser::fontType(), JsonParser::fontSize(20), Color4B::BLACK,"dialogScene/FirstOption.png", [&](ShakeButton* shakeBtn)
	{
		if (JsonParser::getFreeItemSpaceCountFromSave() >= 2)
		{//在和NPC對話時,玩家必須保持背包有五格空位,因為可能一些劇情會送玩家道具
			if (MissionMG::getInstance()->isCompleteMission(this->_talkCharacter->getMissionState()))
			{//任務判斷
				this->_DialogCode = MissionMG::getInstance()->getCurrentMissionDialogCode();
				MissionMG::getInstance()->NextMission();
			}
			else
			{
				this->_DialogCode = this->_talkCharacter->getDialogCode();
			}
			this->startDialog(0);
		}
		else
		{//跳出清背包提示
			TipsLabel::showTip(MyTools::JsonParser::getJsonString("Normal", "CheckBagSpace", MyTools::JsonParser::JsonType::UserInterFace), this, 20, false);
		}
	});
	opt1->setPosition(_optionDialog->getChildByName("FirstOptionPos")->getPosition());
	_optionDialog->addChild(opt1);
	//second option //開始交易
	auto opt2 = ShakeButton::create(JsonParser::getJsonString("DialogScene", "Option2", JsonParser::JsonType::UserInterFace),
		JsonParser::fontType(), JsonParser::fontSize(20), Color4B::BLACK, "dialogScene/SecondOption.png", [this](ShakeButton* shakeBtn)
	{
		auto visible = Director::getInstance()->getVisibleSize();
		auto origin = Director::getInstance()->getVisibleOrigin();
		auto capture = RenderTexture::create(visible.width + origin.x, visible.height + origin.y);
		capture->begin();
		this->getParent()->visit();
		capture->end();
		Director::getInstance()->pushScene(TransitionProgressRadialCCW::create(0.3f, StoreScene::createScene(capture, this->_talkCharacter->getNPCInstance())));
	});
	opt2->setPosition(_optionDialog->getChildByName("SecondOptionPos")->getPosition());
	_optionDialog->addChild(opt2);
	//third option 
	auto opt3 = ShakeButton::create(JsonParser::getJsonString("DialogScene", "Option3", JsonParser::JsonType::UserInterFace),
		JsonParser::fontType(), JsonParser::fontSize(20), Color4B::BLACK, "dialogScene/ThirdOption.png", [this](ShakeButton* shakeBtn)
	{
		if (_isReplacing)
		{
			return true;
		}
		_isReplacing = true;
		Director::getInstance()->popScene([](Scene* scene)
		{
			return TransitionProgressRadialCCW::create(0.3f, scene);
		});
		return true;
	});
	opt3->setPosition(_optionDialog->getChildByName("ThirdOptionPos")->getPosition());
	_optionDialog->addChild(opt3);

	//name Tag
	_nameTag = (Sprite*)_sceneNode->getChildByName("NameTag");
	//name label
	_nameLabel = Label::createWithTTF("", JsonParser::fontType(), JsonParser::fontSize(34));
	_nameLabel->setColor(Color3B::BLACK);
	_nameTag->addChild(_nameLabel);


	//DialogTip
	_dialogTip =(Sprite*)_sceneNode->getChildByName("DialogTip");

	//SkipSprite listener
	_skipSprite = _sceneNode->getChildByName("Skip");
	_skipSprite->setVisible(false);

	auto skipSpriteListener=EventListenerTouchOneByOne::create();
	skipSpriteListener->onTouchBegan=([&](Touch* touch,Event* event)
	{
		if (_isSceneInTalking)
		{
			if (event->getCurrentTarget()->getBoundingBox().containsPoint(touch->getLocation()))
			{
				if (this->_isReplacing)
				{
					return false;
				}
				this->_isReplacing = true;
				Director::getInstance()->popScene([](Scene* scene)
				{
					return TransitionProgressRadialCCW::create(0.3f, scene);
				});
			}
		}
		return true;
	});
	Director::getInstance()->getEventDispatcher()->addEventListenerWithSceneGraphPriority(skipSpriteListener, _skipSprite);

	auto SceneTouchListener = EventListenerTouchOneByOne::create();
	SceneTouchListener->onTouchBegan = ([this](Touch* touch, Event* event)
	{//點擊場景時
		if (_isSceneInTalking)
		{
			if (this->_isRunLabelAnimation == true)
			{//文字動畫停止
				this->_isRunLabelAnimation = false;
				_dramaAnimationCount = 1;
				_dramaLabel->setString(_dialogStr);
				//拿掉註解時，NPC說完話會自動恢復成原來的表情
				/*if (_talkingNpc != nullptr)
					_talkingNpc->doIdleAnimation();*/
			}
			else
			{//準備跳下一段話
				_dramaIndex++;
				_dramaLabel->removeFromParent();
				_dramaLabel = nullptr;
				if (_talkingNpc != nullptr)
				{
					_talkingNpc->removeFromParent();
					_talkingNpc = nullptr;
				}
				this->startDialog(_dramaIndex);
			}
			return true;
		}
	});
	Director::getInstance()->getEventDispatcher()->addEventListenerWithSceneGraphPriority(SceneTouchListener, this);

	this->schedule([this](float dt)
	{
		if (_isSceneInTalking)
		{
			if (this->_isRunLabelAnimation)
			{
				if (_dramaAnimationCount == _dialogStr.length() + 1)
				{//文字動畫自然播完時
					this->_isRunLabelAnimation = false;
					_dramaAnimationCount = 1;
					//拿掉註解時，NPC說完話會自動恢復成原來的表情
					/*if (_talkingNpc != nullptr)
						_talkingNpc->doIdleAnimation();*/
				}
				else
				{//繼續撥放文字動畫
					_dramaLabel->setString(_dialogStr.substr(0, _dramaAnimationCount));
					_dramaLabel->setAnchorPoint(Vec2(0, 1));
				}
				_dramaAnimationCount++;
			}
		}
	}, 0.05f, "DramaAnimationUpdate");

	this->schedule([this](float dt)
	{
		static bool goDown = true;
		if (_dialogTip->getPositionY() < 15.f)
		{
			goDown = false;
		}
		else if (_dialogTip->getPositionY() > 23.f)
		{
			goDown = true;
		}
		if (goDown)
			_dialogTip->setPositionY(_dialogTip->getPositionY() - 1.f);
		else
			_dialogTip->setPositionY(_dialogTip->getPositionY() + 1.f);
	}, 0.04f, "DialogTipUpdate");
	

	if (dialogCode!="")
	{//強制直接開始對話
		this->_DialogCode = dialogCode;
		this->startDialog(0);
	}
	else
	{
		this->waitPlayerOperationScreen();
	}


	return true;
}

void Dialog::startDialog(int index)
{
	if (_talkingNpc != nullptr)
	{
		_talkingNpc->removeFromParent();
		_talkingNpc = nullptr;
	}
	this->_optionDialog->setPosition(this->_sceneNode->getChildByName("InVisiblePos")->getPosition());
	this->_isSceneInTalking = true;
	this->_skipSprite->setVisible(true);
	_dialogStr = JsonParser::getDialogString(this->_DialogCode, index);
	if (_dialogStr.empty())
	{//沒有下一句文本，準備退出對話模式
		if (this->_isReplacing)
		{
			return;
		}
		this->_isReplacing = true;
		Director::getInstance()->popScene([](Scene* scene)
		{
			return TransitionProgressRadialCCW::create(0.3f, scene);
		});
	}
	else
	{
		this->_isRunLabelAnimation = true;
		std::string nameCode = MajitooTool::SplitStr(_dialogStr, MajitooTool::SplitType::wantNameCode);
		if (nameCode == "NA")
		{
			_dialogStr = MajitooTool::SplitStr(JsonParser::getDialogString(this->_DialogCode, index), MajitooTool::SplitType::MarkFront);
			_nameLabel->setString(JsonParser::getJsonString("Code_Table","NA",JsonParser::JsonType::TableCode));
			this->_isMCTalking = false;
		}
		else if (nameCode == "NoNameCode")
		{//如果json文本 drama檔案中 句子中沒有指定NPC代號，就用開啟此對話的NPC代號，當作代號
			_dialogStr = MajitooTool::SplitStr(JsonParser::getDialogString(this->_DialogCode, index), MajitooTool::SplitType::MarkFront);
			_nameLabel->setString(this->_talkCharacter->_chineseName);
			this->_isMCTalking = false;
			this->accordingNameCreateNPC(this->_talkCharacter->_nameCode);
			_talkingNpc->setFacialType(MajitooTool::SplitStr(JsonParser::getDialogString(this->_DialogCode, index), MajitooTool::SplitType::wantFacial));
		}
		else if (nameCode == "MC")
		{//替換成存檔中的玩家名
			_dialogStr = MajitooTool::SplitStr(JsonParser::getDialogString(this->_DialogCode, index), MajitooTool::SplitType::MarkFront);
			_nameLabel->setString(JsonParser::getJsonString("Infor", "CharacterName", JsonParser::JsonType::Save));
			this->_isMCTalking = true;
			_maincharacter->_exCharacter->setArmatureFacial(MajitooTool::SplitStr(JsonParser::getDialogString(this->_DialogCode, index), MajitooTool::SplitType::wantFacial));
		}
		else
		{//NPC名稱不變
			_dialogStr = MajitooTool::SplitStr(JsonParser::getDialogString(this->_DialogCode, index), MajitooTool::SplitType::MarkFront);
			_nameLabel->setString(JsonParser::getJsonString("Code_Table", nameCode.c_str(), JsonParser::JsonType::TableCode));
			this->_isMCTalking = false;
			this->accordingNameCreateNPC(nameCode);
			_talkingNpc->setFacialType(MajitooTool::SplitStr(JsonParser::getDialogString(this->_DialogCode, index), MajitooTool::SplitType::wantFacial));
		}
	}
	ceateDramaLabel();
	if (this->_isMCTalking)
	{
		_nameLabel->setPosition(_sceneNode->getChildByName("NameTag")->getChildByName("NameLabel")->getPosition());
		_nameTag->setPosition(_sceneNode->getChildByName("LeftNameTagPos")->getPosition());
		_dialogTip->setPosition(_sceneNode->getChildByName("LeftDialogTipPos")->getPosition());
		_maincharacter->setVisible(true);
	}
	else
	{
		_nameLabel->setPosition(_sceneNode->getChildByName("NameTag")->getChildByName("NameLabel")->getPosition());
		_nameTag->setPosition(_sceneNode->getChildByName("RightNameTagPos")->getPosition());
		_dialogTip->setPosition(_sceneNode->getChildByName("RightDialogTipPos")->getPosition());
		_maincharacter->setVisible(false);
	}
}


void Dialog::ceateDramaLabel()
{
	if (_dramaLabel != nullptr)
	{
		_dramaLabel->removeFromParent();
		_dramaLabel = nullptr;
	}
	_dramaLabel = Label::createWithTTF("",JsonParser::fontType(), JsonParser::fontSize(20));
	_dramaLabel->setAnchorPoint(Vec2(0, 1));
	if(this->_isMCTalking)
		_dramaLabel->setPosition(_sceneNode->getChildByName("LeftDramaLabelPos")->getPosition());
	else
		_dramaLabel->setPosition(_sceneNode->getChildByName("RightDramaLabelPos")->getPosition());
	_dramaLabel->setMaxLineWidth(330.f);
	_dramaLabel->setColor(Color3B::WHITE);
	this->addChild(_dramaLabel, 2);
}

void Dialog::accordingNameCreateNPC(std::string name)
{
	if (_talkingNpc != nullptr)
	{
		_talkingNpc->removeFromParent();
		_talkingNpc = nullptr;
	}
	if (name == "ZQ" || name == "ZhangQing")
		_talkingNpc = NPCFactory::create(NPCFactory::NPCName::ZhangQing, true);
	else if (name == "ZY" || name == "ZhangYin")
		_talkingNpc = NPCFactory::create(NPCFactory::NPCName::ZhangYin, true);
	else if (name == "ZZ" || name == "ZhangZhen")
		_talkingNpc = NPCFactory::create(NPCFactory::NPCName::ZhangZhen, true);
	else if (name == "ZH" || name == "ZhangHen")
		_talkingNpc = NPCFactory::create(NPCFactory::NPCName::ZhangHen, true);
	else if (name == "ZF" || name == "ZhangFong")
		_talkingNpc = NPCFactory::create(NPCFactory::NPCName::ZhangFong, true);
	else if (name == "ZW" || name == "ZhangWo")
		_talkingNpc = NPCFactory::create(NPCFactory::NPCName::ZhangWo, true);
	else if (name == "ST" || name == "SmallThief")
		_talkingNpc = ScrollMode::ScrollMonsterFactory::createMonster(ScrollMode::ScrollMonsterFactory::MonsterName::SmallThief, true);
	else if (name == "ZB" || name == "Zombie")
		_talkingNpc = ScrollMode::ScrollMonsterFactory::createMonster(ScrollMode::ScrollMonsterFactory::MonsterName::Zombie, true);
	else
		throw "Dialog::accordingNameCreateNPC dont have this npc name";
	_talkingNpc->doIdleAnimation();
	_sceneNode->getChildByName("NPCPos")->addChild(_talkingNpc);
}

void Dialog::waitPlayerOperationScreen()
{
	_nameLabel->setPosition(_sceneNode->getChildByName("NameTag")->getChildByName("NameLabel")->getPosition());
	_nameTag->setPosition(_sceneNode->getChildByName("RightNameTagPos")->getPosition());
	_dialogTip->setPosition(_sceneNode->getChildByName("RightDialogTipPos")->getPosition());
	_maincharacter->setVisible(false);

	////str init
	this->_isMCTalking = false;
	this->ceateDramaLabel();
	_nameLabel->setString(this->_talkCharacter->_chineseName);
	_dramaLabel->setString(MajitooTool::SplitStr(JsonParser::getNPCJsonFile(this->_talkCharacter->_name.c_str(), "Hello"),
		MajitooTool::SplitType::MarkFront));
	//NPC動畫
	accordingNameCreateNPC(this->_talkCharacter->_name);
	_talkingNpc->doIdleAnimation();
	_talkingNpc->setFacialType(NPC::FacialType::NM);

	this->_isSceneInTalking = false;
	
}