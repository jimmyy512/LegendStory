#ifndef __MyJsonParser__H__
#define __MyJsonParser__H__
#include "cocos2d.h"
#include "RPGTools\ItemSprite.h"
namespace MyTools
{
	class JsonParser
	{
	public:
		enum class JsonType
		{
			Save,
			UserInterFace,
			TableCode,
			NPC,
			ItemDOC,
			Mission
		};
		enum class Equipment
		{
			GetID,
			GetValue
		};
	public:
		static std::string getJsonString(const char* first, const char* second, JsonType jsonType);
		//*專門給主選單使用的接口
		static std::string getJsonStringWithOutSave(const char* first, const char* second, JsonType jsonType);
		static std::string getNPCJsonFile(const char* NPCName, const char* Opa, int index = 0);
		static std::string getDialogString(std::string DialogCode, int index);
		//*獲取存檔所有字串
		static std::string getSaveJsonStr();
		static void setSaveJsonStr(std::string newSaveJsonStr);
		static void setSave(const char* first, const char* second, const char* newValue);
		//*ItemSprite class 使用這個API可以獲得物品資訊
		static void getItemDataFromSave(int index, ItemSprite::ItemData& itemData);
		//*重新整理按鈕 使用這個API可以獲得所有物品資訊
		//*並且把所有物品資訊給傳入的vector
		static void getItemAllDataFromSave(std::vector<std::pair<int,int>>& itemDatas);
		//*RPGTool::CharacterPart
		static int getEquipmentFromSave(int characterPart,Equipment equipmentOperator);
		//*獲取當前背包物品數量
		static int getItemCountFromSave();
		//*獲取當前背包物品還有多少空位
		static int getFreeItemSpaceCountFromSave();
		//*傳入物品ID，函數會自動轉換成對應的角色部件
		//*修改裝備中的物品
		static void setEquipmentFromSave(int ItemID,int ItemRank);
		//*傳入要修改的物品在背包中的第幾格，新的ID，新的階級
		//*修改背包中的物品
		static void setItemBagFromSave(int ItemIndex,int ItemID, int ItemRank);
		//*傳入已經有物品所有資料的vector 就能更新所有物品資訊
		static void setItemAllDataFromSave(std::vector<std::pair<int, int>>& itemDatas);
		//*RPGTool::ItemID   第二個參數根據第一個參數有不同效果
		//*如果ItemID < _STACK_LINE 代表ItemValue=ItemRank
		//*如果ItemID > _STACK_LINE 代表ItemValue=ItemAmount
		//*增加物品成功回傳把物品加到第幾格物品，增加失敗回傳-1
		static int AddItemToSave(int ItemID, int ItemValue);
		//*移除物品
		static void removeItemFromSave(int index);
		//*根據存檔裡的語言，來給予不同字形
		static char* fontType();
		//*根據存檔裡的語言，來給予不同字形大小
		static float fontSize(float Size);
		static void buildNewSave();
		static bool haveSaveFile();
	private:
		static char* accordingEnumGivingPath(JsonType jsonType);
		static std::string getFromSaveLanguage();
		static std::string ReadDramaJsonFile(const char* fileName, const char* DialogCode,int index);
		static std::string ReadJsonFile(const char* first, const char* second, const char* fileName);
		//*針對讀取TableCode讀取時設計的接口，當second元素找不到時 會回傳second字串。
		static std::string ReadTableCodeJsonFile(const char* first, const char* second, const char* fileName);
		static void SetJsonFile(const char* first, const char* second, const char* fileName, const char* newValue);

	};
}
#endif