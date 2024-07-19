
#ifndef GAME_STRING_ASSISTANT_H
#define GAME_STRING_ASSISTANT_H

#include <map>
#include <string>
#include <sstream>
#include <time.h>
#include <iomanip>
#include "core/rt2_core.h"
#include "rt1tort2.h"



//关于一些串行化的思考和问题：要记录的信息，严格来说，有数字或字符串本身，还有，这个数字，原来是long int 还是其它，
//也就是说如果不知道保存string中取出的顺序和该取什么，也要能够正确的取出这个流，就是流中应该包含了如何取出的信息
//还有就是，数字是采用定长保存，还是变长保存，同时，上个问题如何在变长下得到解决
//根本上说第二个问题就是压缩的问题
//目前没有完全思考清楚，先采用16进制，定长模式保存，但，仍然丢失了存取序列这样的信息
//保留了原来的，10进制，变长模式，但，这个模式也丢失了部分存取序列这样的信息
//能在不知道存取序列的模式下，正确的读出，但丢失了，原来这个数据是什么类型的
//而且，如何方便的运用<< >>这样的操作符号重载，仍然，不是很了解，
//所以，原来的模式，采用另一个名字的函数解决

//setprecision(9)	//精度
//setw(14)			//宽度
//setfill('0')		//填充元素
//setbase(8)		//进制


namespace fox
{

using std::stringstream;
using std::string;
using std::map;


//单位：秒
const float DEFAULT_LIMIT_TIME=1.0;
class CLimitTime
{
public:
	CLimitTime()
		:mLimitTime(DEFAULT_LIMIT_TIME),mLastTime(0){};

	CLimitTime(float vLimitTime)
		:mLimitTime(vLimitTime),mLastTime(0){};

	bool Limit()
	{
		guard;
		if(mNowTime-mLastTime<=mLimitTime)
		{
			return true;
		}
		else
		{
			mLastTime=mNowTime;
			return false;
		}
		unguard;
	};

	static void UpdateNowTime(float vNowTime)
	{
		guard;
		mNowTime=vNowTime;
		unguard;
	}

private:
	static float mNowTime;
	float mLimitTime;
	float mLastTime;
};



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

//////////////////////////////////////////////////////////////////////////
//添加分隔符[默认每3位加一个","]
inline void AddCompart(string& vrString,const int vCompartSize=3,char const*vCompartFlag=",")
{
	guard;
	string Temp;
	size_t i;
	size_t Size=vrString.size();
	for (i=0;i<Size;i++)
	{
		if((Size-i)%vCompartSize==0 && i!=0)
		{
			Temp+=vCompartFlag;
		}
		Temp+=vrString[i];
	}
	vrString=Temp;
	return;
	unguard;
}


//转换函数
//string到string不希望变化，[不因为分隔符号，将字符串截断]
inline string Convert(const string& vrType,int vBase=10,int vWidth=0,char vFillChar='0')
{
	guard;
	return vrType;
	unguard;
};


//如果是size_t当作是int
template<class OutType>
OutType Convert(const size_t& vrType,int vBase=10,int vWidth=0,char vFillChar='0')
{
	guard;
	return Convert<OutType,const int>((const int&)vrType,vBase,vWidth,vFillChar);
	unguard;
};


template<class OutType,class InType>
OutType Convert(const InType& vrType,int vBase=10,int vWidth=0,char vFillChar='0')
{
	guard;
	std::stringstream Stream;

	Stream<<std::setbase(vBase)<<std::setfill(vFillChar)<<std::setw(vWidth)<<vrType;//向流中传值

	OutType Result;//这里存储转换结果

	Stream>>Result;//向result中写入值

	return Result;
	unguard;
};





//time_t到string

inline string TimeConvert(const time_t& vrTime,const char vDataFlag[]="-",const char vBetweenFlag[]=" ",const char vTimeFlag[]=":")
{
	guard;
	string Temp;
	tm* pTime=NULL;
	pTime = localtime(&(vrTime));

	if(pTime==NULL)
	{
		return "";
	}

	Temp+=Convert<string>(pTime->tm_year+1900)+vDataFlag;
	Temp+=Convert<string>(pTime->tm_mon+1)+vDataFlag;
	Temp+=Convert<string>(pTime->tm_mday)+vBetweenFlag;
	Temp+=Convert<string>(pTime->tm_hour)+vTimeFlag;

	Temp+= (pTime->tm_min/10 > 0) ?  "" : "0";
	Temp+=Convert<string>( pTime->tm_min );

	return Temp;
	unguard;
}

inline string UnitConvert(const int vData,const int vUnitCount=1024,const char vUnitFlag[]=" KB")
{
	guard;
	int Temp;
	string TempString;
	Temp=(vData+vUnitCount-1)/vUnitCount;
	TempString=Convert<string>(Temp);
	AddCompart(TempString);
	TempString+=vUnitFlag;
	return TempString;
	unguard;
}


//锁定帧数的类
class CLockFrame
{
public:
	CLockFrame(DWORD vLockTime)//单位毫秒
	{
		mLastTime=rtMilliseconds();
		mLockTime=vLockTime;
	};

