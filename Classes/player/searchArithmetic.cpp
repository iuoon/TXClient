#include "searchArithmetic.h"
#include "HKScene.h"
using namespace std;
static searchArithmetic* _searchArithmeticInstance = nullptr;
searchArithmetic::searchArithmetic()
{
}

searchArithmetic::~searchArithmetic()
{
    m_endTileCoord = Vec2(0,0);
}

searchArithmetic*searchArithmetic::getArithmeticInstance()
{
    if (_searchArithmeticInstance == nullptr)
    {
        _searchArithmeticInstance = new searchArithmetic();
    }
    return _searchArithmeticInstance;
    
}
void searchArithmetic::setMainthis(HKScene *layer)
{
    m_helloPoint =layer;
}



//******************************************************
// 判断两节点之间是否存在障碍物
//******************************************************
bool searchArithmetic::hasBarrier(Point startPos, Point endPos)
{
    //如果起点终点是同一个点,直接返回
    if( startPos.x == endPos.x && startPos.y == endPos.y )return false;
    
    //循环递增量
    float i;
    //循环起始值
    int loopStart;
    //循环终结值
    int loopEnd;
    //起终点连线所经过的节点
    PointArray * passedNodeList;
    
    //两节点中心位置
    Point point1( startPos.x + 0.5, startPos.y + 0.5 );
    Point point2( endPos.x + 0.5, endPos.y + 0.5 );
    
    //根据起点终点间横纵向距离的大小来判断遍历方向
    int distX = abs(endPos.x - startPos.x);
    int distY = abs(endPos.y - startPos.y);
    
    //遍历方向，为true则为横向遍历，否则为纵向遍历
    bool loopDirection = distX > distY ? true : false;
    
    if( loopDirection )
    {
        loopStart = MIN( startPos.x, endPos.x );
        loopEnd = MAX( startPos.x, endPos.x );
        
        //开始横向遍历起点与终点间的节点看是否存在障碍(不可移动点)
        for( i=loopStart; i<=loopEnd; i++ )
        {
            //由于线段方程是根据终起点中心点连线算出的，所以对于起始点来说需要根据其中心点位置来算，而对于其他点则根据左上角来算
            if( i==loopStart )
            {
                i = i +0.5;
            }
            //根据x得到直线上的y值
            float yPos = this->getLineValue(point1, point2, 0,i);
            //获取经过的点
            passedNodeList = this->getNodesUnderPoint(i, yPos);
            //检查经过的节点是否有障碍物，若有则返回true
            for (ssize_t index = 0,len = passedNodeList->count(); index < len ; index++ )
            {
                Point thisPos = passedNodeList->getControlPointAtIndex(index);
                if( m_helloPoint->isRoadAtTileCoord(thisPos) == false )
                {
                    return true;
                }
                else
                {
                    m_endTileCoord = thisPos;
                }
            }
            
            if( i == loopStart + .5 )i -= .5;
        }
        
    }
    else
    {
        loopStart = MIN( startPos.y, endPos.y );
        loopEnd = MAX( startPos.y, endPos.y );
        
        //开始纵向遍历起点与终点间的节点看是否存在障碍(不可移动点)
        for( i=loopStart; i<=loopEnd; i++ )
        {
            if( i==loopStart )i += .5;
            //根据y得到直线上的x值
            float xPos = this->getLineValue(point1, point2, 1,i);
            
            passedNodeList = this->getNodesUnderPoint( xPos, i );
            
            for (ssize_t index = 0,len = passedNodeList->count(); index < len ; index++ )
            {
                Point thisPos = passedNodeList->getControlPointAtIndex(index);
                if( m_helloPoint->isRoadAtTileCoord(thisPos) == false )
                {
				
                    return true;
                }
                else
                {
                    m_endTileCoord = thisPos;
                }
            }
            
            if( i == loopStart + .5 )i -= .5;
        }
    }
    
    return false;
}
//******************************************************
//获取两点之间最接近目标点最近的可行区（避免寻找）
//******************************************************
Point searchArithmetic::getEndTileCoord()
{
    return m_endTileCoord;
}

//******************************************************
//根据两点确定这两点连线的二元一次方程 y = ax + b或者 x = ay + b
//******************************************************

float searchArithmetic::getLineValue(Point ponit1,Point point2, int type,float value)
{
    // 先考虑两点在一条垂直于坐标轴直线的情况，此时直线方程为 y = a 或者 x = a 的形式
    if( ponit1.x == point2.x )
    {
       if( type == 1 )
        {
            
           return ponit1.x;
            
        }
    }
    else if( ponit1.y == point2.y )
    {
        if( type == 0 )
        {
            return ponit1.y;
        }
       
        return value;
    }
    
    // 当两点确定直线不垂直于坐标轴时直线方程设为 y = ax + b
    //根据y1 = ax1 + b和y2 = ax2 + b上下两式相减消去b, 得到 a = ( y1 - y2 ) / ( x1 - x2 )
   
    float a = (ponit1.y - point2.y)*1.0 / (ponit1.x - point2.x);
   
    //将a的值代入任一方程式即可得到b
    float b = ponit1.y - a * ponit1.x;
    
    //把a,b值代入即可得到结果函数
    if( type == 0 )
    {
        return a * value + b;
    }
    else if( type == 1 )
    {
       return (value - b) / a;
    }
    
    return value;
   
}

//******************************************************
// 得到一个点下的所有节点
//******************************************************
PointArray *searchArithmetic::getNodesUnderPoint( float xPos,float yPos )
{
    PointArray *result = nullptr;
    bool xIsInt = true;
    bool yIsInt = true;
    
    int xsPos = floor(xPos);
    int ysPos = floor(yPos);
    if (xPos > xsPos)
    {
        xIsInt = false;
    }
    
    if (yPos > ysPos)
    {
        yIsInt = false;
    }
    
    //点由四节点共享情况
    if( xIsInt && yIsInt )
    {
        result = PointArray::create(4);
        result->addControlPoint(Vec2(xPos - 1, yPos - 1));
        result->addControlPoint(Vec2(xPos, yPos - 1));
        result->addControlPoint(Vec2(xPos - 1, yPos));
        result->addControlPoint(Vec2(xPos, yPos));
        
    }
    //点由2节点共享情况
    //点落在两节点左右临边上
    else if( xIsInt && !yIsInt )
    {
        result = PointArray::create(2);
        result->addControlPoint(Vec2(floor(xPos - 1), floor(yPos)));
        result->addControlPoint(Vec2(floor(xPos), floor(yPos)));
    }
    //点落在两节点上下临边上
    else if( !xIsInt && yIsInt )
    {
        result = PointArray::create(2);
        result->addControlPoint(Vec2(floor(xPos), floor(yPos - 1)));
        result->addControlPoint(Vec2(floor(xPos), floor(yPos)));
    }
    //点由一节点独享情况
    else
    {
        result = PointArray::create(1);
        result->addControlPoint(Vec2(floor(xPos), floor(yPos)));
        
    }
    
    return result;
}



