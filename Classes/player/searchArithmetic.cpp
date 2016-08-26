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
// �ж����ڵ�֮���Ƿ�����ϰ���
//******************************************************
bool searchArithmetic::hasBarrier(Point startPos, Point endPos)
{
    //�������յ���ͬһ����,ֱ�ӷ���
    if( startPos.x == endPos.x && startPos.y == endPos.y )return false;
    
    //ѭ��������
    float i;
    //ѭ����ʼֵ
    int loopStart;
    //ѭ���ս�ֵ
    int loopEnd;
    //���յ������������Ľڵ�
    PointArray * passedNodeList;
    
    //���ڵ�����λ��
    Point point1( startPos.x + 0.5, startPos.y + 0.5 );
    Point point2( endPos.x + 0.5, endPos.y + 0.5 );
    
    //��������յ����������Ĵ�С���жϱ�������
    int distX = abs(endPos.x - startPos.x);
    int distY = abs(endPos.y - startPos.y);
    
    //��������Ϊtrue��Ϊ�������������Ϊ�������
    bool loopDirection = distX > distY ? true : false;
    
    if( loopDirection )
    {
        loopStart = MIN( startPos.x, endPos.x );
        loopEnd = MAX( startPos.x, endPos.x );
        
        //��ʼ�������������յ��Ľڵ㿴�Ƿ�����ϰ�(�����ƶ���)
        for( i=loopStart; i<=loopEnd; i++ )
        {
            //�����߶η����Ǹ�����������ĵ���������ģ����Զ�����ʼ����˵��Ҫ���������ĵ�λ�����㣬��������������������Ͻ�����
            if( i==loopStart )
            {
                i = i +0.5;
            }
            //����x�õ�ֱ���ϵ�yֵ
            float yPos = this->getLineValue(point1, point2, 0,i);
            //��ȡ�����ĵ�
            passedNodeList = this->getNodesUnderPoint(i, yPos);
            //��龭���Ľڵ��Ƿ����ϰ�������򷵻�true
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
        
        //��ʼ�������������յ��Ľڵ㿴�Ƿ�����ϰ�(�����ƶ���)
        for( i=loopStart; i<=loopEnd; i++ )
        {
            if( i==loopStart )i += .5;
            //����y�õ�ֱ���ϵ�xֵ
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
//��ȡ����֮����ӽ�Ŀ�������Ŀ�����������Ѱ�ң�
//******************************************************
Point searchArithmetic::getEndTileCoord()
{
    return m_endTileCoord;
}

//******************************************************
//��������ȷ�����������ߵĶ�Ԫһ�η��� y = ax + b���� x = ay + b
//******************************************************

float searchArithmetic::getLineValue(Point ponit1,Point point2, int type,float value)
{
    // �ȿ���������һ����ֱ��������ֱ�ߵ��������ʱֱ�߷���Ϊ y = a ���� x = a ����ʽ
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
    
    // ������ȷ��ֱ�߲���ֱ��������ʱֱ�߷�����Ϊ y = ax + b
    //����y1 = ax1 + b��y2 = ax2 + b������ʽ�����ȥb, �õ� a = ( y1 - y2 ) / ( x1 - x2 )
   
    float a = (ponit1.y - point2.y)*1.0 / (ponit1.x - point2.x);
   
    //��a��ֵ������һ����ʽ���ɵõ�b
    float b = ponit1.y - a * ponit1.x;
    
    //��a,bֵ���뼴�ɵõ��������
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
// �õ�һ�����µ����нڵ�
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
    
    //�����Ľڵ㹲�����
    if( xIsInt && yIsInt )
    {
        result = PointArray::create(4);
        result->addControlPoint(Vec2(xPos - 1, yPos - 1));
        result->addControlPoint(Vec2(xPos, yPos - 1));
        result->addControlPoint(Vec2(xPos - 1, yPos));
        result->addControlPoint(Vec2(xPos, yPos));
        
    }
    //����2�ڵ㹲�����
    //���������ڵ������ٱ���
    else if( xIsInt && !yIsInt )
    {
        result = PointArray::create(2);
        result->addControlPoint(Vec2(floor(xPos - 1), floor(yPos)));
        result->addControlPoint(Vec2(floor(xPos), floor(yPos)));
    }
    //���������ڵ������ٱ���
    else if( !xIsInt && yIsInt )
    {
        result = PointArray::create(2);
        result->addControlPoint(Vec2(floor(xPos), floor(yPos - 1)));
        result->addControlPoint(Vec2(floor(xPos), floor(yPos)));
    }
    //����һ�ڵ�������
    else
    {
        result = PointArray::create(1);
        result->addControlPoint(Vec2(floor(xPos), floor(yPos)));
        
    }
    
    return result;
}



