#include "CreateCharacterScene.h"
#include "cocostudio\CocoStudio.h"
#include "ui\CocosGUI.h"
#include "Tools\JsonParser.h"
#include "Tools\AudioManagement.h"
#include "RPGTools\RPGTool.h"
#include "UI\MyButton.h"
#include "MainCharacter\MainCharacter.h"
#include "Scene\SplashSceneFactory.h"
#include <string>
USING_NS_CC;
using namespace cocos2d::experimental;
using namespace MyTools;
NS_SCROLLMODE_BEGAN
void CreateCharacterScene::onExit()
{
	Layer::onExit();
	AudioManagement::getInstance()->clearAudioManagement();
}
Scene * CreateCharacterScene::createScene()
{
	auto scene = Scene::create();
	auto layer = CreateCharacterScene::create();
	scene->addChild(layer);
	return scene;
}
bool CreateCharacterScene::init()
{
	if (!Layer::init())
	{
		return false;
	}
	auto visible = Director::getInstance()->getVisibleSize();
	auto origin = Director::getInstance()->getVisibleOrigin();
	JsonParser::buildNewSave();
	_SceneCSB=CSLoader::createNode("createCharacterScene/CreateCharacterScene.csb");
	this->addChild(_SceneCSB,0);
	auto characterPos = _SceneCSB->getChildByName("characterActionNode")->getPosition();
	//dragonBones init
	_mainCharacter=ScrollMode::MainCharacter::create(characterPos.x, characterPos.y,true);
	_mainCharacter->_exCharacter->resetAllEquipMent();
	_mainCharacter->_exCharacter->setScale(0.4f);
	_mainCharacter->_exCharacter->play("Idle", -1,false);
	this->addChild(_mainCharacter);
	
	//according Save to change equipment
	auto KnifeButton= static_cast<cocos2d::ui::Button*>(_SceneCSB->getChildByName("SamuraiBtn"));
	auto SwordButton = static_cast<cocos2d::ui::Button*>(_SceneCSB->getChildByName("SwordBtn"));
	auto BoxingButton= static_cast<cocos2d::ui::Button*>(_SceneCSB->getChildByName("BoxerBtn"));
	KnifeButton->addTouchEventListener(CC_CALLBACK_2(CreateCharacterScene::weaponChoose, this, CreateCharacterScene::KNIFE_BUTTON));
	SwordButton->addTouchEventListener(CC_CALLBACK_2(CreateCharacterScene::weaponChoose,this,CreateCharacterScene::SWORD_BUTTON));
	BoxingButton->addTouchEventListener(CC_CALLBACK_2(CreateCharacterScene::weaponChoose, this, CreateCharacterScene::BOXING_BUTTON));
	//weapon info label
	cocos2d::ui::Text* KnifeLabel =(cocos2d::ui::Text*)_SceneCSB->getChildByName("WeaponLabelNode")->getChildByName("SamuraiLabel");
	cocos2d::ui::Text* SwordLabel = (cocos2d::ui::Text*)_SceneCSB->getChildByName("WeaponLabelNode")->getChildByName("SwordLabel");
	cocos2d::ui::Text* BoxingLabel = (cocos2d::ui::Text*)_SceneCSB->getChildByName("WeaponLabelNode")->getChildByName("BoxerLabel");
	SwordLabel->setVisible(true);
	KnifeLabel->setString(JsonParser::getJsonString("CreateCharacterScene","SamuraiInfo",JsonParser::JsonType::UserInterFace));
	SwordLabel->setString(JsonParser::getJsonString("CreateCharacterScene", "SwordInfo", JsonParser::JsonType::UserInterFace));
	BoxingLabel->setString(JsonParser::getJsonString("CreateCharacterScene", "BoxerInfo", JsonParser::JsonType::UserInterFace));
	KnifeLabel->setFontName(JsonParser::fontType());
	KnifeLabel->setFontSize(JsonParser::fontSize(40));
	SwordLabel->setFontName(JsonParser::fontType());
	SwordLabel->setFontSize(JsonParser::fontSize(40));
	BoxingLabel->setFontName(JsonParser::fontType());
	BoxingLabel->setFontSize(JsonParser::fontSize(40));
	//on option label.
	Node* changeClothNode = _SceneCSB->getChildByName("ChangeClothNode");
	char OtionLabelBuff[10];
	sprintf(OtionLabelBuff, "%s1", JsonParser::getJsonString("CreateCharacterScene", "HairLabel",JsonParser::JsonType::UserInterFace).c_str());
	_HairOptionLabel = Label::createWithTTF(OtionLabelBuff, JsonParser::fontType(),JsonParser::fontSize(18));
	_HairOptionLabel->setColor(Color3B::BLACK);
	_HairOptionLabel->setPosition(changeClothNode->getChildByName("HairOptionBG")->getPosition()+ changeClothNode->getPosition());
	_HairOptionLabel->setPositionY(_HairOptionLabel->getPositionY() + visible.height / 50);
	this->addChild(_HairOptionLabel);

	sprintf(OtionLabelBuff, "%s1", JsonParser::getJsonString("CreateCharacterScene", "HatLabel", JsonParser::JsonType::UserInterFace).c_str());
	_HatOptionLabel = Label::createWithTTF(OtionLabelBuff, JsonParser::fontType(), JsonParser::fontSize(18));
	_HatOptionLabel->setColor(Color3B::BLACK);
	_HatOptionLabel->setPosition(changeClothNode->getChildByName("HeadOptionBG")->getPosition() + changeClothNode->getPosition());
	_HatOptionLabel->setPositionY(_HatOptionLabel->getPositionY() + visible.height / 70);
	this->addChild(_HatOptionLabel);

	sprintf(OtionLabelBuff, "%s1", JsonParser::getJsonString("CreateCharacterScene", "ClothLabel", JsonParser::JsonType::UserInterFace).c_str());
	_ClothOptionLabel = Label::createWithTTF(OtionLabelBuff, JsonParser::fontType(), JsonParser::fontSize(18));
	_ClothOptionLabel->setColor(Color3B::BLACK);
	_ClothOptionLabel->setPosition(changeClothNode->getChildByName("ClothOptionBG")->getPosition() + changeClothNode->getPosition());
	this->addChild(_ClothOptionLabel);

	sprintf(OtionLabelBuff, "%s1", JsonParser::getJsonString("CreateCharacterScene", "PantsLabel", JsonParser::JsonType::UserInterFace).c_str());
	_PantsOptionLabel = Label::createWithTTF(OtionLabelBuff, JsonParser::fontType(), JsonParser::fontSize(18));
	_PantsOptionLabel->setColor(Color3B::BLACK);
	_PantsOptionLabel->setPosition(changeClothNode->getChildByName("PantsOptionBG")->getPosition() + changeClothNode->getPosition());
	_PantsOptionLabel->setPositionY(_PantsOptionLabel->getPositionY() - visible.height / 50);
	this->addChild(_PantsOptionLabel);

	auto HairRightBtn= static_cast<cocos2d::ui::Button*>(changeClothNode->getChildByName("HairRightBtn"));
	auto HairLeftBtn= static_cast<cocos2d::ui::Button*>(changeClothNode->getChildByName("HairLeftBtn"));
	auto HeadRightBtn= static_cast<cocos2d::ui::Button*>(changeClothNode->getChildByName("HeadRightBtn"));
	auto HeadLeftBtn = static_cast<cocos2d::ui::Button*>(changeClothNode->getChildByName("HeadLeftBtn"));
	auto ClothRightBtn= static_cast<cocos2d::ui::Button*>(changeClothNode->getChildByName("ClothRightBtn"));
	auto ClothLeftBtn = static_cast<cocos2d::ui::Button*>(changeClothNode->getChildByName("ClothLeftBtn"));
	auto PantsRightBtn= static_cast<cocos2d::ui::Button*>(changeClothNode->getChildByName("PantsRightBtn"));
	auto PantsLeftBtn= static_cast<cocos2d::ui::Button*>(changeClothNode->getChildByName("PantsLeftBtn"));

	HairRightBtn->addTouchEventListener(CC_CALLBACK_2(CreateCharacterScene::OptionBtnClick, this, CreateCharacterScene::HAIR_OPTION,CreateCharacterScene::RIGHT_OPTION));
	HairLeftBtn->addTouchEventListener(CC_CALLBACK_2(CreateCharacterScene::OptionBtnClick, this, CreateCharacterScene::HAIR_OPTION, CreateCharacterScene::LEFT_OPTION));
	HeadRightBtn->addTouchEventListener(CC_CALLBACK_2(CreateCharacterScene::OptionBtnClick, this, CreateCharacterScene::HEAD_OPTION, CreateCharacterScene::RIGHT_OPTION));
	HeadLeftBtn->addTouchEventListener(CC_CALLBACK_2(CreateCharacterScene::OptionBtnClick, this, CreateCharacterScene::HEAD_OPTION, CreateCharacterScene::LEFT_OPTION));
	ClothRightBtn->addTouchEventListener(CC_CALLBACK_2(CreateCharacterScene::OptionBtnClick, this, CreateCharacterScene::CLOTH_OPTION, CreateCharacterScene::RIGHT_OPTION));
	ClothLeftBtn->addTouchEventListener(CC_CALLBACK_2(CreateCharacterScene::OptionBtnClick, this, CreateCharacterScene::CLOTH_OPTION, CreateCharacterScene::LEFT_OPTION));
	PantsRightBtn->addTouchEventListener(CC_CALLBACK_2(CreateCharacterScene::OptionBtnClick, this, CreateCharacterScene::PAINTS_OPTION, CreateCharacterScene::RIGHT_OPTION));
	PantsLeftBtn->addTouchEventListener(CC_CALLBACK_2(CreateCharacterScene::OptionBtnClick, this, CreateCharacterScene::PAINTS_OPTION, CreateCharacterScene::LEFT_OPTION));
	//touch character switch animation
	auto touchSprite = _SceneCSB->getChildByName("TouchSpirte");
	auto touchListener = EventListenerTouchOneByOne::create();
	touchListener->onTouchBegan = [this](Touch* touch,Event* event)
	{
		if (event->getCurrentTarget()->getBoundingBox().containsPoint(touch->getLocation()))
		{
			static int count = 1;
			if (count == 0)
				_mainCharacter->_exCharacter->play("Idle", -1,false);
			else if (count == 1)
				_mainCharacter->_exCharacter->play("Run", -1,false);
			else if (count == 2)
				_mainCharacter->_exCharacter->play("NormalAttack1", -1,false);
			else if (count == 3)
				_mainCharacter->_exCharacter->play("NormalAttack2", -1,false);
			else if (count == 4)
				_mainCharacter->_exCharacter->play("NormalAttack3", -1,false);
			if (++count == 5)
				count = 0;
		}
		return false;
	};

	Director::getInstance()->getEventDispatcher()->addEventListenerWithSceneGraphPriority(touchListener, touchSprite);
	//input name
	_inputName = TextFieldTTF::textFieldWithPlaceHolder(JsonParser::getJsonString("CreateCharacterScene", "TextFieldText",
		JsonParser::JsonType::UserInterFace), JsonParser::fontType(), JsonParser::fontSize(16));
	_inputName->setColor(Color3B::BLACK);
	_inputName->setPosition(_SceneCSB->getChildByName("NameInput")->getPosition());
	_inputName->setAlignment(cocos2d::kCCTextAlignmentCenter);
	this->addChild(_inputName, INPUTNAME_TEXTFIELD, INPUTNAME_TEXTFIELD);
	auto inputNameListener = EventListenerTouchOneByOne::create();
	inputNameListener->onTouchBegan = [this](Touch* touch, Event* event)
	{
		//return true 後面的onTouchEnded 或是onTouchMoved等等 才會觸發	
		return true;
	};
	inputNameListener->onTouchEnded=[this](Touch* touch,Event* event)
	{
		Vec2 textPos;
		auto textField=(TextFieldTTF*)event->getCurrentTarget();
		auto textFieldSize=textField->getContentSize();
		textPos.x = textField->getPositionX() - textField->getContentSize().width / 2;
		textPos.y = textField->getPositionY() - textField->getContentSize().height / 2;
		auto textRect = Rect(textPos, textFieldSize);
		if (textRect.containsPoint(touch->getLocation()))
		{
			textField->attachWithIME();
		}
		else
		{
			textField->detachWithIME();
		}
	};
	Director::getInstance()->getEventDispatcher()->addEventListenerWithSceneGraphPriority(inputNameListener,_inputName);
	//start game btn
	auto btn= DimensionalButton::create(JsonParser::getJsonString("CreateCharacterScene","StartGame",JsonParser::JsonType::UserInterFace),
		JsonParser::fontType(), JsonParser::fontSize(24),Color4B::BLACK,"ui/woodunClickedBtn.png","ui/woodClickedBtn.png",
	[this](DimensionalButton* myBtn)
	{
		AudioManagement::getInstance()->play("sound/doneCreate.mp3", AudioManagement::AudioType::Sound, false);
		JsonParser::setSave("Infor", "CharacterName", _inputName->getString().c_str());
		Director::getInstance()->replaceScene(TransitionFade::create(1.f, SplashSceneFactory::createScene(SplashSceneFactory::preLoadScene::preLoadForestField)));
	});
	btn->setPosition(_SceneCSB->getChildByName("StartGame")->getPosition());
	this->addChild(btn, 100);

	AudioManagement::getInstance()->play("music/createCharacterSceneMusic.mp3", AudioManagement::AudioType::Music, true);

	return true;
}
void CreateCharacterScene::OptionBtnClick(cocos2d::Ref * sender, cocos2d::ui::Widget::TouchEventType type, int optionTag,int directionOption)
{
	char newLabelString[10];
	int newItemID=0;
	switch (type)
	{
	case cocos2d::ui::Widget::TouchEventType::BEGAN:
		break;
	case cocos2d::ui::Widget::TouchEventType::MOVED:
		break;
	case cocos2d::ui::Widget::TouchEventType::ENDED:
		switch (optionTag)
		{
		case CreateCharacterScene::HAIR_OPTION:
			AudioManagement::getInstance()->play("sound/ui_select.mp3", AudioManagement::AudioType::Sound, false);
			selectCountMethod(directionOption, _hairSelectCount);
			if (_hairSelectCount == 0)
			{
				_mainCharacter->_exCharacter->setCloth((int)RPGTool::ItemID::Hair_Tail);
				newItemID = (int)RPGTool::ItemID::Hair_Tail;
			}
			else if (_hairSelectCount == 1)
			{
				_mainCharacter->_exCharacter->setCloth((int)RPGTool::ItemID::Hair_Explore);
				newItemID = (int)RPGTool::ItemID::Hair_Explore;
			}
			//更新選單字串
			sprintf(newLabelString,"%s%d", JsonParser::getJsonString("CreateCharacterScene", "HairLabel",
				JsonParser::JsonType::UserInterFace).c_str(), _hairSelectCount + 1);
			_HairOptionLabel->setString(newLabelString);
			break;
		case CreateCharacterScene::HEAD_OPTION:
			AudioManagement::getInstance()->play("sound/ui_select.mp3", AudioManagement::AudioType::Sound, false);
			selectCountMethod(directionOption, _headSelectCount);
			if (_headSelectCount == 0)
			{
				_mainCharacter->_exCharacter->setCloth((int)RPGTool::ItemID::Head_Normal);
				newItemID = (int)RPGTool::ItemID::Head_Normal;
			}
			else if (_headSelectCount == 1)
			{
				_mainCharacter->_exCharacter->setCloth((int)RPGTool::ItemID::Head_Goat);
				newItemID = (int)RPGTool::ItemID::Head_Goat;
			}
			sprintf(newLabelString, "%s%d", JsonParser::getJsonString("CreateCharacterScene", "HatLabel",
				JsonParser::JsonType::UserInterFace).c_str(), _headSelectCount + 1);
			_HatOptionLabel->setString(newLabelString);
			break;
		case CreateCharacterScene::CLOTH_OPTION:
			AudioManagement::getInstance()->play("sound/ui_select.mp3", AudioManagement::AudioType::Sound, false);
			selectCountMethod(directionOption, _clothSelectCount);
			if (_clothSelectCount == 0)
			{
				_mainCharacter->_exCharacter->setCloth((int)RPGTool::ItemID::Cloth_Farmer);
				newItemID = (int)RPGTool::ItemID::Cloth_Farmer;
			}
			else if (_clothSelectCount == 1)
			{
				_mainCharacter->_exCharacter->setCloth((int)RPGTool::ItemID::Cloth_ChiuanJen);
				newItemID = (int)RPGTool::ItemID::Cloth_ChiuanJen;
			}
			sprintf(newLabelString, "%s%d", JsonParser::getJsonString("CreateCharacterScene", "ClothLabel",
				JsonParser::JsonType::UserInterFace).c_str(), _clothSelectCount + 1);
			_ClothOptionLabel->setString(newLabelString);
			break;
		case CreateCharacterScene::PAINTS_OPTION:
			AudioManagement::getInstance()->play("sound/ui_select.mp3", AudioManagement::AudioType::Sound, false);
			selectCountMethod(directionOption, _pantsSelectCount);
			if (_pantsSelectCount == 0)
			{
				_mainCharacter->_exCharacter->setCloth((int)RPGTool::ItemID::Pant_Farmer);
				newItemID = (int)RPGTool::ItemID::Pant_Farmer;
			}
			else if (_pantsSelectCount == 1)
			{
				_mainCharacter->_exCharacter->setCloth((int)RPGTool::ItemID::Pant_ChiuanJen);
				newItemID = (int)RPGTool::ItemID::Pant_ChiuanJen;
			}
			sprintf(newLabelString, "%s%d", JsonParser::getJsonString("CreateCharacterScene", "PantsLabel",
				JsonParser::JsonType::UserInterFace).c_str(), _pantsSelectCount + 1);
			_PantsOptionLabel->setString(newLabelString);
			break;
		}
		JsonParser::setEquipmentFromSave(newItemID, 0);
	case cocos2d::ui::Widget::TouchEventType::CANCELED:
		break;
	}
}
void CreateCharacterScene::weaponChoose(cocos2d::Ref * sender, cocos2d::ui::Widget::TouchEventType type,int weaponTag)
{
	_SceneCSB->getChildByName("WeaponLabelNode")->getChildByName("SamuraiLabel")->setVisible(false);
	_SceneCSB->getChildByName("WeaponLabelNode")->getChildByName("SwordLabel")->setVisible(false);
	_SceneCSB->getChildByName("WeaponLabelNode")->getChildByName("BoxerLabel")->setVisible(false);
	switch (type)
	{
	case cocos2d::ui::Widget::TouchEventType::BEGAN:
		break;
	case cocos2d::ui::Widget::TouchEventType::MOVED:

		break;
	case cocos2d::ui::Widget::TouchEventType::ENDED:
		AudioManagement::getInstance()->play("sound/ui_select2.mp3", AudioManagement::AudioType::Sound, false);
		int newWeapontype;
		switch (weaponTag)
		{
		case CreateCharacterScene::KNIFE_BUTTON:
			_SceneCSB->getChildByName("WeaponLabelNode")->getChildByName("SamuraiLabel")->setVisible(true);
			newWeapontype = (int)RPGTool::ItemID::Weapon_Sword;
			break;
		case CreateCharacterScene::SWORD_BUTTON:
			_SceneCSB->getChildByName("WeaponLabelNode")->getChildByName("SwordLabel")->setVisible(true);
			newWeapontype = (int)RPGTool::ItemID::Weapon_NoobSword;
			break;
		case CreateCharacterScene::BOXING_BUTTON:
			_SceneCSB->getChildByName("WeaponLabelNode")->getChildByName("BoxerLabel")->setVisible(true);
			newWeapontype = (int)RPGTool::ItemID::Weapon_Fist;
			break;
		}
		JsonParser::setEquipmentFromSave(newWeapontype, 0);
		_mainCharacter->_exCharacter->setAllEquipMentAccordingToSave();
		break;
	case cocos2d::ui::Widget::TouchEventType::CANCELED:
		break;
	default:
		break;
	}
}
void CreateCharacterScene::selectCountMethod(int directionOption, int& typeCount)
{
	if (directionOption == CreateCharacterScene::RIGHT_OPTION)
	{
		typeCount == 1 ? typeCount = 0 : typeCount++;
	}
	else
	{
		typeCount == 0 ? typeCount = 1 : typeCount--;
	}

}
NS_SCROLLMODE_END