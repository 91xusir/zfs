
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
                    :mExist(true),									//是否存活
                    mExistTime(vExistTime),							//最大存活时间[0表示无穷]
                    mMaxDistance(vMaxDistance),						//最大存活距离[0表示无穷]
                    mSourcePosition(vrSourcePosition),				//出发点
                    mpListener(vpListener)							//事件的监听者指针[NULL表示没有监听者]
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

void CMissile::Render(RtgDevice& vrDevice,RTGRenderMask vMask)//渲染
{
	mGraph.Render(vrDevice,vMask,false);
}

void CMissile::RenderHighLight(RtgCamera& inCamera, RtgDevice& inDevice, RtgVertex3 &color,RtgVertex3 &scale)
{
    mGraph.RenderHighLight(inCamera, inDevice,color,scale);
}

void CMissile::Run(float vSecond)												//逻辑运行
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
	{//当前距离同起始距离超过最大距离
		SideOut();		
	}

	OnRun(vSecond);//调用不同的子类的逻辑处理函数

	mGraph.FrameMove(vSecond);
}
void CMissile::Arrive()								//到达事件
{
	OnArrive();//调用子类的处理
	//触发到达事件
	if(mpListener!=NULL)
	{
		mpListener->OnArrive(m_dwParameter1, m_dwParameter2, m_dwParameter3);
	}
}


void CMissile::TimeOut()									//超出时间
{
	OnTimeOut();//调用子类的处理
	//触发到达事件
	if(mpListener!=NULL)
	{
		mpListener->OnTimeOut(m_dwParameter1, m_dwParameter2, m_dwParameter3);
	}
	//销毁自己
	DeleteSelf();

}
void CMissile::SideOut()									//超出距离
{
	OnSideOut();//调用子类的处理
	//触发到达事件
	if(mpListener!=NULL)
	{
		mpListener->OnSideOut(m_dwParameter1, m_dwParameter2, m_dwParameter3);
	}
	//销毁自己
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