	~CLockFrame()
	{
		DWORD Now=rtMilliseconds();
		if(Now>=mLastTime)
		{//时间正常
			DWORD PassTime=Now-mLastTime;
			if(mLockTime>PassTime)
			{//要求锁定的时间比过去的时间大//sleep
				rtSleep((float(mLockTime-PassTime))/1000 * 1000);
			}
			else
			{//要求锁定的时间已经小于等于经过的时间//不需要再sleep
			}
		}
	};

private:
	DWORD mLastTime;
	DWORD mLockTime;
};



/*
MD5的封装
class CMD5
{
public:
	enum 
	{
		MD5_LENGTH=16,
	};

	CMD5()
		:mFinal(false)
	{
		rtMD5Init (&mContext);
		//MD5Init(&mContext);
	};


	//拷贝构造
	CMD5(CMD5& vrMD5)
		:mFinal(true)
	{
		rtMD5Init(&mContext);
		//MD5Init(&mContext);

		vrMD5.Finish();
		for(int i=0;i<MD5_LENGTH;++i)
		{
			mMD5[i]=vrMD5.mMD5[i];
		}
	};

	//通过未封装的MD5,拷贝构造
	CMD5(unsigned char vrMD5[])
		:mFinal(true)
	{
		rtMD5Init (&mContext);
		//MD5Init(&mContext);

		for(int i=0;i<MD5_LENGTH;++i)
		{
			mMD5[i]=vrMD5[i];
		}
	};

	~CMD5(){};

	//做update以外其它动作，都会将MD5计算出来，然后再Update就会新计算另一个MD5
	void Update(unsigned char* vpData, unsigned int vDataLength)
	{
		guard;
		if(mFinal)
		{//已经算好原来的那个//新算另一个
			mFinal=false;			//新算另一个
			rtMD5Init(&mContext);	//新算另一个
			//MD5Init(&mContext);
		}

		rtMD5Update(&mContext, vpData, vDataLength);
		//MD5Update(&mContext, vpData, vDataLength);
		return;
		unguard;
	};

	//将MD5输出到string
	friend string operator << (string& vrString,CMD5& vrMD5)
	{
		guard;
		vrMD5.Finish();//得到MD5

		char Temp[2+1];
		for (int i=0;i<MD5_LENGTH;++i)
		{
			sprintf (Temp, "%02x", vrMD5.mMD5[i]);
			vrString+=Temp[0];
			vrString+=Temp[1];
		}

		return vrString;
		unguard;
	};

	//得到string的MD5
	CMD5& operator << (string vrString)
	{
		guard;
		mFinal=true;//重新算
		Update( (unsigned char*) vrString.c_str(),(unsigned int )vrString.size());
		return (*this);
		unguard;
	}

	//比较两个MD5是否一样
	bool operator ==(CMD5& vrMD5)
	{
		guard;
		vrMD5.Finish();
		Finish();

		bool Equal=true;

		for(int i=0;i<MD5_LENGTH;++i)
		{
			if(mMD5[i]!=vrMD5.mMD5[i])
			{//不相等
				Equal=false;
				break;
			}
		}

		return Equal;
		unguard;
	};

	//象未封装的MD5一样访问
	unsigned char operator [] (int vIndex)
	{
		guard;
		Finish();
		return mMD5[vIndex];
		unguard;
	};

	//赋值语句
	CMD5& operator =( CMD5& vrMD5)
	{
		guard;
		mFinal=true;
		vrMD5.Finish();
		for(int i=0;i<MD5_LENGTH;++i)
		{
			mMD5[i]=vrMD5.mMD5[i];
		}
		return *this;
		unguard;
	};


	//通过未封装的MD5赋值
	CMD5& operator =(unsigned char vrMD5[])
	{
		guard;
		mFinal=true;
		for(int i=0;i<MD5_LENGTH;++i)
		{
			mMD5[i]=vrMD5[i];
		}
		return *this;
		unguard;
	};

private:

	void Finish()
	{
		guard;
		if(!mFinal)
		{//没有计算
			mFinal=true;
			rtMD5Final(mMD5, &mContext);	//算好当前的
			//MD5Final(mMD5,&mContext);	//算好当前的
		}

		return;
		unguard;
	};
	
	bool			mFinal;				//是否已经计算结束
	//RtMD5Context	mContext;			//计算MD5要用的一个结构体
	MD5_CTX		mContext;			//计算MD5要用的一个结构体
	unsigned char	mMD5[MD5_LENGTH];	//记录MD5的数组
};
*/



}



