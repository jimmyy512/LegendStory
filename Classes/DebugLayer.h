#ifndef __DebugLayer__H__
#define __DebugLayer__H__

#include "cocos2d.h"

class DebugLayer : public cocos2d::Layer
{
public:
    virtual bool init();
	static DebugLayer* getInstance();
	cocos2d::Label* label;
	cocos2d::Label* label2;
	cocos2d::Label* label3;
private:
	//DebugLayer();
	static DebugLayer* static_debugLayer;
	cocos2d::Size visible;
	cocos2d::Vec2 origin;
};

#endif
