#include "MissionMG.h"
#include "Tools\JsonParser.h"
#include "tools\AudioManagement.h"
#include "UI\ScrollMode\HUDlayer.h"
#include "RPGTools\RPGTool.h"
#include "Scene\ScrollMode\ScrollSceneFactory.h"
USING_NS_CC;
using namespace MyTools;
MissionMG* MissionMG::static_missionMG = nullptr;
MissionMG* MissionMG::getInstance()
{
	if (static_missionMG == nullptr)
		static_missionMG = new(std::nothrow)MissionMG;
	return static_missionMG;
}

MissionMG::MissionMG()
{
	_currentMissionTag = atoi(JsonParser::getJsonString("Infor", "Mission", JsonParser::JsonType::Save).c_str());
}

void MissionMG::checkCompleteMissionReward()
{
	if ((_currentMissionTag-1) == (int)MissionMG::MainName::REFirstAttack)
	{
		MyTools::JsonParser::AddItemToSave((int)RPGTool::ItemID::Weapon_Shadow, 5);
	}
}

void MissionMG::NextMission()
{
	_currentMissionTag =atoi(JsonParser::getJsonString("Infor", "Mission",JsonParser::JsonType::Save).c_str());
	if (_currentMissionTag == (int)MissionMG::MainName::NotTheEnd)
	{
		return;
	}
	char tmpStr[10];
	sprintf(tmpStr, "%d", ++_currentMissionTag);
	JsonParser::setSave("Infor", "Mission",tmpStr);
	if (ScrollMode::HUDlayer::getHUDlayer() != nullptr)
	{
		ScrollMode::HUDlayer::getHUDlayer()->setMissionTip(true);
		this->checkCompleteMissionReward();
	}
	AudioManagement::getInstance()->play("sound/missionTip.mp3", AudioManagement::AudioType::Sound, false);
	
	if (ScrollMode::ScrollScene::getScrollScene() != nullptr)
	{//重新檢查場景需不需要強制彈出視窗
		ScrollMode::ScrollScene::getScrollScene()->ReCheckForceDialog(false);
	}
}

void MissionMG::PrevMission()
{
	_currentMissionTag = atoi(JsonParser::getJsonString("Infor", "Mission", JsonParser::JsonType::Save).c_str());
	if (_currentMissionTag == (int)MissionMG::MainName::FirstInChiuanJen)
	{
		return;
	}
	char tmpStr[10];
	sprintf(tmpStr, "%d", --_currentMissionTag);
	JsonParser::setSave("Infor", "Mission", tmpStr);
	if (ScrollMode::HUDlayer::getHUDlayer() != nullptr)
		ScrollMode::HUDlayer::getHUDlayer()->setMissionTip(true);
	AudioManagement::getInstance()->play("sound/missionTip.mp3", AudioManagement::AudioType::Sound, false);
}

bool MissionMG::isCompleteMission(State newState)
{
	if (newState == MissionMG::getCurrentMissionCompleteState())
	{
		return true;
	}
	else
	{
		return false;
	}
}