using fox::Convert;
using std::string;


//string<<string的旧版本
inline string& OldToString(string& vrString,const string& vrT)
{
	guard;
	string Size=Convert<string,unsigned int>((unsigned int)vrT.size());
	string SizeLength=Convert<string,unsigned int >((unsigned int )Size.size());
	vrString=vrString+SizeLength+Size+vrT;
	return vrString;
	unguard;
}

//string>>string的旧版本
inline string& OldFromString(string& vrString,string& vrT)
{
	guard;
	if(vrString.size()<2)
	{
		return vrString;
	}
	unsigned int SizeLength=Convert<unsigned int >(vrString.substr(0,1));
	unsigned int Size=Convert<unsigned int >(vrString.substr(1,SizeLength));
	vrT =(vrString.substr(1+SizeLength,Size));
	vrString.erase(0,1+SizeLength+Size);
	return vrString;
	unguard;
}


template<class T>
string& OldToString(string& vrString,const T& vrT)
{
	guard;
	string Temp=Convert<string>(vrT);
	string Size=Convert<string,unsigned int>((unsigned int)Temp.size());
	string SizeLength=Convert<string,unsigned int >((unsigned int )Size.size());
	vrString=vrString+SizeLength+Size+Temp;
	return vrString;
	unguard;
}

template<class T>
string& OldFromString(string& vrString,T& vrT)
{
	guard;
	if(vrString.size()<2)
	{
		return vrString;
	}
	unsigned int SizeLength=Convert<unsigned int >(vrString.substr(0,1));
	unsigned int Size=Convert<unsigned int >(vrString.substr(1,SizeLength));
	vrT =Convert<T>(vrString.substr(1+SizeLength,Size));

	vrString.erase(0,1+SizeLength+Size);
	return vrString;
	unguard;
}








//目前没有完全思考清楚，先采用16进制，定长模式保存，但，仍然丢失了存取序列这样的信息
//可以参看文件头的一些说明

//string<<string比较特殊
inline string& operator<<(string& vrString,const string& vrT)
{
	guard;
	const int SizeLength=sizeof(unsigned int) * 2;//定长，16进制
	string Size=Convert<string,unsigned int>((unsigned int)vrT.size(),16,SizeLength);

	vrString=vrString+Size+vrT;
	return vrString;
	unguard;
}


