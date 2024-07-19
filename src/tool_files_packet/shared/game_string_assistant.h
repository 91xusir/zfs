
#ifndef GAME_STRING_ASSISTANT_H
#define GAME_STRING_ASSISTANT_H

#include <map>
#include <string>
#include <sstream>
#include <time.h>
#include <iomanip>
#include "core/rt2_core.h"
#include "rt1tort2.h"



//����һЩ���л���˼�������⣺Ҫ��¼����Ϣ���ϸ���˵�������ֻ��ַ����������У�������֣�ԭ����long int ����������
//Ҳ����˵�����֪������string��ȡ����˳��͸�ȡʲô��ҲҪ�ܹ���ȷ��ȡ�����������������Ӧ�ð��������ȡ������Ϣ
//���о��ǣ������ǲ��ö������棬���Ǳ䳤���棬ͬʱ���ϸ���������ڱ䳤�µõ����
//������˵�ڶ����������ѹ��������
//Ŀǰû����ȫ˼��������Ȳ���16���ƣ�����ģʽ���棬������Ȼ��ʧ�˴�ȡ������������Ϣ
//������ԭ���ģ�10���ƣ��䳤ģʽ���������ģʽҲ��ʧ�˲��ִ�ȡ������������Ϣ
//���ڲ�֪����ȡ���е�ģʽ�£���ȷ�Ķ���������ʧ�ˣ�ԭ�����������ʲô���͵�
//���ң���η��������<< >>�����Ĳ����������أ���Ȼ�����Ǻ��˽⣬
//���ԣ�ԭ����ģʽ��������һ�����ֵĺ������

//setprecision(9)	//����
//setw(14)			//���
//setfill('0')		//���Ԫ��
//setbase(8)		//����


namespace fox
{

using std::stringstream;
using std::string;
using std::map;


//��λ����
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
//��ӷָ���[Ĭ��ÿ3λ��һ��","]
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


//ת������
//string��string��ϣ���仯��[����Ϊ�ָ����ţ����ַ����ض�]
inline string Convert(const string& vrType,int vBase=10,int vWidth=0,char vFillChar='0')
{
	guard;
	return vrType;
	unguard;
};


//�����size_t������int
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

	Stream<<std::setbase(vBase)<<std::setfill(vFillChar)<<std::setw(vWidth)<<vrType;//�����д�ֵ

	OutType Result;//����洢ת�����

	Stream>>Result;//��result��д��ֵ

	return Result;
	unguard;
};





//time_t��string

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


//����֡������
class CLockFrame
{
public:
	CLockFrame(DWORD vLockTime)//��λ����
	{
		mLastTime=rtMilliseconds();
		mLockTime=vLockTime;
	};

	~CLockFrame()
	{
		DWORD Now=rtMilliseconds();
		if(Now>=mLastTime)
		{//ʱ������
			DWORD PassTime=Now-mLastTime;
			if(mLockTime>PassTime)
			{//Ҫ��������ʱ��ȹ�ȥ��ʱ���//sleep
				rtSleep((float(mLockTime-PassTime))/1000 * 1000);
			}
			else
			{//Ҫ��������ʱ���Ѿ�С�ڵ��ھ�����ʱ��//����Ҫ��sleep
			}
		}
	};

private:
	DWORD mLastTime;
	DWORD mLockTime;
};



