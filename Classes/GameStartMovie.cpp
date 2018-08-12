#include "GameStartMovie.h"
#include "tools\JsonParser.h"
#include "RPGTools\RPGTool.h"
#include "Scene\CreateCharacterScene.h"
using namespace cocos2d::ui;
using namespace cocos2d::experimental;
using namespace MyTools;
USING_NS_CC;
GameStartMovie* GameStartMovie::static_instance = nullptr;
GameStartMovie::~GameStartMovie()
{
	static_instance = nullptr;
}
bool GameStartMovie::isAlive()
{
	if (static_instance == nullptr)
		return false;
	else
		return true;
}
Scene* GameStartMovie::createScene()
{
    auto scene = Scene::create();
    auto layer = GameStartMovie::create();
    scene->addChild(layer);
    return scene;
}
bool GameStartMovie::init()
{
    if ( !Layer::init() )
    {
        return false;
    }
    visible = Director::getInstance()->getVisibleSize();
    origin = Director::getInstance()->getVisibleOrigin();
	JsonParser::buildNewSave();
	static_instance = this;
	_touchCount = 0;

	_label = Label::createWithTTF(JsonParser::getJsonString("GameStartMovie","1", JsonParser::JsonType::UserInterFace)
		, JsonParser::fontType(),JsonParser::fontSize(26));
	_label->setColor(Color3B::WHITE);
	_label->setPosition(visible / 2);
	_label->setOpacity(0);
	this->addChild(_label);

	CallFunc* callfunc = CallFunc::create([&]()
	{
		_label->setString(JsonParser::getJsonString("GameStartMovie", "2", JsonParser::JsonType::UserInterFace));
	});
	CallFunc* callfunc2 = CallFunc::create([&]()
	{
		_label->setString(JsonParser::getJsonString("GameStartMovie", "3", JsonParser::JsonType::UserInterFace));
	});
	CallFunc* callfunc3 = CallFunc::create([&]()
	{
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID || CC_TARGET_PLATFORM == CC_PLATFORM_IOS || CC_TARGET_PLATFORM == CC_PLATFORM_TIZEN) && !defined(CC_PLATFORM_OS_TVOS)
		auto videoPlayer = cocos2d::experimental::ui::VideoPlayer::create();
		videoPlayer->setPosition(visible/2);
		videoPlayer->setAnchorPoint(Vec2::ANCHOR_MIDDLE);
		videoPlayer->setContentSize(visible);
		this->addChild(videoPlayer,1);
		if (videoPlayer)
		{
			videoPlayer->setFileName("Start.mp4");
			videoPlayer->play();
		}
		videoPlayer->addEventListener(CC_CALLBACK_2(GameStartMovie::videoEventCallback, this));
#endif
	});
	auto sequence=Sequence::create(FadeIn::create(4.0f), FadeOut::create(4.0f), callfunc,
									FadeIn::create(4.0f), FadeOut::create(4.0f), callfunc2, 
									FadeIn::create(4.0f), FadeOut::create(4.0f), callfunc3, NULL);
	_label->runAction(sequence);
	
    return true;
}
void GameStartMovie::videoPlayOverCallback()
{
	Director::getInstance()->replaceScene(ScrollMode::CreateCharacterScene::createScene());
}
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID || CC_TARGET_PLATFORM == CC_PLATFORM_IOS || CC_TARGET_PLATFORM == CC_PLATFORM_TIZEN) && !defined(CC_PLATFORM_OS_TVOS)
void GameStartMovie::videoEventCallback(Ref * sender, cocos2d::experimental::ui::VideoPlayer::EventType eventType)
{
	switch (eventType) {
	case cocos2d::experimental::ui::VideoPlayer::EventType::PLAYING:
		break;
	case cocos2d::experimental::ui::VideoPlayer::EventType::PAUSED:
		if (++this->_touchCount == 3)
		{
			Director::getInstance()->replaceScene(ScrollMode::CreateCharacterScene::createScene());
		}
		break;
	case cocos2d::experimental::ui::VideoPlayer::EventType::STOPPED:
		break;
	case cocos2d::experimental::ui::VideoPlayer::EventType::COMPLETED:
		videoPlayOverCallback();
		break;
	default:
		break;
	}
}
#endif