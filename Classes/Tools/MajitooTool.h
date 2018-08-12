#ifndef __MajitooTool_H__
#define __MajitooTool_H__

#include "cocos2d.h"

class MajitooTool
{
public:
	enum class SplitType
	{
		wantFacial,		//臉部字串
		wantNameCode,	//名子代號
		MarkFront		//有特殊標記之前的字串
	};
public:
	static std::string SplitStr(std::string readSplitStr, SplitType splitType);
};

#endif
