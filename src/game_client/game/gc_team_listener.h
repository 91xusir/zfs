



#ifndef GC_TEAM_LISTENER_H
#define GC_TEAM_LISTENER_H
typedef unsigned long       DWORD;      



//这个类不用来被使用者继承
class CTeamListener                 //队伍的事件监视接口类
{
public:
	CTeamListener				(){};
	virtual ~CTeamListener      (){};

	virtual bool MemberClickByMouseR(DWORD vDBID)=0;			//鼠标右键按下//如果返回true则，事件被截取
};


//使用者应该使用这个模板类
template<class T>
class CConcreteTeamListener:public CTeamListener
{
public:
	CConcreteTeamListener(T* vpT):mpT(vpT){};
	virtual ~CConcreteTeamListener(){};
	virtual bool MemberClickByMouseR(DWORD vDBID)
	{
		return mpT->MemberClickByMouseR(vDBID);
	};
private:
	T* mpT;
};


#endif//GC_TEAM_LISTENER_H