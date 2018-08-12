#include "tools\JsonParser.h"
#include "RPGTools\RPGTool.h"
#include "cocos2d.h"
#include "rapidjson\rapidjson.h"
#include "rapidjson\document.h"
#include "rapidjson\writer.h"
#include "rapidjson\stringbuffer.h"
#include "UI\ScrollMode\HUDlayer.h"
using namespace rapidjson;
using namespace cocos2d;
namespace MyTools
{
	bool MyTools::JsonParser::haveSaveFile()
	{
		auto savePath = FileUtils::getInstance()->getWritablePath();
		savePath.append("saves.json");
		if (FileUtils::getInstance()->isFileExist(savePath))
			return true;
		else
			return false;
	}
	void JsonParser::buildNewSave()
	{
		auto savePath = FileUtils::getInstance()->getWritablePath();
		savePath.append("saves.json");

		rapidjson::Document d;
		rapidjson::Document::AllocatorType& allocator = d.GetAllocator();
		d.SetObject();

		//遊戲設定
		rapidjson::Value Optional(rapidjson::kObjectType);
		Optional.AddMember("Language", "TW", allocator);
		Optional.AddMember("MusicVolume", "50", allocator);
		Optional.AddMember("SoundVolume", "50", allocator);
		d.AddMember("Optional", Optional, allocator);

		//@debug
		rapidjson::Value debug(rapidjson::kObjectType);
		debug.AddMember("ForceAddHPMP", "0", allocator);
		d.AddMember("Debug", debug, allocator);

		//@基本資訊
		rapidjson::Value Infor(rapidjson::kObjectType);
		Infor.AddMember("SkillType", "0", allocator);
		Infor.AddMember("CharacterName", "Admin", allocator);
		Infor.AddMember("Money", "100", allocator);
		Infor.AddMember("Level", "1", allocator);
		Infor.AddMember("EXP", "0", allocator);
		Infor.AddMember("HP", "999", allocator);
		Infor.AddMember("MP", "999", allocator);
		Infor.AddMember("Popular", "0", allocator);
		Infor.AddMember("Mission","0",allocator);//任務當前在第幾章節
		Infor.AddMember("MissionTargetMount", "0", allocator);//任務目標計數,例如計算擊殺怪物數量
		Infor.AddMember("ItemTip","1", allocator);//物品驚嘆號提示
		Infor.AddMember("MissionTip","1", allocator);//任務驚嘆號提示
		Infor.AddMember("LastPreloadSceneEnum", "0", allocator);//最後一次進入的地圖是在哪一張
		d.AddMember("Infor", Infor, allocator);


		//@初始衣服 裝備中的物品
		//Hair,    //頭部+血量
		//NeckLace,//項鍊+內力  
		//Belt,    //腰帶+內力
		//Cloth,   //上衣+防禦
		//Pant,    //下衣 + 防禦
		//Weapon   //武器+攻擊力
		//head
		//hp amount 
		//mp amount
		rapidjson::Value EquipmentArray(rapidjson::kArrayType);
		char HairItem[5];
		char NeckLaceItem[5];
		char BeltItem[5];
		char ClothItem[5];
		char PantItem[5];
		char WeaponItem[5];
		char HeadItem[5];
		sprintf(HairItem, "%d", (int)RPGTool::ItemID::Hair_Tail);
		sprintf(NeckLaceItem, "%d", -1);
		sprintf(BeltItem, "%d", -1);
		sprintf(ClothItem, "%d", (int)RPGTool::ItemID::Cloth_Farmer);
		sprintf(PantItem, "%d", (int)RPGTool::ItemID::Pant_Farmer);
		sprintf(WeaponItem, "%d", (int)RPGTool::ItemID::Weapon_Fist);
		sprintf(HeadItem, "%d", (int)RPGTool::ItemID::Head_Normal);
		//type value
		rapidjson::Value HairItemtypeValue(HairItem, allocator);
		rapidjson::Value NeckLaceItemtypeValue(NeckLaceItem, allocator);
		rapidjson::Value BeltItemtypeValue(BeltItem, allocator);
		rapidjson::Value ClothItemtypeValue(ClothItem, allocator);
		rapidjson::Value PantItemtypeValue(PantItem, allocator);
		rapidjson::Value WeaponItemtypeValue(WeaponItem, allocator);
		rapidjson::Value HeadItemtypeValue(HeadItem, allocator);
		//object
		rapidjson::Value HairItemObject(rapidjson::kObjectType);
		rapidjson::Value NeckLaceItemObject(rapidjson::kObjectType);
		rapidjson::Value BeltItemObject(rapidjson::kObjectType);
		rapidjson::Value ClothItemObject(rapidjson::kObjectType);
		rapidjson::Value PantItemObject(rapidjson::kObjectType);
		rapidjson::Value WeaponItemObject(rapidjson::kObjectType);
		rapidjson::Value HeadItemObject(rapidjson::kObjectType);
		rapidjson::Value HPItemObject(rapidjson::kObjectType);
		rapidjson::Value MpItemObject(rapidjson::kObjectType);
		//object add member
		//-1 代表沒穿
		HairItemObject.AddMember(HairItemtypeValue, "0", allocator);//第一個是物品ID 第二個是強化階數
		NeckLaceItemObject.AddMember(NeckLaceItemtypeValue, "-1", allocator);//第一個是物品ID 第二個是強化階數
		BeltItemObject.AddMember(BeltItemtypeValue, "-1", allocator);//第一個是物品ID 第二個是強化階數
		ClothItemObject.AddMember(ClothItemtypeValue, "0", allocator);//第一個是物品ID 第二個是強化階數
		PantItemObject.AddMember(PantItemtypeValue, "0", allocator);//第一個是物品ID 第二個是強化階數
		WeaponItemObject.AddMember(WeaponItemtypeValue, "0", allocator);//第一個是物品ID 第二個是強化階數
		HeadItemObject.AddMember(HeadItemtypeValue, "-1", allocator);
		HPItemObject.AddMember("-1", "-1", allocator);//第一個是物品ID 第二個是數量
		MpItemObject.AddMember("-1", "-1", allocator);//第一個是物品ID 第二個是數量

		EquipmentArray.PushBack(HairItemObject, allocator);
		EquipmentArray.PushBack(NeckLaceItemObject, allocator);
		EquipmentArray.PushBack(BeltItemObject, allocator);
		EquipmentArray.PushBack(ClothItemObject, allocator);
		EquipmentArray.PushBack(PantItemObject, allocator);
		EquipmentArray.PushBack(WeaponItemObject, allocator);
		EquipmentArray.PushBack(HeadItemObject, allocator);
		EquipmentArray.PushBack(HPItemObject, allocator);
		EquipmentArray.PushBack(MpItemObject, allocator);
		d.AddMember("Equipment", EquipmentArray, allocator);

		//物品ID
		rapidjson::Value ItemIDArray(rapidjson::kArrayType);
		//起始物品
		//char item1[10];
		//char item2[10];
		//sprintf(item1, "%d", (int)RPGTool::ItemID::Weapon_Sword);
		//sprintf(item2, "%d", (int)RPGTool::ItemID::Cloth_ChiuanJen);
		////type value
		//rapidjson::Value item1typeValue(item1, allocator);
		//rapidjson::Value item2typeValue(item2, allocator);
		////object
		//rapidjson::Value Item1Object(rapidjson::kObjectType);
		//rapidjson::Value item2Object(rapidjson::kObjectType);
		////object add member
		//Item1Object.AddMember(item1typeValue, "0", allocator);;//第一個是物品ID 第二個是強化階數
		//item2Object.AddMember(item2typeValue, "0", allocator);;//第一個是物品ID 第二個是強化階數
		for (int i = 0;i < RPGTool::Const_MaxItemCount;i++)
		{
			rapidjson::Value Item1Object(rapidjson::kObjectType);
			Item1Object.AddMember("-1", "-1", allocator);
			ItemIDArray.PushBack(Item1Object, allocator);
		}
		d.AddMember("ItemID", ItemIDArray, allocator);


		rapidjson::StringBuffer  buffer;
		rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
		d.Accept(writer);
		FILE* file = fopen(savePath.c_str(), "wb");
		if (file)
		{
			fputs(buffer.GetString(), file);
			fclose(file);
		}
	}
	char * JsonParser::accordingEnumGivingPath(JsonType jsonType)
	{
		switch (jsonType)
		{
		case JsonParser::JsonType::UserInterFace:
			return "json/UserInterFace";
			break;
		case JsonParser::JsonType::NPC:
			return "json/NPC";
			break;
		case JsonParser::JsonType::TableCode:
			return "json/NPC_CODE_TABLE";
			break;
		case JsonParser::JsonType::ItemDOC:
			return "json/ItemDOC";
			break;
		case JsonParser::JsonType::Mission:
			return "json/Mission";
			break;
		default:
			throw "in JsonParser accordingEnumGivingPath function no this condition.";
		}
		return nullptr;
	}
	std::string JsonParser::ReadDramaJsonFile(const char* fileName, const char* DialogCode, int index)
	{
		rapidjson::Document d;
		d.Parse<rapidjson::kParseDefaultFlags>(FileUtils::getInstance()->getStringFromFile(fileName).c_str());
		if (d.HasParseError())
		{
			
			CCASSERT(false, "drama.json error!");
		}
		if (d.IsObject())
		{
			if (d.HasMember(DialogCode))
			{
				if (d[DialogCode].IsArray())
				{
					if (index<d[DialogCode].Size())
					{
						return d[DialogCode][index].GetString();
					}
					else
					{//沒有下一句話了，對話結束。
						return "";
					}
				}
			}
			else
			{
				CCASSERT(false,"dont have this DialogCode");
			}
		}
	}

