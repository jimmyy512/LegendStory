#include "SplashSceneFactory.h"
#include "Scene\ScrollMode\ScrollSceneFactory.h"
#include "Scene\BattleMode\BattleScene.h"
#include "Tools\JsonParser.h"
USING_NS_CC;
using namespace ScrollMode;
using namespace MyTools;
cocos2d::Scene * SplashSceneFactory::createScene(preLoadScene preLoadsceneName)
{
	Scene* newScene = nullptr;
	int SceneEnum;
	switch (preLoadsceneName)
	{
	case SplashSceneFactory::preLoadScene::preLoadLuoYangCity:
		SceneEnum =(int)SplashSceneFactory::preLoadScene::preLoadLuoYangCity;
		newScene = PreloadLuoYang::createScene();
		break;
	case  SplashSceneFactory::preLoadScene::preLoadForestField:
		SceneEnum = (int)SplashSceneFactory::preLoadScene::preLoadForestField;
		newScene = PreloadForestField::createScene();
		break;
	case  SplashSceneFactory::preLoadScene::preLoadChiuanJenMount:
		SceneEnum = (int)SplashSceneFactory::preLoadScene::preLoadChiuanJenMount;
		newScene = PreloadChiuanJenMount::createScene();
		break;
	case  SplashSceneFactory::preLoadScene::preLoadChiuanJen:
		SceneEnum = (int)SplashSceneFactory::preLoadScene::preLoadChiuanJen;
		newScene = PreloadChiuanJen::createScene();
		break;
	case SplashSceneFactory::preLoadScene::preLoadBattleTrainingField:
		SceneEnum = (int)SplashSceneFactory::preLoadScene::preLoadBattleTrainingField;
		newScene = PreloadBattleTraining::createScene();
		break;
	case SplashSceneFactory::preLoadScene::preLoadCave:
		SceneEnum = (int)SplashSceneFactory::preLoadScene::preLoadCave;
		newScene = PreloadCave::createScene();
		break;
	case SplashSceneFactory::preLoadScene::preLoadTestMap:
		SceneEnum = (int)SplashSceneFactory::preLoadScene::preLoadTestMap;
		newScene = PreloadTestMap::createScene();
		break;
	default:
		CCAssert(false, "preLoadScene fail");
	}
	char tmpStr[10];
	sprintf(tmpStr, "%d", SceneEnum);
	JsonParser::setSave("Infor", "LastPreloadSceneEnum", tmpStr);
	return newScene;
}
void SplashScene::addLoadingSpriteWitchSpark()
{
	auto loadSprite = Sprite::create("sprite/loading.png");
	Size clipSize = loadSprite->getContentSize();
	Sprite* spark = Sprite::create("sprite/loadingSpark.png");
	spark->setPosition(-clipSize.width, 0);
	ClippingNode* clippingNode = ClippingNode::create();
	clippingNode->setPosition(visible.width / 2 + origin.x, visible.height / 2 + origin.y);
	this->addChild(clippingNode);
	clippingNode->setAlphaThreshold(0.05f);
	clippingNode->setContentSize(clipSize);
	clippingNode->setStencil(loadSprite);
	clippingNode->addChild(loadSprite, 1);
	clippingNode->addChild(spark, 2);
	MoveTo* moveAction = MoveTo::create(2.0f, Vec2(clipSize.width, 0));
	MoveTo* moveBackAction = MoveTo::create(2.0f, Vec2(-clipSize.width, 0));
	spark->runAction(RepeatForever::create(Sequence::create(moveAction, moveBackAction, NULL)));
}
void SplashScene::addWordJumpEffect(char* jumpWordString)
{
	auto CurrentSplashSceneLabel = Label::createWithTTF(jumpWordString, "fonts/arial.ttf", 20);
	CurrentSplashSceneLabel->setColor(Color3B::BLACK);
	CurrentSplashSceneLabel->enableGlow(Color4B::WHITE);
	CurrentSplashSceneLabel->setPosition(visible.width - CurrentSplashSceneLabel->getContentSize().width / 2 + origin.x,
		CurrentSplashSceneLabel->getContentSize().height / 2 + origin.y);
	this->addChild(CurrentSplashSceneLabel);
	for (int i = 0;i <= CurrentSplashSceneLabel->getStringLength() - 1;i++)
	{
		Sprite* currentWord = CurrentSplashSceneLabel->getLetter(i);
		currentWord->runAction(RepeatForever::create(Sequence::create(MoveTo::create(0.5f + (i*0.025), Vec2(currentWord->getPositionX(), currentWord->getPositionY() + 5)),
			MoveTo::create(0.5f + (i*0.025), Vec2(currentWord->getPositionX(), currentWord->getPositionY() - 5)), NULL)));
	}
}
bool SplashScene::init()
{
	if (!Layer::init())
	{
		return false;
	}
	visible = Director::getInstance()->getVisibleSize();
	origin = Director::getInstance()->getVisibleOrigin();
	
	//¼W¥[­I´º
	char BGnameStr[50];
	sprintf(BGnameStr, "SplashScene/splashBG%d.png", rand() % 4 + 1);
	auto sceneBG = Sprite::create(BGnameStr);
	sceneBG->setAnchorPoint(Vec2(0, 0));
	this->addChild(sceneBG);

	return true;
}

