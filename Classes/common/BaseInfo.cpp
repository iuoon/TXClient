#include "common/BaseInfo.h"

BaseInfo::BaseInfo()
{
}

BaseInfo::~BaseInfo()
{
}

// OpenGL����ת�ɸ�������
Point BaseInfo::tileCoordForPosition(Point position)
{
	//������
	int x = position.x / _tileMap->getTileSize().width;
	int y = ((_tileMap->getMapSize().height * _tileMap->getTileSize().height) - position.y) / _tileMap->getTileSize().height;
	return Point(x, y);


	//45�Ƚ�������
	//Size mapSize = _tileMap->getMapSize();
	//Size tileSize = _tileMap->getTileSize();
	//int y = mapSize.height - 2 - ((2 * (int)position.y) / (int)tileSize.height);
	//float x = position.x / tileSize.width - (y % 2) / 2.0f;
	//return Point(x, y);
}

// tile����ת����Ƭ�������ĵ�OpenGL����
Point BaseInfo::positionForTileCoord(Point tileCoord)
{
	//ԭ������tiled
	int x = (tileCoord.x * _tileMap->getTileSize().width) + _tileMap->getTileSize().width / 2;
	int y = (_tileMap->getMapSize().height * _tileMap->getTileSize().height) -(tileCoord.y * _tileMap->getTileSize().height) - _tileMap->getTileSize().height / 2;
	return Point(x, y);

	//��45�Ƚ�������
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
		// ʹ��TMXLayer���tileGIDAt������ȡTileMap����ϵ��ġ�ȫ��Ψһ��ʶ��GID
		int tileGid = _collidable->getTileGIDAt(tileCoord);
		if (tileGid)
		{
			// ʹ��GID������ָ��tile�����ԣ�����һ��Value
			Value properties = _tileMap->getPropertiesForGID(tileGid);
			// ���ص�Valueʵ����һ��ValueMap
			ValueMap map = properties.asValueMap();
			// ����ValueMap���ж��Ƿ��С�����ײ�ġ����壬����У�������������isPass����Ϊfalse
			std::string value = map.at("collidable").asString();
			if (value.compare("true") == 0)
			{
				_grid.at(i) = false;
			}
		}
	}
}





