#ifndef __TipsLabel_H__
#define __TipsLabel_H__

#include "cocos2d.h"

class TipsLabel : public cocos2d::Label
{
public:
	static void showTip(std::string text, cocos2d::Node* sceneNode,int fontSize,bool isWithOutSave);
private:
	static TipsLabel* create(int fontSize,bool isWithOutSave);
	virtual bool init(int fontSize, bool isWithOutSave);
	virtual void update(float dt);
};

#endif
