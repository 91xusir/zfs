#include "scene/rt_scene.h"

RT_IMPLEMENT_DYNAMIC(RtsActor, RtsObject, 0, "scene")
RT_IMPLEMENT_DYNCREATE(RtsModel, RtsActor, 0, "scene")

RtsModel::RtsModel()
{
#if RTS_DRAW_HELPER
    SetShowType(RtGetRender(),SO_MODEL);
#endif
    m_pChar = NULL;
    m_fRenderTime = 0.f;
}

RtsModel::~RtsModel()
{
    ReleaseResource();
}

namespace {

    void _ActorNotify(CRT_ActorInstance* _actor, ActorNotifType nttype, void* param0, void* param1)
    {
        RtsModel* model = (RtsModel*)param0;

        if (!model)
            return;

        switch (nttype)
        {
        case Actor_Create_Finish :
            {
                RTASSERT(_actor);
                RTASSERT(model->m_pChar == NULL|| model->m_pChar == _actor);
                model->m_pChar = _actor;

                model->m_pChar->PlayPose("scene", true);

                if (model->m_aryVc.size())
                    model->m_pChar->SetVC(model->m_aryVc);
                
                model->m_pChar->SetMatrix(model->GetMatrix());
                model->m_fRadius = model->Get_AABBox()->vExt.Length();
                model->m_eObjectSharp = RtsObject::EOS_UNKNOWNOBJECT;	

                if (model->m_fRadius > RtsObject::LEVEL_HUGE) 
                    model->m_eObjectSharp = RtsObject::EOS_HUGEOBJECT;
                
                if (model->m_fRadius < RtsObject::LEVEL_HUGE && model->m_fRadius > RtsObject::LEVEL_BIG)
                    model->m_eObjectSharp = RtsObject::EOS_BIGOBJECT;
                
                if (model->m_fRadius > RtsObject::LEVEL_MEDIA && model->m_fRadius < RtsObject::LEVEL_BIG)
                    model->m_eObjectSharp = RtsObject::EOS_MEDIAOBJECT;
                
                if (model->m_fRadius < RtsObject::LEVEL_MEDIA) 
                    model->m_eObjectSharp = RtsObject::EOS_SMALLOBJECT;

                model->SetState(Object_State_Ready);

                g_pScene->OnModelCreateFinish(model, (RtsSGrid*)param1);
            }
            break;
        default :
            break;
        }
    }
}


bool RtsModel::CreateResource(const char fileName[], RtsSGrid* _grid, bool bForceLoad /* = false */)
{
    SetState(Object_State_Loading);

    m_fileName = fileName;
    m_fRenderTime = RtGetRender()->GetAppTime();
 
	char* szFile = (char*)fileName;
    char* t = strrchr(szFile, '.');
    if (t)
        *t = 0;
	m_fileName = szFile;

    m_pChar = RtcGetActorManager()->CreateActor(m_fileName.c_str(), bForceLoad, ActorNotify(_ActorNotify, this, _grid));
    
    if (!m_pChar)
    {
        RtCoreLog().Error("load scene model %s faild\n", m_fileName.c_str());
        return false;
    }

    return true;
}

void RtsModel::ReleaseResource()
{
    if (m_pChar)
    {
        RtcGetActorManager()->ReleaseActor(m_pChar);
        m_pChar = NULL;
    }

     SetState(Object_State_New);
}

void RtsModel::Render(RtgCamera& inCamera, RtgDevice& inDevice, RTGRenderMask eMask)
{
    m_fRenderTime = inDevice.GetAppTime();

    if (m_pChar)
        m_pChar->Render(&inDevice, eMask);

#if RTS_DRAW_HELPER
    if (g_pScene->m_bRenderObjectBoundbox)
    {
        RtgVertex3 v3;
        RtgMatrix16 m16;
        DWORD dwColor = 0xFFFF0000;

        m16.Unit();
        m16.SetRow(3, m_pChar->GetStaticBoundingBox()->vPos);
        v3 = m_pChar->GetStaticBoundingBox()->vExt;
        inDevice.SetMatrix(RTGTS_WORLD, &GetMatrix());
        inDevice.DrawBox(v3, m16, &dwColor);

        RtgVertex3 vMin, vMax;
        dwColor = 0xFF00FF00;
        //GetActualAABB(vMin, vMax);
        Get_World_AABBox()->GetMin(&vMin);
        Get_World_AABBox()->GetMax(&vMax);
        m16.Unit();
        inDevice.SetMatrix(RTGTS_WORLD, &m16);
        inDevice.DrawBox(vMin, vMax, &dwColor);
    }
#endif
    //UnloadOldModel();
}

void RtsModel::RenderHighLight(RtgCamera& inCamera, RtgDevice& inDevice, RtgVertex3 &color,RtgVertex3 &scale)
{
    if (m_pChar)
    {
        m_pChar->RenderHighLight(&inDevice, color, scale);
    }
}

long RtsModel::Serialize(RtArchive& ar)
{
    if (!RtsObject::Serialize(ar)) 
        return FALSE;
    float fScale;
    ar << m_fileName;
    ar << fScale;
    ar << m_aryVc;
    return TRUE;
}

//void RtsModel::UpdateModelAABB()
//{
//    this->m_AABB = *(m_pChar->GetStaticBoundingBox());
//}
//
void RtsModel::OnMatrixChanged()
{
    if (m_pChar)
        m_pChar->SetMatrix(GetMatrix());
}

void RtsModel::UpdateThreadInstance()
{
 //   SJob job;
	//while(ACTOR()->GetFinishedJob(job))
	//{
	//	if(job.pObject->IsKindOf(RT_RUNTIME_CLASS(RtsModel)))
	//		((RtsModel*)(job.pObject))->Onload((CRT_ActorInstance*)job.pOut);
	//}
}

/*------------------------------------------------------------------------
The End.
------------------------------------------------------------------------*/
