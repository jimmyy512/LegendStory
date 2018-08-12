#ifndef __MissionMG_H__
#define __MissionMG_H__

#include "cocos2d.h"

class MissionMG
{
public:
	enum class MainName
	{
		FirstInChiuanJen,
		FirstMeetZhangZhen,
		FirstMeetZhangQing,
		FirstMeetZhangHen,
		FirstMeetZhangYin,
		FirstMeetZhangFong,
		FirstMeetZhangWo,
		MasterFirstOrder,
		ZhangQingTip,
		FirstAttack,
		REFirstAttack,
		ZhangZhenZombieStart,
		GoToCheckCave,
		KillZombie,
		LeaveCave,
		NotTheEnd
	};
	enum class State
	{
		None,
		GoForestField,
		GoChiuanJen,
		GoChiuanJenMount,
		GoCave,
		GoLuoYangCity,
		TalkZhangQing,
		TalkZhangYin,
		TalkZhangZhen,
		TalkZhangHen,
		TalkZhangFong,
		TalkZhangWo,
		KillSmallThief,
		KillZombie
	};
public:
	static MissionMG* getInstance();
	MissionMG();
	//* 任務完成目標
	State getCurrentMissionCompleteState();
	//* 任務目標數量增加條件
	State getCurrentMissionAmountAddState();
	//* 任務目標數量
	int getCurrentMissionTargetAmount();
	//* 任務標題
	std::string getCurrentMissionTitle();
	//* 任務內容
	std::string getCurrentMissionContent();
	//* 任務提示
	std::string getCurrentMissionTip();
	//* 任務完成對話代號
	std::string getCurrentMissionDialogCode();
	//* 完成任務時,檢查上一個任務是否有獲得對應的獎勵。
	void checkCompleteMissionReward();
	void NextMission();
	void PrevMission();
	//* 傳入的狀態是任務目標的狀態時,回傳true,否則回傳false
	bool isCompleteMission(State newState);
	//* 獲取當前任務進度編號
	inline int getCurrentMissionTag() { return _currentMissionTag; };
private:
	//*將字串State 轉成 Enum State
	State switchStringStateToState(std::string stringState);
	//*將存檔中的任務ID讀出來後，轉換成給Json看的任務代號
	static std::string switchIDToName(int MissionID);
	static MissionMG* static_missionMG;
	int _currentMissionTag;
private:
};

#endif
