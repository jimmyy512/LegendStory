#include "ItemScene.h"
#include "tools\JsonParser.h"
#include "tools\AudioManagement.h"
#include "ui\ScrollMode\HUDlayer.h"
#include "ui\MyButton.h"
#include "RPGTools\ItemSprite.h"
#include "Scene\ScrollMode\ScrollSceneFactory.h"
#include "TipsLabel.h"
#include "UI\ScrollMode\HUDlayer.h"
using namespace cocos2d::ui;
using namespace cocos2d::extension;
using namespace cocos2d::experimental;
using namespace MyTools;
USING_NS_CC;
bool sortFunc(std::pair<int, int> &left, std::pair<int, int> &right)
{
	return left.first > right.first;
}
void ItemScene::onEnterTransitionDidFinish()
{
	Layer::onEnterTransitionDidFinish();
	if (ScrollMode::HUDlayer::getHUDlayer() != nullptr)
	{
		ScrollMode::HUDlayer::getHUDlayer()->_isReplaceingScene = false;
	}
}
Scene* ItemScene::createScene(cocos2d::RenderTexture* capture)
{
    auto scene = Scene::create();
    auto layer = ItemScene::create();
    scene->addChild(layer);
	if (capture != nullptr)
	{
		auto visible = Director::getInstance()->getVisibleSize();
		auto origin = Director::getInstance()->getVisibleOrigin();
		auto pause_background = Sprite::createWithTexture(capture->getSprite()->getTexture());
		pause_background->setFlipY(true);
		pause_background->setColor(Color3B::GRAY);
		pause_background->setPosition(Vec2(visible.width / 2 + origin.x, visible.height / 2 + origin.y));
		layer->addChild(pause_background, 0);
	}
    return scene;
}
void ItemScene::onExit()
{
	Layer::onExit();
	NotificationCenter::getInstance()->removeAllObservers(this);
}
bool ItemScene::init()
{
    if ( !Layer::init() )
    {
        return false;
    }
    visible = Director::getInstance()->getVisibleSize();
    origin = Director::getInstance()->getVisibleOrigin();
	ScrollMode::HUDlayer::getHUDlayer()->setItemTip(false);
	_isTouchEquipment = false;
	_sceneNode = CSLoader::createNode("ItemScene/ItemScene.csb");
	this->addChild(_sceneNode, 1);
	_sceneNode->getChildByName("DialogSprite")->setVisible(false);
	this->_sceneNode->getChildByName("DialogSprite")->setPosition(_sceneNode->getChildByName("DialogInVisiblePos")->getPosition());
	_sceneNode->getChildByName("DialogSprite")->setZOrder(100);
	//主角
	_mainCharacter = ScrollMode::MainCharacter::create(0,0,true);
	_mainCharacter->_exCharacter->setScale(0.35f);
	_mainCharacter->_exCharacter->play("Idle", -1, false);
	_sceneNode->getChildByName("CharacterNode")->addChild(_mainCharacter,2);
													
	_scrollView = (cocos2d::ui::ScrollView*)_sceneNode->getChildByName("ScrollView");
	_scrollView->setSwallowTouches(false);
	//標題
	Label* titleLabel = Label::createWithTTF(JsonParser::getJsonString("ItemScene", "Title", JsonParser::JsonType::UserInterFace),
		JsonParser::fontType(), JsonParser::fontSize(26));
	titleLabel->setAnchorPoint(Vec2(0, 0.5f));
	titleLabel->enableGlow(Color4B::WHITE);
	titleLabel->setPosition(_sceneNode->getChildByName("TitlePos")->getPosition());
	_sceneNode->addChild(titleLabel,10);
	//錢
	Label* moneyLabel=Label::createWithTTF(JsonParser::getJsonString("Infor", "Money", JsonParser::JsonType::Save),
		JsonParser::fontType(), JsonParser::fontSize(24));
	moneyLabel->setAnchorPoint(Vec2(1,0.5));
	moneyLabel->setPosition(_sceneNode->getChildByName("MoneyPos")->getPosition());
	_sceneNode->addChild(moneyLabel, 10);
	//返回鍵
	auto returnSprite= _sceneNode->getChildByName("ReturnSprite");
	auto returnSpriteListener = EventListenerTouchOneByOne::create();
	returnSpriteListener->onTouchBegan = [](Touch* touch, Event* event)
	{
		return true;
	};
	returnSpriteListener->onTouchEnded = [](Touch* touch,Event* event) 
	{
		if (event->getCurrentTarget()->getBoundingBox().containsPoint(touch->getLocation()))
		{
			//上一個場景中的角色裝備更新
			ScrollMode::ScrollScene::getScrollScene()->_maincharacter->_exCharacter->setAllEquipMentAccordingToSave();
			//更新基本能力屬性
			ScrollMode::ScrollScene::getScrollScene()->_maincharacter->refreshBaseProperty();
			//重新刷新HUD一些狀態
			ScrollMode::HUDlayer::getHUDlayer()->refreshHUDlayer();
			//重新刷新HUD技能圖標
			ScrollMode::HUDlayer::getHUDlayer()->refreshSkillSprite();
			Director::getInstance()->popScene([](Scene* scene)
			{
				return TransitionFade::create(0.4f, scene, Color3B::BLACK);
			});
		}
	};
	Director::getInstance()->getEventDispatcher()->addEventListenerWithSceneGraphPriority(returnSpriteListener, returnSprite);
	
	//角色資訊標題欄
	auto HPInfoTitleLabel= Label::createWithTTF(JsonParser::getJsonString("ItemScene", "HPInfo", JsonParser::JsonType::UserInterFace),
		JsonParser::fontType(), JsonParser::fontSize(16));
	HPInfoTitleLabel->setColor(Color3B::WHITE);
	HPInfoTitleLabel->enableOutline(Color4B::BLACK, 1);
	HPInfoTitleLabel->setAnchorPoint(Vec2(0, 0.5));
	HPInfoTitleLabel->setPosition(_sceneNode->getChildByName("InforTitle1Pos")->getPosition());
	_sceneNode->addChild(HPInfoTitleLabel, 10);

	auto AttackInfoTitleLabel = Label::createWithTTF(JsonParser::getJsonString("ItemScene", "AttackInfo", JsonParser::JsonType::UserInterFace),
		JsonParser::fontType(), JsonParser::fontSize(16));
	AttackInfoTitleLabel->setColor(Color3B::WHITE);
	AttackInfoTitleLabel->enableOutline(Color4B::BLACK, 1);
	AttackInfoTitleLabel->setAnchorPoint(Vec2(0, 0.5));
	AttackInfoTitleLabel->setPosition(_sceneNode->getChildByName("InforTitle2Pos")->getPosition());
	_sceneNode->addChild(AttackInfoTitleLabel, 10);

	auto MPInfoTitleLabel = Label::createWithTTF(JsonParser::getJsonString("ItemScene", "MPInfo", JsonParser::JsonType::UserInterFace),
		JsonParser::fontType(), JsonParser::fontSize(16));
	MPInfoTitleLabel->setColor(Color3B::WHITE);
	MPInfoTitleLabel->enableOutline(Color4B::BLACK, 1);
	MPInfoTitleLabel->setAnchorPoint(Vec2(0, 0.5));
	MPInfoTitleLabel->setPosition(_sceneNode->getChildByName("InforTitle3Pos")->getPosition());
	_sceneNode->addChild(MPInfoTitleLabel, 10);

	auto DefendInfoTitleLabel = Label::createWithTTF(JsonParser::getJsonString("ItemScene", "DefendInfo", JsonParser::JsonType::UserInterFace),
		JsonParser::fontType(), JsonParser::fontSize(16));
	DefendInfoTitleLabel->setColor(Color3B::WHITE);
	DefendInfoTitleLabel->enableOutline(Color4B::BLACK, 1);
	DefendInfoTitleLabel->setAnchorPoint(Vec2(0, 0.5));
	DefendInfoTitleLabel->setPosition(_sceneNode->getChildByName("InforTitle4Pos")->getPosition());
	_sceneNode->addChild(DefendInfoTitleLabel, 10);

	auto SpeedInfoTitleLabel = Label::createWithTTF(JsonParser::getJsonString("ItemScene", "SpeedInfo", JsonParser::JsonType::UserInterFace),
		JsonParser::fontType(), JsonParser::fontSize(16));
	SpeedInfoTitleLabel->setColor(Color3B::WHITE);
	SpeedInfoTitleLabel->enableOutline(Color4B::BLACK, 1);
	SpeedInfoTitleLabel->setAnchorPoint(Vec2(0, 0.5));
	SpeedInfoTitleLabel->setPosition(_sceneNode->getChildByName("InforTitle5Pos")->getPosition());
	_sceneNode->addChild(SpeedInfoTitleLabel, 10);

	auto PopularInfoTitleLabel = Label::createWithTTF(JsonParser::getJsonString("ItemScene", "PopularInfo", JsonParser::JsonType::UserInterFace),
		JsonParser::fontType(), JsonParser::fontSize(16));
	PopularInfoTitleLabel->setColor(Color3B::WHITE);
	PopularInfoTitleLabel->enableOutline(Color4B::BLACK, 1);
	PopularInfoTitleLabel->setAnchorPoint(Vec2(0, 0.5));
	PopularInfoTitleLabel->setPosition(_sceneNode->getChildByName("InforTitle6Pos")->getPosition());
	_sceneNode->addChild(PopularInfoTitleLabel, 10);

	auto mainCharacter = ScrollMode::ScrollScene::getScrollScene()->_maincharacter;
	//角色資訊內容欄
	_hpValue = Label::createWithTTF("0",JsonParser::fontType(), JsonParser::fontSize(16));
	_hpValue->setColor(Color3B::ORANGE);
	_hpValue->enableOutline(Color4B::BLACK, 1);
	_hpValue->setAnchorPoint(Vec2(1, 0.5));
	_hpValue->setPosition(_sceneNode->getChildByName("InforValue1Pos")->getPosition());
	_sceneNode->addChild(_hpValue, 10);

	_attackValue = Label::createWithTTF("0", JsonParser::fontType(), JsonParser::fontSize(16));
	_attackValue->setColor(Color3B::ORANGE);
	_attackValue->enableOutline(Color4B::BLACK, 1);
	_attackValue->setAnchorPoint(Vec2(1, 0.5));
	_attackValue->setPosition(_sceneNode->getChildByName("InforValue2Pos")->getPosition());
	_sceneNode->addChild(_attackValue, 10);

	_mpValue = Label::createWithTTF("0", JsonParser::fontType(), JsonParser::fontSize(16));
	_mpValue->setColor(Color3B::ORANGE);
	_mpValue->enableOutline(Color4B::BLACK, 1);
	_mpValue->setAnchorPoint(Vec2(1, 0.5));
	_mpValue->setPosition(_sceneNode->getChildByName("InforValue3Pos")->getPosition());
	_sceneNode->addChild(_mpValue, 10);

	_defendValue = Label::createWithTTF("0", JsonParser::fontType(), JsonParser::fontSize(16));
	_defendValue->setColor(Color3B::ORANGE);
	_defendValue->enableOutline(Color4B::BLACK, 1);
	_defendValue->setAnchorPoint(Vec2(1, 0.5));
	_defendValue->setPosition(_sceneNode->getChildByName("InforValue4Pos")->getPosition());
	_sceneNode->addChild(_defendValue, 10);

	_speedValue = Label::createWithTTF("0", JsonParser::fontType(), JsonParser::fontSize(16));
	_speedValue->setColor(Color3B::ORANGE);
	_speedValue->enableOutline(Color4B::BLACK, 1);
	_speedValue->setAnchorPoint(Vec2(1, 0.5));
	_speedValue->setPosition(_sceneNode->getChildByName("InforValue5Pos")->getPosition());
	_sceneNode->addChild(_speedValue, 10);

	_popularValue = Label::createWithTTF("0", JsonParser::fontType(), JsonParser::fontSize(16));
	_popularValue->setColor(Color3B::ORANGE);
	_popularValue->enableOutline(Color4B::BLACK, 1);
	_popularValue->setAnchorPoint(Vec2(1, 0.5));
	_popularValue->setPosition(_sceneNode->getChildByName("InforValue6Pos")->getPosition());
	_sceneNode->addChild(_popularValue, 10);

	//重新整理物品按鈕
	auto ReSortItemBtn = ShakeButton::create(JsonParser::getJsonString("ItemScene", "ReSortItem", JsonParser::JsonType::UserInterFace),
		JsonParser::fontType(), JsonParser::fontSize(16), Color4B::BLACK, "ui/Button.png",
		[this](ShakeButton* shakeButton)
	{
		std::vector<std::pair<int, int>> itemDatas;
		JsonParser::getItemAllDataFromSave(itemDatas);
		std::sort(itemDatas.begin(), itemDatas.end());
		std::reverse(itemDatas.begin(), itemDatas.end());
		JsonParser::setItemAllDataFromSave(itemDatas);
		this->reCreateScrollView();
	});
	ReSortItemBtn->setPosition(this->_sceneNode->getChildByName("ReSortItemPos")->getPosition());
	this->addChild(ReSortItemBtn, 100);


	static bool isAddItem = false;
	if (!isAddItem)
	{
		//MyTools::JsonParser::AddItemToSave((int)RPGTool::ItemID::Weapon_NoobSword, 7);
		//MyTools::JsonParser::AddItemToSave((int)RPGTool::ItemID::Weapon_Blody, 10);
		//MyTools::JsonParser::AddItemToSave((int)RPGTool::ItemID::Cloth_Blody, 10);
		//MyTools::JsonParser::AddItemToSave((int)RPGTool::ItemID::Pant_Blody, 10);
		//MyTools::JsonParser::AddItemToSave((int)RPGTool::ItemID::MiddleBluePotion, 10);
		//MyTools::JsonParser::AddItemToSave((int)RPGTool::ItemID::LargeBluePotion, 10);
		//MyTools::JsonParser::AddItemToSave((int)RPGTool::ItemID::MiddleRedPotion, 10);
		//MyTools::JsonParser::AddItemToSave((int)RPGTool::ItemID::LargeRedPotion, 10);
		//MyTools::JsonParser::AddItemToSave((int)RPGTool::ItemID::Weapon_Fist, 1);
		//MyTools::JsonParser::AddItemToSave((int)RPGTool::ItemID::Cloth_ChiuanJen, 2);
		//MyTools::JsonParser::AddItemToSave((int)RPGTool::ItemID::Pant_ChiuanJen, 3);
		//MyTools::JsonParser::AddItemToSave((int)RPGTool::ItemID::Cloth_Farmer, 4);
		//MyTools::JsonParser::AddItemToSave((int)RPGTool::ItemID::Pant_Farmer, 5);
		//MyTools::JsonParser::AddItemToSave((int)RPGTool::ItemID::Weapon_Sword, 6);
		//MyTools::JsonParser::AddItemToSave((int)RPGTool::ItemID::Weapon_NoobSword, 7);
		//MyTools::JsonParser::AddItemToSave((int)RPGTool::ItemID::Weapon_Fist_Blue, 7);
		//MyTools::JsonParser::AddItemToSave((int)RPGTool::ItemID::Weapon_Shadow, 7);
		//MyTools::JsonParser::AddItemToSave((int)RPGTool::ItemID::Cloth_BlueKnight, 8);
		//MyTools::JsonParser::AddItemToSave((int)RPGTool::ItemID::Cloth_BlueKnight, 6);
		//MyTools::JsonParser::AddItemToSave((int)RPGTool::ItemID::Cloth_BlueKnight, 2);
		//MyTools::JsonParser::AddItemToSave((int)RPGTool::ItemID::Cloth_BlueKnight, 4);
		//MyTools::JsonParser::AddItemToSave((int)RPGTool::ItemID::Cloth_BlueKnight, 9);
		//MyTools::JsonParser::AddItemToSave((int)RPGTool::ItemID::Pant_BlueKnight, 8);
		//MyTools::JsonParser::AddItemToSave((int)RPGTool::ItemID::Hair_Explore, 9);
		//MyTools::JsonParser::AddItemToSave((int)RPGTool::ItemID::Hair_Tail, 10);
		//MyTools::JsonParser::AddItemToSave((int)RPGTool::ItemID::Hair_Band, 1);
		//MyTools::JsonParser::AddItemToSave((int)RPGTool::ItemID::Hair_Hat, 1);
		//MyTools::JsonParser::AddItemToSave((int)RPGTool::ItemID::SmallRedPotion, 2);
		//MyTools::JsonParser::AddItemToSave((int)RPGTool::ItemID::SmallBluePotion, 2);
		//for (int i = 0;i < 89;i++)
		//{
		//	MyTools::JsonParser::AddItemToSave((int)RPGTool::ItemID::Hair_Hat, 1);
		//}
	}
	isAddItem = true;

	//ScrollView Item
	_scrollViewItem1Pos = _sceneNode->getChildByName("ScrollView")->getChildByName("example1")->getPosition();
	_scrollViewItem2Pos = _sceneNode->getChildByName("ScrollView")->getChildByName("example2")->getPosition();
	_scrollViewItem3Pos = _sceneNode->getChildByName("ScrollView")->getChildByName("example3")->getPosition();
	Vec2 itemSpritePos;
	itemSpritePos.y = _sceneNode->getChildByName("ScrollView")->getChildByName("example1")->getPosition().y;
	for (int i = 0;i < RPGTool::Const_MaxItemCount;i++)
	{
		if (i % 3 == 0 && i>=3)
		{//每三格子換行
			itemSpritePos.y = itemSpritePos.y-50;
		}
		if (i % 3 == 0)
		{
			itemSpritePos.x= _sceneNode->getChildByName("ScrollView")->getChildByName("example1")->getPosition().x;
		}
		else if (i % 3 == 1)
		{
			itemSpritePos.x = _sceneNode->getChildByName("ScrollView")->getChildByName("example2")->getPosition().x;
		}
		else
			itemSpritePos.x = _sceneNode->getChildByName("ScrollView")->getChildByName("example3")->getPosition().x;
		ItemSprite* itemSprite = ItemSprite::create(i,true);
		_itemSprites.push_back(itemSprite);
		itemSprite->setPosition(itemSpritePos);
		itemSprite->setTouchRect();
		_scrollView->addChild(itemSprite);
	}
	ContainerInitPosY = _scrollView->getInnerContainerPosition().y;
	//touch
	auto touchListener = EventListenerTouchOneByOne::create();
	touchListener->onTouchBegan = [&](Touch* touch,Event* event)
	{
		if (event->getCurrentTarget()->getBoundingBox().containsPoint(touch->getLocation()))
		{//如果觸摸點是ScrollView
			for (auto it : _itemSprites)
			{//itemSpritePos.Y 一開始的位子是987.85 假如我一開始點擊的點是第一個的物品 並求出點擊螢幕上的Y軸 為169.30
			 //987.85-169.30=818.55f 這樣子 我就能算出 背包中物品的偏移量
				auto touchNodeSpace = _scrollView->convertToNodeSpace(touch->getLocation());
				touchNodeSpace.y = touchNodeSpace.y + 1318.55f - fabs(ContainerInitPosY - _scrollView->getInnerContainerPosition().y);
				if (it->_touchRect.containsPoint(touchNodeSpace) && it->_structItemData.itemId != -1)
				{
					_touchIndex = it->_Itemindex;
					this->_touchItemSprite = it;
					_isTouchEquipment = false;
					this->setDialogInfor(_touchItemSprite->_structItemData.itemId, _touchItemSprite->_structItemData.itemValue);
					break;
				}
			}
		}
		return true;
	};
	touchListener->onTouchMoved = [&](Touch* touch, Event* event)
	{
		if (event->getCurrentTarget()->getBoundingBox().containsPoint(touch->getLocation()))
		{//如果觸摸點是ScrollView
			for (auto it : _itemSprites)
			{
				auto touchNodeSpace = _scrollView->convertToNodeSpace(touch->getLocation());
				touchNodeSpace.y = touchNodeSpace.y + 1318.55f - fabs(ContainerInitPosY - _scrollView->getInnerContainerPosition().y);
				if (it->_touchRect.containsPoint(touchNodeSpace) && it->_structItemData.itemId !=-1)
				{
					if (_touchIndex != it->_Itemindex)
					{//觸摸到新的點開始計時
						_touchIndex = it->_Itemindex;
						this->_touchItemSprite = it;
						_isTouchEquipment = false;
						this->setDialogInfor(_touchItemSprite->_structItemData.itemId, _touchItemSprite->_structItemData.itemValue);
					}
					break;
				}
			}
		}
	};
	touchListener->onTouchEnded = [&](Touch* touch, Event* event)
	{
	};
	Director::getInstance()->getEventDispatcher()->addEventListenerWithSceneGraphPriority(touchListener, _scrollView);
	
	//對話框 關閉
	auto dialogCancel=this->_sceneNode->getChildByName("DialogSprite")->getChildByName("DialogCancel");
	auto dialogCancelListener = EventListenerTouchOneByOne::create();
	dialogCancelListener->onTouchBegan = [](Touch* touch, Event* event)
	{
		return true;
	};
	dialogCancelListener->onTouchEnded = [this](Touch* touch, Event* event)
	{
		auto NodeSpaceTouch= this->_sceneNode->getChildByName("DialogSprite")->convertToNodeSpace(touch->getLocation());
		if (event->getCurrentTarget()->getBoundingBox().containsPoint(NodeSpaceTouch))
		{
			this->_sceneNode->getChildByName("DialogSprite")->setVisible(false);
			this->_sceneNode->getChildByName("DialogSprite")->setPosition(_sceneNode->getChildByName("DialogInVisiblePos")->getPosition());
		}
	};
	Director::getInstance()->getEventDispatcher()->addEventListenerWithSceneGraphPriority(dialogCancelListener, dialogCancel);

	//對話框 裝備按鈕
	auto submitBtn = ShakeButton::create(JsonParser::getJsonString("ItemScene", "Equipment", JsonParser::JsonType::UserInterFace),
		JsonParser::fontType(),14,Color4B::BLACK, "ui/Button.png",
		[this](ShakeButton* shakeBtn)
	{//Equip
		if (_isTouchEquipment)
		{//如果觸摸的是裝備中的物品 進行脫掉裝備
			ItemSprite* itemSprite = (ItemSprite*)this->_sceneNode->getChildByName("DialogSprite")->getChildByName("DialogItemSprite");
			if (JsonParser::getItemCountFromSave()>=RPGTool::Const_MaxItemCount)
			{//背包已滿 脫下裝備失敗
				AudioManagement::getInstance()->play("sound/warning.mp3", AudioManagement::AudioType::Sound, false);
				TipsLabel::showTip(JsonParser::getJsonString("ItemScene","UnEquipmentFailTip",JsonParser::JsonType::UserInterFace),this,30,false);
			}
			else
			{//脫下裝備成功
				AudioManagement::getInstance()->play("sound/ui_select2.mp3", AudioManagement::AudioType::Sound, false);
				JsonParser::AddItemToSave(itemSprite->_structItemData.itemId, itemSprite->_structItemData.itemValue);
				ItemSprite* EquipmentItemSprite = getEquipmentItemSpriteaccordingItemID(itemSprite->_structItemData.itemId);
				EquipmentItemSprite->inVisibleItemPicAndLabel();
				this->_sceneNode->getChildByName("DialogSprite")->setVisible(false);
				JsonParser::setEquipmentFromSave(itemSprite->_structItemData.itemId, -1);
				//傳入物品ID 根據其部件 更新主角動畫，脫裝後的樣子。
				UnEquipCharacterCloth(itemSprite->_structItemData.itemId);
				reCreateScrollView();
			}
		}
		else
		{//如果觸摸的是物品庫中的物品
			//當前dialogSprite 裡面的ItemSprite 
			AudioManagement::getInstance()->play("sound/ui_select2.mp3", AudioManagement::AudioType::Sound, false);
			ItemSprite* itemSprite = (ItemSprite*)this->_sceneNode->getChildByName("DialogSprite")->getChildByName("DialogItemSprite");
			int equipmentID = JsonParser::getEquipmentFromSave((int)RPGTool::accordingItemIDToData(itemSprite->_structItemData.itemId).characterPart,
				JsonParser::Equipment::GetID);
			int equipmentValue = JsonParser::getEquipmentFromSave((int)RPGTool::accordingItemIDToData(itemSprite->_structItemData.itemId).characterPart,
				JsonParser::Equipment::GetValue);
			int itemSpriteID = itemSprite->_structItemData.itemId;
			int itemSpriteValue = itemSprite->_structItemData.itemValue;
			if (RPGTool::isItemIDCanDrink(itemSprite->_structItemData.itemId))
			{//此物品能喝 像是紅藥水，藍藥水之類的
				ItemSprite* EquipmentItemSprite = getEquipmentItemSpriteaccordingItemID(itemSprite->_structItemData.itemId);
				//更新裝備欄位
				EquipmentItemSprite->setNewItemSpritePicture(itemSpriteID, itemSpriteValue);
				JsonParser::setEquipmentFromSave(itemSpriteID, itemSpriteValue);
				//更新物品欄位
				_touchItemSprite->setNewItemSpritePicture(equipmentID, equipmentValue);
				JsonParser::setItemBagFromSave(this->_touchIndex, equipmentID, equipmentValue);
				//如果交換到物品庫的物品ID是-1，物品id value 設為-1
				if (equipmentID == -1 && equipmentValue == -1)
				{
					JsonParser::removeItemFromSave(_touchIndex);
				}
			}
			else
			{//此物品不能喝 像是武器 裝備等等
				ItemSprite* EquipmentItemSprite = getEquipmentItemSpriteaccordingItemID(itemSprite->_structItemData.itemId);
				//更新裝備欄位
				EquipmentItemSprite->setNewItemSpritePicture(itemSpriteID, itemSpriteValue);
				JsonParser::setEquipmentFromSave(itemSpriteID, itemSpriteValue);
				//更新物品欄位
				_touchItemSprite->setNewItemSpritePicture(equipmentID, equipmentValue);
				JsonParser::setItemBagFromSave(this->_touchIndex, equipmentID, equipmentValue);
				//角色動畫更新
				this->_mainCharacter->_exCharacter->setAllEquipMentAccordingToSave();
				//更新基本能力屬性
				ScrollMode::ScrollScene::getScrollScene()->_maincharacter->refreshBaseProperty();
			}
			//如果equipmentValue=-1 代表這個欄位是空的 就從物品庫移除
			if (equipmentValue == -1)
				JsonParser::removeItemFromSave(this->_touchIndex);
		}
		//物品資訊欄隱藏
		this->_sceneNode->getChildByName("DialogSprite")->setVisible(false);
		this->_sceneNode->getChildByName("DialogSprite")->setPosition(_sceneNode->getChildByName("DialogInVisiblePos")->getPosition());
	});
	submitBtn->setScale(1.1f);
	submitBtn->setName("DialogSubmitBtn");
	this->_sceneNode->getChildByName("DialogSprite")->addChild(submitBtn);
	submitBtn->setPosition(this->_sceneNode->getChildByName("DialogSprite")->getChildByName("BtnEquip")->getPosition());
	//dialog rankUp
	auto rankUpBtn = ShakeButton::create(JsonParser::getJsonString("ItemScene", "RankUp", JsonParser::JsonType::UserInterFace),
		JsonParser::fontType(), 14, Color4B::BLACK, "ui/Button.png",
		[this](ShakeButton* shakeBtn)
	{//rank up
		CCLOG("rankUp");
	});
	rankUpBtn->setScale(1.1f);
	this->_sceneNode->getChildByName("DialogSprite")->addChild(rankUpBtn);
	rankUpBtn->setPosition(this->_sceneNode->getChildByName("DialogSprite")->getChildByName("BtnRankUP")->getPosition());


	//equipment
	//Hair,    //頭部+血量
	//NeckLace,//項鍊+內力  
	//Belt,    //腰帶+內力
	//Cloth,   //上衣+防禦
	//Pant,    //下衣 + 防禦
	//Weapon,  //武器+攻擊力
	//Hair
	auto HairItemSprite = ItemSprite::create(
		JsonParser::getEquipmentFromSave((int)RPGTool::CharacterPart::Hair,JsonParser::Equipment::GetID),
		JsonParser::getEquipmentFromSave((int)RPGTool::CharacterPart::Hair, JsonParser::Equipment::GetValue),true);
	_sceneNode->addChild(HairItemSprite);
	HairItemSprite->setName("HairItemSprite");
	HairItemSprite->setScale(0.9f);
	HairItemSprite->setZOrder(_sceneNode->getChildByName("HairItemPos")->getZOrder());
	HairItemSprite->setPosition(_sceneNode->getChildByName("HairItemPos")->getPosition());

	auto NeckLaceItemSprite = ItemSprite::create(
		JsonParser::getEquipmentFromSave((int)RPGTool::CharacterPart::NeckLace, JsonParser::Equipment::GetID),
		JsonParser::getEquipmentFromSave((int)RPGTool::CharacterPart::NeckLace, JsonParser::Equipment::GetValue), true);
	_sceneNode->addChild(NeckLaceItemSprite);
	NeckLaceItemSprite->setName("NeckLaceItemSprite");
	NeckLaceItemSprite->setScale(0.9f);
	NeckLaceItemSprite->setZOrder(_sceneNode->getChildByName("NeckLaceItemPos")->getZOrder());
	NeckLaceItemSprite->setPosition(_sceneNode->getChildByName("NeckLaceItemPos")->getPosition());

	auto BeltItemSprite = ItemSprite::create(
		JsonParser::getEquipmentFromSave((int)RPGTool::CharacterPart::Belt, JsonParser::Equipment::GetID),
		JsonParser::getEquipmentFromSave((int)RPGTool::CharacterPart::Belt, JsonParser::Equipment::GetValue), true);
	_sceneNode->addChild(BeltItemSprite);
	BeltItemSprite->setName("BeltItemSprite");
	BeltItemSprite->setScale(0.9f);
	BeltItemSprite->setZOrder(_sceneNode->getChildByName("BeltItemPos")->getZOrder());
	BeltItemSprite->setPosition(_sceneNode->getChildByName("BeltItemPos")->getPosition());

	auto ClothItemSprite = ItemSprite::create(
		JsonParser::getEquipmentFromSave((int)RPGTool::CharacterPart::Cloth, JsonParser::Equipment::GetID),
		JsonParser::getEquipmentFromSave((int)RPGTool::CharacterPart::Cloth, JsonParser::Equipment::GetValue), true);
	_sceneNode->addChild(ClothItemSprite);
	ClothItemSprite->setName("ClothItemSprite");
	ClothItemSprite->setScale(0.9f);
	ClothItemSprite->setZOrder(_sceneNode->getChildByName("ClothItemPos")->getZOrder());
	ClothItemSprite->setPosition(_sceneNode->getChildByName("ClothItemPos")->getPosition());

	auto PantItemSprite = ItemSprite::create(
		JsonParser::getEquipmentFromSave((int)RPGTool::CharacterPart::Pant, JsonParser::Equipment::GetID),
		JsonParser::getEquipmentFromSave((int)RPGTool::CharacterPart::Pant, JsonParser::Equipment::GetValue), true);
	_sceneNode->addChild(PantItemSprite);
	PantItemSprite->setName("PantItemSprite");
	PantItemSprite->setScale(0.9f);
	PantItemSprite->setZOrder(_sceneNode->getChildByName("PantItemPos")->getZOrder());
	PantItemSprite->setPosition(_sceneNode->getChildByName("PantItemPos")->getPosition());

	auto WeaponItemSprite = ItemSprite::create(
		JsonParser::getEquipmentFromSave((int)RPGTool::CharacterPart::Weapon, JsonParser::Equipment::GetID),
		JsonParser::getEquipmentFromSave((int)RPGTool::CharacterPart::Weapon, JsonParser::Equipment::GetValue), true);
	_sceneNode->addChild(WeaponItemSprite);
	WeaponItemSprite->setName("WeaponItemSprite");
	WeaponItemSprite->setScale(0.9f);
	WeaponItemSprite->setZOrder(_sceneNode->getChildByName("WeaponItemPos")->getZOrder());
	WeaponItemSprite->setPosition(_sceneNode->getChildByName("WeaponItemPos")->getPosition());

	auto HPBarItemSprite = ItemSprite::create(
		JsonParser::getEquipmentFromSave((int)RPGTool::CharacterPart::HPBar, JsonParser::Equipment::GetID),
		JsonParser::getEquipmentFromSave((int)RPGTool::CharacterPart::HPBar, JsonParser::Equipment::GetValue), false);
	_sceneNode->addChild(HPBarItemSprite);
	HPBarItemSprite->setName("HPBarItemSprite");
	HPBarItemSprite->setScale(0.9f);
	HPBarItemSprite->setZOrder(_sceneNode->getChildByName("HPBarPos")->getZOrder());
	HPBarItemSprite->setPosition(_sceneNode->getChildByName("HPBarPos")->getPosition());

	auto MPBarItemSprite = ItemSprite::create(
		JsonParser::getEquipmentFromSave((int)RPGTool::CharacterPart::MPBar, JsonParser::Equipment::GetID),
		JsonParser::getEquipmentFromSave((int)RPGTool::CharacterPart::MPBar, JsonParser::Equipment::GetValue), false);
	_sceneNode->addChild(MPBarItemSprite);
	MPBarItemSprite->setName("MPBarItemSprite");
	MPBarItemSprite->setScale(0.9f);
	MPBarItemSprite->setZOrder(_sceneNode->getChildByName("MPBarPos")->getZOrder());
	MPBarItemSprite->setPosition(_sceneNode->getChildByName("MPBarPos")->getPosition());
	
	
	auto touchEquipmentItemListener = EventListenerTouchOneByOne::create();
	touchEquipmentItemListener->onTouchBegan = [&](Touch* touch, Event* event)
	{
		ItemSprite* touchItemSprite = (ItemSprite*)event->getCurrentTarget();
		auto nodeTouchPos = touchItemSprite->convertToNodeSpace(touch->getLocation());
		if (touchItemSprite->_itemBG->getBoundingBox().containsPoint(nodeTouchPos) && touchItemSprite->_structItemData.itemId!=-1)
		{//如果觸摸點是裝備中的裝備 並且不是空格子
			this->_touchIndex = (int)RPGTool::accordingItemIDToData(touchItemSprite->_structItemData.itemId).characterPart;
			this->_touchItemSprite = touchItemSprite;
			_isTouchEquipment = true;
			this->setDialogInfor(_touchItemSprite->_structItemData.itemId, _touchItemSprite->_structItemData.itemValue);
		}
		return true;
	};
	touchEquipmentItemListener->onTouchMoved = [&](Touch* touch, Event* event)
	{
		ItemSprite* touchItemSprite = (ItemSprite*)event->getCurrentTarget();
		auto nodeTouchPos = touchItemSprite->convertToNodeSpace(touch->getLocation());
		if (touchItemSprite->_itemBG->getBoundingBox().containsPoint(nodeTouchPos) && touchItemSprite->_structItemData.itemId != -1)
		{//如果觸摸點是裝備中的裝備 並且不是空格子
			if (this->_touchIndex != (int)RPGTool::accordingItemIDToData(touchItemSprite->_structItemData.itemId).characterPart)
			{
				_touchIndex = (int)RPGTool::accordingItemIDToData(touchItemSprite->_structItemData.itemId).characterPart;
				this->_touchItemSprite = touchItemSprite;
				_isTouchEquipment = true;
				this->setDialogInfor(_touchItemSprite->_structItemData.itemId, _touchItemSprite->_structItemData.itemValue);
			}
		}
	};
	touchEquipmentItemListener->onTouchEnded = [&](Touch* touch, Event* event)
	{
	};
	Director::getInstance()->getEventDispatcher()->addEventListenerWithSceneGraphPriority(touchEquipmentItemListener, HairItemSprite);
	Director::getInstance()->getEventDispatcher()->addEventListenerWithSceneGraphPriority(touchEquipmentItemListener->clone(),NeckLaceItemSprite);
	Director::getInstance()->getEventDispatcher()->addEventListenerWithSceneGraphPriority(touchEquipmentItemListener->clone(), BeltItemSprite);
	Director::getInstance()->getEventDispatcher()->addEventListenerWithSceneGraphPriority(touchEquipmentItemListener->clone(), ClothItemSprite);
	Director::getInstance()->getEventDispatcher()->addEventListenerWithSceneGraphPriority(touchEquipmentItemListener->clone(), PantItemSprite);
	Director::getInstance()->getEventDispatcher()->addEventListenerWithSceneGraphPriority(touchEquipmentItemListener->clone(), WeaponItemSprite);
	Director::getInstance()->getEventDispatcher()->addEventListenerWithSceneGraphPriority(touchEquipmentItemListener->clone(), HPBarItemSprite);
	Director::getInstance()->getEventDispatcher()->addEventListenerWithSceneGraphPriority(touchEquipmentItemListener->clone(), MPBarItemSprite);

	//訂閱消息
	NotificationCenter::getInstance()->addObserver(this, callfuncO_selector(ItemScene::setNewCharacterInfor), 
		"ItemSceneSetNewCharacterInfor", nullptr);
	this->setNewCharacterInfor(nullptr);
	return true;
}
void ItemScene::setDialogInfor(int ItemID, int ItemRank)
{
	AudioManagement::getInstance()->play("sound/ui_select.mp3", AudioManagement::AudioType::Sound, false);
	Node* dialogSprite = this->_sceneNode->getChildByName("DialogSprite");
	if (_isTouchEquipment)
	{//觸摸裝備中的裝備 dialog在右邊，否則在左邊
		dialogSprite->setPosition(_sceneNode->getChildByName("DialogRightPos")->getPosition());
		ShakeButton* DialogSubmitBtn = (ShakeButton*)dialogSprite->getChildByName("DialogSubmitBtn");
		//dialog "裝備"變成"取下"
		DialogSubmitBtn->setBtnString(JsonParser::getJsonString("ItemScene", "UnEquipment", JsonParser::JsonType::UserInterFace));
	}
	else
	{
		dialogSprite->setPosition(_sceneNode->getChildByName("DialogLeftPos")->getPosition());
		ShakeButton* DialogSubmitBtn = (ShakeButton*)dialogSprite->getChildByName("DialogSubmitBtn");
		//dialog "取下"變成"裝備"
		DialogSubmitBtn->setBtnString(JsonParser::getJsonString("ItemScene", "Equipment", JsonParser::JsonType::UserInterFace));
	}
	dialogSprite->setVisible(true);
	if (dialogSprite->getChildByName("DialogItemSprite")!=nullptr)
	{//如果之前已經有dialogItemSprite就把他移除
		dialogSprite->getChildByName("DialogItemSprite")->removeFromParent();
	}
	if (dialogSprite->getChildByName("DialogTitleLabel") != nullptr)
	{//如果之前已經有DialogTitleLabel就把他移除
		dialogSprite->getChildByName("DialogTitleLabel")->removeFromParent();
	}
	if (dialogSprite->getChildByName("DialogInforLabel") != nullptr)
	{//如果之前已經有DialogInforLabel就把他移除
		dialogSprite->getChildByName("DialogInforLabel")->removeFromParent();
	}
	//dialog小圖示
	ItemSprite* dialogItemSprite = ItemSprite::create(ItemID, ItemRank, true);
	dialogItemSprite->setPosition(this->_sceneNode->getChildByName("DialogSprite")->getChildByName("DialogItemSpritePos")->getPosition());
	dialogItemSprite->setName("DialogItemSprite");
	dialogSprite->addChild(dialogItemSprite);
	//dialog物品名稱
	std::string DialogTitleStr =JsonParser::getJsonString(RPGTool::accordingItemIDToData(dialogItemSprite->_structItemData.itemId).ItemName.c_str(),
												"Title",JsonParser::JsonType::ItemDOC);
	auto DialogTitleLabel = Label::create(DialogTitleStr, JsonParser::fontType(), JsonParser::fontSize(18));
	DialogTitleLabel->setAnchorPoint(Vec2(0, 1));
	DialogTitleLabel->setColor(Color3B::WHITE);
	DialogTitleLabel->enableOutline(Color4B::BLACK, 1);
	//DialogTitleLabel->enableGlow(Color4B::WHITE);
	DialogTitleLabel->setPosition(dialogSprite->getChildByName("DialogTitlePos")->getPosition());
	DialogTitleLabel->setName("DialogTitleLabel");
	dialogSprite->addChild(DialogTitleLabel);
	//dialog物品資訊
	std::string DialogInforStr = JsonParser::getJsonString(RPGTool::accordingItemIDToData(dialogItemSprite->_structItemData.itemId).ItemName.c_str(),
		"Infor", JsonParser::JsonType::ItemDOC);
	auto DialogInforLabel = Label::create(DialogInforStr, JsonParser::fontType(), JsonParser::fontSize(14));
	DialogInforLabel->setMaxLineWidth(130.f);
	DialogInforLabel->setAnchorPoint(Vec2(0, 1));
	DialogInforLabel->setColor(Color3B(244,199,110));
	//DialogInforLabel->enableOutline(Color4B::YELLOW, 1);
	DialogInforLabel->setPosition(dialogSprite->getChildByName("DialogInforPos")->getPosition());
	DialogInforLabel->setName("DialogInforLabel");
	dialogSprite->addChild(DialogInforLabel);
}
ItemSprite * ItemScene::getEquipmentItemSpriteaccordingItemID(int ItemID)
{
	ItemSprite* EquipmentItemSprite = nullptr;
	switch (RPGTool::accordingItemIDToData(ItemID).characterPart)
	{
	case RPGTool::CharacterPart::Hair:
		EquipmentItemSprite = (ItemSprite*)_sceneNode->getChildByName("HairItemSprite");
		break;
	case RPGTool::CharacterPart::NeckLace:
		EquipmentItemSprite = (ItemSprite*)_sceneNode->getChildByName("NeckLaceItemSprite");
		break;
	case RPGTool::CharacterPart::Belt:
		EquipmentItemSprite = (ItemSprite*)_sceneNode->getChildByName("BeltItemSprite");
		break;
	case RPGTool::CharacterPart::Cloth:
		EquipmentItemSprite = (ItemSprite*)_sceneNode->getChildByName("ClothItemSprite");
		break;
	case RPGTool::CharacterPart::Pant:
		EquipmentItemSprite = (ItemSprite*)_sceneNode->getChildByName("PantItemSprite");
		break;
	case RPGTool::CharacterPart::Weapon:
		EquipmentItemSprite = (ItemSprite*)_sceneNode->getChildByName("WeaponItemSprite");
		break;
	case RPGTool::CharacterPart::HPBar:
		EquipmentItemSprite = (ItemSprite*)_sceneNode->getChildByName("HPBarItemSprite");
		break;
	case RPGTool::CharacterPart::MPBar:
		EquipmentItemSprite = (ItemSprite*)_sceneNode->getChildByName("MPBarItemSprite");
		break;
	default:
		CCASSERT(false, "warning,no this id.");
		break;
	}
	return EquipmentItemSprite;
}
void ItemScene::UnEquipCharacterCloth(int ItemID)
{
	int newItemID=-1;
	switch (RPGTool::accordingItemIDToData(ItemID).characterPart)
	{
	case RPGTool::CharacterPart::Hair:
		_mainCharacter->_exCharacter->setCloth((int)RPGTool::ItemID::Hair_Blade);
		newItemID = (int)RPGTool::ItemID::Hair_Blade;
		break;
	case RPGTool::CharacterPart::Cloth:
		_mainCharacter->_exCharacter->setCloth((int)RPGTool::ItemID::Cloth_Naked);
		newItemID = (int)RPGTool::ItemID::Cloth_Naked;
		break;
	case RPGTool::CharacterPart::Pant:
		_mainCharacter->_exCharacter->setCloth((int)RPGTool::ItemID::Pant_Naked);
		newItemID = (int)RPGTool::ItemID::Pant_Naked;
		break;
	case RPGTool::CharacterPart::Weapon:
		_mainCharacter->_exCharacter->setCloth((int)RPGTool::ItemID::Weapon_Fist);
		newItemID = (int)RPGTool::ItemID::Weapon_Fist;
		break;
	}
	if (newItemID != -1)
	{//new ItemID 一定要賦值 才能去更新動畫
		JsonParser::setEquipmentFromSave(newItemID, -1);
		//角色動畫更新
		this->_mainCharacter->_exCharacter->setAllEquipMentAccordingToSave();
		//更新基本能力屬性
		ScrollMode::ScrollScene::getScrollScene()->_maincharacter->refreshBaseProperty();
	}
}
void ItemScene::reCreateScrollView()
{
	this->_itemSprites.clear();
	_scrollView->removeAllChildren();
	Vec2 itemSpritePos;
	itemSpritePos.y = _scrollViewItem1Pos.y;
	for (int i = 0;i < RPGTool::Const_MaxItemCount;i++)
	{
		if (i % 3 == 0 && i >= 3)
		{//每三格子換行
			itemSpritePos.y = itemSpritePos.y - 50;
		}
		if (i % 3 == 0)
		{
			itemSpritePos.x = _scrollViewItem1Pos.x;
		}
		else if (i % 3 == 1)
		{
			itemSpritePos.x = _scrollViewItem2Pos.x;
		}
		else
			itemSpritePos.x = _scrollViewItem3Pos.x;
		ItemSprite* itemSprite = ItemSprite::create(i, true);
		_itemSprites.push_back(itemSprite);
		itemSprite->setPosition(itemSpritePos);
		itemSprite->setTouchRect();
		_scrollView->addChild(itemSprite);
	}
}

