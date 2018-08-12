#include "ScrollSceneFactory.h"
#include "UI\ScrollMode\HUDlayer.h"
#include "UI\DamageMG.h"
#include "Map\ScrollMode\ScrollMap.h"
#include "Tools\AnimateTool.h"
#include "Tools\JsonParser.h"
#include "MainCharacter\MainCharacter.h"
#include "Monster\ScrollMode\ScrollMonsterFactory.h"
#include "Monster\ScrollMode\MonsterAI.h"
#include "Scene\ScrollMode\ScrollSceneObject.h"
#include "DebugLayer.h"
#include "Tools\JsonParser.h"
#include "Tools\AudioManagement.h"
#include "Npc\NPCFactory.h"
#include "Npc\NPC_AI.h"
#include "MissionMG.h"
USING_NS_CC;
USING_NS_CC_EXT;
using namespace MyTools;
using namespace cocos2d::experimental;
//#define SHOW_PHYSICS_LINE
#define Reset_physics
NS_SCROLLMODE_BEGAN
namespace
{
	//滑動最大X距離,超過就不算由上往下滑動
	const float const_SlideMaxX=50.0f;
	//最小要向下移多少距離才算是滑動
	const float const_SlideMinY = 80.0f;
}
cocos2d::Vec2 ScrollSceneFactory::static_prevCharacterPos = NoprevCharacterPos;
ScrollScene* ScrollScene::static_ScrollScene = nullptr;
ScrollScene::ScrollScene()
{
	static_ScrollScene = nullptr;
	_isAddMonsterModule = false;
	_isOnMaplimit = false;
	_canCharacterJumpDown = false;
	_ClearAudioManagement = false;
	_forceDialogSchduleCount = 0;
}
ScrollScene::~ScrollScene()
{
	if(!_ClearAudioManagement)
		AudioManagement::getInstance()->clearAudioManagement();
	NPC_AI::getInstance()->clearData();
	MonsterAI::getInstance()->clearData();
	static_ScrollScene = nullptr;
	MonsterAI::monsterAmount = 0;
}
cocos2d::Scene * ScrollSceneFactory::createScene(SceneName sceneName)
{
	Scene* newScene = nullptr;
	switch (sceneName)
	{
	case ScrollSceneFactory::SceneName::LuoYangCityScene:
		newScene = LuoYangScene::createScene();
		break;
		case ScrollSceneFactory::SceneName::ForestFieldScene:
		newScene = ForestFieldScene::createScene();
		break;
	case ScrollSceneFactory::SceneName::ChiuanJenMountScene:
		newScene = ChiuanJenMountScene::createScene();
		break;
	case ScrollSceneFactory::SceneName::ChiuanJenScene:
		newScene = ChiuanJenScene::createScene();
		break;
	case ScrollSceneFactory::SceneName::CaveScene:
		newScene = CaveScene::createScene();
		break;
	case ScrollSceneFactory::SceneName::TestScene:
		newScene = TestScene::createScene();
		break;
	default:
		CCAssert(false, "createScene fail");
	}
	return newScene;
}
bool ScrollScene::init()
{
	if (!Layer::init())
	{
		return false;
	}
	static_ScrollScene = this;
	visible = Director::getInstance()->getVisibleSize();
	origin = Director::getInstance()->getVisibleOrigin();
	//檢查有沒有dialog需要彈出
	this->ReCheckForceDialog(true);
	//鏡頭初始化
	initCamera();
	//增加傷害數字圖層
	this->_damageMG = DamageMG::create();
	this->addChild(this->_damageMG,ScrollScene::ShowDamagedNum_ZORDER);
	//地圖名子提示
	_mapNameTip = MapNameTip::create(JsonParser::getJsonString("MapNameTip",_gameMap->_mapName.c_str(),
		JsonParser::JsonType::UserInterFace));
	_mapNameTip->setPosition(visible.width / 2+origin.x, visible.height / 5*4.2+origin.y);
	_hudlayer->addChild(_mapNameTip, 1000);
	//修正主角生成時面對的方向
	fixMainCharacterFaceDirection();
	//把所有傳送點加入到地圖中
	for (auto it : this->_transferPoints)
	{
		_gameMap->addChild(it, 1000);
	}

	//手指往下滑,讓主角跳下事件
	auto listener = EventListenerTouchOneByOne::create();
	listener->onTouchBegan = [&](Touch* touch,Event* event)
	{
		_firstTouchPos = touch->getLocation();
		_canJumpDown = true;
		return true;
	};
	listener->onTouchMoved = [&](Touch* touch, Event* event)
	{
		if (abs((touch->getLocation().x - this->_firstTouchPos.x)) > const_SlideMaxX)
		{
			_canJumpDown = false;
		}
	};
	listener->onTouchEnded = [&](Touch* touch, Event* event)
	{
		if (_canJumpDown &&
			abs((touch->getLocation().x - this->_firstTouchPos.x) < const_SlideMaxX) &&
			(this->_firstTouchPos.y-touch->getLocation().y) > const_SlideMinY &&
			!_maincharacter->status._isJump)
		{
			_canCharacterJumpDown = true;
			NotificationCenter::getInstance()->postNotification("Fall", (Ref*)"Fall");
			this->scheduleOnce([this](float dt) {
				this->_canCharacterJumpDown = false;
			},0.6f, "_canCharacterJumpDownUpdate");
		}
		else
		{
			this->_canCharacterJumpDown = false;
		}
	};
	Director::getInstance()->getEventDispatcher()->addEventListenerWithSceneGraphPriority(listener, this);

	this->schedule([this](float dt)
	{
#ifdef Reset_physics
		for (int i = 0;i < 3;i++)
			_physicsWorld->step(1 / 140.f);
#endif
		this->UpdateCamera();
	}, 0.02f, "GameSceneUpdate");

	this->schedule([this](float dt)
	{
		if (_mapNameTip != nullptr)
		{
			_mapNameTip->setOpacity(_mapNameTip->getOpacity() - 1);
			if (_mapNameTip->getOpacity() < 10)
			{
				_mapNameTip->removeFromParent();
				_mapNameTip = nullptr;
			}
		}
	}, 0.05f, "GameSceneUpdate2");

	this->schedule([this](float dt)
	{
		//如果有子類有添加天氣模組
		if (_isAddMonsterModule)
		{
			this->addOneCloud(false);
		}
	}, 5.f, "ScrollSceneCloutUpdate");

	this->schedule([this](float dt)
	{
		NPC_AI::getInstance()->updateAI();
	}, 0.2f, "NPC_AI_Update");

	this->schedule([this](float dt)
	{
		NPC_AI::getInstance()->updatePos();
	}, 0.02f, "NPC_Pos_update");

	this->schedule([this](float dt)
	{
		this->updateHUDLayerWithTransferPoint();
	}, 0.2f, "TransferToCharacter_update");

	if (JsonParser::getJsonString("Debug", "ForceAddHPMP", JsonParser::JsonType::Save) == "1")
	{//要在開發選單中勾選自動加血 才會啟動
		this->schedule([this](float dt)
		{
			this->_maincharacter->addHPMPToFull();
		}, 0.5f, "addFullHPMPHack");
	}

	
	

	//如果子類有增加怪物種類,代表此場景需生成怪物
	if (_monterTypes.size() != 0)
	{
		Director::getInstance()->getScheduler()->schedule([this](float dt)
		{
			MonsterAI::getInstance()->updateMonsterAI();

		}, this, 0.1f, !this->isRunning(), "MonsterAIUpdate");
		Director::getInstance()->getScheduler()->schedule([this](float dt)
		{
			MonsterAI::getInstance()->updateMonsterPosition();
		}, this, 0.02f, !this->isRunning(), "MonsterPositionUpdate");

		//generate monster
		Director::getInstance()->getScheduler()->schedule([&](float dt) {
			if (MonsterAI::monsterAmount > 5)
				return;
			auto whichMonster = rand() % _monterTypes.size();
			auto monster = ScrollMonsterFactory::createMonster(_monterTypes[whichMonster],false);
			int mapMaxX = ((int)_gameMap->getMapRect().size.width) - 100;
			int mapMinY = 100;
			auto generateXpos=rand() % (mapMaxX - mapMinY + 1) + mapMinY;
			monster->setPosition(generateXpos + origin.x, visible.height / 3 + origin.y);
			monster->setFaceDirection(rand() % 2);
			MonsterAI::getInstance()->addToMonsters(monster);
			_gameMap->addChild(monster, MONSTER_ZORDER);

		}, this, 4.0f, !this->isRunning(), "createMonsters");

	}

	return true;
}
void ScrollScene::UpdateCamera()
{
	if (_maincharacter != nullptr)
	{
		Vec2 mainCharacterPos = MainCharacter::getMainCharacter()->getPosition();
		if (mainCharacterPos.x < this->visible.width / 2 ||
			mainCharacterPos.x > this->_gameMap->getMapRect().size.width - visible.width / 2)
		{//主角已到達地圖邊界 只更新y軸鏡頭
			_isOnMaplimit = true;
			if (mainCharacterPos.x < this->visible.width / 2)
			{
				this->setPosition(0, -mainCharacterPos.y - cameraHeight);
			}
			else
			{
				this->setPosition(-this->_gameMap->getMapRect().size.width+this->visible.width, -mainCharacterPos.y - cameraHeight);
			}
			this->_gameMap->_parallaxNode->setPositionY(this->getPositionY()/2);
			if (_hudlayer != nullptr)
				_hudlayer->updateLayerWithScenePos(this->getPositionX(), this->getPositionY());
		}
		else 
		{//主角仍在地圖範圍內正常更新鏡頭
			_isOnMaplimit = false;
			this->setPosition(-mainCharacterPos.x+visible.width/2,-mainCharacterPos.y- cameraHeight);
			if (_hudlayer != nullptr)
			{
				_hudlayer->updateLayerWithScenePos(this->getPositionX(), this->getPositionY());
				this->_gameMap->_parallaxNode->setPosition(this->getPosition()/2);
			}
		}
	}
}

