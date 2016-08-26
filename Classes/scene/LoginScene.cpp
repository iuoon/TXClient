#include "LoginScene.h"
#include "cocostudio/CocoStudio.h"
#include "ui/CocosGUI.h"
#include "MainScene.h"
#include "LoadingLayer.h"


USING_NS_CC;

using namespace cocostudio::timeline;


LoginScene::LoginScene()
{	
}

LoginScene::~LoginScene()
{
}

cocos2d::Scene * LoginScene::createScene()
{
	auto scene = Scene::create();

	auto layer = LoginScene::create();

	scene->addChild(layer);

	return scene;
}


bool LoginScene::init()
{
	if (!Layer::init())
	{
		return false;
	}

	Size visibleSize = Director::getInstance()->getVisibleSize();
	Vec2 origin = Director::getInstance()->getVisibleOrigin();

	auto rootNode = CSLoader::createNode("scene/LoginScene.csb");

	addChild(rootNode);

	auto start_bt = static_cast<Button*>(rootNode->getChildByName("Button_3"));
	start_bt->addTouchEventListener(CC_CALLBACK_2(LoginScene::GotoGame,this));
	return true;
}

void LoginScene::GotoGame(Ref * pSender, Widget::TouchEventType type)
{		
	auto loadinglayer = Loading::createScene();

//	TransitionScene * res = TransitionCrossFade::create(1.0f, mainlayer);
	switch (type)
	{
	case cocos2d::ui::Widget::TouchEventType::BEGAN:
		break;
	case cocos2d::ui::Widget::TouchEventType::MOVED:
		break;
	case cocos2d::ui::Widget::TouchEventType::ENDED:
		log("touch start game");		
		Director::getInstance()->replaceScene(loadinglayer);
		break;
	case cocos2d::ui::Widget::TouchEventType::CANCELED:
		break;
	default:
		break;
	}
		
}


