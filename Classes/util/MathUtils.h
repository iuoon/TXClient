#pragma once
#include <iostream>  
#include <sstream>    //ʹ��stringstream��Ҫ�������ͷ�ļ�  
using namespace std;


class MathUtils
{
public:
	MathUtils();
	~MathUtils();

	int stringToNum(const string& str);
	string  numToString(const int a);

private:

};



////ģ�庯������string���ͱ���ת��Ϊ���õ���ֵ���ͣ��˷��������ձ������ԣ�  
//template <class Type>
//Type stringToNum(const string& str)
//{
//	istringstream iss(str);
//	Type num;
//	iss >> num;
//	return num;
//}
//
//
//template <class Type>
//Type numToString(const int a)
//{
//	stringstream strStream;
//	strStream << a;
//	string s = strStream.str();
//	return s;
//}