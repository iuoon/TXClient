#include "scene/MainScene.h"
#include "ui/CocosGUI.h"//UI��ص�ͷ�ļ�
#include "cocostudio/CocoStudio.h"
#include "YijianScene.h"
#include "ui/ChatUI.h"


MainScene::MainScene()
{
}

MainScene::~MainScene()
{
}


cocos2d::Scene * MainScene::createScene()
{
	auto scene = Scene::create();

	auto layer = MainScene::create();

	scene->addChild(layer);
	return scene;
}

bool MainScene::init()
{
	if (!Layer::init())
	{
		return false;
	}
	auto  ui_node = CSLoader::createNode("scene/UILayer.csb");
	this->addChild(ui_node, 200,100);

	auto listview = static_cast<ListView*>(ui_node->getChildByName("ListView_1"));

	//�ӷ���˻�ȡ��ɫ���ڵ�ͼ������Ĭ�ϼ���
	auto yjlayer = YijianScene::createScene();
	this->addChild(yjlayer, 1,200);
	
	auto chatui = ChatUI::create();
	RichText* text1=chatui->getChatMsg(0, "������", "��Ӣ����Щ�������ÿɺã�", "");
	RichText* text2 = chatui->getChatMsg(1, "Ľ����Ӣ", "����ν���벻�ã�����ֻ������һ����մ��Σ��ػ����ף�ת˲���š�Ψ��������ڣ�����ѭ������������...", "");
	RichText* text3 = chatui->getChatMsg(2, "������", "...", "");
	RichText* text4 = chatui->getChatMsg(2, "������", "...", "");
	RichText* text5 = chatui->getChatMsg(2, "������", "...", "");
	RichText* text6 = chatui->getChatMsg(2, "������", "...", "");
	RichText* text7 = chatui->getChatMsg(2, "������", "...", "");
	RichText* text8 = chatui->getChatMsg(2, "������", "...", "");
	RichText* text9 = chatui->getChatMsg(2, "������", "...", "");
	RichText* text10 = chatui->getChatMsg(2, "������", "...", "");
	RichText* text11 = chatui->getChatMsg(1, "Ľ����Ӣ", "����ν���벻�ã�����ֻ������һ����մ��Σ��ػ����ף�ת˲���š�Ψ��������ڣ�����ѭ������������...", "");
	RichText* text12 = chatui->getChatMsg(1, "Ľ����Ӣ", "����ν���벻�ã�����ֻ������һ����մ��Σ��ػ����ף�ת˲���š�Ψ��������ڣ�����ѭ������������...", "");
	RichText* text13 = chatui->getChatMsg(1, "Ľ����Ӣ", "����ν���벻�ã�����ֻ������һ����մ��Σ��ػ����ף�ת˲���š�Ψ��������ڣ�����ѭ������������...", "");
	RichText* text14 = chatui->getChatMsg(1, "Ľ����Ӣ", "����ν���벻�ã�����ֻ������һ����մ��Σ��ػ����ף�ת˲���š�Ψ��������ڣ�����ѭ������������...", "");

	listview->addChild(text1);
	listview->addChild(text2);
	listview->addChild(text3);
	listview->addChild(text4);
	listview->addChild(text5);
	listview->addChild(text6);
	listview->addChild(text7);
	listview->addChild(text8);
	listview->addChild(text9);
	listview->addChild(text10);
	listview->addChild(text11);
	listview->addChild(text12);
	listview->addChild(text13);
	listview->addChild(text14);
	return true;
}