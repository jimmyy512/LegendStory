#include "UI\BattleMode\BattleHUDlayer.h"
#include "Map\BattleMode\BattleMap.h"
#include "Monster\BattleMode\SoldierAI.h"
#include "ui\MyButton.h"
#include "ui\UILoadingBar.h"
#include "ui\BattleMode\JoyStick.h"
#include "DebugLayer.h"
#include "Tools\AudioManagement.h"
#include "Scene\SettingMenu.h"
#include "Scene\BattleMode\BattleScene.h"
USING_NS_CC;
using namespace experimental;
using namespace cocos2d::extension;
NS_BATTLEMODE_BEGAN
BattleHUDlayer* BattleHUDlayer::static_BattleHUDlayer = nullptr;
namespace
{
	const float controlBtnBiggerDelta = 0.1f;
}
BattleHUDlayer::BattleHUDlayer():
_isFirstLayerDowing(false)
{
	_isReplaceingScene = false;
}
BattleHUDlayer::~BattleHUDlayer()
{
	static_BattleHUDlayer = nullptr;
}
void BattleHUDlayer::onExit()
{
	Layer::onExit();
	NotificationCenter::getInstance()->removeAllObservers(this);
}
BattleHUDlayer* BattleHUDlayer::create()
{
	BattleHUDlayer* battleHUDlayer = new(std::nothrow)BattleHUDlayer;
	if (battleHUDlayer && battleHUDlayer->init())
	{
		battleHUDlayer->autorelease();
	}
	else
	{
		CC_SAFE_DELETE(battleHUDlayer);
	}
	return battleHUDlayer;
}
BattleHUDlayer* BattleHUDlayer::getBattleHUDlayer()
{
	if (BattleHUDlayer::static_BattleHUDlayer == nullptr)
	{
		return nullptr;
	}
	else
		return BattleHUDlayer::static_BattleHUDlayer;
}
bool BattleHUDlayer::init()
{
	if (!Layer::init())
	{
		return false;
	}
	static_BattleHUDlayer = this;

	visible = Director::getInstance()->getVisibleSize();
	origin = Director::getInstance()->getVisibleOrigin();

	//設定圖示
	auto settingEntryBtn = ShakeButton::create(" ", "fonts/arial.ttf", 12, Color4B::BLACK, "ui/settingEntrySprite.png",
		[this](ShakeButton* btn)
	{
		if (this->_isReplaceingScene == true)
			return;
		this->_isReplaceingScene = true;
		auto visible = Director::getInstance()->getVisibleSize();
		auto origin = Director::getInstance()->getVisibleOrigin();
		AudioManagement::getInstance()->pauseAllAudio();
		auto capture = RenderTexture::create(visible.width + origin.x, visible.height + origin.y);
		capture->begin();
		this->getParent()->visit();
		capture->end();
		Director::getInstance()->pushScene(TransitionPageTurn::create(0.4f, SettingMenu::createScene(capture), true));

	});
	settingEntryBtn->setOpacity(180);
	settingEntryBtn->setScale(0.3f);
	settingEntryBtn->setPosition(visible.width - settingEntryBtn->getBoundingBox().size.width / 2 + origin.x,
		visible.height - settingEntryBtn->getBoundingBox().size.height / 2 + origin.x);
	this->addChild(settingEntryBtn);

	//@Debug
	/*auto Debuglayer=DebugLayer::getInstance();
	Debuglayer->setPosition(0, 0);
	this->addChild(Debuglayer,9999,9999);*/

	_firstLayer = Layer::create();
	_secondLayer = Layer::create();
	
	this->addChild(_firstLayer,FIRST_LAYER);
	this->addChild(_secondLayer,SECOND_LAYER);

	auto ShootBtn = Sprite::create("ui/BattleMode_Skill_Shoot.png");
	auto BayonetBtn = Sprite::create("ui/BattleMode_Skill_Bayonet.png");	
	ShootBtn->setTag(SHOOT);
	BayonetBtn->setTag(BAYONET);
	ShootBtn->setOpacity(__CONTROL_BUTTON_OPACITY);
	BayonetBtn->setOpacity(__CONTROL_BUTTON_OPACITY);
	ShootBtn->setScale(__CONTROL_BUTTON_SCALE + controlBtnBiggerDelta);//shoot button want bigger
	BayonetBtn->setScale(__CONTROL_BUTTON_SCALE);
	//skill control button background position
	ShootBtn->setPosition((visible.width + origin.x) - (ShootBtn->getBoundingBox().size.width / 2),
		ShootBtn->getBoundingBox().size.height / 2 + origin.y);
	BayonetBtn->setPosition(ShootBtn->getPositionX() - ShootBtn->getBoundingBox().size.width,
		ShootBtn->getPositionY());

	_firstLayer->addChild(ShootBtn);
	_firstLayer->addChild(BayonetBtn);

	auto battleScrollBar = Scale9Sprite::create("ui/BattleScroll.jpg");
	battleScrollBar->setOpacity(__CONTROL_BUTTON_OPACITY + 40);
	battleScrollBar->setScale(0.3f);
	battleScrollBar->setPosition(-battleScrollBar->getBoundingBox().size.width / 2,
		battleScrollBar->getBoundingBox().size.height / 2);
	this->addChild(battleScrollBar, BATTLE_SCROLL_BAR, BATTLE_SCROLL_BAR);

	_battleScrollSkills.push_back(Sprite::create("ui/defend.png"));
	_battleScrollSkills.push_back(Sprite::create("ui/easy.png"));
	_battleScrollSkills.push_back(Sprite::create("ui/spear.png"));
	_battleScrollSkills.push_back(Sprite::create("ui/rush.png"));
	const float intervalSpace = visible.width /5;
	int index = 0;
	for (auto& it : _battleScrollSkills)
	{
		_secondLayer->addChild(it,2);
		it->setOpacity(180);
		it->setScale(0.5);
		it->setPosition(-visible.width+it->getBoundingBox().size.width/1.6+index*intervalSpace, 
			it->getBoundingBox().size.height/2 + origin.y);
		index++;
	}
		
	auto switchBtn = Sprite::create("ui/BattleMode_HUD_Triangle.png");
	switchBtn->setPosition(visible.width / 2 + origin.x, switchBtn->getBoundingBox().size.height/2);
	this->addChild(switchBtn, TRIANGLE_SWITCH, TRIANGLE_SWITCH);
	_triangleSwitchListener = EventListenerTouchOneByOne::create();
	_triangleSwitchListener->onTouchBegan = [this](Touch* touch, Event* event)
	{
		Sprite* triangleSwitch = (Sprite*)getChildByTag(TRIANGLE_SWITCH);
		if (event->getCurrentTarget()->getBoundingBox().containsPoint(touch->getLocation()))
		{
			triangleSwitch->setFlipY(!triangleSwitch->isFlipY());
			if (triangleSwitch->isFlipY())
			{
				AudioManagement::getInstance()->play("sound/scrollOpen.mp3", AudioManagement::AudioType::Sound, false);
				this->_isFirstLayerDowing = true;
			}
			else
			{
				AudioManagement::getInstance()->play("sound/scrollClose.mp3", AudioManagement::AudioType::Sound, false);
				this->_isFirstLayerDowing = false;
			}
			this->schedule(schedule_selector(BattleHUDlayer::ClickSwitchBtnCallBack));
		}
		return true;
	};
	Director::getInstance()->getEventDispatcher()->addEventListenerWithSceneGraphPriority(_triangleSwitchListener, switchBtn);
		
	auto Skilllistener = EventListenerTouchOneByOne::create();
	Skilllistener->onTouchBegan = [this](Touch* touch, Event* event)
	{
		if (event->getCurrentTarget()->getBoundingBox().containsPoint(touch->getLocation()) && !_isFirstLayerDowing)
		{
			ShakeBtn(event->getCurrentTarget()->getTag());
			if (event->getCurrentTarget()->getTag() == BattleHUDlayer::SHOOT)
			{
				ShootCallBack();
			}
			if (event->getCurrentTarget()->getTag() == BattleHUDlayer::BAYONET)
			{
				ReadyToShootCallBack();
			}
		}
		return false;
	};
	Director::getInstance()->getEventDispatcher()->addEventListenerWithSceneGraphPriority(Skilllistener, ShootBtn);
	Director::getInstance()->getEventDispatcher()->addEventListenerWithSceneGraphPriority(Skilllistener->clone(), BayonetBtn);
	
	//start Battle
	auto startBattleSprite = Sprite::create("ui/startBattle.png");
	startBattleSprite->setScale(0.8f);
	startBattleSprite->setPosition(visible.width / 2+origin.x, visible.height / 2 + origin.y);
	this->addChild(startBattleSprite);

	auto startBattleSpriteListener = EventListenerTouchOneByOne::create();
	startBattleSpriteListener->onTouchBegan = [this](Touch* touch,Event* event)
	{
		if (event->getCurrentTarget()->getBoundingBox().containsPoint(touch->getLocation()))
		{
			AudioManagement::getInstance()->play("sound/startBattle.mp3", AudioManagement::AudioType::Sound, false);
			SoldierAI::getInstance()->_structAICommand.StartBattleCommand = true;
			event->getCurrentTarget()->removeFromParent();
			Director::getInstance()->getEventDispatcher()->removeEventListenersForTarget(this);
		}
		return true;
	};
	Director::getInstance()->getEventDispatcher()->addEventListenerWithSceneGraphPriority(startBattleSpriteListener, startBattleSprite);

	auto selectedAll = Sprite::create("ui/standPoint.png");
	selectedAll->setPosition(visible.width / 20*1.3 + origin.x, visible.height / 10 * 7 + origin.y);
	auto unselectedAll = Sprite::create("ui/standPoint.png");
	unselectedAll->setPosition(visible.width / 20*4 + origin.x, visible.height / 10 * 7 + origin.y);
	selectedAll->setScale(0.8f);
	unselectedAll->setScale(0.8f);
	this->addChild(selectedAll);
	this->addChild(unselectedAll);

	auto selectedAllLabel = Label::create();
	selectedAllLabel->setString("selected");
	selectedAllLabel->setColor(Color3B::BLACK);
	selectedAllLabel->setPosition(selectedAll->getPosition());
	auto unselectedAllLabel = Label::create();
	unselectedAllLabel->setString("unselected");
	unselectedAllLabel->setColor(Color3B::BLACK);
	unselectedAllLabel->setPosition(unselectedAll->getPosition());
	this->addChild(selectedAllLabel);
	this->addChild(unselectedAllLabel);

	auto selectedAllListener = EventListenerTouchOneByOne::create();
	selectedAllListener->onTouchBegan = [](Touch* touch, Event* event)
	{
		if (event->getCurrentTarget()->getBoundingBox().containsPoint(touch->getLocation()))
		{
			AudioManagement::getInstance()->play("sound/allSelected.mp3", AudioManagement::AudioType::Sound, false);
			auto ourLegions = SoldierAI::getInstance()->_ourLegions;
			for (auto legion : ourLegions)
			{
				legion->_isSelected = true;
				legion->showSoldierStandSprite();
			}
			return true;
		}
		return false;
	};
	selectedAllListener->setSwallowTouches(true);
	Director::getInstance()->getEventDispatcher()->addEventListenerWithSceneGraphPriority(selectedAllListener, selectedAll);
	//Director::getInstance()->getEventDispatcher()->addEventListenerWithFixedPriority(selectedAllListener, 2);
	auto unselectedAllListener = EventListenerTouchOneByOne::create();
	unselectedAllListener->onTouchBegan = [](Touch* touch, Event* event)
	{
		if (event->getCurrentTarget()->getBoundingBox().containsPoint(touch->getLocation()))
		{
			AudioManagement::getInstance()->play("sound/allunSelected.mp3", AudioManagement::AudioType::Sound, false);
			auto ourLegions = SoldierAI::getInstance()->_ourLegions;
			for (auto legion : ourLegions)
			{
				legion->_isSelected = false;
				legion->hideSoldierStandSprite();
			}
		}
		return true;
	};
	Director::getInstance()->getEventDispatcher()->addEventListenerWithSceneGraphPriority(unselectedAllListener, unselectedAll);

	return true;
}
void BattleHUDlayer::onEnter()
{
	Layer::onEnter();
}

