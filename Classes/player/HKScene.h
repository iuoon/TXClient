#ifndef __HK_SCENE_H__
#define __HK_SCENE_H__

#include "cocos2d.h"

class RoleNode;

class HKScene : public cocos2d::Layer
{
public:
    HKScene();
    ~HKScene();
    
    static cocos2d::Scene* createScene();

    virtual bool init();      
    
    CREATE_FUNC(HKScene);

    bool isRoadAtTileCoord(const cocos2d::Point &tileCoord) const;
    cocos2d::Point tileCoordForPosition(const cocos2d::Point &position) const;
    cocos2d::Point positionForTileCoord(const cocos2d::Point &tileCoord) const;

    bool isValidTileCoord(const cocos2d::Point &tileCoord) const;
    cocos2d::PointArray *walkableAdjacentTilesCoordForTileCoord(int checkType,int index,const cocos2d::Point &tileCoord) const;

    bool isRoadRect(const cocos2d::Point &tileCoord)const;
private:
    bool isPropAtTileCoordForLayer(const char *prop, const cocos2d::Point &tileCoord, cocos2d::TMXLayer *layer) const;
    void setViewpointCenter(const cocos2d::Point &position);
  

    virtual void update(float delta) override;
    
private:
    Node* m_mapNode;
	cocos2d::TMXTiledMap *_tileMap;
	cocos2d::TMXLayer *_bgLayer;

    cocos2d::SpriteBatchNode *_batchNode;
    RoleNode *m_pRole;
};

#endif // __HK_SCENE_H__
