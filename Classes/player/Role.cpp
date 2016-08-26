#include "player/Role.h"


Role::Role()
{
}

Role::~Role()
{
}



Role *Role::createNode()
{
	Role *pRet = new Role();
	if (pRet && pRet->initRole())
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

bool Role::initRole()
{
	bool bRet = false;
	do
	{
		std::string fileName = "model/orc.c3b";
		auto node = Sprite3D::create(fileName);
		node->setScale(3.0f);
		node->setGlobalZOrder(1);
		node->setRotation3D(Vec3(0, 180, 0));
		node->setPosition(0, 0);
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
		bRet = true;
	} while (0);

	return bRet;
}

void Role::moveToward(int* map, Point sp, Point ep, TMXTiledMap* tilemap)
{
	log("from====>%f ,%f    to====>%f ,%f", sp.x, sp.y, ep.x, ep.y);
	this->_tileMap = tilemap;

	this->stopAllActions();	//��һ������Ҫ������ڶ��ε�Ѱ·ʱ��ɫ�ҷ�

	AAStar astar;
	auto t1 = ::clock();
	astar.Init(map, 192, 128, tilemap);
	astar.Find(sp.x, sp.y, ep.x, ep.y);
	auto t2 = ::clock();
	Paths = astar.GetPaths();

	log("the time =======>%d", t2 - t1);
	log("the size =======>%d", Paths.size());

	if (Paths.size() == 0)
	{
		log("no find end!");
	}
	else
	{
		this->stepAndAnimate();
	}
}

void Role::stepAndAnimate()
{
	int i = Paths.size() - 1;
	if (i>=0)
	{
		auto it = Paths.at(i);
		Point currentp = this->getPosition();
		Point nextp = this->positionForTileCoord(Point(it->x, it->y));
		float distance = currentp.distance(nextp);
		if (currentp != nextp)
		{
			log("the distance======>%f", distance);
			float time = distance*0.01;
			//��϶����ͻص�
			MoveTo *moveAction = MoveTo::create(time, nextp);
			CallFunc *moveCallback = CallFunc::create(CC_CALLBACK_0(Role::stepAndAnimate, this));
			//�Ƴ����β���
			Paths.erase(i);
			// ���ж���
			Sequence *moveSequence = Sequence::create(moveAction, moveCallback, nullptr);
			moveSequence->setTag(100);
			this->runAction(moveSequence); //������϶����ص��ķ����������߶���
		}
	}
	
}