void BattleHUDlayer::ShakeBtn(int _enum)
{
	ActionInterval* sequence;
	Sprite* SwingSwordBtn = static_cast<Sprite*>(_firstLayer->getChildByTag(_enum));
	if (_enum == SHOOT)
	{
		//shoot btn bigger,than __CONTROL_BUTTON_SCALE need bigger
		sequence = Sequence::create(ScaleTo::create(__CONTROL_BUTTON_SHAKE_TIME_DELAY, (__CONTROL_BUTTON_SCALE+ controlBtnBiggerDelta)*1.3, (__CONTROL_BUTTON_SCALE+ controlBtnBiggerDelta)*1.3),
			ScaleTo::create(__CONTROL_BUTTON_SHAKE_TIME_DELAY, __CONTROL_BUTTON_SCALE+ controlBtnBiggerDelta, __CONTROL_BUTTON_SCALE+ controlBtnBiggerDelta), NULL);
	}
	else
	{
		sequence = Sequence::create(ScaleTo::create(__CONTROL_BUTTON_SHAKE_TIME_DELAY, __CONTROL_BUTTON_SCALE*1.3, __CONTROL_BUTTON_SCALE*1.3),
			ScaleTo::create(__CONTROL_BUTTON_SHAKE_TIME_DELAY, __CONTROL_BUTTON_SCALE , __CONTROL_BUTTON_SCALE ), NULL);
	}
	SwingSwordBtn->runAction(sequence);
}
void BattleHUDlayer::ShootCallBack()
{
	BattleScene::getBattleScene()->getBattleScene()->_continuousClickCount=0;
	SoldierAI::getInstance()->updateMusketSoldierShoot();
	AudioManagement::getInstance()->play("sound/Fire.mp3", AudioManagement::AudioType::Sound, false);
}

