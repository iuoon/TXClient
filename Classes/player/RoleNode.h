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
    //��ɫ���
    void changeRoleState(int iState,bool isForever);
    void endActionCallFun();
    //Ѱ·���
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
    class ShortestPathStepObject : public cocos2d::Ref //��Ӵ洢·������
    {
    public:
        ShortestPathStepObject();
        ~ShortestPathStepObject();

        static ShortestPathStepObject *createWithPosition(const cocos2d::Point &pos);
        bool initWithPosition(const cocos2d::Point &pos);

        int getFScore() const;
        bool isEqual(const ShortestPathStepObject *other) const;//�ж�����Ե�
        std::string getDescription() const;

        CC_SYNTHESIZE(cocos2d::Point, _position, Position);
        CC_SYNTHESIZE(int, _gScore, GScore); //Gֵ
        CC_SYNTHESIZE(int, _hScore, HScore);//Hֵ
        CC_SYNTHESIZE(ShortestPathStepObject*, _parent, Parent);

		friend bool operator < (ShortestPathStepObject node1, ShortestPathStepObject node2)
		{
			//��С���ȶ���
			return node1.getFScore() > node2.getFScore();
		}

    };

	

    cocos2d::Vector<ShortestPathStepObject*> _spOpenSteps;//�����б�
    cocos2d::Vector<ShortestPathStepObject*> _spClosedSteps;//�ر��б�
    cocos2d::Vector<ShortestPathStepObject*> _shortestPath;//�洢�ƶ�·��
	priority_queue<ShortestPathStepObject*> _spOpens;  //�����б����ȶ���
	

    //��Ӷ����ȥ�����б�
    void insertInOpenSteps(ShortestPathStepObject *step);
    //������������֮���Hֵ
    int computeHScoreFromCoordToCoord(const cocos2d::Point &fromCoord, const cocos2d::Point &toCoord);
    //�ж�����֮������ĳɱ�Gֵ
    int costToMoveFromStepToAdjacentStep(const ShortestPathStepObject *fromStep, const ShortestPathStepObject *toStep);
     //��ȡ�ڵ�����ֵ
    ssize_t getStepIndex(const cocos2d::Vector<ShortestPathStepObject*> &steps, const ShortestPathStepObject *step);

    //���򱣴沽���б���ִ���ƶ�
    void constructPathAndStartAnimationFromStep(ShortestPathStepObject *step);
    //ѭ��ִ�д洢�ƶ�·����ֱ���б�Ϊ��
    void popStepAndAnimate();
};

#endif // __CAT_SPRITE_H__
