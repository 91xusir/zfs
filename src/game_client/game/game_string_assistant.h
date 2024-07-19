
#ifndef GAME_STRING_ASSISTANT_H
#define GAME_STRING_ASSISTANT_H

#include <map>
#include <string>

using std::string;
using std::map;

class CStrMap
{
public:
	CStrMap();
	~CStrMap();

	bool Str(const string& vrIndexString,const string& vrString);
	bool Str(const int vIndex,const string& vrIndexString);

	const string& operator[](const string& vrIndexString);
	const string& operator[](const int vIndex);
	bool operator()(const string& vrIndexString,string& vGetString);
	bool operator()(const int vIndex,string& vGetString);
private:
	static string STR_NOT_EXIST;
	static string mTemp;
	map<string,string>	mStrMap;
	map<int,string>		mInt2StrMap;
};

#endif//GAME_STRING_ASSISTANT_H
