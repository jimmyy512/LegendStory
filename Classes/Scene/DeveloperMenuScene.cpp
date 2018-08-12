#include "DeveloperMenuScene.h"
#include "Scene\SplashSceneFactory.h"
#include "Scene\CreateCharacterScene.h"
#include "Tools\JsonParser.h"
#include "MissionMG.h"
using namespace MyTools;
USING_NS_CC;
Scene* DeveloperMenuScene::createScene()
{
    auto scene = Scene::create();
    auto layer = DeveloperMenuScene::create();
    scene->addChild(layer);
    return scene;
}
bool DeveloperMenuScene::init()
{
    if ( !Layer::init() )
    {
        return false;
    }
    visible = Director::getInstance()->getVisibleSize();
    origin = Director::getInstance()->getVisibleOrigin();
	Sprite* BackGround = Sprite::create("sprite/DeveloperMenu.png");
	BackGround->setPosition(visible.width / 2+origin.x, visible.height / 2+origin.y);
	this->addChild(BackGround,0);
	

	auto ScrollModeLabel=Label::createWithTTF(JsonParser::getJsonString("DEVMenuScene","ScrollMode",
		JsonParser::JsonType::UserInterFace), JsonParser::fontType(), JsonParser::fontSize(26));
	ScrollModeLabel->setColor(Color3B::BLACK);
	auto BattleModeLabel = Label::createWithTTF(JsonParser::getJsonString("DEVMenuScene", "BattleMode",
		JsonParser::JsonType::UserInterFace), JsonParser::fontType(), JsonParser::fontSize(26));
	BattleModeLabel->setColor(Color3B::BLACK);
	auto CreateCharacterSceneLabel=Label::createWithTTF(JsonParser::getJsonString("DEVMenuScene", "CreateCharacterScene",
		JsonParser::JsonType::UserInterFace), JsonParser::fontType(), JsonParser::fontSize(26));
	CreateCharacterSceneLabel->setColor(Color3B::BLACK);
	auto LuoyangSceneLabel = Label::createWithTTF(JsonParser::getJsonString("DEVMenuScene", "ScrollMode_Luoyang",
		JsonParser::JsonType::UserInterFace), JsonParser::fontType(), JsonParser::fontSize(26));
	LuoyangSceneLabel->setColor(Color3B::BLACK);
	auto BattleMode2Label = Label::createWithTTF(JsonParser::getJsonString("DEVMenuScene", "TestMap",
		JsonParser::JsonType::UserInterFace), JsonParser::fontType(), JsonParser::fontSize(26));
	BattleMode2Label->setColor(Color3B::BLACK);
	auto BackMount = Label::createWithTTF(JsonParser::getJsonString("DEVMenuScene", "BackMount",
		JsonParser::JsonType::UserInterFace), JsonParser::fontType(), JsonParser::fontSize(26));
	BackMount->setColor(Color3B::BLACK);
	auto ChiuanJen = Label::createWithTTF(JsonParser::getJsonString("DEVMenuScene", "ChiuanJen",
		JsonParser::JsonType::UserInterFace), JsonParser::fontType(), JsonParser::fontSize(26));
	ChiuanJen->setColor(Color3B::BLACK);
	auto Cave = Label::createWithTTF(JsonParser::getJsonString("DEVMenuScene", "Cave",
		JsonParser::JsonType::UserInterFace), JsonParser::fontType(), JsonParser::fontSize(26));
	Cave->setColor(Color3B::BLACK);

	auto ScrollModeMenuItem=MenuItemLabel::create(ScrollModeLabel,CC_CALLBACK_1(DeveloperMenuScene::ScrollModeMenuItemCallBack,this));
	ScrollModeMenuItem->setPosition(visible.width / 10 * 2 + origin.x, visible.height / 10 * 7 + origin.y);

	auto BattleModeMenuItem = MenuItemLabel::create(BattleModeLabel, CC_CALLBACK_1(DeveloperMenuScene::BattleModeMenuItemCallBack, this));
	BattleModeMenuItem->setPosition(visible.width / 10 * 2 + origin.x, visible.height / 10 * 5 + origin.y);

	auto CreateCharacterMenuItem= MenuItemLabel::create(CreateCharacterSceneLabel, CC_CALLBACK_1(DeveloperMenuScene::CreateCharacterSceneItemCallBack, this));
	CreateCharacterMenuItem->setPosition(visible.width / 10 * 2 + origin.x, visible.height / 10 * 3 + origin.y);

	auto LuoyangSceneMenuItem = MenuItemLabel::create(LuoyangSceneLabel, CC_CALLBACK_1(DeveloperMenuScene::LuoYangSceneCallBack, this));
	LuoyangSceneMenuItem->setPosition(visible.width / 10 * 5 + origin.x, visible.height / 10 * 7 + origin.y);

	auto BattleMode2MenuItem = MenuItemLabel::create(BattleMode2Label, CC_CALLBACK_1(DeveloperMenuScene::TestMapMenuItemCallBack, this));
	BattleMode2MenuItem->setPosition(visible.width / 10 * 5 + origin.x, visible.height / 10 * 5 + origin.y);

	auto BattleMountMenuItem = MenuItemLabel::create(BackMount, CC_CALLBACK_1(DeveloperMenuScene::BackMountSceneCallBack, this));
	BattleMountMenuItem->setPosition(visible.width / 10 * 5 + origin.x, visible.height / 10 * 3 + origin.y);

	auto ChiuanJenMenuItem = MenuItemLabel::create(ChiuanJen, CC_CALLBACK_1(DeveloperMenuScene::ChiuanJenSceneCallBack, this));
	ChiuanJenMenuItem->setPosition(visible.width / 10 * 8 + origin.x, visible.height / 10 * 7 + origin.y);

	auto CaveMenuItem = MenuItemLabel::create(Cave, CC_CALLBACK_1(DeveloperMenuScene::CaveSceneCallBack, this));
	CaveMenuItem->setPosition(visible.width / 10 * 8 + origin.x, visible.height / 10 * 5 + origin.y);

	_menu = Menu::create(ScrollModeMenuItem, BattleModeMenuItem, CreateCharacterMenuItem, LuoyangSceneMenuItem, BattleMode2MenuItem,
		BattleMountMenuItem, ChiuanJenMenuItem, CaveMenuItem,NULL);
	_menu->setPosition(Vec2::ZERO);
	this->addChild(_menu,1);
	
	_page2Node = Node::create();
	_page2Node->setVisible(false);
	this->addChild(_page2Node, 1);

	//調整任務進度
	_MissionName= Label::createWithTTF(MissionMG::getInstance()->getCurrentMissionTitle(),
		JsonParser::fontType(), JsonParser::fontSize(24));
	_MissionName->setColor(Color3B::BLACK);
	_MissionName->setPosition(visible.width / 10 * 2 + origin.x, visible.height / 10 * 7 + origin.y);
	_page2Node->addChild(_MissionName);

	auto missionSub = ShakeButton::create("0", "fonts/arial.ttf", 16, Color4B::BLACK, "ui/Button.png",
		[this](ShakeButton* shakeButton)
	{
		MissionMG::getInstance()->PrevMission();
		_MissionName->setString(MissionMG::getInstance()->getCurrentMissionTitle());
	});
	missionSub->setScaleX(0.5f);
	missionSub->setPosition(_MissionName->getPositionX() + origin.x+100, _MissionName->getPositionY());
	_page2Node->addChild(missionSub);

	auto missionAdd = ShakeButton::create("1", "fonts/arial.ttf", 16, Color4B::BLACK, "ui/Button.png",
		[this](ShakeButton* shakeButton)
	{
		MissionMG::getInstance()->NextMission();
		_MissionName->setString(MissionMG::getInstance()->getCurrentMissionTitle());
	});
	missionAdd->setScaleX(0.5f);
	missionAdd->setPosition(_MissionName->getPositionX() + origin.x + 140, _MissionName->getPositionY());
	_page2Node->addChild(missionAdd);

	//自動加血加魔測試
	auto autoAdd = Label::createWithTTF(JsonParser::getJsonString("DEVMenuScene", "AutoAdd",JsonParser::JsonType::UserInterFace),
		JsonParser::fontType(), JsonParser::fontSize(24));
	autoAdd->setColor(Color3B::BLACK);
	autoAdd->setPosition(visible.width / 10 * 2 + origin.x, visible.height / 10 * 5 + origin.y);
	_page2Node->addChild(autoAdd);

	auto _newCheckBox = ui::CheckBox::create("ui/checkBackGround.png", "ui/check.png");
	_newCheckBox->setPosition(Vec2(autoAdd->getPositionX()+110, autoAdd->getPositionY()));
	_newCheckBox->setScale(0.3f);
	_newCheckBox->addEventListener(CC_CALLBACK_2(DeveloperMenuScene::checkBoxCallback, this));
	_page2Node->addChild(_newCheckBox);

	if (JsonParser::getJsonString("Debug", "ForceAddHPMP", JsonParser::JsonType::Save) == "1")
		_newCheckBox->setSelected(true);
	else
		_newCheckBox->setSelected(false);

	//條等級
	_setLevelLabel = Label::createWithTTF(JsonParser::getJsonString("DEVMenuScene", "SetLevel", JsonParser::JsonType::UserInterFace),
		JsonParser::fontType(), JsonParser::fontSize(24));
	_setLevelLabel->setColor(Color3B::BLACK);
	_setLevelLabel->setPosition(visible.width / 10 * 2 + origin.x, visible.height / 10 * 3 + origin.y);
	_page2Node->addChild(_setLevelLabel);

	auto levelSub = ShakeButton::create("0", "fonts/arial.ttf", 16, Color4B::BLACK, "ui/Button.png",
		[this](ShakeButton* shakeButton)
	{
		auto currentLevel=atoi(JsonParser::getJsonString("Infor", "Level", JsonParser::JsonType::Save).c_str());
		if ((currentLevel - 1) == 0)
			return;
		char newLevel[10];
		sprintf(newLevel, "%d", --currentLevel);
		JsonParser::setSave("Infor", "Level", newLevel);
		_setLevelLabel->setString(newLevel);
	});
	levelSub->setScaleX(0.5f);
	levelSub->setPosition(_setLevelLabel->getPositionX() + origin.x + 100, _setLevelLabel->getPositionY());
	_page2Node->addChild(levelSub);

	auto levelAdd = ShakeButton::create("1", "fonts/arial.ttf", 16, Color4B::BLACK, "ui/Button.png",
		[this](ShakeButton* shakeButton)
	{
		auto currentLevel = atoi(JsonParser::getJsonString("Infor", "Level", JsonParser::JsonType::Save).c_str());
		char newLevel[10];
		sprintf(newLevel, "%d", ++currentLevel);
		JsonParser::setSave("Infor", "Level", newLevel);
		_setLevelLabel->setString(newLevel);
	});
	levelAdd->setScaleX(0.5f);
	levelAdd->setPosition(_setLevelLabel->getPositionX() + origin.x + 140, _setLevelLabel->getPositionY());
	_page2Node->addChild(levelAdd);


	//頁面管理
	_pageBtn = ShakeButton::create("Next", "fonts/arial.ttf", 16, Color4B::BLACK, "ui/Button.png",
		[this](ShakeButton* shakeButton)
	{
		_pageTurn = !_pageTurn;
		if (_pageTurn)
		{
			_menu->setVisible(false);
			_page2Node->setVisible(true);
		}
		else
		{
			_menu->setVisible(true);
			_page2Node->setVisible(false);
		}
	});
	_pageBtn->setPosition(visible.width/2+origin.x,30);
	this->addChild(_pageBtn, 100);

    return true;
}
void DeveloperMenuScene::ScrollModeMenuItemCallBack(Ref* sender)
{
	Director::getInstance()->replaceScene(SplashSceneFactory::createScene(SplashSceneFactory::preLoadScene::preLoadForestField));
}
void DeveloperMenuScene::BattleModeMenuItemCallBack(Ref* sender)
{
	Director::getInstance()->replaceScene(SplashSceneFactory::createScene(SplashSceneFactory::preLoadScene::preLoadBattleTrainingField));
}
void DeveloperMenuScene::TestMapMenuItemCallBack(Ref* sender)
{
	Director::getInstance()->replaceScene(SplashSceneFactory::createScene(SplashSceneFactory::preLoadScene::preLoadTestMap));
}
void DeveloperMenuScene::CreateCharacterSceneItemCallBack(Ref* sender)
{
	Director::getInstance()->replaceScene(ScrollMode::CreateCharacterScene::createScene());
}
void DeveloperMenuScene::LuoYangSceneCallBack(Ref * sender)
{
	Director::getInstance()->replaceScene(SplashSceneFactory::createScene(SplashSceneFactory::preLoadScene::preLoadLuoYangCity));
}
void DeveloperMenuScene::BackMountSceneCallBack(Ref * sender)
{
	Director::getInstance()->replaceScene(SplashSceneFactory::createScene(SplashSceneFactory::preLoadScene::preLoadChiuanJenMount));
}
void DeveloperMenuScene::ChiuanJenSceneCallBack(Ref * sender)
{
	Director::getInstance()->replaceScene(SplashSceneFactory::createScene(SplashSceneFactory::preLoadScene::preLoadChiuanJen));
}
void DeveloperMenuScene::CaveSceneCallBack(Ref * sender)
{
	Director::getInstance()->replaceScene(SplashSceneFactory::createScene(SplashSceneFactory::preLoadScene::preLoadCave));
}
void DeveloperMenuScene::checkBoxCallback(cocos2d::Ref * ref, ui::CheckBox::EventType type)
{
	switch (type)
	{
	case cocos2d::ui::CheckBox::EventType::SELECTED:
		JsonParser::setSave("Debug", "ForceAddHPMP", "1");
		break;
	case cocos2d::ui::CheckBox::EventType::UNSELECTED:
		JsonParser::setSave("Debug", "ForceAddHPMP", "0");
		break;
	default:
		break;
	}
}


