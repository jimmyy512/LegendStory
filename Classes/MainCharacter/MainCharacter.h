#ifndef __MainCharacter__H__
#define __MainCharacter__H__
#include "cocos2d.h"
#include "cocostudio/CocoStudio.h"
#include "ui/CocosGUI.h"
#include "DefinitionScrollMode.h"
#include "dragonBones\cocos2dx\CCDragonBonesHeaders.h"
#include "Npc\NPCFactory.h"
NS_SCROLLMODE_BEGAN
class MainCharacter;
//主角動畫class
class EXCharacter
{
	friend class MainCharacter;
public:
	EXCharacter();
	~EXCharacter();
	dragonBones::CCArmatureDisplay* init(MainCharacter* mainCharacter);
	void play(const std::string& animationName, int playTimes,bool isAttackAnimation);
	inline dragonBones::CCArmatureDisplay* getArmatureDisplay() { return _armatureDisplay; };
	inline bool isAnimationPlaying() { return _armatureDisplay->getAnimation()->isPlaying(); };
	inline void setScale(float scale) { _armatureDisplay->setScale(scale); };
	inline void setScaleX(float scaleX) { _armatureDisplay->setScaleX(scaleX); };
	inline void setScaleY(float scaleY) { _armatureDisplay->setScaleY(scaleY); };
	inline float getAnimationTotalTime(const std::string& animationName) { return _armatureDisplay->getAnimation()->getState(animationName)->getTotalTime(); };
	inline float getLastAnimationTotalTime() { return _armatureDisplay->getAnimation()->getLastAnimationState()->getTotalTime(); };
	inline float getLastAnimationCurrentTime() { return _armatureDisplay->getAnimation()->getLastAnimationState()->getCurrentTime(); };
	//*清空身上所有裝備
	void resetAllEquipMent();
	void setAllEquipMentAccordingToSave();
	void setCloth(int ItemID);
	void setArmatureFacial(std::string newFacial);
private:
	//*由於每次撥放動畫，會覆蓋隱藏骨骼的設定，所以此函數針對武器去判斷。
	void updateWeapon();
	void updateHair();
	void updateHead();
private:
	dragonBones::DragonBonesData* _dragonBonesData;
	dragonBones::Armature* _armature;
	dragonBones::CCArmatureDisplay* _armatureDisplay;
	dragonBones::CCFactory _factory;
	int _WeaponIndex;
	int _HairType;
	int _HeadType;
	int _ClothType;
	int _PantType;
private:
	MainCharacter* _mainCharacter;
};
//主角屬性class
class MainCharacter : public cocos2d::Sprite
{
public:
	friend class EXCharacter;
	friend class HUDlayer;
	friend class Monster;
public:
	struct Status
	{
		bool _isIdle;
		bool _isRun;
		bool _isAttack;
		bool _isHurt;
		bool _isJump;
		bool _isDying;
		bool _faceDirection;//面對方向false面對右邊，true面對左邊
		bool _isGodMode;//無敵狀態
	};
	struct Property
	{//角色基本屬性
		int _Blood;    //氣血
		int _Force;	   //武力
		int _Magic;	   //內力
		int _Defend;   //防禦
		int _Dodge;	   //身法
		int _Popular;  //聲望
	};
	struct Infor
	{
		int _level;
		float _moveSpeed;
		int _exp;
		float _hp;
		float _mp;
		int _maxexp;
		float _maxhp;
		float _maxmp;
		float _damage;
		float _defend;
		float _dodge;
	};
	enum
	{
		RUNNING_TAG,
		IDLE_TAG,
		JUMP_TAG,
		SWINGSWORD_TAG,
		KICK_TAG,
		SHIED_TAG,
		JUMPATTACK_TAG
	};
public:
	MainCharacter();
	virtual ~MainCharacter();
	static MainCharacter* getMainCharacter();
	static MainCharacter* create(float rebornPosX, float rebornPosY,bool isShowCase);
	void resetAllStatus();
	//從存檔中讀取，並重新算出最新角色屬性數值
	void refreshBaseProperty();
	//*獲取招式名稱回傳觸發傷害的時機
	//*並且根據招式名稱給予主角攻擊距離
	//*並且根據招式名稱給予敵人受到傷害後的物理動力
	float getAttackTiming(const std::string& animationName);
	void addExp(float exp);
	//傳入攻擊來的方向
	void HurtAnimation(bool AttackDirection);
	//更改面對的方向
	void reviseDirection(bool faceDirection);
	void addHPMPToFull();
	void doDyingAnimation();
public:
	//*hudlayer利用這個偏移量，能讓hud在跟著主角移動時，站在正確的位子。
	float _startPosY;
	cocos2d::PhysicsBody* _CharacterPhysicsBody;
	Status status;
	//*封裝了一個EXCharacter 龍骨動畫類，專門控制主角動畫
	EXCharacter* _exCharacter;
	Property struct_property;
	Infor struct_infor;
	bool _isReplaceScene;
private:
	bool initMainCharacter(float rebornPosX, float rebornPosY, bool isShowCase);
	//*用來計時主角攻擊動畫，並發送傷害消息給怪物。
	void AttackMethod();
	void AccordingAttackPlaySound(const std::string& animationName);
	void RunAnimation(Ref* ref);
	void AttackAnimation(Ref* ref);
	void JumpAnimation(Ref* ref);
	void FallDownAnimation(Ref* ref);
	void checkIdleAnimation();
	void checkToNPCDistance();
	void StopRunningAnimation(Ref* ref);
	void updateCharacterOnTheSkyState();
	inline int getMaxJumpCount() { return _MaxJumpCount; }
	//*回傳true時代表當前魔力足夠消耗放技能,反之則否。
	bool checkAndSetMP(const std::string & animationName);
	void updatePhysicsFriction();
private:
	static MainCharacter* static_mainCharacter;
	cocos2d::Size _characterAttackRange;//主角攻擊距離
	cocos2d::Size visible;
	cocos2d::Vec2 origin;
	//*用來給普通連續攻擊時的鎖住旗標
	bool lockNormalClick;
	//*如果是展示用，就不會有物理引擎。像是在創角選單時，ShowCase就可以設為true。
	bool _isShowCase;
	//*當前跳躍次數
	int _currentJumpCount;
	//*最大跳躍次數
	int _MaxJumpCount;
	//*當前普通攻擊次數，普通攻擊共三招
	int _normalAttackCount;
	//*給予ScrollMonster::HurtStatus enum的值
	//*getAttackTiming()函數中進行賦值及更新。
	int _SendToMonsterHurtAnimation = 0;
	//*0代表只有正面敵人都會受到傷害 
	//*1代表只有背面敵人會受到傷害
	//*2代表兩面敵人都會受到傷害
	int _AttackRangeType = 0;
	//*放技能時的傷害加成
	int _SkillDamage = 0;
	//*0 代表普通攻擊
	//*1 代表技能1
	//*2 代表技能2
	//*3 代表技能3
	int _AttackType;
};


NS_SCROLLMODE_END
#endif