void ScrollScene::initCamera()
{
	Vec2 mainCharacterPos = MainCharacter::getMainCharacter()->getPosition();
	if (mainCharacterPos.x > this->_gameMap->getMapRect().size.width - visible.width / 2)
	{//出生點最右邊邊界
		this->setPosition(-this->_gameMap->getMapRect().size.width + visible.width, -mainCharacterPos.y - cameraHeight);
	}
	else if (mainCharacterPos.x < this->visible.width / 2)
	{//出生點最左邊邊界
		this->setPosition(0, -mainCharacterPos.y - cameraHeight);
	}
	else
		this->setPosition(-mainCharacterPos.x + visible.width / 2, -mainCharacterPos.y - cameraHeight);
	_hudlayer->updateLayerWithScenePos(this->getPositionX(), this->getPositionY());
	this->_gameMap->_parallaxNode->setPosition(this->getPosition() / 2);
}
ScrollScene* ScrollScene::getScrollScene()
{
	if (static_ScrollScene == nullptr)
		return nullptr;
	else
		return static_ScrollScene;
}
void ScrollScene::forceDialog(MissionMG::MainName WhichMissionState,std::string dialogCode ,float delay)
{
	if ((int)MissionMG::getInstance()->getCurrentMissionTag() == (int)WhichMissionState)
	{
		_forceDialogSchduleCount++;
		char schduleName[50];
		sprintf(schduleName, "forceDialog%d", _forceDialogSchduleCount);
		this->scheduleOnce([&, dialogCode](float dt) {
			if (HUDlayer::getHUDlayer()->_isReplaceingScene)
			{
				return;
			}
			HUDlayer::getHUDlayer()->_isReplaceingScene = true;
			auto capture = RenderTexture::create(visible.width + origin.x, visible.height + origin.y);
			capture->begin();
			this->getParent()->visit();
			capture->end();
			auto scene = Dialog::createScene(capture,NPCFactory::create(NPCFactory::NPCName::ZhangQing,true), dialogCode.c_str());
			Director::getInstance()->pushScene(TransitionProgressRadialCW::create(0.3f, scene));
		}, delay, schduleName);
	}
}

