#include <math.h>  
#include "Astar.h"  


void Astar::InitAstar(std::vector<bool> &_maze, int mapwidth, int mapheight)
{
	maze = _maze;
	width = mapwidth;
	height = mapheight;
}

int Astar::calcG(APoint *temp_start, APoint *point)
{
	int extraG = (abs(point->x - temp_start->x) + abs(point->y - temp_start->y)) == 1 ? kCost1 : kCost2;
	int parentG = point->parent == NULL ? 0 : point->parent->G; //����ǳ�ʼ�ڵ㣬���丸�ڵ��ǿ�  
	return parentG + extraG;
}

int Astar::calcH(APoint *point, APoint *end)
{
	//�ü򵥵�ŷ����þ������H�����H�ļ����ǹؼ������кܶ��㷨��û�����о�^_^  
	return sqrt((double)(end->x - point->x)*(double)(end->x - point->x) + (double)(end->y - point->y)*(double)(end->y - point->y))*kCost1;
}

int Astar::calcF(APoint *point)
{
	return point->G + point->H;
}

APoint *Astar::getLeastFpoint()
{
	if (!openList.empty())
	{
		auto resPoint = openList.front();
		for (auto &point : openList)
		if (point->F<resPoint->F)
			resPoint = point;
		return resPoint;
	}
	return NULL;
}

APoint *Astar::findPath(APoint &startPoint, APoint &endPoint, bool isIgnoreCorner)
{
	openList.clear();
	closeList.clear();
	//CCLOG("%d,%d", startPoint.x,startPoint.y);
	//CCLOG("%d,%d", endPoint.x,endPoint.y);
	openList.push_back(new APoint(startPoint.x, startPoint.y)); //�������,��������һ���ڵ㣬�������  
	while (!openList.empty())
	{
		auto curPoint = getLeastFpoint(); //�ҵ�Fֵ��С�ĵ�  
		openList.remove(curPoint); //�ӿ����б���ɾ��  
		closeList.push_back(curPoint); //�ŵ��ر��б�  
		//1,�ҵ���ǰ��Χ�˸����п���ͨ���ĸ���  
		auto surroundPoints = getSurroundPoints(curPoint, isIgnoreCorner);
		for (auto &target : surroundPoints)
		{
			//2,��ĳһ�����ӣ���������ڿ����б��У����뵽�����б����õ�ǰ��Ϊ�丸�ڵ㣬����F G H  
			if (!isInList(openList, target))
			{
				target->parent = curPoint;

				target->G = calcG(curPoint, target);
				target->H = calcH(target, &endPoint);
				target->F = calcF(target);

				openList.push_back(target);
			}
			//3����ĳһ�����ӣ����ڿ����б��У�����Gֵ, �����ԭ���Ĵ�, ��ʲô������, �����������ĸ��ڵ�Ϊ��ǰ��,������G��F  
			else
			{
				int tempG = calcG(curPoint, target);
				if (tempG<target->G)
				{
					target->parent = curPoint;

					target->G = tempG;
					target->F = calcF(target);
					//CCLOG("%d,%d", target.x, target.y);
				}
			}
			APoint *resPoint = isInList(openList, &endPoint);
			if (resPoint)
				return resPoint; //�����б���Ľڵ�ָ�룬��Ҫ��ԭ�������endpointָ�룬��Ϊ���������  
		}
	}

	return NULL;
}

std::vector<APoint *> Astar::GetPath(APoint &startPoint, APoint &endPoint, bool isIgnoreCorner)
{
	APoint *result = findPath(startPoint, endPoint, isIgnoreCorner);
	std::vector<APoint *> path;
	//����·�������û�ҵ�·�������ؿ�����  
	while (result)
	{
		result->y = height - result->y - 1;
		path.insert(path.begin(),result);
		result = result->parent;
	}
	
	//CCLOG("%d", path.size());
	return path;
}

APoint *Astar::isInList(const std::list<APoint *> &list, const APoint *point) const
{
	//�ж�ĳ���ڵ��Ƿ����б��У����ﲻ�ܱȽ�ָ�룬��Ϊÿ�μ����б����¿��ٵĽڵ㣬ֻ�ܱȽ�����  
	for (auto p : list)
	if (p->x == point->x&&p->y == point->y)
		return p;
	return NULL;
}

bool Astar::isCanreach(const APoint *point, const APoint *target, bool isIgnoreCorner) const
{
	if (target->x<0 || target->x>width-1
		|| target->y<0 || target->y>height-1
		|| maze[width*target->y + target->x] == false
		|| (target->x == point->x&&target->y == point->y)
		|| isInList(closeList, target)) //������뵱ǰ�ڵ��غϡ�������ͼ�����ϰ�������ڹر��б��У�����false  
		return false;
	else
	{
		if (abs(point->x - target->x) + abs(point->y - target->y) == 1) //��б�ǿ���  
			return true;
		else
		{
			//б�Խ�Ҫ�ж��Ƿ��ס  
			if (maze[width*target->y + point->x] == true && maze[width*point->y + target->x] == true)
				return true;
			else
				return isIgnoreCorner;
		}
	}
}

std::vector<APoint *> Astar::getSurroundPoints(const APoint *point, bool isIgnoreCorner) const
{
	std::vector<APoint *> surroundPoints;

	for (int x = point->x - 1; x <= point->x + 1; x++)
	for (int y = point->y - 1; y <= point->y + 1; y++)
	if (isCanreach(point, new APoint(x, y), isIgnoreCorner))
		surroundPoints.push_back(new APoint(x, y));

	return surroundPoints;
}