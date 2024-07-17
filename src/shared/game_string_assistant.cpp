#include "game_string_assistant.h"


namespace fox
{

	float CLimitTime::mNowTime=0;//初始化


//----------------------------------------------------
//		名称：CStrMap::CStrMap
//		功能：
//		入口：
//			
//		出口：
//		备注：2003.7.9   fox
//----------------------------------------------------
CStrMap::CStrMap()
:mStrMap(),mInt2StrMap()
{
	return;
}

//----------------------------------------------------
//		名称：CStrMap::CStrMap
//		功能：
//		入口：
//			
//		出口：
//		备注：2003.7.9   fox
//----------------------------------------------------
CStrMap::~CStrMap()
{
	return;
}

//----------------------------------------------------
//		名称：CStrMap::Str
//		功能：
//		入口：
//			
//		出口：
//		备注：2003.7.9   fox
//----------------------------------------------------
bool CStrMap::Str(const string& vrIndexString,const string& vrString)
{
	guard;
	std::map<string,string>::iterator It;
	It=mStrMap.find(vrIndexString);
	if(It!=mStrMap.end())
	{
		return false;
	}
	mStrMap[vrIndexString]=vrString;
	return true;
	unguard;
}

bool CStrMap::Str(const int vIndex,const string& vrIndexString)
{
	guard;
	std::map<int,string>::iterator It;
	It=mInt2StrMap.find(vIndex);
	if(It!=mInt2StrMap.end())
	{
		return false;
	}

	static string Temp;
	if(!((*this)(vrIndexString,Temp)))
	{//要关联的索引字符串不存在
		return false;
	}

	mInt2StrMap[vIndex]=Temp;
	return true;
	unguard;
}





//----------------------------------------------------
//		名称：CStrMap::operator[]
//		功能：
//		入口：
//			
//		出口：
//		备注：2003.7.9   fox
//----------------------------------------------------
string CStrMap::STR_NOT_EXIST("This Index String is not exist!");
string CStrMap::mTemp;
const string& CStrMap::operator[](const string& vrIndexString)
{
	guard;
	std::map<string,string>::iterator It;
	It=mStrMap.find(vrIndexString);
	if(It==mStrMap.end())
	{
		mTemp="";
		mTemp="\""+vrIndexString+"\"<--"+STR_NOT_EXIST;
		return mTemp;
	}
	return (*It).second;
	unguard;
}

const string& CStrMap::operator[](const int vIndex)
{
	guard;
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
	unguard;
}


bool CStrMap::operator()(const string& vrIndexString,string& vGetString)
{
	guard;
	std::map<string,string>::iterator It;
	It=mStrMap.find(vrIndexString);
	if(It==mStrMap.end())
	{
		return false;
	}
	vGetString=(*It).second;
	return true;
	unguard;
}

bool CStrMap::operator()(const int vIndex,string& vGetString)
{
	guard;
	std::map<int,string>::iterator It;
	It=mInt2StrMap.find(vIndex);
	if(It==mInt2StrMap.end())
	{
		return false;
	}
	vGetString=(*It).second;
	return true;
	unguard;
}


}

//////////////////////////////////////////////////////////////////////////////

//#include "debug_interface.h"
namespace game_client
{

	CRtDebug	Log("log");
	CRtDebug	Err("err");
	CNullDebug	Debug("game_client_debug_.txt");
	//CStdDebug	Debug("game_client_debug_.txt");//CNullDebug	Debug("game_client_debug_.txt");
}


namespace region_server
{

	CRtDebug		Log("log");
	CRtDebug		Err("err");

#ifdef _DEBUG
	CStdDebug Debug("region_server_debug_.txt");
#else
	CNullDebug  Debug("region_server_debug_.txt");
#endif

}

namespace update_server
{

	//CStdDebug Log("update_server_log_.txt");
	//CStdDebug Err("update_server_err_.txt");
	CRtDebug	Log("log");
	CRtDebug	Err("err");

#ifdef _DEBUG
	CStdDebug Debug("update_server_debug_.txt");
#else
	CNullDebug  Debug("update_client_debug_.txt");
#endif
	
	
	
	//using update::CNullDebug;
	//CNullDebug Log("update_server_log_.txt");
	//CNullDebug Err("update_server_err_.txt");
	//CNullDebug Debug("update_server_debug_.txt");
	
}

namespace update_client
{

	CRtDebug	Log("log");
	CRtDebug	Err("err");


#ifdef _DEBUG
	//CStdDebug	Debug("update_client_debug_.txt");
	CNullDebug  Debug("update_client_debug_.txt");
#else
	CNullDebug  Debug("update_client_debug_.txt");
#endif
	
	
	//using update::CNullDebug;

	//CNullDebug Log("log");
	//CNullDebug Err("err");
	//CNullDebug Debug("update_client_debug.txt");

}

