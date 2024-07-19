


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

	bool IsExist(){return mExist;}													//�Ƿ���
	void Render(RtgDevice& inDevice,RTGRenderMask eMask);				            //��Ⱦ
    void RenderHighLight(RtgCamera& inCamera, RtgDevice& inDevice, RtgVertex3 &color,RtgVertex3 &scale);
	void Run(float vSecond);														//�߼�����

    GcRenderMissile* GetRender()    { return &m_Render; }
    RtgMatrix16& GetMatrix()        { return mCurrentMatrix; }

	GcActorGraph&		Graph(){return mGraph;};		//�õ�ͼ���ָ��

protected:
	RtgMatrix16					mCurrentMatrix;			//��ǰλ�úͷ���[���ฺ����ǰ��λ�úͷ���]
    DWORD                       m_dwParameter1;         // ����1
    DWORD                       m_dwParameter2;         // ����2
    DWORD                       m_dwParameter3;         // ����3

	void Arrive();										//�����¼�[�����ڵ���Ŀ�ĵ�����]
	void DeleteSelf(){mExist=false;};					//�ս��Լ�������
	void UpdateGraph();									//����ͼ���λ�úͷ���[������λ�ñ䶯ʱ����]

    void UpdateBaseColor();

private:
	virtual void OnRun(float vSecond)=0;				//���಻ͬ���߼�����[�����������]

	virtual void OnArrive(){};							//�����¼�[����������ظú���]
	virtual void OnTimeOut(){};							//����ʱ��[����������ظú���]
	virtual void OnSideOut(){};							//��������[����������ظú���]


	void TimeOut();										//����ʱ��
	void SideOut();										//��������

	
	
	bool						mExist;					//�Ƿ���
	float						mExistTime;				//���ʱ��
	float						mMaxDistance;			//��������
	RtgVertex3					mSourcePosition;		//������
	
	CMissileListener*			mpListener;				//�¼��ļ�����ָ��[NULL��ʾû�м�����]
	GcActorGraph				mGraph;					//ͼ�����
    GcRenderMissile             m_Render;				//ͼ����Ⱦ�ӿ�
	
};


//	DWORD						mTargetID;				//Ŀ��ID[IDΪ0��ʾû��Ŀ��]
//	RtgVertex3					mTargetPosition;		//Ŀ���[ֻ��û��Ŀ��IDʱ��Ŀ��������]


/*


// ������, ���������������࣬���п����˶����ӵ�����Ҫ������̳�
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

	ELmVector   m_SourcePos;    // ԭ����λ��

	long        m_lParam1, m_lParam2, m_lParam3;

};
*/


#endif//GC_MISSILE_H