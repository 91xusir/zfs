



#ifndef GC_MISSILE_LISTENER_H
#define GC_MISSILE_LISTENER_H

class CMissileListener                 //导弹的事件的监视听类
{
public:
	CMissileListener(){};
	virtual ~CMissileListener(){};

	//virtual void OnDepart();			//导弹出发时刻[产生导弹的时刻就是该时刻]
	virtual void OnTimeOut(DWORD dwParam1, DWORD dwParam2, DWORD dwParam3){};			//时间到时刻
	virtual void OnSideOut(DWORD dwParam1, DWORD dwParam2, DWORD dwParam3){};			//超出距离时刻
	virtual void OnArrive(DWORD dwParam1, DWORD dwParam2, DWORD dwParam3){};			//到达目标时刻
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