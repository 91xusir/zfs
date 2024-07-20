



#ifndef GAME_LISTENER_H
#define GAME_LISTENER_H
typedef unsigned long       DWORD;      


//用户请使用CGameListenerManager


//这个类不用来被使用者继承
class CGameListener                 //队伍的事件监视接口类
{
public:
	CGameListener				(){};
	virtual ~CGameListener      (){};

	virtual bool OnEvent(DWORD vDBID,const char* vpName)=0;			//如果返回true则，事件被截取
};


template<class T,bool (T::*F)(DWORD vDBID,const char* vpName)>
class CConcreteGameListener:public CGameListener
{
public:
	CConcreteGameListener(T& vrT):mrT(vrT){};
	virtual ~CConcreteGameListener(){};
	virtual bool OnEvent(DWORD vDBID,const char* vpName)
	{
		return (mrT.*F)(vDBID,vpName);
	};

private:
	T& mrT;
};



//用户接口
class CGameListenerManager
{
public:
	CGameListenerManager(){};
	~CGameListenerManager()
	{
		//将Listener删除
		std::list<CGameListener*>::iterator It;
		for(It=mpListenerList.begin();It!=mpListenerList.end();It++)
		{
			DEL_ONE(*It);
		}
	};
	
	//增加监听者<类型,该类的方法>//返回true表示该事件被截获[不再发给其它监听者]
	template<class T,bool (T::*F)(DWORD vDBID,const char* vpName)>
	void AddListener(T& vrT)
	{
		CGameListener* pListener = RT_NEW CConcreteGameListener<T,F>(vrT);
		if(pListener!=NULL)
		{
			mpListenerList.push_back(pListener);
		}
	};
	
	template<class T>
	void DeleteListener(T& vrT)
	{
		std::list<CGameListener*>::iterator It;
		for(It=mpListenerList.begin();It!=mpListenerList.end();It++)
		{
			if((*It)==vrT)
			{
				DEL_ONE(*It);
				mpListenerList.erase(It);
				break;
			}
		}
	}
	


	void OnEvent(DWORD vDBID,const char* vpName)
	{
		std::list<CGameListener*>::iterator It;
		for(It=mpListenerList.begin();It!=mpListenerList.end();It++)
		{
			if((*It)->OnEvent(vDBID,vpName))
			{
				break;
			}
		}
		return;
	}



private:
	std::list<CGameListener*>	mpListenerList;
};

namespace fox
{
	//事件的基类
	//class CEvent;

	//游戏的监听者的基类
	class CListener
	{
	public:
		CListener(){};
		virtual ~CListener(){};

		virtual bool OnEvent(void* vpEvent)=0;			//如果返回true则，事件被截取
		virtual bool	operator == (void* vpMaster)=0;	//宿主是否相同
	};

	//具体的监听者
	template<class T,bool (T::*F)(void* vpEvent) >
	class CConcreteListener:public CListener
	{
	public:
		CConcreteListener(T& vrT):mrT(vrT){};
		virtual ~CConcreteListener(){};
		virtual bool OnEvent(void* vpEvent)
		{
			return (mrT.*F)(vpEvent);
		}

		virtual bool  operator == (void* vpMaster)
		{
			return ((&mrT)==vpMaster);
		}

	private:
		T& mrT;
	};


	
	//监听的管理类//用户的接口
	class CListenerManager
	{
	public:

		CListenerManager(){};
		~CListenerManager()
		{
			//将Listener删除
			std::list<CListener*>::iterator It;
			for(It=mpListenerList.begin();It!=mpListenerList.end();++It)
			{
				DEL_ONE(*It);
			}
		};

		//增加监听者<类型，该类的方法>//返回true表示该事件被截获[不再发给其它监听者]
		template<class T,bool (T::*F)(void* vpEvent)>
		void AddListener(T& vrT)
		{
			CListener* pListener=RT_NEW CConcreteListener<T,F>(vrT);
			if(pListener!=NULL)
			{
				mpListenerList.push_back(pListener);
			}
		}

		template<class T>
		void DeleteListener(T& vrT)
		{
			std::list<CListener*>::iterator It;
			for(It=mpListenerList.begin();It!=mpListenerList.end();++It)
			{
				if( (*(*It)) == (&vrT) )
				{
					DEL_ONE(*It);
					mpListenerList.erase(It);
					break;
				}
			}
		}

		void OnEvent(void* vpEvent=NULL)
		{
			std::list<CListener*>::iterator It;

			for(It=mpListenerList.begin();It!=mpListenerList.end();++It)
			{
				if((*It)->OnEvent(vpEvent))
				{
					break;
				}
			}
			return ;
		}
	private:
		std::list<CListener*>	mpListenerList;
	};


	//事件的基类
	//class CEvent;
}














#endif//GAME_LISTENER_H