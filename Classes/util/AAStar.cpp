#include "AAStar.h"


AAStar::AAStar()
{
	memset(m_OpenList, 0, sizeof(m_OpenList));
	//memset(m_Node, 0, sizeof(m_Node));
	m_OpenCount = 0;
	m_OpenID = -1;                           //ID要作为索引值使用开始赋值为-1
	m_NodeInfo = NULL;
	m_Node = NULL;
	m_Map = NULL;
	m_Path.clear();
}


AAStar::~AAStar()
{
	for (int i = 0; i < m_H; i++)
	{
		delete[m_W] m_Map[i];
		m_Map[i] = NULL;
		delete[m_W] m_NodeInfo[i];
		m_NodeInfo[i] = NULL;
	}
	delete[m_H] m_Map;
	m_Map = NULL;
	delete[m_H] m_NodeInfo;
	m_NodeInfo = NULL;
	delete[] m_Node;
	m_Node = NULL;
}


void AAStar::Init(int* Map, int w, int h,TMXTiledMap * map)
{
	m_W = w;
	m_H = h;
	assert(Map != NULL);   //assert宏的原型定义在<assert.h>中，其作用是如果它的条件返回错误，则终止程序执行

	//动态初始化二维数组保存地图
	m_Map = new int*[h];
	assert(m_Map != NULL);  
	for (int i = 0; i < h; i++)
	{
		m_Map[i] = new int[w];
		assert(m_Map[i] != NULL);
		memcpy(m_Map[i], Map+i*w, w*sizeof(int));
	}

	//用于保存加入过开放列表的节点
	m_Node = new NodePT[w*h];
	assert(m_Node != NULL);
	memset(m_Node, 0, sizeof(NodePT)*w*h);

	//用于记录保每个节点信息
	m_NodeInfo = new NodeInfo*[h];
	assert(m_NodeInfo != NULL);
	for (int i = 0 ; i < h; i++)
	{
		m_NodeInfo[i]=new NodeInfo[w];
		assert(m_NodeInfo[i] != NULL);
		memset(m_NodeInfo[i], 0, sizeof(NodeInfo)*w);
	}
	this->_tileMap = map;
}


void AAStar::Find(int xBegin, int yBegin, int xEnd, int yEnd)
{
	PT PtBegin = {xBegin, yBegin};
	OpenNote(PtBegin, 0, 0, 0);
	NodePT CurrNode;
	memset(&CurrNode, 0, sizeof(NodePT));
	int currID = 0;
	int tmpID = 0;
	int tmpScoreF = 0;
	int tmpScoreG = 0;
	std::list<PT> AroundNode;
	//若开放列表不为空
	while (m_OpenCount > 0)
	{
		//每次取出二叉堆（开放列表）中最前面的ID
		currID = m_OpenList[0];
		//作加入关闭列表标记
		CloseNote(currID);
		//取得此节点坐标
		CurrNode.m_Pos = m_Node[currID].m_Pos;
		//是否是重点坐标
		if ((CurrNode.m_Pos.x == xEnd) && (CurrNode.m_Pos.y == yEnd))
		{
			GetPath(PtBegin,currID);
			break;
		}
		//得到此节点周围的节点
		GetAroundNode(CurrNode.m_Pos, &AroundNode);
		std::list<PT>::iterator it = AroundNode.begin();
		for( ;it != AroundNode.end(); it++)
		{
			//计算G值和F值
			tmpScoreG = CurrNode.m_ScoreF + ((CurrNode.m_Pos.x == (*it).x) || (CurrNode.m_Pos.y == (*it).y)) ? W_H_MOVE : SLANTMOVE;
			tmpScoreF = tmpScoreG + (abs((*it).x - xEnd) + abs((*it).y - yEnd)) * W_H_MOVE;  //曼哈顿法
			//节点是否已在开放列表
			if (IsInOpenList(*it))
			{
				//由坐标获得节点在开放列表中的ID
				tmpID = m_NodeInfo[(*it).y][(*it).x].m_NodeID;
				//现在的G值与原来的G值进行比较
				if (tmpScoreG < m_Node[tmpID].m_ScoreG)
				{
					//若现在的G值更小则修改其F、G值，并更换其父节点
					m_Node[tmpID].m_ScoreF = tmpScoreF;
					m_Node[tmpID].m_ScoreG = tmpScoreG;
					m_Node[tmpID].m_FatherID = currID;
					//重新排序
					HeapAheadSort(GetIndex(tmpID));
				}
			}
			else //不再开放列表则将节点加入开放列表
			{
				PT Point = {(*it).x, (*it).y};
				OpenNote(Point, tmpScoreF, tmpScoreG, currID);
			}
		}
	}
}


