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
		//加载武器
		auto sp = Sprite3D::create("model/axe.c3b");
		sp->setGlobalZOrder(1);
		//将武器放到玩家手上
		node->getAttachNode("Bip001 R Hand")->addChild(sp);
		//获取骨骼动画信息
		auto animation = Animation3D::create(fileName);
		if (animation)
		{
			auto _idle = Animate3D::create(animation);
			_idle->retain();
			//让精灵循环播放动作
			Sequence*	pSequence = Sequence::create(_idle, NULL);
			node->runAction(RepeatForever::create(pSequence));
		}

        _layer = layer;
        _searchInstance = searchArithmetic::getArithmeticInstance();

        bRet = true;
    } while (0);

    return bRet;
}



//A＊寻路
void RoleNode::moveToward(const Point toTileCoord)
{
    this->stopActionByTag(1);
    Point fromTileCoord = _layer->tileCoordForPosition(this->getPosition());
    bool isHasBarrier = true;
    bool isUnPass = false;//点击到特殊地方（例如类似孤岛的回字形障碍）
    isHasBarrier = _searchInstance->hasBarrier(fromTileCoord, toTileCoord);
    
    if (fromTileCoord == toTileCoord)//原地不处理
    {
        return ;
    }
    
    if (!_layer->isValidTileCoord(toTileCoord))//确认位置是否在地图内
    {
        return ;
    }
    
    if(!isHasBarrier)//起点终点两点之间无障碍，则不采用A＊避免在不必要的时候依然使用A*寻路
    {
        //组合动作和回调
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
        //是否在可行区,如果在不可行区域进行特殊处理
        if (!_layer->isRoadAtTileCoord(toTileCoord))
        {
            this->searchInRoadObject(toTileCoord);
            return;
        }

        _spOpenSteps.clear();
        _spClosedSteps.clear();
		_spOpens.emplace();
        
        // 首先添加起点的方块坐标到open列表
        this->insertInOpenSteps(ShortestPathStepObject::createWithPosition(fromTileCoord));
		auto t1 = ::clock();
        do
        {
            ShortestPathStepObject *currentStep = _spOpenSteps.at(0);// 因为是有序列表，第一个步骤总是最小的F值
            _spClosedSteps.pushBack(currentStep); // 添加当前步骤到closed列表
            _spOpenSteps.erase(0);// 将它从open列表里面移除	
            if (currentStep->getPosition() == toTileCoord)// 如果当前步骤是目标方块坐标，那么就完成了
            {
                isUnPass = true;
                this->constructPathAndStartAnimationFromStep(currentStep);
				 _spOpenSteps.clear();
				 _spClosedSteps.clear();
				 _spOpens.emplace();
                break;
            }
            
            // 得到当前步骤的相邻方块坐标
            PointArray *adjSteps = _layer->walkableAdjacentTilesCoordForTileCoord(1,1,currentStep->getPosition());
            
            for (ssize_t i = 0; i < adjSteps->count(); ++i)
            {
                ShortestPathStepObject *step = ShortestPathStepObject::createWithPosition(adjSteps->getControlPointAtIndex(i));
                
                
                if (this->getStepIndex(_spClosedSteps, step) != -1)// 检查步骤是不是已经在closed列表
                {
                    continue;
                }
            
                // 计算从当前步骤到此步骤的成本
                int moveCost = this->costToMoveFromStepToAdjacentStep(currentStep, step);
                
                // 检查此步骤是否已经在open列表
                ssize_t index = this->getStepIndex(_spOpenSteps, step);
			
                
                if (index == -1)// 不在open列表，添加它
                {
                    step->setParent(currentStep);// 设置当前步骤作为上一步操作
                    step->setGScore(currentStep->getGScore() + moveCost);// G值等同于上一步的G值 + 从上一步到这里的成本
                    // H值即是从此步骤到目标方块坐标的移动量估算值
                    step->setHScore(this->computeHScoreFromCoordToCoord(step->getPosition(), toTileCoord));
                    this->insertInOpenSteps(step);// 按序添加到open列表
                }
                else
                {
                    step = _spOpenSteps.at(index);// 获取旧的步骤，其值已经计算过
                    if ((currentStep->getGScore() + moveCost) < step->getGScore())// 检查G值是否低于当前步骤到此步骤的值
                    {
                        // G值等同于上一步的G值 + 从上一步到这里的成本
                        step->setGScore(currentStep->getGScore() + moveCost);
                        step->setParent(currentStep);// 设置当前步骤作为上一步操作
                        // 在移除之前，需要先保持引用
                        step->retain();
                        // 因为G值改变了，F值也会跟着改变所以为了保持open列表有序，需要将此步骤移除，再重新按序插入
                        _spOpenSteps.erase(index);
                        // 重新按序插入
                        this->insertInOpenSteps(step);
                        // 现在可以释放它了，因为open列表应该持有它
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
//针对孤岛型障碍处理(暂时不处理)
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
//处理三点共线
cocos2d::Point RoleNode::floydVector( cocos2d::Point n1, cocos2d::Point n2)
{
    cocos2d::Point target;
    target.x = n1.x - n2.x;
    target.y = n1.y - n2.y;
    return target;
}

// 弗洛伊德路径平滑处理
void RoleNode::floydMethod()
{
    Point fromTileCoord = _layer->tileCoordForPosition(this->getPosition());
    _shortestPath.insert(0, ShortestPathStepObject::createWithPosition(fromTileCoord));
    ssize_t len  = _shortestPath.size();
   
    if (len > 2)
    {
        Point selectPoint;
        Point tempPoint;
        //遍历路径数组中全部路径节点，合并在同一直线上的路径节点
        //假设有1,2,3,三点，若2与1的横、纵坐标差值分别与3与2的横、纵坐标差值相等则
        //判断此三点共线，此时可以删除中间点2
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
    //合并消除拐点操作。
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
                
                if(!isHasBarrier)//两点之间无障碍，则合并
                {
                    for (ssize_t ia = i-1 ; ia > iStarIndex; ia--)
                    {
                        log("delete point %f,%f", _shortestPath.at(ia)->getPosition().x,_shortestPath.at(ia)->getPosition().y);
                        _shortestPath.erase(ia);
                        
                    }
                    iStarIndex = iStarIndex+1;//保存当前下次起点
                    break;
                }
                else
                {
                    if (i == iStarIndex+1)//查询已经带到尽头
                    {
                        iStarIndex = iStarIndex+1;//保存当前下次起点
                    }
                }
            }
        }
        else{
            break;
        }
       
    }
    _shortestPath.erase(0);//除去起点
}


//搜索周边可行走位置
void RoleNode::searchInRoadObject(cocos2d::Point toTileCoord)
{
    log("To: %f, %f", toTileCoord.x, toTileCoord.y);
    cocos2d::Vector<ShortestPathStepObject*> tempVector;//列表
    int runIndex = 1;
    while (1)
    {
        PointArray *adjSteps = _layer->walkableAdjacentTilesCoordForTileCoord(2,runIndex,toTileCoord);
        Point fromTileCoord = _layer->tileCoordForPosition(this->getPosition());
        for (ssize_t i = 0; i < adjSteps->count(); ++i)
        {
            ShortestPathStepObject *step = ShortestPathStepObject::createWithPosition(adjSteps->getControlPointAtIndex(i));
            //如果选取中有自身节点则结束
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
    //搜索到目标
    if (tempVector.size() > 0)
    {
        int hValue = 0;
        for (ssize_t i = 0 , len = tempVector.size() ; i < len; ++i)
        {
            ShortestPathStepObject *thisStep = tempVector.at(i);
           
            //获取当前位置距离目标的H值
            int h = this->computeHScoreFromCoordToCoord(thisStep->getPosition(), toTileCoord);
            goalStep = goalStep == nullptr?thisStep:goalStep;
            hValue = hValue == 0?h:hValue;
            //晒选最近目标
            if (hValue > h )
            {
                hValue = h;
                goalStep = thisStep;
            }
        }
        //再次A＊
        if(goalStep)
        {
            this->moveToward(goalStep->getPosition());
        }
        
    }
  
}

//将改步奏加入开放列表并按F值排序，最小的排在前面
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
    // 这里使用曼哈顿方法，计算从当前步骤到达目标步骤，在水平和垂直方向总的步数（忽略各种障碍）
    return abs(toCoord.x - fromCoord.x) + abs(toCoord.y - fromCoord.y);
}
//计算两个对象之间消耗的成本
int RoleNode::costToMoveFromStepToAdjacentStep(const ShortestPathStepObject *fromStep, const ShortestPathStepObject *toStep)
{
    return ((fromStep->getPosition().x != toStep->getPosition().x)
            && (fromStep->getPosition().y != toStep->getPosition().y)) ? 14 : 10;
}
//检测对象是否在开启列表中
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




//倒序保存步骤列表，并执行移动
void RoleNode::constructPathAndStartAnimationFromStep(ShortestPathStepObject *step)
{
    _shortestPath.clear();

    do
    {
        // 起始位置不要进行添加
        if (step->getParent())
        {
            // 总是插入到索引0的位置，以便反转路径
            _shortestPath.insert(0, step);
        }
        step = step->getParent();   // 倒退直到没有上一步
    } while (step);
    //
    this->floydMethod();
    this->popStepAndAnimate();
}
//循环执行存储移动路径表，直到列表为空
void RoleNode::popStepAndAnimate()
{
    //获取角色对应在地图上的位置
    Point currentPosition = _layer->tileCoordForPosition(this->getPosition());
    // 检查是否仍有路径步骤需要前进
    if (_shortestPath.size() == 0)
    {
        return;
    }
    // 得到下一步移动的步骤
    ShortestPathStepObject *step = _shortestPath.at(0);

    Point thisTileCoord = _layer->tileCoordForPosition(this->getPosition());
    
    int distanceX = abs(abs(thisTileCoord.x) - abs(step->getPosition().x));
    int distanceY = abs(abs(thisTileCoord.y) - abs(step->getPosition().y));
    int time = sqrt(distanceX*distanceX+distanceY*distanceY);
    
    //组合动作和回调
    MoveTo *moveAction = MoveTo::create(0.2f*time, _layer->positionForTileCoord(step->getPosition()));
    CallFunc *moveCallback = CallFunc::create(CC_CALLBACK_0(RoleNode::popStepAndAnimate, this));
    //移除本次步骤
    _shortestPath.erase(0);
    // 运行动作
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
//创建位置对象
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
