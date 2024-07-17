//===========================================================
//	�ļ����ƣ�	game_debug.cpp
//	��    �ߣ�  ����
//	ʱ    �䣺	2001.12.24
//	��    �ݣ� 	��Ϸ������Դ����
//	������ʷ��
//				2003.12.12		����һ����ʾ��Ϣ��BUG
//				2003.10.30		���ӿڲ���������һ��
//				2003.9.19		���һЩ����
//				2003.5.28		����һЩ���ֵ�BUG
//				2003.2.27		�޸�ĳЩ����
//				2003.2.22
//===========================================================z
#include "game_debug.h" 

#ifndef GRAPH_PRINT//ͼ������ϵͳ
#define TEXT_PRINT//��������ϵͳ
#endif//GRAPH_PRINT//ͼ������ϵͳ



//<ͬϵͳ��ش���>
#ifdef  LINUX_SYSTEM//linuxϵͳ
inline DWORD GET_TICK_COUNT(){return 0;};//���ʱ��

//�����Ϣ����Ļ
inline void PRINT_TO_SCREEN(int vX,int vY,char* vpMessage,DWORD vColor)
{
#ifdef TEXT_PRINT//��ͼ������ϵͳ
	printf(vpMessage);
#endif
};
#else//winϵͳ


#include "windows.h"

#ifdef MEMORY_USED_STATUS//����ڴ���Ϣ
#include "psapi.h"
#pragma comment(lib,"psapi.lib")
#endif//MEMORY_USED_STATUS

#ifdef  GRAPH_PRINT//ͼ������ϵͳ
#include "game_app.h"
#endif//GRAPH_PRINT


inline DWORD GET_TICK_COUNT(){return GetTickCount();};//���ʱ��
//�����Ϣ����Ļ
inline void PRINT_TO_SCREEN(int vX,int vY,const char* vpMessage,DWORD vColor)
{
#ifdef GRAPH_PRINT//ͼ������ϵͳ
	int R;
	int G;
	int B;

	R=(vColor&0x00ff0000)>>16;
	G=(vColor&0x0000ff00)>>8;
	B=(vColor&0x000000ff);
	//<ͬ��ͼϵͳ���>
	g_pApp->mDFWorld.MessageOnScreen(vX,vY,(char*)vpMessage,R,G,B);
	//</ͬ��ͼϵͳ���>
#endif

#ifdef TEXT_PRINT//��ͼ������ϵͳ
	printf(vpMessage);
#endif
};
#endif//LINUX_SYSTEM
//</ͬϵͳ��ش���>






namespace fox
{
	static char TempString[STRING_LENGTH];	//��ʱ�ַ���

	DebugClass Debug;	//������
	DebugClass Err;		//������
	//----------------------------------------------------
	//		���ƣ�DebugClass::DebugClass
	//		���ܣ����캯��
	//		��ڣ���
	//		���ڣ���
	//		��ע��2001.12.24   fox
	//			  2002.1.21	   fox �޸�
	//----------------------------------------------------
	long DebugClass::MsgIndex=0;						//��ǰ��Ϣ������
	char DebugClass::MsgTxt[MAX_LINES_IN_SCREEN][STRING_LENGTH]={{0}};
	DWORD DebugClass::MsgTime[MAX_LINES_IN_SCREEN]={0};	//�����Ϣ��ʱ��
	DWORD DebugClass::MsgColor[MAX_LINES_IN_SCREEN]={0};//�������Ϣ����ɫ
	bool DebugClass::OutputScreen=false;				//�Ƿ�������һ��ʵ��Ҫ���������Ļ
	long DebugClass::Fps=0;								//һ������ͳ������֡��
	int  DebugClass::mScreenWidth=DEFAULT_SCREEN_WIDTH;			//��Ļ�Ŀ�
	int  DebugClass::mScreenHeight=DEFAULT_SCREEN_HEIGHT;		//��Ļ�ĸ�


	DebugClass::DebugClass()
	{
		this->OutputFileIsSet=false;//����ļ�δ����
		this->ScreenOutputActive=false;
		this->FileOutputActive=false;
		this->FileIsOpen =false;
		this->FileName[0]=0; //��ʼ���ļ���Ϊ��
		this->fp=NULL;
		this->IsActive=false;
		this->Color=0xffffff;


	}

	//----------------------------------------------------
	//		���ƣ�DebugClass::~DebugClass
	//		���ܣ���������
	//		��ڣ���
	//		���ڣ���
	//		��ע��2001.12.24   fox
	//----------------------------------------------------
	DebugClass::~DebugClass()
	{
		if(this->FileIsOpen=true && this->fp!=NULL)
		{
			fclose(fp);
			this->OutputFileIsSet=false;
			this->ScreenOutputActive=false;
			this->FileOutputActive=false;
			this->FileIsOpen =false;
			this->FileName[0]=0; //��ʼ���ļ���Ϊ��
			this->fp=NULL;
			this->IsActive=false;
		}
	}

