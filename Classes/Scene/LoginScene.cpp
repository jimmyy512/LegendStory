//#include "LoginScene.h"
//#if (CC_TARGET_PLATFORM == CC_PLATFORM_WIN32)
//	#include "curl/include/win32/curl/curl.h"//window下頭文件位子
//	#pragma comment(lib,"libcurl_imp.lib")//動態連結庫的位子
//#else if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
//	#include "curl\include\android\curl\curl.h"
//#endif
//USING_NS_CC;
//Scene* LoginScene::createScene()
//{
//    // 'scene' is an autorelease object
//    auto scene = Scene::create();
//    
//    // 'layer' is an autorelease object
//    auto layer = LoginScene::create();
//
//    // add layer as a child to scene
//    scene->addChild(layer);
//
//    // return the scene
//    return scene;
//}
//size_t processData(char *ptr, std::size_t size, std::size_t nmemb, std::string *stream)
//{
//	//char* ptr就是返回的服务器数据，服务器echo 1,这里就返回"1"  
//	CCLOG("Writting data");
//	if (stream == NULL)
//	{
//		return 0;
//	}
//	size_t sizes = size * nmemb;
//	//string* ss = (string *)stream;  
//	stream->append(ptr, sizes);
//	return sizes;
//}
//// on "init" you need to initialize your instance
//bool LoginScene::init()
//{
//    if ( !Layer::init() )
//    {
//        return false;
//    }
//    visible = Director::getInstance()->getVisibleSize();
//    origin = Director::getInstance()->getVisibleOrigin();
//
//	auto csbNode = CSLoader::createNode("scene/loginScene.csb");
//	this->addChild(csbNode);
//
//	resaultLabel = Label::create();
//	resaultLabel->setPosition(visible.width / 2, visible.height / 18 * 3);
//	resaultLabel->setTextColor(Color4B::BLACK);
//	resaultLabel->setSystemFontSize(24);
//	csbNode->addChild(resaultLabel);
//	
//	userName = static_cast<cocos2d::ui::TextField*>(csbNode->getChildByName("inputField_UserName"));
//	userPass = static_cast<cocos2d::ui::TextField*>(csbNode->getChildByName("inputField_Password"));
//	loginButton = static_cast<cocos2d::ui::Button*>(csbNode->getChildByName("loginButton"));
//
//	loginButton->addTouchEventListener([this](cocos2d::Ref* sender, cocos2d::ui::Widget::TouchEventType type)
//	{
//		switch (type)
//		{
//			case cocos2d::ui::Widget::TouchEventType::BEGAN:
//				break;
//			case cocos2d::ui::Widget::TouchEventType::MOVED:
//				break;
//			case cocos2d::ui::Widget::TouchEventType::ENDED:
//				if (userName->getString() == "")  //沒有輸入帳號
//				{
//					resaultLabel->setString("User is empty");
//
//					break;
//				}
//				if (userPass->getString() == "") //沒有輸入密碼
//				{
//					resaultLabel->setString("Pass is empty");
//					break;
//				}
//				resaultLabel->setString("click login btn"); //以點擊登入紐
//				curlConnect();
//				break;
//			case cocos2d::ui::Widget::TouchEventType::CANCELED:
//				break;
//			default:
//				break;
//		}
//	});
//
//
//
//    return true;
//}
//
//void LoginScene::curlConnect()
//{
//	std::string strUser = userName->getString();
//	std::string strPass = userPass->getString();
//	CURL* curl = curl_easy_init();
//	if (curl)  //如果初始化成功  
//	{
//		
//		//char postStr[100] = { 0 };
//		//char url[1000] = { "http://localhost:3000/login" };
//		////併接字串  根據玩家輸入的帳號密碼拼出url網址，如同我們前面網址測試的內容一樣
//		//sprintf(postStr, "Account=%s&Password=%s", strUser.c_str(), strPass.c_str());
//		//int res;
//		////網路連接初始化
//		//res = curl_easy_setopt(curl, CURLOPT_URL, url);  //設定要連接的網址， 返回0表示成功  
//		//curl_easy_setopt(curl, CURLOPT_POST, true);
//		//curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, processData);//設定數據接收的方法
//		//curl_easy_setopt(curl, CURLOPT_POSTFIELDS, postStr);
//		//curl_easy_setopt(curl, CURLOPT_WRITEDATA, &recvbuf);
//		//res = curl_easy_perform(curl);//連網請求 若連線伺服器成功回傳0
//		
//		//char url[1000];
//		//sprintf(url, "http://localhost:3000/login?Account=%s&Password=%s", strUser.c_str(), strPass.c_str());
//		//int res;
//		//res = curl_easy_setopt(curl, CURLOPT_URL, url);
//		//curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, processData);
//		//std::string recvbuf;
//		//curl_easy_setopt(curl, CURLOPT_WRITEDATA, &recvbuf);
//		////curl_easy_setopt(curl, CURLOPT_POSTFIELDS);
//		//res = curl_easy_perform(curl);
//		//if (CURLE_OK == res)  //CURLE_OK == 0  
//		//{	
//		//	if (recvbuf.compare("0")==0)
//		//	{ //返回值"1  " 即是帳密正確 這邊網站上是"1" 不過我自己測試是用"1  "才成功
//		//		resaultLabel->setString("Login Success!");
//		//		//Director::getInstance()->replaceScene(SplashScene::createScene());//我自己寫的遊戲場景
//		//	}
//		//	else  //登入失敗  
//		//	{
//		//		resaultLabel->setString("Login Failed!");
//		//	}
//		//}
//		char url[1000];
//		sprintf(url, "http://localhost:3000/login?Account=%s&Password=%s", strUser.c_str(), strPass.c_str());
//		int res;
//		res = curl_easy_setopt(curl, CURLOPT_URL, url);
//		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, processData);
//		std::string recvbuf;
//		curl_easy_setopt(curl, CURLOPT_WRITEDATA, &recvbuf);
//		curl_easy_setopt(curl, CURLOPT_POSTFIELDS);
//		res = curl_easy_perform(curl);
//		if (CURLE_OK == res)  //CURLE_OK == 0  
//		{
//			if (recvbuf.compare("0") == 0)
//			{ //返回值"1  " 即是帳密正確 這邊網站上是"1" 不過我自己測試是用"1  "才成功
//				resaultLabel->setString("Login Success!");
//				//Director::getInstance()->replaceScene(SplashScene::createScene());//我自己寫的遊戲場景
//			}
//			else  //登入失敗  
//			{
//				resaultLabel->setString("Login Failed!");
//			}
//		}
//	}
//	else//連線伺服器失敗
//	{
//		resaultLabel->setString("server down");
//	}
//	curl_easy_cleanup(curl);
//}