
//////////////////////////////////////////////////////////////////////////
//
//   FileName : rts_object_render.cpp
//     Author : zxy
// CreateTime : 2010.03.12
//       Desc :
//
//////////////////////////////////////////////////////////////////////////

#include "graph/rt_graph.h"
#include "rt_scene.h"
#include "rts_object_render.h"

RtsObjectRender::RtsObjectRender()
{
    m_objArray.alloc(Object_Array_Init_Size);
    m_objNum = 0;
}

RtsObjectRender::~RtsObjectRender()
{
}

void RtsObjectRender::AddObject(RtsObject* _obj)
{
    if (!_obj)
        return;

    if (m_objNum >= m_objArray.size())
        m_objArray.resize(m_objArray.size() + Object_Array_Gain_Size);
    m_objArray[m_objNum++] = _obj;
    ++RtGetPref()->RenderObj_Num;
}

void RtsObjectRender::DelObject(RtsObject* _obj)
{
    if (!_obj)
        return;

    for (int i = 0; i < m_objNum; ++i)
    {
        if (m_objArray[i] == _obj)
        {
            m_objArray[i] = m_objArray[--m_objNum];
            --RtGetPref()->RenderObj_Num;
            break;
        }
    }

}

void RtsObjectRender::Reset()
{
    m_objNum = 0;
    RtGetPref()->RenderObj_Num = 0;
}

void RtsObjectRender::ProcessDynamicLight(RtsDynamicLight* light)
{
    RTASSERT(light);

    for (int i = 0; i < m_objNum; ++i)
    {
        if (m_objArray[i]->IsKindOf(RT_RUNTIME_CLASS(RtsActor)))
        {
      
        }
    }
}

void RtsObjectRender::Update(float elapse)
{
    for (int i = 0; i < m_objNum; ++i)
        if (m_objArray[i])
            m_objArray[i]->FrameMove(elapse * 1000.f);
}

void RtsObjectRender::PrepareRender(RtgCamera* cam)
{
    struct SortObject {        
        inline bool operator() (RtsObject* obj0, RtsObject* obj1) const {
            return obj0->m_fRenderDistance < obj1->m_fRenderDistance;
        }
    };

    for (int i = 0; i < m_objNum; ++i)
        m_objArray[i]->m_fRenderDistance = rtgDistanceSqr(cam->m_vEyePt, m_objArray[i]->GetMatrix().Position());     
    m_objArray.sort<SortObject>(m_objNum);

}

void RtsObjectRender::RenderSceneObjectNormal(RtScene* scene, RtgCamera* cam, RTGRenderMask _Mask)
{
    RTASSERT(scene);
    RTASSERT(cam);

    RtgDevice* dev = scene->GetDevice();

    DWORD dwLight = dev->GetRenderState(RTGRS_LIGHTING);
    DWORD dwEmiSc = dev->GetRenderState(RTGRS_EMISSIVEMATERIALSOURCE);
    DWORD dwDifSc = dev->GetRenderState(RTGRS_DIFFUSEMATERIALSOURCE);

    if (_Mask & RTGRM_NOBLENDOBJ_ONLY)
    {
        for (int i = 0; i < m_objNum; ++i)
        {
            RtsObject* _obj = m_objArray[i];

            if (!_obj)
                continue;

            if (scene->m_bUseDynamicLight)
            {
                if (_obj->IsKindOf(RT_RUNTIME_CLASS(RtsActor)))
                {
                    int iLight = 0;
                    for (; iLight < RtsActor::EADL_MAX_DYNAMIC_LIGHT && ((RtsActor*)_obj)->m_pLights[iLight].pLight; 
                        ++iLight)
                    {
                        dev->SetLight(iLight + scene->m_iCustomLightCount+1, ((RtsActor*)_obj)->m_pLights[iLight].pLight);
                    }
                    dev->SetLight(iLight + scene->m_iCustomLightCount + 1, NULL);
                }
                else
                {
                    dev->SetLight(scene->m_iCustomLightCount + 1, NULL);
                }
            }

            _obj->Render(*cam, *dev, RTGRM_NOBLENDOBJ_ONLY);
        }
    }

    if (_Mask & RTGRM_BLENDOBJ_ONLY)
    {
        for (int i = m_objNum - 1; i >= 0; --i)
        {
            RtsObject* _obj = m_objArray[i];

            if (!_obj)
                continue;

            if (scene->m_bUseDynamicLight)
            {
                if (_obj->IsKindOf(RT_RUNTIME_CLASS(RtsActor)))
                {
                    int iLight = 0;
                    for (; iLight < RtsActor::EADL_MAX_DYNAMIC_LIGHT && ((RtsActor*)_obj)->m_pLights[iLight].pLight; 
                        ++iLight)
                    {
                        dev->SetLight(iLight + scene->m_iCustomLightCount+1, ((RtsActor*)_obj)->m_pLights[iLight].pLight);
                    }
                    dev->SetLight(iLight + scene->m_iCustomLightCount + 1, NULL);
                }
                else
                {
                    dev->SetLight(scene->m_iCustomLightCount + 1, NULL);
                }
            }
            _obj->Render(*cam, *dev, RTGRM_BLENDOBJ_ONLY);
        }
    }

    dev->SetRenderState(RTGRS_LIGHTING, dwLight);
    dev->SetRenderState(RTGRS_EMISSIVEMATERIALSOURCE, dwEmiSc);
    dev->SetRenderState(RTGRS_DIFFUSEMATERIALSOURCE, dwDifSc);
}

