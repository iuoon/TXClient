#ifndef __searchArithmetic_H__
#define __searchArithmetic_H__

#include "cocos2d.h"
USING_NS_CC;
class HKScene;
class searchArithmetic : public cocos2d::Ref
{
public:
    searchArithmetic();
    ~searchArithmetic();
    //获取算法指针
    static searchArithmetic*getArithmeticInstance();
    void setMainthis(HKScene *layer);
    //检测两点之间是否有障碍
    bool hasBarrier(Point startPos, Point endPos);
    // 得到一个点下的所有节点
    PointArray *getNodesUnderPoint( float xPos,float yPos);
    
    float getLineValue(Point ponit1,Point point2, int type,float value);
    
    //避免寻找
    Point getEndTileCoord();
private:
    HKScene *m_helloPoint;
    Point m_endTileCoord;
};

#endif 
