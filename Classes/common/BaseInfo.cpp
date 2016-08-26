#include "common/BaseInfo.h"

BaseInfo::BaseInfo()
{
}

BaseInfo::~BaseInfo()
{
}

// OpenGL坐标转成格子坐标
Point BaseInfo::tileCoordForPosition(Point position)
{
	//正方形
	int x = position.x / _tileMap->getTileSize().width;
	int y = ((_tileMap->getMapSize().height * _tileMap->getTileSize().height) - position.y) / _tileMap->getTileSize().height;
	return Point(x, y);


	//45度交错菱形
	//Size mapSize = _tileMap->getMapSize();
	//Size tileSize = _tileMap->getTileSize();
	//int y = mapSize.height - 2 - ((2 * (int)position.y) / (int)tileSize.height);
	//float x = position.x / tileSize.width - (y % 2) / 2.0f;
	//return Point(x, y);
}

// tile坐标转成瓦片格子中心的OpenGL坐标
Point BaseInfo::positionForTileCoord(Point tileCoord)
{
	//原正方形tiled
	int x = (tileCoord.x * _tileMap->getTileSize().width) + _tileMap->getTileSize().width / 2;
	int y = (_tileMap->getMapSize().height * _tileMap->getTileSize().height) -(tileCoord.y * _tileMap->getTileSize().height) - _tileMap->getTileSize().height / 2;
	return Point(x, y);

	//现45度交错菱形
	/*Size mapSize = _tileMap->getMapSize();
	Size tileSize = _tileMap->getTileSize();
	int x = tileCoord.x*tileSize.width + ((int)tileCoord.y % 2)*tileSize.width / 2;
	int y = (mapSize.height - (tileCoord.y + 1))*tileSize.height / 2 - tileSize.height / 2;
	return Point(x, y);*/
}


void BaseInfo::initGrid()
{
	_grid.clear();
	for (int i = 0; i < _tileMap->getMapSize().width*_tileMap->getMapSize().height; i++)
	{
		_grid.push_back(true);
		Vec2 tileCoord = Vec2(i%MapWidth, i / MapWidth);
		// 使用TMXLayer类的tileGIDAt函数获取TileMap坐标系里的“全局唯一标识”GID
		int tileGid = _collidable->getTileGIDAt(tileCoord);
		if (tileGid)
		{
			// 使用GID来查找指定tile的属性，返回一个Value
			Value properties = _tileMap->getPropertiesForGID(tileGid);
			// 返回的Value实际是一个ValueMap
			ValueMap map = properties.asValueMap();
			// 查找ValueMap，判断是否有”可碰撞的“物体，如果有，设置网格对象的isPass变量为false
			std::string value = map.at("collidable").asString();
			if (value.compare("true") == 0)
			{
				_grid.at(i) = false;
			}
		}
	}
}





