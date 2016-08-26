#include "util/MathUtils.h"

MathUtils::MathUtils()
{
}

MathUtils::~MathUtils()
{
}

int MathUtils::stringToNum(const string & str)
{
	return 0;
}

string MathUtils::numToString(const int a)
{
	stringstream strStream;
	strStream << a;
	string s = strStream.str();
	return s;
}
