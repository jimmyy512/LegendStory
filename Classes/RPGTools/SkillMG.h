#ifndef __SkillMG__H__
#define __SkillMG__H__

#include "cocos2d.h"

class SkillMG
{
public:
	enum class Name
	{
		Sword,
		Fist
	};
	enum class SkillPart
	{
		Skill1,
		Skill2,
		Skill3,
		Normal
	};
	enum class SkillType
	{
		Skill,
		Normal
	};
public:
	static SkillMG* getInstance();
	//*回傳HUDlayer的按鈕圖片路徑
	std::string getCurrentSkillPath(SkillMG::SkillPart skillPart);
	//*傳入是技能還是圖片,並且是第幾階段的
	//*回傳對應的主角動畫名稱
	std::string getCurrentSkillAnimationName(SkillMG::SkillType skillType,std::string stageStr);
	virtual ~SkillMG();
private:
	static SkillMG* static_skillMG;
};

#endif
