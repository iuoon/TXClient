#include "HKScene.h"
#include "RoleNode.h"
#include "SimpleAudioEngine.h"
#include "ui/CocosGUI.h"//UI相关的头文件
#include "cocostudio/CocoStudio.h"
#include "searchArithmetic.h"
USING_NS_CC;
using namespace CocosDenshion;
using namespace cocostudio;
using namespace cocos2d;//CSLoader位于cocos2d命名空间。
using namespace cocostudio::timeline;//动画相关的类位于cocostuio::timeline当中

Scene* HKScene::createScene()
{
    // 'scene' is an autorelease object
    auto scene = Scene::create();
    
    // 'layer' is an autorelease object
    auto layer = HKScene::create();

    // add layer as a child to scene
    scene->addChild(layer);


    
    return scene;
}

HKScene::HKScene():
    _tileMap(nullptr),
    _bgLayer(nullptr),
    _batchNode(nullptr),
    m_pRole(nullptr)
{
}

HKScene::~HKScene()
{
}

bool HKScene::init()
{
    bool bRet = false;
    do
    {
        CC_BREAK_IF(!Layer::init());
        //地图csb
        m_mapNode = CSLoader::createNode("scene/XQLayer.csb");  //cocosScene.csb
        this->addChild(m_mapNode);      
        //获取地图
        _tileMap = (TMXTiledMap *)m_mapNode->getChildByName("Map_1");

        //初始坐标
        Point spawnTileCoord(71, 124); //Point spawnTileCoord(71, 124)
        Point spawnPos = this->positionForTileCoord(spawnTileCoord);
        this->setViewpointCenter(spawnPos);

        //碰撞区
        _bgLayer = _tileMap->getLayer("road");
        
        //设置搜索算法
        searchArithmetic::getArithmeticInstance()->setMainthis(this);
               
        m_pRole = RoleNode::createWithLayer(this);
        m_pRole->setPosition(spawnPos); 
        _tileMap->addChild(m_pRole,300);

        //事件监听
        auto listener = EventListenerTouchOneByOne::create();
        listener->setSwallowTouches(true);
        listener->onTouchBegan = [this](Touch *touch, Event *event){
           
            Point touchLocation = _tileMap->convertTouchToNodeSpace(touch);
            Point toTileCoord = this->tileCoordForPosition(touchLocation);
            m_pRole->moveToward(toTileCoord);
            
            
            return true;
        };
        _eventDispatcher->addEventListenerWithSceneGraphPriority(listener, this);

        this->scheduleUpdate();

        
        bRet = true;
    } while (0);
    
    
    return bRet;
}

//检测是不是可行区域
bool HKScene::isRoadAtTileCoord(const Point &tileCoord) const
{
    return this->isPropAtTileCoordForLayer("road", tileCoord, _bgLayer);
}

//位置转换到地图
Point HKScene::tileCoordForPosition(const Point &position) const
{
    int x = position.x / _tileMap->getTileSize().width;
    int y = ((_tileMap->getMapSize().height * _tileMap->getTileSize().height) - position.y) / _tileMap->getTileSize().height;
    return Point(x, y);
}

Point HKScene::positionForTileCoord(const Point &tileCoord) const
{
    int x = (tileCoord.x * _tileMap->getTileSize().width) + _tileMap->getTileSize().width / 2;
    
    int y = (_tileMap->getMapSize().height * _tileMap->getTileSize().height) -
            (tileCoord.y * _tileMap->getTileSize().height) - _tileMap->getTileSize().height / 2;
    
    return Point(x, y);
}



bool HKScene::isPropAtTileCoordForLayer(const char *prop, const Point &tileCoord, TMXLayer *layer) const
{
    if (!this->isValidTileCoord(tileCoord))//确认位置是否在地图内
    {
        return false;
    }
    
    int gid = layer->getTileGIDAt(tileCoord);
    Value properties = _tileMap->getPropertiesForGID(gid);//获取设置属性
    if (properties.isNull())
    {
        return false;
    }
    return properties.asValueMap().find(prop) != properties.asValueMap().end();
}
//确认位置是否在地图内
bool HKScene::isValidTileCoord(const Point &tileCoord) const
{
    if (tileCoord.x < 0 || tileCoord.y < 0 ||
        tileCoord.x >= _tileMap->getMapSize().width ||
        tileCoord.y >= _tileMap->getMapSize().height)
    {
        return false;
    }
    else
    {
        return true;
    }
}


