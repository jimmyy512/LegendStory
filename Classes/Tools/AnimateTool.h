#ifndef __AnimateTool_SCENE_H__
#define __AnimateTool_SCENE_H__

#include "cocos2d.h"

class AnimateTool
{
public: 
	static AnimateTool* getInstance();
	AnimateTool();
	~AnimateTool();
	cocos2d::Animate* setAnimation(const char *FileName, const float delayUnit, const int loop, const char* plistFile=nullptr);
    cocos2d::Animate* setAnimation(const char *FileName, const float delayUnit, const int loop, int tag, bool wantToRestoreOriginFrame = true,const char* plistFile = nullptr);
	cocos2d::Size getFirstAnimationFrameSize(char* fileName, const char* plistFile = nullptr);
	cocos2d::SpriteFrame* getFirstSpriteFrame(char* fileName, const char* plistFile = nullptr);
private:
	cocos2d::Animation* baseInit(const char *FileName);
private:
	cocos2d::__Dictionary* _animates;
};

#endif // __GameScene_SCENE_H__
