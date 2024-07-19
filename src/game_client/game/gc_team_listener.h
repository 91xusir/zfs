



#ifndef GC_TEAM_LISTENER_H
#define GC_TEAM_LISTENER_H
typedef unsigned long       DWORD;      



//����಻������ʹ���߼̳�
class CTeamListener                 //������¼����ӽӿ���
{
public:
	CTeamListener				(){};
	virtual ~CTeamListener      (){};

	virtual bool MemberClickByMouseR(DWORD vDBID)=0;			//����Ҽ�����//�������true���¼�����ȡ
};


//ʹ����Ӧ��ʹ�����ģ����
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