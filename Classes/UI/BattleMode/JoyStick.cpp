#include "JoyStick.h"
USING_NS_CC;
NS_BATTLEMODE_BEGAN
Scene* JoyStick::createScene()
{
    // 'scene' is an autorelease object
    auto scene = Scene::create();
    
    // 'layer' is an autorelease object
    auto layer = JoyStick::create();

    // add layer as a child to scene
    scene->addChild(layer);

    // return the scene
    return scene;
}

// on "init" you need to initialize your instance
bool JoyStick::init()
{
    if ( !Layer::init() )
    {
        return false;
    }
    visible = Director::getInstance()->getVisibleSize();
    origin = Director::getInstance()->getVisibleOrigin();

	auto joyStickBG = Sprite::create("ui/JoyStickBG.png");
	//joyStickBG->setPosition(joyStickBG->getContentSize().width / 2+origin.x, joyStickBG->getContentSize().height/2 + origin.y);
	this->addChild(joyStickBG,0,JOYSTICKBACKGROUND);

	auto joyStickBall = Sprite::create("ui/JoyStickBall.png");
	//joyStickBall->setPosition(joyStickBG->getPosition());
	this->addChild(joyStickBall,1,JOYSTICKBALL);
	hideJoyStick();

	auto listener = EventListenerTouchOneByOne::create();
	listener->onTouchBegan = [this](Touch* touch ,Event* event)
	{
		if (touch->getLocation().x < visible.width / 2+origin.x-visible.width/20)
		{
			showJoyStick();
			Sprite* joyStickBall = (Sprite*)getChildByTag(JOYSTICKBALL);
			Sprite* joyStickBG = (Sprite*)getChildByTag(JOYSTICKBACKGROUND);
			joyStickBG->setPosition(touch->getLocation() + origin);
			joyStickBall->setPosition(joyStickBG->getPosition());
			return true;
		}
	};
	listener->onTouchMoved = [this](Touch* touch, Event* event)
	{

		Sprite* joyStickBall = (Sprite*)getChildByTag(JOYSTICKBALL);
		Sprite* joyStickBG = (Sprite*)getChildByTag(JOYSTICKBACKGROUND);
		float joyStickRadius = joyStickBG->getContentSize().width / 2;
		Vec2 joyStickOriginCenter = joyStickBG->getPosition();
		float touchPosToCenterdistance = touch->getLocation().distance(joyStickOriginCenter);
		if (touchPosToCenterdistance < joyStickRadius)
		{
			joyStickBall->setPosition(touch->getLocation());
		}
		else
		{
			// 內分點公式: n/(m+n)*A+m/(m+n)*B 
			float n = touchPosToCenterdistance - joyStickRadius;
			float m = touchPosToCenterdistance - n;
			Vec2 A = joyStickOriginCenter;
			Vec2 B = touch->getLocation();
			Vec2 resault=(n*A + B*m) / (m + n);
			Sprite* joyStickBall = (Sprite*)getChildByTag(JOYSTICKBALL);
			joyStickBall->setPosition(resault);

			//得到兩點x的距離
			float x = joyStickBall->getPositionX() - joyStickBG->getPositionX();
			//算出斜邊長度
			float r = joyStickBG->getBoundingBox().size.width / 2;
			//得到這個角度的餘弦值(通過三角函數中的店裡：角度餘弦值=斜邊/斜邊)
			//cos=斜邊分之鄰邊
			float cosAngle = x / r;
			//反餘弦後 求出我們要的弧度
			float angle = acos(cosAngle);
			//如果條件成立 用負的弧度去代表
			if (joyStickBall->getPosition().y < joyStickBG->getPosition().y)
			{
				angle = -angle;
			}
			//CCLOG("degree %f", CC_RADIANS_TO_DEGREES(angle));//弧度換算成角度
			const float PI = 3.1415926;
			if (angle >= -PI / 4 && angle<PI / 4)
			{
				//CCLOG("right");
				

			}

			else if (angle >= PI / 4 && angle<3 * PI / 4)
			{

				//CCLOG("up");

			}
			else if ((angle >= 3 * PI / 4 && angle <= PI) || (angle >= -PI&&angle<-3 * PI / 4))
			{

				//CCLOG("left");

			}
			else if (angle >= -3 * PI / 4 && angle<-PI / 4)
			{

				//CCLOG("down");

			}
		}
	};
	listener->onTouchEnded = [this](Touch* touch, Event* event)
	{
		hideJoyStick();
	};
	Director::getInstance()->getEventDispatcher()->addEventListenerWithSceneGraphPriority(listener, joyStickBall);

    return true;
}
void JoyStick::showJoyStick()
{
	Sprite* joyStickBall = (Sprite*)getChildByTag(JOYSTICKBALL);
	Sprite* joyStickBG = (Sprite*)getChildByTag(JOYSTICKBACKGROUND);
	joyStickBall->setVisible(true);
	joyStickBG->setVisible(true);
}
void JoyStick::hideJoyStick()
{
	Sprite* joyStickBall = (Sprite*)getChildByTag(JOYSTICKBALL);
	Sprite* joyStickBG = (Sprite*)getChildByTag(JOYSTICKBACKGROUND);
	joyStickBall->setVisible(false);
	joyStickBG->setVisible(false);
}
NS_BATTLEMODE_END