std::string MissionMG::switchIDToName(int MissionID)
{
	switch ((MissionMG::MainName)MissionID)
	{
	case MissionMG::MainName::FirstInChiuanJen:
		return "FirstInChiuanJen";
		break;
	case MissionMG::MainName::FirstMeetZhangZhen:
		return "FirstMeetZhangZhen";
		break;
	case MissionMG::MainName::FirstMeetZhangQing:
		return "FirstMeetZhangQing";
		break;
	case MissionMG::MainName::FirstMeetZhangHen:
		return "FirstMeetZhangHen";
		break;
	case MissionMG::MainName::FirstMeetZhangYin:
		return "FirstMeetZhangYin";
		break;
	case MissionMG::MainName::FirstMeetZhangFong:
		return "FirstMeetZhangFong";
		break;
	case MissionMG::MainName::FirstMeetZhangWo:
		return "FirstMeetZhangWo";
		break;
	case MissionMG::MainName::MasterFirstOrder:
		return "MasterFirstOrder";
		break;
	case MissionMG::MainName::ZhangQingTip:
		return "ZhangQingTip";
		break;
	case MissionMG::MainName::FirstAttack:
		return "FirstAttack";
		break;
	case MissionMG::MainName::REFirstAttack:
		return "REFirstAttack";
		break;
	case MissionMG::MainName::ZhangZhenZombieStart:
		return "ZhangZhenZombieStart";
		break;
	case MissionMG::MainName::GoToCheckCave:
		return "GoToCheckCave";
		break;
	case MissionMG::MainName::KillZombie:
		return "KillZombie";
		break;
	case MissionMG::MainName::LeaveCave:
		return "LeaveCave";
		break;
	case MissionMG::MainName::NotTheEnd:
		return "NotTheEnd";
		break;
	default:
		throw "in MissionMG switchIDToName function no this condition.";
	}
}

MissionMG::State MissionMG::switchStringStateToState(std::string stringState)
{
	if (stringState == "GoCave")
		return State::GoCave;
	else if (stringState == "GoChiuanJen")
		return State::GoChiuanJen;
	else if (stringState == "GoChiuanJenMount")
		return State::GoChiuanJenMount;
	else if (stringState == "GoForestField")
		return State::GoForestField;
	else if (stringState == "GoLuoYangCity")
		return State::GoLuoYangCity;
	else if (stringState == "TalkZhangQing")
		return State::TalkZhangQing;
	else if (stringState == "TalkZhangZhen")
		return State::TalkZhangZhen;
	else if (stringState == "TalkZhangYin")
		return State::TalkZhangYin;
	else if (stringState == "TalkZhangHen")
		return State::TalkZhangHen;
	else if (stringState == "TalkZhangFong")
		return State::TalkZhangFong;
	else if (stringState == "TalkZhangWo")
		return State::TalkZhangWo;
	else if (stringState == "KillSmallThief")
		return State::KillSmallThief;
	else if (stringState == "KillZombie")
		return State::KillZombie;
	else if (stringState == "Impossible")
		return State::None;
	else if (stringState == "None")
		return State::None;
	else
	{
		throw "in MissionMG switchStringStateToState no this condition";
	}
}

MissionMG::State MissionMG::getCurrentMissionAmountAddState()
{
	return switchStringStateToState(JsonParser::getJsonString(switchIDToName(_currentMissionTag).c_str(), "AmountAddState", JsonParser::JsonType::Mission).c_str());
}

int MissionMG::getCurrentMissionTargetAmount()
{
	return atoi(JsonParser::getJsonString(switchIDToName(_currentMissionTag).c_str(), "Amount", JsonParser::JsonType::Mission).c_str());
}

std::string MissionMG::getCurrentMissionTitle()
{
	return JsonParser::getJsonString(switchIDToName(_currentMissionTag).c_str(), "Title", JsonParser::JsonType::Mission);
}

std::string MissionMG::getCurrentMissionContent()
{
	return JsonParser::getJsonString(switchIDToName(_currentMissionTag).c_str(), "Content", JsonParser::JsonType::Mission);
}

std::string MissionMG::getCurrentMissionTip()
{
	return JsonParser::getJsonString(switchIDToName(_currentMissionTag).c_str(), "Tip", JsonParser::JsonType::Mission);
}

std::string MissionMG::getCurrentMissionDialogCode()
{
	return JsonParser::getJsonString(switchIDToName(_currentMissionTag).c_str(), "DialogCode", JsonParser::JsonType::Mission);
}

MissionMG::State MissionMG::getCurrentMissionCompleteState()
{
	std::string StateString=JsonParser::getJsonString(switchIDToName(_currentMissionTag).c_str(), "State", JsonParser::JsonType::Mission);
	return switchStringStateToState(StateString);
}
