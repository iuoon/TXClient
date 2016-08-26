#include "RoleNode.h"
#include "HKScene.h"
#include "SimpleAudioEngine.h"
#include "searchArithmetic.h"
USING_NS_CC;
using namespace CocosDenshion;
using namespace std;
RoleNode::RoleNode()
{
   
}

RoleNode::~RoleNode()
{
    
}

RoleNode *RoleNode::createWithLayer(HKScene *layer)
{
    RoleNode *pRet = new RoleNode();
    if (pRet && pRet->initWithLayer(layer))
    {
        pRet->autorelease();
        return pRet;
    }
    else
    {
        CC_SAFE_DELETE(pRet);
        return nullptr;
    }
}

bool RoleNode::initWithLayer(HKScene *layer)
{
    bool bRet = false;
    do 
    {
		std::string fileName = "model/orc.c3b";
		auto node = Sprite3D::create(fileName);
		node->setScale(3.0f);		
		node->setGlobalZOrder(1);
		node->setRotation3D(Vec3(0, 180, 0));

        //auto node = Sprite::create("head.png");
       // node->setScale(0.5);
        node->setPosition(0,0);
        this->addChild(node);
		//��������
		auto sp = Sprite3D::create("model/axe.c3b");
		sp->setGlobalZOrder(1);
		//�������ŵ��������
		node->getAttachNode("Bip001 R Hand")->addChild(sp);
		//��ȡ����������Ϣ
		auto animation = Animation3D::create(fileName);
		if (animation)
		{
			auto _idle = Animate3D::create(animation);
			_idle->retain();
			//�þ���ѭ�����Ŷ���
			Sequence*	pSequence = Sequence::create(_idle, NULL);
			node->runAction(RepeatForever::create(pSequence));
		}

        _layer = layer;
        _searchInstance = searchArithmetic::getArithmeticInstance();

        bRet = true;
    } while (0);

    return bRet;
}



