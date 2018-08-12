#include "SettingMenu.h"
#include "ui\MyButton.h"
#include "tools\JsonParser.h"
#include "tools\AudioManagement.h"
#include "Scene\DeveloperMenuScene.h"
#include "UI\BattleMode\BattleHUDlayer.h"
#include "UI\ScrollMode\HUDlayer.h"
#include "Npc\NPC_AI.h"
#include "TipsLabel.h"
#if (CC_TARGET_PLATFORM == CC_PLATFORM_WIN32)
#include "curl/include/win32/curl/curl.h"//window下頭文件位子
#pragma comment(lib,"libcurl.lib")//動態連結庫的位子
#else if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
#include "curl\include\android\curl\curl.h"
#endif
USING_NS_CC;
using namespace cocos2d::ui;
using namespace cocos2d::experimental;
using namespace MyTools;
std::string SettingMenu::userAccount = "";
std::string SettingMenu::userPassword = "";
size_t SettingMenu::processData(char *ptr, std::size_t size, std::size_t nmemb, std::string *stream)
{
	if (stream == NULL)
	{
		return 0;
	}
	size_t sizes = size * nmemb;
	stream->append(ptr, sizes);
	return sizes;
}
void SettingMenu::onEnterTransitionDidFinish()
{
	Layer::onEnterTransitionDidFinish();
	if (ScrollMode::HUDlayer::getHUDlayer() != nullptr)
	{
		ScrollMode::HUDlayer::getHUDlayer()->_isReplaceingScene = false;
	}
	else if (BattleMode::BattleHUDlayer::getBattleHUDlayer() != nullptr)
	{
		BattleMode::BattleHUDlayer::getBattleHUDlayer()->_isReplaceingScene = false;
	}
}
Scene* SettingMenu::createScene(cocos2d::RenderTexture* capture)
{
	Size visible = Director::getInstance()->getVisibleSize();
	Vec2 origin = Director::getInstance()->getVisibleOrigin();
	auto scene = Scene::create();
	auto layer = SettingMenu::create();
	scene->addChild(layer, 1);
	
	if (capture!=nullptr)
	{
		auto pause_background = Sprite::createWithTexture(capture->getSprite()->getTexture());
		pause_background->setFlipY(true);
		pause_background->setColor(Color3B::GRAY);
		pause_background->setPosition(Vec2(visible.width / 2 + origin.x, visible.height / 2 + origin.y));
		layer->addChild(pause_background, 0);
	}
	return scene;
}
bool SettingMenu::init()
{
    if ( !Layer::init() )
    {
        return false;
    }
    visible = Director::getInstance()->getVisibleSize();
    origin = Director::getInstance()->getVisibleOrigin();
	this->_sceneCSB= CSLoader::createNode("settingScene/settingScene.csb");
	this->addChild(_sceneCSB, 1);

	//開發人員模式  點擊標題五次可以回到開發人員選單
	_testTouchCount = 0;
	auto testTouch=this->_sceneCSB->getChildByName("TestTouch");
	auto testTouchListener = EventListenerTouchOneByOne::create();
	testTouchListener->onTouchBegan = [&](Touch* touch,Event* event)
	{
		if (event->getCurrentTarget()->getBoundingBox().containsPoint(touch->getLocation()))
		{//如果點擊了標題
			_testTouchCount++;
			if (_testTouchCount == 5)
			{
				AudioManagement::getInstance()->clearAudioManagement();
				Director::getInstance()->popToRootScene();
				Director::getInstance()->replaceScene(DeveloperMenuScene::createScene());
			}
		}
		return true;
	};
	Director::getInstance()->getEventDispatcher()->addEventListenerWithSceneGraphPriority(testTouchListener, testTouch);
	//Audio Layer
	this->_sceneCSB->getChildByName("AudioLayer")->setVisible(true);

	auto MusicItemLabel= Label::createWithTTF(JsonParser::getJsonString("SettingMenu", "MusicItemLabel", JsonParser::JsonType::UserInterFace),
		JsonParser::fontType(), JsonParser::fontSize(20));
	MusicItemLabel->setColor(Color3B::BLACK);
	this->_sceneCSB->getChildByName("AudioLayer")->getChildByName("MusicItemLabel")->addChild(MusicItemLabel, 2);

	auto SoundItemLabel = Label::createWithTTF(JsonParser::getJsonString("SettingMenu", "SoundItemLabel", JsonParser::JsonType::UserInterFace),
		JsonParser::fontType(), JsonParser::fontSize(20));
	SoundItemLabel->setColor(Color3B::BLACK);
	this->_sceneCSB->getChildByName("AudioLayer")->getChildByName("SoundItemLabel")->addChild(SoundItemLabel, 2);

	_musicSlider = (cocos2d::ui::Slider*) _sceneCSB->getChildByName("AudioLayer")->getChildByName("swordGrip")->getChildByName("Slider");
	_musicSlider->setPercent(atoi(JsonParser::getJsonString("Optional","MusicVolume",JsonParser::JsonType::Save).c_str()));

	_soundSlider = (cocos2d::ui::Slider*)_sceneCSB->getChildByName("AudioLayer")->getChildByName("swordGrip2")->getChildByName("Slider2");
	_soundSlider->setPercent(atoi(JsonParser::getJsonString("Optional", "SoundVolume", JsonParser::JsonType::Save).c_str()));
	

	//Language Layer
	this->_sceneCSB->getChildByName("LanguageLayer")->setVisible(false);
	_radioGroup = RadioButtonGroup::create();
	this->addChild(_radioGroup,10);
	for (int i = 0;i < 3;i++)
	{
		RadioButton* radioButton = RadioButton::create("ui/radio_button_off.png", "ui/radio_button_on.png");
		if (i == 0)
			this->_sceneCSB->getChildByName("LanguageLayer")->getChildByName("TCHRadio")->addChild(radioButton);
		if (i == 1)
			this->_sceneCSB->getChildByName("LanguageLayer")->getChildByName("SCHRadio")->addChild(radioButton);
		if (i == 2)
			this->_sceneCSB->getChildByName("LanguageLayer")->getChildByName("ENGRadio")->addChild(radioButton);
		radioButton->setScale(1.2f);
		_radioGroup->addRadioButton(radioButton);
	}
	std::string language=JsonParser::getJsonString("Optional", "Language", JsonParser::JsonType::Save);
	if (language == "TW")
		_radioGroup->setSelectedButton(_radioGroup->getRadioButtonByIndex(0));
	else if(language=="CN")
		_radioGroup->setSelectedButton(_radioGroup->getRadioButtonByIndex(1));
	else
		_radioGroup->setSelectedButton(_radioGroup->getRadioButtonByIndex(2));

	//Save Layer
	this->_SaveState = "Login";
	this->_sceneCSB->getChildByName("SaveLayer")->setVisible(false);
	_accountFather = this->_sceneCSB->getChildByName("SaveLayer")->getChildByName("AccountFather");
	_passwordFather = this->_sceneCSB->getChildByName("SaveLayer")->getChildByName("PasswordFather");
	_checkPasswordFather = this->_sceneCSB->getChildByName("SaveLayer")->getChildByName("CheckPasswordFather");
	_readySaveFather= this->_sceneCSB->getChildByName("SaveLayer")->getChildByName("ReadySaveFather");
	_readySaveFather->setVisible(false);

	_accountInput = (cocos2d::ui::TextField*)_accountFather->getChildByName("AccountInput");
	_passwordInput = (cocos2d::ui::TextField*)_passwordFather->getChildByName("PasswordInput");
	_checkPasswordInput = (cocos2d::ui::TextField*)_checkPasswordFather->getChildByName("CheckPasswordInput");

	
	auto accountLabel = Label::createWithTTF(JsonParser::getJsonString("SettingMenu", "Account", JsonParser::JsonType::UserInterFace),
		JsonParser::fontType(), JsonParser::fontSize(14));
	accountLabel->setColor(Color3B::WHITE);
	accountLabel->enableOutline(Color4B::BLACK, 1);
	accountLabel->setPosition(_accountFather->getChildByName("AccountLabelPos")->getPosition());
	_accountFather->addChild(accountLabel);

	auto PasswordLabel = Label::createWithTTF(JsonParser::getJsonString("SettingMenu", "Password", JsonParser::JsonType::UserInterFace),
		JsonParser::fontType(), JsonParser::fontSize(14));
	PasswordLabel->setColor(Color3B::WHITE);
	PasswordLabel->enableOutline(Color4B::BLACK, 1);
	PasswordLabel->setPosition(_passwordFather->getChildByName("PasswordLabelPos")->getPosition());
	_passwordFather->addChild(PasswordLabel);

	auto CheckPasswordLabel = Label::createWithTTF(JsonParser::getJsonString("SettingMenu", "CheckPassword", JsonParser::JsonType::UserInterFace),
		JsonParser::fontType(), JsonParser::fontSize(14));
	CheckPasswordLabel->setColor(Color3B::WHITE);
	CheckPasswordLabel->enableOutline(Color4B::BLACK, 1);
	CheckPasswordLabel->setPosition(_checkPasswordFather->getChildByName("CheckPasswordLabelPos")->getPosition());
	_checkPasswordFather->addChild(CheckPasswordLabel);

	_accountInforLabel= Label::createWithTTF("NULL",JsonParser::fontType(), JsonParser::fontSize(14));
	_accountInforLabel->setColor(Color3B::WHITE);
	_accountInforLabel->enableOutline(Color4B::BLACK, 1);
	_accountInforLabel->setPosition(_readySaveFather->getChildByName("RS_AccountInforPos")->getPosition());
	_readySaveFather->addChild(_accountInforLabel);

	auto LogoutButton = ShakeButton::create(JsonParser::getJsonString("SettingMenu", "Logout", JsonParser::JsonType::UserInterFace),
		JsonParser::fontType(), JsonParser::fontSize(10), Color4B::BLACK, "ui/Logout.png",
		[&](ShakeButton* shakeBtn)
		{
			if (this->_SaveState == "ReadySave")
			{//登出
				this->_accountInput->setString("");
				this->_passwordInput->setString("");
				this->_checkPasswordInput->setString("");
				this->ShowSaveLoginPage();
			}
		}
	);
	LogoutButton->setScale(1.5f);
	LogoutButton->setPosition(_readySaveFather->getChildByName("RS_LogoutPos")->getPosition());
	_readySaveFather->addChild(LogoutButton);

	_saveLeftBtn=ShakeButton::create(JsonParser::getJsonString("SettingMenu","Login",JsonParser::JsonType::UserInterFace),
		JsonParser::fontType(), JsonParser::fontSize(10), Color4B::BLACK, "ui/SaveBtn.png",
		[&](ShakeButton* shakeBtn)
		{
			if (_currentPage == 2)
			{
				CCLOG("LeftBtnClick");
				if (this->_SaveState == "Login")
				{//登入
					CurlLogin();
				}
				else if (this->_SaveState == "Regist")
				{//提交註冊資料
					CurlRegist();
				}
				else if (this->_SaveState == "ReadySave")
				{//上傳存檔
					CurlUploadSave();
				}
				else
				{
					throw "SettingMenu _SaveState no this State.";
				}
			}
		}
	);
	_saveLeftBtn->setScale(1.5f);
	_saveLeftBtn->setPosition(this->_sceneCSB->getChildByName("SaveLayer")->getChildByName("SaveLeftBtnPos")->getPosition());
	this->_sceneCSB->getChildByName("SaveLayer")->addChild(_saveLeftBtn);

	_saveRightBtn = ShakeButton::create(JsonParser::getJsonString("SettingMenu", "Regist", JsonParser::JsonType::UserInterFace),
		JsonParser::fontType(), JsonParser::fontSize(10), Color4B::BLACK, "ui/SaveBtn.png",
		[&](ShakeButton* shakeBtn)
		{
			if (_currentPage == 2)
			{
				CCLOG("RightBtnClick");
				if (this->_SaveState == "Login")
				{//進入註冊頁面
					ShowSaveRegistPage();
				}
				else if (this->_SaveState == "Regist")
				{//返回登入頁面
					ShowSaveLoginPage();
				}
				else if (this->_SaveState == "ReadySave")
				{//繼承存檔
					CurlDownloadSave();
				}
				else
				{
					throw "SettingMenu _SaveState no this State.";
				}
			}
		}
	);
	_saveRightBtn->setScale(1.5f);
	_saveRightBtn->setPosition(this->_sceneCSB->getChildByName("SaveLayer")->getChildByName("SaveRightBtnPos")->getPosition());
	this->_sceneCSB->getChildByName("SaveLayer")->addChild(_saveRightBtn);

	//normal controlbtn
	//標題  預設畫面是音頻設定
	_titleLabel = Label::createWithTTF(JsonParser::getJsonString("SettingMenu", "TitleAudio", JsonParser::JsonType::UserInterFace),
		JsonParser::fontType(), JsonParser::fontSize(24));
	_titleLabel->setColor(Color3B::BLACK);
	this->_sceneCSB->getChildByName("TitleLabel")->addChild(_titleLabel);
	//左箭頭
	auto  LeftClickBtn = (Button*)this->_sceneCSB->getChildByName("LeftClick");
	LeftClickBtn->addTouchEventListener([this](Ref* ref, Widget::TouchEventType touchType)
	{
		switch (touchType)
		{
		case cocos2d::ui::Widget::TouchEventType::BEGAN:
			if (_currentPage == 0)
				_currentPage = _maxPage;
			else
				_currentPage--;
			this->accordingCurrentPageRefresh();
			break;
		default:
			break;
		}
	});
	//右箭頭
	auto  RightClickBtn = (Button*)this->_sceneCSB->getChildByName("RightClick");
	RightClickBtn->addTouchEventListener([this](Ref* ref, Widget::TouchEventType touchType)
	{
		switch (touchType)
		{
		case cocos2d::ui::Widget::TouchEventType::BEGAN:
			if (_currentPage == _maxPage)
				_currentPage = 0;
			else
				_currentPage++;
			this->accordingCurrentPageRefresh();
			break;
		default:
			break;
		}
	});
	
	auto submitBtn=ShakeButton::create(JsonParser::getJsonString("SettingMenu","Submit",JsonParser::JsonType::UserInterFace),
		JsonParser::fontType(),	JsonParser::fontSize(26),Color4B::BLACK,"ui/settingSubmit.png",
	[this](ShakeButton* shakeBtn)
	{
		//audio slider
		char buffer[20];
		sprintf(buffer, "%d", _musicSlider->getPercent());
		JsonParser::setSave("Optional", "MusicVolume", buffer);
		memset(buffer, 0, 20);
		sprintf(buffer, "%d", _soundSlider->getPercent());
		JsonParser::setSave("Optional", "SoundVolume", buffer);

		//language
		int RadioButtonSelectedIndex = _radioGroup->getSelectedButtonIndex();
		if(RadioButtonSelectedIndex ==0)
			JsonParser::setSave("Optional", "Language", "TW");
		if (RadioButtonSelectedIndex == 1)
			JsonParser::setSave("Optional", "Language", "CN");
		if (RadioButtonSelectedIndex == 2)
			JsonParser::setSave("Optional", "Language", "EN");
		//NPC更新label font
		NPC_AI::getInstance()->updateNPCLabelFontType();
		Director::getInstance()->popScene([](Scene* scene)
		{
			return TransitionPageTurn::create(0.4f, scene, false);
		});

		//音頻處理
		AudioManagement::getInstance()->updateNewAudioVolume();
		AudioManagement::getInstance()->resumeAllAudio();
	});
	submitBtn->setScale(0.8f);
	submitBtn->setPosition(_sceneCSB->getChildByName("submitBtn")->getPosition());
	this->addChild(submitBtn,2);

	auto returnBtn = ShakeButton::create(JsonParser::getJsonString("SettingMenu", "Return", JsonParser::JsonType::UserInterFace),
		JsonParser::fontType(), JsonParser::fontSize(26), Color4B::BLACK, "ui/settingSubmit.png",
		[](ShakeButton* shakeBtn)
	{
		AudioManagement::getInstance()->resumeAllAudio();
		Director::getInstance()->popScene([](Scene* scene)
		{
			return TransitionPageTurn::create(0.4f, scene, false);
		});
	});
	returnBtn->setScale(0.8f);
	returnBtn->setPosition(_sceneCSB->getChildByName("returnBtn")->getPosition());
	this->addChild(returnBtn, 2);
	
    return true;
}
void SettingMenu::ShowSaveRegistPage()
{
	this->_SaveState = "Regist";
	this->_saveLeftBtn->setBtnString(JsonParser::getJsonString("SettingMenu", "SubmitRegist", JsonParser::JsonType::UserInterFace));
	this->_saveRightBtn->setBtnString(JsonParser::getJsonString("SettingMenu", "Return", JsonParser::JsonType::UserInterFace));
	this->_accountFather->setVisible(true);
	this->_passwordFather->setVisible(true);
	this->_checkPasswordFather->setVisible(true);
	this->_readySaveFather->setVisible(false);
	this->_accountFather->setPosition(this->_sceneCSB->getChildByName("SaveLayer")->getChildByName("REG_AccountPos")->getPosition());
	this->_passwordFather->setPosition(this->_sceneCSB->getChildByName("SaveLayer")->getChildByName("REG_PasswordPos")->getPosition());
	this->_checkPasswordFather->setPosition(this->_sceneCSB->getChildByName("SaveLayer")->getChildByName("REG_CheckPasswordPos")->getPosition());
}
void SettingMenu::ShowSaveLoginPage()
{
	this->_SaveState = "Login";
	this->_saveLeftBtn->setBtnString(JsonParser::getJsonString("SettingMenu", "Login", JsonParser::JsonType::UserInterFace));
	this->_saveRightBtn->setBtnString(JsonParser::getJsonString("SettingMenu", "Regist", JsonParser::JsonType::UserInterFace));
	this->_accountFather->setVisible(true);
	this->_passwordFather->setVisible(true);
	this->_checkPasswordFather->setVisible(false);
	this->_readySaveFather->setVisible(false);
	this->_accountFather->setPosition(this->_sceneCSB->getChildByName("SaveLayer")->getChildByName("LOG_AccountPos")->getPosition());
	this->_passwordFather->setPosition(this->_sceneCSB->getChildByName("SaveLayer")->getChildByName("LOG_PasswordPos")->getPosition());
}
void SettingMenu::ShowReadySavePage()
{
	this->_SaveState = "ReadySave";
	this->_readySaveFather->setVisible(true);
	std::string accountInforStr;
	accountInforStr += JsonParser::getJsonString("SettingMenu", "AccountInforWelcome", JsonParser::JsonType::UserInterFace);
	accountInforStr += " ";
	accountInforStr += userAccount;
	accountInforStr += " ";
	accountInforStr += JsonParser::getJsonString("SettingMenu", "AccountInforMember", JsonParser::JsonType::UserInterFace);
	this->_accountInforLabel->setString(accountInforStr);
	this->_accountFather->setVisible(false);
	this->_passwordFather->setVisible(false);
	this->_checkPasswordFather->setVisible(false);
	this->_saveLeftBtn->setBtnString(JsonParser::getJsonString("SettingMenu", "UploadSave", JsonParser::JsonType::UserInterFace));
	this->_saveRightBtn->setBtnString(JsonParser::getJsonString("SettingMenu", "DownloadSave", JsonParser::JsonType::UserInterFace));
}
void SettingMenu::CurlLogin()
{
	std::string accountStr = this->_accountInput->getString();
	std::string passwordStr = this->_passwordInput->getString();
	std::string checkPasswordStr = this->_checkPasswordInput->getString();
	CURL* curl = curl_easy_init();
	if (curl)  //如果初始化成功  
	{
		//無效用法
		//char postStr[100] = { 0 };
		//char url[1000] = { "http://localhost:3000/Login" };
		////併接字串  根據玩家輸入的帳號密碼拼出url網址，如同我們前面網址測試的內容一樣
		//sprintf(postStr, "Account=%s&Password=%s", "jim", "123");
		//int res;
		////網路連接初始化
		//res = curl_easy_setopt(curl, CURLOPT_URL, url);  //設定要連接的網址， 返回0表示成功  
		//curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, processData);//設定數據接收的方法
		//curl_easy_setopt(curl, CURLOPT_POSTFIELDS, postStr);
		//curl_easy_setopt(curl, CURLOPT_WRITEDATA, &recvbuf);
		//res = curl_easy_perform(curl);//連網請求 若連線伺服器成功回傳0
		//curl_easy_cleanup(curl);
		std::string recvbuf;
		int res;
		char postStr[1000] = { 0 };
		char url[50] = { "http://122.116.29.95:3000/Login" };
		//併接字串  根據玩家輸入的帳號密碼拼出url網址，如同我們前面網址測試的內容一樣
		sprintf(postStr, "%s?Account=%s&Password=%s", url, accountStr.c_str(), passwordStr.c_str());
		//sprintf(postStr, "%s?Account=%s&Password=%s", url, "jim", "123");
		curl_easy_setopt(curl, CURLOPT_URL, postStr);
		curl_easy_setopt(curl, CURLOPT_POST, true); 
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, processData);   
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, &recvbuf);      
		res = curl_easy_perform(curl);
		if (CURLE_OK == res) 
		{
			if (recvbuf.compare("0") == 0)
			{
				userAccount = accountStr.c_str();
				userPassword = passwordStr.c_str();
				ShowReadySavePage();
			}
			else if(recvbuf.compare("1") == 0)
			{
				TipsLabel::showTip(JsonParser::getJsonString("SettingMenu", "WrongUser", JsonParser::JsonType::UserInterFace), this, 24, false);
			}
			else
			{
				TipsLabel::showTip(JsonParser::getJsonString("SettingMenu", "didntGetresFromServer", JsonParser::JsonType::UserInterFace), this, 24, false);
			}
		}
		else
		{
			TipsLabel::showTip(JsonParser::getJsonString("SettingMenu", "ServerDown", JsonParser::JsonType::UserInterFace), this, 24, false);
		}
	}
	else//連線伺服器失敗
	{
		TipsLabel::showTip(JsonParser::getJsonString("SettingMenu", "InternetConfigError", JsonParser::JsonType::UserInterFace), this, 24, false);
	}
	curl_easy_cleanup(curl);
}
void SettingMenu::CurlRegist()
{
	std::string accountStr = this->_accountInput->getString();
	std::string passwordStr = this->_passwordInput->getString();
	std::string checkPasswordStr = this->_checkPasswordInput->getString();
	if (passwordStr != checkPasswordStr && this->_SaveState == "Regist")
	{
		TipsLabel::showTip(JsonParser::getJsonString("SettingMenu", "NotSame", JsonParser::JsonType::UserInterFace), this, 24, false);
	}
	else if (accountStr == "")
	{
		TipsLabel::showTip(JsonParser::getJsonString("SettingMenu", "AccountEmpty", JsonParser::JsonType::UserInterFace), this, 24, false);
	}
	else if (passwordStr == "")
	{
		TipsLabel::showTip(JsonParser::getJsonString("SettingMenu", "PasswordEmpty", JsonParser::JsonType::UserInterFace), this, 24, false);
	}
	else if (checkPasswordStr == "")
	{
		TipsLabel::showTip(JsonParser::getJsonString("SettingMenu", "CheckPasswordEmpty", JsonParser::JsonType::UserInterFace), this, 24, false);
	}
	else
	{
		CURL* curl = curl_easy_init();
		if (curl)  //如果初始化成功  
		{
			std::string recvbuf;
			int res;
			char postStr[1000] = { 0 };
			char url[50] = { "http://122.116.29.95:3000/Reg" };
			//併接字串  根據玩家輸入的帳號密碼拼出url網址，如同我們前面網址測試的內容一樣
			sprintf(postStr, "%s?Account=%s&Password=%s", url, accountStr.c_str(), passwordStr.c_str());
			curl_easy_setopt(curl, CURLOPT_URL, postStr);
			curl_easy_setopt(curl, CURLOPT_POST, true);
			curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, processData);
			curl_easy_setopt(curl, CURLOPT_WRITEDATA, &recvbuf);
			curl_easy_setopt(curl, CURLOPT_PROXYPORT, "3000");
			curl_easy_setopt(curl, CURLOPT_TIMEOUT, 5);
			
			res = curl_easy_perform(curl);
			if (CURLE_OK == res)
			{
				if (recvbuf.compare("0") == 0)
				{
					userAccount = accountStr.c_str();
					userPassword = passwordStr.c_str();
					ShowReadySavePage();
				}
				else if (recvbuf.compare("1") == 0)
				{
					TipsLabel::showTip(JsonParser::getJsonString("SettingMenu", "AlreadyRegist", JsonParser::JsonType::UserInterFace), this, 24, false);
				}
				else
				{
					TipsLabel::showTip(JsonParser::getJsonString("SettingMenu", "didntGetresFromServer", JsonParser::JsonType::UserInterFace), this, 24, false);
				}
			}
			else
			{
				char errorStr[50];
				sprintf(errorStr, "%s (%d)", JsonParser::getJsonString("SettingMenu", "ServerDown", JsonParser::JsonType::UserInterFace).c_str(), res);
				TipsLabel::showTip(errorStr, this, 20, false);
			}
		}
		else//連線伺服器失敗
		{
			TipsLabel::showTip(JsonParser::getJsonString("SettingMenu", "InternetConfigError", JsonParser::JsonType::UserInterFace), this, 24, false);
		}
		curl_easy_cleanup(curl);
	}
}
void SettingMenu::CurlUploadSave()
{
	CURL* curl = curl_easy_init();
	if (curl)  //如果初始化成功  
	{
		std::string recvbuf;
		int res;
		char postStr[10000] = { 0 };
		char url[50] = { "http://122.116.29.95:3000/UploadSave" };
		//併接字串  根據玩家輸入的帳號密碼拼出url網址，如同我們前面網址測試的內容一樣
		sprintf(postStr, "%s?Account=%s&Password=%s&SaveStr=%s",
			url, userAccount.c_str(), userPassword.c_str(),JsonParser::getSaveJsonStr().c_str());
		curl_easy_setopt(curl, CURLOPT_URL, postStr);
		curl_easy_setopt(curl, CURLOPT_POST, true);
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, processData);
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, &recvbuf);
		res = curl_easy_perform(curl);
		if (CURLE_OK == res)
		{
			if (recvbuf.compare("0") == 0)
			{
				TipsLabel::showTip(JsonParser::getJsonString("SettingMenu", "UploadSaveSuccess", JsonParser::JsonType::UserInterFace), this, 24, false);
			}
			else if (recvbuf.compare("1") == 0)
			{
				TipsLabel::showTip(JsonParser::getJsonString("SettingMenu", "UploadSaveFail", JsonParser::JsonType::UserInterFace), this, 24, false);
			}
			else
			{
				TipsLabel::showTip(JsonParser::getJsonString("SettingMenu", "didntGetresFromServer", JsonParser::JsonType::UserInterFace), this, 24, false);
			}
		}
		else
		{
			TipsLabel::showTip(JsonParser::getJsonString("SettingMenu", "ServerDown", JsonParser::JsonType::UserInterFace), this, 24, false);
		}
	}
	else//連線伺服器失敗
	{
		TipsLabel::showTip(JsonParser::getJsonString("SettingMenu", "InternetConfigError", JsonParser::JsonType::UserInterFace), this, 24, false);
	}
	curl_easy_cleanup(curl);
}
void SettingMenu::CurlDownloadSave()
{
	CURL* curl = curl_easy_init();
	if (curl)  //如果初始化成功  
	{
		std::string recvbuf;
		int res;
		char postStr[10000] = { 0 };
		char url[50] = { "http://122.116.29.95:3000/DownloadSave" };
		//併接字串  根據玩家輸入的帳號密碼拼出url網址，如同我們前面網址測試的內容一樣
		sprintf(postStr, "%s?Account=%s&Password=%s",
			url, userAccount.c_str(), userPassword.c_str());
		curl_easy_setopt(curl, CURLOPT_URL, postStr);
		curl_easy_setopt(curl, CURLOPT_POST, true);
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, processData);
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, &recvbuf);
		res = curl_easy_perform(curl);
		if (CURLE_OK == res)
		{
			if (recvbuf.compare("1") == 0)
			{
				TipsLabel::showTip(JsonParser::getJsonString("SettingMenu", "DownloadSaveFail", JsonParser::JsonType::UserInterFace), this, 24, false);
			}
			else
			{
				TipsLabel::showTip(JsonParser::getJsonString("SettingMenu", "DownloadSaveSuccess", JsonParser::JsonType::UserInterFace), this, 24, false);
				JsonParser::setSaveJsonStr(recvbuf);
				Director::getInstance()->end();
			}
		}
		else
		{
			TipsLabel::showTip(JsonParser::getJsonString("SettingMenu", "ServerDown", JsonParser::JsonType::UserInterFace), this, 24, false);
		}
	}
	else//連線伺服器失敗
	{
		TipsLabel::showTip(JsonParser::getJsonString("SettingMenu", "InternetConfigError", JsonParser::JsonType::UserInterFace), this, 24, false);
	}
	curl_easy_cleanup(curl);
}
void SettingMenu::accordingCurrentPageRefresh()
{
	if (_currentPage == 0)
	{
		this->_sceneCSB->getChildByName("AudioLayer")->setVisible(true);
		this->_sceneCSB->getChildByName("LanguageLayer")->setVisible(false);
		this->_sceneCSB->getChildByName("SaveLayer")->setVisible(false);
		_titleLabel->setString(JsonParser::getJsonString("SettingMenu", "TitleAudio", JsonParser::JsonType::UserInterFace));
	}
	else if (_currentPage == 1)
	{
		this->_sceneCSB->getChildByName("AudioLayer")->setVisible(false);
		this->_sceneCSB->getChildByName("LanguageLayer")->setVisible(true);
		this->_sceneCSB->getChildByName("SaveLayer")->setVisible(false);
		_titleLabel->setString(JsonParser::getJsonString("SettingMenu", "TitleLanguage", JsonParser::JsonType::UserInterFace));
	}
	else if (_currentPage == 2)
	{
		this->_sceneCSB->getChildByName("AudioLayer")->setVisible(false);
		this->_sceneCSB->getChildByName("LanguageLayer")->setVisible(false);
		this->_sceneCSB->getChildByName("SaveLayer")->setVisible(true);
		_titleLabel->setString(JsonParser::getJsonString("SettingMenu", "TitleSave", JsonParser::JsonType::UserInterFace));
	}
}