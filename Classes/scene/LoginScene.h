#pragma once
#ifndef _LOGIN_SCENE_H_
#define _LOGIN_SCENE_H_

#include "cocos2d.h"
#include "ui/UIWidget.h"
USING_NS_CC;
using namespace cocos2d::ui;

class LoginScene :public cocos2d::Layer
{
public:
	LoginScene();
	~LoginScene();

	static cocos2d::Scene* createScene();

	virtual bool init();

	CREATE_FUNC(LoginScene);

	void GotoGame(Ref*pSender, Widget::TouchEventType type);

private:
	Scene * map;

};

#endif