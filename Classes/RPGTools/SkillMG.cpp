#include "SkillMG.h"
#include "tools\JsonParser.h"
USING_NS_CC;
using namespace MyTools;
SkillMG* SkillMG::static_skillMG = nullptr;
SkillMG * SkillMG::getInstance()
{
	if (static_skillMG == nullptr)
		static_skillMG = new SkillMG;
	return static_skillMG;
}

std::string SkillMG::getCurrentSkillPath(SkillMG::SkillPart skillPart)
{
	int SkillTypeIndex= atoi(JsonParser::getJsonString("Infor", "SkillType", JsonParser::JsonType::Save).c_str());
	std::string path;
	switch (SkillTypeIndex)
	{
	case (int)Name::Sword:
		path = "ui/SkillBG/Sword_Skill";
		break;
	case (int)Name::Fist:
		path = "ui/SkillBG/Fist_Skill";
		break;
	}
	switch (skillPart)
	{
	case SkillPart::Normal:
		path.append("0.png");
		break;
	case SkillPart::Skill1:
		path.append("1.png");
		break;
	case SkillPart::Skill2:
		path.append("2.png");
		break;
	case SkillPart::Skill3:
		path.append("3.png");
		break;
	}
	return path;
}

std::string SkillMG::getCurrentSkillAnimationName(SkillMG::SkillType skillType, std::string stageStr)
{
	std::string animationNameStr;
	int SkillTypeIndex = atoi(JsonParser::getJsonString("Infor", "SkillType", JsonParser::JsonType::Save).c_str());
	switch (SkillTypeIndex)
	{
	case 0:
		animationNameStr.append("Normal");
		break;
	case 1:
		animationNameStr.append("Dragon");
		break;
	default:
		CCAssert(false, "no this Condition");
	}
	switch (skillType)
	{
	case SkillType::Normal:
		animationNameStr.append("Attack");
		break;
	case SkillType::Skill:
		animationNameStr.append("Skill");
		break;
	default:
		CCAssert(false, "no this Condition");
	}
	animationNameStr.append(stageStr);
	return animationNameStr;
}

SkillMG::~SkillMG()
{
	delete static_skillMG;
}
