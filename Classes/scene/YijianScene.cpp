#include "YijianScene.h"
#include "ui/CocosGUI.h"//UI相关的头文件
#include "cocostudio/CocoStudio.h"
#include <iostream>
#include <fstream>

USING_NS_CC; //CSLoader位于cocos2d命名空间。
using namespace cocostudio::timeline; //动画相关的类位于cocostuio::timeline当中

YijianScene::YijianScene()
{
}

YijianScene::~YijianScene()
{
}

cocos2d::Scene * YijianScene::createScene()
{
	auto scene = Scene::create();

	auto layer = YijianScene::create();

	scene->addChild(layer);

	return scene;
}

bool YijianScene::init()
{
	if (!Layer::init())
	{
		return false;
	}

	auto  m_mapNode = CSLoader::createNode("scene/XQLayer.csb");	
	this->addChild(m_mapNode,1);
	

	_tileMap = (cocos2d::TMXTiledMap *)m_mapNode->getChildByName("Map_1");
	_collidable = _tileMap->getLayer("road");

	//初始坐标
	Point spawnTileCoord(88, 121); 
	Point spawnPos = this->positionForTileCoord(spawnTileCoord);

	role=Role::createNode();
	role->setPosition(spawnPos);
	_tileMap->addChild(role, 300);


	this->setViewpointCenter(spawnPos);
	this->initGrids();


	//事件监听
	auto listener = EventListenerTouchOneByOne::create();
	listener->setSwallowTouches(true);
	listener->onTouchBegan = [this](Touch *touch, Event *event) {

		Point touchLocation = _tileMap->convertTouchToNodeSpace(touch);
		Point toTileCoord = this->tileCoordForPosition(touchLocation);
		role->moveToward(a_map, this->tileCoordForPosition(role->getPosition()), toTileCoord,_tileMap);
		return true;
	};
	_eventDispatcher->addEventListenerWithSceneGraphPriority(listener, this);

	this->scheduleUpdate();

	return true;
}

void YijianScene::initGrids()
{		
	auto size = _tileMap->getMapSize();
	int mapweight = size.width;  //192
	int mapheight = size.height; //128

	a_map = (ArrayMap)malloc(sizeof(int)*mapheight*mapweight);

	g_Map = new int *[mapheight];
	for (int i = 0; i < mapheight; i++)
	{
		g_Map[i] = new int[mapweight];
	}

	for (int i = 0; i < mapheight; i++)
	{
		for (int j = 0; j < mapweight; j++)
		{
			int tileGid = _collidable->getTileGIDAt(Vec2(j, i));
			if (tileGid) {
				Value properties = _tileMap->getPropertiesForGID(tileGid);
				ValueMap map = properties.asValueMap();
				std::string value = map.at("road").asString();
				if (value.compare("1") == 0)
				{
					g_Map[i][j] = 0;
					*(a_map+i*mapweight+j)=0; //按模拟二维数组寻址方式的方法给数组指向的每个地址赋值
				}
				else {
					g_Map[i][j] = 1;
					*(a_map + i*mapweight + j) = 1;
				}
			}
			else
			{
				g_Map[i][j] = 1;
				*(a_map + i*mapweight + j) = 1;
			}
		}
	}

	//ofstream fout("D:/test.txt");
	//for (int i = 0; i < mapheight; i++)
	//{
	//	fout << "{";
	//	for (int j = 0; j < mapweight; j++)
	//	{
	//		fout << *(a_map + i*mapweight + j) << ",";
	//	}
	//	fout << "}"<<"\n";
	//}
	//fout << flush; 
	//fout.close();


}


void YijianScene::setViewpointCenter(Point pos)  //这个是移动地图,同时跟踪X,Y轴标准算法
{
	Size winSize = Director::getInstance()->getWinSize();
	int x = MAX(pos.x, winSize.width / 2);
	int y = MAX(pos.y, winSize.height / 2);
	x = MIN(x, (_tileMap->getMapSize().width *_tileMap->getTileSize().width) - winSize.width / 2);
	y = MIN(y, (_tileMap->getMapSize().height * _tileMap->getTileSize().height) - winSize.height / 2);
	Point actualPosition = Vec2(x, y);
	Point centerOfView = Vec2(winSize.width / 2, winSize.height / 2);
	Point viewPoint = centerOfView - actualPosition;

	this->setPosition(viewPoint);
}

void YijianScene::update(float second)
{
	this->setViewpointCenter(role->getPosition());
}
