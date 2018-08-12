#include "MissionScene.h"
#include "tools\JsonParser.h"
#include "UI\MyButton.h"
#include "UI\ScrollMode\HUDlayer.h"
#include "tools\AudioManagement.h"
USING_NS_CC;
using namespace MyTools;
Scene* MissionScene::createScene(cocos2d::RenderTexture* capture)
{
	Size visible = Director::getInstance()->getVisibleSize();
	Vec2 origin = Director::getInstance()->getVisibleOrigin();
	auto scene = Scene::create();
	auto layer = MissionScene::create();
	scene->addChild(layer, 1);
	
	if (capture!=nullptr)
	{
		auto pause_background = Sprite::createWithTexture(capture->getSprite()->getTexture());
		pause_background->setFlipY(true);
		pause_background->setColor(Color3B::GRAY);
		pause_background->setPosition(Vec2(visible.width / 2 + origin.x, visible.height / 2 + origin.y));
		layer->addChild(pause_background, 0);
	}
	return scene;
}
bool MissionScene::init()
{
    if ( !Layer::init() )
    {
        return false;
    }
    visible = Director::getInstance()->getVisibleSize();
    origin = Director::getInstance()->getVisibleOrigin();
	ScrollMode::HUDlayer::getHUDlayer()->setMissionTip(false);
	this->_sceneCSB= CSLoader::createNode("MissionScene/MissionScene.csb");
	this->addChild(_sceneCSB, 1);
	
	_returnBtn=ShakeButton::create(JsonParser::getJsonString("MissionScene","Return",JsonParser::JsonType::UserInterFace),
		JsonParser::fontType(), JsonParser::fontSize(14), Color4B::BLACK, "ui/Button2.png", 
	[](ShakeButton* btn)
	{
		AudioManagement::getInstance()->play("sound/scrollClose.mp3", AudioManagement::AudioType::Sound, false);
		Director::getInstance()->popScene([](Scene* scene)
		{
			return TransitionPageTurn::create(0.4f, scene, false);
		});
	});
	_returnBtn->setPosition(_sceneCSB->getChildByName("ReturnBtnPos")->getPosition());
	this->_sceneCSB->addChild(_returnBtn);

	_missionTitle = Label::createWithTTF(MissionMG::getInstance()->getCurrentMissionTitle(), JsonParser::fontType(), JsonParser::fontSize(24));
	_missionTitle->setPosition(_sceneCSB->getChildByName("MissionTtitlePos")->getPosition());
	_missionTitle->enableOutline(Color4B::BLACK, 1);
	_missionTitle->setColor(Color3B::WHITE);
	this->_sceneCSB->addChild(_missionTitle);

	_missionContent= Label::createWithTTF(MissionMG::getInstance()->getCurrentMissionContent(), JsonParser::fontType(), JsonParser::fontSize(16));
	_missionContent->setPosition(_sceneCSB->getChildByName("MissionContentPos")->getPosition());
	_missionContent->setAnchorPoint(Vec2(0, 1));
	_missionContent->setMaxLineWidth(200.f);
	_missionContent->setColor(Color3B::BLACK);
	this->_sceneCSB->addChild(_missionContent);

	std::string tipContent = JsonParser::getJsonString("MissionScene", "Tip", JsonParser::JsonType::UserInterFace);
	tipContent += MissionMG::getInstance()->getCurrentMissionTip();
	_missionTip= Label::createWithTTF(tipContent, JsonParser::fontType(), JsonParser::fontSize(15));
	_missionTip->setPosition(_sceneCSB->getChildByName("MissionTipPos")->getPosition());
	_missionTip->enableOutline(Color4B::GRAY, 1);
	_missionTip->setAnchorPoint(Vec2(0.5, 0));
	//_missionTip->setColor(Color3B(255,128,0));
	/*_missionTip->setColor(Color3B(255, 140, 0));*/
	/*_missionTip->setColor(Color3B(11, 155, 169));*/
	_missionTip->setColor(Color3B::WHITE);
	this->_sceneCSB->addChild(_missionTip);
	int targetAmount = MissionMG::getInstance()->getCurrentMissionTargetAmount();
	if (targetAmount != -1)
	{
		int currentAmount = atoi(JsonParser::getJsonString("Infor", "MissionTargetMount", JsonParser::JsonType::Save).c_str());
		char newStr[30];
		sprintf(newStr," %d / %d", currentAmount, targetAmount);
		_missionTip->setString(_missionTip->getString() + std::string(newStr));
	}
	
    return true;
}
void MissionScene::onEnterTransitionDidFinish()
{
	Layer::onEnterTransitionDidFinish();
	if (ScrollMode::HUDlayer::getHUDlayer() != nullptr)
	{
		ScrollMode::HUDlayer::getHUDlayer()->_isReplaceingScene = false;
	}
}