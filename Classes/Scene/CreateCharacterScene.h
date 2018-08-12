#ifndef _CreateCharacterScene__H__
#define _CreateCharacterScene__H__
#include "cocos2d.h"
#include "cocostudio/CocoStudio.h"
#include "ui/CocosGUI.h"
#include "DefinitionScrollMode.h"
#include "dragonBones\cocos2dx\CCDragonBonesHeaders.h"
NS_SCROLLMODE_BEGAN
class MainCharacter;
class CreateCharacterScene:public cocos2d::Layer
{
	enum
	{
		SWORD_BUTTON,
		KNIFE_BUTTON,
		BOXING_BUTTON,
		//character
		HAIR_OPTION,
		HEAD_OPTION,
		CLOTH_OPTION,
		PAINTS_OPTION,
		//direction
		LEFT_OPTION,
		RIGHT_OPTION,
		INPUTNAME_TEXTFIELD
	};
public:
	static cocos2d::Scene* createScene();
	void weaponChoose(cocos2d::Ref * sender, cocos2d::ui::Widget::TouchEventType type, int weaponTag);
	void OptionBtnClick(cocos2d::Ref * sender, cocos2d::ui::Widget::TouchEventType type, int optionTag,int directionOption);
	CREATE_FUNC(CreateCharacterScene);
private:
	//*點擊換裝時的計數作處理
	void selectCountMethod(int directionOption,int& typeCount);
	virtual void onExit();
private:
	virtual bool init();
	MainCharacter* _mainCharacter;
	int _hairSelectCount=0;
	int _headSelectCount = 0;
	int _clothSelectCount = 0;
	int _pantsSelectCount = 0;
	//dragonBones::DragonBonesData* _dragonBonesData;
	//dragonBones::Armature* _armature;
	//dragonBones::CCArmatureDisplay* _armatureDisplay;
	//dragonBones::CCFactory _factory;

	cocos2d::Label* _HairOptionLabel;
	cocos2d::Label* _HatOptionLabel;
	cocos2d::Label* _ClothOptionLabel;
	cocos2d::Label* _PantsOptionLabel;
	cocos2d::TextFieldTTF* _inputName;
	cocos2d::Node* _CharacteNode;
	cocos2d::Node* _SceneCSB;
	cocos2d::Sprite* _newBody;
	cocos2d::Sprite* _newRightUpArm;
	cocos2d::Sprite* _newRightDownArm;
	cocos2d::Sprite* _newLeftUpArm;
	cocos2d::Sprite* _newLeftDownArm;
	cocos2d::Sprite* _newRightFist;
	cocos2d::Sprite* _newRightFistFlip;
	cocos2d::Sprite* _newLeftFist;
	cocos2d::Sprite* _newRightBigLeg;
	cocos2d::Sprite* _newLeftBigLeg;
	cocos2d::Sprite* _newRightSmallLeg;
	cocos2d::Sprite* _newLeftSmallLeg;
	cocos2d::Sprite* _newRightFoot;
	cocos2d::Sprite* _newLeftFoot;
};
NS_SCROLLMODE_END
#endif