bool HKScene::isRoadRect(const cocos2d::Point &tileCoord)const
{
    bool isRoad = false;
    bool t = false;
    bool l = false;
    bool b = false;
    bool r = false;
    
    // 上
    Point UpPoint(tileCoord.x, tileCoord.y - 1);
    if (this->isValidTileCoord(UpPoint) && this->isRoadAtTileCoord(UpPoint))//确认位置是否在地图内以及是不是障碍物
    {
        t = true;
    }
    
    // 左
    Point LeftPoint(tileCoord.x - 1, tileCoord.y);
    if (this->isValidTileCoord(LeftPoint) && this->isRoadAtTileCoord(LeftPoint))
    {
        l = true;
    }
    
    // 下
    Point DownPoint(tileCoord.x, tileCoord.y + 1);
    if (this->isValidTileCoord(DownPoint) && this->isRoadAtTileCoord(DownPoint))
    {
        b = true;
    }
    
    // 右
    Point RightPoint(tileCoord.x + 1, tileCoord.y);
    if (this->isValidTileCoord(RightPoint) && this->isRoadAtTileCoord(RightPoint))
    {
        r = true;
    }
    if (t && l && b && r)
    {
        isRoad = true;
    }
    return isRoad;
}
//获取指定位置的周边对象(类型1.为点击可行区域、类型2.为点击不可行区域)
PointArray *HKScene::walkableAdjacentTilesCoordForTileCoord(int checkType,int index,const Point &tileCoord) const
{
    int len = index*8;
    PointArray *tmp = PointArray::create(len);

    bool t = false;
    bool l = false;
    bool b = false;
    bool r = false;

    // 上
    Point UpPoint(tileCoord.x, tileCoord.y - index);
    if (this->isValidTileCoord(UpPoint) && this->isRoadAtTileCoord(UpPoint))//确认位置是否在地图内以及是不是障碍物
    {
        tmp->addControlPoint(UpPoint);
        t = true;
    }
    
    // 左
    Point LeftPoint(tileCoord.x - index, tileCoord.y);
    if (this->isValidTileCoord(LeftPoint) && this->isRoadAtTileCoord(LeftPoint))
    {
        tmp->addControlPoint(LeftPoint);
        l = true;
    }
    
    // 下
    Point DownPoint(tileCoord.x, tileCoord.y + index);
    if (this->isValidTileCoord(DownPoint) && this->isRoadAtTileCoord(DownPoint))
    {
        tmp->addControlPoint(DownPoint);
        b = true;
    }
    
    // 右
    Point RightPoint(tileCoord.x + index, tileCoord.y);
    if (this->isValidTileCoord(RightPoint) && this->isRoadAtTileCoord(RightPoint))
    {
        tmp->addControlPoint(RightPoint);
        r = true;
    }
    
    if(checkType == 1)//点击可行区域
    {
        // 左上
        Point p(tileCoord.x - index, tileCoord.y - index);
        if (t && l && this->isValidTileCoord(p) && this->isRoadAtTileCoord(p))
        {
            tmp->addControlPoint(p);
        }
        
        // 左下
        p.setPoint(tileCoord.x - index, tileCoord.y + index);
        if (b && l && this->isValidTileCoord(p) && this->isRoadAtTileCoord(p))
        {
            tmp->addControlPoint(p);
        }
        
        // 右上
        p.setPoint(tileCoord.x + index, tileCoord.y - index);
        if (t && r && this->isValidTileCoord(p) && this->isRoadAtTileCoord(p))
        {
            tmp->addControlPoint(p);
        }
        
        // 右下
        p.setPoint(tileCoord.x + index, tileCoord.y + index);
        if (b && r && this->isValidTileCoord(p) && this->isRoadAtTileCoord(p))
        {
            tmp->addControlPoint(p);
        }
    }
    else //不可行区域特殊处理
    {
        //处理四个方向区域
        for (int i = -index; i <= index; i++)
        {
            if(i != 0)
            {
                // 上侧
                Point tempPosition1(UpPoint.x - i, UpPoint.y);
                if (this->isValidTileCoord(tempPosition1) && this->isRoadAtTileCoord(tempPosition1))
                {
                    tmp->addControlPoint(tempPosition1);
                }
                // 下侧
                Point tempPosition2(DownPoint.x - i, DownPoint.y);
                if (this->isValidTileCoord(tempPosition2) && this->isRoadAtTileCoord(tempPosition2))
                {
                    tmp->addControlPoint(tempPosition2);
                }
            }
            
        }
        
        int rangeIndex = index -1;
        for (int i = -rangeIndex; i <= rangeIndex; i++)
        {
            if(i != 0)
            {
                // 右侧
                Point tempPosition1(RightPoint.x , RightPoint.y - i);
                if (this->isValidTileCoord(tempPosition1) && this->isRoadAtTileCoord(tempPosition1))
                {
                    tmp->addControlPoint(tempPosition1);
                }
                //左侧
                Point tempPosition2(LeftPoint.x , LeftPoint.y - i);
                if (this->isValidTileCoord(tempPosition2) && this->isRoadAtTileCoord(tempPosition2))
                {
                    tmp->addControlPoint(tempPosition2);
                }
            }
        }
    }

    return tmp;
}


//地图跟随镜头
void HKScene::setViewpointCenter(const Point &position)
{
    Size winSize = Director::getInstance()->getWinSize();

    int x = MAX(position.x, winSize.width / 2);
    int y = MAX(position.y, winSize.height / 2);
    x = MIN(x, (_tileMap->getMapSize().width * _tileMap->getTileSize().width) - winSize.width / 2);
    y = MIN(y, (_tileMap->getMapSize().height * _tileMap->getTileSize().height) - winSize.height / 2);
    Point actualPosition(x, y);

    Point centerOfView(winSize.width / 2, winSize.height / 2);
    Point viewPoint = centerOfView - actualPosition;

    m_mapNode->setPosition(viewPoint);
	
	
}


void HKScene::update(float delta)
{
    this->setViewpointCenter(m_pRole->getPosition());
}