void ScrollScene::forceDialogWithCharacterPos(MissionMG::MainName WhichMissionState, std::string dialogCode, Vec2 pos, bool isBigger)
{
	if ((int)MissionMG::getInstance()->getCurrentMissionTag() == (int)WhichMissionState)
	{
		_forceDialogSchduleCount++;
		char schduleName[50] = {};
		sprintf(schduleName, "forceDialog%d", _forceDialogSchduleCount);
		std::string schduleNameStr = std::string(schduleName);
		this->schedule([=](float dt) {
			bool condition=false;
			if (isBigger)
			{
				if (_maincharacter->getPosition().x > pos.x)
					condition = true;
				else
					condition = false;
			}
			else
			{
				if (_maincharacter->getPosition().x < pos.x)
					condition = true;
				else
					condition = false;
			}
			if (condition)
			{
				if (HUDlayer::getHUDlayer()->_isReplaceingScene)
				{
					return;
				}
				HUDlayer::getHUDlayer()->_isReplaceingScene = true;
				auto capture = RenderTexture::create(visible.width + origin.x, visible.height + origin.y);
				capture->begin();
				this->getParent()->visit();
				capture->end();
				auto scene = Dialog::createScene(capture, NPCFactory::create(NPCFactory::NPCName::ZhangQing, true), dialogCode.c_str());
				Director::getInstance()->pushScene(TransitionProgressRadialCW::create(0.3f, scene));
				this->unschedule(schduleNameStr);
			}
		}, 0.1f, schduleName);
	}
}
void ScrollScene::ShakeCamera(float duration,float degree)
{
	if (this->_isOnMaplimit == false)
	{//沒到達地圖邊界時，可以完整晃動
		auto sequence = Sequence::create(
			MoveBy::create(duration, Vec2(degree, 0)),
			MoveBy::create(duration, Vec2(0, -degree)),
			MoveBy::create(duration, Vec2(0, degree)),
			MoveBy::create(duration, Vec2(-degree, 0)),
			NULL);
		this->runAction(sequence);
	}
	else
	{//到達地圖邊界時，只能上下晃動
		auto sequence = Sequence::create(
			MoveBy::create(duration, Vec2(0, -degree)),
			MoveBy::create(duration, Vec2(0, degree)),
			MoveBy::create(duration, Vec2(0, -degree)),
			MoveBy::create(duration, Vec2(0, degree)),
			NULL);
		this->runAction(sequence);
	}
}

