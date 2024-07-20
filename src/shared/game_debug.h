
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

//һ����Ƕ�Ӧ����һЩ��̬��Ҫ��

class CMessenger
{
public:


	//vLevel[BYTE]��10�ȼ�0���Ҫ,9����Ҫ//vColor [DWORD]��ʽΪ[ r g b]ÿ������8bit
	CMessenger(const string& vrTo,const string& vrFrom,const int vOutputType,BYTE vLevel ,DWORD vColor)
		:mOn(true),mLevel(vLevel),mColor(vColor),mpOutput(NULL),mFrom(vrFrom)
	{
		mpOutput=COutputFactory::GetOutput(vOutputType,vrTo);
	};

	CMessenger(const CMessenger& vrMessage)
	{
		mOn=vrMessage.mOn;						//��Ϣ����
		mLevel=vrMessage.mLevel;				//��Ϣ�ĵȼ�
		mColor=vrMessage.mColor;				//��Ϣ����ɫ
		mpOutput=vrMessage.mpOutput->Clone();	//��Ϣ�����
		mFrom=vrMessage.mFrom;					//��Ϣ����Դ
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
	bool		mOn;		//��Ϣ����
	BYTE		mLevel;		//��Ϣ�ĵȼ�
	DWORD		mColor;		//��Ϣ����ɫ
	COutput*	mpOutput;	//��Ϣ�����
	string		mFrom;		//��Ϣ����Դ

};






#endif//GAME_DEBUG