//A��Ѱ·
void RoleNode::moveToward(const Point toTileCoord)
{
    this->stopActionByTag(1);
    Point fromTileCoord = _layer->tileCoordForPosition(this->getPosition());
    bool isHasBarrier = true;
    bool isUnPass = false;//���������ط����������ƹµ��Ļ������ϰ���
    isHasBarrier = _searchInstance->hasBarrier(fromTileCoord, toTileCoord);
    
    if (fromTileCoord == toTileCoord)//ԭ�ز�����
    {
        return ;
    }
    
    if (!_layer->isValidTileCoord(toTileCoord))//ȷ��λ���Ƿ��ڵ�ͼ��
    {
        return ;
    }
    
    if(!isHasBarrier)//����յ�����֮�����ϰ����򲻲���A�������ڲ���Ҫ��ʱ����Ȼʹ��A*Ѱ·
    {
        //��϶����ͻص�
        isUnPass = true;
        this->stopActionByTag(10);
        int distanceX = abs(abs(fromTileCoord.x) - abs(toTileCoord.x));
        int distanceY = abs(abs(fromTileCoord.y) - abs(toTileCoord.y));
        int time = sqrt(distanceX*distanceX+distanceY*distanceY);
        
        MoveTo *moveAction = MoveTo::create(0.2f*time, _layer->positionForTileCoord(toTileCoord));
        
        moveAction->setTag(10);
        this->runAction(moveAction);
        return;
    }
    else
    {
        log("from: %f, %f", fromTileCoord.x, fromTileCoord.y);
        log("To: %f, %f", toTileCoord.x, toTileCoord.y);
        //�Ƿ��ڿ�����,����ڲ���������������⴦��
        if (!_layer->isRoadAtTileCoord(toTileCoord))
        {
            this->searchInRoadObject(toTileCoord);
            return;
        }

        _spOpenSteps.clear();
        _spClosedSteps.clear();
		_spOpens.emplace();
        
        // ����������ķ������굽open�б�
        this->insertInOpenSteps(ShortestPathStepObject::createWithPosition(fromTileCoord));
		auto t1 = ::clock();
        do
        {
            ShortestPathStepObject *currentStep = _spOpenSteps.at(0);// ��Ϊ�������б���һ������������С��Fֵ
            _spClosedSteps.pushBack(currentStep); // ��ӵ�ǰ���赽closed�б�
            _spOpenSteps.erase(0);// ������open�б������Ƴ�	
            if (currentStep->getPosition() == toTileCoord)// �����ǰ������Ŀ�귽�����꣬��ô�������
            {
                isUnPass = true;
                this->constructPathAndStartAnimationFromStep(currentStep);
				 _spOpenSteps.clear();
				 _spClosedSteps.clear();
				 _spOpens.emplace();
                break;
            }
            
            // �õ���ǰ��������ڷ�������
            PointArray *adjSteps = _layer->walkableAdjacentTilesCoordForTileCoord(1,1,currentStep->getPosition());
            
            for (ssize_t i = 0; i < adjSteps->count(); ++i)
            {
                ShortestPathStepObject *step = ShortestPathStepObject::createWithPosition(adjSteps->getControlPointAtIndex(i));
                
                
                if (this->getStepIndex(_spClosedSteps, step) != -1)// ��鲽���ǲ����Ѿ���closed�б�
                {
                    continue;
                }
            
                // ����ӵ�ǰ���赽�˲���ĳɱ�
                int moveCost = this->costToMoveFromStepToAdjacentStep(currentStep, step);
                
                // ���˲����Ƿ��Ѿ���open�б�
                ssize_t index = this->getStepIndex(_spOpenSteps, step);
			
                
                if (index == -1)// ����open�б������
                {
                    step->setParent(currentStep);// ���õ�ǰ������Ϊ��һ������
                    step->setGScore(currentStep->getGScore() + moveCost);// Gֵ��ͬ����һ����Gֵ + ����һ��������ĳɱ�
                    // Hֵ���ǴӴ˲��赽Ŀ�귽��������ƶ�������ֵ
                    step->setHScore(this->computeHScoreFromCoordToCoord(step->getPosition(), toTileCoord));
                    this->insertInOpenSteps(step);// ������ӵ�open�б�
                }
                else
                {
                    step = _spOpenSteps.at(index);// ��ȡ�ɵĲ��裬��ֵ�Ѿ������
                    if ((currentStep->getGScore() + moveCost) < step->getGScore())// ���Gֵ�Ƿ���ڵ�ǰ���赽�˲����ֵ
                    {
                        // Gֵ��ͬ����һ����Gֵ + ����һ��������ĳɱ�
                        step->setGScore(currentStep->getGScore() + moveCost);
                        step->setParent(currentStep);// ���õ�ǰ������Ϊ��һ������
                        // ���Ƴ�֮ǰ����Ҫ�ȱ�������
                        step->retain();
                        // ��ΪGֵ�ı��ˣ�FֵҲ����Ÿı�����Ϊ�˱���open�б�������Ҫ���˲����Ƴ��������°������
                        _spOpenSteps.erase(index);
                        // ���°������
                        this->insertInOpenSteps(step);
                        // ���ڿ����ͷ����ˣ���Ϊopen�б�Ӧ�ó�����
                        step->release();
                    }
                }                
            }
        } while (_spOpenSteps.size() > 0);
        
		auto t2 = ::clock();
		log("find path time=====>%d",t2-t1);
    }
    if(!isUnPass)
    {
        this->getBorderUponPlace();
    }
}
//��Թµ����ϰ�����(��ʱ������)
void RoleNode::getBorderUponPlace()
{
    Point endPosition = _searchInstance->getEndTileCoord();
    Point fromTileCoord = _layer->tileCoordForPosition(this->getPosition());
    int distanceX = abs(abs(fromTileCoord.x) - abs(endPosition.x));
    int distanceY = abs(abs(fromTileCoord.y) - abs(endPosition.y));
    int time = sqrt(distanceX*distanceX+distanceY*distanceY);
    MoveTo *moveAction = MoveTo::create(0.2f*time, _layer->positionForTileCoord(endPosition));
    moveAction->setTag(1);
    this->runAction(moveAction);
}
//�������㹲��
cocos2d::Point RoleNode::floydVector( cocos2d::Point n1, cocos2d::Point n2)
{
    cocos2d::Point target;
    target.x = n1.x - n2.x;
    target.y = n1.y - n2.y;
    return target;
}

