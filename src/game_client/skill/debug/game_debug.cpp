//===========================================================
//	文件名称：	game_debug.cpp
//	作    者：  吴麟
//	时    间：	2001.12.24
//	内    容： 	游戏调试器源代码
//	修正历史：
//				2003.12.12		修正一个显示信息的BUG
//				2003.10.30		将接口部分整理了一下
//				2003.9.19		添加一些方法
//				2003.5.28		修正一些上字的BUG
//				2003.2.27		修改某些功能
//				2003.2.22
//===========================================================z
#include "game_debug.h" 

#ifndef GRAPH_PRINT//图形上字系统
#define TEXT_PRINT//文字上字系统
#endif//GRAPH_PRINT//图形上字系统



//<同系统相关代码>
#ifdef  LINUX_SYSTEM//linux系统
inline DWORD GET_TICK_COUNT(){return 0;};//获得时间

//输出信息到屏幕
inline void PRINT_TO_SCREEN(int vX,int vY,char* vpMessage,DWORD vColor)
{
#ifdef TEXT_PRINT//非图形文字系统
	printf(vpMessage);
#endif
};
#else//win系统


#include "windows.h"

#ifdef MEMORY_USED_STATUS//输出内存信息
#include "psapi.h"
#pragma comment(lib,"psapi.lib")
#endif//MEMORY_USED_STATUS

#ifdef  GRAPH_PRINT//图形上字系统
#include "game_app.h"
#endif//GRAPH_PRINT


inline DWORD GET_TICK_COUNT(){return GetTickCount();};//获得时间
//输出信息到屏幕
inline void PRINT_TO_SCREEN(int vX,int vY,const char* vpMessage,DWORD vColor)
{
#ifdef GRAPH_PRINT//图形上字系统
	int R;
	int G;
	int B;

	R=(vColor&0x00ff0000)>>16;
	G=(vColor&0x0000ff00)>>8;
	B=(vColor&0x000000ff);
	//<同上图系统相关>
	g_pApp->mDFWorld.MessageOnScreen(vX,vY,(char*)vpMessage,R,G,B);
	//</同上图系统相关>
#endif

#ifdef TEXT_PRINT//非图形文字系统
	printf(vpMessage);
#endif
};
#endif//LINUX_SYSTEM
//</同系统相关代码>






namespace fox
{
	static char TempString[STRING_LENGTH];	//临时字符串

	DebugClass Debug;	//调试用
	DebugClass Err;		//出错用
	//----------------------------------------------------
	//		名称：DebugClass::DebugClass
	//		功能：构造函数
	//		入口：无
	//		出口：无
	//		备注：2001.12.24   fox
	//			  2002.1.21	   fox 修改
	//----------------------------------------------------
	long DebugClass::MsgIndex=0;						//当前信息的索引
	char DebugClass::MsgTxt[MAX_LINES_IN_SCREEN][STRING_LENGTH]={{0}};
	DWORD DebugClass::MsgTime[MAX_LINES_IN_SCREEN]={0};	//输出信息的时间
	DWORD DebugClass::MsgColor[MAX_LINES_IN_SCREEN]={0};//输出的信息的颜色
	bool DebugClass::OutputScreen=false;				//是否至少有一个实例要求输出到屏幕
	long DebugClass::Fps=0;								//一般用来统计上屏帧数
	int  DebugClass::mScreenWidth=DEFAULT_SCREEN_WIDTH;			//屏幕的宽
	int  DebugClass::mScreenHeight=DEFAULT_SCREEN_HEIGHT;		//屏幕的高


	DebugClass::DebugClass()
	{
		this->OutputFileIsSet=false;//输出文件未设置
		this->ScreenOutputActive=false;
		this->FileOutputActive=false;
		this->FileIsOpen =false;
		this->FileName[0]=0; //初始化文件名为空
		this->fp=NULL;
		this->IsActive=false;
		this->Color=0xffffff;


	}

