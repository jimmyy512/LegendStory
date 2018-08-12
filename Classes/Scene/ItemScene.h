#ifndef __ItemScene_SCENE_H__
#define __ItemScene_SCENE_H__

#include "cocos2d.h"
#include "cocos-ext.h"
#include "cocostudio\CocoStudio.h"
#include "UI\CocosGUI.h"
#include "RPGTools\RPGTool.h"
#include "MainCharacter\MainCharacter.h"
class ItemSprite;
class ItemScene : public cocos2d::Layer
{
public:
	virtual void onEnterTransitionDidFinish();
    static cocos2d::Scene* createScene(cocos2d::RenderTexture* capture);
	ItemSprite* getEquipmentItemSpriteaccordingItemID(int ItemID);
    virtual bool init();
	virtual void onExit();
    CREATE_FUNC(ItemScene);
private:
	void setDialogInfor(int ItemID,int ItemRank);
	//*傳入物品ID 根據其部件 更新主角動畫，脫裝後的樣子。
	void UnEquipCharacterCloth(int ItemID);
	//*重新從存檔中讀取物品庫數據，並且重新加入到scroll view中
	void reCreateScrollView();
	//*更新角色能力資訊
	void setNewCharacterInfor(cocos2d::Ref* sender);
private:
	cocos2d::Size visible;
	cocos2d::Vec2 origin;
	//*scroll view item 一開始的位子
	cocos2d::Vec2 _scrollViewItem1Pos;
	cocos2d::Vec2 _scrollViewItem2Pos;
	cocos2d::Vec2 _scrollViewItem3Pos;
	cocos2d::Label* _hpValue;
	cocos2d::Label* _attackValue;
	cocos2d::Label* _mpValue;
	cocos2d::Label* _defendValue;
	cocos2d::Label* _speedValue;
	cocos2d::Label* _popularValue;
	cocos2d::Node* _sceneNode;
	bool _isTouchEquipment;
	cocos2d::ui::ScrollView* _scrollView;
	std::vector<ItemSprite*> _itemSprites;
	ScrollMode::MainCharacter* _mainCharacter;
	ItemSprite* _touchItemSprite;
	//一開始的Y值
	float ContainerInitPosY;
	//*觸摸背包欄位的第幾格。
	//*如果觸摸是裝備中的欄位，則是賦值RPGTool::CharacterPart。
	int _touchIndex;
	
};

#endif
