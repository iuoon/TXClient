#pragma once
#ifndef _BASE_INFO_H_
#define _BASE_INFO_H_
#include "cocos2d.h"
#include "Astar.h"

USING_NS_CC;


class BaseInfo
{
public:
	BaseInfo();
	~BaseInfo();

	Point tileCoordForPosition(Point position); // OpenGL坐标转成格子坐标
	Point positionForTileCoord(Point tileCoord); // tile坐标转成瓦片格子中心的OpenGL坐标

	Astar astar;

//	cocos2d::experimental::TMXTiledMap* _tileMap;  // 地图
//	cocos2d::experimental::TMXLayer* _collidable;  // 障碍层
	TMXTiledMap* _tileMap;
	TMXLayer* _collidable;

	int MapWidth;
	int MapHeight;
	std::vector<bool> _grid;
	//初始化地图块参数
	void initGrid();


private:

};




#endif