void AAStar::OpenNote(PT Point, int ScoreF, int ScoreG, int FatherID)
{
	m_OpenCount++;
	m_OpenID++;
	m_NodeInfo[Point.y][Point.x].m_InCloseList = true; //标记加入开放列表
	m_NodeInfo[Point.y][Point.x].m_NodeID = m_OpenID;  //赋予唯一ID
	m_Node[m_OpenID].m_Pos = Point;                    //以ID为索引给节点变量赋值
	m_Node[m_OpenID].m_ScoreF = ScoreF;
	m_Node[m_OpenID].m_ScoreG = ScoreG;
	m_Node[m_OpenID].m_FatherID = FatherID;
	m_OpenList[m_OpenCount-1] = m_OpenID;                //将节点ID存入开放列表
	HeapAheadSort(m_OpenCount);                        //进行二叉堆排序以保证F最小ID排在最前
}


void AAStar::CloseNote(int ID)
{
	m_NodeInfo[m_Node[ID].m_Pos.y][m_Node[ID].m_Pos.x].m_InCloseList = true;
	m_NodeInfo[m_Node[ID].m_Pos.y][m_Node[ID].m_Pos.x].m_InOpenList = false;
	//将二叉堆最后一个ID置于最前，并进行排序
	m_OpenList[0] = m_OpenList[--m_OpenCount];
	HeapBackSort();
}


void AAStar::HeapAheadSort(int Index)
{
	//索引均为在二叉堆m_OpenList中索引
	int fatherIndex = 0;
	int tmpID = 0;
	while (Index > 1)
	{
		fatherIndex = Index/2;
		//若当前节点F值小于父节点F值，将F值较小的ID向前移动
		if (m_Node[m_OpenList[Index-1]].m_ScoreF < m_Node[m_OpenList[fatherIndex-1]].m_ScoreF)
		{
			tmpID = m_OpenList[Index-1];
			m_OpenList[Index-1] = m_OpenList[fatherIndex-1];
			m_OpenList[fatherIndex-1] = tmpID;
			Index = fatherIndex;
		}
		else
		{
			break;
		}
	}
}


void AAStar::HeapBackSort()
{
	int currIndex = 1;                            //当前节点
	int currChildIndex = 2*currIndex;             //当前节点的子节点
	int tmp = m_OpenList[currIndex-1];            //保存首节点ID
	//判断节点是否有子节点
	while (currChildIndex <= m_OpenCount)
	{  
		//判断是否有两个子节点
		if (currChildIndex + 1 <= m_OpenCount)
		{
			//获得最小F值的子节点
			if (m_Node[m_OpenList[currChildIndex-1]].m_ScoreF > m_Node[m_OpenList[currChildIndex-1+1]].m_ScoreF)
			{
				currChildIndex++;
			}
		}
		//如果子节点的F值比当前节点大，结束循环
		if (m_Node[m_OpenList[currIndex-1]].m_ScoreF < m_Node[m_OpenList[currChildIndex-1]].m_ScoreF)
		{
			break;
		}
		else
		{
			//否则将子节点ID赋予当前前节点
			m_OpenList[currIndex-1] = m_OpenList[currChildIndex-1];
			//改变索引继续循环
			currIndex = currChildIndex;
			currChildIndex *= 2;
		}
	}
	//循环结束时找到保存的节点ID的正确位置
	m_OpenList[currIndex-1] = tmp;

}


bool AAStar::IsInOpenList(PT Point)
{
	return m_NodeInfo[Point.y][Point.x].m_InOpenList;
}


bool AAStar::IsInCloseList(PT Point)
{
	return m_NodeInfo[Point.y][Point.x].m_InCloseList;
}


