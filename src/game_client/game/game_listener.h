



#ifndef GAME_LISTENER_H
#define GAME_LISTENER_H
typedef unsigned long       DWORD;      


//�û���ʹ��CGameListenerManager


//����಻������ʹ���߼̳�
class CGameListener                 //������¼����ӽӿ���
{
public:
	CGameListener				(){};
	virtual ~CGameListener      (){};

	virtual bool OnEvent(DWORD vDBID,const char* vpName)=0;			//�������true���¼�����ȡ
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



//�û��ӿ�
class CGameListenerManager
{
public:
	CGameListenerManager(){};
	~CGameListenerManager()
	{
		//��Listenerɾ��
		std::list<CGameListener*>::iterator It;
		for(It=mpListenerList.begin();It!=mpListenerList.end();It++)
		{
			DEL_ONE(*It);
		}
	};
	
	//���Ӽ�����<����,����ķ���>//����true��ʾ���¼����ػ�[���ٷ�������������]
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
	//�¼��Ļ���
	//class CEvent;

	//��Ϸ�ļ����ߵĻ���
	class CListener
	{
	public:
		CListener(){};
		virtual ~CListener(){};

		virtual bool OnEvent(void* vpEvent)=0;			//�������true���¼�����ȡ
		virtual bool	operator == (void* vpMaster)=0;	//�����Ƿ���ͬ
	};

	//����ļ�����
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


	
	//�����Ĺ�����//�û��Ľӿ�
	class CListenerManager
	{
	public:

		CListenerManager(){};
		~CListenerManager()
		{
			//��Listenerɾ��
			std::list<CListener*>::iterator It;
			for(It=mpListenerList.begin();It!=mpListenerList.end();++It)
			{
				DEL_ONE(*It);
			}
		};

		//���Ӽ�����<���ͣ�����ķ���>//����true��ʾ���¼����ػ�[���ٷ�������������]
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


	//�¼��Ļ���
	//class CEvent;
}














#endif//GAME_LISTENER_H