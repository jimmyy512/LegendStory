#ifndef __NPCFactory_H__
#define __NPCFactory_H__
#include "cocos2d.h"
#include "dragonBones\cocos2dx\CCDragonBonesHeaders.h"
#include "MissionMG.h"
#include "TalkCharacter.h"
class NPC;
class NPCFactory
{
public:
	enum class NPCName
	{
		ZhangQing,
		ZhangYin,
		ZhangZhen,
		ZhangHen,
		ZhangFong,
		ZhangWo
   };
	static NPC* create(NPCName npcName,bool isShowCase,cocos2d::Vec2 bornPos=cocos2d::Vec2::ZERO);
private:
	
};

class NPC : public TalkCharacter
{
public:
	struct Status
	{
		bool _isIdle = false;
		bool _isIdle2 = false;
		bool _isWalk = false;
		bool _isRun = false;
		bool _isDying = false;
		bool _isDeath = false;
		bool _isTalk = false;
		bool _faceDirection;//*一開始所面對的方向
	};
public:
	//*子類 建構函數初始化完，才會呼叫init函數，
	//*不要重載init函數，除非你知道你在幹甚麼。
	bool init(bool isShowCase);
	NPC();
	~NPC();
	//*NPC_AI update this method
	virtual void update();
	void resetAllStatus();
	void setFaceDirection(bool newFaceDirection);
	void changeChatLabelAndFacialState();
	//*當玩家進到範圍內 更新npc面對方向
	void updateFaceDirectionWithCharacter();
	void setFacialType(FacialType facialType);
	void setFacialType(std::string facialNameStr);
	std::string inline getFacialType() { return this->_facial; };
	//*根據_facial成員變量 來更新表情動畫
	void updateArmatureFacial();
	virtual void doIdleAnimation();
	void doIdle2Animation();
	void doWalkAnimation();
	void doRunAnimation();
	void doDyingAnimation();
	inline MissionMG::State getMissionState() { return _missionState; };
	inline NPC* getNPCInstance() { return this; };
	//*獲取NPC要說的對話代號
	std::string getDialogCode();
public:
	//*對話框距離npc多少x  and  y
	cocos2d::Vec2 _dialogXY;
	cocos2d::Vec2 _prevPos;
	cocos2d::Sprite* _chatSprite;
	cocos2d::Label* _chatLabel;
	cocos2d::PhysicsBody* _physicsBody;
	dragonBones::DragonBonesData* _dragonBonesData;
	dragonBones::Armature* _armature;
	dragonBones::CCArmatureDisplay* _armatureDisplay;
	dragonBones::CCFactory _factory;
	Status status;
	//物品ID,物品value,出現機率
	std::vector<std::tuple<int, int, int>> _sellItems;
	//*show case為true,就不會彈出對話框
	bool _isShowCase;
	//*NPC能否自由走動
	bool _isNpcCanMove;
	//*移動速率
	float _moveSpeed;
	//*NPC能看見主角的可視距離
	float _visibleRange;
protected:
	//*角色思考時間間隔
	float _thinkInterval;
	float _thinkInterval2;
	//*解鎖對話劇情的任務進度
	int _unlockDialogCodeStage;
	//*對話代號
	std::string _DialogCode;
	//*驚嘆號
	cocos2d::Sprite* _exclam;
	MissionMG::State _missionState;
private:
	//*用來儲存NPC上一句說話的內容是哪一個
	int _prevRandNum = -1;
	std::string _facial;
private:
	//*根據子類建構子時所添加的販賣物品，並依照其機率去判斷那個物品是否會出現在交易選單中。
	void judgmentSellItem();
	//*固定間格思考AI
	void ThinkAI(float dt);
	void ThinkAI2(float dt);
};

////@WeaponTrader
//class WeaponTrader:public NPC
//{
//public:
//	static WeaponTrader* create(bool isShowCase);
//private:
//	WeaponTrader();
//};


//徐長卿
class ZhangQing :public NPC
{
public:
	static ZhangQing* create(bool isShowCase);
private:
	ZhangQing();
};

//蘇長胤
class ZhangYin :public NPC
{
public:
	static ZhangYin* create(bool isShowCase);
private:
	ZhangYin();
};

//長真道長
class ZhangZhen :public NPC
{
public:
	static ZhangZhen* create(bool isShowCase);
private:
	ZhangZhen();
};

//洪長恨
class ZhangHen :public NPC
{
public:
	static ZhangHen* create(bool isShowCase);
private:
	ZhangHen();
};

//王長風
class ZhangFong :public NPC
{
public:
	static ZhangFong* create(bool isShowCase);
private:
	ZhangFong();
};

//陳長悟
class ZhangWo :public NPC
{
public:
	static ZhangWo* create(bool isShowCase);
private:
	ZhangWo();
};
#endif
