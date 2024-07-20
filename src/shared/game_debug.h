
#ifndef GAME_DEBUG
#define GAME_DEBUG

#include <string>
#include "game_debug_output.h"

using std::string;

class CMessengerEnd
{
};

extern CMessengerEnd end;
extern CMessengerEnd End;

//一个外壳对应处理一些动态的要求

class CMessenger
{
public:


	//vLevel[BYTE]分10等级0最次要,9最重要//vColor [DWORD]格式为[ r g b]每个分量8bit
	CMessenger(const string& vrTo,const string& vrFrom,const int vOutputType,BYTE vLevel ,DWORD vColor)
		:mOn(true),mLevel(vLevel),mColor(vColor),mpOutput(NULL),mFrom(vrFrom)
	{
		mpOutput=COutputFactory::GetOutput(vOutputType,vrTo);
	};

	CMessenger(const CMessenger& vrMessage)
	{
		mOn=vrMessage.mOn;						//信息开关
		mLevel=vrMessage.mLevel;				//信息的等级
		mColor=vrMessage.mColor;				//信息的颜色
		mpOutput=vrMessage.mpOutput->Clone();	//信息的输出
		mFrom=vrMessage.mFrom;					//信息的来源
	}

	CMessenger& operator = (const CMessenger& vrMessage)
	{
		if(this==(&vrMessage))
		{
			return *this;
		}

		mOn=vrMessage.mOn;
		mLevel=vrMessage.mLevel;
		mColor=vrMessage.mColor;
		DEL_ONE(mpOutput);
		mpOutput=vrMessage.mpOutput->Clone();
		mFrom=vrMessage.mFrom;
	}
	
	virtual ~CMessenger()
	{
		DEL_ONE(mpOutput);
		mpOutput=NULL;
	};

	virtual CMessenger* Clone()=0;


	void TurnOn(){mOn=true;};
	void TurnOff(){mOn=false;};
	void SetFrom(const string& vrFrom)
	{
		mFrom=vrFrom;
	};


	CMessenger& operator << (const CMessengerEnd& vEnd)
	{
		if(mOn)
		{
			mpOutput->Print("",mFrom,mLevel,mColor);
		}
		return *this;
	}

	CMessenger& operator << (const string& vrMessage)
	{
		if(mOn)
		{
			mpOutput->Print(vrMessage,"",mLevel,mColor);
		}
		return *this;
	}

	CMessenger& operator << (const int vInt)
	{
		char Temp[10];
		itoa(vInt,Temp,10);
		return operator << (string(Temp));
	}


	CMessenger& operator << (const char* vpString)
	{
		return operator << (string(vpString));
	}

	CMessenger& operator << (const void* vPointer)
	{
		//string TempString="0x";
		int TempInt=0;
		memcpy(&TempInt,&vPointer,sizeof(void*));
		char Temp[10];
		itoa(TempInt,Temp,16);
		//TempString+=TempInt;
		return (operator << (string("0x"))).operator << (string(Temp));
	}

	
private:
	bool		mOn;		//信息开关
	BYTE		mLevel;		//信息的等级
	DWORD		mColor;		//信息的颜色
	COutput*	mpOutput;	//信息的输出
	string		mFrom;		//信息的来源

};






#endif//GAME_DEBUG