void ItemScene::setNewCharacterInfor(Ref* sender)
{
	char tmp[10];
	sprintf(tmp,"%d", ScrollMode::ScrollScene::getScrollScene()->_maincharacter->struct_property._Blood);
	this->_hpValue->setString(tmp);
	strcpy(tmp, "");

	sprintf(tmp, "%d", ScrollMode::ScrollScene::getScrollScene()->_maincharacter->struct_property._Force);
	this->_attackValue->setString(tmp);
	strcpy(tmp, "");

	sprintf(tmp, "%d", ScrollMode::ScrollScene::getScrollScene()->_maincharacter->struct_property._Magic);
	this->_mpValue->setString(tmp);
	strcpy(tmp, "");

	sprintf(tmp, "%d", ScrollMode::ScrollScene::getScrollScene()->_maincharacter->struct_property._Defend);
	this->_defendValue->setString(tmp);
	strcpy(tmp, "");

	sprintf(tmp, "%d", ScrollMode::ScrollScene::getScrollScene()->_maincharacter->struct_property._Dodge);
	this->_speedValue->setString(tmp);
	strcpy(tmp, "");

	sprintf(tmp, "%d", ScrollMode::ScrollScene::getScrollScene()->_maincharacter->struct_property._Popular);
	this->_popularValue->setString(tmp);
	strcpy(tmp, "");
}