void AAStar::GetAroundNode(PT Point, std::list<PT> *AroundNode)
{
	//每次获得前清空list
	AroundNode->clear();

	int CurrX = 0;
	int CurrY = 0;
	bool LCanPass = false;
	bool RCanPass = false;
	bool UCanPass = false;
	bool DCanPass = false;

	//左边节点
	CurrX = Point.x - 1;
	CurrY = Point.y;
	//是否出界
	if ((CurrX >= 0) && (CurrX < m_W) && (CurrY >= 0) && (CurrY < m_H))
	{
		//是否为障碍物
		if (m_Map[CurrY][CurrX] != 1)
		{
			LCanPass = true;
			PT tmpPoint = { CurrX, CurrY };
			//是否在关闭列表
			if (!IsInCloseList(tmpPoint))
			{
				AroundNode->push_back(tmpPoint);
			}
			
		}
		
	}
	//右边节点
	CurrX = Point.x + 1;
	CurrY = Point.y;
	//是否出界
	if ((CurrX >= 0) && (CurrX < m_W) && (CurrY >= 0) && (CurrY < m_H))
	{
		//是否为障碍物
		if (m_Map[CurrY][CurrX] != 1)
		{
			RCanPass = true;
			PT tmpPoint = { CurrX, CurrY };
			if (!IsInCloseList(tmpPoint))
			{
				AroundNode->push_back(tmpPoint);
			}
		}
	}
	//上节点
	CurrX = Point.x;
	CurrY = Point.y - 1;
	//是否出界
	if ((CurrX >= 0) && (CurrX < m_W) && (CurrY >= 0) && (CurrY < m_H))
	{
		//是否为障碍物
		if (m_Map[CurrY][CurrX] != 1)
		{
			UCanPass = true;
			PT tmpPoint = { CurrX, CurrY };
			if (!IsInCloseList(tmpPoint))
			{
				AroundNode->push_back(tmpPoint);
			}
		}
	}
	//下节点
	CurrX = Point.x;
	CurrY = Point.y + 1;
	//是否出界
	if ((CurrX >= 0) && (CurrX < m_W) && (CurrY >= 0) && (CurrY < m_H))
	{
		//是否为障碍物
		if (m_Map[CurrY][CurrX] != 1)
		{
			DCanPass = true;
			PT tmpPoint = { CurrX, CurrY };
			if (!IsInCloseList(tmpPoint))
			{
				AroundNode->push_back(tmpPoint);
			}
		}
	}
	//左上节点，首先判断正左和正上方是否有障碍物
	if (LCanPass && UCanPass)
	{
		CurrX = Point.x - 1;
		CurrY = Point.y - 1;
		//是否出界
		if ((CurrX >= 0) && (CurrX < m_W) && (CurrY >= 0) && (CurrY < m_H))
		{
			//是否为障碍物
			if (m_Map[CurrY][CurrX] != 1)
			{
				PT tmpPoint = { CurrX, CurrY };
				if (!IsInCloseList(tmpPoint))
				{
					AroundNode->push_back(tmpPoint);
				}
			}
		}
	}
	//左下节点
	if (LCanPass && DCanPass)
	{
		CurrX = Point.x - 1;
		CurrY = Point.y + 1;
		//是否出界
		if ((CurrX >= 0) && (CurrX < m_W) && (CurrY >= 0) && (CurrY < m_H))
		{
			//是否为障碍物
			if (m_Map[CurrY][CurrX] != 1)
			{
				PT tmpPoint = { CurrX, CurrY };
				if (!IsInCloseList(tmpPoint))
				{
					AroundNode->push_back(tmpPoint);
				}
			}
		}
	}
	//右上节点
	if (RCanPass && UCanPass)
	{
		CurrX = Point.x + 1;
		CurrY = Point.y - 1;
		//是否出界
		if ((CurrX >= 0) && (CurrX < m_W) && (CurrY >= 0) && (CurrY < m_H))
		{
			//是否为障碍物
			if (m_Map[CurrY][CurrX] != 1)
			{
				PT tmpPoint = { CurrX, CurrY };
				if (!IsInCloseList(tmpPoint))
				{
					AroundNode->push_back(tmpPoint);
				}
			}
		}
	}
	//右下节点
	if (RCanPass && DCanPass)
	{
		CurrX = Point.x + 1;
		CurrY = Point.y + 1;
		//是否出界
		if ((CurrX >= 0) && (CurrX < m_W) && (CurrY >= 0) && (CurrY < m_H))
		{
			//是否为障碍物
			if (m_Map[CurrY][CurrX] != 1)
			{
				PT tmpPoint = { CurrX, CurrY };
				if (!IsInCloseList(tmpPoint))
				{
					AroundNode->push_back(tmpPoint);
				}
			}
		}
	}
}