/*
MD5�ķ�װ
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


	//��������
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

	//ͨ��δ��װ��MD5,��������
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

	//��update�����������������ὫMD5���������Ȼ����Update�ͻ��¼�����һ��MD5
	void Update(unsigned char* vpData, unsigned int vDataLength)
	{
		guard;
		if(mFinal)
		{//�Ѿ����ԭ�����Ǹ�//������һ��
			mFinal=false;			//������һ��
			rtMD5Init(&mContext);	//������һ��
			//MD5Init(&mContext);
		}

		rtMD5Update(&mContext, vpData, vDataLength);
		//MD5Update(&mContext, vpData, vDataLength);
		return;
		unguard;
	};

	//��MD5�����string
	friend string operator << (string& vrString,CMD5& vrMD5)
	{
		guard;
		vrMD5.Finish();//�õ�MD5

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

	//�õ�string��MD5
	CMD5& operator << (string vrString)
	{
		guard;
		mFinal=true;//������
		Update( (unsigned char*) vrString.c_str(),(unsigned int )vrString.size());
		return (*this);
		unguard;
	}

	//�Ƚ�����MD5�Ƿ�һ��
	bool operator ==(CMD5& vrMD5)
	{
		guard;
		vrMD5.Finish();
		Finish();

		bool Equal=true;

		for(int i=0;i<MD5_LENGTH;++i)
		{
			if(mMD5[i]!=vrMD5.mMD5[i])
			{//�����
				Equal=false;
				break;
			}
		}

		return Equal;
		unguard;
	};

	//��δ��װ��MD5һ������
	unsigned char operator [] (int vIndex)
	{
		guard;
		Finish();
		return mMD5[vIndex];
		unguard;
	};

	//��ֵ���
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


	//ͨ��δ��װ��MD5��ֵ
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
		{//û�м���
			mFinal=true;
			rtMD5Final(mMD5, &mContext);	//��õ�ǰ��
			//MD5Final(mMD5,&mContext);	//��õ�ǰ��
		}

		return;
		unguard;
	};
	
	bool			mFinal;				//�Ƿ��Ѿ��������
	//RtMD5Context	mContext;			//����MD5Ҫ�õ�һ���ṹ��
	MD5_CTX		mContext;			//����MD5Ҫ�õ�һ���ṹ��
	unsigned char	mMD5[MD5_LENGTH];	//��¼MD5������
};
*/



}



using fox::Convert;
using std::string;


//string<<string�ľɰ汾
inline string& OldToString(string& vrString,const string& vrT)
{
	guard;
	string Size=Convert<string,unsigned int>((unsigned int)vrT.size());
	string SizeLength=Convert<string,unsigned int >((unsigned int )Size.size());
	vrString=vrString+SizeLength+Size+vrT;
	return vrString;
	unguard;
}

//string>>string�ľɰ汾
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








//Ŀǰû����ȫ˼��������Ȳ���16���ƣ�����ģʽ���棬������Ȼ��ʧ�˴�ȡ������������Ϣ
//���Բο��ļ�ͷ��һЩ˵��

//string<<string�Ƚ�����
inline string& operator<<(string& vrString,const string& vrT)
{
	guard;
	const int SizeLength=sizeof(unsigned int) * 2;//������16����
	string Size=Convert<string,unsigned int>((unsigned int)vrT.size(),16,SizeLength);

	vrString=vrString+Size+vrT;
	return vrString;
	unguard;
}


//string>>string�Ƚ�����
inline string& operator>>(string& vrString,string& vrT)
{
	guard;
	const int SizeLength=sizeof(unsigned int) * 2;//������16����
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
	const int Size=sizeof(T) * 2;//������16����

	string Temp;
	if(Size==2)
	{//��һ��byte��������Convert<T>�������ᵱ��char
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
	const int Size=sizeof(T) * 2;//������16����

	if(vrString.size()<Size)
	{
		return vrString;
	}

	if(Size==2)
	{//��һ��byte��������Convert<T>�������ᵱ��char
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
	const int Size=sizeof(unsigned int) * 2;//������16����

	if(vrString.size()<Size)
	{
		return vrString;
	}

	vrInt =Convert<unsigned int>(vrString.substr(0,Size),16);
	vrString.erase(0,Size);
	return vrString;
	unguard;
}



//���� encrypt 
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

//���� decrypt
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
	//�ӿ�
	virtual void Print(const string& vrString)=0;
	virtual void Print(const char* vpString)=0;
	//�ӿ�
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
//ͬinline string& operator<<(string& vrString,const string& vrT)
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