void ScrollScene::addWeatherModule(int weatherMinY, int weatherMaxY)
{
	_isAddMonsterModule = true;
	_weatherMinY = weatherMinY;
	_weatherMaxY = weatherMaxY;
	for (int i = 0;i < 10;i++)
	{
		addOneCloud(true);
	}
}
void ScrollScene::addTransferPoint(int x, int y, SplashSceneFactory::preLoadScene nextScene)
{
	auto testTransfer = TransferPoint::create(nextScene);
	testTransfer->setPosition(x, y);
	_transferPoints.push_back(testTransfer);
}
void ScrollScene::addOneCloud(bool isFirstTime)
{
	char weatherFilePath[50];
	float randScaleX = (rand() % (60 - 1) + 60);
	float randScaleY = (rand() % (60 - 1) + 60);
	randScaleX /= 100;
	randScaleY /= 100;
	int randNum = rand() % 4 + 1;
	sprintf(weatherFilePath, "sprite/whiteCloud%d.png", randNum);
	Sprite* cloud = Cloud::create(weatherFilePath);
	int gameMapWidth = _gameMap->_firstMap->getBoundingBox().size.width;
	Vec2 cloudPos;
	if (isFirstTime)
		cloudPos.x = rand() % (gameMapWidth - 200 + 1) + 100;
	else
		cloudPos.x = gameMapWidth + 200;
	cloudPos.y = rand() % (_weatherMaxY - _weatherMinY + 1) + _weatherMinY;
	cloud->setPosition(cloudPos);
	cloud->setScale(randScaleX, randScaleY);//0.6~1.2
	//圖層隨機
	this->addChild(cloud, rand() % 4);//0~3
}
void ScrollScene::updateHUDLayerWithTransferPoint()
{
	bool isCharacterContainsTransferPoint = false;
	for (auto it : this->_transferPoints)
	{
		if (it->getBoundingBox().containsPoint(this->_maincharacter->getPosition()))
		{
			isCharacterContainsTransferPoint = true;
			HUDlayer::getHUDlayer()->_transferPoint = it;
		}
	}
	if (isCharacterContainsTransferPoint == true)
	{
		HUDlayer::getHUDlayer()->_normalSkillState.Transfer = true;
	}
	else
	{
		HUDlayer::getHUDlayer()->_normalSkillState.Transfer = false;
	}
}
cocos2d::Vec2 ScrollScene::checkCharacterLastMapPos()
{
	auto prevCharacterPos = ScrollSceneFactory::static_prevCharacterPos;
	if (prevCharacterPos != NoprevCharacterPos && _transferPoints.size()!=0)
	{
		float pretendMaxDistance=0;
		TransferPoint* pretendMaxDistanceTransferPoint=nullptr;
		for (auto point : _transferPoints)
		{
			float currentDistance = prevCharacterPos.distance(point->getPosition());
			if (currentDistance > pretendMaxDistance)
			{
				pretendMaxDistance = currentDistance;
				pretendMaxDistanceTransferPoint = point;
			}
		}
		auto characterBornPos = CheckMissionStateAndSetCharacterBornPos();
		if (characterBornPos != Vec2(-1, -1))
		{
			return characterBornPos;
		}
		else
		{
			if (pretendMaxDistanceTransferPoint == nullptr)
				return Vec2(480, 120);
			else
				return pretendMaxDistanceTransferPoint->getPosition();
		}
	}
	else
	{
		auto characterBornPos = CheckMissionStateAndSetCharacterBornPos();
		if (characterBornPos != Vec2(-1, -1))
		{
			return characterBornPos;
		}
		else
		{
			return Vec2(480, 90);
		}	
	}
}
void ScrollScene::fixMainCharacterFaceDirection()
{
	if (_characterBornPos.x > _gameMap->getMapRect().size.width / 2)
	{
		_maincharacter->reviseDirection(true);
	}
	else
	{
		_maincharacter->reviseDirection(false);
	}
}
Vec2 ScrollScene::CheckMissionStateAndSetCharacterBornPos()
{
	if ((int)MissionMG::getInstance()->getCurrentMissionTag() == (int)MissionMG::MainName::FirstInChiuanJen)
		return Vec2(1850, 90);
	else
	{
		return Vec2(-1, -1);
	}
}

