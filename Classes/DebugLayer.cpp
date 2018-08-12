#include "DebugLayer.h"
USING_NS_CC;
DebugLayer* DebugLayer::static_debugLayer = nullptr;
bool DebugLayer::init()
{
    if ( !Layer::init() )
    {
        return false;
    }
    visible = Director::getInstance()->getVisibleSize();
    origin = Director::getInstance()->getVisibleOrigin();

	this->setAnchorPoint(Vec2(0, 0));

	label = Label::createWithSystemFont("test1", "arial.ttf", 20);
	label->setPosition(visible.width / 2+origin.x, visible.height / 10 * 8+origin.y);
	this->addChild(label);
	label2 = Label::createWithSystemFont("test2", "arial.ttf", 20);
	label2->setPosition(visible.width / 2+origin.x, visible.height / 10 * 6+origin.y);
	this->addChild(label2);
	label3 = Label::createWithSystemFont("test3", "arial.ttf", 20);
	label3->setPosition(visible.width / 2+origin.x, visible.height / 10 * 4+origin.y);
	this->addChild(label3);


    return true;
}

DebugLayer * DebugLayer::getInstance()
{
	if (static_debugLayer == nullptr)
	{
		static_debugLayer = new(std::nothrow)DebugLayer;
		static_debugLayer->init();
	}
	return static_debugLayer;
}
