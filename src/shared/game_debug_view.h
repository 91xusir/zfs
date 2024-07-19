

#ifndef GAME_DEBUG_VIEW_H
#define GAME_DEBUG_VIEW_H

#include <iostream>
#include "game_filter.h"
#include "core/rt2_core.h"
using std::cout;
using std::endl;

typedef unsigned char BYTE;
typedef unsigned long DWORD;


class CView
{
	friend class CViewFactory;
public:
	virtual void Print(const string& vrMessage,const string& vrFrom,BYTE vLevel,DWORD vColor)=0;
	CFilter<CFilterEqual> mFromFilter;
	CFilter<CFilterEqual> mLevelFilter;



protected:
	CView(){};
	virtual ~CView(){};
};


class CConsoleView:public CView
{
	friend class CViewFactory;
public:
	virtual void Print(const string& vrMessage,const string& vrFrom,BYTE vLevel,DWORD vColor)
	{
		if(mFromFilter.Filter(vrFrom))
		{
			return;
		}

		if(vrFrom=="")
		{
			cout<<vrMessage;
		}
		else
		{
			cout<<vrMessage<< "["<< vrFrom <<"]"<<endl;
		}
	};

protected:
		CConsoleView(){};
		CConsoleView(const CConsoleView& vrConsoleView)
		{
		};
		~CConsoleView(){};
};


class CViewFactory
{
public:
	static CView* GetView()//要调用FreeView释放资源
	{
		if(InitCount==0)
		{
			mpView=RT_NEW CConsoleView;
		}

		InitCount++;

		return mpView;
	};

	static void FreeView()
	{
		InitCount--;
		if(InitCount==0)
		{
			DEL_ONE(mpView)
		}
	}

private:
	static int InitCount;
	static CView* mpView;
};





/*

//Output装饰
class COutputDecorate:public COutput
{
public:
virtual void Print(const string& vrMessage,const string& vrFrom,BYTE vLevel,DWORD vColor)=0;
};



//包含From过滤器和内容过滤器
template<typename TFromRule,typename TMessageRule>
class CFilterOutput:public COutputDecorate
{
public:
CFilterOutput(COutput* vpOutput):mpOutput(vpOutput){};
~CFilterOutput()
{
if(mpOutput!=NULL)
{
delete mpOutput;
}
};

virtual void Print(const string& vrMessage,const string& vrFrom,BYTE vLevel,DWORD vColor)
{
if(!mFromFilter.Filter(vrFrom))
{//不被过滤
if(!mMessageFilter.Filter(vrMessage))
{//不被过滤
mpOutput->Print(vrMessage,vrFrom,vLevel,vColor);
}
}
};

void AddFromFilter(const string& vrFilter)
{//增加一个来自什么地方的过滤属性
mFromFilter.Add(vrFilter);
};

void DelFromFilter(const string& vrFilter)
{//减少一个来自什么地方的过滤属性
mFromFilter.Del(vrFilter);
};

void AddMsgFilter(const string& vrFilter)
{//增加一个信息的过滤属性
mMessageFilter.Add(vrFilter);
}

void DelMsgFilter(const string& vrFilter)
{//减少一个信息的过滤属性
mMessageFilter.Del(vrFilter);
}

CFilter<TFromRule>			mFromFilter;
CFilter<TMessageRule>		mMessageFilter;
private:
COutput* mpOutput;
};


*/




#endif//GAME_DEBUG_VIEW_H