//@ForestFieldScene
Scene* ForestFieldScene::createScene()
{
	auto scene = Scene::createWithPhysics();
#ifdef SHOW_PHYSICS_LINE
	scene->getPhysicsWorld()->setDebugDrawMask(PhysicsWorld::DEBUGDRAW_ALL);
#endif
	scene->getPhysicsWorld()->setGravity(Vec2(0, -250));
	auto layer = ForestFieldScene::create();
	static_cast<ScrollScene*>(layer)->_physicsWorld = scene->getPhysicsWorld();
#ifdef Reset_physics
	layer->_physicsWorld->setAutoStep(false);
#endif
	scene->addChild(layer);
	return scene;
}
ForestFieldScene::ForestFieldScene()
{
	AudioManagement::getInstance()->play("music/Training.mp3", AudioManagement::AudioType::Music, true);

	this->_missionState = MissionMG::State::GoForestField;

	//判斷進入場景任務
	if (this->_missionState == MissionMG::getInstance()->getCurrentMissionCompleteState())
	{
		MissionMG::getInstance()->NextMission();
		JsonParser::setSave("Infor", "MissionTip", "1");
	}

	this->addTransferPoint(45, 77, SplashSceneFactory::preLoadScene::preLoadChiuanJen);
	//this->addTransferPoint(1955, 77, SplashSceneFactory::preLoadScene::preLoadLuoYangCity);


	_gameMap = ScrollMapFactory::createMap(ScrollMapFactory::MAP_NAME::ForestField);
	this->addChild(_gameMap, GAMEMAP_ZORDER);

	_characterBornPos = checkCharacterLastMapPos();

	_maincharacter = MainCharacter::create(_characterBornPos.x + origin.x, _characterBornPos.y + origin.y, false);
	_gameMap->addChild(_maincharacter, MAINCHARACTER_ZORDER);

	_hudlayer = HUDlayer::create(_maincharacter);
	this->addChild(_hudlayer, Control_BUTTON_ZORDER);

	auto testLabel = Label::create();
	testLabel->setPosition(visible.width / 2 + origin.x, visible.height / 2 + origin.y);
	testLabel->setTag(ShowMonsterOnScreenLabel);
	this->addChild(testLabel, 3);

	if((int)MissionMG::getInstance()->getCurrentMissionTag()>=(int)MissionMG::MainName::FirstAttack)
		this->_monterTypes.push_back(ScrollMonsterFactory::MonsterName::SmallThief);
	
}

