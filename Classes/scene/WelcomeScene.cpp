#include "WelcomeScene.h"
#include "cocostudio/CocoStudio.h"
#include "ui/CocosGUI.h"
using namespace cocostudio::timeline;

WelcomeScene::WelcomeScene()
{
}

WelcomeScene::~WelcomeScene()
{
}

cocos2d::Scene * WelcomeScene::createScene()
{
	auto scene = Scene::create();
	auto layer = WelcomeScene::create();
	scene->addChild(layer);
	return scene;
}

bool WelcomeScene::init()
{
	if (!Layer::init())
	{
		return false;
	}
	auto rootNode = CSLoader::createNode("scene/WelcomeScene.csb");
	addChild(rootNode);
	scheduleOnce(schedule_selector(WelcomeScene::GotoLogin),2.0f); //ÑÓ³ÙÁ½ÃëÖ´ÐÐ	
	return true;
}

void WelcomeScene::GotoLogin(float dt)
{
	auto loginscene = LoginScene::createScene();
	TransitionScene * reScene = TransitionCrossFade::create(1.0f, loginscene);
	Director::getInstance()->replaceScene(reScene);	
}

