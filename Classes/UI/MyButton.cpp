#include "MyButton.h"
USING_NS_CC;
DimensionalButton* DimensionalButton::create(const std::string btnString, std::string fontName, float fontSize, const cocos2d::Color4B &color,
	std::string unclickStringPath, std::string clickStringPath,  std::function<void(DimensionalButton*)> ontriggered)
{
	DimensionalButton* p = new(std::nothrow)DimensionalButton;
	TTFConfig ttfconfig(fontName, fontSize);
	if (p && p->init(btnString,fontName,fontSize, color, unclickStringPath, clickStringPath))
	{
		p->autorelease();
		p->_onTriggered = ontriggered;
		return p;
	}
	delete p;
	return nullptr;
}

bool DimensionalButton::init(const std::string btnString, std::string fontName, float fontSize, const cocos2d::Color4B &color,
	std::string unclickStringPath, std::string clickStringPath)
{
    visible = Director::getInstance()->getVisibleSize();
    origin = Director::getInstance()->getVisibleOrigin();
	_btnString = Label::createWithTTF(btnString, fontName, fontSize);
	_btnString->setTextColor(color);
	_unclickedSprite = Sprite::create(unclickStringPath);
	_clickedSprite = Sprite::create(clickStringPath);
	float delta_unClickedToClicked=_unclickedSprite->getBoundingBox().size.height- _clickedSprite->getBoundingBox().size.height;
	_clickedSprite->setPositionY(_clickedSprite->getPositionY() - delta_unClickedToClicked/2);
	_clickedSprite->setVisible(false);
	this->addChild(_unclickedSprite,0,12);
	this->addChild(_clickedSprite);
	this->addChild(_btnString);
	//根据label string大小 讓按鈕圖片變得和label相似
	float clickedSpriteToLabelContentScaleX=(_btnString->getContentSize().width*1.4/ _unclickedSprite->getContentSize().width);
	float clickedSpriteToLabelContentScaleY = (_btnString->getContentSize().height*1.4 / _unclickedSprite->getContentSize().height);
	_unclickedSprite->setScaleX(clickedSpriteToLabelContentScaleX);
	_unclickedSprite->setScaleY(clickedSpriteToLabelContentScaleY);
	_clickedSprite->setScaleX(clickedSpriteToLabelContentScaleX);
	_clickedSprite->setScaleY(clickedSpriteToLabelContentScaleY);
	//@touch event
	auto listener = EventListenerTouchOneByOne::create();
	listener->onTouchBegan = CC_CALLBACK_2(DimensionalButton::onTouchBegan, this);
	listener->onTouchEnded= CC_CALLBACK_2(DimensionalButton::onTouchEnded, this);
	listener->onTouchCancelled= CC_CALLBACK_2(DimensionalButton::onTouchCancelled, this);
	_eventDispatcher->addEventListenerWithSceneGraphPriority(listener, this);
    return true;
}
void DimensionalButton::onEnter()
{
	Node::onEnter();
	_touchRect = CCRectMake(this->getPositionX() - _unclickedSprite->getBoundingBox().size.width / 2,
		this->getPositionY() - _unclickedSprite->getBoundingBox().size.height / 2,
		_unclickedSprite->getBoundingBox().size.width,
		_unclickedSprite->getBoundingBox().size.height);
}
bool DimensionalButton::onTouchBegan(cocos2d::Touch * touch, cocos2d::Event * event)
{
	if (_touchRect.containsPoint(touch->getLocation()))
	{
		_btnString->setPositionY(_clickedSprite->getPositionY());
		_unclickedSprite->setVisible(false);
		_clickedSprite->setVisible(true);
	}
	return true;
}
void DimensionalButton::onTouchEnded(cocos2d::Touch * touch, cocos2d::Event * event)
{
	_btnString->setPositionY(_unclickedSprite->getPositionY());
	_unclickedSprite->setVisible(true);
	_clickedSprite->setVisible(false);
	if (_touchRect.containsPoint(touch->getLocation()))
	{
		_onTriggered(this);
	}
}
void DimensionalButton::onTouchCancelled(cocos2d::Touch * touch, cocos2d::Event * event)
{
	_btnString->setPositionY(_unclickedSprite->getPositionY());
	_unclickedSprite->setVisible(true);
	_clickedSprite->setVisible(false);
}

//@ShakeButton
ShakeButton::ShakeButton()
{


}
ShakeButton * ShakeButton::create(const std::string btnString, std::string fontName, float fontSize, const cocos2d::Color4B & color, 
	std::string BtnPath, std::function<void(ShakeButton*)> ontriggered)
{
	ShakeButton* p = new(std::nothrow)ShakeButton;
	TTFConfig ttfconfig(fontName, fontSize);
	if (p && p->init(btnString, fontName, fontSize, color, BtnPath))
	{
		p->autorelease();
		p->_onTriggered = ontriggered;
		return p;
	}
	delete p;
	return nullptr;
}
void ShakeButton::onEnter()
{
	Node::onEnter();
	_touchRect = CCRectMake(this->getPositionX() - (_btnSprite->getBoundingBox().size.width*this->getScaleX()) / 2,
		this->getPositionY() - (_btnSprite->getBoundingBox().size.height* this->getScaleY()) / 2,
		_btnSprite->getBoundingBox().size.width*this->getScaleX(),
		_btnSprite->getBoundingBox().size.height*this->getScaleY());
}
bool ShakeButton::init(const std::string btnString, std::string fontName, float fontSize, const cocos2d::Color4B &color,
	std::string BtnPath)
{
	visible = Director::getInstance()->getVisibleSize();
	origin = Director::getInstance()->getVisibleOrigin();
	_btnSprite = Sprite::create(BtnPath);
	this->setContentSize(_btnSprite->getContentSize());
	_btnString = Label::createWithTTF(btnString, fontName, fontSize);
	_btnString->setTextColor(color);
	_btnString->setPosition(_btnSprite->getBoundingBox().size.width / 2, _btnSprite->getBoundingBox().size.height / 2);
	this->addChild(_btnSprite, 0, 12);
	_btnSprite->addChild(_btnString);
	//@touch event
	_listener = EventListenerTouchOneByOne::create();
	_listener->setSwallowTouches(true);
	_listener->onTouchBegan = CC_CALLBACK_2(ShakeButton::onTouchBegan, this);
	_listener->onTouchEnded = CC_CALLBACK_2(ShakeButton::onTouchEnded, this);
	_listener->onTouchCancelled = CC_CALLBACK_2(ShakeButton::onTouchCancelled, this);
	_eventDispatcher->addEventListenerWithSceneGraphPriority(_listener, this);
	return true;
}
bool ShakeButton::onTouchBegan(cocos2d::Touch * touch, cocos2d::Event * event)
{
	if (_touchRect.containsPoint(this->_parent->convertToNodeSpace(touch->getLocation())))
	{
		_btnSprite->setScale(0.8f);
		return true;
	}
	return false;
}
void ShakeButton::onTouchEnded(cocos2d::Touch * touch, cocos2d::Event * event)
{
	_btnSprite->setScale(1.0f);
	if (_touchRect.containsPoint(this->_parent->convertToNodeSpace(touch->getLocation())))
	{
		_onTriggered(this);
	}
}
void ShakeButton::onTouchCancelled(cocos2d::Touch * touch, cocos2d::Event * event)
{
	
}
void ShakeButton::setOpacity(GLubyte opacity)
{
	return this->_btnSprite->Node::setOpacity(opacity);
}
void ShakeButton::setBtnString(std::string newText)
{
	_btnString->setString(newText);
}

