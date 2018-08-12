#include "MajitooTool.h"
#include "tools\JsonParser.h"
USING_NS_CC;
using namespace MyTools;
std::string MajitooTool::SplitStr(std::string readSplitStr, SplitType splitType)
{
	std::string wantReturnStr;
	std::string::size_type findPos;
	std::string::size_type findPos2;
	switch (splitType)
	{
	case SplitType::wantFacial:
		findPos = readSplitStr.find("@");
		if (findPos == std::string::npos)
		{
			throw "no Facial mark,then why you call this method?";
		}
		wantReturnStr.assign(readSplitStr, findPos, 3);
		wantReturnStr.erase(0, 1);
		break;
	case SplitType::wantNameCode:
		findPos = readSplitStr.find("*");
		if (findPos == std::string::npos)
		{
			return "NoNameCode";
		}
		wantReturnStr.assign(readSplitStr, findPos, 3);
		wantReturnStr.erase(0, 1);
		break;
	case SplitType::MarkFront:
		findPos = readSplitStr.find("@");
		findPos2 = readSplitStr.find("*");
		if (findPos != std::string::npos && findPos2 != std::string::npos)
		{
			wantReturnStr = readSplitStr.substr(0, readSplitStr.size() - 6);
		}
		else
		{
			wantReturnStr = readSplitStr.substr(0, readSplitStr.size() - 3);
		}
		//´À´«ª±®a©m¦W
		std::string::size_type characterCodePos = wantReturnStr.find("$");
		if (characterCodePos != std::string::npos)
			wantReturnStr.replace(characterCodePos, 1, JsonParser::getJsonString("Infor","CharacterName",JsonParser::JsonType::Save));
		break;
	}
	return wantReturnStr;
}