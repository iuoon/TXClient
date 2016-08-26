#ifndef __RoleNode_H__
#define __RoleNode_H__

#include "cocos2d.h"
#include<queue> 
using namespace std;


class HKScene;
class searchArithmetic;


class RoleNode : public cocos2d::Node
{
public:
    RoleNode();
    ~RoleNode();
    //角色相关
    void changeRoleState(int iState,bool isForever);
    void endActionCallFun();
    //寻路相关
    static RoleNode *createWithLayer(HKScene *layer);
    bool initWithLayer(HKScene *layer);
    void searchInRoadObject(cocos2d::Point pos);
    void moveToward(const cocos2d::Point toTileCoord);
    void floydMethod();
    void getBorderUponPlace();
    cocos2d::Point floydVector( cocos2d::Point n1, cocos2d::Point n2);
private:
    cocos2d::Animation *createCatAnimation(const char *animType);
    
private:
	HKScene *_layer;
    searchArithmetic*_searchInstance;
private:
    class ShortestPathStepObject : public cocos2d::Ref //添加存储路径对象
    {
    public:
        ShortestPathStepObject();
        ~ShortestPathStepObject();

        static ShortestPathStepObject *createWithPosition(const cocos2d::Point &pos);
        bool initWithPosition(const cocos2d::Point &pos);

        int getFScore() const;
        bool isEqual(const ShortestPathStepObject *other) const;//判断坐标对等
        std::string getDescription() const;

        CC_SYNTHESIZE(cocos2d::Point, _position, Position);
        CC_SYNTHESIZE(int, _gScore, GScore); //G值
        CC_SYNTHESIZE(int, _hScore, HScore);//H值
        CC_SYNTHESIZE(ShortestPathStepObject*, _parent, Parent);

		friend bool operator < (ShortestPathStepObject node1, ShortestPathStepObject node2)
		{
			//最小优先队列
			return node1.getFScore() > node2.getFScore();
		}

    };

	

    cocos2d::Vector<ShortestPathStepObject*> _spOpenSteps;//开启列表
    cocos2d::Vector<ShortestPathStepObject*> _spClosedSteps;//关闭列表
    cocos2d::Vector<ShortestPathStepObject*> _shortestPath;//存储移动路径
	priority_queue<ShortestPathStepObject*> _spOpens;  //开放列表优先队列
	

    //添加对象进去开启列表
    void insertInOpenSteps(ShortestPathStepObject *step);
    //计算两个坐标之间的H值
    int computeHScoreFromCoordToCoord(const cocos2d::Point &fromCoord, const cocos2d::Point &toCoord);
    //判断两步之间的消耗成本G值
    int costToMoveFromStepToAdjacentStep(const ShortestPathStepObject *fromStep, const ShortestPathStepObject *toStep);
     //获取节点索引值
    ssize_t getStepIndex(const cocos2d::Vector<ShortestPathStepObject*> &steps, const ShortestPathStepObject *step);

    //倒序保存步骤列表，并执行移动
    void constructPathAndStartAnimationFromStep(ShortestPathStepObject *step);
    //循环执行存储移动路径表，直到列表为空
    void popStepAndAnimate();
};

#endif // __CAT_SPRITE_H__