void ForestFieldScene::ReCheckForceDialog(bool isInitCall)
{
	if (!isInitCall)
	{
		this->forceDialog(MissionMG::MainName::FirstAttack, "SmallThiefTalk", 0.1f);
		this->forceDialog(MissionMG::MainName::FirstInChiuanJen, "FirstInChiuanJenTip", 0.1f);
		this->forceDialog(MissionMG::MainName::FirstInChiuanJen, "FirstInChiuanJenCharacterTalk", 0.1f);
	}
	else
	{
		this->forceDialog(MissionMG::MainName::FirstAttack, "SmallThiefTalk", 2.1f);
		this->forceDialog(MissionMG::MainName::FirstInChiuanJen, "FirstInChiuanJenTip", 2.1f);
		this->forceDialog(MissionMG::MainName::FirstInChiuanJen, "FirstInChiuanJenCharacterTalk", 14.0f);
		this->forceDialogWithCharacterPos(MissionMG::MainName::FirstInChiuanJen, "AttackOptionTip", Vec2(250, 0), false);
	}
}

//@LuoYangScene
Scene* LuoYangScene::createScene()
{
	auto scene = Scene::createWithPhysics();
#ifdef SHOW_PHYSICS_LINE
	scene->getPhysicsWorld()->setDebugDrawMask(PhysicsWorld::DEBUGDRAW_ALL);
#endif
	scene->getPhysicsWorld()->setGravity(Vec2(0, -200));
	auto layer = LuoYangScene::create();
	static_cast<ScrollScene*>(layer)->_physicsWorld = scene->getPhysicsWorld();
#ifdef Reset_physics
	layer->_physicsWorld->setAutoStep(false);
#endif
	scene->addChild(layer);
	return scene;
}
LuoYangScene::LuoYangScene()
{
	AudioManagement::getInstance()->play("music/LuoYangCityMusic.mp3", AudioManagement::AudioType::Music, true);

	this->_missionState = MissionMG::State::GoLuoYangCity;

	//判斷進入場景任務
	if (this->_missionState == MissionMG::getInstance()->getCurrentMissionCompleteState())
	{
		MissionMG::getInstance()->NextMission();
		JsonParser::setSave("Infor", "MissionTip", "1");
	}

	this->addTransferPoint(45, 50, SplashSceneFactory::preLoadScene::preLoadForestField);

	_gameMap = ScrollMapFactory::createMap(ScrollMapFactory::MAP_NAME::LuoYangCity);
	this->addChild(_gameMap, GAMEMAP_ZORDER);

	

	_characterBornPos = checkCharacterLastMapPos();

	_maincharacter = MainCharacter::create(_characterBornPos.x + origin.x, _characterBornPos.y + origin.y, false);
	_gameMap->addChild(_maincharacter, 5);

	_hudlayer = HUDlayer::create(_maincharacter);
	this->addChild(_hudlayer, Control_BUTTON_ZORDER);

	addWeatherModule(460, 800);


}

void LuoYangScene::ReCheckForceDialog(bool isInitCall)
{
	if (!isInitCall)
	{

	}
	else
	{

	}
}

//@ChiuanJenMountScene
Scene* ChiuanJenMountScene::createScene()
{
	auto scene = Scene::createWithPhysics();
#ifdef SHOW_PHYSICS_LINE
	scene->getPhysicsWorld()->setDebugDrawMask(PhysicsWorld::DEBUGDRAW_ALL);
#endif
	scene->getPhysicsWorld()->setGravity(Vec2(0, -200));
	auto layer = ChiuanJenMountScene::create();
	static_cast<ScrollScene*>(layer)->_physicsWorld = scene->getPhysicsWorld();
#ifdef Reset_physics
	layer->_physicsWorld->setAutoStep(false);
#endif
	scene->addChild(layer);
	return scene;
}
ChiuanJenMountScene::ChiuanJenMountScene()
{
	AudioManagement::getInstance()->play("music/ChiuanJenMountMusic.mp3", AudioManagement::AudioType::Music, true);

	this->_missionState = MissionMG::State::GoChiuanJenMount;

	//判斷進入場景任務
	if (this->_missionState == MissionMG::getInstance()->getCurrentMissionCompleteState())
	{
		MissionMG::getInstance()->NextMission();
		JsonParser::setSave("Infor", "MissionTip", "1");
	}

	this->addTransferPoint(1460, 50, SplashSceneFactory::preLoadScene::preLoadChiuanJen);
	this->addTransferPoint(75, 500, SplashSceneFactory::preLoadScene::preLoadCave);

	_gameMap = ScrollMapFactory::createMap(ScrollMapFactory::MAP_NAME::ChiuanJenMount);
	this->addChild(_gameMap, GAMEMAP_ZORDER);

	_characterBornPos=checkCharacterLastMapPos();

	_maincharacter = MainCharacter::create(_characterBornPos.x + origin.x, _characterBornPos.y + origin.y, false);
	_gameMap->addChild(_maincharacter, MAINCHARACTER_ZORDER);

	_hudlayer = HUDlayer::create(_maincharacter);
	this->addChild(_hudlayer, Control_BUTTON_ZORDER);

	addWeatherModule(500, 900);

	

	
}

