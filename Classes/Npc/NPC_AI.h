#ifndef __NPC_AI__
#define __NPC_AI__

#include "cocos2d.h"
class NPC;
class NPC_AI 
{
public:
	static NPC_AI* static_NPC_AI;
public:
	static NPC_AI* getInstance();
	~NPC_AI();
	void addToAI(NPC* npc);
	void updateAI();
	void updateNPCLabelFontType();
	void updatePos();
	void clearData();
	std::vector<NPC*> _npcs;
};

#endif
