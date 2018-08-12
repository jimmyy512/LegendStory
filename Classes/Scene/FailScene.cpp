#include "FailScene.h"
#include "tools\JsonParser.h"
#include "UI\MyButton.h"
#include "tools\AudioManagement.h"
#include "SplashSceneFactory.h"
USING_NS_CC;
using namespace experimental;
using namespace MyTools;
Scene* FailScene::createScene()
{
    auto scene = Scene::create();
    auto layer = FailScene::create();
    scene->addChild(layer);
    return scene;
}
bool FailScene::init()
{
    if ( !Layer::init() )
    {
        return false;
    }
    visible = Director::getInstance()->getVisibleSize();
    origin = Director::getInstance()->getVisibleOrigin();
	_isReplaceScene = false;
	AudioManagement::getInstance()->play("music/FailScene.mp3", AudioManagement::AudioType::Music, false);
	_sceneNode = CSLoader::createNode("FailScene/FailScene.csb");
	_sceneNode->setVisible(true);
	this->addChild(_sceneNode);

	auto ReturnButton = ShakeButton::create(JsonParser::getJsonString("FailScene","Return",JsonParser::JsonType::UserInterFace), 
		JsonParser::fontType(), JsonParser::fontSize(12), Color4B::BLACK, "ui/Button.png",
		[this](ShakeButton* btn)
	{
		if (_isReplaceScene)
			return;
		_isReplaceScene = true;
		AudioManagement::getInstance()->play("sound/ui_select2.mp3", AudioManagement::AudioType::Sound, false);
		Director::getInstance()->replaceScene(TransitionFade::create(1.f, SplashSceneFactory::createScene(SplashSceneFactory::preLoadScene::preLoadChiuanJen)));
	});
	ReturnButton->setScale(1.3);
	ReturnButton->setPosition(_sceneNode->getChildByName("ReturnBtnPos")->getPosition());
	this->addChild(ReturnButton);

	JsonParser::setSave("Infor", "HP", "1000");
	JsonParser::setSave("Infor", "MP", "1000");
    return true;
}

void FailScene::onExit()
{
	Layer::onExit();
	AudioManagement::getInstance()->clearAudioManagement();
}
