#ifndef _MAIN_SCENE_H_
#define _MAIN_SCENE_H_
#include "cocos2d.h"
#include "ui/ChatUI.h"

USING_NS_CC;

class MainScene :public cocos2d::Layer
{

public:
	MainScene();
	~MainScene();

	static cocos2d::Scene* createScene();

	virtual bool init();

	CREATE_FUNC(MainScene);


public:
	ChatUI* chat;

};


#endif
