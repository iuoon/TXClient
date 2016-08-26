#pragma once
#ifndef _CHAT_UI_H_
#define _CHAT_UI_H_

#include "cocos2d.h"
#include "ui\UIRichText.h"
#include <string.h>


USING_NS_CC;
using namespace cocos2d::ui;
using namespace std;

class ChatUI :public cocos2d::Layer
{
public:
	ChatUI();
	~ChatUI();

	static cocos2d::Scene* createScene();

	virtual bool init();

	CREATE_FUNC(ChatUI);

	void initRichEdit();

	RichText* getChatMsg(int channel, string roleName, string chatMsg, char* signs);

private:
	RichText* _richBugle;//À®°È

};



#endif