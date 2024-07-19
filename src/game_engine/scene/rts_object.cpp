
#include "scene/rt_scene.h"

RT_IMPLEMENT_DYNAMIC(RtsObject, RtObject, 0, "scene")

  
RtsObject::RtsObject()
    : m_fRenderDistance(0.f),
      m_pObjectLink(NULL),
      m_uRef(1),
      m_fBlendFactor(1.f),
      m_fRadius(1.f),
      m_eObjectSharp(EOS_UNKNOWNOBJECT),
      m_lObjectVersion(OBJECT_VERSION),
      m_bLastAttribFlag(true),
      m_uLastRenderFrame(0)
#if RTS_DRAW_HELPER
      ,m_eShowType(SO_NONE),
      m_Geometry(RtgGeometry(RtGetRender()))
#endif
{
    SetState(Object_State_New);
}

RtsObject::~RtsObject()
{
#if RTS_DRAW_HELPER
    RtGetRender()->DeleteShader(&m_Shader);
#endif
}

void RtsObject::FrameMove(float _timeMill)
{

}

void RtsObject::Render(RtgCamera& inCamera, RtgDevice& inDevice, RTGRenderMask eMask)
{
#if RTS_DRAW_HELPER

    RtgMatrix16 m16;
    
    switch (m_eShowType)
    {
    case SO_NONE:
        break;
    case SO_SPRITE:
        if (g_pScene->m_bDrawHelper && (eMask&RTGRM_NOBLENDOBJ_ONLY))
        {
            m16 = inCamera.m_matBillboard;
            m16.SetRow(3, m_Matrix.Position());
            inDevice.SetRenderState(RTGRS_LIGHTING, FALSE);
            inDevice.SetMatrix(RTGTS_WORLD, &m16);
            inDevice.SetShader(&m_Shader);
            m_Geometry.Render();
            inDevice.RestoreShader(&m_Shader);
        }
        break;
    case SO_BOX:
        if (g_pScene->m_bDrawHelper && (eMask&RTGRM_NOBLENDOBJ_ONLY))
        {
            inDevice.SetMatrix(RTGTS_WORLD, &m_Matrix);
            inDevice.SetRenderState(RTGRS_LIGHTING, FALSE);
            inDevice.SetShader(&m_Shader);
            m_Geometry.Render();
            inDevice.RestoreShader(&m_Shader);
        }
        break;
    case SO_MODEL:
        inDevice.SetMatrix(RTGTS_WORLD, &m_Matrix);
        break;
    }
#else
    inDevice.SetMatrix(RTGTS_WORLD, &m_Matrix);
#endif
}

void RtsObject::RenderHighLight(RtgCamera& inCamera, RtgDevice& inDevice, RtgVertex3 &color,RtgVertex3 &scale)
{
}

long RtsObject::Serialize(RtArchive& ar)
{
   Compatible_Serialize_ObjId(ar, m_Id);

    ar<<m_Matrix
      <<m_bLastAttribFlag;

	m_bLastAttribFlag = true;

    if (ar.IsLoading())
        OnMatrixChanged();

    return true;
}

void RtsObject::GetGridRange(int& iMinX, int& iMinY, int& iMaxX, int& iMaxY)
{
    RtgVertex3 vMin, vMax;
    float fGridWidth = g_fSceneTileWidth*RTS_BLOCK_GRID_SIZE;

    if (IsKindOf(RT_RUNTIME_CLASS(RtsAudio3D)) || IsKindOf(RT_RUNTIME_CLASS(RtsLight)))
    {
        vMin = this->m_Matrix.Position();
        g_pScene->OffsetToMap(vMin.x, vMin.y);
        iMaxX = iMinX = vMin.x/fGridWidth;
        iMaxY = iMinY = vMin.y/fGridWidth;
    }
    else
    {
        Get_World_AABBox()->GetMin(&vMin);
        Get_World_AABBox()->GetMax(&vMax);
        g_pScene->OffsetToMap(vMin.x, vMin.y);
        g_pScene->OffsetToMap(vMax.x, vMax.y);
        iMinX = vMin.x/fGridWidth;
        iMinY = vMin.y/fGridWidth;
        iMaxX = vMax.x/fGridWidth;
        iMaxY = vMax.y/fGridWidth;
    }
}

