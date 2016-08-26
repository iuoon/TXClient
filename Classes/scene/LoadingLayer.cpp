#include "LoadingLayer.h"
#include "cocostudio/CocoStudio.h"
#include "ui/CocosGUI.h"
#include "MainScene.h"
#include "common/Tags.h"


Loading::Loading()
{
}

Loading::~Loading()
{
}

cocos2d::Scene * Loading::createScene()
{
	auto scene = Scene::create();

	auto layer = Loading::create();

	scene->addChild(layer);

	return scene;
}


bool Loading::init()
{
	if (!Layer::init())
	{
		return false;
	}

	Size visibleSize = Director::getInstance()->getVisibleSize();
	Vec2 origin = Director::getInstance()->getVisibleOrigin();

	rootNode = CSLoader::createNode("scene/LoadingLayer.csb");
	addChild(rootNode);

	scheduleUpdate();//调用每帧更新事件
	return true;
}

void Loading::update(float delta)
{
	
	if (_count < 100)
	{
		_count++;			
	}
	auto loadingBar = static_cast<LoadingBar*>(rootNode->getChildByName("LoadingBar_1"));
	auto progresstext = static_cast<Text*>(rootNode->getChildByName("Text_1"));
	loadingBar->setPercent(_count);
	char s[10];
	sprintf(s, "%d%%", _count);
	progresstext->setString(s);

	if (_count>=100)
	{
		this->GotoScene(tag1);//进入西郊场景
	}
}

void Loading::GotoScene(int tag)
{
	log("%d", tag);
	switch (tag)
	{
	case 1: {
		auto mainlayer = MainScene::createScene();
		Director::getInstance()->replaceScene(mainlayer);
		break;
	}
	case 2:
	{
		//第二个场景待开发
		break;
	}
	default: {
		break;
	}
	}	
}

