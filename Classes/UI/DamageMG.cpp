#include "DamageMG.h"
USING_NS_CC;
bool DamageMG::init()
{
	if (!Layer::init())
		return false;
	visible = Director::getInstance()->getVisibleSize();
	origin = Director::getInstance()->getVisibleOrigin();
	this->setAnchorPoint(Vec2(0, 0));
	return true;
}
void DamageMG::showDamage(int damage, Direction direction,Vec2 monsterPos)
{
	Label* damageLabel = Label::createWithCharMap("ui/DamageNM.png", 24, 31, '0');
	damageLabel->setPosition(monsterPos.x, monsterPos.y+25);
	this->addChild(damageLabel);

	char tmp[20];
	sprintf(tmp, "%d", damage);
	damageLabel->setString(tmp);
	//傷害顯示 jump動畫
	//int JumpByPosX,JumpByPosY;
	//if (direction == Direction::ToLeft)
	//{
	//	JumpByPosX = -80;
	//	JumpByPosY = 50;
	//	JumpByPosX -= rand() % JumpByPosX - 20.f;
	//	JumpByPosY += rand() % JumpByPosY + 10.f;
	//}
	//else if (direction == Direction::ToRight)
	//{
	//	JumpByPosX = 80;
	//	JumpByPosY = 50;
	//	JumpByPosX += rand() % JumpByPosX + 20.f;
	//	JumpByPosY += rand() % JumpByPosY + 10.f;
	//}
	//else
	//	CCASSERT(false,"Direction error!");
	//auto jumpAction = JumpBy::create(1.5f, Vec2(JumpByPosX, JumpByPosY), 50, 1);

	int MoveActionXoffset;
	int MoveActionYoffset=150;
	MoveActionXoffset = rand() % 2 ? rand() % 25 : -rand() % 25;
	MoveActionYoffset = rand() % 2 ? rand() % 50 : -rand() % 10;
	damageLabel->setPositionX(damageLabel->getPositionX() + MoveActionXoffset);
	damageLabel->setPositionY(damageLabel->getPositionY() + MoveActionYoffset);

	auto moveAction = MoveBy::create(2.f,Vec2(0,150));
	auto callFunc = CallFunc::create(std::bind(&DamageMG::CallFunc,this, damageLabel));
	auto sequence = Sequence::create(moveAction, callFunc, NULL);
	damageLabel->runAction(sequence);
	damageLabel->runAction(FadeOut::create(1.5f));
}

void DamageMG::CallFunc(Label* damageLabel)
{
	damageLabel->removeFromParent();
}
