


#ifndef GC_MISSILE_H 
#define GC_MISSILE_H
#include "graph/rt_graph.h"
#include "gc_actor_graph.h"
#include <string>

const float MISSILE_MAX_EXIST_TIME=60000.0f;
const float MISSILE_MAX_DISTANCE=4000.0f;

class CMissile;
class CMissileListener;

//
//	class GcRenderMissile
//

class GcRenderMissile : public RtsRenderAgent
{
    RT_DECLARE_DYNAMIC(GcRenderMissile, RtsRenderAgent, 0, "gc")
public:
    GcRenderMissile(){}
    virtual ~GcRenderMissile(){}
    void SetGraph(CMissile* pGraph)     { m_pGraph = pGraph;}
    virtual void Render(RtgCamera& inCamera, RtgDevice& inDevice, RTGRenderMask eMask);
    virtual void RenderHighLight(RtgCamera& inCamera, RtgDevice& inDevice, RtgVertex3 &color,RtgVertex3 &scale);
private:
    CMissile* m_pGraph;
};

//
//	class CMissile
//


class CMissile
{
public:
	CMissile(
        RtgVertex3& vrSourcePosition,
        char vpGraphName[],
        CMissileListener* vpListener=NULL,
        float vExistTime=MISSILE_MAX_EXIST_TIME,
        float vMaxDistance=MISSILE_MAX_DISTANCE);

    virtual ~CMissile();
    void Destroy();

    void SetParameter(DWORD dwParam1=0, DWORD dwParam2=0, DWORD dwParam3=0)
    {
        m_dwParameter1 = dwParam1;
        m_dwParameter2 = dwParam2;
        m_dwParameter3 = dwParam3;
    }

    void GetParameter(DWORD& dwParam1, DWORD& dwParam2, DWORD& dwParam3)
    {
        dwParam1 = m_dwParameter1;
        dwParam2 = m_dwParameter2;
        dwParam3 = m_dwParameter3;
    }

	bool IsExist(){return mExist;}													//是否存活
	void Render(RtgDevice& inDevice,RTGRenderMask eMask);				            //渲染
    void RenderHighLight(RtgCamera& inCamera, RtgDevice& inDevice, RtgVertex3 &color,RtgVertex3 &scale);
	void Run(float vSecond);														//逻辑运行

    GcRenderMissile* GetRender()    { return &m_Render; }
    RtgMatrix16& GetMatrix()        { return mCurrentMatrix; }

	GcActorGraph&		Graph(){return mGraph;};		//得到图象的指针

protected:
	RtgMatrix16					mCurrentMatrix;			//当前位置和方向[子类负责处理当前的位置和方向]
    DWORD                       m_dwParameter1;         // 参数1
    DWORD                       m_dwParameter2;         // 参数2
    DWORD                       m_dwParameter3;         // 参数3

	void Arrive();										//到达事件[子类在到达目的点后调用]
	void DeleteSelf(){mExist=false;};					//终结自己的生命
	void UpdateGraph();									//更新图象的位置和方向[子类在位置变动时调用]

    void UpdateBaseColor();

private:
	virtual void OnRun(float vSecond)=0;				//子类不同的逻辑运行[子类必须重载]

	virtual void OnArrive(){};							//到达事件[子类可以重载该函数]
	virtual void OnTimeOut(){};							//超出时间[子类可以重载该函数]
	virtual void OnSideOut(){};							//超出距离[子类可以重载该函数]


	void TimeOut();										//超出时间
	void SideOut();										//超出距离

	
	
	bool						mExist;					//是否存活
	float						mExistTime;				//存活时间
	float						mMaxDistance;			//最大存活距离
	RtgVertex3					mSourcePosition;		//出发点
	
	CMissileListener*			mpListener;				//事件的监听者指针[NULL表示没有监听者]
	GcActorGraph				mGraph;					//图象表现
    GcRenderMissile             m_Render;				//图象渲染接口
	
};


//	DWORD						mTargetID;				//目标ID[ID为0表示没有目标]
//	RtgVertex3					mTargetPosition;		//目标点[只有没有目标ID时，目标点才有用]


/*


// 发射物, 用来发射武器的类，所有空中运动的子弹都需要从这里继承
class ELMissile
{
public:
	ELMissile(ELEmitter* pEmitter, ELScene* pScene);
	virtual ~ELMissile();

	virtual void OnStop(){};
	void Stop();


	EL_BOOL IsRun()                 { return m_bRun;}
	ELNode& Body()                  { return m_Body;}
	const ELmVector GetSourcePos()  { return m_SourcePos;}
	ELNode* GetTarget()             { return m_pTarget;}
	ELEmitter* GetEmitter()         { return m_pEmitter;}
	const ELmVector GetTargetPos()
	{
		if (m_pTarget) return m_pTarget->Position();
		return m_TargetPos;
	}

protected:

	ELmVector   m_SourcePos;    // 原来的位置

	long        m_lParam1, m_lParam2, m_lParam3;

};
*/


#endif//GC_MISSILE_H