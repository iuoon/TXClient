#ifndef  _MY_UTILITY_H_
#define  _MY_UTILITY_H_

#include "cocos2d.h"

using namespace std;
USING_NS_CC;

class FontChina 
{
public:
	static string getCurrentTime();

	static string gbk_2_utf8(const string text);

	static const string getStringUTF8(const string text);
};

#endif // _MY_UTILITY_H_