	//----------------------------------------------------
	//		���ƣ�DebugClass::UsedMemory
	//		���ܣ�����Ѿ�ʹ�õ��ڴ�
	//		��ڣ���
	//		���ڣ�bool ���ɹ���ʧ�ܣ�
	//		��ע��2002.6.3   fox
	//----------------------------------------------------
	bool DebugClass::UsedMemory()
	{


#ifdef MEMORY_USED_STATUS
		if(!DebugClass::OutputScreen)
		{
			return false;
		}
		//DEBUGģʽ
		static HANDLE hProcess = GetCurrentProcess();
		static PROCESS_MEMORY_COUNTERS pmc;
		static long mem=0;
		if ( GetProcessMemoryInfo( hProcess, &pmc, sizeof(pmc)) )
		{
			mem = pmc.WorkingSetSize>>10;
		}
		long mem0=mem/1000;				//�õ�����M����������
		long mem1=mem%1000;				//�õ�С������

		sprintf(TempString,"Used:%d.%d M",mem0,mem1);
#ifdef  GRAPH_PRINT
		PRINT_TO_SCREEN(mScreenWidth*2/3,LINE_HEIGHT,TempString,0xffffff);//��2/3������ʾ
#endif//GRAPH_PRINT
		return true;

#else
		return false;

#endif//MEMORY_USED_STATUS
	}

	//----------------------------------------------------
	//		���ƣ�DebugClass::SetOutputFile
	//		���ܣ�����Ҫ������ļ���
	//		��ڣ�Ҫ�������ļ���(������1024��������β��)
	//		���ڣ�bool ���ɹ���ʧ�ܣ�
	//		��ע��2002.2.11   fox
	//----------------------------------------------------
	bool DebugClass::SetOutputFile(const char vOutputFileName[])
	{
		if(vOutputFileName==NULL)
		{//����
			return false;
		}

		sprintf(this->FileName,"%s",vOutputFileName);
		this->OutputFileIsSet=true;//����ļ����Ѿ�������
		return true;
	}


	//----------------------------------------------------
	//		���ƣ�DebugClass::Init
	//		���ܣ���ʼ����������Ӧ�������йغ�������֮ǰ��
	//               ���øú�����
	//		��ڣ��������Ļ����Ϣ����ɫ��24Bits��,
	//				����������ļ�,�����������Ļ
	//		���ڣ�bool ���ɹ���ʧ�ܣ�
	//		��ע��2002.2.11   fox
	//----------------------------------------------------
	bool DebugClass::Init(DWORD vInfoColor,const bool vOutputFile,const bool vOutputScreen)
	{
		this->ScreenOutputActive=vOutputScreen;
		this->FileOutputActive=vOutputFile;
		this->IsActive=true;
		if(ScreenOutputActive)
		{
			DebugClass::OutputScreen=true;//������һ��Ҫ���������Ļ
		}
		this->Color=vInfoColor;
		return true;
	}

	//----------------------------------------------------
	//		���ƣ�DebugClass::On
	//		���ܣ��򿪵�����
	//		��ڣ���
	//		���ڣ�bool ���ɹ���ʧ�ܣ�
	//		��ע��2003.2.15   fox
	//----------------------------------------------------
	bool DebugClass::On()//�Ƿ���Ҫ������Ϣ
	{
		this->IsActive=true;
		return true;
	}

	//----------------------------------------------------
	//		���ƣ�DebugClass::Off
	//		���ܣ��رյ�����
	//		��ڣ���
	//		���ڣ�bool ���ɹ���ʧ�ܣ�
	//		��ע��2003.2.15   fox
	//----------------------------------------------------
	bool DebugClass::Off()
	{
		this->IsActive=false;
		return true;
	}
	//----------------------------------------------------
	//		���ƣ�DebugClass::Msg
	//		���ܣ�DebugClass�������Ϣ���������������
	//		��ڣ��ַ���ָ�루Ҫ�������Ϣ��,
	//				����ֵ��
	//				�ַ���ָ�루����Ϣ��ʲô�ط������
	//		���ڣ�bool(д�ⲿ�ļ��ɹ���д�ⲿ�ļ�ʧ��)
	//		��ע��2001.1.10   fox
	//----------------------------------------------------
	bool DebugClass::Msg(const char* const vMsg,long vValue,const char* const vMsgFrom)
	{
		if(this->IsActive==true)
		{//�Ѿ�����
			if(vMsg==NULL)
			{//��ȫ���
				return false;
			}

			char Temp[STRING_LENGTH]="";
			sprintf(Temp,"%s : %d",vMsg,vValue);

			if(this->Msg(Temp,vMsgFrom)==false)
			{
				return false;
			}
			return true;
		}
		return false;
	}
	bool DebugClass::Msg(const int vValue,const char* const vMsgFrom)
	{
		return Msg((const long)vValue,vMsgFrom);
	}
	bool DebugClass::Msg(const unsigned int vValue, const char* const vMsgFrom)
	{
		return Msg((const unsigned long)vValue,vMsgFrom);
	}