void BattleHUDlayer::ReadyToShootCallBack()
{
	BattleScene::getBattleScene()->getBattleScene()->_continuousClickCount = 0;
	SoldierAI::getInstance()->updateMusketSoldierReadyToShootState();
	AudioManagement::getInstance()->play("sound/ready.mp3", AudioManagement::AudioType::Sound, false);
}

void BattleHUDlayer::ClickSwitchBtnCallBack(float dt)
{
	const int const_MaxSwitchLayerTimeCount = 10;
	if (this->_isFirstLayerDowing)
	{
		if (_SwitchLayerTimeCount!=const_MaxSwitchLayerTimeCount)
		{
			//第一圖層控鍵 往下縮
			this->_firstLayer->setPositionY(this->_firstLayer->getPositionY() - ((visible.height+origin.x)*(7.f / 270)));
			//第二圖層控鍵 往右顯示
			this->_secondLayer->setPositionX(this->_secondLayer->getPositionX() + ((visible.width+origin.x)*(50.f/480)));
			//batlle scroll showing
			auto battleScrollBar =(Scale9Sprite*)this->getChildByTag(BATTLE_SCROLL_BAR);
			battleScrollBar->setContentSize(Size(battleScrollBar->getContentSize().width + ((visible.width + origin.x)*(320.f/480)),
												battleScrollBar->getContentSize().height));
			_SwitchLayerTimeCount++;
		}
		else
		{
			this->unschedule(schedule_selector(BattleHUDlayer::ClickSwitchBtnCallBack));
		}
	}
	else
	{
		if (_SwitchLayerTimeCount != 0)
		{
			//第一圖層控鍵 往上顯示
			this->_firstLayer->setPositionY(this->_firstLayer->getPositionY() + ((visible.height+origin.x)*(7.f / 270)));
			//第二圖層控鍵 往左縮
			this->_secondLayer->setPositionX(this->_secondLayer->getPositionX() - ((visible.width+origin.x)*(50.f / 480)));
			auto battleScrollBar = (Scale9Sprite*)this->getChildByTag(BATTLE_SCROLL_BAR);
			//batlle scroll not showing
			battleScrollBar->setContentSize(Size(battleScrollBar->getContentSize().width - ((visible.width+origin.x)*(320.f / 480)),
				battleScrollBar->getContentSize().height));
			_SwitchLayerTimeCount--;
		}
		else
		{
			this->unschedule(schedule_selector(BattleHUDlayer::ClickSwitchBtnCallBack));
		}
	}
}
	

NS_BATTLEMODE_END