int AAStar::GetIndex(int ID)
{
	for (int i = 1; i <= m_OpenCount; i++)
	{
		if (m_OpenList[i] == ID)
		{
			return i;
		}
	}
	return -1;
}


void AAStar::GetPath(PT Point, int ID)
{
	int CurrX = m_Node[ID].m_Pos.x;
	int CurrY = m_Node[ID].m_Pos.y;
	while ((CurrX != Point.x) || (CurrY != Point.y))
	{
		PT Pt = { CurrX, CurrY }; ANode* node=new ANode(CurrX,CurrY); 
		m_Path.push_back(Pt);
		s_Path.pushBack(node);
		ID = m_Node[ID].m_FatherID;
		CurrX = m_Node[ID].m_Pos.x;
		CurrY = m_Node[ID].m_Pos.y;
	}
	PT Pt = {CurrX, CurrY}; ANode* node = new ANode(CurrX, CurrY);
	m_Path.push_back(Pt);
	s_Path.pushBack(node);
}

//处理三点共线
ANode* AAStar::floydVector(ANode* n1, ANode* n2)
{
	ANode* target= new ANode(0, 0);
	target->x = n1->x - n2->x;
	target->y = n1->y - n2->y;
	return target;
}

//弗洛伊德平滑处理
void AAStar::floyd()
{
	if (s_Path.empty())
		return;
	int len = s_Path.size();
	if (len>2)
	{
		ANode* spt;
		ANode* tpt;
		spt= this->floydVector(s_Path.at(len - 1), s_Path.at(len - 2));
		for (int i = len - 3; i >= 0; i--)
		{
			tpt = this->floydVector(s_Path.at(i + 1), s_Path.at(i));
			if (spt->x == tpt->x && spt->y == tpt->y)
			{
				s_Path.erase(i + 1);				
			}else{
				spt->x = tpt->x;
				spt->y = tpt->y;
			}
		}
	}

	ssize_t index = 0;
	//合并消除拐点操作。
	while (1)
	{
		len = s_Path.size();
		if (len - index > 3)
		{
			ANode* starPoint = s_Path.at(index);
			for (ssize_t i = len - 1; i > index; i--)
			{
				ANode* tempPoint = s_Path.at(i);			
				if (!this->ishasBarrier(starPoint, tempPoint))//两点之间无障碍，则合并
				{
					for (ssize_t ia = i - 1; ia > index; ia--)
					{
						log("delete point %d,%d", s_Path.at(ia)->x, s_Path.at(ia)->y);
					    s_Path.erase(ia);					
					}
					index = index + 1;//保存当前下次起点
					break;
				}
				else
				{
					if (i == index + 1)//查询已经带到尽头
					{
						index = index + 1;//保存当前下次起点
					}
				}
			}
		}
		else {
			break;
		}
	}
	//除去起点
	s_Path.erase(s_Path.size()-1);
	
}



