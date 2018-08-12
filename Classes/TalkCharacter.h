#ifndef __TalkCharacter_H__
#define __TalkCharacter_H__
#include "MissionMG.h"
#include "Npc\NPCFactory.h"
#include "cocos2d.h"
using namespace cocos2d;
class NPC;
//給Dialog用的抽象接口
class TalkCharacter :public cocos2d::Node{
public:
	enum class FacialType
	{
		NM,	//普通扎眼
		NT,	//普通說話
		AG,	//生氣說話
		SD,	//傷心說話
		BL,	//吐血說話
		HP,	//高興說話
		DO,	//死亡表情1
		DT,	//死亡表情2
		DB, //傻眼說話
		CF  //困惑說話
	};
public:
	virtual void setFacialType(FacialType facialType) = 0;
	virtual void setFacialType(std::string facialNameStr) = 0;
	virtual void updateArmatureFacial() = 0;
	virtual MissionMG::State getMissionState() = 0;
	virtual std::string getDialogCode() = 0;
	virtual NPC* getNPCInstance()=0;
	virtual void doIdleAnimation() = 0;
public:
	//*example:ZQ
	std::string _nameCode;
	//*example:ZhangQing
	std::string _name;
	//*example:徐長卿
	std::string _chineseName;
	//*對話代號
	std::string _DialogCode;
};
#endif
