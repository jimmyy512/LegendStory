#include "AnimateTool.h"
USING_NS_CC;
Animation* AnimateTool::baseInit(const char* FileName)
{
	Animation* animation = Animation::create();
	String* fileName;
	int index = 1;
	while (1)
	{
		if (index < 10)
			fileName = String::createWithFormat("%s(0%d).png", FileName, index);
		else
			fileName = String::createWithFormat("%s(%d).png", FileName, index);
		SpriteFrame* spriteframe = SpriteFrameCache::getInstance()->getSpriteFrameByName(fileName->getCString());
		if (spriteframe == nullptr)
		{
			CCASSERT(index>1, "filename should not be nullptr");//可能找不到圖片
			break;
		}
		animation->addSpriteFrame(spriteframe);
		index++;
	}
	return animation;
}
AnimateTool * AnimateTool::getInstance()
{
	static AnimateTool* ptr = nullptr;
	if (ptr == nullptr)
	{
		ptr = new AnimateTool;
	}
	return ptr;
}
AnimateTool::AnimateTool()
{
	this->_animates = __Dictionary::create();
	this->_animates->retain();
}
AnimateTool::~AnimateTool()
{
	this->_animates->removeAllObjects();
	CC_SAFE_RELEASE_NULL(this->_animates);
}
Animate* AnimateTool::setAnimation(const char *FileName, const float delayUnit, const int loop, const char* plistFile)
{
	Animate* animate= nullptr;
	if (this->_animates->objectForKey(FileName) == nullptr)
	{
		if (plistFile != nullptr)
			SpriteFrameCache::getInstance()->addSpriteFramesWithFile(plistFile);
		Animation* animation = baseInit(FileName);
		animation->setDelayPerUnit(delayUnit);
		animation->setRestoreOriginalFrame(false);
		animation->setLoops(loop);
		this->_animates->setObject(animation, FileName);
		animate = Animate::create(animation);
	}
	else
	{
		Animation* animation = (Animation*)this->_animates->objectForKey(FileName);
		animation->setDelayPerUnit(delayUnit);
		animate = Animate::create(animation);
	}
	return animate;
}
Animate* AnimateTool::setAnimation(const char *FileName,const float delayUnit,const int loop, int tag,bool wantToRestoreOriginFrame, const char* plistFile)
{
	Animate* animate=nullptr;
	if (this->_animates->objectForKey(FileName) == nullptr)
	{
		if (plistFile != nullptr)
			SpriteFrameCache::getInstance()->addSpriteFramesWithFile(plistFile);
		Animation* animation = baseInit(FileName);
		animation->setDelayPerUnit(delayUnit);
		animation->setRestoreOriginalFrame(wantToRestoreOriginFrame);
		animation->setLoops(loop);
		this->_animates->setObject(animation, FileName);
		animate = Animate::create(animation);
		animate->setTag(tag);
	}
	else
	{
		Animation* animation = (Animation*)this->_animates->objectForKey(FileName);
		animation->setDelayPerUnit(delayUnit);
		animate = Animate::create(animation);
		animate->setTag(tag);
	}
	return animate;
}

cocos2d::Size AnimateTool::getFirstAnimationFrameSize(char* fileName, const char* plistFile)
{
	if (plistFile != nullptr)
		SpriteFrameCache::getInstance()->addSpriteFramesWithFile(plistFile);
	Animation* animation = Animation::create();
	String* _fileName= String::createWithFormat("%s(01).png", fileName);
	SpriteFrame* spriteframe = SpriteFrameCache::getInstance()->getSpriteFrameByName(_fileName->getCString());
	if (spriteframe == nullptr)
	{
		CCASSERT(false, "filename should not be nullptr");//可能找不到圖片
	}
	return Sprite::createWithSpriteFrame(spriteframe)->getContentSize();
}

cocos2d::SpriteFrame* AnimateTool::getFirstSpriteFrame(char* fileName, const char* plistFile)
{
	if (plistFile != nullptr)
		SpriteFrameCache::getInstance()->addSpriteFramesWithFile(plistFile);
	Animation* animation = Animation::create();
	String* _fileName = String::createWithFormat("%s(01).png", fileName);
	SpriteFrame* spriteframe = SpriteFrameCache::getInstance()->getSpriteFrameByName(_fileName->getCString());
	if (spriteframe == nullptr)
	{
		CCASSERT(false, "filename should not be nullptr");//可能找不到圖片
	}
	return spriteframe;
}
