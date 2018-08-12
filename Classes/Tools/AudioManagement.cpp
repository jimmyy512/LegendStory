#include "AudioManagement.h"
#include "audio\include\AudioEngine.h"
#include "tools\JsonParser.h"
USING_NS_CC;
using namespace experimental;
using namespace MyTools;
AudioManagement * AudioManagement::getInstance()
{
	static AudioManagement* static_audioManagement = nullptr;
	if(static_audioManagement ==nullptr)
		static_audioManagement = new (std::nothrow)AudioManagement;
	return static_audioManagement;
}
int AudioManagement::play(const char * audioPath, AudioType audioType,bool isloop)
{
	float volume = 0;
	if (audioType == AudioManagement::AudioType::Music)
	{
		volume=atoi(JsonParser::getJsonString("Optional", "MusicVolume", JsonParser::JsonType::Save).c_str())/100.f;
	}
	else if (audioType == AudioManagement::AudioType::Sound)
	{
		volume = atoi(JsonParser::getJsonString("Optional", "SoundVolume", JsonParser::JsonType::Save).c_str())/100.f;
	}
	int audioId = AudioEngine::play2d(audioPath, isloop, volume);
	_audios.insert(std::pair<int, AudioType>(audioId,audioType));
	AudioEngine::setFinishCallback(audioId, [&](int id, std::string path)
	{//當其中一首音樂播完時，就移出當前撥放中的清單
		std::map<int, AudioType>::iterator it;
		it = _audios.find(id);
		if (it != _audios.end())
		{
			_audios.erase(id);
		}
		else
		{
			CCLOG("warring! cant find audios id");
		}
	});
	return audioId;
}

void AudioManagement::updateNewAudioVolume()
{
	float musicVolume = atoi(JsonParser::getJsonString("Optional", "MusicVolume", JsonParser::JsonType::Save).c_str());
	float soundVolume = atoi(JsonParser::getJsonString("Optional", "SoundVolume", JsonParser::JsonType::Save).c_str());
	for (auto it = _audios.begin();it != _audios.end();it++)
	{
		if (AudioType::Music == it->second)
			AudioEngine::setVolume(it->first, musicVolume/100);
		if (AudioType::Sound == it->second)
			AudioEngine::setVolume(it->first, soundVolume/100);
	}
}

void AudioManagement::pauseAllAudio()
{
	for (auto it = _audios.begin();it != _audios.end();it++)
	{
		AudioEngine::pause(it->first);
	}
}

void AudioManagement::resumeAllAudio()
{
	for (auto it=_audios.begin();it!=_audios.end();it++)
	{
		AudioEngine::resume(it->first);
	}
}

void AudioManagement::clearAudioManagement()
{
	AudioEngine::stopAll();
	_audios.erase(_audios.begin(), _audios.end());
}

void AudioManagement::stopAudioFromID(int ID)
{
	for (auto it = _audios.begin();it != _audios.end();it++)
	{
		if (it->first == ID)
		{
			AudioEngine::stop(ID);
			_audios.erase(it);
			break;
		}
	}
}