//string>>string比较特殊
inline string& operator>>(string& vrString,string& vrT)
{
	guard;
	const int SizeLength=sizeof(unsigned int) * 2;//定长，16进制
	unsigned int Size=0;


	if(vrString.size()<SizeLength)
	{
		return vrString;
	}
	
	Size=Convert<unsigned int >(vrString.substr(0,SizeLength),16);
	vrT =(vrString.substr(SizeLength,Size));

	vrString.erase(0,SizeLength+Size);
	return vrString;
	unguard;

}


template<class T>
string& operator<<(string& vrString,const T& vrT)
{
	guard;
	const int Size=sizeof(T) * 2;//定长，16进制

	string Temp;
	if(Size==2)
	{//是一个byte，不能用Convert<T>处理，它会当成char
		Temp=Convert<string,const short int>((const short int)vrT,16,Size);
	}
	else
	{
		Temp=Convert<string,T>(vrT,16,Size);
	}

	vrString=vrString+Temp;
	return vrString;
	unguard;
}


template<class T>
string& operator>>(string& vrString,T& vrT)
{
	guard;
	const int Size=sizeof(T) * 2;//定长，16进制

	if(vrString.size()<Size)
	{
		return vrString;
	}

	if(Size==2)
	{//是一个byte，不能用Convert<T>处理，它会当成char
		vrT =( T) Convert< short int>(vrString.substr(0,Size),16);
	}
	else
	{
		vrT =Convert<T>(vrString.substr(0,Size),16);
	}

	vrString.erase(0,Size);
	return vrString;
	unguard;
}




inline string& operator >> (string& vrString,unsigned int & vrInt)
{
	guard;
	const int Size=sizeof(unsigned int) * 2;//定长，16进制

	if(vrString.size()<Size)
	{
		return vrString;
	}

	vrInt =Convert<unsigned int>(vrString.substr(0,Size),16);
	vrString.erase(0,Size);
	return vrString;
	unguard;
}



//加密 encrypt 
inline void GSAssistantEncrypt(char* vpData,int vDataSize,string& vrSeed)
{
	guard;
	for(int i=0,j=0;i<vDataSize;i++,j++)
	{
		if(j==vrSeed.size()) {j=0;};
		vpData[i]^=vrSeed[j];
	}
	return ;
	unguard;
}

//解密 decrypt
inline void Decrypt(char* vpData,int vDataSize,string& vrSeed)
{
	guard;
	for(int i=0,j=0;i<vDataSize;i++,j++)
	{
		if(j==vrSeed.size()) {j=0;};
		vpData[i]^=vrSeed[j];
	}
	return ;
	unguard;
}






////////////////////////////////////////////////////////////////////////////////////////////




#ifndef DEBUG_INTERFACE_H
#define DEBUG_INTERFACE_H
#include <string>
#include <iostream>
#include <fstream>
#include "game_string_assistant.h"
#include "core/rt2_core.h"




using std::string;



class CDebugInterface
{
public:
	CDebugInterface(){};
	virtual ~CDebugInterface(){};

	void SetFile(string vFileName){};

	CDebugInterface& operator <<(const string& vrMessage);
	CDebugInterface& operator <<(const char*  vpMessage);

	template<class T>
		CDebugInterface& operator <<(const T& vrT)
	{
		guard;
		string Temp=Convert<string>(vrT);
		Print(Temp);
		return *this;
		unguard;
	};
private:
	//接口
	virtual void Print(const string& vrString)=0;
	virtual void Print(const char* vpString)=0;
	//接口
};

inline CDebugInterface& CDebugInterface::operator <<(const string& vrMessage)
{
	guard;
	Print(vrMessage);
	return *this;
	unguard;
}

inline CDebugInterface& CDebugInterface::operator <<(const char*  vpMessage)
{
	guard;
	Print(vpMessage);
	return *this;
	unguard;
}