//@PreloadForestField
Scene* PreloadForestField::createScene()
{
	auto scene = Scene::create();
	auto layer = PreloadForestField::create();
	scene->addChild(layer);
	return scene;
}
bool PreloadForestField::init()
{
	if (!SplashScene::init())
	{
		return false;
	}
	this->addWordJumpEffect("Loading...");
	
	//png
	Director::getInstance()->getTextureCache()->addImage("dialog/talkDialog.png");
	//png+plist
	/*SpriteFrameCache::getInstance()->addSpriteFramesWithFile("ui/gameLayer.plist", "ui/gameLayer.png");*/
	this->loadTextureCallBack(nullptr);
    return true;
}
void PreloadForestField::loadTextureCallBack(cocos2d::Texture2D * texture)
{
	this->scheduleOnce(schedule_selector(PreloadForestField::nextScene), 2.0f);
}
void PreloadForestField::nextScene(float dt)
{
	Director::getInstance()->replaceScene(TransitionFade::create(2, ScrollSceneFactory::createScene(
		ScrollSceneFactory::SceneName::ForestFieldScene)));
}

//@PreloadLuoYang
Scene* PreloadLuoYang::createScene()
{
	auto scene = Scene::create();
	auto layer = PreloadLuoYang::create();
	scene->addChild(layer);
	return scene;
}
void PreloadLuoYang::nextScene(float dt)
{
	Director::getInstance()->replaceScene(TransitionFade::create(2, ScrollSceneFactory::createScene(
		ScrollSceneFactory::SceneName::LuoYangCityScene)));
}

//@ChiuanJenMount
Scene* PreloadChiuanJenMount::createScene()
{
	auto scene = Scene::create();
	auto layer = PreloadChiuanJenMount::create();
	scene->addChild(layer);
	return scene;
}
void PreloadChiuanJenMount::nextScene(float dt)
{
	Director::getInstance()->replaceScene(TransitionFade::create(2, ScrollSceneFactory::createScene(
		ScrollSceneFactory::SceneName::ChiuanJenMountScene)));
}

//@ChiuanJen
Scene* PreloadChiuanJen::createScene()
{
	auto scene = Scene::create();
	auto layer = PreloadChiuanJen::create();
	scene->addChild(layer);
	return scene;
}
void PreloadChiuanJen::nextScene(float dt)
{
	Director::getInstance()->replaceScene(TransitionFade::create(2, ScrollSceneFactory::createScene(
		ScrollSceneFactory::SceneName::ChiuanJenScene)));
}

//@PreloadCave
Scene* PreloadCave::createScene()
{
	auto scene = Scene::create();
	auto layer = PreloadCave::create();
	scene->addChild(layer);
	return scene;
}
void PreloadCave::nextScene(float dt)
{
	Director::getInstance()->replaceScene(TransitionFade::create(2, ScrollSceneFactory::createScene(
		ScrollSceneFactory::SceneName::CaveScene)));
}

///////////////////////////////BattleMode//////////////////////////////////////////
cocos2d::Scene * PreloadBattleTraining::createScene()
{
	auto scene = Scene::create();
	auto layer = PreloadBattleTraining::create();
	scene->addChild(layer);
	return scene;
}
bool PreloadBattleTraining::init()
{
	if (!SplashScene::init())
	{
		return false;
	}
	this->addWordJumpEffect("Battle...");

	//png
	Director::getInstance()->getTextureCache()->addImage("map/BattleMode/testmap.png");
	Director::getInstance()->getTextureCache()->addImage(AutoPolygon::generatePolygon("ui/BattleMode_Skill_Shoot.png").getFilename());
	Director::getInstance()->getTextureCache()->addImage(AutoPolygon::generatePolygon("ui/BattleMode_Skill_Bayonet.png").getFilename());
	Director::getInstance()->getTextureCache()->addImage(AutoPolygon::generatePolygon("ui/JoyStickBG.png").getFilename());
	Director::getInstance()->getTextureCache()->addImage(AutoPolygon::generatePolygon("ui/JoyStickBall.png").getFilename());
	Director::getInstance()->getTextureCache()->addImage(AutoPolygon::generatePolygon("ui/BattleMode_HUD_Triangle.png").getFilename());
	//png+plist
	SpriteFrameCache::getInstance()->addSpriteFramesWithFile("animate/soldierAction/soldierAttack.plist", "animate/soldierAction/soldierAttack.png");
	Director::getInstance()->getTextureCache()->addImageAsync("animate/soldierAction/soldierWalk.png",
		CC_CALLBACK_1(PreloadBattleTraining::loadTextureCallBack,this));

	return true;
}
void PreloadBattleTraining::loadTextureCallBack(cocos2d::Texture2D * texture)
{
	SpriteFrameCache::getInstance()->addSpriteFramesWithFile("animate/soldierAction/soldierWalk.plist", texture);
	this->scheduleOnce(schedule_selector(PreloadBattleTraining::nextScene), 2.0f);
}
void PreloadBattleTraining::nextScene(float dt)
{
	Director::getInstance()->replaceScene(TransitionFade::create(2, BattleMode::BattleScene::createScene(200,200)));
}

cocos2d::Scene * PreloadTestMap::createScene()
{
	auto scene = Scene::create();
	auto layer = PreloadTestMap::create();
	scene->addChild(layer);
	return scene;
}
bool PreloadTestMap::init()
{
	if (!SplashScene::init())
	{
		return false;
	}
	this->addWordJumpEffect("TestMap Loading...");
	Director::getInstance()->getTextureCache()->addImageAsync("map/ScrollMode/TestMap.png",
		CC_CALLBACK_1(PreloadTestMap::loadTextureCallBack, this));
	return true;
}
void PreloadTestMap::loadTextureCallBack(cocos2d::Texture2D * texture)
{
	this->scheduleOnce(schedule_selector(PreloadTestMap::nextScene), 2.0f);
}
void PreloadTestMap::nextScene(float dt)
{
	Director::getInstance()->replaceScene(TransitionFade::create(2, ScrollSceneFactory::createScene(
		ScrollSceneFactory::SceneName::TestScene)));
}
