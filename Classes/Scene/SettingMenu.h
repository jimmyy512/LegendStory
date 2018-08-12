#ifndef __SettingMenu_SCENE_H__
#define __SettingMenu_SCENE_H__
#include "cocos2d.h"
#include "cocostudio\CocoStudio.h"
#include "ui\CocosGUI.h"
class ShakeButton;
class SettingMenu : public cocos2d::Layer
{
	static std::string userAccount;
	static std::string userPassword;
public:
    static cocos2d::Scene* createScene(cocos2d::RenderTexture* capture);
	static size_t processData(char *ptr, std::size_t size, std::size_t nmemb, std::string *stream);
    virtual bool init();
    CREATE_FUNC(SettingMenu);
private:
	//*根據_currentPage變數 來了解頁面計數 並且更新頁面
	void accordingCurrentPageRefresh();
	void ShowSaveRegistPage();
	void ShowSaveLoginPage();
	void ShowReadySavePage();
	void CurlLogin();
	void CurlRegist();
	void CurlUploadSave();
	void CurlDownloadSave();
	virtual void onEnterTransitionDidFinish();
private:
	cocos2d::Size visible;
	cocos2d::Vec2 origin;
	cocos2d::Node* _sceneCSB;
	cocos2d::Node* _accountFather;
	cocos2d::Node* _passwordFather;
	cocos2d::Node* _checkPasswordFather;
	cocos2d::Node* _readySaveFather;
	cocos2d::Label* _titleLabel;
	cocos2d::Label* _accountInforLabel;
	cocos2d::ui::TextField* _accountInput;
	cocos2d::ui::TextField* _passwordInput;
	cocos2d::ui::TextField* _checkPasswordInput;
	ShakeButton* _saveLeftBtn;
	ShakeButton* _saveRightBtn;
	ShakeButton* _logoutBtn;
	//*存檔頁面狀態有 Login(登入) ReadySave(準備存檔) Regist(註冊)
	std::string _SaveState;
	cocos2d::ui::Slider* _musicSlider;
	cocos2d::ui::Slider* _soundSlider;
	cocos2d::ui::RadioButtonGroup* _radioGroup;
	//*當前是第幾頁面
	int _currentPage=0;
	//*頁面總數
	int _maxPage = 2;
	//*開發人員選單開啟點擊次數;
	int _testTouchCount;
};

#endif