void RtsObjectRender::RenderSceneObjectShadow(RtScene* scene, RtgCamera* cam)
{
    RTASSERT(scene);
    RTASSERT(cam);

    RtgDevice* dev = scene->GetDevice();

    DWORD dwLight = dev->GetRenderState(RTGRS_LIGHTING);
    DWORD dwEmiSc = dev->GetRenderState(RTGRS_EMISSIVEMATERIALSOURCE);
    DWORD dwDifSc = dev->GetRenderState(RTGRS_DIFFUSEMATERIALSOURCE);

    dev->SetRenderState(RTGRS_LIGHTING, TRUE);
    dev->SetRenderState(RTGRS_EMISSIVEMATERIALSOURCE, RTGMCS_MATERIAL);
    dev->SetRenderState(RTGRS_DIFFUSEMATERIALSOURCE, RTGMCS_MATERIAL);

    for (int i = 0; i < m_objNum; ++i)
    {
        RtsObject* _obj = m_objArray[i];

        if (!_obj)
            continue;

        if (_obj->IsKindOf(RT_RUNTIME_CLASS(RtsRenderAgentShadow)))
            ((RtsRenderAgentShadow*)_obj)->AgentRenderShader();
    }

    dev->SetRenderState(RTGRS_LIGHTING, dwLight);
    dev->SetRenderState(RTGRS_EMISSIVEMATERIALSOURCE, dwEmiSc);
    dev->SetRenderState(RTGRS_DIFFUSEMATERIALSOURCE, dwDifSc);
}

void RtsObjectRender::RenderSceneMirrorObject(RtScene* scene, RtgCamera* cam)
{
    RTASSERT(scene);

    RtgDevice* dev = scene->GetDevice();

    DWORD dwLight = dev->GetRenderState(RTGRS_LIGHTING);
    DWORD dwEmiSc = dev->GetRenderState(RTGRS_EMISSIVEMATERIALSOURCE);
    DWORD dwDifSc = dev->GetRenderState(RTGRS_DIFFUSEMATERIALSOURCE);

    dev->SetRenderState(RTGRS_LIGHTING, TRUE);
    dev->SetRenderState(RTGRS_EMISSIVEMATERIALSOURCE, RTGMCS_MATERIAL);
    dev->SetRenderState(RTGRS_DIFFUSEMATERIALSOURCE, RTGMCS_MATERIAL);

    RtgVertex4 vPlane(0, 0, 1, 0);
    RtgMatrix16 vReflect;
    RtgMatrix16 mViewMatrix = dev->m_pCamera->m_matView;
    BOOL bFog = dev->GetFogEnable();

    if (bFog) 
        dev->SetFogEnable(FALSE);
    dev->SetRenderState(RTGRS_Z_TEST, FALSE);

    for (int i = 0; i < m_objNum; ++i)
    {
        RtsObject* _obj = m_objArray[i];

        if (!_obj)
            continue;

        if (_obj->m_pObjectLink)
        {
            if (_obj->IsKindOf(RT_RUNTIME_CLASS(RtsActor)))
            {
                if (((RtsActor*)_obj)->GetActor())
                {
                    RtgVertex3 vPos(&(((RtsActor*)_obj)->GetActor()->GetWorldMatrix()->_30));

                    if (scene->m_bUseDynamicLight)
                    {
                        if (_obj->IsKindOf(RT_RUNTIME_CLASS(RtsActor)))
                        {
                            int iLight;
                            for (iLight=0; iLight<RtsActor::EADL_MAX_DYNAMIC_LIGHT && ((RtsActor*)_obj)->m_pLights[iLight].pLight; iLight++)
                            {
                                dev->SetLight(iLight+scene->m_iCustomLightCount+1, ((RtsActor*)_obj)->m_pLights[iLight].pLight);
                            }
                            dev->SetLight(iLight+scene->m_iCustomLightCount+1, NULL);
                        }else
                        {
                            dev->SetLight(scene->m_iCustomLightCount+1, NULL);
                        }
                    }
                    vPlane.w = -vPos.z;
                    vReflect.Reflect(vPlane);
                    vReflect = vReflect*mViewMatrix;
                    dev->SetMatrix(RTGTS_VIEW, &vReflect);
                    ((RtsActor*)_obj)->GetActor()->SetVisible(0.3f);
                    _obj->Render(*cam, *dev, RTGRM_ALL);
                    ((RtsActor*)_obj)->GetActor()->SetVisible(1.f);
                }
            }
        }
    }

    if (bFog) 
        dev->SetFogEnable(TRUE);
    dev->SetRenderState(RTGRS_Z_TEST, TRUE);
    dev->SetMatrix(RTGTS_VIEW, &mViewMatrix);

    dev->SetRenderState(RTGRS_LIGHTING, dwLight);
    dev->SetRenderState(RTGRS_EMISSIVEMATERIALSOURCE, dwEmiSc);
    dev->SetRenderState(RTGRS_DIFFUSEMATERIALSOURCE, dwDifSc);
}
