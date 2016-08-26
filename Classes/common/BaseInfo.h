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

	Point tileCoordForPosition(Point position); // OpenGL����ת�ɸ�������
	Point positionForTileCoord(Point tileCoord); // tile����ת����Ƭ�������ĵ�OpenGL����

	Astar astar;

//	cocos2d::experimental::TMXTiledMap* _tileMap;  // ��ͼ
//	cocos2d::experimental::TMXLayer* _collidable;  // �ϰ���
	TMXTiledMap* _tileMap;
	TMXLayer* _collidable;

	int MapWidth;
	int MapHeight;
	std::vector<bool> _grid;
	//��ʼ����ͼ�����
	void initGrid();


private:

};




#endif