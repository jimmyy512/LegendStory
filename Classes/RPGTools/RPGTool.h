#ifndef __RPGTool_H__
#define __RPGTool_H__

#include "cocos2d.h"

class RPGTool
{
public:
	enum class ItemID
	{
		ItemID_Start,
		Hair_Tail,
		Hair_Explore,
		Hair_Blade,
		Hair_Band,
		Hair_Hat,
		Head_Normal,
		Head_Goat,
		Cloth_Farmer,
		Cloth_ChiuanJen,
		Cloth_Naked,
		Cloth_BlueKnight,
		Cloth_Blody,
		Pant_ChiuanJen,
		Pant_Farmer,
		Pant_Naked,
		Pant_BlueKnight,
		Pant_Blody,
		Weapon_Fist,
		Weapon_Fist_Blue,
		Weapon_Sword,
		Weapon_NoobSword,
		Weapon_Shadow,
		Weapon_Blody,


		_STACK_LineBegin,
		_Drink_LineBegin,

		_HP_PotionLineBegin,
		SmallRedPotion,
		MiddleRedPotion,
		LargeRedPotion,
		_HP_PotionLineEnd,

		_MP_PotionLineBegin,
		SmallBluePotion,
		MiddleBluePotion,
		LargeBluePotion,
		_MP_PotionLineEnd,

		_Drink_LineEnd,
		_STACK_LineEnd,
		
		ItemID_End
	};
	enum class CharacterPart
	{
		Hair,    //頭部+血量
		NeckLace,//項鍊+內力  
		Belt,    //腰帶+內力
		Cloth,   //上衣+防禦
		Pant,    //下衣 + 防禦
		Weapon,  //武器+攻擊力
		Head,     //非可更改項目
		HPBar,
		MPBar,
		None     //都不是
	};
	enum class BaseProperty
	{
		Blood,    //氣血
		Force,    //武力
		Magic,    //內力
		Defend,   //防禦
		Dodge,    //身法
		Popular,  //聲望
	};
	struct BaseData
	{
		CharacterPart characterPart;
		std::string SpritePath;//圖片路徑
		std::string ItemName;
		int Price=0;//賣到商店的價格
		int Blood=0;
		int Force=0;
		int Magic=0;
		int Defend=0;
		int Dodge=0;
		int Popular=0;
		int RecoverHP = 0;//回復Hp量
		int RecoverMP = 0;//回復Mp量
	};
	//*傳入RPGTool::ItemID 如果此物品能堆疊，回傳true 否則false
	static bool isItemIDCanStack(const int ItemID);
	//*傳入RPGTool::ItemID 如果此物品能喝下，回傳true 否則false
	static bool isItemIDCanDrink(const int ItemID);
	static BaseData accordingItemIDToData(const int itemId);
	//*傳入想要知道的屬性，穿在身上的裝備，共加多少
	static int getEquipmentBaseProperty(BaseProperty baseProperty);
	//*回傳0~4之間的隨機數
	static inline int getRandMaxItemValue() { return rand() % 11; };
	//*背包最高堆疊數
	static const int Const_MaxItemStack;
	static const int Const_MaxItemCount;
};

#endif
