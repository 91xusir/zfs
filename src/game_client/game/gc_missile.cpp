
#include "gc_include.h"
#include "gc_missile.h"
#include "gc_missile_manager.h"
#include "gc_missile_listener.h"
#include "game_assistant.h"


using std::string;


//
//	class GcRenderMissile
//

RT_IMPLEMENT_DYNAMIC(GcRenderMissile, RtsRenderAgent, 0, "gc")
void GcRenderMissile::Render(RtgCamera& inCamera, RtgDevice& inDevice, RTGRenderMask eMask)
{
    m_pGraph->Render(inDevice, eMask);
}

void GcRenderMissile::RenderHighLight(RtgCamera& inCamera, RtgDevice& inDevice, RtgVertex3 &color,RtgVertex3 &scale)
{
    m_pGraph->RenderHighLight(inCamera, inDevice, color, scale);
}

//
//	class CMissile
//

CMissile::CMissile(RtgVertex3& vrSourcePosition,char vpGraphName[],CMissileListener* vpListener,
					float vExistTime,float vMaxDistance)
                    :mExist(true),									//�Ƿ���
                    mExistTime(vExistTime),							//�����ʱ��[0��ʾ����]
                    mMaxDistance(vMaxDistance),						//��������[0��ʾ����]
                    mSourcePosition(vrSourcePosition),				//������
                    mpListener(vpListener)							//�¼��ļ�����ָ��[NULL��ʾû�м�����]
{
    m_dwParameter1 = 0;
    m_dwParameter2 = 0;
    m_dwParameter3 = 0;

    mGraph.LoadActor(vpGraphName);
    mGraph.DisplayActor(true);
    mGraph.PlayPose("effect", true);
    m_Render.SetGraph(this);
	
	mCurrentMatrix.Unit();
	mCurrentMatrix._30=vrSourcePosition.x;
	mCurrentMatrix._31=vrSourcePosition.y;
	mCurrentMatrix._32=vrSourcePosition.z;

    UpdateBaseColor();

	if (GetWorld())
    {
        GetWorld()->m_pMissileManager->Add(this);
	}
	if (g_pScene)
	{
		g_pScene->m_camMatPrev.Unit();
	}
}

CMissile::~CMissile()
{
}

void CMissile::Destroy()
{
    g_pScene->RemoveObjectByLastRenderList(GetRender());
}

void CMissile::Render(RtgDevice& vrDevice,RTGRenderMask vMask)//��Ⱦ
{
	mGraph.Render(vrDevice,vMask,false);
}

void CMissile::RenderHighLight(RtgCamera& inCamera, RtgDevice& inDevice, RtgVertex3 &color,RtgVertex3 &scale)
{
    mGraph.RenderHighLight(inCamera, inDevice,color,scale);
}

void CMissile::Run(float vSecond)												//�߼�����
{
	if(!mExist)
	{
		return ;
	}

	mExistTime-=vSecond;
	
	if(mExistTime<=0.0f)
	{
		TimeOut();
	}

	if(DistanceSquare(mCurrentMatrix._30,mCurrentMatrix._31,mCurrentMatrix._32,
		mSourcePosition.x,mSourcePosition.y,mSourcePosition.z)>=mMaxDistance*mMaxDistance)
	{//��ǰ����ͬ��ʼ���볬��������
		SideOut();		
	}

	OnRun(vSecond);//���ò�ͬ��������߼�������

	mGraph.FrameMove(vSecond);
}
void CMissile::Arrive()								//�����¼�
{
	OnArrive();//��������Ĵ���
	//���������¼�
	if(mpListener!=NULL)
	{
		mpListener->OnArrive(m_dwParameter1, m_dwParameter2, m_dwParameter3);
	}
}


void CMissile::TimeOut()									//����ʱ��
{
	OnTimeOut();//��������Ĵ���
	//���������¼�
	if(mpListener!=NULL)
	{
		mpListener->OnTimeOut(m_dwParameter1, m_dwParameter2, m_dwParameter3);
	}
	//�����Լ�
	DeleteSelf();

}
void CMissile::SideOut()									//��������
{
	OnSideOut();//��������Ĵ���
	//���������¼�
	if(mpListener!=NULL)
	{
		mpListener->OnSideOut(m_dwParameter1, m_dwParameter2, m_dwParameter3);
	}
	//�����Լ�
	DeleteSelf();
}



void CMissile::UpdateGraph()
{
	mGraph.SetPosition(mCurrentMatrix._30,mCurrentMatrix._31,mCurrentMatrix._32);
	mGraph.SetRotation(mCurrentMatrix);
}

void CMissile::UpdateBaseColor()
{
    RtgVertex3 vColor(1.f, 1.f, 1.f);
    if (g_pScene)
    {
        g_pScene->GetTerrainColor(mCurrentMatrix._30, mCurrentMatrix._31, vColor);
        if(vColor.x < 0.3) vColor.x = 0.3;
        if(vColor.y < 0.3) vColor.y = 0.3;
        if(vColor.z < 0.3) vColor.z = 0.3;
    }
    mGraph.SetBaseColor(vColor);
}