void ChiuanJenMountScene::ReCheckForceDialog(bool isInitCall)
{
	if (!isInitCall)
	{
		this->forceDialog(MissionMG::MainName::GoToCheckCave, "afterZhangZhenZombieStart", 0.1f);
	}
	else
	{
		this->forceDialog(MissionMG::MainName::GoToCheckCave, "afterZhangZhenZombieStart", 2.1f);
	}
}

//@ChiuanJenScene
Scene* ChiuanJenScene::createScene()
{
	auto scene = Scene::createWithPhysics();
#ifdef SHOW_PHYSICS_LINE
	scene->getPhysicsWorld()->setDebugDrawMask(PhysicsWorld::DEBUGDRAW_ALL);
#endif
	scene->getPhysicsWorld()->setGravity(Vec2(0, -200));
	auto layer = ChiuanJenScene::create();
	static_cast<ScrollScene*>(layer)->_physicsWorld = scene->getPhysicsWorld();
#ifdef Reset_physics
	layer->_physicsWorld->setAutoStep(false);
#endif
	scene->addChild(layer);
	return scene;
}
ChiuanJenScene::ChiuanJenScene()
{
	AudioManagement::getInstance()->play("music/ChiuanJen.mp3", AudioManagement::AudioType::Music, true);

	this->_missionState = MissionMG::State::GoChiuanJen;

	//判斷進入場景任務
	if (this->_missionState == MissionMG::getInstance()->getCurrentMissionCompleteState())
	{
		MissionMG::getInstance()->NextMission();
		JsonParser::setSave("Infor", "MissionTip", "1");
	}

	this->addTransferPoint(45, 75, SplashSceneFactory::preLoadScene::preLoadChiuanJenMount);
	this->addTransferPoint(1955, 75, SplashSceneFactory::preLoadScene::preLoadForestField);

	_gameMap = ScrollMapFactory::createMap(ScrollMapFactory::MAP_NAME::ChiuanJen);
	this->addChild(_gameMap, GAMEMAP_ZORDER);


	_characterBornPos = checkCharacterLastMapPos();

	_maincharacter = MainCharacter::create(_characterBornPos.x + origin.x, _characterBornPos.y + origin.y, false);
	_gameMap->addChild(_maincharacter, MAINCHARACTER_ZORDER);

	_hudlayer = HUDlayer::create(_maincharacter);
	this->addChild(_hudlayer, Control_BUTTON_ZORDER);
	

	addWeatherModule(700, 1000);

	_gameMap->addChild(NPCFactory::create(NPCFactory::NPCName::ZhangWo, false, Vec2(1800, 100)), this->NPCzorderRand());
	_gameMap->addChild(NPCFactory::create(NPCFactory::NPCName::ZhangFong, false, Vec2(1500, 100)), this->NPCzorderRand());
	_gameMap->addChild(NPCFactory::create(NPCFactory::NPCName::ZhangYin, false, Vec2(1200, 100)), this->NPCzorderRand());
	_gameMap->addChild(NPCFactory::create(NPCFactory::NPCName::ZhangHen, false, Vec2(900, 100)), this->NPCzorderRand());
	_gameMap->addChild(NPCFactory::create(NPCFactory::NPCName::ZhangQing, false, Vec2(600, 100)),this->NPCzorderRand());
	_gameMap->addChild(NPCFactory::create(NPCFactory::NPCName::ZhangZhen, false, Vec2(300, 100)), this->NPCzorderRand());
	
	
}

void ChiuanJenScene::ReCheckForceDialog(bool isInitCall)
{
	if (!isInitCall)
	{
		this->forceDialog(MissionMG::MainName::FirstMeetZhangZhen, "FallTip", 0.1f);
	}
	else
	{
		this->forceDialog(MissionMG::MainName::FirstMeetZhangZhen, "FallTip", 2.1f);
	}
}


