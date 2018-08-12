#ifndef __AudioManagement_H__
#define __AudioManagement_H__

#include "cocos2d.h"
class AudioManagement
{
public:
	enum class AudioType
	{
		Music,
		Sound
	};
public:
	static AudioManagement* getInstance();
	int play(const char* audioPath, AudioType audioType,bool isloop=false);
	//*把播放中的音頻，根據新的音量設置來更新，新的聲音大小。
	void updateNewAudioVolume();
	//*暫停所有播放中的音頻
	void pauseAllAudio();
	//*復原所有暫停中的音頻
	void resumeAllAudio();
	//*換場景時，能把所有聲音停止，並清除紀錄
	void clearAudioManagement();
	//*傳入音樂ID,並停止撥放
	void stopAudioFromID(int ID);
private:
	static AudioManagement* static_audioManagement;
	std::map<int, AudioType> _audios;
};

#endif
