//===========================================================
//	文件名称：	game_debug.h
//	作    者：	吴麟
//	时    间：	2001.12.24
//	内    容：	游戏调试器信息头文件
//	修正历史：
//				2003.12.12		修正一个显示信息的BUG
//				2003.10.30		将接口部分整理了一下
//				2003.9.19		添加一些方法	
//				2003.5.28		修正一些上字的BUG
//				2003.2.27		修改某些功能
//				2003.2.22
//===========================================================
#ifndef _GAME_DEBUG_H_
#define _GAME_DEBUG_H_


#define GAME_CLIENT



#ifdef	GAME_CLIENT	//客户端
//#define MEMORY_USED_STATUS//是否要输出内存使用情况(仅能在WIN2000系统使用)
//#define GRAPH_PRINT//图形上字系统
#else				//服务器端
//#define LINUX_SYSTEM//服务器端用linux
#endif//GAME_CLIENT




typedef unsigned long       DWORD;


#include "stdio.h"
#include <string>
#ifdef  LINUX_SYSTEM
#define CODE_NOT_FINISH {char Temp[1024]; sprintf(Temp,"%s%s%s%d%s","CODE_NOT_FINISH :[","unknow","]-->","unknow","行");fox::Debug.Msg(Temp);}
#define CODE_NOT_DEAL   {char Temp[1024]; sprintf(Temp,"%s%s%s%d%s","CODE_NOT_DEAL:[","unknow","]-->","unknow","行");fox::Debug.Msg(Temp);}
#define CODE_MUST_FIX   {char Temp[1024]; sprintf(Temp,"%s%s%s%d%s","CODE_MUST_FIX:[","unknow","]-->","unknow","行");fox::Debug.Msg(Temp);}
#else
#define CODE_NOT_FINISH {char Temp[1024]; sprintf(Temp,"%s%s%s%d%s","CODE_NOT_FINISH :[",__FILE__,"]-->",__LINE__,"行");fox::Debug.Msg(Temp);}
#define CODE_NOT_DEAL   {char Temp[1024]; sprintf(Temp,"%s%s%s%d%s","CODE_NOT_DEAL:[",__FILE__,"]-->",__LINE__,"行");fox::Debug.Msg(Temp);}
#define CODE_MUST_FIX   {char Temp[1024]; sprintf(Temp,"%s%s%s%d%s","CODE_MUST_FIX:[",__FILE__,"]-->",__LINE__,"行");fox::Debug.Msg(Temp);}
#endif//LINUX_SYSTEM

namespace fox
{
	const int STRING_LENGTH=1024;		//字符串长度（包括结尾零）
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
	//已经准备好了一个用于调试的全局对象，请不要另行实例对象
	extern DebugClass Debug;
	//已经准备好了一个用于处理出错的全局对象，请不要另行实例对象
	extern DebugClass Err;

	class DebugClass
	{
	public:
		static bool MsgOutputToScreen(); 	
		static bool FPS() ;//根据两次调用该函数的时间差，推算每秒调用该函数几次，用来统计上屏次数
		static bool UsedMemory();

		DebugClass();//构造函数
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
		static void SetScreen(const int vWidth,const int vHeight){mScreenWidth=vWidth;mScreenHeight=vHeight;};//设置输出屏幕的大小

		bool On();
		bool Off();
	private:
		static int MsgOutputY(){return mScreenHeight-MAX_LINES_IN_SCREEN*LINE_HEIGHT;};


		static char MsgTxt[MAX_LINES_IN_SCREEN][STRING_LENGTH];		//输出信息上屏的记录
		static DWORD MsgColor[MAX_LINES_IN_SCREEN];					//输出信息的颜色
		static DWORD MsgTime[MAX_LINES_IN_SCREEN];					//输出信息的时间
		static int  mScreenWidth;									//屏幕的宽
		static int  mScreenHeight;									//屏幕的高

		static long MsgIndex;										//当前信息的索引
		static bool OutputScreen;									//是否至少有一个实例要求输出到屏幕
		static long Fps;											//一般用来统计上屏帧数


		bool OutputFileIsSet;										//输出文件是否被设置
		bool IsActive;												//调试是否被激活
		bool ScreenOutputActive;									//屏幕输出激活
		bool FileOutputActive;										//文件输出信息激活
		bool FileIsOpen;											//文件是否已经打开
		char FileName[STRING_LENGTH];								//文件名的存储空间

		DWORD Color;												//要输出到屏幕上的信息的颜色

		FILE* fp;													//文件指针
	};
}
#endif//_GAME_DEBUG_H_




