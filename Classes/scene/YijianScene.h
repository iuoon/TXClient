#pragma once
#ifndef _YIJIAN_SCENE_H_
#define _YIJIAN_SCENE_H_
#include "cocos2d.h"
#include "common/BaseInfo.h"
#include "player/Role.h"

USING_NS_CC;
typedef int* ArrayMap;

class YijianScene :public cocos2d::Layer,public BaseInfo
{
public:
	YijianScene();
	~YijianScene();

	static cocos2d::Scene* createScene();

	virtual bool init();

	void initGrids();

	void update(float second) override;

	CREATE_FUNC(YijianScene);

	void setViewpointCenter(Point point);

private:
    int** g_Map;
	Role* role;
	int* a_map;
	

};



#endif