#include "ItemSprite.h"
#include "RPGTools\RPGTool.h"
#include "tools\JsonParser.h"
USING_NS_CC;
ItemSprite* ItemSprite::create(const int Itemindex,const bool isSquareBG)
{
	ItemSprite* ptr = new(std::nothrow) ItemSprite;
	if (ptr && ptr->init(Itemindex,isSquareBG))
	{
		ptr->autorelease();
	}
	else
	{
		CC_SAFE_DELETE(ptr);
	}
	return ptr;
}
ItemSprite * ItemSprite::create(const int ItemID, const int ItemValue, const bool isSquareBG)
{
	ItemSprite* ptr = new(std::nothrow) ItemSprite;
	if (ptr && ptr->init(ItemID, ItemValue, isSquareBG))
	{
		ptr->autorelease();
	}
	else
	{
		CC_SAFE_DELETE(ptr);
	}
	return ptr;
}
ItemSprite * ItemSprite::create(const int ItemIndex, const int ItemID, const int ItemValue, const bool isSquareBG)
{
	ItemSprite* ptr = new(std::nothrow) ItemSprite;
	if (ptr && ptr->init(ItemIndex,ItemID, ItemValue, isSquareBG))
	{
		ptr->autorelease();
	}
	else
	{
		CC_SAFE_DELETE(ptr);
	}
	return ptr;
}
bool ItemSprite::init(const int Itemindex, const bool isSquareBG)
{//讀取物品ID的初始方式
    if ( !Sprite::init() )
    {
        return false;
    }
	this->_Itemindex = Itemindex;
	MyTools::JsonParser::getItemDataFromSave(Itemindex, this->_structItemData);
	this->_ItemSpritePicture = nullptr;
	commonInit(isSquareBG);
    return true;
}
bool ItemSprite::init(const int ItemID, const int ItemRank, const bool isSquareBG)
{//直接客製化ItemSprite的初始方式
	if (!Sprite::init())
	{
		return false;
	}
	this->_Itemindex = -1;
	if (ItemID == (int)RPGTool::ItemID::Cloth_Naked ||
		ItemID == (int)RPGTool::ItemID::Pant_Naked ||
		ItemID == (int)RPGTool::ItemID::Hair_Blade)
	{
		this->_structItemData.itemId = -1;
		this->_structItemData.itemValue = -1;
	}
	else
	{
		this->_structItemData.itemId = ItemID;
		this->_structItemData.itemValue = ItemRank;
	}
	this->_ItemSpritePicture = nullptr;
	commonInit(isSquareBG);
	return true;
}
bool ItemSprite::init(const int ItemIndex, const int ItemID, const int ItemRank, const bool isSquareBG)
{//直接客製化ItemSprite的初始方式
	if (!Sprite::init())
	{
		return false;
	}
	this->_Itemindex = ItemIndex;
	if (ItemID == (int)RPGTool::ItemID::Cloth_Naked ||
		ItemID == (int)RPGTool::ItemID::Pant_Naked ||
		ItemID == (int)RPGTool::ItemID::Hair_Blade)
	{
		this->_structItemData.itemId = -1;
		this->_structItemData.itemValue = -1;
	}
	else
	{
		this->_structItemData.itemId = ItemID;
		this->_structItemData.itemValue = ItemRank;
	}
	this->_ItemSpritePicture = nullptr;
	commonInit(isSquareBG);
	return true;
}
void ItemSprite::commonInit(const bool isSquareBG)
{
	if (isSquareBG)
	{//是否是正方形
		_itemBG = Sprite::create("ui/item/itemBG.png");
		this->addChild(_itemBG);
		//黑底圖片
		_blackBG = Sprite::create("ui/item/BlackBG.png");
		_blackBG->setPosition(_itemBG->getBoundingBox().size.width / 2, _itemBG->getBoundingBox().size.height / 2);
		_itemBG->addChild(_blackBG);
	}
	else
	{
		_itemBG = Sprite::create("ui/item/StackItemBG.png");
		this->addChild(_itemBG);
		//黑底圖片
		_blackBG = Sprite::create("ui/item/CircleBlackBG.png");
		_blackBG->setPosition(_itemBG->getBoundingBox().size.width / 2, _itemBG->getBoundingBox().size.height / 2);
		_itemBG->addChild(_blackBG);
	}
	//Label init
	_LabelValue = Label::createWithTTF("", "fonts/arial.ttf", 16);
	_LabelValue->setAnchorPoint(Vec2(1, 0));
	_LabelValue->setPosition(_itemBG->getBoundingBox().size.width-5.0f, 0);
	_itemBG->addChild(_LabelValue,2);
	if (this->_structItemData.itemId != -1 && this->_structItemData.itemValue!=-1)
	{
		//ItemSpritePicture
		//替換物品圖片
		_ItemSpritePicture = Sprite::create(RPGTool::accordingItemIDToData(this->_structItemData.itemId).SpritePath);
		_ItemSpritePicture->setPosition(_itemBG->getBoundingBox().size.width / 2, _itemBG->getBoundingBox().size.width / 2);
		_itemBG->addChild(_ItemSpritePicture,1);

		if (!RPGTool::isItemIDCanStack(this->_structItemData.itemId))
		{//物品不能堆疊，物品顯示rank
		 //ItemRankLabel
			char rankChar[5];
			sprintf(rankChar, "+%d", this->_structItemData.itemValue);
			_LabelValue->setString(rankChar);
			_LabelValue->enableGlow(this->getLabel4BColorAccordingItemRank(this->_structItemData.itemValue));
			_LabelValue->setColor(this->getLabel3BColorAccordingItemRank(this->_structItemData.itemValue));
			_LabelValue->setPosition(_itemBG->getBoundingBox().size.width - 2.0f, 0);
		}
		else
		{//物品能堆疊物品顯示數量
		 //ItemAmountLabel
			char amountChar[5];
			sprintf(amountChar, "%d", this->_structItemData.itemValue);
			_LabelValue->setString(amountChar);
			_LabelValue->enableGlow(Color4B::WHITE);
			_LabelValue->setColor(Color3B::WHITE);
			_LabelValue->setPosition(_itemBG->getBoundingBox().size.width -5.0f, 0);
		}
	}
	else
	{//空格子
		_blackBG->setVisible(false);
	}
}
void ItemSprite::setTouchRect()
{
	_touchRect = CCRectMake(this->getPosition().x- _itemBG->getBoundingBox().size.width/2,
		this->getPosition().y- _itemBG->getBoundingBox().size.height/2,
		_itemBG->getBoundingBox().size.width,
		_itemBG->getBoundingBox().size.height);
}
cocos2d::Color3B ItemSprite::getLabel3BColorAccordingItemRank(const int itemRank)
{
	if (itemRank == 0)
	{
		return Color3B::WHITE;
	}
	else if (itemRank == 1 ||
		itemRank == 2 ||
		itemRank == 3)
	{
		return Color3B::GREEN;
	}
	else if (itemRank == 4 ||
		itemRank == 5 ||
		itemRank == 6)
	{
		return Color3B::YELLOW;
	}
	else if (itemRank == 7 ||
		itemRank == 8 ||
		itemRank == 9)
	{
		return Color3B::ORANGE;
	}
	else
	{
		return Color3B::RED;
	}
}
cocos2d::Color4B ItemSprite::getLabel4BColorAccordingItemRank(const int itemRank)
{
	if (itemRank == 0)
	{
		return Color4B::WHITE;
	}
	else if (itemRank == 1 ||
		itemRank == 2 ||
		itemRank == 3)
	{
		return Color4B::GREEN;
	}
	else if (itemRank == 4 ||
		itemRank == 5 ||
		itemRank == 6)
	{
		return Color4B::YELLOW;
	}
	else if (itemRank == 7 ||
		itemRank == 8 ||
		itemRank == 9)
	{
		return Color4B::ORANGE;
	}
	else
	{
		return Color4B::RED;
	}
}
void ItemSprite::setNewItemSpritePicture(const int ItemID, const int ItemValue)
{
	this->_structItemData.itemId = ItemID;
	this->_structItemData.itemValue = ItemValue;
	if (ItemValue == -1)
	{//傳進來的物品Value為-1 代表物品欄沒東西 直接return
		inVisibleItemPicAndLabel();
		return;
	}
	if (RPGTool::isItemIDCanStack(this->_structItemData.itemId))
	{
		char amountChar[5];
		sprintf(amountChar, "%d", this->_structItemData.itemValue);
		_LabelValue->setString(amountChar);
		_LabelValue->enableGlow(Color4B::WHITE);
		_LabelValue->setColor(Color3B::WHITE);
	}
	else
	{
		char rankChar[5];
		//替換裝備等級文字
		sprintf(rankChar, "+%d", this->_structItemData.itemValue);
		_LabelValue->setString(rankChar);
		_LabelValue->enableGlow(getLabel4BColorAccordingItemRank(this->_structItemData.itemValue));
		_LabelValue->setColor(getLabel3BColorAccordingItemRank(this->_structItemData.itemValue));
	}
	//替換物品圖片
	if (_ItemSpritePicture != nullptr)
	{
		_ItemSpritePicture->removeFromParent();
	}
	_ItemSpritePicture = Sprite::create(RPGTool::accordingItemIDToData(this->_structItemData.itemId).SpritePath);
	_ItemSpritePicture->setPosition(_itemBG->getBoundingBox().size.width / 2, _itemBG->getBoundingBox().size.width / 2);
	_itemBG->addChild(_ItemSpritePicture);
	this->_LabelValue->setVisible(true);
	this->_ItemSpritePicture->setVisible(true);
	this->_blackBG->setVisible(true);
}
void ItemSprite::inVisibleItemPicAndLabel()
{
	this->_LabelValue->setVisible(false);
	this->_ItemSpritePicture->setVisible(false);
	this->_blackBG->setVisible(false);
	this->_structItemData.itemId = -1;
	this->_structItemData.itemValue = -1;
}