#ifndef __StoreScene_SCENE_H__
#define __StoreScene_SCENE_H__

#include "cocos2d.h"
#include "cocos-ext.h"
#include "cocostudio\CocoStudio.h"
#include "UI\CocosGUI.h"
#include "RPGTools\RPGTool.h"
#include "MainCharacter\MainCharacter.h"
class ItemSprite;
class NPC;
class StoreScene : public cocos2d::Layer
{
public:
	virtual void onEnterTransitionDidFinish();
    static cocos2d::Scene* createScene(cocos2d::RenderTexture* capture,NPC* npc);
    virtual bool init(NPC* npc);
	static StoreScene* create(NPC* npc);
private:
	//*物品操作選單
	void setDialogInfor(ItemSprite* touchItemSprite,bool isTouchItemBag);
	//*物品數量選擇選單
	void setAndShowChooseDialog(int ItemID,int ItemAmount);
	//*重新從存檔中讀取物品庫數據，並且重新加入到scroll view中
	void reCreateScrollView();
	//*如果玩家不夠錢支付物品時，return false
	bool setTradeMoney(int itemID,int itemValue,bool isBuy);
private:
	NPC* _npc;
	cocos2d::Size visible;
	cocos2d::Vec2 origin;
	//*scroll view item 一開始的位子
	cocos2d::Vec2 _ItemScrollViewItem1Pos;
	cocos2d::Vec2 _ItemScrollViewItem2Pos;
	cocos2d::Vec2 _ItemScrollViewItem3Pos;
	cocos2d::Vec2 _StoreScrollViewItem1Pos;
	cocos2d::Vec2 _StoreScrollViewItem2Pos;
	cocos2d::Vec2 _StoreScrollViewItem3Pos;
	cocos2d::Label* _hpValue;
	cocos2d::Label* _attackValue;
	cocos2d::Label* _mpValue;
	cocos2d::Label* _defendValue;
	cocos2d::Label* _savvyValue;
	cocos2d::Label* _popularValue;
	cocos2d::Label* _tradeMoneyLabel;
	cocos2d::Node* _chooseDialog;
	cocos2d::Node* _sceneNode;
	cocos2d::ui::ScrollView* _itemScrollView;
	cocos2d::ui::ScrollView* _storeScrollView;
	std::vector<ItemSprite*> _itemSprites;
	std::vector<ItemSprite*> _storeSprites;
	ItemSprite* _touchItemSprite;
	bool _isTouchItemBag;
	bool _isTouchChooseDialog;
	//物品容器一開始的Y值
	float ItemScrollViewContainerInitPosY;
	//商店容器一開始的Y值
	float StoreScrollViewContainerInitPosY;
	//*觸摸背包欄位的第幾格。
	int _touchIndex;
	int _currentMoney;
	int _tradeMoney;
	int _prevSliderPercent;
	//*當前物品最大數量 用來給Choose Dialog做判斷
	int _maxItemAmount;
};

#endif
