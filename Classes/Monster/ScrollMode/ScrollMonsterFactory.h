#ifndef __Monster__H__
#define __Monster__H__
#include "cocos2d.h"
#include "DefinitionScrollMode.h"
#include "dragonBones\cocos2dx\CCDragonBonesHeaders.h"
#include "MissionMG.h"
#include "Npc\NPCFactory.h"
#include "TalkCharacter.h"
NS_SCROLLMODE_BEGAN
class ScrollMonster;

class ScrollMonsterFactory
{
public:
	enum class MonsterName
	{
		Zombie,
		SmallThief
	};
	static ScrollMonster* createMonster(MonsterName monsterName,bool isShowCase);
};

class ScrollMonster : public TalkCharacter
{
	friend class MonsterAI;
public:
	enum class HurtStatus//決定你受傷的動畫
	{
		ShortBackWard,
		MiddleBackWard,
		StrongBackWard,
		HighBackWard
	};
	enum
	{
		monsterHP_Bar,
		monsterHP,
		monsterYellowHP
	};
	struct MonsterStatus//monster狀態
	{
		bool _isAttack = false;
		bool _isIdle = false;
		bool _isWalk = false;
		bool _isRun = false;
		bool _isHurt = false;
		bool _isDeath = false;
		bool _isDying = false;
		bool _faceDirection = false;
	};
	struct Reward
	{
		int exp;
		//第一個物品ID 第二個掉落機率 1~100%
		std::vector<std::pair<int,int>> items;
	};
public:
	//*子類建構子完，CREATE_FUNC會呼叫init()函數
	//*不要去重載他，除非你明白你在幹甚麼。
	virtual bool init(bool isShowCase)final;
	ScrollMonster();
	~ScrollMonster();
	void checkIdleAnimation();
	bool isMainCharacterInAttackRange();
	//*true:怪物在主角的右邊 false:怪物在主角的左邊
	bool isXpointMoreThenMainCharacter();
	void doAttackAnimation();
	virtual void doIdleAnimation();
	void doWalkAnimation();
	void doDyingAnimation();
	void doRunAnimation();
	void doHurtAnimation(HurtStatus hurtState);

	void AttackMethod();
	void inVisibleMonsterHP_Bar();
	void setHPByAttack(float attackDamage);//扣血，扣掉傷害後的血量
	void setFaceDirection(bool newFaceDirection);
	void showMonsterHP_Bar();
	cocos2d::Vec2 getDistanceToMainCharacter();
public://繼承的子類必須實例化
	virtual float getAttackTiming() = 0;//子類必須實作
	virtual void playAttackSound() = 0;
	virtual void playHurtSound() = 0;
	inline MissionMG::State getMissionState() { return _missionState; };
	inline std::string getDialogCode() { return _DialogCode; };
	inline NPC* getNPCInstance() { return nullptr; };
	void setFacialType(FacialType facialType);
	void setFacialType(std::string facialNameStr);
	//*根據_facial成員變量 來更新表情動畫
	void updateArmatureFacial();
public:
	MonsterStatus monsterStatus;
	Reward struct_Reward;
	MissionMG::State _missionState;
	//*show case為true不會上物理引擎 也不會有AI
	bool _isShowCase;
protected:
	void updateYellowHP();
	void updatePosition();
protected:
	cocos2d::Size visible;
	cocos2d::Vec2 origin;
	cocos2d::Vec2 _lastPosition;
	cocos2d::PhysicsBody* _physicsBody;
	cocos2d::Vec2 _prevPos;
	cocos2d::Vec2 _hpBarOffset;//血量條偏移量
	float _walkSpeed;
	float _displacement_x;
	float _yellowHP;
	float _currentHP;
	float _maxHP;
	float _damage;
	cocos2d::Size _AttackRange;
	dragonBones::DragonBonesData* _dragonBonesData;
	dragonBones::Armature* _armature;
	dragonBones::CCArmatureDisplay* _armatureDisplay;
	dragonBones::CCFactory _factory;
	std::string _facial;
private:
	void resetAllMonsterState();
	void repelWithPhysicsBody(HurtStatus hurtState);
	virtual void update(float dt);
	//*固定間格思考AI
	void ThinkAI(float dt);
	void ThinkAI2(float dt);
private:
	float _thinkInterval;
	float _thinkInterval2;

};

class Zombie : public ScrollMonster
{
public:
	Zombie();
	static Zombie* create(bool isShowCase);
private:
	virtual float getAttackTiming()override;
	virtual void playAttackSound()override;
	virtual void playHurtSound()override;
};

class SmallThief : public ScrollMonster
{
public:
	SmallThief();
	static SmallThief* create(bool isShowCase);
private:
	virtual float getAttackTiming()override;
	virtual void playAttackSound()override;
	virtual void playHurtSound()override;
};
NS_SCROLLMODE_END
#endif // __Monster__H__