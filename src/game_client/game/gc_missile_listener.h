



#ifndef GC_MISSILE_LISTENER_H
#define GC_MISSILE_LISTENER_H

class CMissileListener                 //�������¼��ļ�������
{
public:
	CMissileListener(){};
	virtual ~CMissileListener(){};

	//virtual void OnDepart();			//��������ʱ��[����������ʱ�̾��Ǹ�ʱ��]
	virtual void OnTimeOut(DWORD dwParam1, DWORD dwParam2, DWORD dwParam3){};			//ʱ�䵽ʱ��
	virtual void OnSideOut(DWORD dwParam1, DWORD dwParam2, DWORD dwParam3){};			//��������ʱ��
	virtual void OnArrive(DWORD dwParam1, DWORD dwParam2, DWORD dwParam3){};			//����Ŀ��ʱ��
};
/*
class CActorListener:public class CMissileListener () 
{
public:
	CActorListener(CBaseActor* vpMaster):CMissileListener(),mpMaster(vpMaster){};
	virtual ~CActorListener();

	virtual void OnArrive();
private:
	CBaseActor* mpMaster;
};
*/
#endif//GC_MISSILE_LISTENER_H