//@CaveScene
Scene* CaveScene::createScene()
{
	auto scene = Scene::createWithPhysics();
#ifdef SHOW_PHYSICS_LINE
	scene->getPhysicsWorld()->setDebugDrawMask(PhysicsWorld::DEBUGDRAW_ALL);
#endif
	scene->getPhysicsWorld()->setGravity(Vec2(0, -200));
	auto layer = CaveScene::create();
	static_cast<ScrollScene*>(layer)->_physicsWorld = scene->getPhysicsWorld();
#ifdef Reset_physics
	layer->_physicsWorld->setAutoStep(false);
#endif
	scene->addChild(layer);
	return scene;
}

CaveScene::CaveScene()
{
	AudioManagement::getInstance()->play("music/CaveMusic.mp3", AudioManagement::AudioType::Music, true);

	this->_missionState = MissionMG::State::GoCave;

	//判斷進入場景任務
	if (this->_missionState == MissionMG::getInstance()->getCurrentMissionCompleteState())
	{
		MissionMG::getInstance()->NextMission();
		JsonParser::setSave("Infor", "MissionTip", "1");
	}

	this->addTransferPoint(1955, 72, SplashSceneFactory::preLoadScene::preLoadChiuanJenMount);

	_characterBornPos = checkCharacterLastMapPos();

	_gameMap = ScrollMapFactory::createMap(ScrollMapFactory::MAP_NAME::Cave);
	this->addChild(_gameMap, GAMEMAP_ZORDER);

	_maincharacter = MainCharacter::create(_characterBornPos.x + origin.x, _characterBornPos.y + origin.y, false);
	_gameMap->addChild(_maincharacter, MAINCHARACTER_ZORDER);

	_hudlayer = HUDlayer::create(_maincharacter);
	this->addChild(_hudlayer, Control_BUTTON_ZORDER);

	//此場景有怪物
	this->_monterTypes.push_back(ScrollMonsterFactory::MonsterName::Zombie);


}

void CaveScene::ReCheckForceDialog(bool isInitCall)
{
	if (!isInitCall)
	{
		this->forceDialog(MissionMG::MainName::KillZombie, "ZombieScream", 0.1f);
		this->forceDialog(MissionMG::MainName::LeaveCave, "FindSomeThing", 0.1f);
	}
	else
	{
		this->forceDialog(MissionMG::MainName::KillZombie, "ZombieScream", 2.1f);
		this->forceDialog(MissionMG::MainName::LeaveCave, "FindSomeThing", 2.1f);
	}
}

//@TestScene
Scene* TestScene::createScene()
{
	auto scene = Scene::createWithPhysics();
#ifdef SHOW_PHYSICS_LINE
	scene->getPhysicsWorld()->setDebugDrawMask(PhysicsWorld::DEBUGDRAW_ALL);
#endif
	scene->getPhysicsWorld()->setGravity(Vec2(0, -200));
	auto layer = TestScene::create();
	static_cast<ScrollScene*>(layer)->_physicsWorld = scene->getPhysicsWorld();
#ifdef Reset_physics
	layer->_physicsWorld->setAutoStep(false);
#endif
	scene->addChild(layer);
	return scene;
}

TestScene::TestScene()
{
	AudioManagement::getInstance()->play("music/getAmped.mp3", AudioManagement::AudioType::Music, true);

	_characterBornPos = checkCharacterLastMapPos();

	_maincharacter = MainCharacter::create(_characterBornPos.x + origin.x, _characterBornPos.y + origin.y, false);

	_hudlayer = HUDlayer::create(_maincharacter);
	this->addChild(_hudlayer, Control_BUTTON_ZORDER);

	_gameMap = ScrollMapFactory::createMap(ScrollMapFactory::MAP_NAME::TestMap);
	_gameMap->addChild(_maincharacter, MAINCHARACTER_ZORDER);
	this->addChild(_gameMap, GAMEMAP_ZORDER);


	_gameMap->addChild(NPCFactory::create(NPCFactory::NPCName::ZhangQing, false, Vec2(300, 75)), this->NPCzorderRand());


}
void TestScene::ReCheckForceDialog(bool isInitCall)
{
	if (!isInitCall)
	{

	}
	else
	{

	}
}
NS_SCROLLMODE_END