	std::string JsonParser::ReadJsonFile(const char* first, const char* second, const char* fileName)
	{
		rapidjson::Document d;
		std::string str = FileUtils::getInstance()->getStringFromFile(fileName);
		d.Parse<rapidjson::kParseDefaultFlags>(str.c_str());
		if (d.HasParseError())
		{
			CCASSERT(false, "Json格式錯誤!");
		}
		if (d.IsObject())
		{
			if (d.HasMember(first))
			{
				if (d[first].HasMember(second))
				{
					return d[first][second].GetString();
				}
				else
				{
					CCASSERT(false,"ReadJsonFile second not found.");
				}
			}
			else
			{
				CCASSERT(false, "ReadJsonFile first not found.");
			}
		}
	}

	std::string MyTools::JsonParser::ReadTableCodeJsonFile(const char * first, const char * second, const char * fileName)
	{
		rapidjson::Document d;
		std::string str = FileUtils::getInstance()->getStringFromFile(fileName);
		d.Parse<rapidjson::kParseDefaultFlags>(str.c_str());
		if (d.HasParseError())
		{
			CCASSERT(false, "json格式錯誤");
		}
		if (d.IsObject())
		{
			if (d.HasMember(first))
			{
				if (d[first].HasMember(second))
				{
					return d[first][second].GetString();
				}
				else
				{//針對table code的優化，找不到第二個名稱時，回傳第二個名稱.
					return second;
				}
			}
			else
			{
				CCASSERT(false, "ReadJsonFile first not found.");
			}
		}
	}
	void JsonParser::SetJsonFile(const char * first, const char * second, const char* fileName, const char * newValue)
	{
		rapidjson::Document d;
		std::string str = FileUtils::getInstance()->getStringFromFile(fileName);
		d.Parse<rapidjson::kParseDefaultFlags>(str.c_str());
		if (d.HasParseError())
		{
			
			CCASSERT(false, "json格式錯誤");
		}
		if (d.IsObject())
		{
			if (d.HasMember(first))
			{
				if (d[first].IsObject())
				{
					d[first][second].SetString(StringRef(newValue));
					auto savePath = FileUtils::getInstance()->getWritablePath();
					savePath.append("saves.json");
					//將json數據重新寫入
					StringBuffer buffer;
					rapidjson::Writer<StringBuffer> writer(buffer);
					d.Accept(writer);
					FILE* file = fopen(savePath.c_str(), "wb");
					if (file) {
						fputs(buffer.GetString(), file);
						fclose(file);
					}
				}
			}
		}
	}
	std::string MyTools::JsonParser::getSaveJsonStr()
	{
		auto savePath = FileUtils::getInstance()->getWritablePath();
		savePath.append("saves.json");
		std::string str = FileUtils::getInstance()->getStringFromFile(savePath.c_str());
		return str;
	}
	void MyTools::JsonParser::setSaveJsonStr(std::string newSaveJsonStr)
	{
		rapidjson::Document d;
		d.Parse<rapidjson::kParseDefaultFlags>(newSaveJsonStr.c_str());
		if (d.HasParseError())
		{
			
			CCASSERT(false, "json格式錯誤");
		}
		if (d.IsObject())
		{
			auto savePath = FileUtils::getInstance()->getWritablePath();
			savePath.append("saves.json");
			//將json數據重新寫入
			StringBuffer buffer;
			rapidjson::Writer<StringBuffer> writer(buffer);
			d.Accept(writer);
			FILE* file = fopen(savePath.c_str(), "wb");
			if (file) {
				fputs(buffer.GetString(), file);
				fclose(file);
			}
		}
	}
	std::string JsonParser::getFromSaveLanguage()
	{
		auto savePath = FileUtils::getInstance()->getWritablePath();
		savePath.append("saves.json");
		return ReadJsonFile("Optional", "Language", (char*)savePath.c_str());
	}
	std::string JsonParser::getJsonString(const char * first, const char * second, JsonType jsonType)
	{
		char RealFile[100];
		std::string language = JsonParser::getFromSaveLanguage();
		//存檔路徑
		auto savePath = FileUtils::getInstance()->getWritablePath();
		savePath.append("saves.json");
		if (jsonType == JsonParser::JsonType::Save)
			return  ReadJsonFile(first, second, (char*)savePath.c_str());
		else if(jsonType==JsonParser::JsonType::TableCode)
		{
			sprintf(RealFile, "%s_%s.json", accordingEnumGivingPath(jsonType), language.c_str());
			return  ReadTableCodeJsonFile(first, second, RealFile);
		}
		else
		{
			sprintf(RealFile, "%s_%s.json", accordingEnumGivingPath(jsonType), language.c_str());
			return  ReadJsonFile(first, second, RealFile);
		}
	}
	std::string MyTools::JsonParser::getJsonStringWithOutSave(const char * first, const char * second, JsonType jsonType)
	{
		char RealFile[100];
		sprintf(RealFile, "%s_%s.json", accordingEnumGivingPath(jsonType), "TW");
		return  ReadJsonFile(first, second, RealFile);
	}
	std::string MyTools::JsonParser::getNPCJsonFile(const char * NPCName, const char * Opa, int index)
	{
		char RealFile[100];
		std::string language = JsonParser::getFromSaveLanguage();
		//檔案路徑
		sprintf(RealFile, "%s_%s.json", accordingEnumGivingPath(JsonType::NPC), language.c_str());

		rapidjson::Document d;
		d.Parse<rapidjson::kParseDefaultFlags>(FileUtils::getInstance()->getStringFromFile(RealFile).c_str());
		if (d.HasParseError())
		{
			
			CCASSERT(false, "json格式錯誤");
		}
		if (d.IsObject())
		{
			if (d.HasMember(NPCName))
			{
				if (d[NPCName][Opa].IsArray())
				{
					if (index<d[NPCName][Opa].Size())
					{
						return d[NPCName][Opa][index].GetString();
					}
					else
					{//沒有資料
 						return "NoStr";
					}
				}
			}
			else
			{
				CCASSERT(false, "dont have this NPCNAME");
			}
		}
	}
	int MyTools::JsonParser::getItemCountFromSave()
	{
		auto savePath = FileUtils::getInstance()->getWritablePath();
		savePath.append("saves.json");
		rapidjson::Document d;
		std::string str = FileUtils::getInstance()->getStringFromFile(savePath.c_str());
		d.Parse<rapidjson::kParseDefaultFlags>(str.c_str());
		if (d.HasParseError())
		{
			
			CCASSERT(false, "save.json wrong format!");
		}
		if (d.IsObject())
		{
			if (d.HasMember("ItemID"))
			{
				int count = 0;
				for (int i = 0;i < RPGTool::Const_MaxItemCount;i++)
				{
					auto it = d["ItemID"][i].GetObject().begin();
					if (it->value != "-1")
					{
						count++;
					}
				}
				return count;
			}
		}
	}
	int MyTools::JsonParser::getFreeItemSpaceCountFromSave()
	{
		auto savePath = FileUtils::getInstance()->getWritablePath();
		savePath.append("saves.json");
		rapidjson::Document d;
		std::string str = FileUtils::getInstance()->getStringFromFile(savePath.c_str());
		d.Parse<rapidjson::kParseDefaultFlags>(str.c_str());
		if (d.HasParseError())
		{
			
			CCASSERT(false, "save.json wrong format!");
		}
		if (d.IsObject())
		{
			if (d.HasMember("ItemID"))
			{
				int count = 0;
				for (int i = 0;i < RPGTool::Const_MaxItemCount;i++)
				{
					auto it = d["ItemID"][i].GetObject().begin();
					if (it->value == "-1")
					{
						count++;
					}
				}
				return count;
			}
		}
	}
	int MyTools::JsonParser::getEquipmentFromSave(int characterPart, Equipment equipmentOperator)
	{
		auto savePath = FileUtils::getInstance()->getWritablePath();
		savePath.append("saves.json");
		rapidjson::Document d;
		std::string str = FileUtils::getInstance()->getStringFromFile(savePath.c_str());
		d.Parse<rapidjson::kParseDefaultFlags>(str.c_str());
		if (d.HasParseError())
		{
			
			CCASSERT(false, "save.json wrong format!");
		}
		if (d.IsObject())
		{
			if (d.HasMember("Equipment"))
			{
				//依照CharacterPart順序
				//Hair,    //頭部+血量
				//NeckLace,//項鍊+內力  
				//Belt,    //腰帶+內力
				//Cloth,   //上衣+防禦
				//Pant,    //下衣 + 防禦
				//Weapon   //武器+攻擊力
				//Head     //非可更改項目
				for (int i = 0; i < d["Equipment"].Size(); i++) {
					if (i == characterPart)
					{
						auto it = d["Equipment"][i].GetObject().begin();
						if(equipmentOperator==Equipment::GetID)
							return atoi((*it).name.GetString());
						else
							return atoi((*it).value.GetString());
						break;
					}
				}
			}
		}
	}
	void MyTools::JsonParser::getItemDataFromSave(int index, ItemSprite::ItemData& itemData)
	{
		auto savePath = FileUtils::getInstance()->getWritablePath();
		savePath.append("saves.json");
		rapidjson::Document d;
		std::string str = FileUtils::getInstance()->getStringFromFile(savePath.c_str());
		d.Parse<rapidjson::kParseDefaultFlags>(str.c_str());
		if (d.HasParseError())
		{
			
			CCASSERT(false, "save.json wrong format!");
		}
		if (d.IsObject())
		{
			if (d.HasMember("ItemID"))
			{
				itemData.itemId = -1;
				itemData.itemValue = -1;
				for (int i = 0; i < d["ItemID"].Size(); i++) {
					if (i == index)
					{
						auto it = d["ItemID"][i].GetObject().begin();
						itemData.itemId = atoi((*it).name.GetString());
						itemData.itemValue = atoi((*it).value.GetString());
						break;
					}
				}
			}
		}
	}
	void MyTools::JsonParser::getItemAllDataFromSave(std::vector<std::pair<int, int>>& itemDatas)
	{
		auto savePath = FileUtils::getInstance()->getWritablePath();
		savePath.append("saves.json");
		rapidjson::Document d;
		std::string str = FileUtils::getInstance()->getStringFromFile(savePath.c_str());
		d.Parse<rapidjson::kParseDefaultFlags>(str.c_str());
		if (d.HasParseError())
		{
			
			CCASSERT(false, "save.json wrong format!");
		}
		if (d.IsObject())
		{
			if (d.HasMember("ItemID"))
			{
				for (int i = 0; i < d["ItemID"].Size(); i++) 
				{
					auto it = d["ItemID"][i].GetObject().begin();
					itemDatas.push_back(std::make_pair(atoi(it->name.GetString()), atoi(it->value.GetString())));
				}
			}
		}
	}
	void MyTools::JsonParser::removeItemFromSave(int index)
	{
		auto savePath = FileUtils::getInstance()->getWritablePath();
		savePath.append("saves.json");
		rapidjson::Document d;
		std::string str = FileUtils::getInstance()->getStringFromFile(savePath.c_str());
		d.Parse<rapidjson::kParseDefaultFlags>(str.c_str());
		if (d.HasParseError())
		{
			
			CCASSERT(false, "save.json wrong format!");
		}
		if (d.IsObject())
		{
			if (d.HasMember("ItemID"))
			{
				for (int i = 0;i < RPGTool::Const_MaxItemCount;i++)
				{
					if (i == index)
					{
						auto it = d["ItemID"][i].GetObject().begin();
						it->name = "-1";
						it->value = "-1";
						rapidjson::StringBuffer  buffer;
						rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
						d.Accept(writer);
						FILE* file = fopen(savePath.c_str(), "wb");
						if (file)
						{
							fputs(buffer.GetString(), file);
							fclose(file);
						}
						break;
					}
				}
			}
		}
	}
	int MyTools::JsonParser::AddItemToSave(int ItemID, int ItemValue)
	{
		if(RPGTool::isItemIDCanStack(ItemID))
			CCAssert(ItemValue <= RPGTool::Const_MaxItemStack, "ItemValue > MaxItemStack warring");
		//加入到哪一格ItemIndex中
		int Itemindex=-1;
		auto savePath = FileUtils::getInstance()->getWritablePath();
		savePath.append("saves.json");
		rapidjson::Document d;
		std::string str = FileUtils::getInstance()->getStringFromFile(savePath.c_str());
		d.Parse<rapidjson::kParseDefaultFlags>(str.c_str());
		if (getItemCountFromSave() >= RPGTool::Const_MaxItemCount)
		{//背包已滿 添加失敗
			return -1;
		}
		else
		{
			ScrollMode::HUDlayer::getHUDlayer()->setItemTip(true);
		}
		if (d.HasParseError())
		{
			
			CCASSERT(false, "save.json wrong format!");
		}
		if (d.IsObject())
		{
			if (d.HasMember("ItemID"))
			{
				rapidjson::Document::AllocatorType& allocator = d.GetAllocator();
				if (!RPGTool::isItemIDCanStack(ItemID))
				{//非堆疊物品，像是武器裝備等等
					char ItemIDStr[10];
					char ItemValueStr[10];
					sprintf(ItemIDStr, "%d", ItemID);
					sprintf(ItemValueStr, "%d", ItemValue);
					rapidjson::Value itemIDtypeValue(ItemIDStr, allocator);
					rapidjson::Value itemValuetypeValue(ItemValueStr, allocator);
					for (int i = 0;i < RPGTool::Const_MaxItemCount;i++)
					{
						auto it = d["ItemID"][i].GetObject().begin();
						if (it->value == "-1")
						{
							Itemindex = i;
							it->name = itemIDtypeValue;
							it->value = itemValuetypeValue;
							break;
						}
					}
				}
				else
				{//可堆疊物品，像是食物藥水等等
					bool isFindSameItem = false;
					//先尋找是否存在相同的物品
					for (int i = 0; i < d["ItemID"].Size(); i++)
					{
						auto IteratorItem = d["ItemID"][i].GetObject().begin();
						int IteratorItemID = atoi((*IteratorItem).name.GetString());
						int IteratorItemAmount = atoi((*IteratorItem).value.GetString());
						if (IteratorItemID == ItemID)
						{//如果找到相同的物品，做疊加
							if (IteratorItemAmount == RPGTool::Const_MaxItemStack)
							{//找到了相同物品，但是數量仍然是滿的所以再次尋找下一個相同物品
								continue;
							}
							else
							{
								isFindSameItem = true;
								if (IteratorItemAmount + ItemValue <= RPGTool::Const_MaxItemStack)
								{//把新加入的物品，加入已有的物品疊加起來
									Itemindex = i;
									char ItemAmount[10];
									sprintf(ItemAmount, "%d", IteratorItemAmount + ItemValue);
									rapidjson::Value itemAmounttypeValue(ItemAmount, allocator);
									IteratorItem->value = itemAmounttypeValue;
								}
								else
								{//如果疊加數量超過堆疊數量
									char ItemAmount[10];
									sprintf(ItemAmount, "%d", RPGTool::Const_MaxItemStack);
									rapidjson::Value itemAmounttypeValue(ItemAmount, allocator);
									IteratorItem->value = itemAmounttypeValue;
									char ItemIDStr[10];
									char ItemAmountkStr[10];
									sprintf(ItemIDStr, "%d", ItemID);
									sprintf(ItemAmountkStr, "%d", IteratorItemAmount + ItemValue - RPGTool::Const_MaxItemStack);
									rapidjson::Value newitemIDtypeValue(ItemIDStr, allocator);
									rapidjson::Value newitemAmounttypeValue(ItemAmountkStr, allocator);
									for (int i = 0;i < RPGTool::Const_MaxItemCount;i++)
									{
										auto it = d["ItemID"][i].GetObject().begin();
										if (it->value == "-1")
										{
											Itemindex = i;
											it->name = newitemIDtypeValue;
											it->value = newitemAmounttypeValue;
											break;
										}
									}
								}
								break;
							}
						}
					}
					if (!isFindSameItem)
					{
						//物品庫中沒有此ID，新增ID
						char newItemIDStr[10];
						char newItemAmountkStr[10];
						sprintf(newItemIDStr, "%d", ItemID);
						sprintf(newItemAmountkStr, "%d", ItemValue);
						rapidjson::Value newitemIDtypeValue(newItemIDStr, allocator);
						rapidjson::Value newitemAmounttypeValue(newItemAmountkStr, allocator);
						for (int i = 0;i < RPGTool::Const_MaxItemCount;i++)
						{
							auto it = d["ItemID"][i].GetObject().begin();
							if (it->value == "-1")
							{
								Itemindex = i;
								it->name = newitemIDtypeValue;
								it->value = newitemAmounttypeValue;
								break;
							}
						}
					}
				}
				//寫入存檔
				rapidjson::StringBuffer  buffer;
				rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
				d.Accept(writer);
				FILE* file = fopen(savePath.c_str(), "wb");
				if (file)
				{
					fputs(buffer.GetString(), file);
					fclose(file);
					return Itemindex;
				}
			}
		}
	}
	void MyTools::JsonParser::setEquipmentFromSave(int ItemID, int ItemRank)
	{
		auto savePath = FileUtils::getInstance()->getWritablePath();
		savePath.append("saves.json");
		rapidjson::Document d;
		std::string str = FileUtils::getInstance()->getStringFromFile(savePath.c_str());
		d.Parse<rapidjson::kParseDefaultFlags>(str.c_str());
		if (d.HasParseError())
		{
			
			CCASSERT(false, "save.json wrong format!");
		}
		if (d.IsObject())
		{
			if (d.HasMember("Equipment"))
			{
				//依照CharacterPart順序
				//Hair,    //頭部+血量
				//NeckLace,//項鍊+內力  
				//Belt,    //腰帶+內力
				//Cloth,   //上衣+防禦
				//Pant,    //下衣 + 防禦
				//Weapon   //武器+攻擊力
				//Head     //非可更改項目

				//根據傳進來的物品ID轉換成對應的角色部件
				int characterPart = (int)RPGTool::accordingItemIDToData(ItemID).characterPart;
				for (int i = 0; i < d["Equipment"].Size(); i++) {
					if (i == characterPart)
					{
						rapidjson::Document::AllocatorType& allocator = d.GetAllocator();
						auto it = d["Equipment"][i].GetObject().begin();
						char ItemIDStr[10];
						char ItemRankStr[5];
						sprintf(ItemIDStr, "%d", ItemID);
						sprintf(ItemRankStr, "%d", ItemRank);
						rapidjson::Value itemIDtypeValue(ItemIDStr, allocator);
						rapidjson::Value itemRanktypeValue(ItemRankStr, allocator);
						it->name = itemIDtypeValue;
						it->value = itemRanktypeValue;
						//寫入存檔
						rapidjson::StringBuffer  buffer;
						rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
						d.Accept(writer);
						FILE* file = fopen(savePath.c_str(), "wb");
						if (file)
						{
							fputs(buffer.GetString(), file);
							fclose(file);
						}
						break;
					}
				}
			}
		}
	}
	void MyTools::JsonParser::setItemBagFromSave(int ItemIndex, int ItemID, int ItemRank)
	{
		auto savePath = FileUtils::getInstance()->getWritablePath();
		savePath.append("saves.json");
		rapidjson::Document d;
		std::string str = FileUtils::getInstance()->getStringFromFile(savePath.c_str());
		d.Parse<rapidjson::kParseDefaultFlags>(str.c_str());
		if (d.HasParseError())
		{
			
			CCASSERT(false, "save.json wrong format!");
		}
		if (d.IsObject())
		{
			if (d.HasMember("ItemID"))
			{				
				for (int i = 0; i < d["ItemID"].Size(); i++) {
					//循環背包格數
					if (i == ItemIndex)
					{
						rapidjson::Document::AllocatorType& allocator = d.GetAllocator();
						auto it = d["ItemID"][i].GetObject().begin();
						char ItemIDStr[10];
						char ItemRankStr[5];
						sprintf(ItemIDStr, "%d", ItemID);
						sprintf(ItemRankStr, "%d", ItemRank);
						rapidjson::Value itemIDtypeValue(ItemIDStr, allocator);
						rapidjson::Value itemRanktypeValue(ItemRankStr, allocator);
						(*it).name = itemIDtypeValue;
						(*it).value = itemRanktypeValue;
						//寫入存檔
						rapidjson::StringBuffer  buffer;
						rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
						d.Accept(writer);
						FILE* file = fopen(savePath.c_str(), "wb");
						if (file)
						{
							fputs(buffer.GetString(), file);
							fclose(file);
						}
						break;
					}
				}
			}
		}
	}
	void MyTools::JsonParser::setItemAllDataFromSave(std::vector<std::pair<int, int>>& itemDatas)
	{
		auto savePath = FileUtils::getInstance()->getWritablePath();
		savePath.append("saves.json");
		rapidjson::Document d;
		std::string str = FileUtils::getInstance()->getStringFromFile(savePath.c_str());
		d.Parse<rapidjson::kParseDefaultFlags>(str.c_str());
		if (d.HasParseError())
		{
			
			CCASSERT(false, "save.json wrong format!");
		}
		if (d.IsObject())
		{
			if (d.HasMember("ItemID"))
			{
				for (int i = 0; i < d["ItemID"].Size(); i++) 
				{
					rapidjson::Document::AllocatorType& allocator = d.GetAllocator();
					auto it = d["ItemID"][i].GetObject().begin();
					char ItemIDStr[10];
					char ItemRankStr[5];
					sprintf(ItemIDStr, "%d", itemDatas[i].first);
					sprintf(ItemRankStr, "%d", itemDatas[i].second);
					rapidjson::Value itemIDtypeValue(ItemIDStr, allocator);
					rapidjson::Value itemRanktypeValue(ItemRankStr, allocator);
					(*it).name = itemIDtypeValue;
					(*it).value = itemRanktypeValue;
				}
				//寫入存檔
				rapidjson::StringBuffer  buffer;
				rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
				d.Accept(writer);
				FILE* file = fopen(savePath.c_str(), "wb");
				if (file)
				{
					fputs(buffer.GetString(), file);
					fclose(file);
				}
			}
		}
	}
	std::string MyTools::JsonParser::getDialogString(std::string DialogCode,int index)
	{
		char RealFile[100];
		sprintf(RealFile, "json/Drama_%s.json", JsonParser::getFromSaveLanguage().c_str());
		std::string readDramaResault=ReadDramaJsonFile(RealFile,DialogCode.c_str(), index);
		if (readDramaResault.empty())
		{
			return "";
		}
		else
		{
			return readDramaResault;
		}
	}
	void JsonParser::setSave(const char * first, const char * second, const char* newValue)
	{
		char RealFile[100];
		auto savePath = FileUtils::getInstance()->getWritablePath();
		savePath.append("saves.json");
		SetJsonFile(first, second, (char*)savePath.c_str(), newValue);
	}
	char * JsonParser::fontType()
	{
		std::string Language = getFromSaveLanguage();
		if (Language == "TW")
		{
			return "fonts/chinese.ttf";
		}
		else if (Language == "CN")
		{
			return "fonts/schinese.ttf";
		}
		else if (Language == "EN")
		{
			return "fonts/english.ttf";
		}
		else
		{
			CCASSERT(false, "didnt have this language");
		}
	}
	float JsonParser::fontSize(float Size)
	{
		std::string Language = getFromSaveLanguage();
		if (Language == "TW")
		{
			return Size;
		}
		else if (Language == "CN")
		{
			return Size;
		}
		else if (Language == "EN")
		{
			return Size;
		}
		else
		{
			CCASSERT(false, "didnt have this language");
		}
	}
}