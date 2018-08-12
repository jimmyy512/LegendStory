#include "ScrollSceneObject.h"
#include "tools\AnimateTool.h"
#include "tools\JsonParser.h"
USING_NS_CC;
using namespace MyTools;
NS_SCROLLMODE_BEGAN
//Cloud
Cloud * Cloud::create(char * cloudPath)
{
	Cloud* pret = new(std::nothrow)Cloud;
	if (pret && pret->init(cloudPath))
	{
		pret->autorelease();
		return pret;
	}
	else
	{
		pret = nullptr;
		return nullptr;
	}
}
bool Cloud::init(char * cloudPath)
{
	if (!Sprite::initWithFile(cloudPath))
	{
		return false;
	}
	float randSpeed= (rand() % (20 +1) + 20);
	randSpeed /= 100;
	_moveSpeed = randSpeed;
	this->scheduleUpdate();
	return true;
}
void Cloud::update(float dt)
{
	this->setPositionX(this->getPositionX() - _moveSpeed);
	if (this->getPositionX() < -200)
	{
		this->unscheduleUpdate();
		this->removeFromParent();
	}
}

//TransferPoint
TransferPoint * TransferPoint::create(SplashSceneFactory::preLoadScene NextSceneName)
{
	TransferPoint* pret = new(std::nothrow)TransferPoint;
	if (pret && pret->init(NextSceneName))
	{
		pret->autorelease();
		return pret;
	}
	else
	{
		pret = nullptr;
		return nullptr;
	}
}
bool TransferPoint::init(SplashSceneFactory::preLoadScene NextSceneName)
{
	if (!Sprite::init())
	{
		return false;
	}
	this->_nextSceneNameEnum = NextSceneName;
	this->setAnchorPoint(Vec2(0.5, 0));
	this->setContentSize(AnimateTool::getInstance()->getFirstAnimationFrameSize(
		"TransferPoint", "map/ScrollMode/TransferPoint.plist"));
	this->runAction(AnimateTool::getInstance()->setAnimation("TransferPoint", 0.045f,
		-1, "map/ScrollMode/TransferPoint.plist"));
	return true;
}
cocos2d::Scene * TransferPoint::getNextScene()
{
	Scene* newScene = nullptr;
	switch (this->_nextSceneNameEnum)
	{
	case SplashSceneFactory::preLoadScene::preLoadLuoYangCity:
		newScene = PreloadLuoYang::createScene();
		break;
	case  SplashSceneFactory::preLoadScene::preLoadForestField:
		newScene = PreloadForestField::createScene();
		break;
	case  SplashSceneFactory::preLoadScene::preLoadChiuanJenMount:
		newScene = PreloadChiuanJenMount::createScene();
		break;
	case  SplashSceneFactory::preLoadScene::preLoadChiuanJen:
		newScene = PreloadChiuanJen::createScene();
		break;
	case SplashSceneFactory::preLoadScene::preLoadBattleTrainingField:
		newScene = PreloadBattleTraining::createScene();
		break;
	case SplashSceneFactory::preLoadScene::preLoadCave:
		newScene = PreloadCave::createScene();
		break;
	case SplashSceneFactory::preLoadScene::preLoadTestMap:
		newScene = PreloadTestMap::createScene();
		break;
	default:
		CCAssert(false, "preLoadScene fail");
	}
	return newScene;
}

//MapNameTip
MapNameTip * MapNameTip::create(std::string ShowText)
{
	MapNameTip* pret = new(std::nothrow)MapNameTip;
	if (pret && pret->init(ShowText))
	{
		pret->autorelease();
		return pret;
	}
	else
	{
		pret = nullptr;
		return nullptr;
	}
}
bool MapNameTip::init(std::string ShowText)
{
	if (!Sprite::initWithFile("ui/MapNameTip.png"))
	{
		return false;
	}
	this->_showText = ShowText;
	_label = Label::createWithTTF(this->_showText, JsonParser::fontType(), JsonParser::fontSize(20));
	_label->enableOutline(Color4B::BLACK, 2);
	_label->setColor(Color3B::WHITE);
	_label->setPosition(this->getBoundingBox().size.width / 2, this->getBoundingBox().size.height / 2);
	this->addChild(_label);

	this->scheduleUpdate();
	return true;
}
void MapNameTip::update(float dt)
{
	_label->setOpacity(this->getOpacity());
}
NS_SCROLLMODE_END