// ��������·��ƽ������
void RoleNode::floydMethod()
{
    Point fromTileCoord = _layer->tileCoordForPosition(this->getPosition());
    _shortestPath.insert(0, ShortestPathStepObject::createWithPosition(fromTileCoord));
    ssize_t len  = _shortestPath.size();
   
    if (len > 2)
    {
        Point selectPoint;
        Point tempPoint;
        //����·��������ȫ��·���ڵ㣬�ϲ���ͬһֱ���ϵ�·���ڵ�
        //������1,2,3,���㣬��2��1�ĺᡢ�������ֵ�ֱ���3��2�ĺᡢ�������ֵ�����
        //�жϴ����㹲�ߣ���ʱ����ɾ���м��2
        selectPoint = this->floydVector( _shortestPath.at(len - 1)->getPosition(), _shortestPath.at(len - 2)->getPosition());
        
        for (ssize_t i = len - 3; i >= 0; i--)
        {
            tempPoint = this-> floydVector( _shortestPath.at(i + 1)->getPosition(), _shortestPath.at(i)->getPosition());
            
            if (selectPoint.x == tempPoint.x && selectPoint.y == tempPoint.y)
            {
                _shortestPath.erase(i + 1);
            }
            else
            {
                selectPoint.x = tempPoint.x;
                selectPoint.y = tempPoint.y;
            }
        }
    }
    
    for (const ShortestPathStepObject *s : _shortestPath)
    {
        log("%s", s->getDescription().c_str());
    }
    
    ssize_t iStarIndex = 0;
    //�ϲ������յ������
    while (1)
    {
        len  = _shortestPath.size();
        if (len - iStarIndex > 3)
        {
            Point starPoint = _shortestPath.at(iStarIndex)->getPosition();
            for (ssize_t i = len - 1; i > iStarIndex; i--)
            {
                Point tempPoint = _shortestPath.at(i)->getPosition();
                
                bool isHasBarrier = _searchInstance->hasBarrier(starPoint, tempPoint);
                
                if(!isHasBarrier)//����֮�����ϰ�����ϲ�
                {
                    for (ssize_t ia = i-1 ; ia > iStarIndex; ia--)
                    {
                        log("delete point %f,%f", _shortestPath.at(ia)->getPosition().x,_shortestPath.at(ia)->getPosition().y);
                        _shortestPath.erase(ia);
                        
                    }
                    iStarIndex = iStarIndex+1;//���浱ǰ�´����
                    break;
                }
                else
                {
                    if (i == iStarIndex+1)//��ѯ�Ѿ�������ͷ
                    {
                        iStarIndex = iStarIndex+1;//���浱ǰ�´����
                    }
                }
            }
        }
        else{
            break;
        }
       
    }
    _shortestPath.erase(0);//��ȥ���
}


//�����ܱ߿�����λ��
void RoleNode::searchInRoadObject(cocos2d::Point toTileCoord)
{
    log("To: %f, %f", toTileCoord.x, toTileCoord.y);
    cocos2d::Vector<ShortestPathStepObject*> tempVector;//�б�
    int runIndex = 1;
    while (1)
    {
        PointArray *adjSteps = _layer->walkableAdjacentTilesCoordForTileCoord(2,runIndex,toTileCoord);
        Point fromTileCoord = _layer->tileCoordForPosition(this->getPosition());
        for (ssize_t i = 0; i < adjSteps->count(); ++i)
        {
            ShortestPathStepObject *step = ShortestPathStepObject::createWithPosition(adjSteps->getControlPointAtIndex(i));
            //���ѡȡ��������ڵ������
            if (step->getPosition() == fromTileCoord)
            {
                tempVector.clear();
                return;
            }
            tempVector.pushBack(step);
        }
        int count = (int)tempVector.size();
        if (count >=1){
            break;
        }
        runIndex = runIndex+1;
    }

    ShortestPathStepObject *goalStep = nullptr;
    //������Ŀ��
    if (tempVector.size() > 0)
    {
        int hValue = 0;
        for (ssize_t i = 0 , len = tempVector.size() ; i < len; ++i)
        {
            ShortestPathStepObject *thisStep = tempVector.at(i);
           
            //��ȡ��ǰλ�þ���Ŀ���Hֵ
            int h = this->computeHScoreFromCoordToCoord(thisStep->getPosition(), toTileCoord);
            goalStep = goalStep == nullptr?thisStep:goalStep;
            hValue = hValue == 0?h:hValue;
            //ɹѡ���Ŀ��
            if (hValue > h )
            {
                hValue = h;
                goalStep = thisStep;
            }
        }
        //�ٴ�A��
        if(goalStep)
        {
            this->moveToward(goalStep->getPosition());
        }
        
    }
  
}

//���Ĳ�����뿪���б���Fֵ������С������ǰ��
void RoleNode::insertInOpenSteps(ShortestPathStepObject *step)
{
	int stepFScore = step->getFScore();
	ssize_t count = _spOpenSteps.size();
	ssize_t i = 0;
	for (; i < count; ++i)
	{
		if (stepFScore <= _spOpenSteps.at(i)->getFScore())
		{
			break;
		}
	}
	_spOpenSteps.insert(i, step);
	
}