	//----------------------------------------------------
	//		���ƣ�DebugClass::Msg
	//		���ܣ�DebugClass�������Ϣ���������������
	//		��ڣ�����ֵ,�ַ���ָ�루����Ϣ��ʲô�ط������
	//		���ڣ�bool(д�ⲿ�ļ��ɹ���д�ⲿ�ļ�ʧ��)
	//		��ע��2002.9.26   fox
	//----------------------------------------------------
	bool DebugClass::Msg(const long vValue,const char* const vMsgFrom)
	{
		if(this->IsActive)
		{
			char Temp[STRING_LENGTH]="";
			sprintf(Temp,"%d",vValue);
			if(this->Msg(Temp,vMsgFrom))
			{
				return true;
			}
		}
		return false;
	}

	//----------------------------------------------------
	//		���ƣ�DebugClass::Msg
	//		���ܣ�DebugClass�������Ϣ���������������
	//		��ڣ�����ֵ,�ַ���ָ�루����Ϣ��ʲô�ط������
	//		���ڣ�bool(д�ⲿ�ļ��ɹ���д�ⲿ�ļ�ʧ��)
	//		��ע��2002.9.26   fox
	//----------------------------------------------------
	bool DebugClass::Msg(const unsigned long vValue,const char* const vMsgFrom)
	{
		if(this->IsActive)
		{
			char Temp[STRING_LENGTH]="";
			sprintf(Temp,"%u",vValue);
			if(this->Msg(Temp,vMsgFrom))
			{
				return true;
			}
		}
		return false;
	}


	//----------------------------------------------------
	//		���ƣ�DebugClass::Msg
	//		���ܣ�DebugClass�������Ϣ
	//		��ڣ�string�����ã�Ҫ�������Ϣ��,
	//				�ַ���ָ�루����Ϣ��ʲô�ط������
	//		���ڣ�bool(д�ⲿ�ļ��ɹ���д�ⲿ�ļ�ʧ��)
	//		��ע��2003.7.9   fox
	//----------------------------------------------------
	bool DebugClass::Msg(const string& vrMsg,const char* const vMsgFrom)
	{
		return Msg(vrMsg.c_str(),vMsgFrom);
	}

	//----------------------------------------------------
	//		���ƣ�DebugClass::Msg
	//		���ܣ�DebugClass�������Ϣ
	//		��ڣ��ַ���ָ�루Ҫ�������Ϣ��,
	//				�ַ���ָ�루����Ϣ��ʲô�ط������
	//		���ڣ�bool(д�ⲿ�ļ��ɹ���д�ⲿ�ļ�ʧ��)
	//		��ע��2001.12.24   fox
	//----------------------------------------------------
	bool DebugClass::Msg(const char* const vMsg,const char* const vMsgFrom)
	{
		if(this->IsActive==true)
		{//�Ѿ�����
			char String1[STRING_LENGTH]="";
			char String2[STRING_LENGTH]="";
			if(vMsg!=NULL && vMsgFrom!=NULL)
			{
				sprintf(String1,"%s%s",vMsg,vMsgFrom);
			}
			else
			{
				if(vMsg==NULL )
				{
					sprintf(String2,"ERROR! Msg's Pointer is NULL!");
				}
				if(vMsgFrom==NULL)
				{
					sprintf(String1,"%s%s",String2,"MsgFrom's Pointer is NULL!");
				}
			}


			//��¼�������Ϣ׼������Ļ���
			MsgIndex++;
			MsgIndex=MsgIndex%MAX_LINES_IN_SCREEN;

			if(this->ScreenOutputActive)
			{
#ifdef  GRAPH_PRINT
				strcpy(MsgTxt[MsgIndex],String1);
				MsgTime[MsgIndex]=GET_TICK_COUNT();//��ǰʱ��
				MsgColor[MsgIndex]=Color;//�ö���������Ϣ����ɫ
#endif//GRAPH_PRINT
#ifdef  TEXT_PRINT
				PRINT_TO_SCREEN(0,0,String1,0xffffff);
				PRINT_TO_SCREEN(0,0,"\n",0xffffff);
#endif//TEXT_PRINT
			}

			if(this->FileOutputActive)
			{
				if(this->OutputFileIsSet==false)
				{//����ļ�δ������
					this->SetOutputFile(DEFAULT_OUTPUT_FILE);
				}


				if(this->FileIsOpen==false)
				{//��û���ļ���������
					this->fp=fopen((const char*)(this->FileName),"a+" );
					if(this->fp!=NULL)
					{
						this->FileIsOpen=true;
					}
				}

				if(this->FileIsOpen==true)
				{//����Ѿ��򿪣�д����Ϣ�����̹����ļ�����֤���̸���
					//д���ļ�
					fputs(String1,fp);
					fputs("\n",fp);
					fclose(fp);
					this->FileIsOpen =false;
					this->fp=NULL;
				}
				else
				{//δ���ļ�
					return false;
				}
			}
			return true;
		}
		return false;
	}

