#include "ui/ChatUI.h"
#include "util/FontChina.h"
#include "ui/UIText.h"


ChatUI::ChatUI()
{
}

ChatUI::~ChatUI()
{
}

cocos2d::Scene * ChatUI::createScene()
{
	auto scene = Scene::create();
	auto layer = ChatUI::create();
	scene->addChild(layer);
	return scene;
}


bool ChatUI::init()
{
	if (!Layer::init())
	{
		return false;
	}
	this->initRichEdit();
	return true;
}


void ChatUI::initRichEdit()
{
	
}

RichText* ChatUI::getChatMsg(int channel, string  roleName, string  chatMsg, char * signs)
{
	string chanStr = "【当前】";
	string siz = "：";
	int msglen = chatMsg.size() + siz.size()+ chanStr.size() + roleName.size();
	int s = msglen / 40;
	if (msglen%40>0)
	{
		s += 1;
	}
	RichText* _richChat = RichText::create();
	_richChat->ignoreContentAdaptWithSize(false);
	if (s==1)
	{
		_richChat->setContentSize(Size(300, 18 * s));
	}
	else {
		_richChat->setContentSize(Size(300, 15 * s+3));
	}
		
	RichElementText* res = new RichElementText();
	RichElementText* resrole = new RichElementText();
	
	if (channel==1)
	{
		chanStr = "【世界】";
		res = RichElementText::create(1, Color3B::YELLOW, 255, FontChina::getStringUTF8(chanStr), "fzxk.ttf", 15);
	}
	else if (channel == 2) {
		chanStr = "【地区】";
		res = RichElementText::create(1, Color3B::ORANGE, 255, FontChina::getStringUTF8(chanStr), "fzxk.ttf", 15);
	}
	else {
		res = RichElementText::create(1, Color3B::WHITE, 255, FontChina::getStringUTF8(chanStr), "fzxk.ttf", 15);
	}
	resrole = RichElementText::create(1, Color3B::GREEN, 255, FontChina::getStringUTF8(roleName), "fzxk.ttf", 15);
	RichElementText* re = RichElementText::create(1, Color3B::BLUE, 255, FontChina::getStringUTF8("：" + chatMsg) , "fzxk.ttf", 15);
	_richChat->pushBackElement(res);
	_richChat->pushBackElement(resrole);
	_richChat->pushBackElement(re);
	RichElementNewLine* line = RichElementNewLine::create(2,Color3B::WHITE,255);
	_richChat->pushBackElement(line);
	//auto size=_richChat->getLayoutSize();
	//log("w=%f,h=%f",size.width,size.height);
	return _richChat;
}
