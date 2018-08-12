#include "ui\ScrollMode\HUDlayer.h"
#include "MainCharacter\MainCharacter.h"
#include "ui\ScrollMode\Dialog.h"
#include "ui\MyButton.h"
#include "Scene\SettingMenu.h"
#include "Scene\ItemScene.h"
#include "Scene\ScrollMode\ScrollSceneFactory.h"
#include "Scene\MissionScene.h"
#include "tools\AudioManagement.h"
#include "tools\JsonParser.h"
#include "RPGTools\SkillMG.h"
#include "RPGTools\RPGTool.h"
#include "TipsLabel.h"
USING_NS_CC;
using namespace cocos2d::ui;
using namespace cocos2d::extension;
using namespace cocos2d::experimental;
NS_SCROLLMODE_BEGAN
using namespace MyTools;
HUDlayer* HUDlayer::static_hudlayer = nullptr;
HUDlayer::HUDlayer()
{
	static_hudlayer = nullptr;
	this->_isReplaceingScene = false;
	this->_isTouchMove = false;
}
HUDlayer::~HUDlayer()
{
	static_hudlayer = nullptr;
}
HUDlayer * HUDlayer::getHUDlayer()
{
	if (static_hudlayer == nullptr)
		return nullptr;
	else
		return static_hudlayer;
}
void HUDlayer::onExit()
{
	Layer::onExit();
	NotificationCenter::getInstance()->removeAllObservers(this);
}
HUDlayer* HUDlayer::create(MainCharacter* mainCharacter)
{
	HUDlayer* hudlayer = new(std::nothrow)HUDlayer;
	if (hudlayer && hudlayer->init(mainCharacter))
	{
		hudlayer->autorelease();
	}
	else
	{
		CC_SAFE_DELETE(hudlayer);
	}
	return hudlayer;
}
bool HUDlayer::init(MainCharacter* mainCharacter)
{
	if (!Layer::init())
	{
		return false;
	}
	static_hudlayer = this;
	visible = Director::getInstance()->getVisibleSize();
	origin = Director::getInstance()->getVisibleOrigin();
	//character
	this->_mainCharacter = mainCharacter;
	//this->setAnchorPoint(Vec2(0, 0));
	_sceneCSB = CSLoader::createNode("HUDLayer/HUDLayer.csb");
	this->addChild(_sceneCSB, 10);
	//level label
	_levelLabel = Label::createWithTTF(JsonParser::getJsonString("Infor", "Level", JsonParser::JsonType::Save),"fonts/arial.ttf",14);
	_levelLabel->setColor(Color3B::YELLOW);
	_levelLabel->setPosition(this->_sceneCSB->getChildByName("LevelLabelPos")->getPosition());
	_levelLabel->enableBold();
	this->addChild(_levelLabel, 11);
	//hp mp exp bar初始值設定
	this->setHPBarPercent(atoi(JsonParser::getJsonString("Infor","HP",JsonParser::JsonType::Save).c_str()));
	this->setMPBarPercent(atoi(JsonParser::getJsonString("Infor", "MP", JsonParser::JsonType::Save).c_str()));
	this->setEXPBarPercent(atoi(JsonParser::getJsonString("Infor", "EXP", JsonParser::JsonType::Save).c_str()));
	//hp mp exp bar上面的數字label
	char hpDetailStr[30];
	char mpDetailStr[30];
	char expDetailStr[30];
	sprintf(hpDetailStr, "%d / %d", atoi(JsonParser::getJsonString("Infor", "HP", JsonParser::JsonType::Save).c_str()),
		(int)this->_mainCharacter->struct_infor._maxhp);
	_HPDetailLabel = Label::create(hpDetailStr,"fonts/arial.ttf",13);
	_HPDetailLabel->setPosition(_sceneCSB->getChildByName("HPBarDetail")->getPosition());
	_HPDetailLabel->setColor(Color3B::WHITE);

	this->addChild(_HPDetailLabel, 11);
	sprintf(mpDetailStr, "%d / %d", atoi(JsonParser::getJsonString("Infor", "MP", JsonParser::JsonType::Save).c_str()),
		(int)this->_mainCharacter->struct_infor._maxmp);
	_MPDetailLabel = Label::create(mpDetailStr, "fonts/arial.ttf", 13);
	_MPDetailLabel->setPosition(_sceneCSB->getChildByName("MPBarDetail")->getPosition());
	_MPDetailLabel->setColor(Color3B::WHITE);
	this->addChild(_MPDetailLabel, 11);

	sprintf(expDetailStr, "%d / %d", atoi(JsonParser::getJsonString("Infor", "EXP", JsonParser::JsonType::Save).c_str()),
		(int)this->_mainCharacter->struct_infor._maxexp);
	_EXPDetailLabel = Label::create(expDetailStr, "fonts/arial.ttf", 12);
	_EXPDetailLabel->setPosition(_sceneCSB->getChildByName("EXPBarDetail")->getPosition());
	_EXPDetailLabel->setColor(Color3B::WHITE);
	this->addChild(_EXPDetailLabel, 11);
	//hp label文字
	auto hpLabel = Label::create(JsonParser::getJsonString("HUDLayer","HP",JsonParser::JsonType::UserInterFace),
		JsonParser::fontType(), JsonParser::fontSize(16));
	hpLabel->setAnchorPoint(Vec2(0, 0.5));
	hpLabel->setColor(Color3B::WHITE);
	hpLabel->enableGlow(Color4B::WHITE);
	hpLabel->setPosition(_sceneCSB->getChildByName("hpLabelPos")->getPosition());
	hpLabel->setName("hpLabel");
	this->addChild(hpLabel,11);
	//mp label文字
	auto mpLabel = Label::create(JsonParser::getJsonString("HUDLayer", "MP", JsonParser::JsonType::UserInterFace),
		JsonParser::fontType(), JsonParser::fontSize(16));
	mpLabel->setAnchorPoint(Vec2(0, 0.5));
	mpLabel->setColor(Color3B::WHITE);
	mpLabel->enableGlow(Color4B::WHITE);
	mpLabel->setPosition(_sceneCSB->getChildByName("mpLabelPos")->getPosition());
	mpLabel->setName("mpLabel");
	this->addChild(mpLabel, 11);
	//mp label文字
	auto expLabel = Label::create(JsonParser::getJsonString("HUDLayer", "EXP", JsonParser::JsonType::UserInterFace),
		JsonParser::fontType(), JsonParser::fontSize(16));
	expLabel->setAnchorPoint(Vec2(0, 0.5));
	expLabel->setColor(Color3B::WHITE);
	expLabel->enableGlow(Color4B::WHITE);
	expLabel->setPosition(_sceneCSB->getChildByName("expLabelPos")->getPosition());
	expLabel->setName("expLabel");
	this->addChild(expLabel, 11);
	//設定圖示
	auto settingEntryBtn = ShakeButton::create(" ", "fonts/arial.ttf", 12, Color4B::BLACK, "ui/settingEntrySprite.png",
		[this](ShakeButton* btn)
	{
		if (!_mainCharacter->status._isDying)
		{
			if (this->_isReplaceingScene == true)
				return;
			this->_isReplaceingScene = true;
			AudioManagement::getInstance()->pauseAllAudio();
			auto capture = RenderTexture::create(visible.width + origin.x, visible.height + origin.y);
			capture->begin();
			this->getParent()->visit();
			capture->end();
			Director::getInstance()->pushScene(TransitionPageTurn::create(0.4f, SettingMenu::createScene(capture), true));
		}
	});
	settingEntryBtn->setOpacity(180);
	settingEntryBtn->setScale(0.3f);
	settingEntryBtn->setPosition(_sceneCSB->getChildByName("SettingPoint")->getPosition());
	this->addChild(settingEntryBtn);

	//背包圖示
	auto itemEntryBtn = ShakeButton::create(" ", "fonts/arial.ttf", 12, Color4B::BLACK, "ui/itemEntrySprite.png",
		[this](ShakeButton* btn)
	{
		if (!_mainCharacter->status._isDying)
		{
			if (this->_isReplaceingScene == true)
				return;
			this->_isReplaceingScene = true;
			AudioManagement::getInstance()->play("sound/ui_select2.mp3", AudioManagement::AudioType::Sound, false);
			auto capture = RenderTexture::create(visible.width + origin.x, visible.height + origin.y);
			capture->begin();
			this->getParent()->visit();
			capture->end();
			Director::getInstance()->pushScene(TransitionFadeBL::create(0.4f, ItemScene::createScene(capture)));
		}
	});
	itemEntryBtn->setOpacity(200);
	itemEntryBtn->setScale(0.3f);
	itemEntryBtn->setPosition(_sceneCSB->getChildByName("ItemPoint")->getPosition());
	this->addChild(itemEntryBtn);
	//背包提示
	_itemTipBall = Sprite::create("ui/tipSquar.png");
	_itemTipBall->setPosition(this->_sceneCSB->getChildByName("ItemTipBallPos")->getPosition());
	_itemTipBall->setOpacity(240);
	_itemTipBall->setScale(0.3f);
	if (JsonParser::getJsonString("Infor", "ItemTip", JsonParser::JsonType::Save) == std::string("0"))
		_itemTipBall->setVisible(false);
	this->addChild(_itemTipBall);

	//任務圖示
	auto missionEntryBtn = ShakeButton::create(" ", "fonts/arial.ttf", 12, Color4B::BLACK, "ui/missionEntrySprite.png",
		[this](ShakeButton* btn)
	{
		if (!_mainCharacter->status._isDying)
		{
			if (this->_isReplaceingScene == true)
				return;
			this->_isReplaceingScene = true;
			AudioManagement::getInstance()->play("sound/btn_page.mp3", AudioManagement::AudioType::Sound, false);
			auto capture = RenderTexture::create(visible.width + origin.x, visible.height + origin.y);
			capture->begin();
			this->getParent()->visit();
			capture->end();
			Director::getInstance()->pushScene(TransitionPageTurn::create(0.4f, MissionScene::createScene(capture), true));
		}
	});
	missionEntryBtn->setOpacity(220);
	missionEntryBtn->setScale(0.3f);
	missionEntryBtn->setPosition(_sceneCSB->getChildByName("MissionPoint")->getPosition());
	this->addChild(missionEntryBtn);
	//任務提示
	_missionTipBall = Sprite::create("ui/tipSquar.png");
	_missionTipBall->setPosition(this->_sceneCSB->getChildByName("MissionTipBallPos")->getPosition());
	_missionTipBall->setOpacity(240);
	_missionTipBall->setScale(0.3f);
	if (JsonParser::getJsonString("Infor", "MissionTip", JsonParser::JsonType::Save) == std::string("0"))
		_missionTipBall->setVisible(false);
	this->addChild(_missionTipBall);

	//move btn
	_leftMoveUnClick = (Sprite*)this->_sceneCSB->getChildByName("LeftMoveUnClick");
	_rightMoveUnClick = (Sprite*)this->_sceneCSB->getChildByName("RightMoveUnClick");
	_leftMoveUnClick->setOpacity(100);
	_rightMoveUnClick->setOpacity(100);
	_leftMoveUnClick->setTag(HUDlayer::LeftMove);
	_rightMoveUnClick->setTag(HUDlayer::RightMove);
	_moveBtns.push_back(_leftMoveUnClick);
	_moveBtns.push_back(_rightMoveUnClick);

	_leftMoveClick = (Sprite*)this->_sceneCSB->getChildByName("LeftMoveClick");
	_rightMoveClick = (Sprite*)this->_sceneCSB->getChildByName("RightMoveClick");
	_leftMoveClick->setOpacity(150);
	_rightMoveClick->setOpacity(150);
	_leftMoveClick->setVisible(false);
	_rightMoveClick->setVisible(false);
	
	//Skill btn
	_normalBtn = (Sprite*)this->_sceneCSB->getChildByName("normal_Btn");
	_jumpBtn = (Sprite*)this->_sceneCSB->getChildByName("Jump_Btn");
	_skill1Btn = (Sprite*)this->_sceneCSB->getChildByName("Skill_1");
	_skill2Btn = (Sprite*)this->_sceneCSB->getChildByName("Skill_2");
	_skill3Btn = (Sprite*)this->_sceneCSB->getChildByName("Skill_3");
	_HPPotionBtn= (Sprite*)this->_sceneCSB->getChildByName("HPPotionPos");
	_MPPotionBtn = (Sprite*)this->_sceneCSB->getChildByName("MPPotionPos");
	_normalBtn->setTag(HUDlayer::NormalAttack);
	_jumpBtn->setTag(HUDlayer::JUMP);
	_skill1Btn->setTag(HUDlayer::SKILL1);
	_skill2Btn->setTag(HUDlayer::SKILL2);
	_skill3Btn->setTag(HUDlayer::SKILL3);
	_HPPotionBtn->setTag(HUDlayer::HPPotion);
	_MPPotionBtn->setTag(HUDlayer::MPPotion);
	_normalBtn->setOpacity(150);
	_jumpBtn->setOpacity(150);
	_skill1Btn->setOpacity(150);
	_skill2Btn->setOpacity(150);
	_skill3Btn->setOpacity(150);
	_HPPotionBtn->setOpacity(150);
	_MPPotionBtn->setOpacity(150);
	_skillBtns.push_back(_normalBtn);
	_skillBtns.push_back(_jumpBtn);
	_skillBtns.push_back(_skill1Btn);
	_skillBtns.push_back(_skill2Btn);
	_skillBtns.push_back(_skill3Btn);
	_skillBtns.push_back(_HPPotionBtn);
	_skillBtns.push_back(_MPPotionBtn);
	//在技能按鈕上的圖片
	_jumpBG = Sprite::create("ui/SkillBG/Jump.png");
	_normalBG = Sprite::create(SkillMG::getInstance()->getCurrentSkillPath(SkillMG::SkillPart::Normal));
	_skill1BG = Sprite::create(SkillMG::getInstance()->getCurrentSkillPath(SkillMG::SkillPart::Skill1));
	_skill2BG = Sprite::create(SkillMG::getInstance()->getCurrentSkillPath(SkillMG::SkillPart::Skill2));
	_skill3BG = Sprite::create(SkillMG::getInstance()->getCurrentSkillPath(SkillMG::SkillPart::Skill3));
	
	_normalBG->setPosition(_normalBtn->getBoundingBox().size.width-visible.width/96,
		_normalBtn->getBoundingBox().size.height - visible.height / 54);//visible.width=480  480/96=5 這樣就可以根據螢幕大小做偏移量
	_jumpBG->setPosition(_normalBtn->getBoundingBox().size.width - visible.width / 96,
		_normalBtn->getBoundingBox().size.height - visible.height / 54);
	_skill1BG->setPosition(_normalBtn->getBoundingBox().size.width - visible.width / 96,
		_normalBtn->getBoundingBox().size.height - visible.height / 54);
	_skill2BG->setPosition(_normalBtn->getBoundingBox().size.width - visible.width / 96,
		_normalBtn->getBoundingBox().size.height - visible.height / 54);
	_skill3BG->setPosition(_normalBtn->getBoundingBox().size.width - visible.width / 96,
		_normalBtn->getBoundingBox().size.height - visible.height / 54);
	_normalBtn->addChild(_normalBG);
	_jumpBtn->addChild(_jumpBG);
	_skill1Btn->addChild(_skill1BG);
	_skill2Btn->addChild(_skill2BG);
	_skill3Btn->addChild(_skill3BG);
	//更新藥水按鈕
	this->refreshPotionBG();

	//移動事件
	auto multiTouchListener = EventListenerTouchAllAtOnce::create();
	multiTouchListener->onTouchesBegan = [&](std::vector<Touch*> touchs, Event* event)
	{
		Vec2 point1 = touchs[0]->getLocation();
		Vec2 point2;
		if (touchs.size()>1)
			point2 = touchs[1]->getLocation();
		for (auto& move : _moveBtns)
		{
			if (!_mainCharacter->status._isDying)
			{
				if (move->getBoundingBox().containsPoint(point1) || move->getBoundingBox().containsPoint(point2))
				{
					MoveListenerSendNotifiToCharacter(move);
					_isTouchMove = true;
				}
			}
		}
		//釋放技能攻擊事件
		for (auto& skill : _skillBtns)
		{
			if (skill->getBoundingBox().containsPoint(point1) || skill->getBoundingBox().containsPoint(point2))
			{
				if (skill->getTag() == HUDlayer::JUMP && !_mainCharacter->status._isDying)
				{
					ShakeBtn(_jumpBtn,BtnType::SkillBtn);
					JumpCallBack();
				}
				else
				{
					if (skill->getTag() == HUDlayer::NormalAttack && !_mainCharacter->status._isDying)
					{
						ShakeBtn(_normalBtn, BtnType::NormalBtn);
						if (this->_intNormalState == 0)
						{//如果攻擊扭變成傳送狀態時  傳送到下一張地圖
							if (this->_isReplaceingScene == true)
								return;
							this->_isReplaceingScene = true;
							ScrollSceneFactory::static_prevCharacterPos = this->_transferPoint->getPosition();
							AudioManagement::getInstance()->play("sound/transferSound.mp3",AudioManagement::AudioType::Sound,false);
							Director::getInstance()->replaceScene(TransitionFade::create(1.0f, this->_transferPoint->getNextScene()));
						}
						else if (this->_intNormalState == 1)
						{//如果攻擊扭變成說話狀態時
							if (this->_isReplaceingScene == true)
								return;
							this->_isReplaceingScene = true;
							auto capture = RenderTexture::create(visible.width + origin.x, visible.height + origin.y);
							capture->begin();
							this->getParent()->visit();
							capture->end();
							Director::getInstance()->pushScene(TransitionProgressRadialCW::create(0.3f, Dialog::createScene(capture, _closerNPC,"")));
						}
						else
						{//正常攻擊
							NormalAttackCallBack();
						}
					}
					else if (skill->getTag() == HUDlayer::SKILL1 && !_mainCharacter->status._isDying)
					{
						ShakeBtn(_skill1Btn, BtnType::SkillBtn);
						Skill1CallBack();
					}
					else if (skill->getTag() == HUDlayer::SKILL2 && !_mainCharacter->status._isDying)
					{
						ShakeBtn(_skill2Btn, BtnType::SkillBtn);
						Skill2CallBack();
					}
					else if (skill->getTag() == HUDlayer::SKILL3 && !_mainCharacter->status._isDying)
					{
						ShakeBtn(_skill3Btn, BtnType::SkillBtn);
						Skill3CallBack();
					}
					else if (skill->getTag() == HUDlayer::HPPotion && !_mainCharacter->status._isDying)
					{
						ShakeBtn(_HPPotionBtn, BtnType::PotionBtn);
						hpPotionCallBack();
					}
					else if (skill->getTag() == HUDlayer::MPPotion && !_mainCharacter->status._isDying)
					{
						ShakeBtn(_MPPotionBtn, BtnType::PotionBtn);
						mpPotionCallBack();
					}
				}
			}
		}
	};
	multiTouchListener->onTouchesMoved = [this](std::vector<Touch*> touchs, Event* event)
	{
		Vec2 point1 = touchs[0]->getLocation();
		Vec2 point2;
		if (touchs.size()>1)
			point2 = touchs[1]->getLocation();
		for (auto& move : _moveBtns)
		{
			if (!_mainCharacter->status._isDying)
			{
				if (move->getBoundingBox().containsPoint(point1) || move->getBoundingBox().containsPoint(point2))
				{
					MoveListenerSendNotifiToCharacter(move);
					_isTouchMove = true;
				}
			}
		}
		
	};
	multiTouchListener->onTouchesEnded = [this](std::vector<Touch*> touchs, Event* event)
	{
		Vec2 point1 = touchs[0]->getLocation();
		Vec2 point2;
		if (touchs.size()>1)
			point2 = touchs[1]->getLocation();
		for (auto& move : _moveBtns)
		{
			if (!_mainCharacter->status._isDying)
			{
				if (move->getBoundingBox().containsPoint(point1) || move->getBoundingBox().containsPoint(point2))
				{//如果在移動按鈕上放開觸摸，則發送停止移動消息給主角
					MoveListenerSendNotifiToCharacter(move);
					_isTouchMove = false;
				}
				else
				{//如果在移動按鈕以外的地方放開觸摸，則讓主角自動走路
					if (_mainCharacter->status._isAttack)
					{//如果主角是攻擊狀態，則不觸發自動走路功能
						_isTouchMove = false;
					}
				}
			}
		}
		if (_isTouchMove == false)
		{
			NotificationCenter::getInstance()->postNotification("StopRun");
			_rightMoveClick->setVisible(false);
			_leftMoveClick->setVisible(false);
		}
	};
	Director::getInstance()->getEventDispatcher()->addEventListenerWithSceneGraphPriority(multiTouchListener, this);



	this->schedule([this](float dt)
	{
		if (_normalSkillState.Transfer == true)
		{
			this->SwitchTransferBtn();
			this->_intNormalState = 0;
		}
		else if (_normalSkillState.Talk == true)
		{
			this->SwitchTalkBtn();
			this->_intNormalState = 1;
		}
		else
		{
			this->SwitchAttackBtn();
			this->_intNormalState = 2;
		}
	}, 0.1f, "HUDUpdate");

	return true;
}
void HUDlayer::setHPBarPercent(float hp)
{
	cocos2d::ui::LoadingBar* hpProgress = static_cast<cocos2d::ui::LoadingBar*>(_sceneCSB->getChildByName("hpBar"));
	if (hp > this->_mainCharacter->struct_infor._maxhp)
	{
		hp = this->_mainCharacter->struct_infor._maxhp;
	}
	float newhp = (hp / this->_mainCharacter->struct_infor._maxhp)*100.0f;
	hpProgress->setPercent(newhp);
}
float HUDlayer::getHPBarPercent()
{
	cocos2d::ui::LoadingBar* hpProgress = static_cast<cocos2d::ui::LoadingBar*>(_sceneCSB->getChildByName("hpBar"));
	return hpProgress->getPercent();
}
void HUDlayer::setMPBarPercent(float mp)
{
	cocos2d::ui::LoadingBar* mpProgress = static_cast<cocos2d::ui::LoadingBar*>(_sceneCSB->getChildByName("mpBar"));
	if (mp > this->_mainCharacter->struct_infor._maxmp)
	{
		mp = this->_mainCharacter->struct_infor._maxmp;
		char mpStr[10];
		sprintf(mpStr, "%d", (int)mp);
		JsonParser::setSave("Infor", "MP", mpStr);
	}
	float newmp = (mp / this->_mainCharacter->struct_infor._maxmp)*100.0f;
	mpProgress->setPercent(newmp);
}
float HUDlayer::getMPBarPercent()
{
	cocos2d::ui::LoadingBar* mpProgress = static_cast<cocos2d::ui::LoadingBar*>(_sceneCSB->getChildByName("mpBar"));
	return mpProgress->getPercent();
}
void HUDlayer::setEXPBarPercent(float exp)
{
	cocos2d::ui::LoadingBar* expProgress = static_cast<cocos2d::ui::LoadingBar*>(_sceneCSB->getChildByName("expBar"));
	if (exp > this->_mainCharacter->struct_infor._maxexp)
	{
		exp = this->_mainCharacter->struct_infor._maxexp;
	}
	float newexp = (exp / this->_mainCharacter->struct_infor._maxexp)*100.0f;
	expProgress->setPercent(newexp);
}
float HUDlayer::getEXPBarPercent()
{
	cocos2d::ui::LoadingBar* expProgress = static_cast<cocos2d::ui::LoadingBar*>(_sceneCSB->getChildByName("expBar"));
	return expProgress->getPercent();
}
void HUDlayer::SwitchTalkBtn()
{
	Sprite* newSprite = Sprite::create("ui/SkillBG/Talk.png");
	_normalBG->setTexture(newSprite->getTexture());
}
void HUDlayer::SwitchAttackBtn()
{
	_normalBG->setTexture(Sprite::create(SkillMG::getInstance()->getCurrentSkillPath(SkillMG::SkillPart::Normal))->getTexture());
}
void HUDlayer::SwitchTransferBtn()
{
	Sprite* newSprite = Sprite::create("ui/SkillBG/Transfer.png");
	_normalBG->setTexture(newSprite->getTexture());
}
void HUDlayer::ShakeBtn(cocos2d::Node* wantToShakeNode, BtnType btnType)
{
	ActionInterval* sequence;
	float Scale;
	switch (btnType)
	{
	case BtnType::NormalBtn:
		Scale = const_NormalBtn;
		break;
	case BtnType::SkillBtn:
		Scale = const_SkillBtn;
		break;
	case BtnType::PotionBtn:
		Scale = const_PotionBtn;
		break;
	default:
		break;
	}
	sequence = Sequence::create(ScaleTo::create(0.05f, Scale + 0.2f, Scale + 0.2f),
		ScaleTo::create(0.05f, Scale, Scale), NULL);
	wantToShakeNode->runAction(sequence);
}
void HUDlayer::NormalAttackCallBack()
{
	NotificationCenter::getInstance()->postNotification("NormalAttack", (Ref*)"NormalAttack");
}
void HUDlayer::Skill1CallBack()
{
	NotificationCenter::getInstance()->postNotification("Skill1", (Ref*)"Skill1");
}
void HUDlayer::Skill2CallBack()
{
	NotificationCenter::getInstance()->postNotification("Skill2", (Ref*)"Skill2");
}
void HUDlayer::Skill3CallBack()
{
	NotificationCenter::getInstance()->postNotification("Skill3", (Ref*)"Skill3");
}
void HUDlayer::JumpCallBack()
{
	if (_mainCharacter->_currentJumpCount == _mainCharacter->getMaxJumpCount() || _mainCharacter->status._isAttack)
		return;
	NotificationCenter::getInstance()->postNotification("Jump", (Ref*)"Jump");
}
void HUDlayer::hpPotionCallBack()
{
	if (_mainCharacter->struct_infor._hp == _mainCharacter->struct_infor._maxhp)
	{//當前血量滿血直接return
		return;
	}
	int HPBarPotionID = JsonParser::getEquipmentFromSave((int)RPGTool::CharacterPart::HPBar, JsonParser::Equipment::GetID);
	int HPBarPotionValue = JsonParser::getEquipmentFromSave((int)RPGTool::CharacterPart::HPBar, JsonParser::Equipment::GetValue);
	if (HPBarPotionValue > 0)
	{//當前藥水數量大於0時
		AudioManagement::getInstance()->play("sound/ui_select2.mp3", AudioManagement::AudioType::Sound, false);
		_mainCharacter->struct_infor._hp = _mainCharacter->struct_infor._hp + RPGTool::accordingItemIDToData(HPBarPotionID).RecoverHP;
		if (_mainCharacter->struct_infor._hp > _mainCharacter->struct_infor._maxhp)
		{//如果補血後的血量超過上限，那血量就等於血量上限
			_mainCharacter->struct_infor._hp = _mainCharacter->struct_infor._maxhp;
		}
		char hpStr[10];
		sprintf(hpStr, "%d", (int)_mainCharacter->struct_infor._hp);
		JsonParser::setSave("Infor", "HP", hpStr);
		if (--HPBarPotionValue == 0)
		{//如果藥水量為0時，設為-1，-1代表物品不存在
			HPBarPotionValue = -1;
		}
		JsonParser::setEquipmentFromSave(HPBarPotionID, HPBarPotionValue);
		refreshHUDlayer();
	}
}
void HUDlayer::mpPotionCallBack()
{
	if (_mainCharacter->struct_infor._mp == _mainCharacter->struct_infor._maxmp)
	{//當前血量滿血直接return
		return;
	}
	int MPBarPotionID = JsonParser::getEquipmentFromSave((int)RPGTool::CharacterPart::MPBar, JsonParser::Equipment::GetID);
	int MPBarPotionValue = JsonParser::getEquipmentFromSave((int)RPGTool::CharacterPart::MPBar, JsonParser::Equipment::GetValue);
	if (MPBarPotionValue > 0)
	{//當前藥水數量大於0時
		AudioManagement::getInstance()->play("sound/ui_select2.mp3", AudioManagement::AudioType::Sound, false);
		_mainCharacter->struct_infor._mp = _mainCharacter->struct_infor._mp + RPGTool::accordingItemIDToData(MPBarPotionID).RecoverMP;
		if (_mainCharacter->struct_infor._mp > _mainCharacter->struct_infor._maxmp)
		{//如果補血後的血量超過上限，那血量就等於血量上限
			_mainCharacter->struct_infor._mp = _mainCharacter->struct_infor._maxmp;
		}
		char mpStr[10];
		sprintf(mpStr, "%d", (int)_mainCharacter->struct_infor._mp);
		JsonParser::setSave("Infor", "MP", mpStr);
		if (--MPBarPotionValue == 0)
		{//如果藥水量為0時，設為-1，-1代表物品不存在
			MPBarPotionValue = -1;
		}
		JsonParser::setEquipmentFromSave(MPBarPotionID, MPBarPotionValue);
		refreshHUDlayer();
	}
}
void HUDlayer::MoveListenerSendNotifiToCharacter(Sprite* moveSprite)
{
	if (this->_mainCharacter->status._isHurt)
		return;
	if (moveSprite->getTag() == HUDlayer::RightMove)
	{
		_leftMoveClick->setVisible(false);
		_rightMoveClick->setVisible(true);
		if (!this->_mainCharacter->status._isAttack)
			this->_mainCharacter->reviseDirection(false);//角色面向右邊
	}
	if (moveSprite->getTag() == HUDlayer::LeftMove)
	{
		_rightMoveClick->setVisible(false);
		_leftMoveClick->setVisible(true);
		if (!this->_mainCharacter->status._isAttack)
			this->_mainCharacter->reviseDirection(true);//角色面向右邊
	}
	NotificationCenter::getInstance()->postNotification("Run",(Ref*)moveSprite);
}
void HUDlayer::updateLayerWithScenePos(float x, float y)
{
	this->setPosition(-x,-y);
}
void HUDlayer::refreshHUDlayer()
{
	this->refreshLevelLabel();
	this->refreshPotionBG();
	this->refreshBarNumberLabel();
	this->setHPBarPercent(atoi(JsonParser::getJsonString("Infor", "HP", JsonParser::JsonType::Save).c_str()));
	this->setMPBarPercent(atoi(JsonParser::getJsonString("Infor", "MP", JsonParser::JsonType::Save).c_str()));
	this->setEXPBarPercent(atoi(JsonParser::getJsonString("Infor", "EXP", JsonParser::JsonType::Save).c_str()));
}
void HUDlayer::refreshMPBar()
{
	this->setMPBarPercent(atoi(JsonParser::getJsonString("Infor", "MP", JsonParser::JsonType::Save).c_str()));
	this->refreshBarNumberLabel();
}
void HUDlayer::refreshPotionBG()
{
	_HPPotionBtn->removeAllChildren();
	_MPPotionBtn->removeAllChildren();
	//藥水瓶按鈕
	int HPBarPotionID = JsonParser::getEquipmentFromSave((int)RPGTool::CharacterPart::HPBar, JsonParser::Equipment::GetID);
	int HPBarPotionValue = JsonParser::getEquipmentFromSave((int)RPGTool::CharacterPart::HPBar, JsonParser::Equipment::GetValue);
	int MPBarPotionID = JsonParser::getEquipmentFromSave((int)RPGTool::CharacterPart::MPBar, JsonParser::Equipment::GetID);
	int MPBarPotionValue = JsonParser::getEquipmentFromSave((int)RPGTool::CharacterPart::MPBar, JsonParser::Equipment::GetValue);
	if (HPBarPotionValue != -1)
	{
		//potion BG
		auto HPPotionBG = Sprite::create(RPGTool::accordingItemIDToData(HPBarPotionID).SpritePath);
		HPPotionBG->setPosition(_HPPotionBtn->getBoundingBox().size.width / 2, _HPPotionBtn->getBoundingBox().size.height / 2);
		_HPPotionBtn->addChild(HPPotionBG);
		//amount label
		char amount[10];
		sprintf(amount,"%d", HPBarPotionValue);
		auto PotionAmountLabel = Label::createWithTTF(amount, "fonts/arial.ttf",13);
		PotionAmountLabel->setColor(Color3B::WHITE);
		PotionAmountLabel->setPosition(_HPPotionBtn->getBoundingBox().size.width/2, 5);
		_HPPotionBtn->addChild(PotionAmountLabel);
	}
	if (MPBarPotionValue != -1)
	{
		//potion BG
		auto MPPotionBG = Sprite::create(RPGTool::accordingItemIDToData(MPBarPotionID).SpritePath);
		MPPotionBG->setPosition(_MPPotionBtn->getBoundingBox().size.width / 2, _MPPotionBtn->getBoundingBox().size.height / 2);
		_MPPotionBtn->addChild(MPPotionBG);
		//amount label
		char amount[10];
		sprintf(amount, "%d", MPBarPotionValue);
		auto PotionAmountLabel = Label::createWithTTF(amount, "fonts/arial.ttf", 13);
		PotionAmountLabel->setColor(Color3B::WHITE);
		PotionAmountLabel->setPosition(_MPPotionBtn->getBoundingBox().size.width / 2, 5);
		_MPPotionBtn->addChild(PotionAmountLabel);
	}
}
void HUDlayer::refreshBarNumberLabel()
{
	char hpDetailStr[30];
	char mpDetailStr[30];
	char expDetailStr[30];
	sprintf(hpDetailStr, "%d / %d", atoi(JsonParser::getJsonString("Infor", "HP", JsonParser::JsonType::Save).c_str()),
		(int)this->_mainCharacter->struct_infor._maxhp);
	sprintf(mpDetailStr, "%d / %d", atoi(JsonParser::getJsonString("Infor", "MP", JsonParser::JsonType::Save).c_str()),
		(int)this->_mainCharacter->struct_infor._maxmp);
	sprintf(expDetailStr, "%d / %d", atoi(JsonParser::getJsonString("Infor", "EXP", JsonParser::JsonType::Save).c_str()),
		(int)this->_mainCharacter->struct_infor._maxexp);
	_HPDetailLabel->setString(hpDetailStr);
	_MPDetailLabel->setString(mpDetailStr);
	_EXPDetailLabel->setString(expDetailStr);
}
void HUDlayer::refreshLevelLabel()
{
	_levelLabel->setString(JsonParser::getJsonString("Infor", "Level", JsonParser::JsonType::Save));
}
void HUDlayer::refreshSkillSprite()
{
	_normalBG->setTexture(Sprite::create(SkillMG::getInstance()->getCurrentSkillPath(SkillMG::SkillPart::Normal))->getTexture());
	_skill1BG->setTexture(Sprite::create(SkillMG::getInstance()->getCurrentSkillPath(SkillMG::SkillPart::Skill1))->getTexture());
	_skill2BG->setTexture(Sprite::create(SkillMG::getInstance()->getCurrentSkillPath(SkillMG::SkillPart::Skill2))->getTexture());
	_skill3BG->setTexture(Sprite::create(SkillMG::getInstance()->getCurrentSkillPath(SkillMG::SkillPart::Skill3))->getTexture());
}
void HUDlayer::setItemTip(bool wantShow)
{
	if (wantShow)
	{
		JsonParser::setSave("Infor", "ItemTip", "1");
		_itemTipBall->setVisible(true);
	}
	else
	{
		JsonParser::setSave("Infor", "ItemTip", "0");
		_itemTipBall->setVisible(false);
	}
}
void HUDlayer::setMissionTip(bool wantShow)
{
	if (wantShow)
	{
		JsonParser::setSave("Infor", "MissionTip", "1");
		_missionTipBall->setVisible(true);
	}
	else
	{
		JsonParser::setSave("Infor", "MissionTip", "0");
		_missionTipBall->setVisible(false);
	}
}
NS_SCROLLMODE_END