	//----------------------------------------------------
	//		���ƣ�DebugClass::FPS
	//		���ܣ��������ε��øú�����ʱ������ÿ�����
	//				�ú������Σ�����ͳ����������
	//		��ڣ���
	//		���ڣ�bool(�ɹ���ʧ��)
	//		��ע��2002.4.25   fox
	//----------------------------------------------------
	bool DebugClass::FPS()
	{
		if(!DebugClass::OutputScreen)
		{
			return false;
		}

		static DWORD LastCall=0;
		static int CallFPSTimes=0;
		const int COUNT_TIME=1000;//1�����һ����������
		CallFPSTimes++;


		long PassTime=GET_TICK_COUNT()-LastCall;
		if(PassTime>=COUNT_TIME)
		{
			if(PassTime==0)
			{
				Fps=1000;
			}
			else
			{
				Fps=1000/(PassTime/CallFPSTimes);//һ�����1000����
			}

			CallFPSTimes=0;
			LastCall=GET_TICK_COUNT();
		}

		//��ʾÿ�뻻֡��
		sprintf(TempString,"FPS:%d",Fps);

#ifdef  GRAPH_PRINT
		PRINT_TO_SCREEN(mScreenWidth/2,LINE_HEIGHT,TempString,0xff0000);
#endif//GRAPH_PRINT

		return true;
	}


	//----------------------------------------------------
	//		���ƣ�DebugClass::MsgOutputToScreen
	//		���ܣ�DebugClass�������Ϣ����Ļ
	//		��ڣ���
	//		���ڣ�bool(�ɹ���ʧ��)
	//		��ע��2002.3.22   fox
	//----------------------------------------------------
	bool DebugClass::MsgOutputToScreen()
	{
#ifdef  GRAPH_PRINT
		if(!DebugClass::OutputScreen)
		{
			return false;
		}

		long Index;
		long OutputIndex;
		bool IsBreak=false;
		Index=(MAX_LINES_IN_SCREEN-1)-MsgIndex;
		OutputIndex=(MAX_LINES_IN_SCREEN-1)-Index;
		//��ʱ����Ϣ���
		for(long i=0;i<MAX_LINES_IN_SCREEN;i++)
		{
			if(GET_TICK_COUNT()-MsgTime[i]>DEFAULT_WAIT_TIME)
			{
				MsgTxt[i][0]='\0';//�����Ϣ
			}
		}
		//�����������һ�����ַ���
		for(i=0;i<MAX_LINES_IN_SCREEN;i++)
		{
			if(MsgTxt[OutputIndex][0]=='\0')
			{
				IsBreak=true;
				break;
			}

			//����OutputIndex--;��Ϊȡģ������������Index
			Index++;
			Index=Index % MAX_LINES_IN_SCREEN;
			OutputIndex=(MAX_LINES_IN_SCREEN-1)-Index;
		}

		//�õ������һ�����ַ�����ص�����


		if(IsBreak && OutputIndex==MsgIndex)
		{//����Ϊ��
			return false;
		}
		else
		{//������һ���ַ���
			OutputIndex++;
			OutputIndex=OutputIndex % MAX_LINES_IN_SCREEN;
		}

		static int R;
		static int G;
		static int B;

		for(long j=0;;j++)
		{
			PRINT_TO_SCREEN(0,j*LINE_HEIGHT+MsgOutputY(), MsgTxt[OutputIndex],MsgColor[OutputIndex]);
			if(OutputIndex==MsgIndex)
			{
				break;
			}
			OutputIndex++;
			OutputIndex=OutputIndex % MAX_LINES_IN_SCREEN;
		}
		return true;
#endif//GRAPH_PRINT
		return false;
	}

}












