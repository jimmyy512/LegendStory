#ifndef _MyButton_H__
#define _MyButton_H__

#include "cocos2d.h"

class DimensionalButton : public cocos2d::Node
{
public:
	static DimensionalButton* create(const std::string btnString, std::string fontName, float fontSize, const cocos2d::Color4B &color,
		std::string unclickStringPath, std::string clickStringPath,std::function<void(DimensionalButton*)> ontriggered);
    virtual bool init(const std::string btnString, std::string fontName,float fontSize, const cocos2d::Color4B &color,
		std::string unclickStringPath, std::string clickStringPath);
	virtual void onEnter();
private:
	bool onTouchBegan(cocos2d::Touch* touch, cocos2d::Event* event);
	void onTouchCancelled(cocos2d::Touch* touch, cocos2d::Event* event);
	void onTouchEnded(cocos2d::Touch* touch, cocos2d::Event* event);
private:
	cocos2d::Size visible;
	cocos2d::Vec2 origin;
	cocos2d::Rect _touchRect;
	cocos2d::Label* _btnString;
	cocos2d::Sprite* _unclickedSprite;
	cocos2d::Sprite* _clickedSprite;
	std::function<void(DimensionalButton*)> _onTriggered;
};

class ShakeButton : public cocos2d::Node
{
public:
	cocos2d::Sprite* _btnSprite;
	ShakeButton();
	static ShakeButton* create(const std::string btnString, std::string fontName, float fontSize, const cocos2d::Color4B &color,
		std::string BtnPath, std::function<void(ShakeButton*)> ontriggered);
	virtual bool init(const std::string btnString, std::string fontName, float fontSize, const cocos2d::Color4B &color,
		std::string BtnPath);
	virtual void onEnter();
	virtual void setOpacity(GLubyte opacity) override;
	void setBtnString(std::string newText);
private:
	bool onTouchBegan(cocos2d::Touch* touch, cocos2d::Event* event);
	void onTouchCancelled(cocos2d::Touch* touch, cocos2d::Event* event);
	void onTouchEnded(cocos2d::Touch* touch, cocos2d::Event* event);
private:
	cocos2d::Size visible;
	cocos2d::Vec2 origin;
	cocos2d::Rect _touchRect;
	cocos2d::Label* _btnString;
	cocos2d::EventListenerTouchOneByOne* _listener;
	std::function<void(ShakeButton*)> _onTriggered;
};
#endif
