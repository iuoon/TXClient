#pragma once  
/*
//A*�㷨������
*/
#include <vector>  
#include <list>  
using namespace std;

const int kCost1 = 10; //ֱ��һ������  
const int kCost2 = 14; //б��һ������  

struct APoint
{
	int x, y; //�����꣬����Ϊ�˷��㰴��C++�����������㣬x������ţ�y��������  
	int F, G, H; //F=G+H  
	APoint *parent; //parent�����꣬����û����ָ�룬�Ӷ��򻯴���  
	APoint(int _x, int _y) :x(_x), y(_y), F(0), G(0), H(0), parent(NULL)  //������ʼ��  
	{
	}
};


class Astar
{
public:
	void InitAstar(std::vector<bool> &_maze,int mapwidth, int mapheight);
	std::vector<APoint *> GetPath(APoint &startPoint, APoint &endPoint, bool isIgnoreCorner);

private:
	APoint *findPath(APoint &startPoint, APoint &endPoint, bool isIgnoreCorner);
	std::vector<APoint *> getSurroundPoints(const APoint *point, bool isIgnoreCorner) const;
	bool isCanreach(const APoint *point, const APoint *target, bool isIgnoreCorner) const; //�ж�ĳ���Ƿ����������һ���ж�  
	APoint *isInList(const std::list<APoint *> &list, const APoint *point) const; //�жϿ���/�ر��б����Ƿ����ĳ��  
	APoint *getLeastFpoint(); //�ӿ����б��з���Fֵ��С�Ľڵ�  
	//����FGHֵ  
	int calcG(APoint *temp_start, APoint *point);
	int calcH(APoint *point, APoint *end);
	int calcF(APoint *point);
private:
	std::vector<bool> maze;
	std::list<APoint *> openList;  //�����б�  
	std::list<APoint *> closeList; //�ر��б�  
	int width;
	int height;
};