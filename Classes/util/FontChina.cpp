
#include "util/FontChina.h"


string FontChina::getCurrentTime()  
{  
	time_t t;  
	time(&t);
	char tmp[64];  
	strftime(tmp,sizeof(tmp),"%Y-%m-%d %X",localtime((&t)));  
	string timeStr=tmp;  
	return timeStr;  
}


//在Win32平台下，将GBK编码转换为UTF-8
string FontChina::gbk_2_utf8(const string text)
{
#if (CC_TARGET_PLATFORM == CC_PLATFORM_WIN32)
	//采用Lambda表达式,将string转换成wstring
	wstring tes = [=]() {
		setlocale(LC_ALL, "chs"); 
		const char* _Source = text.c_str();
		size_t _Dsize = text.size() + 1;
		wchar_t *_Dest = new wchar_t[_Dsize];
		wmemset(_Dest, 0, _Dsize);
		mbstowcs(_Dest,_Source,_Dsize);
		std::wstring result = _Dest;
		delete []_Dest;
		setlocale(LC_ALL, "C");
		return result;
	}();

	int asciSize = WideCharToMultiByte(CP_UTF8,0,tes.c_str(),tes.size(),NULL,0,NULL,NULL);
	if (asciSize == ERROR_NO_UNICODE_TRANSLATION || asciSize == 0)	
	{
		return string();
	}

	char *resultString = new char[asciSize];
	int conveResult = WideCharToMultiByte(CP_UTF8,0,tes.c_str(),tes.size(),resultString,asciSize,NULL,NULL);
	if (conveResult != asciSize)
	{
		return string();
	}
	string buffer = "";
	buffer.append(resultString,asciSize);

	delete[] resultString;
	return buffer;

#else
	return text;
#endif

}


//根据平台不同进行转换UTF-8
const string FontChina::getStringUTF8(const string text)
{
	__String* ns = __String::create(text);
	const char* cns;
#if (CC_TARGET_PLATFORM == CC_PLATFORM_WIN32)
	std::string sns =  FontChina::gbk_2_utf8(ns->getCString());
	cns = sns.c_str();
#else
	cns = ns->getCString();
#endif
	return cns;
}