#include "NPC_AI.h"
#include "NPCFactory.h"
#include "tools\JsonParser.h"
USING_NS_CC;
using namespace MyTools;
NPC_AI* NPC_AI::static_NPC_AI = nullptr;
NPC_AI * NPC_AI::getInstance()
{
	if (static_NPC_AI == nullptr)
		static_NPC_AI = new(std::nothrow)NPC_AI;
	return static_NPC_AI;
}

NPC_AI::~NPC_AI()
{
	CC_SAFE_DELETE(static_NPC_AI);
}

void NPC_AI::clearData()
{
	_npcs.clear();
}

void NPC_AI::addToAI(NPC * npc)
{
	_npcs.push_back(npc);
}

void NPC_AI::updateAI()
{
	for (auto it = _npcs.begin();it != _npcs.end();it++)
	{
		(*it)->update();
	}
}
void NPC_AI::updateNPCLabelFontType()
{
	for (auto it = _npcs.begin();it != _npcs.end();it++)
	{
		TTFConfig ttfConfig(JsonParser::fontType(), JsonParser::fontSize(16), GlyphCollection::DYNAMIC);
		(*it)->_chatLabel->setTTFConfig(ttfConfig);
	}
}
void NPC_AI::updatePos()
{
	for (auto it = _npcs.begin();it != _npcs.end();it++)
	{
		static bool test = false;
		if (!test)
		{//可以在這邊對NPC下測試指令
			//(*it)->doIdle2Animation();
			//(*it)->setFaceDirection(false);
			//test = true;
		}
		if ((*it)->_isNpcCanMove)
		{
			if ((*it)->status._isWalk || (*it)->status._isRun)
			{
				if ((*it)->status._faceDirection)
				{
					if((*it)->status._isWalk)
						(*it)->setPositionX((*it)->getPositionX() - (*it)->_moveSpeed);
					else
						(*it)->setPositionX((*it)->getPositionX() - (*it)->_moveSpeed*1.5);
				}
				else
				{
					if ((*it)->status._isWalk)
						(*it)->setPositionX((*it)->getPositionX() + (*it)->_moveSpeed);
					else
						(*it)->setPositionX((*it)->getPositionX() + (*it)->_moveSpeed*1.5);
				}
				if (fabs((*it)->getPosition().x - (*it)->_prevPos.x) < 0.1f)
				{//撞牆了，轉換方向
					(*it)->setFaceDirection(!(*it)->status._faceDirection);
				}
				(*it)->_prevPos = (*it)->getPosition();
			}
		}
	}
}