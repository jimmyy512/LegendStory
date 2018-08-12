#ifndef __ItemSprite_H__
#define __ItemSprite_H__

#include "cocos2d.h"

class ItemSprite : public cocos2d::Sprite
{
public:
	struct ItemData
	{
		int itemId;
		int itemValue;
	};
public:
	//*傳入是背包第幾格和RPGTool的ItemID enum class id
	static ItemSprite* create(const int Itemindex,const bool isSquareBG);
	//*傳入物品ID和物品階級，客製化物品圖片
	static ItemSprite* create(const int ItemID,const int ItemValue,const bool isSquareBG);
	//*給商店ScrollView用的，傳入第幾格物品和物品ID和物品階級，客製化物品圖片
	static ItemSprite* create(const int ItemIndex,const int ItemID, const int ItemValue, const bool isSquareBG);
    virtual bool init(const int Itemindex,const bool isSquareBG);
	virtual bool init(const int ItemID, const int ItemRank,const bool isSquareBG);
	virtual bool init(const int ItemIndex,const int ItemID, const int ItemRank, const bool isSquareBG);
	void setTouchRect();
	//*更換物品圖片 傳入(int)RPGTool::ItemID ItemRank
	void setNewItemSpritePicture(const int ItemID, const int ItemValue);
	//*隱藏物品圖片和label，並且把ItemID和ItemValue設為-1，設為-1將不能做一系列事件判斷
	void inVisibleItemPicAndLabel();
	cocos2d::Color3B getLabel3BColorAccordingItemRank(const int itemRank);
	cocos2d::Color4B getLabel4BColorAccordingItemRank(const int itemRank);
public:
	ItemData _structItemData;
	//*背包第幾格數
	int _Itemindex;
	cocos2d::Rect _touchRect;
	cocos2d::Sprite* _itemBG;
	cocos2d::Sprite* _blackBG;
private:
	cocos2d::Sprite* _ItemSpritePicture;
	cocos2d::Label* _LabelValue;
private:
	void commonInit(const bool isSquareBG);
};

#endif
