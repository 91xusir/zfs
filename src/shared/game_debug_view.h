

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
	static CView* GetView()//Ҫ����FreeView�ͷ���Դ
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

//Outputװ��
class COutputDecorate:public COutput
{
public:
virtual void Print(const string& vrMessage,const string& vrFrom,BYTE vLevel,DWORD vColor)=0;
};



//����From�����������ݹ�����
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
{//��������
if(!mMessageFilter.Filter(vrMessage))
{//��������
mpOutput->Print(vrMessage,vrFrom,vLevel,vColor);
}
}
};

void AddFromFilter(const string& vrFilter)
{//����һ������ʲô�ط��Ĺ�������
mFromFilter.Add(vrFilter);
};

void DelFromFilter(const string& vrFilter)
{//����һ������ʲô�ط��Ĺ�������
mFromFilter.Del(vrFilter);
};

void AddMsgFilter(const string& vrFilter)
{//����һ����Ϣ�Ĺ�������
mMessageFilter.Add(vrFilter);
}

void DelMsgFilter(const string& vrFilter)
{//����һ����Ϣ�Ĺ�������
mMessageFilter.Del(vrFilter);
}

CFilter<TFromRule>			mFromFilter;
CFilter<TMessageRule>		mMessageFilter;
private:
COutput* mpOutput;
};


*/




#endif//GAME_DEBUG_VIEW_H