int RoleNode::computeHScoreFromCoordToCoord(const Point &fromCoord, const Point &toCoord)
{
    // ����ʹ�������ٷ���������ӵ�ǰ���赽��Ŀ�경�裬��ˮƽ�ʹ�ֱ�����ܵĲ��������Ը����ϰ���
    return abs(toCoord.x - fromCoord.x) + abs(toCoord.y - fromCoord.y);
}
//������������֮�����ĵĳɱ�
int RoleNode::costToMoveFromStepToAdjacentStep(const ShortestPathStepObject *fromStep, const ShortestPathStepObject *toStep)
{
    return ((fromStep->getPosition().x != toStep->getPosition().x)
            && (fromStep->getPosition().y != toStep->getPosition().y)) ? 14 : 10;
}
//�������Ƿ��ڿ����б���
ssize_t RoleNode::getStepIndex(const cocos2d::Vector<ShortestPathStepObject *> &steps, const ShortestPathStepObject *step)
{
	int counts = steps.size();
    for (ssize_t i = 0; i < counts; ++i)
    {
        if (steps.at(i)->isEqual(step))
        {
            return i;
        }
    }
    return -1;
}




//���򱣴沽���б���ִ���ƶ�
void RoleNode::constructPathAndStartAnimationFromStep(ShortestPathStepObject *step)
{
    _shortestPath.clear();

    do
    {
        // ��ʼλ�ò�Ҫ�������
        if (step->getParent())
        {
            // ���ǲ��뵽����0��λ�ã��Ա㷴ת·��
            _shortestPath.insert(0, step);
        }
        step = step->getParent();   // ����ֱ��û����һ��
    } while (step);
    //
    this->floydMethod();
    this->popStepAndAnimate();
}
//ѭ��ִ�д洢�ƶ�·����ֱ���б�Ϊ��
void RoleNode::popStepAndAnimate()
{
    //��ȡ��ɫ��Ӧ�ڵ�ͼ�ϵ�λ��
    Point currentPosition = _layer->tileCoordForPosition(this->getPosition());
    // ����Ƿ�����·��������Ҫǰ��
    if (_shortestPath.size() == 0)
    {
        return;
    }
    // �õ���һ���ƶ��Ĳ���
    ShortestPathStepObject *step = _shortestPath.at(0);

    Point thisTileCoord = _layer->tileCoordForPosition(this->getPosition());
    
    int distanceX = abs(abs(thisTileCoord.x) - abs(step->getPosition().x));
    int distanceY = abs(abs(thisTileCoord.y) - abs(step->getPosition().y));
    int time = sqrt(distanceX*distanceX+distanceY*distanceY);
    
    //��϶����ͻص�
    MoveTo *moveAction = MoveTo::create(0.2f*time, _layer->positionForTileCoord(step->getPosition()));
    CallFunc *moveCallback = CallFunc::create(CC_CALLBACK_0(RoleNode::popStepAndAnimate, this));
    //�Ƴ����β���
    _shortestPath.erase(0);
    // ���ж���
    Sequence *moveSequence = Sequence::create(moveAction, moveCallback, nullptr);
    moveSequence->setTag(1);
    this->runAction(moveSequence);
}

RoleNode::ShortestPathStepObject::ShortestPathStepObject() :
    _position(Point::ZERO),
    _gScore(0),
    _hScore(0),
    _parent(nullptr)
{
}

RoleNode::ShortestPathStepObject::~ShortestPathStepObject()
{
}
//*************************************************************
//����λ�ö���
//*************************************************************
RoleNode::ShortestPathStepObject *RoleNode::ShortestPathStepObject::createWithPosition(const Point &pos)
{
    ShortestPathStepObject *pRet = new ShortestPathStepObject();
    if (pRet && pRet->initWithPosition(pos))
    {
        pRet->autorelease();
        return pRet;
    }
    else
    {
        CC_SAFE_DELETE(pRet);
        return nullptr;
    }
}

bool RoleNode::ShortestPathStepObject::initWithPosition(const Point &pos)
{
    bool bRet = false;
    do
    {
        this->setPosition(pos);

        bRet = true;
    } while (0);

    return bRet;
}

int RoleNode::ShortestPathStepObject::getFScore() const
{
    return this->getGScore() + this->getHScore();
}

bool RoleNode::ShortestPathStepObject::isEqual(const ShortestPathStepObject *other) const
{
    return this->getPosition() == other->getPosition();
}

std::string RoleNode::ShortestPathStepObject::getDescription() const
{
    return StringUtils::format("pos=[%.0f;%.0f]  g=%d  h=%d  f=%d",
                               this->getPosition().x, this->getPosition().y,
                               this->getGScore(), this->getHScore(), this->getFScore());
}
