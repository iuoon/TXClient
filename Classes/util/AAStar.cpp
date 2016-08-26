#include "AAStar.h"


AAStar::AAStar()
{
	memset(m_OpenList, 0, sizeof(m_OpenList));
	//memset(m_Node, 0, sizeof(m_Node));
	m_OpenCount = 0;
	m_OpenID = -1;                           //IDҪ��Ϊ����ֵʹ�ÿ�ʼ��ֵΪ-1
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
	assert(Map != NULL);   //assert���ԭ�Ͷ�����<assert.h>�У�����������������������ش�������ֹ����ִ��

	//��̬��ʼ����ά���鱣���ͼ
	m_Map = new int*[h];
	assert(m_Map != NULL);  
	for (int i = 0; i < h; i++)
	{
		m_Map[i] = new int[w];
		assert(m_Map[i] != NULL);
		memcpy(m_Map[i], Map+i*w, w*sizeof(int));
	}

	//���ڱ������������б�Ľڵ�
	m_Node = new NodePT[w*h];
	assert(m_Node != NULL);
	memset(m_Node, 0, sizeof(NodePT)*w*h);

	//���ڼ�¼��ÿ���ڵ���Ϣ
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
	//�������б�Ϊ��
	while (m_OpenCount > 0)
	{
		//ÿ��ȡ������ѣ������б�����ǰ���ID
		currID = m_OpenList[0];
		//������ر��б���
		CloseNote(currID);
		//ȡ�ô˽ڵ�����
		CurrNode.m_Pos = m_Node[currID].m_Pos;
		//�Ƿ����ص�����
		if ((CurrNode.m_Pos.x == xEnd) && (CurrNode.m_Pos.y == yEnd))
		{
			GetPath(PtBegin,currID);
			break;
		}
		//�õ��˽ڵ���Χ�Ľڵ�
		GetAroundNode(CurrNode.m_Pos, &AroundNode);
		std::list<PT>::iterator it = AroundNode.begin();
		for( ;it != AroundNode.end(); it++)
		{
			//����Gֵ��Fֵ
			tmpScoreG = CurrNode.m_ScoreF + ((CurrNode.m_Pos.x == (*it).x) || (CurrNode.m_Pos.y == (*it).y)) ? W_H_MOVE : SLANTMOVE;
			tmpScoreF = tmpScoreG + (abs((*it).x - xEnd) + abs((*it).y - yEnd)) * W_H_MOVE;  //�����ٷ�
			//�ڵ��Ƿ����ڿ����б�
			if (IsInOpenList(*it))
			{
				//�������ýڵ��ڿ����б��е�ID
				tmpID = m_NodeInfo[(*it).y][(*it).x].m_NodeID;
				//���ڵ�Gֵ��ԭ����Gֵ���бȽ�
				if (tmpScoreG < m_Node[tmpID].m_ScoreG)
				{
					//�����ڵ�Gֵ��С���޸���F��Gֵ���������丸�ڵ�
					m_Node[tmpID].m_ScoreF = tmpScoreF;
					m_Node[tmpID].m_ScoreG = tmpScoreG;
					m_Node[tmpID].m_FatherID = currID;
					//��������
					HeapAheadSort(GetIndex(tmpID));
				}
			}
			else //���ٿ����б��򽫽ڵ���뿪���б�
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
	m_NodeInfo[Point.y][Point.x].m_InCloseList = true; //��Ǽ��뿪���б�
	m_NodeInfo[Point.y][Point.x].m_NodeID = m_OpenID;  //����ΨһID
	m_Node[m_OpenID].m_Pos = Point;                    //��IDΪ�������ڵ������ֵ
	m_Node[m_OpenID].m_ScoreF = ScoreF;
	m_Node[m_OpenID].m_ScoreG = ScoreG;
	m_Node[m_OpenID].m_FatherID = FatherID;
	m_OpenList[m_OpenCount-1] = m_OpenID;                //���ڵ�ID���뿪���б�
	HeapAheadSort(m_OpenCount);                        //���ж���������Ա�֤F��СID������ǰ
}


void AAStar::CloseNote(int ID)
{
	m_NodeInfo[m_Node[ID].m_Pos.y][m_Node[ID].m_Pos.x].m_InCloseList = true;
	m_NodeInfo[m_Node[ID].m_Pos.y][m_Node[ID].m_Pos.x].m_InOpenList = false;
	//����������һ��ID������ǰ������������
	m_OpenList[0] = m_OpenList[--m_OpenCount];
	HeapBackSort();
}


void AAStar::HeapAheadSort(int Index)
{
	//������Ϊ�ڶ����m_OpenList������
	int fatherIndex = 0;
	int tmpID = 0;
	while (Index > 1)
	{
		fatherIndex = Index/2;
		//����ǰ�ڵ�FֵС�ڸ��ڵ�Fֵ����Fֵ��С��ID��ǰ�ƶ�
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
	int currIndex = 1;                            //��ǰ�ڵ�
	int currChildIndex = 2*currIndex;             //��ǰ�ڵ���ӽڵ�
	int tmp = m_OpenList[currIndex-1];            //�����׽ڵ�ID
	//�жϽڵ��Ƿ����ӽڵ�
	while (currChildIndex <= m_OpenCount)
	{  
		//�ж��Ƿ��������ӽڵ�
		if (currChildIndex + 1 <= m_OpenCount)
		{
			//�����СFֵ���ӽڵ�
			if (m_Node[m_OpenList[currChildIndex-1]].m_ScoreF > m_Node[m_OpenList[currChildIndex-1+1]].m_ScoreF)
			{
				currChildIndex++;
			}
		}
		//����ӽڵ��Fֵ�ȵ�ǰ�ڵ�󣬽���ѭ��
		if (m_Node[m_OpenList[currIndex-1]].m_ScoreF < m_Node[m_OpenList[currChildIndex-1]].m_ScoreF)
		{
			break;
		}
		else
		{
			//�����ӽڵ�ID���赱ǰǰ�ڵ�
			m_OpenList[currIndex-1] = m_OpenList[currChildIndex-1];
			//�ı���������ѭ��
			currIndex = currChildIndex;
			currChildIndex *= 2;
		}
	}
	//ѭ������ʱ�ҵ�����Ľڵ�ID����ȷλ��
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
	//ÿ�λ��ǰ���list
	AroundNode->clear();

	int CurrX = 0;
	int CurrY = 0;
	bool LCanPass = false;
	bool RCanPass = false;
	bool UCanPass = false;
	bool DCanPass = false;

	//��߽ڵ�
	CurrX = Point.x - 1;
	CurrY = Point.y;
	//�Ƿ����
	if ((CurrX >= 0) && (CurrX < m_W) && (CurrY >= 0) && (CurrY < m_H))
	{
		//�Ƿ�Ϊ�ϰ���
		if (m_Map[CurrY][CurrX] != 1)
		{
			LCanPass = true;
			PT tmpPoint = { CurrX, CurrY };
			//�Ƿ��ڹر��б�
			if (!IsInCloseList(tmpPoint))
			{
				AroundNode->push_back(tmpPoint);
			}
			
		}
		
	}
	//�ұ߽ڵ�
	CurrX = Point.x + 1;
	CurrY = Point.y;
	//�Ƿ����
	if ((CurrX >= 0) && (CurrX < m_W) && (CurrY >= 0) && (CurrY < m_H))
	{
		//�Ƿ�Ϊ�ϰ���
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
	//�Ͻڵ�
	CurrX = Point.x;
	CurrY = Point.y - 1;
	//�Ƿ����
	if ((CurrX >= 0) && (CurrX < m_W) && (CurrY >= 0) && (CurrY < m_H))
	{
		//�Ƿ�Ϊ�ϰ���
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
	//�½ڵ�
	CurrX = Point.x;
	CurrY = Point.y + 1;
	//�Ƿ����
	if ((CurrX >= 0) && (CurrX < m_W) && (CurrY >= 0) && (CurrY < m_H))
	{
		//�Ƿ�Ϊ�ϰ���
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
	//���Ͻڵ㣬�����ж���������Ϸ��Ƿ����ϰ���
	if (LCanPass && UCanPass)
	{
		CurrX = Point.x - 1;
		CurrY = Point.y - 1;
		//�Ƿ����
		if ((CurrX >= 0) && (CurrX < m_W) && (CurrY >= 0) && (CurrY < m_H))
		{
			//�Ƿ�Ϊ�ϰ���
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
	//���½ڵ�
	if (LCanPass && DCanPass)
	{
		CurrX = Point.x - 1;
		CurrY = Point.y + 1;
		//�Ƿ����
		if ((CurrX >= 0) && (CurrX < m_W) && (CurrY >= 0) && (CurrY < m_H))
		{
			//�Ƿ�Ϊ�ϰ���
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
	//���Ͻڵ�
	if (RCanPass && UCanPass)
	{
		CurrX = Point.x + 1;
		CurrY = Point.y - 1;
		//�Ƿ����
		if ((CurrX >= 0) && (CurrX < m_W) && (CurrY >= 0) && (CurrY < m_H))
		{
			//�Ƿ�Ϊ�ϰ���
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
	//���½ڵ�
	if (RCanPass && DCanPass)
	{
		CurrX = Point.x + 1;
		CurrY = Point.y + 1;
		//�Ƿ����
		if ((CurrX >= 0) && (CurrX < m_W) && (CurrY >= 0) && (CurrY < m_H))
		{
			//�Ƿ�Ϊ�ϰ���
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

//�������㹲��
ANode* AAStar::floydVector(ANode* n1, ANode* n2)
{
	ANode* target= new ANode(0, 0);
	target->x = n1->x - n2->x;
	target->y = n1->y - n2->y;
	return target;
}

//��������ƽ������
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
	//�ϲ������յ������
	while (1)
	{
		len = s_Path.size();
		if (len - index > 3)
		{
			ANode* starPoint = s_Path.at(index);
			for (ssize_t i = len - 1; i > index; i--)
			{
				ANode* tempPoint = s_Path.at(i);			
				if (!this->ishasBarrier(starPoint, tempPoint))//����֮�����ϰ�����ϲ�
				{
					for (ssize_t ia = i - 1; ia > index; ia--)
					{
						log("delete point %d,%d", s_Path.at(ia)->x, s_Path.at(ia)->y);
					    s_Path.erase(ia);					
					}
					index = index + 1;//���浱ǰ�´����
					break;
				}
				else
				{
					if (i == index + 1)//��ѯ�Ѿ�������ͷ
					{
						index = index + 1;//���浱ǰ�´����
					}
				}
			}
		}
		else {
			break;
		}
	}
	//��ȥ���
	s_Path.erase(s_Path.size()-1);
	
}



bool AAStar::ishasBarrier(ANode * startPos, ANode * endPos)
{
	//�������յ���ͬһ����,ֱ�ӷ���
	if (startPos->x == endPos->x && startPos->y == endPos->y)return false;
	//ѭ��������
	float i;
	//ѭ����ʼֵ
	int loopStart;
	//ѭ���ս�ֵ
	int loopEnd;
	//���յ������������Ľڵ�
	PointArray * passedNodeList;

	//���ڵ�����λ��
	ANode* point1=new ANode(startPos->x + 0.5, startPos->y + 0.5);
	ANode* point2 =new ANode(endPos->x + 0.5, endPos->y + 0.5);

	//��������յ����������Ĵ�С���жϱ�������
	int distX = abs(endPos->x - startPos->x);
	int distY = abs(endPos->y - startPos->y);

	//��������Ϊtrue��Ϊ�������������Ϊ�������
	bool loopDirection = distX > distY ? true : false;

	if (loopDirection)
	{
		loopStart = MIN(startPos->x, endPos->x);
		loopEnd = MAX(startPos->x, endPos->x);

		//��ʼ�������������յ��Ľڵ㿴�Ƿ�����ϰ�(�����ƶ���)
		for (i = loopStart; i <= loopEnd; i++)
		{
			//�����߶η����Ǹ�����������ĵ���������ģ����Զ�����ʼ����˵��Ҫ���������ĵ�λ�����㣬��������������������Ͻ�����
			if (i == loopStart)
			{
				i = i + 0.5;
			}
			//����x�õ�ֱ���ϵ�yֵ
			float yPos = this->getLineValue(point1, point2, 0, i);
			//��ȡ�����ĵ�
			passedNodeList = this->getNodesUnderPoint(i, yPos);
			//��龭���Ľڵ��Ƿ����ϰ�������򷵻�true
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

		//��ʼ�������������յ��Ľڵ㿴�Ƿ�����ϰ�(�����ƶ���)
		for (i = loopStart; i <= loopEnd; i++)
		{
			if (i == loopStart)i += .5;
			//����y�õ�ֱ���ϵ�xֵ
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

	//�����Ľڵ㹲�����
	if (xIsInt && yIsInt)
	{
		result = PointArray::create(4);
		result->addControlPoint(Vec2(xPos - 1, yPos - 1));
		result->addControlPoint(Vec2(xPos, yPos - 1));
		result->addControlPoint(Vec2(xPos - 1, yPos));
		result->addControlPoint(Vec2(xPos, yPos));
	}
	//����2�ڵ㹲�����
	//���������ڵ������ٱ���
	else if (xIsInt && !yIsInt)
	{
		result = PointArray::create(2);
		result->addControlPoint(Vec2(floor(xPos - 1), floor(yPos)));
		result->addControlPoint(Vec2(floor(xPos), floor(yPos)));
	}
	//���������ڵ������ٱ���
	else if (!xIsInt && yIsInt)
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

float AAStar::getLineValue(ANode * ponit1, ANode * point2, int type, float value)
{
	// �ȿ���������һ����ֱ��������ֱ�ߵ��������ʱֱ�߷���Ϊ y = a ���� x = a ����ʽ
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

	// ������ȷ��ֱ�߲���ֱ��������ʱֱ�߷�����Ϊ y = ax + b
	//����y1 = ax1 + b��y2 = ax2 + b������ʽ�����ȥb, �õ� a = ( y1 - y2 ) / ( x1 - x2 )

	float a = (ponit1->y - point2->y)*1.0 / (ponit1->x - point2->x);

	//��a��ֵ������һ����ʽ���ɵõ�b
	float b = ponit1->y - a * ponit1->x;

	//��a,bֵ���뼴�ɵõ��������
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
	if ((CurrX >= 0) && (CurrX < m_W) && (CurrY >= 0) && (CurrY < m_H))	//ȷ��λ���Ƿ��ڵ�ͼ��
	{
		return false;
	}
	TMXLayer* _road = _tileMap->getLayer("road");
	int gid = _road->getTileGIDAt(Point(CurrX, CurrY));
	Value properties = _tileMap->getPropertiesForGID(gid);//��ȡ��������
	ValueMap map = properties.asValueMap();
	std::string value = map.at("road").asString();
	if (value.compare("1") == 0)
		return true;
	else
		return false;
}