bool AAStar::ishasBarrier(ANode * startPos, ANode * endPos)
{
	//如果起点终点是同一个点,直接返回
	if (startPos->x == endPos->x && startPos->y == endPos->y)return false;
	//循环递增量
	float i;
	//循环起始值
	int loopStart;
	//循环终结值
	int loopEnd;
	//起终点连线所经过的节点
	PointArray * passedNodeList;

	//两节点中心位置
	ANode* point1=new ANode(startPos->x + 0.5, startPos->y + 0.5);
	ANode* point2 =new ANode(endPos->x + 0.5, endPos->y + 0.5);

	//根据起点终点间横纵向距离的大小来判断遍历方向
	int distX = abs(endPos->x - startPos->x);
	int distY = abs(endPos->y - startPos->y);

	//遍历方向，为true则为横向遍历，否则为纵向遍历
	bool loopDirection = distX > distY ? true : false;

	if (loopDirection)
	{
		loopStart = MIN(startPos->x, endPos->x);
		loopEnd = MAX(startPos->x, endPos->x);

		//开始横向遍历起点与终点间的节点看是否存在障碍(不可移动点)
		for (i = loopStart; i <= loopEnd; i++)
		{
			//由于线段方程是根据终起点中心点连线算出的，所以对于起始点来说需要根据其中心点位置来算，而对于其他点则根据左上角来算
			if (i == loopStart)
			{
				i = i + 0.5;
			}
			//根据x得到直线上的y值
			float yPos = this->getLineValue(point1, point2, 0, i);
			//获取经过的点
			passedNodeList = this->getNodesUnderPoint(i, yPos);
			//检查经过的节点是否有障碍物，若有则返回true
			for (ssize_t index = 0, len = passedNodeList->count(); index < len; index++)
			{
				Point thisPos = passedNodeList->getControlPointAtIndex(index);
				if (this->isRoadAtTileCoord(thisPos) == false)
				{
					return true;
				}
				else
				{
				//m_endTileCoord = thisPos;
				}
			}

			if (i == loopStart + .5)i -= .5;
		}

	}
	else
	{
		loopStart = MIN(startPos->y, endPos->y);
		loopEnd = MAX(startPos->y, endPos->y);

		//开始纵向遍历起点与终点间的节点看是否存在障碍(不可移动点)
		for (i = loopStart; i <= loopEnd; i++)
		{
			if (i == loopStart)i += .5;
			//根据y得到直线上的x值
			float xPos = this->getLineValue(point1, point2, 1, i);

			passedNodeList = this->getNodesUnderPoint(xPos, i);

			for (ssize_t index = 0, len = passedNodeList->count(); index < len; index++)
			{
				Point thisPos = passedNodeList->getControlPointAtIndex(index);
				if (this->isRoadAtTileCoord(thisPos) == false)
				{
					return true;
				}
				else
				{
				//	m_endTileCoord = thisPos;
				}
			}

			if (i == loopStart + .5)i -= .5;
		}
	}

	return false;
}

PointArray * AAStar::getNodesUnderPoint(float xPos, float yPos)
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
	if (xIsInt && yIsInt)
	{
		result = PointArray::create(4);
		result->addControlPoint(Vec2(xPos - 1, yPos - 1));
		result->addControlPoint(Vec2(xPos, yPos - 1));
		result->addControlPoint(Vec2(xPos - 1, yPos));
		result->addControlPoint(Vec2(xPos, yPos));
	}
	//点由2节点共享情况
	//点落在两节点左右临边上
	else if (xIsInt && !yIsInt)
	{
		result = PointArray::create(2);
		result->addControlPoint(Vec2(floor(xPos - 1), floor(yPos)));
		result->addControlPoint(Vec2(floor(xPos), floor(yPos)));
	}
	//点落在两节点上下临边上
	else if (!xIsInt && yIsInt)
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

float AAStar::getLineValue(ANode * ponit1, ANode * point2, int type, float value)
{
	// 先考虑两点在一条垂直于坐标轴直线的情况，此时直线方程为 y = a 或者 x = a 的形式
	if (ponit1->x == point2->x)
	{
		if (type == 1)
		{

			return ponit1->x;

		}
	}
	else if (ponit1->y == point2->y)
	{
		if (type == 0)
		{
			return ponit1->y;
		}

		return value;
	}

	// 当两点确定直线不垂直于坐标轴时直线方程设为 y = ax + b
	//根据y1 = ax1 + b和y2 = ax2 + b上下两式相减消去b, 得到 a = ( y1 - y2 ) / ( x1 - x2 )

	float a = (ponit1->y - point2->y)*1.0 / (ponit1->x - point2->x);

	//将a的值代入任一方程式即可得到b
	float b = ponit1->y - a * ponit1->x;

	//把a,b值代入即可得到结果函数
	if (type == 0)
	{
		return a * value + b;
	}
	else if (type == 1)
	{
		return (value - b) / a;
	}

	return value;
}

bool AAStar::isRoadAtTileCoord(Point node)
{

	int CurrX = node.x; int CurrY = node.y;
	if ((CurrX >= 0) && (CurrX < m_W) && (CurrY >= 0) && (CurrY < m_H))	//确认位置是否在地图内
	{
		return false;
	}
	TMXLayer* _road = _tileMap->getLayer("road");
	int gid = _road->getTileGIDAt(Point(CurrX, CurrY));
	Value properties = _tileMap->getPropertiesForGID(gid);//获取设置属性
	ValueMap map = properties.asValueMap();
	std::string value = map.at("road").asString();
	if (value.compare("1") == 0)
		return true;
	else
		return false;
}