	//----------------------------------------------------
	//		名称：DebugClass::~DebugClass
	//		功能：析构函数
	//		入口：无
	//		出口：无
	//		备注：2001.12.24   fox
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
			this->FileName[0]=0; //初始化文件名为空
			this->fp=NULL;
			this->IsActive=false;
		}
	}

	//----------------------------------------------------
	//		名称：DebugClass::UsedMemory
	//		功能：输出已经使用的内存
	//		入口：无
	//		出口：bool （成功，失败）
	//		备注：2002.6.3   fox
	//----------------------------------------------------
	bool DebugClass::UsedMemory()
	{


#ifdef MEMORY_USED_STATUS
		if(!DebugClass::OutputScreen)
		{
			return false;
		}
		//DEBUG模式
		static HANDLE hProcess = GetCurrentProcess();
		static PROCESS_MEMORY_COUNTERS pmc;
		static long mem=0;
		if ( GetProcessMemoryInfo( hProcess, &pmc, sizeof(pmc)) )
		{
			mem = pmc.WorkingSetSize>>10;
		}
		long mem0=mem/1000;				//得到多少M的整数部分
		long mem1=mem%1000;				//得到小数部分

		sprintf(TempString,"Used:%d.%d M",mem0,mem1);
#ifdef  GRAPH_PRINT
		PRINT_TO_SCREEN(mScreenWidth*2/3,LINE_HEIGHT,TempString,0xffffff);//在2/3屏处显示
#endif//GRAPH_PRINT
		return true;

#else
		return false;

#endif//MEMORY_USED_STATUS
	}

	//----------------------------------------------------
	//		名称：DebugClass::SetOutputFile
	//		功能：设置要输出的文件名
	//		入口：要导出的文件名(不超过1024个，含结尾零)
	//		出口：bool （成功，失败）
	//		备注：2002.2.11   fox
	//----------------------------------------------------
	bool DebugClass::SetOutputFile(const char vOutputFileName[])
	{
		if(vOutputFileName==NULL)
		{//错误
			return false;
		}

		sprintf(this->FileName,"%s",vOutputFileName);
		this->OutputFileIsSet=true;//输出文件名已经被设置
		return true;
	}


	//----------------------------------------------------
	//		名称：DebugClass::Init
	//		功能：初始化调试器，应在其它有关函数调用之前，
	//               调用该函数。
	//		入口：输出到屏幕上信息的颜色（24Bits）,
	//				激活输出到文件,激活输出到屏幕
	//		出口：bool （成功，失败）
	//		备注：2002.2.11   fox
	//----------------------------------------------------
	bool DebugClass::Init(DWORD vInfoColor,const bool vOutputFile,const bool vOutputScreen)
	{
		this->ScreenOutputActive=vOutputScreen;
		this->FileOutputActive=vOutputFile;
		this->IsActive=true;
		if(ScreenOutputActive)
		{
			DebugClass::OutputScreen=true;//至少有一个要求输出到屏幕
		}
		this->Color=vInfoColor;
		return true;
	}

	//----------------------------------------------------
	//		名称：DebugClass::On
	//		功能：打开调试器
	//		入口：无
	//		出口：bool （成功，失败）
	//		备注：2003.2.15   fox
	//----------------------------------------------------
	bool DebugClass::On()//是否需要调试信息
	{
		this->IsActive=true;
		return true;
	}

	//----------------------------------------------------
	//		名称：DebugClass::Off
	//		功能：关闭调试器
	//		入口：无
	//		出口：bool （成功，失败）
	//		备注：2003.2.15   fox
	//----------------------------------------------------
	bool DebugClass::Off()
	{
		this->IsActive=false;
		return true;
	}
	//----------------------------------------------------
	//		名称：DebugClass::Msg
	//		功能：DebugClass的输出信息（重载输出函数）
	//		入口：字符串指针（要输出的信息）,
	//				变量值，
	//				字符串指针（该信息由什么地方输出）
	//		出口：bool(写外部文件成功，写外部文件失败)
	//		备注：2001.1.10   fox
	//----------------------------------------------------
	bool DebugClass::Msg(const char* const vMsg,long vValue,const char* const vMsgFrom)
	{
		if(this->IsActive==true)
		{//已经激活
			if(vMsg==NULL)
			{//安全检查
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
	//		名称：DebugClass::Msg
	//		功能：DebugClass的输出信息（重载输出函数）
	//		入口：变量值,字符串指针（该信息由什么地方输出）
	//		出口：bool(写外部文件成功，写外部文件失败)
	//		备注：2002.9.26   fox
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
	//		名称：DebugClass::Msg
	//		功能：DebugClass的输出信息（重载输出函数）
	//		入口：变量值,字符串指针（该信息由什么地方输出）
	//		出口：bool(写外部文件成功，写外部文件失败)
	//		备注：2002.9.26   fox
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
	//		名称：DebugClass::Msg
	//		功能：DebugClass的输出信息
	//		入口：string的引用（要输出的信息）,
	//				字符串指针（该信息由什么地方输出）
	//		出口：bool(写外部文件成功，写外部文件失败)
	//		备注：2003.7.9   fox
	//----------------------------------------------------
	bool DebugClass::Msg(const string& vrMsg,const char* const vMsgFrom)
	{
		return Msg(vrMsg.c_str(),vMsgFrom);
	}

	//----------------------------------------------------
	//		名称：DebugClass::Msg
	//		功能：DebugClass的输出信息
	//		入口：字符串指针（要输出的信息）,
	//				字符串指针（该信息由什么地方输出）
	//		出口：bool(写外部文件成功，写外部文件失败)
	//		备注：2001.12.24   fox
	//----------------------------------------------------
	bool DebugClass::Msg(const char* const vMsg,const char* const vMsgFrom)
	{
		if(this->IsActive==true)
		{//已经激活
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


			//记录输出的信息准备向屏幕输出
			MsgIndex++;
			MsgIndex=MsgIndex%MAX_LINES_IN_SCREEN;

			if(this->ScreenOutputActive)
			{
#ifdef  GRAPH_PRINT
				strcpy(MsgTxt[MsgIndex],String1);
				MsgTime[MsgIndex]=GET_TICK_COUNT();//当前时间
				MsgColor[MsgIndex]=Color;//该对象的输出信息的颜色
#endif//GRAPH_PRINT
#ifdef  TEXT_PRINT
				PRINT_TO_SCREEN(0,0,String1,0xffffff);
				PRINT_TO_SCREEN(0,0,"\n",0xffffff);
#endif//TEXT_PRINT
			}

			if(this->FileOutputActive)
			{
				if(this->OutputFileIsSet==false)
				{//输出文件未被设置
					this->SetOutputFile(DEFAULT_OUTPUT_FILE);
				}


				if(this->FileIsOpen==false)
				{//若没打开文件，将它打开
					this->fp=fopen((const char*)(this->FileName),"a+" );
					if(this->fp!=NULL)
					{
						this->FileIsOpen=true;
					}
				}

				if(this->FileIsOpen==true)
				{//如果已经打开，写入信息，立刻关上文件，保证立刻更新
					//写入文件
					fputs(String1,fp);
					fputs("\n",fp);
					fclose(fp);
					this->FileIsOpen =false;
					this->fp=NULL;
				}
				else
				{//未打开文件
					return false;
				}
			}
			return true;
		}
		return false;
	}

	//----------------------------------------------------
	//		名称：DebugClass::FPS
	//		功能：根据两次调用该函数的时间差，推算每秒调用
	//				该函数几次，用来统计上屏次数
	//		入口：无
	//		出口：bool(成功，失败)
	//		备注：2002.4.25   fox
	//----------------------------------------------------
	bool DebugClass::FPS()
	{
		if(!DebugClass::OutputScreen)
		{
			return false;
		}

		static DWORD LastCall=0;
		static int CallFPSTimes=0;
		const int COUNT_TIME=1000;//1秒计算一次上屏次数
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
				Fps=1000/(PassTime/CallFPSTimes);//一秒等于1000毫秒
			}

			CallFPSTimes=0;
			LastCall=GET_TICK_COUNT();
		}

		//显示每秒换帧数
		sprintf(TempString,"FPS:%d",Fps);

#ifdef  GRAPH_PRINT
		PRINT_TO_SCREEN(mScreenWidth/2,LINE_HEIGHT,TempString,0xff0000);
#endif//GRAPH_PRINT

		return true;
	}


	//----------------------------------------------------
	//		名称：DebugClass::MsgOutputToScreen
	//		功能：DebugClass的输出信息到屏幕
	//		入口：无
	//		出口：bool(成功，失败)
	//		备注：2002.3.22   fox
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
		//超时的信息清空
		for(long i=0;i<MAX_LINES_IN_SCREEN;i++)
		{
			if(GET_TICK_COUNT()-MsgTime[i]>DEFAULT_WAIT_TIME)
			{
				MsgTxt[i][0]='\0';//清空信息
			}
		}
		//搜索到最近的一个空字符串
		for(i=0;i<MAX_LINES_IN_SCREEN;i++)
		{
			if(MsgTxt[OutputIndex][0]=='\0')
			{
				IsBreak=true;
				break;
			}

			//避免OutputIndex--;因为取模问题所以引进Index
			Index++;
			Index=Index % MAX_LINES_IN_SCREEN;
			OutputIndex=(MAX_LINES_IN_SCREEN-1)-Index;
		}

		//得到最近的一个空字符串或回到自身


		if(IsBreak && OutputIndex==MsgIndex)
		{//自身为空
			return false;
		}
		else
		{//它的下一个字符串
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












