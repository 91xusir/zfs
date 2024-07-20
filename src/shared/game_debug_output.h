
#ifndef GAME_DEBUG_OUTPUT_H 
#define GAME_DEBUG_OUTPUT_H

typedef unsigned char BYTE;
typedef unsigned long DWORD;

#include <string>
#include <map>
#include <list>
#include "game_reference_ofstream.h"
#include "game_debug_view.h"

using std::map;
using std::list;
using std::string;





class COutput
{
	friend class COutputFactory;
public:
	enum EType
	{
		CONSOLE = (1<<0),
		FILES   = (1<<1),
		//REMOTE  = (1<<2),
	};
	virtual void Print(const string& vrMessage,const string& vrFrom,BYTE vLevel,DWORD vColor)=0;
	virtual ~COutput(){};
	virtual COutput* Clone() const =0;
protected:
	COutput(){};
};

class CConsoleOutput:public COutput
{
	friend class COutputFactory;
public:
	
	virtual ~CConsoleOutput()
	{
		if(mpView!=NULL)
		{
			CViewFactory::FreeView();
		}
	};
	virtual void Print(const string& vrMessage,const string& vrFrom,BYTE vLevel,DWORD vColor)
	{
		mpView->Print(vrMessage,vrFrom,vLevel,vColor);
	}

	virtual COutput* Clone() const
	{
		return RT_NEW CConsoleOutput(*this);
	}

protected:
	CConsoleOutput(CView* vpView):mpView(vpView){};
	CConsoleOutput(const CConsoleOutput& vrConsoleOutput)
		:mpView(CViewFactory::GetView())
	{
	};
private:
	CView*	mpView;
};


class CFileOutput:public COutput
{
	friend class COutputFactory;
public:
	virtual ~CFileOutput()
	{
		if(mpRefOfstream->DelRef()==0)
		{
			map<string,RefOfstream*>::iterator It;
			for(It=mpFiles->begin();It!=mpFiles->end();++It)
			{
				if(It->second == mpRefOfstream)
				{
					mpFiles->erase(It);
					break;
				}
			}

			DEL_ONE(mpRefOfstream);
		}

		mInitCount--;
		if(mInitCount==0)
		{
			DEL_ONE(mpFiles);
			mpFiles=NULL;
		}
	};

	virtual void Print(const string& vrMessage,const string& vrFrom,BYTE vLevel,DWORD vColor)
	{
		if(vrFrom=="")
		{
			mpRefOfstream->Get()<<vrMessage;
		}
		else
		{
			mpRefOfstream->Get()<<vrMessage<< "["<< vrFrom <<"]"<<endl;
		}
		
	};

	virtual COutput* Clone() const
	{
		return RT_NEW CFileOutput(*this);
	}

protected:
	CFileOutput(const string& vrFileName):COutput()
	{

		if(mpFiles==NULL)
		{
			mpFiles=RT_NEW map<string,RefOfstream*>;
		}
		mInitCount++;

		map<string,RefOfstream*>::iterator It=mpFiles->find(vrFileName);
		if(It==mpFiles->end())
		{//没有这个文件
			mpRefOfstream=RT_NEW RefOfstream(vrFileName);
			(*mpFiles)[vrFileName]=mpRefOfstream;
		}
		else
		{//有这个文件
			mpRefOfstream=It->second;
			mpRefOfstream->AddRef();
		}
	};

	CFileOutput(const CFileOutput& vrFileOutput)
	{
		vrFileOutput.mpRefOfstream->AddRef();
		mpRefOfstream=vrFileOutput.mpRefOfstream;
		
		mInitCount++;
	}

private:
	RefOfstream*							mpRefOfstream;
	static map<string,RefOfstream*>*		mpFiles;
	static int								mInitCount;
	
};

class COutputContainer:public COutput
{
	friend class COutputFactory;
public:
	virtual ~COutputContainer()
	{
		for(list<COutput*>::iterator It=mList.begin();It!=mList.end();++It)
		{
			DEL_ONE(*It);
		}
	};

	virtual void Print(const string& vrMessage,const string& vrFrom,BYTE vLevel,DWORD vColor)
	{
		for(list<COutput*>::iterator It=mList.begin();It!=mList.end();++It)
		{
			(*It)->Print(vrMessage,vrFrom,vLevel,vColor);
		}
	};

	void Add(COutput* vpOutput){mList.push_back(vpOutput);};
	void Clear(){mList.clear();};
	int Size()
	{
		return (int) (mList.size());
	};

	COutput* operator [](const int vIndex)
	{
		int Index=0;
		for(list<COutput*>::iterator It=mList.begin();It!=mList.end();++It)
		{
			
			if( Index == vIndex)
			{
				return (*It);
			}
			++Index;
		}
		return NULL;
	};

	virtual COutput* Clone() const
	{
		COutputContainer* p=RT_NEW COutputContainer;
		list<COutput*>::const_iterator It;
		for(It=mList.begin();It!=mList.end();++It)
		{
			p->Add((*It)->Clone());
		}

		return p;
	}

protected:
	COutputContainer(){};
private:
	list<COutput*> mList;
};


class COutputFactory
{
public:
	static COutput* GetOutput(const int vType,const string& vrFlag)//得到的指针，要销毁
	{
		return GetOutput( (const COutput::EType) vType,vrFlag);
	}

	static COutput* GetOutput(const COutput& vrOutput)//得到的指针，要销毁
	{
		return vrOutput.Clone();
	}

	static COutput* GetOutput(const COutput::EType vType,const string& vrFlag)//得到的指针，要销毁
	{
		COutputContainer * pTempContainer=RT_NEW COutputContainer;

		//根据vOutput得到//需要的COutput
		if(vType & COutput::CONSOLE)
		{
			pTempContainer->Add(RT_NEW CConsoleOutput(CViewFactory::GetView()));
		}

		if(vType & COutput::FILES)
		{
			pTempContainer->Add(RT_NEW CFileOutput(vrFlag));
		}

		if(pTempContainer->Size()==1)
		{//容器仅包含一个//那就不用返回容器//只返回包含的那个CCoutput
			COutput* pOutput= (*pTempContainer)[0];
			pTempContainer->Clear();
			DEL_ONE(pTempContainer);
			return pOutput;
		}
		else
		{//不只包含一个//返回整个容器
			return pTempContainer;
		}
	};
};


#endif//GAME_DEBUG_OUTPUT_H
