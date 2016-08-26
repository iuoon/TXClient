#pragma once
#ifndef _WELCOME_SCENE_H_
#define _WELCOME_SCENE_H_

#include "cocos2d.h"
#include "LoginScene.h"
USING_NS_CC;

class WelcomeScene : public cocos2d::Layer
{
public:
	WelcomeScene();
	~WelcomeScene();

	static cocos2d::Scene* createScene();

	virtual bool init();

	CREATE_FUNC(WelcomeScene);

	void GotoLogin(float dt);

private:

};



#endif