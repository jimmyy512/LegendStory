#include "MainScene.h"
#include "UI\MyButton.h"
#include "tools\JsonParser.h"
#include "tools\AudioManagement.h"
#include "GameStartMovie.h"
#include "TipsLabel.h"
#include "Scene\SplashSceneFactory.h"
#include "audio\include\AudioEngine.h"
#include "Scene\CreateCharacterScene.h"
USING_NS_CC;
using namespace experimental;
using namespace MyTools;
Scene* MainScene::createScene()
{
    auto scene = Scene::create();
    auto layer = MainScene::create();
    scene->addChild(layer);
    return scene;
}
bool MainScene::init()
{
    if ( !Layer::init() )
    {
        return false;
    }
    visible = Director::getInstance()->getVisibleSize();
    origin = Director::getInstance()->getVisibleOrigin();
	AudioEngine::play2d("music/beautiful.mp3", true, 100);
	_sceneNode = CSLoader::createNode("MainScene/MainScene.csb");
	this->addChild(_sceneNode);

	auto stitle1=ShakeButton::create("", "fonts/arial.ttf", 12, Color4B::BLACK, "MainScene/stitle1.png", 
		[this](ShakeButton* btn)
	{
		if (_isReplaceScene == true)
			return;
		this->_isReplaceScene = true;
		AudioEngine::play2d("sound/ui_select2.mp3", false, 100);
		AudioManagement::getInstance()->clearAudioManagement();
		Director::getInstance()->replaceScene(TransitionFade::create(1.0f, ScrollMode::CreateCharacterScene::createScene()));
	});
	stitle1->setPosition(_sceneNode->getChildByName("Node1")->getPosition());
	stitle1->setScale(0.5f);
	this->addChild(stitle1);

	auto stitle2 = ShakeButton::create("", "fonts/arial.ttf", 12, Color4B::BLACK, "MainScene/stitle2.png",
		[this](ShakeButton* btn)
	{
		if (!JsonParser::haveSaveFile())
		{
			TipsLabel::showTip(JsonParser::getJsonStringWithOutSave("MainScene","NoSaveTip",JsonParser::JsonType::UserInterFace),
				this, 30,true);
			return;
		}
		if (_isReplaceScene == true)
			return;
		this->_isReplaceScene = true;
		AudioEngine::play2d("sound/ui_select2.mp3", false, 100);
		AudioManagement::getInstance()->clearAudioManagement();
		Director::getInstance()->replaceScene(SplashSceneFactory::createScene(
			(SplashSceneFactory::preLoadScene)atoi(JsonParser::getJsonString("Infor", "LastPreloadSceneEnum", 
			JsonParser::JsonType::Save).c_str())));
	});
	stitle2->setPosition(_sceneNode->getChildByName("Node2")->getPosition());
	stitle2->setScale(0.5f);
	this->addChild(stitle2);

	auto stitle3 = ShakeButton::create("", "fonts/arial.ttf", 12, Color4B::BLACK, "MainScene/stitle3.png",
		[](ShakeButton* btn)
	{
		AudioEngine::play2d("sound/ui_select2.mp3", false, 100);
		Director::getInstance()->end();
	});
	//
	stitle3->setPosition(_sceneNode->getChildByName("Node3")->getPosition());
	stitle3->setScale(0.5f);
	this->addChild(stitle3);

    return true;
}