void RtsObject::GetTileRange(int& iMinX, int& iMinY, int& iMaxX, int& iMaxY)
{
	RtgVertex3 vMin, vMax;
	float fTileWidth = g_fSceneTileWidth;

	if (IsKindOf(RT_RUNTIME_CLASS(RtsAudio3D)) || IsKindOf(RT_RUNTIME_CLASS(RtsLight)))
	{
		vMin = this->m_Matrix.Position();
		g_pScene->OffsetToMap(vMin.x, vMin.y);
		iMaxX = iMinX = vMin.x/fTileWidth;
		iMaxY = iMinY = vMin.y/fTileWidth;
	}
    else
	{
        Get_World_AABBox()->GetMin(&vMin);
        Get_World_AABBox()->GetMin(&vMax);
		g_pScene->OffsetToMap(vMin.x, vMin.y);
		g_pScene->OffsetToMap(vMax.x, vMax.y);
		iMinX = vMin.x/fTileWidth;
		iMinY = vMin.y/fTileWidth;
		iMaxX = vMax.x/fTileWidth;
		iMaxY = vMax.y/fTileWidth;
	}
}

void RtsObject::GetTerrainTileRange(int& iMinX, int& iMinY, int& iMaxX, int& iMaxY)
{
	RtgVertex3 vMin, vMax;
	float fTerrainTileWidth = g_fSceneTerrainTileWidth;

	if (IsKindOf(RT_RUNTIME_CLASS(RtsAudio3D)) || IsKindOf(RT_RUNTIME_CLASS(RtsLight)))
	{
		vMin = this->m_Matrix.Position();
		g_pScene->OffsetToMap(vMin.x, vMin.y);
		iMaxX = iMinX = vMin.x/fTerrainTileWidth;
		iMaxY = iMinY = vMin.y/fTerrainTileWidth;
	}else
	{
        Get_World_AABBox()->GetMin(&vMin);
        Get_World_AABBox()->GetMin(&vMax);
		g_pScene->OffsetToMap(vMin.x, vMin.y);
		g_pScene->OffsetToMap(vMax.x, vMax.y);
		iMinX = vMin.x/fTerrainTileWidth;
		iMinY = vMin.y/fTerrainTileWidth;
		iMaxX = vMax.x/fTerrainTileWidth;
		iMaxY = vMax.y/fTerrainTileWidth;
	}
}

#if RTS_DRAW_HELPER

    void RtsObject::SetShowType(RtgDevice* pDevice, EShowObject eType)
    {
        if (m_eShowType == eType)
            return;

        m_eShowType = eType;

        RtString szTextureName;

        if (this->IsKindOf(RT_RUNTIME_CLASS(RtsLight)))
        {
            szTextureName = "scene/texture/obj_light_bulb";
        }else if (this->IsKindOf(RT_RUNTIME_CLASS(RtsAudio3D)))
        {
            szTextureName = "scene/texture/obj_audio";
        }else
        {
            szTextureName = "scene/texture/obj_unknow";
        }

        switch (m_eShowType)
        {
        case SO_NONE:
            break;
        case SO_SPRITE:
            RtGetRender()->DeleteShader(&m_Shader);
            m_Geometry.SetVertexFormat(RTG_VCT);
            m_Geometry.CreateRect(20.f, 20.f);
            m_Shader.iNumStage = 1;
            m_Shader.bColorKey = TRUE;
            m_Shader.Shaders[0].SetTexture(RtGetRender()->GetTextMgr()->CreateTextureFromFile(szTextureName));
            break;
        case SO_BOX:
            RtGetRender()->DeleteShader(&m_Shader);
            m_Geometry.SetVertexFormat(RTG_VCT);
            m_Geometry.CreateBox(20.f, 20.f, 40.f);
            m_Shader.iNumStage = 1;
            m_Shader.bColorKey = TRUE;
            m_Shader.Shaders[0].SetTexture(RtGetRender()->GetTextMgr()->CreateTextureFromFile(szTextureName));
            break;
        case SO_MODEL:
            break;
        }
    }

    RtsObject::EShowObject RtsObject::GetShowType()
    {
        return m_eShowType;
    }

#endif