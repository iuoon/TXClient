/***********************************************************************************/
/* ��ͼ�趨��0��ʾ��ͨ��1��ʾ�ϰ����ͼ��б���ƶ��� ��������б�����������֮һ���ϰ� 
/* ���򲻿�б���ƶ��������������ƶ��������ҷ������·�����֮һ���ϰ��򲻿��������ƶ���   
/* ��Ѱ·�㷨�Ѽ���������Ż�
/***********************************************************************************/
#pragma once
#include <memory>
#include <assert.h>
#include <iostream>
#include <math.h>
#include <list>
#include <vector>
#include "cocos2d.h"
USING_NS_CC;

using namespace std;

#define W_H_MOVE       10 //ˮƽ����ֱ�ƶ�һ��Ļ���
#define SLANTMOVE      14 //б���ƶ������ŶԽ����ƶ�����һ��Ļ���
#define MAXNODECOUNT   10000 //�����б�ɱ�������ڵ���

struct PT 
{
	int x;
	int y;
};



//���ڱ���ڵ���Ϣ
struct NodeInfo 
{
	int m_NodeID;        //�ڵ�ID
	bool m_InOpenList;   //�Ƿ��ڿ����б��У�trueΪ�ڿ����б��У�false���ڿ����б���
	bool m_InCloseList;  //�Ƿ��ڹر��б��У�true�ڹر��б�false���ڹر��б�
};

//���ڱ���ڵ�����
struct NodePT
{
	PT m_Pos;             //�ڵ�����
	int m_ScoreF;        //�ڵ�·������
	int m_ScoreG;        //�ڵ㵽��ʼ���·������
	int m_FatherID;      //�ڵ�ĸ��ڵ�ID
	//bool m_InOpenList;   //�Ƿ��ڿ����б��У�trueΪ�ڿ����б��У�false���ڿ����б���
	//bool m_InCloseList;  //�Ƿ��ڹر��б��У�true�ڹر��б�false���ڹر��б�
};


class ANode :public Ref
{
public:
	ANode(int x, int y) {
		this->x = x; this->y = y;
	};
	int x;
	int y;
};




class AAStar
{
public:
	AAStar();
	~AAStar();

	void Init(int* Map, int w, int h, TMXTiledMap* map); //����

	//���ܣ�Ѱ·
	//������xBegin������ʼ��X���� yBegin������ʼ��Y����
	//      xEnd�����յ�X����     yEnd�����յ�Y����
	//����:
	void Find(int xBegin, int yBegin, int xEnd, int yEnd); 

	//���ܣ�Ϊ�ڵ������뿪���б���
	//������Point�����ڵ����� ScoreF�����ڵ�·������ 
	//      ScoreG�����ڵ㵽���·������ FatherID�����ڵ�ĸ��ڵ�ID
	//���أ�
	void OpenNote(PT Point, int ScoreF, int ScoreG,int FatherID);

	//���ܣ�Ϊ�ڵ�������ر��б���
	//������ID����Ҫ����ǵĽڵ�ID
	//���أ�
	void CloseNote(int ID);
	
	//���ܣ��¼��뿪���б���޸�·�����ֺ�Զ���ѽ�������ʹ��Fֵ��С�Ľڵ��ID����ǰ��
	//������Index�����޸Ļ��¼���ڵ�ID��m_OpenList������
	//���أ�
	void HeapAheadSort(int Index);
	
	//���ܣ��ӿ����б���ȡ��Fֵ��С�ڵ��ID��Զ���ѽ�������ʹ��Fֵ��С�Ľڵ��ID����ǰ��
	//������
	//���أ�
	void HeapBackSort();

	//���ܣ��ж��ڵ��Ƿ��ڿ����б���
	//������Point����Ҫ�ж��ڵ�����
	//���أ�true�����ڿ����б� false��������
	bool IsInOpenList(PT Point);

	//���ܣ��ж��ڵ��Ƿ��ڹر��б���
	//������Point����Ҫ�ж��ڵ�����
	//���أ�true�����ڿ����б� false��������
	bool IsInCloseList(PT Point);
	
	//���ܣ���ýڵ����Χ��ͨ���ڵ�
	//������Point�����ڵ����� AroundNode�������ڱ�����Χ�ڵ�����
	//���أ�
	void GetAroundNode(PT Point,std::list<PT>* AroundNode);

	//���ܣ����ĳID�ڵ���OpenList������
	//������ID����Ҫ�õ������Ľڵ�ID
	//���أ��ڵ�����ֵ
	int GetIndex(int ID);

	//���ܣ���·������������m_Path��
	//������Point����������� ID�����յ�ID
	//���أ�
	void  GetPath(PT Point,int ID);

	//��������·�����յ�����ǰ��
	std::list<PT>& GetPath()
	{
		return m_Path;
	}
	//����cocos2d Vector����·��
	Vector<ANode*>& GetPaths() {
		this->floyd();
		return s_Path;
	}

	//��������ƽ������
	void floyd();
	//�������㹲��
	ANode* floydVector(ANode* Point1, ANode* Point2);

	//�ж�����֮���Ƿ����ϰ�
	bool ishasBarrier(ANode* p1,ANode* p2);

	// �õ�һ�����µ����нڵ�
	PointArray *getNodesUnderPoint(float xPos, float yPos);
	float getLineValue(ANode* ponit1, ANode* point2, int type, float value);

	//�Ƿ�Ϊ��·
	bool isRoadAtTileCoord(Point node);


private:
	int m_OpenList[MAXNODECOUNT];         //�����б�������Ż���������Žڵ��ID��Fֵ��С��������ǰ��
	int m_OpenCount;                      //�����б�ǰ�Ľڵ���Ŀ  
	int m_OpenID;                         //ÿ��Ϊ���뿪���б�Ľڵ����һ��Ψһ��ID
	NodePT* m_Node;                         //����ÿ������������б�Ľڵ㣬IDΪ������ֵ
	NodeInfo** m_NodeInfo;                //��ָ�����ڱ����ͼ�ڵ����Ϣ���ýڵ������������ɵõ��ڵ���Ϣ
	int** m_Map;                          //��ͼָ��  
	int m_W, m_H;                          //��ͼ���
	TMXTiledMap* _tileMap;                       //���߲�
	std::list<PT> m_Path;              //����·������     
	Vector<ANode*>  s_Path;             //ƽ���Ż����·��

};