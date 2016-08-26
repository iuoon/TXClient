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
    //��ȡ�㷨ָ��
    static searchArithmetic*getArithmeticInstance();
    void setMainthis(HKScene *layer);
    //�������֮���Ƿ����ϰ�
    bool hasBarrier(Point startPos, Point endPos);
    // �õ�һ�����µ����нڵ�
    PointArray *getNodesUnderPoint( float xPos,float yPos);
    
    float getLineValue(Point ponit1,Point point2, int type,float value);
    
    //����Ѱ��
    Point getEndTileCoord();
private:
    HKScene *m_helloPoint;
    Point m_endTileCoord;
};

#endif 
