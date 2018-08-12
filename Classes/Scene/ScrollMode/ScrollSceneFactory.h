#ifndef __GameScene_SCENE_H__
#define __GameScene_SCENE_H__
#include "cocos2d.h"
#include "cocos-ext.h"
#include "DefinitionScrollMode.h"
#include "UI\ScrollMode\Dialog.h"
#include "UI\DamageMG.h"
#include "Scene\SplashSceneFactory.h"
#include "Monster\ScrollMode\ScrollMonsterFactory.h"
NS_SCROLLMODE_BEGAN
class ScrollMap;
class HUDlayer;
class MainCharacter;
class TransferPoint;
class MapNameTip;
class ScrollMonsterFactory;
#define NoprevCharacterPos Vec2(-1000,-1000)
class ScrollSceneFactory
{
public:
	enum class SceneName
	{
		LuoYangCityScene,
		ForestFieldScene,
		ChiuanJenMountScene,
		ChiuanJenScene,
		CaveScene,
		TestScene
	};
	static cocos2d::Scene* createScene(SceneName sceneName);
	//*儲存主角上一張地圖的傳送點位子，用來計算到下一張地圖時，應該在哪一個傳送點出現。
	static cocos2d::Vec2 static_prevCharacterPos;
};

class ScrollScene : public cocos2d::Layer
{
public:
	enum
	{
		WEATHER_ZORDER,
		GAMEMAP_ZORDER,
		DROP_ITEM_ZORDER,
		MONSTER_ZORDER,
		MAINCHARACTER_ZORDER,
		NPC_ZORDER,
		NPC2_ZORDER,
		Control_BUTTON_ZORDER,
		ShowMonsterOnScreenLabel,
		ShowDamagedNum_ZORDER,
		DIALOG_ZORDER
	};
public:
	cocos2d::PhysicsWorld* _physicsWorld;
	ScrollScene();
	virtual ~ScrollScene();
	static ScrollScene* getScrollScene();
	void ShakeCamera(float duration,float degree);
	//*子類建構子完，CREATE_FUNC會呼叫init()函數
	//*不要去重載他，除非你明白你在幹甚麼。
	bool init();
	virtual void ReCheckForceDialog(bool isInitCall)=0;
public:
	//*鏡頭高度
	int cameraHeight = -60;
	DamageMG* _damageMG;
	ScrollMap* _gameMap;
	MainCharacter* _maincharacter;
	//*主角是否可以跳下,給ScrollMap判斷
	bool _canCharacterJumpDown;
	//*判斷有沒有其他地方強制清除音樂,如果有的話此類就不要再清除
	bool _ClearAudioManagement;
protected:
	cocos2d::Size visible;
	cocos2d::Vec2 origin;
	cocos2d::Vec2 _characterBornPos;
	//*場景上的傳送點
	std::vector<TransferPoint*> _transferPoints;
	HUDlayer* _hudlayer;
	//*場景上怪物種類
	std::vector<ScrollMonsterFactory::MonsterName>_monterTypes;
	MissionMG::State _missionState;
protected:
	void UpdateCamera();
	//*天氣模組
	void addWeatherModule(int weatherMinY,int weatherMaxY);
	void addTransferPoint(int x, int y, SplashSceneFactory::preLoadScene nextScene);
	//*與這張地圖最近的是哪一個傳送點
	cocos2d::Vec2 checkCharacterLastMapPos();
	//*NPC Zorder rand,will behind the character or not.
	inline int NPCzorderRand() { return rand() % 2 ? NPC_ZORDER : NPC2_ZORDER; };
	//*多少秒後彈出dialog
	void forceDialog(MissionMG::MainName WhichMissionState, std::string dialogCode, float delay);
	//*傳入x y isBigger如果為false 就會看主角pos是不是小於傳入的Pos 如果條件達到就會彈出dialog
	void forceDialogWithCharacterPos(MissionMG::MainName WhichMissionState, std::string dialogCode, cocos2d::Vec2 pos,bool isBigger);
private:
	//*要判斷玩家操作由上往下滑,第一個觸摸的pos
	cocos2d::Vec2 _firstTouchPos;
	//*主角是否可以跳下,自己內部判斷
	bool _canJumpDown;
	static ScrollScene* static_ScrollScene;
	MapNameTip* _mapNameTip;
	bool _isAddMonsterModule;
	//*是否到達地圖邊界
	bool _isOnMaplimit;
	int _weatherMinY;
	int _weatherMaxY;
	int _forceDialogSchduleCount;
private:
	//*增加一朵雲
	//*isFirstTime為true雲出生位子為隨機
	//*isFirstTime為false時雲出生位子是在地圖的右端
	void addOneCloud(bool isFirstTime);
	//*場景剛生成時，根據主角座標初始化鏡頭位子
	void initCamera();
	//*檢查主角有沒有在上一張地圖的位子，如果有找到位子，根據位子去換算上一張地圖的傳送點
	void updateHUDLayerWithTransferPoint();
	//*根據主角出生點,來判斷主角該面對哪個方向
	void fixMainCharacterFaceDirection();
	cocos2d::Vec2 CheckMissionStateAndSetCharacterBornPos();
};
class LuoYangScene :public ScrollScene
{
public:
	static cocos2d::Scene* createScene();
	CREATE_FUNC(LuoYangScene);
	LuoYangScene();
	void ReCheckForceDialog(bool isInitCall);
};
class ChiuanJenMountScene :public ScrollScene
{
public:
	static cocos2d::Scene* createScene();
	CREATE_FUNC(ChiuanJenMountScene);
	ChiuanJenMountScene();
	void ReCheckForceDialog(bool isInitCall);
};
class ChiuanJenScene :public ScrollScene
{
public:
	static cocos2d::Scene* createScene();
	CREATE_FUNC(ChiuanJenScene);
	ChiuanJenScene();
	void ReCheckForceDialog(bool isInitCall);
};
class ForestFieldScene :public ScrollScene
{
public:
	static cocos2d::Scene* createScene();
	CREATE_FUNC(ForestFieldScene);
	ForestFieldScene();
	void ReCheckForceDialog(bool isInitCall);
};
class CaveScene :public ScrollScene
{
public:
	static cocos2d::Scene* createScene();
	CREATE_FUNC(CaveScene);
	void ReCheckForceDialog(bool isInitCall);
	CaveScene();
};
class TestScene :public ScrollScene
{
public:
	static cocos2d::Scene* createScene();
	CREATE_FUNC(TestScene);
	void ReCheckForceDialog(bool isInitCall);
	TestScene();
};
NS_SCROLLMODE_END
#endif 
