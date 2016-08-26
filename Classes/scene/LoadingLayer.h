#pragma once
#ifndef _LOADING_LAYER_H_
#define _LOADING_LAYER_H_

#include "cocos2d.h"
USING_NS_CC;
using namespace cocos2d::ui;



class Loading :public Layer
{
public:
	Loading();
	~Loading();

	static cocos2d::Scene* createScene();

	virtual bool init();

	CREATE_FUNC(Loading);

	void update(float delta) override;

	void GotoScene(int tag);

private:
	int _count;
	Node* rootNode;

};



#endif