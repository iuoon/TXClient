#ifndef __Role_H__
#define __Role_H__
#include "cocos2d.h"
#include "common\BaseInfo.h"
#include "util/AAStar.h"
USING_NS_CC;

class Role : public cocos2d::Node,public BaseInfo
{
public:
	Role();
	~Role();

	static Role* createNode();
	bool initRole() ;

	void moveToward(int* map,Point sp,Point ep,TMXTiledMap* tilemap);

	void stepAndAnimate();

private:
	Vector<ANode*> Paths;

};


#endif
