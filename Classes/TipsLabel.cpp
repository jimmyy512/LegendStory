#include "TipsLabel.h"
#include "tools\JsonParser.h"
USING_NS_CC;
using namespace MyTools;
void TipsLabel::showTip(std::string text,cocos2d::Node* sceneNode,int fontSize,bool isWithOutSave)
{
	auto visible=Director::getInstance()->getVisibleSize();
	auto origin= Director::getInstance()->getVisibleOrigin();
	TipsLabel* tipsLabel = TipsLabel::create(fontSize, isWithOutSave);
	tipsLabel->setColor(Color3B::GREEN);
	tipsLabel->enableOutline(Color4B::BLACK, 2);
	tipsLabel->setString(text);
	tipsLabel->setPosition(visible.width / 2 + origin.x, visible.height/2+ origin.y);
	auto delayAction = DelayTime::create(2.0f);
	auto moveByAction = MoveTo::create(3.0f, Vec2(visible.width/2, visible.height));
	auto sequence = Sequence::create(delayAction, moveByAction,NULL);
	tipsLabel->runAction(sequence);
	sceneNode->addChild(tipsLabel,9999);
}
TipsLabel * TipsLabel::create(int fontSize, bool isWithOutSave)
{
	TipsLabel* ptr = new (std::nothrow)TipsLabel;
	if (ptr && ptr->init(fontSize, isWithOutSave))
	{
		ptr->autorelease();
	}
	else
	{
		CC_SAFE_DELETE(ptr);
	}
	return ptr;
}
bool TipsLabel::init(int fontSize,bool isWithOutSave)
{
	if (isWithOutSave)
	{
		if (!Label::initWithTTF("", "fonts/chinese.ttf", fontSize))
		{
			return false;
		}
	}
	else
	{
		if (!Label::initWithTTF("", JsonParser::fontType(), JsonParser::fontSize(fontSize)))
		{
			return false;
		}
	}
	this->schedule(schedule_selector(TipsLabel::update), 0.1f);
	return true;
}
void TipsLabel::update(float dt)
{
	auto visible = Director::getInstance()->getVisibleSize();
	auto origin = Director::getInstance()->getVisibleOrigin();
	if (this->getPositionY() >= visible.height)
	{
		this->unschedule(schedule_selector(TipsLabel::update));
		this->removeFromParent();
	}
	else
	{
		if (this->getOpacity() > 10)
		{
			this->setOpacity(this->getOpacity() - 4);
		}
	}
}
