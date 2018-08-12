#include "RPGTools\RPGTool.h"
#include "tools\JsonParser.h"
USING_NS_CC;
using namespace MyTools;
const int RPGTool::Const_MaxItemStack = 99;
const int RPGTool::Const_MaxItemCount = 90;
bool RPGTool::isItemIDCanStack(const int ItemID)
{
	if (ItemID > (int)RPGTool::ItemID::_STACK_LineBegin &&
		ItemID < (int)RPGTool::ItemID::_STACK_LineEnd)
	{//可以堆疊的物品
		return true;
	}
	else
	{//不可堆疊的物品
		return false;
	}
}
bool RPGTool::isItemIDCanDrink(const int ItemID)
{
	if (ItemID > (int)RPGTool::ItemID::_Drink_LineBegin &&
		ItemID < (int)RPGTool::ItemID::_Drink_LineEnd)
	{//可以喝的物品
		return true;
	}
	else
	{//不可以喝的物品
		return false;
	}
}
RPGTool::BaseData RPGTool::accordingItemIDToData(const int itemId)
{
	BaseData data;
	if (itemId == (int)ItemID::Hair_Tail)
	{
		data.characterPart= CharacterPart::Hair;
		data.SpritePath = "ui/item/Hair_Tail.png";
		data.ItemName = "Hair_Tail";
		data.Blood = 2;
		data.Defend = 5;
		data.Price = 5;
		
	}
	else if (itemId == (int)ItemID::Hair_Explore)
	{
		data.characterPart = CharacterPart::Hair;
		data.SpritePath = "ui/item/Hair_Explore.png";
		data.ItemName = "Hair_Explore";
		data.Blood = 5;
		data.Defend = 2;
		data.Price = 5;
		
	}
	else if (itemId == (int)ItemID::Hair_Blade)
	{
		data.characterPart = CharacterPart::Hair;
		data.SpritePath = "ui/item/Hair_Blade.png";
		data.ItemName = "Hair_Blade";
	}
	else if (itemId == (int)ItemID::Hair_Band)
	{
		data.characterPart = CharacterPart::Hair;
		data.SpritePath = "ui/item/Hair_Band.png";
		data.ItemName = "Hair_Band";
		data.Price = 10;
		
	}
	else if (itemId == (int)ItemID::Hair_Hat)
	{
		data.characterPart = CharacterPart::Hair;
		data.SpritePath = "ui/item/Hair_Hat.png";
		data.ItemName = "Hair_Hat";
		data.Price = 15;
		
	}
	else if (itemId == (int)ItemID::Cloth_Farmer)
	{
		data.characterPart = CharacterPart::Cloth;
		data.SpritePath = "ui/item/Cloth_Farmer.png";
		data.ItemName = "Cloth_Farmer";
		data.Defend = 5;
		data.Price = 20;
		
	}
	else if (itemId == (int)ItemID::Cloth_ChiuanJen)
	{
		data.characterPart = CharacterPart::Cloth;
		data.SpritePath = "ui/item/Cloth_ChiuanJen.png";
		data.ItemName = "Cloth_ChiuanJen";
		data.Defend = 7;
		data.Price = 20;
		
	}
	else if (itemId == (int)ItemID::Cloth_Naked)
	{
		data.characterPart = CharacterPart::Cloth;
		data.SpritePath = "ui/item/Cloth_Naked.png";
		data.ItemName = "Cloth_Naked";
	}
	else if (itemId == (int)ItemID::Cloth_BlueKnight)
	{
		data.characterPart = CharacterPart::Cloth;
		data.SpritePath = "ui/item/Cloth_BlueKnight.png";
		data.ItemName = "Cloth_BlueKnight";
		data.Defend = 10;
		data.Price = 100;
	}
	else if (itemId == (int)ItemID::Cloth_Blody)
	{
		data.characterPart = CharacterPart::Cloth;
		data.SpritePath = "ui/item/Cloth_Blody.png";
		data.ItemName = "Cloth_Blody";
		data.Defend = 15;
		data.Price = 300;
	}
	else if (itemId == (int)ItemID::Pant_Farmer)
	{
		data.characterPart = CharacterPart::Pant;
		data.SpritePath = "ui/item/Pant_Farmer.png";
		data.ItemName = "Pant_Farmer";
		data.Defend = 1;
		data.Dodge = 3;
		data.Price = 50;
		
	}
	else if (itemId == (int)ItemID::Pant_ChiuanJen)
	{
		data.characterPart = CharacterPart::Pant;
		data.SpritePath = "ui/item/Pant_ChiuanJen.png";
		data.ItemName = "Pant_ChiuanJen";
		data.Dodge = 5;
		data.Price = 70;
		
	}
	else if (itemId == (int)ItemID::Pant_Naked)
	{
		data.characterPart = CharacterPart::Pant;
		data.SpritePath = "ui/item/Pant_Naked.png";
		data.ItemName = "Pant_Naked";
	}
	else if (itemId == (int)ItemID::Pant_BlueKnight)
	{
		data.characterPart = CharacterPart::Pant;
		data.SpritePath = "ui/item/Pant_BlueKnight.png";
		data.ItemName = "Pant_BlueKnight";
		data.Dodge = 7;
		data.Price = 75;
	}
	else if (itemId == (int)ItemID::Pant_Blody)
	{
		data.characterPart = CharacterPart::Pant;
		data.SpritePath = "ui/item/Pant_Blody.png";
		data.ItemName = "Pant_Blody";
		data.Dodge = 10;
		data.Price = 100;
	}
	else if (itemId == (int)ItemID::Weapon_Fist)
	{
		data.characterPart = CharacterPart::Weapon;
		data.SpritePath = "ui/item/Weapon_Fist.png";
		data.ItemName = "Weapon_Fist";
		data.Force = 5;
		data.Price = 100;
	}
	else if (itemId == (int)ItemID::Weapon_Fist_Blue)
	{
		data.characterPart = CharacterPart::Weapon;
		data.SpritePath = "ui/item/Weapon_Fist_Blue.png";
		data.ItemName = "Weapon_Fist_Blue";
		data.Force = 20;
		data.Price = 200;
	}
	else if (itemId == (int)ItemID::Weapon_NoobSword)
	{
		data.characterPart = CharacterPart::Weapon;
		data.SpritePath = "ui/item/Weapon_NoobSword.png";
		data.ItemName = "Weapon_NoobSword";
		data.Force = 5;
		data.Price = 100;
	}
	else if (itemId == (int)ItemID::Weapon_Sword)
	{
		data.characterPart = CharacterPart::Weapon;
		data.SpritePath = "ui/item/Weapon_Sword.png";
		data.ItemName = "Weapon_Sword";
		data.Force = 7;
		data.Price = 50;
	}
	else if (itemId == (int)ItemID::Weapon_Shadow)
	{
		data.characterPart = CharacterPart::Weapon;
		data.SpritePath = "ui/item/Weapon_Shadow.png";
		data.ItemName = "Weapon_Shadow";
		data.Force = 30;
		data.Price = 1000;
	}
	else if (itemId == (int)ItemID::Weapon_Blody)
	{
		data.characterPart = CharacterPart::Weapon;
		data.SpritePath = "ui/item/Weapon_Blody.png";
		data.ItemName = "Weapon_Blody";
		data.Force = 40;
		data.Price = 2000;
	}
	else if (itemId == (int)ItemID::Head_Normal)
	{
		data.characterPart = CharacterPart::Head;
		data.SpritePath = "ui/item/Head_Normal.png";
		data.ItemName = "Head_Normal";
	}
	else if (itemId == (int)ItemID::Head_Goat)
	{
		data.characterPart = CharacterPart::Head;
		data.SpritePath = "ui/item/Head_Goat.png";
		data.ItemName = "Head_Goat";
	}
	else if (itemId == (int)ItemID::SmallRedPotion)
	{
		data.characterPart = CharacterPart::HPBar;
		data.SpritePath = "ui/item/SmallRedPotion.png";
		data.ItemName = "SmallRedPotion";
		data.RecoverHP = 20;
		data.Price = 1;	
	}
	else if (itemId == (int)ItemID::MiddleRedPotion)
	{
		data.characterPart = CharacterPart::HPBar;
		data.SpritePath = "ui/item/MiddleRedPotion.png";
		data.ItemName = "MiddleBluePotion";
		data.RecoverHP = 50;
		data.Price = 5;
	}
	else if (itemId == (int)ItemID::LargeRedPotion)
	{
		data.characterPart = CharacterPart::HPBar;
		data.SpritePath = "ui/item/LargeRedPotion.png";
		data.ItemName = "LargeRedPotion";
		data.RecoverHP = 100;
		data.Price = 30;
	}
	else if (itemId == (int)ItemID::SmallBluePotion)
	{
		data.characterPart = CharacterPart::MPBar;
		data.SpritePath = "ui/item/SmallBluePotion.png";
		data.ItemName = "SmallBluePotion";
		data.RecoverMP = 10;
		data.Price = 1;
	}
	else if (itemId == (int)ItemID::MiddleBluePotion)
	{
		data.characterPart = CharacterPart::MPBar;
		data.SpritePath = "ui/item/MiddleBluePotion.png";
		data.ItemName = "MiddleBluePotion";
		data.RecoverMP = 25;
		data.Price = 5;
	}
	else if (itemId == (int)ItemID::LargeBluePotion)
	{
		data.characterPart = CharacterPart::MPBar;
		data.SpritePath = "ui/item/LargeBluePotion.png";
		data.ItemName = "LargeBluePotion";
		data.RecoverMP = 50;
		data.Price = 30;
	}
	else
	{
		data.characterPart = CharacterPart::None;
		data.SpritePath = "";
		data.ItemName = "";
	}
	return data;
}
int RPGTool::getEquipmentBaseProperty(BaseProperty baseProperty)
{
	int Blood = 0;
	int Force = 0;
	int Magic = 0;
	int Defend = 0;
	int Dodge = 0;
	int Popular = 0;
	for (int i = 0;i < 6;i++)
	{
		int equipmentID=JsonParser::getEquipmentFromSave(i, JsonParser::Equipment::GetID);
		int equipmentRank = JsonParser::getEquipmentFromSave(i, JsonParser::Equipment::GetValue);
		if (equipmentRank == -1)
		{
			continue;
		}
		switch (baseProperty)
		{
		case RPGTool::BaseProperty::Blood:
			Blood+=accordingItemIDToData(equipmentID).Blood+equipmentRank;
			break;
		case RPGTool::BaseProperty::Force:
			Force += accordingItemIDToData(equipmentID).Force + equipmentRank;
			break;
		case RPGTool::BaseProperty::Magic:
			Magic += accordingItemIDToData(equipmentID).Magic + equipmentRank;
			break;
		case RPGTool::BaseProperty::Defend:
			Defend += accordingItemIDToData(equipmentID).Defend + equipmentRank;
			break;
		case RPGTool::BaseProperty::Dodge:
			Dodge += accordingItemIDToData(equipmentID).Dodge + equipmentRank;
			break;
		case RPGTool::BaseProperty::Popular:
			Popular += accordingItemIDToData(equipmentID).Popular + equipmentRank;
			break;
		}
	}
	if (Blood != 0)
		return Blood;
	else if (Force != 0)
		return Force;
	else if (Magic != 0)
		return Magic;
	else if (Defend != 0)
		return Defend;
	else if (Dodge != 0)
		return Dodge;
	else
		return Popular;
}