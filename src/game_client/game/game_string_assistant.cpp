

#include "game_string_assistant.h"


//----------------------------------------------------
//		���ƣ�CStrMap::CStrMap
//		���ܣ�
//		��ڣ�
//			
//		���ڣ�
//		��ע��2003.7.9   fox
//----------------------------------------------------
CStrMap::CStrMap()
:mStrMap(),mInt2StrMap()
{
	return;
}

//----------------------------------------------------
//		���ƣ�CStrMap::CStrMap
//		���ܣ�
//		��ڣ�
//			
//		���ڣ�
//		��ע��2003.7.9   fox
//----------------------------------------------------
CStrMap::~CStrMap()
{
	return;
}

//----------------------------------------------------
//		���ƣ�CStrMap::Str
//		���ܣ�
//		��ڣ�
//			
//		���ڣ�
//		��ע��2003.7.9   fox
//----------------------------------------------------
bool CStrMap::Str(const string& vrIndexString,const string& vrString)
{
	std::map<string,string>::iterator It;
	It=mStrMap.find(vrIndexString);
	if(It!=mStrMap.end())
	{
		return false;
	}
	mStrMap[vrIndexString]=vrString;
	return true;
}

bool CStrMap::Str(const int vIndex,const string& vrIndexString)
{
	std::map<int,string>::iterator It;
	It=mInt2StrMap.find(vIndex);
	if(It!=mInt2StrMap.end())
	{
		return false;
	}

	static string Temp;
	if(!((*this)(vrIndexString,Temp)))
	{//Ҫ�����������ַ���������
		return false;
	}

	mInt2StrMap[vIndex]=Temp;
	return true;
}





//----------------------------------------------------
//		���ƣ�CStrMap::operator[]
//		���ܣ�
//		��ڣ�
//			
//		���ڣ�
//		��ע��2003.7.9   fox
//----------------------------------------------------
string CStrMap::STR_NOT_EXIST("This Index String is not exist!");
string CStrMap::mTemp;
const string& CStrMap::operator[](const string& vrIndexString)
{
	std::map<string,string>::iterator It;
	It=mStrMap.find(vrIndexString);
	if(It==mStrMap.end())
	{
		mTemp="";
		mTemp="\""+vrIndexString+"\"<--"+STR_NOT_EXIST;
		return mTemp;
	}
	return (*It).second;
}

const string& CStrMap::operator[](const int vIndex)
{
	std::map<int,string>::iterator It;
	It=mInt2StrMap.find(vIndex);
	if(It==mInt2StrMap.end())
	{
		mTemp="";
		mTemp="index:\""+vIndex;
		mTemp+="\"<--";
		mTemp+=STR_NOT_EXIST;
		return mTemp;
	}
	return (*It).second;
}


bool CStrMap::operator()(const string& vrIndexString,string& vGetString)
{
	std::map<string,string>::iterator It;
	It=mStrMap.find(vrIndexString);
	if(It==mStrMap.end())
	{
		return false;
	}
	vGetString=(*It).second;
	return true;
}

bool CStrMap::operator()(const int vIndex,string& vGetString)
{
	std::map<int,string>::iterator It;
	It=mInt2StrMap.find(vIndex);
	if(It==mInt2StrMap.end())
	{
		return false;
	}
	vGetString=(*It).second;
	return true;
}

