
#include "scene/rt_scene.h"

RtsFootPrint::RtsFootPrint()
{
    return;
    m_iCount = 0;
    m_iMax = 0;
    m_pHead = NULL;
    m_pUnused = NULL;
    m_pArray = NULL;
    m_pPanes = NULL;
    m_fTime = 0.f;
    m_fLifecycle = 3.0f;
    m_bInit = FALSE;

    const float fFootPrintSize = 3.f;
    m_pPaneVertex[0].Set(0.f, -fFootPrintSize, -fFootPrintSize);
    m_pPaneVertex[1].Set(0.f, -fFootPrintSize,  fFootPrintSize);
    m_pPaneVertex[2].Set(0.f,  fFootPrintSize,  fFootPrintSize);
    m_pPaneVertex[3].Set(0.f,  fFootPrintSize,  fFootPrintSize);
    m_pPaneVertex[4].Set(0.f,  fFootPrintSize, -fFootPrintSize);
    m_pPaneVertex[5].Set(0.f, -fFootPrintSize, -fFootPrintSize);
}

RtsFootPrint::~RtsFootPrint()
{
    return;
    CHECK(m_bInit==FALSE);
}

void RtsFootPrint::InitOnce(RtgDevice* pDevice, int iMax)
{
    m_pDevice = pDevice;
    return;
    m_bInit = TRUE;

    int i;
    m_iMax = iMax;
    m_iCount = 0;
    m_pArray = RT_NEW SFootPrintNode[m_iMax];
    m_pArray->pNext = NULL;
    for (i=1; i<iMax; i++)
    {
        m_pArray[i].pNext = m_pArray+(i-1);
    }
    m_pUnused = m_pArray + (iMax-1);

    m_pPanes = RT_NEW SFootPrintPane[m_iMax*6];
    for (i=0; i<iMax; i++)
    {
        m_pPanes[i*6+0].t.Set(1.f, 0.f);
        m_pPanes[i*6+1].t.Set(0.f, 0.f);
        m_pPanes[i*6+2].t.Set(0.f, 1.f);
        m_pPanes[i*6+3].t.Set(0.f, 1.f);
        m_pPanes[i*6+4].t.Set(1.f, 1.f);
        m_pPanes[i*6+5].t.Set(1.f, 0.f);
    }

    m_Shader.iNumStage = 1;
    m_Shader.Shaders[0].SetTexture(RtGetRender()->GetTextMgr()->CreateTextureFromFile("FootPrint"));   
    // m_Shader.Shaders[0].SetTexture(m_pDevice->m_Texture.CreateTexture("FootPrint", RTGTBT_AUTO, true, 1));
    m_Shader.bEnableBlendFunc = true;
    m_Shader.eBlendSrcFunc = RTGBLEND_ZERO;
    m_Shader.eBlendDstFunc = RTGBLEND_INVSRCCOLOR;
}

void RtsFootPrint::ClearOnce()
{
    return;
    CHECK(m_bInit==TRUE);
    m_pDevice->DeleteShader(&m_Shader);
    DEL_ARRAY(m_pArray);
    DEL_ARRAY(m_pPanes);
    m_bInit = FALSE;
}

void RtsFootPrint::Add(RtgMatrix16& m16, DWORD dwSize, DWORD dwType)
{
    return;
    CHECK(m_bInit==TRUE);
    if (m_pUnused==NULL)
    {   // 如果已经没有空间了，就把最后一个挤掉
        m_pUnused = m_pHead;
        m_pHead = m_pHead->pNext;
        m_pUnused->pNext = NULL;
    }

    m_pUnused->m16 = m16;
    //m_pUnused->m16.SetRow(3, pPos);
    m_pUnused->fTime = m_fTime + m_fLifecycle;

    // 添加到尾部
    if (m_pHead)
    {
        SFootPrintNode* pNode = m_pHead;
        while (pNode->pNext)
        {
            pNode = pNode->pNext;
        }
        pNode->pNext = m_pUnused;
        m_pUnused = m_pUnused->pNext;
        pNode->pNext->pNext = NULL;
    }else
    {
        m_pHead = m_pUnused;
        m_pUnused = m_pUnused->pNext;
        m_pHead->pNext = NULL;
    }
}

void RtsFootPrint::Run(float fDifSecond)
{
    return;
    CHECK(m_bInit==TRUE);
    m_fTime += fDifSecond;

    SFootPrintNode *p1, *p2;
    for (p1 = m_pHead; p1; p1 = p2)
    {
        p2 = p1->pNext;
        if (p1->fTime < m_fTime)
        {
            m_pHead = p1->pNext;
            p1->pNext = m_pUnused;
            m_pUnused = p1;
        }else
        {
            break;
        }
    }
}

void RtsFootPrint::Render(RtgCamera& inCamera, RTGRenderMask eMask)
{
    return;
    CHECK(m_bInit==TRUE);
    int i=0;
    SFootPrintNode *pNode;
    for (pNode=m_pHead; pNode; pNode=pNode->pNext)
    {
        // inDevice.DrawAxis(pNode->m16, 100.f);
        m_pPanes[i++].v = m_pPaneVertex[0] * pNode->m16;
        m_pPanes[i++].v = m_pPaneVertex[1] * pNode->m16;
        m_pPanes[i++].v = m_pPaneVertex[2] * pNode->m16;
        m_pPanes[i++].v = m_pPaneVertex[3] * pNode->m16;
        m_pPanes[i++].v = m_pPaneVertex[4] * pNode->m16;
        m_pPanes[i++].v = m_pPaneVertex[5] * pNode->m16;
    }
    if (i)
    {
        m_pDevice->SetShader(&m_Shader);
        m_pDevice->SetVertexFormat(RTG_VT);
        m_pDevice->DrawPrimitiveUP(D3DPT_TRIANGLELIST, m_pPanes, sizeof(SFootPrintPane), i/3);
        m_pDevice->RestoreShader(&m_Shader);
    }
}

/*------------------------------------------------------------------------
-   The End.
------------------------------------------------------------------------*/
