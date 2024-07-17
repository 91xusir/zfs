//===========================================================
//	�ļ����ƣ�	game_debug.h
//	��    �ߣ�	����
//	ʱ    �䣺	2001.12.24
//	��    �ݣ�	��Ϸ��������Ϣͷ�ļ�
//	������ʷ��
//				2003.12.12		����һ����ʾ��Ϣ��BUG
//				2003.10.30		���ӿڲ���������һ��
//				2003.9.19		���һЩ����	
//				2003.5.28		����һЩ���ֵ�BUG
//				2003.2.27		�޸�ĳЩ����
//				2003.2.22
//===========================================================
#ifndef _GAME_DEBUG_H_
#define _GAME_DEBUG_H_


#define GAME_CLIENT



#ifdef	GAME_CLIENT	//�ͻ���
//#define MEMORY_USED_STATUS//�Ƿ�Ҫ����ڴ�ʹ�����(������WIN2000ϵͳʹ��)
//#define GRAPH_PRINT//ͼ������ϵͳ
#else				//��������
//#define LINUX_SYSTEM//����������linux
#endif//GAME_CLIENT




typedef unsigned long       DWORD;


#include "stdio.h"
#include <string>
#ifdef  LINUX_SYSTEM
#define CODE_NOT_FINISH {char Temp[1024]; sprintf(Temp,"%s%s%s%d%s","CODE_NOT_FINISH :[","unknow","]-->","unknow","��");fox::Debug.Msg(Temp);}
#define CODE_NOT_DEAL   {char Temp[1024]; sprintf(Temp,"%s%s%s%d%s","CODE_NOT_DEAL:[","unknow","]-->","unknow","��");fox::Debug.Msg(Temp);}
#define CODE_MUST_FIX   {char Temp[1024]; sprintf(Temp,"%s%s%s%d%s","CODE_MUST_FIX:[","unknow","]-->","unknow","��");fox::Debug.Msg(Temp);}
#else
#define CODE_NOT_FINISH {char Temp[1024]; sprintf(Temp,"%s%s%s%d%s","CODE_NOT_FINISH :[",__FILE__,"]-->",__LINE__,"��");fox::Debug.Msg(Temp);}
#define CODE_NOT_DEAL   {char Temp[1024]; sprintf(Temp,"%s%s%s%d%s","CODE_NOT_DEAL:[",__FILE__,"]-->",__LINE__,"��");fox::Debug.Msg(Temp);}
#define CODE_MUST_FIX   {char Temp[1024]; sprintf(Temp,"%s%s%s%d%s","CODE_MUST_FIX:[",__FILE__,"]-->",__LINE__,"��");fox::Debug.Msg(Temp);}
#endif//LINUX_SYSTEM

namespace fox
{
	const int STRING_LENGTH=1024;		//�ַ������ȣ�������β�㣩
	const int DEFAULT_SCREEN_HEIGHT = 600;
	const int DEFAULT_SCREEN_WIDTH = 800;

	const int MAX_LINES_IN_SCREEN = 10;
	const int DEFAULT_WAIT_TIME = 7000;
	const int LINE_HEIGHT = 18;

	const char DEFAULT_OUTPUT_FILE[] ="DebugOutput.txt";
	const char DEFAULT_MSG[]= "NO MSG!";
	const char DEFAULT_MSG_FROM[]= "     (From program)";



	using std::string;

	class DebugClass;
	//�Ѿ�׼������һ�����ڵ��Ե�ȫ�ֶ����벻Ҫ����ʵ������
	extern DebugClass Debug;
	//�Ѿ�׼������һ�����ڴ�������ȫ�ֶ����벻Ҫ����ʵ������
	extern DebugClass Err;

	class DebugClass
	{
	public:
		static bool MsgOutputToScreen(); 	
		static bool FPS() ;//�������ε��øú�����ʱ������ÿ����øú������Σ�����ͳ����������
		static bool UsedMemory();

		DebugClass();//���캯��
		~DebugClass();
		bool Msg(const string& vrMsg,const char* const MsgFrom=DEFAULT_MSG_FROM);
		bool Msg(const char* const Msg=DEFAULT_MSG,const char* const MsgFrom=DEFAULT_MSG_FROM);
		bool Msg(const long ,const char* const MsgFrom=DEFAULT_MSG_FROM);
		bool Msg(const unsigned long,const char* const MsgFrom=DEFAULT_MSG_FROM);
		bool Msg(const int,const char* const MsgFrom=DEFAULT_MSG_FROM);
		bool Msg(const unsigned int, const char* const MsgFrom=DEFAULT_MSG_FROM);
		bool Msg(const char* const Msg,const long,const char* const MsgFrom=DEFAULT_MSG_FROM);
		bool SetOutputFile(const char vOutputFileName[]);
		bool Init(DWORD vInfoColor=0xffffff,const bool vOutputFile=false,const bool vOutputScreen=true);
		static void SetScreen(const int vWidth,const int vHeight){mScreenWidth=vWidth;mScreenHeight=vHeight;};//���������Ļ�Ĵ�С

		bool On();
		bool Off();
	private:
		static int MsgOutputY(){return mScreenHeight-MAX_LINES_IN_SCREEN*LINE_HEIGHT;};


		static char MsgTxt[MAX_LINES_IN_SCREEN][STRING_LENGTH];		//�����Ϣ�����ļ�¼
		static DWORD MsgColor[MAX_LINES_IN_SCREEN];					//�����Ϣ����ɫ
		static DWORD MsgTime[MAX_LINES_IN_SCREEN];					//�����Ϣ��ʱ��
		static int  mScreenWidth;									//��Ļ�Ŀ�
		static int  mScreenHeight;									//��Ļ�ĸ�

		static long MsgIndex;										//��ǰ��Ϣ������
		static bool OutputScreen;									//�Ƿ�������һ��ʵ��Ҫ���������Ļ
		static long Fps;											//һ������ͳ������֡��


		bool OutputFileIsSet;										//����ļ��Ƿ�����
		bool IsActive;												//�����Ƿ񱻼���
		bool ScreenOutputActive;									//��Ļ�������
		bool FileOutputActive;										//�ļ������Ϣ����
		bool FileIsOpen;											//�ļ��Ƿ��Ѿ���
		char FileName[STRING_LENGTH];								//�ļ����Ĵ洢�ռ�

		DWORD Color;												//Ҫ�������Ļ�ϵ���Ϣ����ɫ

		FILE* fp;													//�ļ�ָ��
	};
}
#endif//_GAME_DEBUG_H_