class CRtDebug:public CDebugInterface
{
public:
	CRtDebug(string vType):mType(vType){};
	~CRtDebug(){}
	virtual void Print (const string& vrString)
	{
		guard;
		if(mType=="log")
		{
			RtCoreLog().Info(vrString.c_str());
		}
		else if(mType=="err")
		{
			RtCoreLog().Info(vrString.c_str());
		}
		else
		{
			RtCoreLog().Info(vrString.c_str());
		}
		return;
		unguard;
	}
	virtual void Print(const char* vpString)
	{
		guard;
		if(mType=="log")
		{
			RtCoreLog().Info(vpString);
		}
		else if(mType=="err")
		{
			RtCoreLog().Info(vpString);
		}
		return;
		unguard;
	}
private:
	string mType;
};

class CStdDebug:public CDebugInterface
{
public:
	CStdDebug(const char vFileName[]):mOutputFileName(vFileName),mpOutputFile(NULL){};
	~CStdDebug()
	{
		if(mpOutputFile!=NULL)
		{
			mpOutputFile->close();
			delete mpOutputFile;
		}
	}

private:
	virtual void Print (const string& vrString)
	{
		guard;
		if(mpOutputFile==NULL){mpOutputFile=RT_NEW std::ofstream(mOutputFileName.c_str());}
		if(mpOutputFile==NULL){return;};

		std::cout<<vrString;
		(*mpOutputFile)<<vrString;
		return;
		unguard;
	}
	virtual void Print(const char* vpString)
	{
		guard;
		if(mpOutputFile==NULL){mpOutputFile=RT_NEW std::ofstream(mOutputFileName.c_str());}
		if(mpOutputFile==NULL){return;};

		std::cout<<vpString;
		(*mpOutputFile)<<vpString;
		return;
		unguard;
	}
private:
	string		mOutputFileName;
	std::ofstream*	mpOutputFile;
};


class CNullDebug:public CDebugInterface
{
public:
	CNullDebug(const char vFileName[]){};
	virtual ~CNullDebug(){};
private:
	virtual void Print(const string& vrString){};
	virtual void Print(const char* vpString){};
};



namespace region_server
{

	extern CRtDebug		Log;
	extern CRtDebug		Err;

#ifdef _DEBUG
	extern CStdDebug Debug;
#else
	
	extern CNullDebug Debug;
#endif

}

namespace game_client
{

	extern CRtDebug		Log;
	extern CRtDebug		Err;
	extern CNullDebug	Debug;
	//extern CRtDebug		Debug;
}


namespace update_server
{

	extern CRtDebug Log;
	extern CRtDebug Err;

#ifdef _DEBUG
	extern CStdDebug Debug;
#else
	extern CNullDebug Debug;
#endif

	
//	using update::CNullDebug;
//	extern CNullDebug Log;
//	extern CNullDebug Err;
//	extern CNullDebug Debug;
}

namespace update_client
{
	extern CRtDebug		Log;
	extern CRtDebug		Err;

#ifdef _DEBUG
	extern CNullDebug Debug;//extern CStdDebug Debug;
#else
	extern CNullDebug Debug;
#endif


}













#endif//DEBUG_INTERFACE_H


#endif//GAME_STRING_ASSISTANT_H


/*
//同inline string& operator<<(string& vrString,const string& vrT)
inline string& ToString(string& vrString,const string& vrT)
{
guard;
string Size=Convert<string,unsigned int>((unsigned int)vrT.size());
string SizeLength=Convert<string,unsigned int >((unsigned int )Size.size());
vrString=vrString+SizeLength+Size+vrT;
return vrString;
unguard;
}


//inline string& operator>>(string& vrString,string& vrT)
inline string& FromString(string& vrString, string& vrT)
{
guard;
if(vrString.size()<2)
{
return vrString;
}
unsigned int SizeLength=Convert<unsigned int >(vrString.substr(0,1));
unsigned int Size=Convert<unsigned int >(vrString.substr(1,SizeLength));
vrT =(vrString.substr(1+SizeLength,Size));

vrString.erase(0,1+SizeLength+Size);
return vrString;
unguard;
}
*/

