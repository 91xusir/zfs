
#include "rtg_graph_inter.h"

namespace rt_graph {
/*----------------------------------------------------------------------------
-   Class RtgGeometryNode.
----------------------------------------------------------------------------*/

bool RtgGeometryNode::CopyBufTo(RTGVertexFormat einFormat, BYTE* pVertexBuf, int iVBufSize, BYTE* pIndexBuf, int iIBufSize)
{
    if (!CopyVertexBufTo(einFormat, pVertexBuf, iVBufSize))
        return false;
    return CopyIndexBufTo(pIndexBuf, iIBufSize);
}

bool RtgGeometryNode::CopyVertexBufTo(RTGVertexFormat einFormat, BYTE* pVertexBuf, int iVBufSize)
{
    if (iVBufSize<iVertexCnt*RtgGetVertexFormatSize(eFormat)) return false;
    if (eFormat==einFormat)
    {
        memcpy(pVertexBuf, pVertexBuffer, iVertexCnt*RtgGetVertexFormatSize(eFormat));
    }else
    {
        RtgGeometryNode node;
        node.pVertexBuffer = pVertexBuf;
        for (int i=0; i<iVertexCnt; i++)
        {
            node.SetVertex(i, *GetVertex(i));
            if (eFormat&RTGVF_N && einFormat&RTGVF_N)
            {
                node.SetNormal(i, *GetNormal(i));
            }
            if (eFormat&RTGVF_C && einFormat&RTGVF_C)
            {
                node.SetColor(i, *GetColor(i));
            }
            if (eFormat&RTGVF_T1 && einFormat&RTGVF_T1)
            {
                node.SetTexCoord(i, *(RtgVertex2*)GetTexCoord(i), 0);
            }
            if (eFormat&RTGVF_T2 && einFormat&RTGVF_T2)
            {
                node.SetTexCoord(i, *(RtgVertex2*)GetTexCoord(i), 1);
            }
            if (eFormat&RTGVF_T3 && einFormat&RTGVF_T3)
            {
                node.SetTexCoord(i, *(RtgVertex2*)GetTexCoord(i), 2);
            }
            if (eFormat&RTGVF_T4 && einFormat&RTGVF_T4)
            {
                node.SetTexCoord(i, *(RtgVertex2*)GetTexCoord(i), 3);
            }
        }
    }
    return true;
}

bool RtgGeometryNode::CopyIndexBufTo(BYTE* pIndexBuf, int iIBufSize)
{
    if (iIBufSize<(int)sizeof(short)*iIndexCnt) return false;
    memcpy(pIndexBuf, pIndexBuffer, iIndexCnt*sizeof(short));
    return true;
}

void RtgGeometryNode::SetVertex(int iIdx, RtgVertex3 &v3)
{
    SetVertex(iIdx, v3.x, v3.y, v3.z);
}

void RtgGeometryNode::SetVertex(int iIdx, float x, float y, float z)
{
    static RTGVertexFormat eLastFormat = RTGVF_NULL;
    static int iLastOffset=0, iLastSize;
    if (eLastFormat!=eFormat)
    {
        eLastFormat = eFormat;
        iLastOffset = 0;
        iLastSize = RtgGetVertexFormatSize(eLastFormat);
    }
    float *v = (float *)(pVertexBuffer + iIdx*iLastSize + iLastOffset);
    v[0] = x; v[1] = y; v[2] = z;
}

void RtgGeometryNode::GetVertex(int iIdx, RtgVertex3 &v3)
{
    v3 = *GetVertex(iIdx);
}

RtgVertex3* RtgGeometryNode::GetVertex(int iIdx)
{
    static RTGVertexFormat eLastFormat = RTGVF_NULL;
    static int iLastOffset=0, iLastSize;
    if (eLastFormat!=eFormat)
    {
        eLastFormat = eFormat;
        iLastOffset = 0;
        iLastSize = RtgGetVertexFormatSize(eLastFormat);
    }
    return (RtgVertex3*)(pVertexBuffer + iIdx*iLastSize + iLastOffset);
}

void RtgGeometryNode::SetNormal(int iIdx, RtgVertex3 &v3)
{
    SetNormal(iIdx, v3.x, v3.y, v3.z);
}

void RtgGeometryNode::SetNormal(int iIdx, float x, float y, float z)
{
    if ((eFormat&RTGVF_N)==0) return;
    static RTGVertexFormat eLastFormat = RTGVF_NULL;
    static int iLastOffset=0, iLastSize;
    if (eLastFormat!=eFormat)
    {
        eLastFormat = eFormat;
        iLastOffset = sizeof(float)*3;
        iLastSize = RtgGetVertexFormatSize(eLastFormat);
    }
    float *v = (float *)(pVertexBuffer + iIdx*iLastSize + iLastOffset);
    v[0] = x; v[1] = y; v[2] = z;
}

void RtgGeometryNode::GetNormal(int iIdx, RtgVertex3 &v3)
{
    v3 = *GetNormal(iIdx);
}

RtgVertex3* RtgGeometryNode::GetNormal(int iIdx)
{
    static RTGVertexFormat eLastFormat = RTGVF_NULL;
    static int iLastOffset=0, iLastSize;
    if (eLastFormat!=eFormat)
    {
        eLastFormat = eFormat;
        iLastOffset = sizeof(float)*3;
        iLastSize = RtgGetVertexFormatSize(eLastFormat);
    }
    return (RtgVertex3 *)(pVertexBuffer + iIdx*iLastSize + iLastOffset);
}

void RtgGeometryNode::SetColor(int iIdx, DWORD dwColor)
{
    if ((eFormat&RTGVF_C)==0) return;
    static RTGVertexFormat eLastFormat = RTGVF_NULL;
    static int iLastOffset=0, iLastSize;
    if (eLastFormat!=eFormat)
    {
        eLastFormat = eFormat;
        iLastOffset = (sizeof(float)*3)+((eFormat&RTGVF_N)?(sizeof(float)*3):0);
        iLastSize = RtgGetVertexFormatSize(eLastFormat);
    }
    *((DWORD*)(pVertexBuffer + iIdx*iLastSize + iLastOffset)) = dwColor;
}

void RtgGeometryNode::SetColor(int iIdx, float r, float g, float b, float a)
{
    SetColor(iIdx, RtgGetColor(r*255.f,g*255.f,b*255.f,a*255.f));
}

DWORD* RtgGeometryNode::GetColor(int iIdx)
{
    static RTGVertexFormat eLastFormat = RTGVF_NULL;
    static int iLastOffset=0, iLastSize;
    if (eLastFormat!=eFormat)
    {
        eLastFormat = eFormat;
        iLastOffset = (sizeof(float)*3)+((eFormat&RTGVF_N)?(sizeof(float)*3):0);
        iLastSize = RtgGetVertexFormatSize(eLastFormat);
    }
    return (DWORD *)(pVertexBuffer + iIdx*iLastSize + iLastOffset);
}

void RtgGeometryNode::SetTexCoord(int iIdx, RtgVertex2 &v2, int iChannel)
{
    SetTexCoord(iIdx, v2.x, v2.y, iChannel);
}

void RtgGeometryNode::SetTexCoord(int iIdx, float u, float v, int iChannel)
{
    if (iChannel==0 && (eFormat&RTGVF_T1)==0) return;
    if (iChannel==1 && (eFormat&RTGVF_T2)==0) return;
    if (iChannel==2 && (eFormat&RTGVF_T3)==0) return;
    if (iChannel==3 && (eFormat&RTGVF_T4)==0) return;
    static RTGVertexFormat eLastFormat = RTGVF_NULL;
    static int iLastOffset=0, iLastSize;
    if (eLastFormat!=eFormat)
    {
        eLastFormat = eFormat;
        iLastOffset = (sizeof(float)*3) + ((eFormat&RTGVF_N)?(sizeof(float)*3):0) + ((eFormat&RTGVF_C)?sizeof(DWORD):0);
        iLastSize = RtgGetVertexFormatSize(eLastFormat);
    }
    float* p = (float*)(pVertexBuffer + iIdx*iLastSize + iLastOffset + iChannel*sizeof(float)*2);
    p[0] = u;
    p[1] = v;
}

void RtgGeometryNode::GetTexCoord(int iIdx, float &u, float &v, int iChannel)
{
    float* p = GetTexCoord(iIdx);
    u = p[iChannel*2];
    v = p[iChannel*2+1];
}

void RtgGeometryNode::GetTexCoord(int iIdx, RtgVertex2 &v2, int iChannel)
{
    float* p = GetTexCoord(iIdx);
    v2.x = p[iChannel*2];
    v2.y = p[iChannel*2+1];
}

float* RtgGeometryNode::GetTexCoord(int iIdx, int iChannel)
{
    static RTGVertexFormat eLastFormat = RTGVF_NULL;
    static int iLastOffset=0, iLastSize;
    if (eLastFormat!=eFormat)
    {
        eLastFormat = eFormat;
        iLastOffset = (sizeof(float)*3) + ((eFormat&RTGVF_N)?(sizeof(float)*3):0) + ((eFormat&RTGVF_C)?sizeof(DWORD):0);
        iLastSize = RtgGetVertexFormatSize(eLastFormat);
    }
    return (float *)(pVertexBuffer + iIdx*iLastSize + iLastOffset + iChannel*sizeof(float)*2);
}

void RtgGeometryNode::SetIndex(int iIdx, short sValue)
{
    *(short*)(pIndexBuffer + iIdx*sizeof(short)) = sValue;
}

void RtgGeometryNode::GetIndex(int iIdx, short &sValue)
{
    sValue = *(short*)(pIndexBuffer + iIdx*sizeof(short));
}

void RtgGeometryNode::Draw(RtgDevice* pDevice)
{
    pDevice->SetVertexFormat(eFormat);
    pDevice->DrawIndexedPrimitiveUP(
        D3DPT_TRIANGLELIST,
        0,
        iVertexCnt,
        pIndexBuffer,
        pVertexBuffer,
        pDevice->GetCurrentVertexStride(),
        iIndexCnt / 3);
/*
    if(!pDevice->DrawIndexPrimitive(RTG_TRIANGLES, pVertexBuffer, iVertexCnt, pIndexBuffer, iIndexCnt))
    {
        CHECKEX("DrawIndexPrimitive失败");
    */
}

/*----------------------------------------------------------------------------
-   Class RtgGeometryFactory.
----------------------------------------------------------------------------*/

RtgGeometryFactory::RtgGeometryFactory()
{
    m_iPieces = 20;
    m_iStacks = 20;
}

RtgGeometryFactory::~RtgGeometryFactory()
{
}

void RtgGeometryFactory::SetGeometryQuality(int quality)
{
    switch(quality)
    {
    case 0:
        m_iPieces = 10;
        m_iStacks = 10;
        break;
    case 1:
        m_iPieces = 15;
        m_iStacks = 15;
        break;
    case 2:
        m_iPieces = 30;
        m_iStacks = 30;
        break;
    default:
        m_iPieces = 20;
        m_iStacks = 20;
        break;
    }
}

/// 矩形面片
bool RtgGeometryFactory::CreateRect(RTGVertexFormat eFormat, BYTE* pVertexBuf, int iVBufSize, BYTE* pIndexBuf, int iIBufSize, float w, float h, DWORD dwColor, ETexCoordMode eMode)
{
    int iVertexCnt = 4;
    int iFaceCnt = 2;
    int iIndexCnt = 6;
    RtgGeometryNode node;
    node.pVertexBuffer = pVertexBuf;
    node.pIndexBuffer = pIndexBuf;
    node.eFormat = eFormat;
    if (iVBufSize<RtgGetVertexFormatSize(eFormat)*iVertexCnt) return false;
    if (iIBufSize<(int)sizeof(short)*iIndexCnt) return false;

    float fExtX = w/2.0f;
    float fExtY = h/2.0f;

    // Vertex 0
    node.SetVertex  (0, -fExtX, -fExtY, 0.0f);
    node.SetNormal  (0, 0.0f, 0.0f, 1.0f);
    node.SetColor   (0, dwColor);
    node.SetTexCoord(0, 0.0f, 0.0f);
    // vertex 1
    node.SetVertex  (1, fExtX, -fExtY, 0.0f);
    node.SetNormal  (1, 0.0f, 0.0f, 1.0f);
    node.SetColor   (1, dwColor);
    node.SetTexCoord(1, 1.0f, 0.0f);
    // vertex 2
    node.SetVertex  (2, fExtX, fExtY, 0.0f);
    node.SetNormal  (2, 0.0f, 0.0f, 1.0f);
    node.SetColor   (2, dwColor);
    node.SetTexCoord(2, 1.0f, 1.0f);
    // vertex 3
    node.SetVertex  (3,-fExtX, fExtY, 0.0f);
    node.SetNormal  (3, 0.0f, 0.0f, 1.0f);
    node.SetColor   (3, dwColor);
    node.SetTexCoord(3, 0.0f, 1.0f);

    // face 0
    node.SetIndex(0, 0);
    node.SetIndex(1, 1);
    node.SetIndex(2, 2);
    // face 1
    node.SetIndex(3, 0);
    node.SetIndex(4, 2);
    node.SetIndex(5, 3);

    return true;
}

/// 梯形面片
bool RtgGeometryFactory::CreateTrape(RTGVertexFormat eFormat, BYTE* pVertexBuf, int iVBufSize, BYTE* pIndexBuf, int iIBufSize, float t, float b, float h, DWORD dwColor, ETexCoordMode eMode)
{
    int iVertexCnt = 4;
    int iFaceCnt = 2;
    int iIndexCnt = 6;
    RtgGeometryNode node;
    node.pVertexBuffer = pVertexBuf;
    node.pIndexBuffer = pIndexBuf;
    node.eFormat = eFormat;
    if (iVBufSize<RtgGetVertexFormatSize(eFormat)*iVertexCnt) return false;
    if (iIBufSize<(int)sizeof(short)*iIndexCnt) return false;

    float x_top_div = t/2.0f;
    float x_bottom_div = b/2.0f;
    float y_ext_div = h/2.0f;

    // vertex 0
    node.SetVertex  (0, -x_bottom_div, -y_ext_div, 0.0f);
    node.SetNormal  (0, 0.0f, 0.0f, 1.0f);
    node.SetColor   (0, dwColor);
    node.SetTexCoord(0, 0.0f, 0.0f);
    // vertex 1
    node.SetVertex  (1, x_bottom_div, -y_ext_div, 0.0f);
    node.SetNormal  (1, 0.0f, 0.0f, 1.0f);
    node.SetColor   (1, dwColor);
    node.SetTexCoord(1, 1.0f, 0.0f);
    // vertex 2
    node.SetVertex  (2, x_top_div, y_ext_div, 0.0f);
    node.SetNormal  (2, 0.0f, 0.0f, 1.0f);
    node.SetColor   (2, dwColor);
    node.SetTexCoord(2, 1.0f, 1.0f);
    // vertex 3
    node.SetVertex  (3, -x_top_div, y_ext_div, 0.0f);
    node.SetNormal  (3, 0.0f, 0.0f, 1.0f);
    node.SetColor   (3, dwColor);
    node.SetTexCoord(3, 0.0f, 1.0f);

    // face 0
    node.SetIndex(0, 0);
    node.SetIndex(1, 1);
    node.SetIndex(2, 2);
    // face 1
    node.SetIndex(3, 0);
    node.SetIndex(4, 2);
    node.SetIndex(5, 3);
    return true;
}

/// 盒子
bool RtgGeometryFactory::CreateBox(RTGVertexFormat eFormat, BYTE* pVertexBuf, int iVBufSize, BYTE* pIndexBuf, int iIBufSize, float w, float h, float d, DWORD dwColor, ETexCoordMode eMode)
{
    int iVertexCnt = 24;
    int iFaceCnt = 12;
    int iIndexCnt = 36;
    RtgGeometryNode node;
    node.pVertexBuffer = pVertexBuf;
    node.pIndexBuffer = pIndexBuf;
    node.eFormat = eFormat;
    if (iVBufSize<RtgGetVertexFormatSize(eFormat)*iVertexCnt) return false;
    if (iIBufSize<(int)sizeof(short)*iIndexCnt) return false;

    float x_ext_div = w/2.0f;
    float y_ext_div = h/2.0f;
    float z_ext_div = d/2.0f;

    // vertex 0
    node.SetVertex  ( 0, -x_ext_div, -y_ext_div, z_ext_div);
    node.SetNormal  ( 0, 0.0f, 0.0f, 1.0f);
    node.SetColor   ( 0, dwColor);
    node.SetTexCoord( 0, 0.0f, 0.0f);
    // vertex 1
    node.SetVertex  ( 1, x_ext_div, -y_ext_div, z_ext_div);
    node.SetNormal  ( 1, 0.0f, 0.0f, 1.0f);
    node.SetColor   ( 1, dwColor);
    node.SetTexCoord( 1, 1.0f, 0.0f);
    // vertex 2
    node.SetVertex  ( 2, x_ext_div, y_ext_div, z_ext_div);
    node.SetNormal  ( 2, 0.0f, 0.0f, 1.0f);
    node.SetColor   ( 2, dwColor);
    node.SetTexCoord( 2, 1.0f, 1.0f);
    // vertex 3
    node.SetVertex  ( 3, -x_ext_div, y_ext_div, z_ext_div);
    node.SetNormal  ( 3, 0.0f, 0.0f, 1.0f);
    node.SetColor   ( 3, dwColor);
    node.SetTexCoord( 3, 0.0f, 1.0f);
    // back face
    // vertex 4
    node.SetVertex  ( 4, -x_ext_div, -y_ext_div, -z_ext_div);
    node.SetNormal  ( 4, 0.0f, 0.0f, -1.0f);
    node.SetColor   ( 4, dwColor);
    node.SetTexCoord( 4, 1.0f, 0.0f);
    // vertex 5
    node.SetVertex  ( 5, x_ext_div, -y_ext_div, -z_ext_div);
    node.SetNormal  ( 5, 0.0f, 0.0f, -1.0f);
    node.SetColor   ( 5, dwColor);
    node.SetTexCoord( 5, 0.0f, 0.0f);
    // vertex 6
    node.SetVertex  ( 6, x_ext_div, y_ext_div, -z_ext_div);
    node.SetNormal  ( 6, 0.0f, 0.0f, -1.0f);
    node.SetColor   ( 6, dwColor);
    node.SetTexCoord( 6, 0.0f, 1.0f);
    // vertex 7
    node.SetVertex  ( 7, -x_ext_div, y_ext_div, -z_ext_div);
    node.SetNormal  ( 7, 0.0f, 0.0f, -1.0f);
    node.SetColor   ( 7, dwColor);
    node.SetTexCoord( 7, 1.0f, 1.0f);
    // top face
    // vertex 8
    node.SetVertex  ( 8, -x_ext_div, y_ext_div, -z_ext_div);
    node.SetNormal  ( 8, 0.0f, 1.0f, 0.0f);
    node.SetColor   ( 8, dwColor);
    node.SetTexCoord( 8, 0.0f, 1.0f);
    // vertex 9
    node.SetVertex  ( 9, x_ext_div, y_ext_div, -z_ext_div);
    node.SetNormal  ( 9, 0.0f, 1.0f, 0.0f);
    node.SetColor   ( 9, dwColor);
    node.SetTexCoord( 9, 1.0f, 1.0f);
    // vertex 10
    node.SetVertex  (10, x_ext_div, y_ext_div, z_ext_div);
    node.SetNormal  (10, 0.0f, 1.0f, 0.0f);
    node.SetColor   (10, dwColor);
    node.SetTexCoord(10, 1.0f, 0.0f);
    // vertex 11
    node.SetVertex  (11, -x_ext_div, y_ext_div, z_ext_div);
    node.SetNormal  (11, 0.0f, 1.0f, 0.0f);
    node.SetColor   (11, dwColor);
    node.SetTexCoord(11, 0.0f, 0.0f);
    // bottom face
    // vertex 12
    node.SetVertex  (12, -x_ext_div, -y_ext_div, -z_ext_div);
    node.SetNormal  (12, 0.0f, -1.0f, 0.0f);
    node.SetColor   (12, dwColor);
    node.SetTexCoord(12, 0.0f, 0.0f);
    // vertex 13
    node.SetVertex  (13, x_ext_div, -y_ext_div, -z_ext_div);
    node.SetNormal  (13, 0.0f, -1.0f, 0.0f);
    node.SetColor   (13, dwColor);
    node.SetTexCoord(13, 1.0f, 0.0f);
    // vertex 14
    node.SetVertex  (14, x_ext_div, -y_ext_div, z_ext_div);
    node.SetNormal  (14, 0.0f, -1.0f, 0.0f);
    node.SetColor   (14, dwColor);
    node.SetTexCoord(14, 1.0f, 1.0f);
    // vertex 15
    node.SetVertex  (15, -x_ext_div, -y_ext_div, z_ext_div);
    node.SetNormal  (15, 0.0f, -1.0f, 0.0f);
    node.SetColor   (15, dwColor);
    node.SetTexCoord(15, 0.0f, 1.0f);
    // left face
    // vertex 16
    node.SetVertex  (16, -x_ext_div, -y_ext_div, -z_ext_div);
    node.SetNormal  (16, -1.0, 0.0f, 0.0f);
    node.SetColor   (16, dwColor);
    node.SetTexCoord(16, 0.0f, 0.0f);
    // vertex 17
    node.SetVertex  (17, -x_ext_div, -y_ext_div, z_ext_div);
    node.SetNormal  (17, -1.0, 0.0f, 0.0f);
    node.SetColor   (17, dwColor);
    node.SetTexCoord(17, 1.0f, 0.0f);
    // vertex 18
    node.SetVertex  (18, -x_ext_div, y_ext_div, z_ext_div);
    node.SetNormal  (18, -1.0, 0.0f, 0.0f);
    node.SetColor   (18, dwColor);
    node.SetTexCoord(18, 1.0f, 1.0f);
    // vertex 19
    node.SetVertex  (19, -x_ext_div, y_ext_div, -z_ext_div);
    node.SetNormal  (19, -1.0, 0.0f, 0.0f);
    node.SetColor   (19, dwColor);
    node.SetTexCoord(19, 0.0f, 1.0f);
    // right face
    // vertex 20
    node.SetVertex  (20, x_ext_div, -y_ext_div, -z_ext_div);
    node.SetNormal  (20, 1.0, 0.0f, 0.0f);
    node.SetColor   (20, dwColor);
    node.SetTexCoord(20, 0.0f, 0.0f);
    // vertex 21
    node.SetVertex  (21, x_ext_div, -y_ext_div, z_ext_div);
    node.SetNormal  (21, 1.0, 0.0f, 0.0f);
    node.SetColor   (21, dwColor);
    node.SetTexCoord(21, 0.0f, 1.0f);
    // vertex 22
    node.SetVertex  (22, x_ext_div, y_ext_div, z_ext_div);
    node.SetNormal  (22, 1.0, 0.0f, 0.0f);
    node.SetColor   (22, dwColor);
    node.SetTexCoord(22, 1.0f, 1.0f);
    // vertex 23
    node.SetVertex  (23, x_ext_div, y_ext_div, -z_ext_div);
    node.SetNormal  (23, 1.0, 0.0f, 0.0f);
    node.SetColor   (23, dwColor);
    node.SetTexCoord(23, 1.0f, 0.0f);

    // front face
    node.SetIndex(0, 0); node.SetIndex(1, 1); node.SetIndex(2, 2);
    node.SetIndex(3, 0); node.SetIndex(4, 2); node.SetIndex(5, 3);
    // back face
    node.SetIndex(6, 4); node.SetIndex(7, 6); node.SetIndex(8, 5);
    node.SetIndex(9, 4); node.SetIndex(10, 7); node.SetIndex(11, 6);
    // top face
    node.SetIndex(12, 8); node.SetIndex(13, 10); node.SetIndex(14, 9);
    node.SetIndex(15, 8); node.SetIndex(16, 11); node.SetIndex(17, 10);
    // bottom face
    node.SetIndex(18, 12); node.SetIndex(19, 13); node.SetIndex(20, 14);
    node.SetIndex(21, 12); node.SetIndex(22, 14); node.SetIndex(23, 15);
    // left face
    node.SetIndex(24, 16); node.SetIndex(25, 17); node.SetIndex(26, 18);
    node.SetIndex(27, 16); node.SetIndex(28, 18); node.SetIndex(29, 19);
    // right face
    node.SetIndex(30, 20); node.SetIndex(31, 22); node.SetIndex(32, 21);
    node.SetIndex(33, 20); node.SetIndex(34, 23); node.SetIndex(35, 22);

    return true;
}

/// 圆形面片
bool RtgGeometryFactory::CreateCircle(RTGVertexFormat eFormat, BYTE* pVertexBuf, int iVBufSize, BYTE* pIndexBuf, int iIBufSize, float fRadius, DWORD dwColor, ETexCoordMode eMode)
{
    return CreateCircleEx(eFormat, pVertexBuf, iVBufSize, pIndexBuf, iIBufSize, fRadius, 90.0f, 450.0f, dwColor, eMode);
}

/// 扇形面片
bool RtgGeometryFactory::CreateCircleEx(RTGVertexFormat eFormat, BYTE* pVertexBuf, int iVBufSize, BYTE* pIndexBuf, int iIBufSize, float fRadius, float rs, float es, DWORD dwColor, ETexCoordMode eMode)
{
    float theta_total = DegreeToRadian(es-rs);
    float theta_inc = (theta_total)/m_iPieces;
    float theta;
    int num_vertices = m_iPieces + 2;
    int num_triangles = m_iPieces;
    int i;

    int iVertexCnt = num_vertices;
    int iFaceCnt = num_triangles;
    int iIndexCnt = num_triangles*3;
    RtgGeometryNode node;
    node.pVertexBuffer = pVertexBuf;
    node.pIndexBuffer = pIndexBuf;
    node.eFormat = eFormat;
    if (iVBufSize<RtgGetVertexFormatSize(eFormat)*iVertexCnt) return false;
    if (iIBufSize<(int)sizeof(short)*iIndexCnt) return false;

    node.SetVertex  (0, 0.0f, 0.0f, 0.0f);
    node.SetNormal  (0, 0.0f, 0.0f, 1.0f);
    node.SetColor   (0, dwColor);
    node.SetTexCoord(0, 0.5f, 0.5f);

    theta = DegreeToRadian(rs);
    for ( i=1; i<num_vertices; i++ )
    {
        node.SetVertex  (i, fRadius * fcos(theta), fRadius * fsin(theta), 0.0f);
        node.SetColor   (i, dwColor);
        node.SetNormal  (i, 0.0f, 0.0f, 1.0f);
        node.SetTexCoord(i, (fcos(theta)+1.0f)*0.5f, (fsin(theta)+1.0f)*0.5f);
        theta += theta_inc;
    }

    int index = 0;
    for (i=1; i<=m_iPieces; i++ )
    {
        node.SetIndex(index++, 0);
        node.SetIndex(index++, i);
        node.SetIndex(index++, i+1);
    }

    return true;
}

/// 圆锥体(无底的)
bool RtgGeometryFactory::CreateCone(RTGVertexFormat eFormat, BYTE* pVertexBuf, int iVBufSize, BYTE* pIndexBuf, int iIBufSize, float r, float h, DWORD dwColor, ETexCoordMode eMode)
{
    return CreateCylinder(eFormat, pVertexBuf, iVBufSize, pIndexBuf, iIBufSize, r, 0, h, dwColor, eMode);
}

/// 圆台体(无底的)
bool RtgGeometryFactory::CreateCylinder(RTGVertexFormat eFormat, BYTE* pVertexBuf, int iVBufSize, BYTE* pIndexBuf, int iIBufSize, float r1, float r2, float h, DWORD dwColor, ETexCoordMode eMode)
{
    return CreateCylinderEx(eFormat, pVertexBuf, iVBufSize, pIndexBuf, iIBufSize, 90.0f, 450.0f, r1, r2, h, dwColor, eMode);
}

/// 圆台体(无底的, 截面是扇形的)
bool RtgGeometryFactory::CreateCylinderEx(RTGVertexFormat eFormat, BYTE* pVertexBuf, int iVBufSize, BYTE* pIndexBuf, int iIBufSize, float ts, float te, float radius1, float radius2, float height, DWORD dwColor, ETexCoordMode eMode)
{
    int num_vertices = (m_iPieces+1)*2;
    int num_triangles = m_iPieces*2;

    int iVertexCnt = num_vertices;
    int iFaceCnt = num_triangles;
    int iIndexCnt = num_triangles*3;
    RtgVertex3* pV;
    RtgGeometryNode node;
    node.pVertexBuffer = pVertexBuf;
    node.pIndexBuffer = pIndexBuf;
    node.eFormat = eFormat;
    if (iVBufSize<RtgGetVertexFormatSize(eFormat)*iVertexCnt) return false;
    if (iIBufSize<(int)sizeof(short)*iIndexCnt) return false;

    float theta_total = DegreeToRadian(te-ts);
    float theta_inc = (theta_total)/m_iPieces;
    float theta = DegreeToRadian(ts);
    float fMaxRadius = (radius2>radius1)?radius2:radius1;

    int index = 0;
    for ( int i=0; i<=m_iPieces; i++, theta += theta_inc )
    {
        // bottom vertex
        node.SetVertex( index, radius1 * fcos(theta), radius1 * fsin(theta), -height/2.0f);
        // bottom normal
        node.SetNormal( index, fcos(theta), fsin(theta), 0.0f);
        // bottom color
        node.SetColor ( index, dwColor);
        // bottom vertex tex coord
        switch (eMode)
        {
        case TCM_PLANE:
            pV = node.GetVertex(index);
            node.SetTexCoord( index, (pV->x+fMaxRadius)*0.5f/fMaxRadius, (pV->y+fMaxRadius)*0.5f/fMaxRadius);
            break;
        default:
            node.SetTexCoord( index, (float)i/(float)m_iPieces, 1.0f);
            break;
        }
        index++;

        // top vertex position
        node.SetVertex( index, radius2 * fcos(theta), radius2 * fsin(theta), height/2.0f);
        // top vertex normal
        node.SetNormal( index, fcos(theta), fsin(theta), 0.0f );
        // top vertex tex coord
        switch (eMode)
        {
        case TCM_PLANE:
            pV = node.GetVertex(index);
            node.SetTexCoord( index, (pV->x+fMaxRadius)*0.5f/fMaxRadius, (pV->y+fMaxRadius)*0.5f/fMaxRadius);
            break;
        default:
            node.SetTexCoord( index, (float)i/(float)m_iPieces, 0.0f);
            break;
        }
        index++;
    }

    int base = 0;
    index = 0;
    for (int i=0; i<m_iPieces; i++ )
    {
        node.SetIndex(index++, base);
        node.SetIndex(index++, base+2);
        node.SetIndex(index++, base+1);
        base++;

        node.SetIndex(index++, base);
        node.SetIndex(index++, base+1);
        node.SetIndex(index++, base+2);
        base++;
    }
    return true;
}

/// 球体
bool RtgGeometryFactory::CreateSphereEx(RTGVertexFormat eFormat, BYTE* pVertexBuf, int iVBufSize, BYTE* pIndexBuf, int iIBufSize, float fRadius, float ts, float te, float ps, float pe, DWORD dwColor, ETexCoordMode eMode)
{
    int num_vertices = (m_iStacks+1)*(m_iPieces+1);
    int num_triangles = m_iStacks*m_iPieces*2;

    int iVertexCnt = num_vertices;
    int iFaceCnt = num_triangles;
    int iIndexCnt = num_triangles*3;
    RtgGeometryNode node;
    node.pVertexBuffer = pVertexBuf;
    node.pIndexBuffer = pIndexBuf;
    node.eFormat = eFormat;
    if (iVBufSize<RtgGetVertexFormatSize(eFormat)*iVertexCnt) return false;
    if (iIBufSize<(int)sizeof(short)*iIndexCnt) return false;

    float theta_total = DegreeToRadian(te - ts);
    float phi_total = DegreeToRadian(pe - ps);
    float theta_inc = (theta_total)/m_iStacks;
    float phi_inc = (phi_total)/m_iPieces;
    RtgVertex3* pV;

    int i,j;
    int index = 0;
    float theta = DegreeToRadian(ts);

    for ( i=0; i<=m_iStacks; i++ )
    {
        float phi = DegreeToRadian(ps);
        float cos_theta = fcos(theta);
        float sin_theta = fsin(theta);
        for ( j=0; j<=m_iPieces; j++, index++ )
        {
            float cos_phi = fcos(phi);
            float sin_phi = fsin(phi);
            // vertex
            node.SetVertex( index, (fRadius * cos_theta) * cos_phi, (fRadius * cos_theta) * sin_phi, fRadius * sin_theta );
            // normal
            RtgVertex3 n( (fRadius * cos_theta) * cos_phi, (fRadius * cos_theta) * sin_phi, fRadius * sin_theta );
            n.Normalize();
            node.SetNormal( index, n);
            // color
            node.SetColor( index, dwColor);
            // texcoord
            switch (eMode)
            {
            case TCM_PLANE:
                pV = node.GetVertex(index);
                node.SetTexCoord( index, (pV->x+fRadius)*0.5f/fRadius, (pV->y+fRadius)*0.5f/fRadius);
                break;
            default:
                node.SetTexCoord( index, (float)j/(float)(m_iPieces), (float)i/(float)(m_iStacks) );
                break;
            }
            phi += phi_inc;
        }
        theta += theta_inc;
    }

    int base = 0;
    index = 0;
    for ( i=0; i<m_iStacks; i++ )
    {
        for ( j=0; j<m_iPieces; j++ )
        {
            node.SetIndex(index++, base);
            node.SetIndex(index++, base+1);
            node.SetIndex(index++, base+m_iPieces+1);

            node.SetIndex(index++, base+1);
            node.SetIndex(index++, base+m_iPieces+1+1);
            node.SetIndex(index++, base+m_iPieces+1);

            base++;
        }
        base++;
    }

    return true;
}

/// 球体
bool RtgGeometryFactory::CreateSphere(RTGVertexFormat eFormat, BYTE* pVertexBuf, int iVBufSize, BYTE* pIndexBuf, int iIBufSize, float fRadius, DWORD dwColor, ETexCoordMode eMode)
{
    return CreateSphereEx(eFormat, pVertexBuf, iVBufSize, pIndexBuf, iIBufSize, fRadius, -90.0f, 90.0f, 90.0f, 450.0f, dwColor, eMode);
}

void RtgGeometryFactory::CreateOneStack(RtgGeometryNode& node, int stack, float InnerRadius, float OuterRadius, float ts, float te, float ps, float pe, int vbase, int ibase, DWORD dwColor, ETexCoordMode eMode)
{
    float phi_total = DegreeToRadian(pe-ps);
    float phi_inc = phi_total/m_iPieces;
    float phi = DegreeToRadian(pe);

    RtgVertex3 center0, center1;
    RtgVertex3 vec0( (float) cos(ts), (float) sin(ts), 0.0f );
    RtgVertex3 vec1( (float) cos(te), (float) sin(te), 0.0f );
    RtgVertex3 vecy(0.0f, 0.0f, 1.0f);
    RtgVertex3 vecz;

    float ir = (OuterRadius - InnerRadius)/2.0f;
    float radius = (InnerRadius+OuterRadius)/2.0f;
    center0.Set( (float)(radius * cos(ts)), (float)(radius *sin(ts)), 0.0f );
    center1.Set( (float)(radius * cos(te)), (float)(radius *sin(te)), 0.0f );

    RtgMatrix16 matrix0, matrix1;
    matrix0.Unit();
    matrix1.Unit();

    rtgVectorCross(vecz, vec0, vecy);
    matrix0.SetRow(0, vec0);
    matrix0.SetRow(1, vecy);
    matrix0.SetRow(2, vecz);
    matrix0.SetRow(3, center0);

    rtgVectorCross(vecz, vec1, vecy);
    matrix1.SetRow(0, vec1);
    matrix1.SetRow(1, vecy);
    matrix1.SetRow(2, vecz);
    matrix1.SetRow(3, center1);

    RtgVertex3 vector;
    RtgVertex3 output;
    RtgVertex3 normal;

    int index = vbase;
    for ( int i=0; i<=m_iPieces; i++ )
    {
        vector.Set( (float)cos(phi), (float)sin(phi), 0.0f);
        output = (vector * ir) * matrix0;
        normal = output - center0;
        normal.Normalize();

        node.SetVertex( index, output );
        node.SetNormal( index, normal);
        node.SetColor ( index, dwColor);
        node.SetTexCoord( index, (float)stack/(float)m_iStacks, (float)i/(float)m_iPieces );
        index++;

        vector.Set( (float)cos(phi), (float)sin(phi), 0.0f);
        output = (vector*ir) * matrix1;
        normal = output - center1;
        normal.Normalize();

        node.SetVertex( index, output );
        node.SetNormal( index, normal );
        node.SetColor ( index, dwColor );
        node.SetTexCoord( index, (float)(stack+1)/(float)m_iStacks, (float)i/(float)m_iPieces );
        index++;

        phi += phi_inc;
    }

    index = ibase;
    int base = stack * m_iPieces * 2;
    for ( int i=0; i<m_iPieces; i++ )
    {
        node.SetIndex(index++, base);
        node.SetIndex(index++, base+1);
        node.SetIndex(index++, base+2);
        base++;

        node.SetIndex(index++, base);
        node.SetIndex(index++, base+2);
        node.SetIndex(index++, base+1);
        base++;
    }
}

/// 扇环体
bool RtgGeometryFactory::CreateDonutEx(RTGVertexFormat eFormat, BYTE* pVertexBuf, int iVBufSize, BYTE* pIndexBuf, int iIBufSize, float InnerRadius, float OuterRadius, float ts, float te, float ps, float pe, DWORD dwColor, ETexCoordMode eMode)
{
    int num_vertices = m_iStacks*(m_iPieces+1)*2;
    int num_triangles = m_iStacks*m_iPieces*2;

    int iVertexCnt = num_vertices;
    int iFaceCnt = num_triangles;
    int iIndexCnt = num_triangles*3;
    RtgGeometryNode node;
    node.pVertexBuffer = pVertexBuf;
    node.pIndexBuffer = pIndexBuf;
    node.eFormat = eFormat;
    if (iVBufSize<RtgGetVertexFormatSize(eFormat)*iVertexCnt) return false;
    if (iIBufSize<(int)sizeof(short)*iIndexCnt) return false;

    float theta_total = DegreeToRadian(te - ts);
    float theta = DegreeToRadian(ts);
    float theta_inc = (theta_total)/(m_iStacks-1);

    int vbase = 0;
    int ibase = 0;
    for ( int i=0; i<m_iStacks; i++ )
    {
        CreateOneStack(node, i, InnerRadius, OuterRadius, theta, theta+theta_inc, ps, pe, vbase, ibase, dwColor, eMode);
        vbase += (m_iPieces+1)*2;
        ibase += m_iPieces*2*3;
        theta += theta_inc;
    }
    return true;
}

/// 圆环体
bool RtgGeometryFactory::CreateDonut(RTGVertexFormat eFormat, BYTE* pVertexBuf, int iVBufSize, BYTE* pIndexBuf, int iIBufSize, float or, float ir, DWORD dwColor, ETexCoordMode eMode)
{
    return CreateDonutEx(eFormat, pVertexBuf, iVBufSize, pIndexBuf, iIBufSize, ir, or, 0.0f, 360.0f, 0.0f, 360.0f, dwColor, eMode);
}

/// 箭头体(由两个圆锥体和两个圆形的面组成)
bool RtgGeometryFactory::CreateArrowhead(RTGVertexFormat eFormat, BYTE* pVertexBuf, int iVBufSize, BYTE* pIndexBuf, int iIBufSize, DWORD dwColor, ETexCoordMode eMode)
{
    short sIndexValue, iIb;
    int i, iV, iI;
    RtgVertex3* pV;
    int num_vertices = (m_iPieces+1)*4 + (m_iPieces+2)*2;
    int num_triangles = m_iPieces*4 + m_iPieces*2;
    int iVertexCnt = num_vertices;
    int iFaceCnt = num_triangles;
    int iIndexCnt = num_triangles*3;
    RtgGeometryNode node;
    node.pVertexBuffer = pVertexBuf;
    node.pIndexBuffer = pIndexBuf;
    node.eFormat = eFormat;
    if (iVBufSize<RtgGetVertexFormatSize(eFormat)*iVertexCnt) return false;
    if (iIBufSize<(int)sizeof(short)*iIndexCnt) return false;
    const float fHeight1 = 30.f, fHeight2 = 10.f, fRadius1 = 5.f, fRadius2 = 10.f, fRadius3 = 24.f;
    iV = 0;
    iI = 0;

    // 柄
    CreateCylinder(eFormat, pVertexBuf, iVBufSize, pIndexBuf, iIBufSize, fRadius1, fRadius2, fHeight1, dwColor, eMode);
    pVertexBuf += RtgGetVertexFormatSize(eFormat)*(m_iPieces+1)*2;
    pIndexBuf += m_iPieces*2*3*sizeof(short);
    i = iV;
    iIb = (short)iV;
    iV += (m_iPieces+1)*2;
    for (; i<iV; i++)
    {
        pV = node.GetVertex(i);
        pV->z += fHeight1/2.f;
    }
    i = iI;
    iI += m_iPieces*2*3;
    for (; i<iI; i++)
    {
        node.GetIndex(i, sIndexValue);
        node.SetIndex(i, sIndexValue);
    }
    // 簇
    CreateCylinder(eFormat, pVertexBuf, iVBufSize, pIndexBuf, iIBufSize, fRadius3, 0.f, fHeight2, dwColor, eMode);
    pVertexBuf += RtgGetVertexFormatSize(eFormat)*(m_iPieces+1)*2;
    pIndexBuf += m_iPieces*2*3*sizeof(short);
    i = iV;
    iIb = (short)iV;
    iV += (m_iPieces+1)*2;
    for (; i<iV; i++)
    {
        pV = node.GetVertex(i);
        pV->z += (fHeight1+fHeight2/2.f);
    }
    i = iI;
    iI += m_iPieces*2*3;
    for (; i<iI; i++)
    {
        node.GetIndex(i, sIndexValue);
        node.SetIndex(i, sIndexValue + iIb);
    }
    // 簇的面
    CreateCircle(eFormat, pVertexBuf, iVBufSize, pIndexBuf, iIBufSize, fRadius3, dwColor, eMode);
    pVertexBuf += RtgGetVertexFormatSize(eFormat)*(m_iPieces+2);
    pIndexBuf += m_iPieces*3*sizeof(short);
    i = iV;
    iIb = (short)iV;
    iV += (m_iPieces+2);
    for (; i<iV; i++)
    {
        pV = node.GetVertex(i);
        pV->z += fHeight1;
    }
    i = iI;
    iI += m_iPieces*3;
    for (; i<iI; i++)
    {
        node.GetIndex(i, sIndexValue);
        node.SetIndex(i, sIndexValue + iIb);
    }
    // 柄的下面
    CreateCircle(eFormat, pVertexBuf, iVBufSize, pIndexBuf, iIBufSize, fRadius1, dwColor, eMode);
    iIb = (short)iV;
    i = iI;
    iI += m_iPieces*3;
    for (; i<iI; i++)
    {
        node.GetIndex(i, sIndexValue);
        node.SetIndex(i, sIndexValue + iIb);
    }
    return true;
}

/// 箭头体(由一个圆锥体和一个圆形的面组成)
bool RtgGeometryFactory::CreateArrowhead2(RTGVertexFormat eFormat, BYTE* pVertexBuf, int iVBufSize, BYTE* pIndexBuf, int iIBufSize, DWORD dwColorH, DWORD dwColorF, ETexCoordMode eMode)
{
    short sIndexValue, iIb;
    int i, iV, iI;
    RtgVertex3* pV;
    int num_vertices = (m_iPieces+1)*2 + (m_iPieces+2);
    int num_triangles = m_iPieces*2 + m_iPieces;
    int iVertexCnt = num_vertices;
    int iFaceCnt = num_triangles;
    int iIndexCnt = num_triangles*3;
    RtgGeometryNode node;
    node.pVertexBuffer = pVertexBuf;
    node.pIndexBuffer = pIndexBuf;
    node.eFormat = eFormat;
    if (iVBufSize<RtgGetVertexFormatSize(eFormat)*iVertexCnt) return false;
    if (iIBufSize<(int)sizeof(short)*iIndexCnt) return false;
    const float fHeight = 10.f, fRadius = 24.f;
    iV = 0;
    iI = 0;

    // 簇
    CreateCylinder(eFormat, pVertexBuf, iVBufSize, pIndexBuf, iIBufSize, fRadius, 0.f, fHeight, dwColorH, eMode);
    pVertexBuf += RtgGetVertexFormatSize(eFormat)*(m_iPieces+1)*2;
    pIndexBuf += m_iPieces*2*3*sizeof(short);
    i = iV;
    iIb = (short)iV;
    iV += (m_iPieces+1)*2;
    for (; i<iV; i++)
    {
        pV = node.GetVertex(i);
        pV->z += fHeight/2.f;
    }
    iI += m_iPieces*2*3;
    // 簇的面
    CreateCircle(eFormat, pVertexBuf, iVBufSize, pIndexBuf, iIBufSize, fRadius, dwColorF, eMode);
    iIb = (short)iV;
    i = iI;
    iI += m_iPieces*3;
    for (; i<iI; i++)
    {
        node.GetIndex(i, sIndexValue);
        node.SetIndex(i, sIndexValue + iIb);
    }
    return true;
}

RtgGeometryNode* RtgGeometryFactory::CreateRect(RTGVertexFormat eFormat, float w, float h, DWORD dwColor, ETexCoordMode eMode)
{
    int iVertexCnt = 4;
    int iFaceCnt = 2;
    int iBufSize = RtgGetVertexFormatSize(eFormat)*iVertexCnt + sizeof(short)*iFaceCnt*3 + sizeof(RtgGeometryNode);
    RtgGeometryNode* pNode = (RtgGeometryNode*) RtCoreMem().Alloc(iBufSize, __FILE__, __LINE__);
    pNode->eType            = RTGGT_RECTANGLE;
    pNode->eFormat          = eFormat;
    pNode->iVertexCnt       = iVertexCnt;
    pNode->iIndexCnt        = iFaceCnt*3;
    pNode->pVertexBuffer    = (BYTE*)(pNode+1);
    pNode->pIndexBuffer     = pNode->pVertexBuffer + RtgGetVertexFormatSize(eFormat)*iVertexCnt;
    CreateRect(eFormat, pNode->pVertexBuffer, RtgGetVertexFormatSize(eFormat)*iVertexCnt, pNode->pIndexBuffer, sizeof(short)*iFaceCnt*3, w, h, dwColor, eMode);
    return pNode;
}

RtgGeometryNode* RtgGeometryFactory::CreateTrape(RTGVertexFormat eFormat, float t, float b, float h, DWORD dwColor, ETexCoordMode eMode)
{
    int iVertexCnt = 4;
    int iFaceCnt = 2;
    int iBufSize = RtgGetVertexFormatSize(eFormat)*iVertexCnt + sizeof(short)*iFaceCnt*3 + sizeof(RtgGeometryNode);
    RtgGeometryNode* pNode = (RtgGeometryNode*) RtCoreMem().Alloc(iBufSize, __FILE__, __LINE__);
    pNode->eType            = RTGGT_TRAPEZIA;
    pNode->eFormat          = eFormat;
    pNode->iVertexCnt       = iVertexCnt;
    pNode->iIndexCnt        = iFaceCnt*3;
    pNode->pVertexBuffer    = (BYTE*)(pNode+1);
    pNode->pIndexBuffer     = pNode->pVertexBuffer + RtgGetVertexFormatSize(eFormat)*iVertexCnt;
    CreateTrape(eFormat, pNode->pVertexBuffer, RtgGetVertexFormatSize(eFormat)*iVertexCnt, pNode->pIndexBuffer, sizeof(short)*iFaceCnt*3, t, b, h, dwColor, eMode);
    return pNode;
}

RtgGeometryNode* RtgGeometryFactory::CreateBox(RTGVertexFormat eFormat, float w, float h, float d, DWORD dwColor, ETexCoordMode eMode)
{
    int iVertexCnt = 24;
    int iFaceCnt = 12;
    int iBufSize = RtgGetVertexFormatSize(eFormat)*iVertexCnt + sizeof(short)*iFaceCnt*3 + sizeof(RtgGeometryNode);
    RtgGeometryNode* pNode = (RtgGeometryNode*) RtCoreMem().Alloc(iBufSize, __FILE__, __LINE__);
    pNode->eType            = RTGGT_BOX;
    pNode->eFormat          = eFormat;
    pNode->iVertexCnt       = iVertexCnt;
    pNode->iIndexCnt        = iFaceCnt*3;
    pNode->pVertexBuffer    = (BYTE*)(pNode+1);
    pNode->pIndexBuffer     = pNode->pVertexBuffer + RtgGetVertexFormatSize(eFormat)*iVertexCnt;
    CreateBox(eFormat, pNode->pVertexBuffer, RtgGetVertexFormatSize(eFormat)*iVertexCnt, pNode->pIndexBuffer, sizeof(short)*iFaceCnt*3, w, h, d, dwColor, eMode);
    return pNode;
}

RtgGeometryNode* RtgGeometryFactory::CreateCircle(RTGVertexFormat eFormat, float r, DWORD dwColor, ETexCoordMode eMode)
{
    int iVertexCnt = m_iPieces + 2;
    int iFaceCnt = m_iPieces;
    int iBufSize = RtgGetVertexFormatSize(eFormat)*iVertexCnt + sizeof(short)*iFaceCnt*3 + sizeof(RtgGeometryNode);
    RtgGeometryNode* pNode = (RtgGeometryNode*) RtCoreMem().Alloc(iBufSize, __FILE__, __LINE__);
    pNode->eType            = RTGGT_CIRCLE;
    pNode->eFormat          = eFormat;
    pNode->iVertexCnt       = iVertexCnt;
    pNode->iIndexCnt        = iFaceCnt*3;
    pNode->pVertexBuffer    = (BYTE*)(pNode+1);
    pNode->pIndexBuffer     = pNode->pVertexBuffer + RtgGetVertexFormatSize(eFormat)*iVertexCnt;
    CreateCircle(eFormat, pNode->pVertexBuffer, RtgGetVertexFormatSize(eFormat)*iVertexCnt, pNode->pIndexBuffer, sizeof(short)*iFaceCnt*3, r, dwColor, eMode);
    return pNode;
}

RtgGeometryNode* RtgGeometryFactory::CreateCone(RTGVertexFormat eFormat, float w, float h, DWORD dwColor, ETexCoordMode eMode)
{
    int iVertexCnt = (m_iPieces+1)*2;
    int iFaceCnt = m_iPieces*2;
    int iBufSize = RtgGetVertexFormatSize(eFormat)*iVertexCnt + sizeof(short)*iFaceCnt*3 + sizeof(RtgGeometryNode);
    RtgGeometryNode* pNode = (RtgGeometryNode*) RtCoreMem().Alloc(iBufSize, __FILE__, __LINE__);
    pNode->eType            = RTGGT_CONE;
    pNode->eFormat          = eFormat;
    pNode->iVertexCnt       = iVertexCnt;
    pNode->iIndexCnt        = iFaceCnt*3;
    pNode->pVertexBuffer    = (BYTE*)(pNode+1);
    pNode->pIndexBuffer     = pNode->pVertexBuffer + RtgGetVertexFormatSize(eFormat)*iVertexCnt;
    CreateCone(eFormat, pNode->pVertexBuffer, RtgGetVertexFormatSize(eFormat)*iVertexCnt, pNode->pIndexBuffer, sizeof(short)*iFaceCnt*3, w, h, dwColor, eMode);
    return pNode;
}

RtgGeometryNode* RtgGeometryFactory::CreateCylinder(RTGVertexFormat eFormat, float r1, float r2, float h, DWORD dwColor, ETexCoordMode eMode)
{
    int iVertexCnt = (m_iPieces+1)*2;
    int iFaceCnt = m_iPieces*2;
    int iBufSize = RtgGetVertexFormatSize(eFormat)*iVertexCnt + sizeof(short)*iFaceCnt*3 + sizeof(RtgGeometryNode);
    RtgGeometryNode* pNode = (RtgGeometryNode*) RtCoreMem().Alloc(iBufSize, __FILE__, __LINE__);
    pNode->eType            = RTGGT_CYLINDER;
    pNode->eFormat          = eFormat;
    pNode->iVertexCnt       = iVertexCnt;
    pNode->iIndexCnt        = iFaceCnt*3;
    pNode->pVertexBuffer    = (BYTE*)(pNode+1);
    pNode->pIndexBuffer     = pNode->pVertexBuffer + RtgGetVertexFormatSize(eFormat)*iVertexCnt;
    CreateCylinder(eFormat, pNode->pVertexBuffer, RtgGetVertexFormatSize(eFormat)*iVertexCnt, pNode->pIndexBuffer, sizeof(short)*iFaceCnt*3, r1, r2, h, dwColor, eMode);
    return pNode;
}

RtgGeometryNode* RtgGeometryFactory::CreateSphere(RTGVertexFormat eFormat, float fRadius, DWORD dwColor, ETexCoordMode eMode)
{
    int iVertexCnt = (m_iStacks+1)*(m_iPieces+1);
    int iFaceCnt = m_iStacks*m_iPieces*2;
    int iBufSize = RtgGetVertexFormatSize(eFormat)*iVertexCnt + sizeof(short)*iFaceCnt*3 + sizeof(RtgGeometryNode);
    RtgGeometryNode* pNode = (RtgGeometryNode*) RtCoreMem().Alloc(iBufSize, __FILE__, __LINE__);
    pNode->eType            = RTGGT_SPHERE;
    pNode->eFormat          = eFormat;
    pNode->iVertexCnt       = iVertexCnt;
    pNode->iIndexCnt        = iFaceCnt*3;
    pNode->pVertexBuffer    = (BYTE*)(pNode+1);
    pNode->pIndexBuffer     = pNode->pVertexBuffer + RtgGetVertexFormatSize(eFormat)*iVertexCnt;
    CreateSphere(eFormat, pNode->pVertexBuffer, RtgGetVertexFormatSize(eFormat)*iVertexCnt, pNode->pIndexBuffer, sizeof(short)*iFaceCnt*3, fRadius, dwColor, eMode);
    return pNode;
}

RtgGeometryNode* RtgGeometryFactory::CreateDonut(RTGVertexFormat eFormat, float or, float ir, DWORD dwColor, ETexCoordMode eMode)
{
    int iVertexCnt = m_iStacks*(m_iPieces+1)*2;
    int iFaceCnt = m_iStacks*m_iPieces*2;
    int iBufSize = RtgGetVertexFormatSize(eFormat)*iVertexCnt + sizeof(short)*iFaceCnt*3 + sizeof(RtgGeometryNode);
    RtgGeometryNode* pNode = (RtgGeometryNode*) RtCoreMem().Alloc(iBufSize, __FILE__, __LINE__);
    pNode->eType            = RTGGT_DONUT;
    pNode->eFormat          = eFormat;
    pNode->iVertexCnt       = iVertexCnt;
    pNode->iIndexCnt        = iFaceCnt*3;
    pNode->pVertexBuffer    = (BYTE*)(pNode+1);
    pNode->pIndexBuffer     = pNode->pVertexBuffer + RtgGetVertexFormatSize(eFormat)*iVertexCnt;
    CreateDonut(eFormat, pNode->pVertexBuffer, RtgGetVertexFormatSize(eFormat)*iVertexCnt, pNode->pIndexBuffer, sizeof(short)*iFaceCnt*3, or, ir, dwColor, eMode);
    return pNode;
}

RtgGeometryNode* RtgGeometryFactory::CreateArrowhead(RTGVertexFormat eFormat, DWORD dwColor, ETexCoordMode eMode)
{
    int iVertexCnt = (m_iPieces+1)*4 + (m_iPieces+2)*2;
    int iFaceCnt = m_iPieces*4 + m_iPieces*2;
    int iBufSize = RtgGetVertexFormatSize(eFormat)*iVertexCnt + sizeof(short)*iFaceCnt*3 + sizeof(RtgGeometryNode);
    RtgGeometryNode* pNode = (RtgGeometryNode*) RtCoreMem().Alloc(iBufSize, __FILE__, __LINE__);
    pNode->eType            = RTGGT_MAX;
    pNode->eFormat          = eFormat;
    pNode->iVertexCnt       = iVertexCnt;
    pNode->iIndexCnt        = iFaceCnt*3;
    pNode->pVertexBuffer    = (BYTE*)(pNode+1);
    pNode->pIndexBuffer     = pNode->pVertexBuffer + RtgGetVertexFormatSize(eFormat)*iVertexCnt;
    CreateArrowhead(eFormat, pNode->pVertexBuffer, RtgGetVertexFormatSize(eFormat)*iVertexCnt, pNode->pIndexBuffer, sizeof(short)*iFaceCnt*3, dwColor, eMode);
    return pNode;
}

RtgGeometryNode* RtgGeometryFactory::CreateArrowhead2(RTGVertexFormat eFormat, DWORD dwColorH, DWORD dwColorF, ETexCoordMode eMode)
{
    int iVertexCnt = (m_iPieces+1)*2 + (m_iPieces+2);
    int iFaceCnt = m_iPieces*2 + m_iPieces;
    int iBufSize = RtgGetVertexFormatSize(eFormat)*iVertexCnt + sizeof(short)*iFaceCnt*3 + sizeof(RtgGeometryNode);
    RtgGeometryNode* pNode = (RtgGeometryNode*) RtCoreMem().Alloc(iBufSize, __FILE__, __LINE__);
    pNode->eType            = RTGGT_MAX;
    pNode->eFormat          = eFormat;
    pNode->iVertexCnt       = iVertexCnt;
    pNode->iIndexCnt        = iFaceCnt*3;
    pNode->pVertexBuffer    = (BYTE*)(pNode+1);
    pNode->pIndexBuffer     = pNode->pVertexBuffer + RtgGetVertexFormatSize(eFormat)*iVertexCnt;
    CreateArrowhead2(eFormat, pNode->pVertexBuffer, RtgGetVertexFormatSize(eFormat)*iVertexCnt, pNode->pIndexBuffer, sizeof(short)*iFaceCnt*3, dwColorH, dwColorF, eMode);
    return pNode;
}

void RtgGeometryFactory::DestroyGeometry(RtgGeometryNode* pGNode)
{
    BYTE* pBuf = (BYTE*)pGNode;
    RtCoreMem().Free(pBuf);
}

/*----------------------------------------------------------------------------
-   Class RtgGeometry.
----------------------------------------------------------------------------*/

int RtgGeometry::m_iStacks = 20;
int RtgGeometry::m_iPieces = 20;
RtgGeometry::RtgGeometry()
{
}
RtgGeometry::RtgGeometry(RtgDevice* pDevice)
{
    m_pDevice = pDevice;
	m_iNumVertices = 0;
	m_iNumFaces = 0;
	m_iNumIndices = 0;

	SetVertexFormat(RTG_VCT);
	SetResourceType(RTG_SYSTEM_MEMORY);
}

RtgGeometry::~RtgGeometry()
{
	ReleaseResource();
}

void RtgGeometry::SetResourceType(RTGMemoryType mtype)
{
	m_eMType = mtype;
	m_Vertices.SetResourceType(mtype, RTGBT_VERTEX_BUFFER);
	m_Indices.SetResourceType(mtype, RTGBT_INDEX_BUFFER);
}

/*
void RtgGeometry::SetVertexFormat(RtgVertexSpec fmt)
{
	m_VertexSpec = fmt;
	m_Vertices.SetVertexFormat(fmt);
}

int RtgGeometry::SetVertexFormat(int vsize, int normal, int color, int numtex, int *texcoords)
{
	if ( m_Vertices.SetVertexFormat(vsize, normal, color, numtex, texcoords) )
	{
		m_VertexSpec = m_Vertices.GetVertexFormat();
		return 1;
	}
	else
	{
		return 0;
	}
}
*/

void RtgGeometry::SetVertexFormat(RTGVertexFormat fmt)
{
	m_eVertexFormat = fmt;
	m_Vertices.SetVertexFormat(fmt);
    m_VertexSpec = m_Vertices.GetVertexFormat();
}

void RtgGeometry::SetGeometryQuality(int quality)
{
	switch(quality)
	{
	case 0:
		m_iPieces = 10;
		m_iStacks = 10;
		break;
	case 1:
		m_iPieces = 15;
		m_iStacks = 15;
		break;
	case 2:
		m_iPieces = 30;
		m_iStacks = 30;
		break;
	default:
		m_iPieces = 20;
		m_iStacks = 20;
		break;
	}
}

void RtgGeometry::ReleaseResource(void)
{
	m_Vertices.ReleaseBuffer(m_pDevice);
	m_Indices.ReleaseBuffer(m_pDevice);
	m_iNumVertices = 0;
	m_iNumFaces = 0;
	m_iNumIndices = 0;
}

int RtgGeometry::GetTangentSpace(int iIdx, RtgVertex3 vec[3])
{
	RtgVertex3 normal;
	m_Vertices.GetNormal(iIdx, normal);

	vec[2] = normal;

	if ( normal.x==0.0f && normal.y==0.0f )
		vec[0].Set(1.0f, 0.0f, 0.0f);
	else
		vec[0].Set( -normal.y, normal.x, 0.0f);

	vec[1] = rtgVectorCross(vec[2], vec[0]);

	return 0;
}

void RtgGeometry::OffsetVertices(float x, float y, float z)
{
	float *fp = (float *) m_Vertices.LockBuffer(m_pDevice);
	for ( int i=0; i<m_iNumVertices; i++ )
	{
		fp[0] += x;
		fp[1] += y;
		fp[2] += z;
		fp += m_VertexSpec.iNumVertexSize/4;
	}
}

int RtgGeometry::CreateResource(int vertices, int faces, int indices)
{
	ReleaseResource();

	m_iNumVertices = vertices;
	m_iNumFaces = faces;
	m_iNumIndices = indices;

	if ( m_iNumVertices )
		m_Vertices.AllocateElements(m_pDevice, vertices);

	if ( m_iNumIndices )
		m_Indices.AllocateElements(m_pDevice, indices);
	
	if ( vertices && indices )
	{
		if ( m_Vertices.GetMemoryType()==RTG_VIDEO_MEMORY && m_Indices.GetMemoryType()==RTG_VIDEO_MEMORY )
		{
			m_eMType = RTG_VIDEO_MEMORY;
		}
		else if ( m_Vertices.GetMemoryType()==RTG_VIDEO_MEMORY && m_Indices.GetMemoryType()==RTG_SYSTEM_MEMORY )
		{
			m_Vertices.SetResourceType(RTG_SYSTEM_MEMORY, RTGBT_VERTEX_BUFFER);
			m_Vertices.AllocateElements(m_pDevice, vertices);
			m_eMType = RTG_SYSTEM_MEMORY;
		}
		else if ( m_Vertices.GetMemoryType()==RTG_SYSTEM_MEMORY && m_Indices.GetMemoryType()==RTG_VIDEO_MEMORY )
		{
			m_Indices.SetResourceType(RTG_SYSTEM_MEMORY, RTGBT_INDEX_BUFFER);
			m_Indices.AllocateElements(m_pDevice, indices);
			m_eMType = RTG_SYSTEM_MEMORY;
		}
		else if ( m_Vertices.GetMemoryType()==RTG_SYSTEM_MEMORY && m_Indices.GetMemoryType()==RTG_SYSTEM_MEMORY )
		{
			m_eMType = RTG_SYSTEM_MEMORY;
		}
	}
	else
	{
		if ( vertices )
		{
			m_eMType = m_Vertices.GetMemoryType();
		}
		else if ( indices )
		{
			m_eMType = m_Indices.GetMemoryType();
		}
	}

	return 1;
}

void RtgGeometry::SetFVF() // Set Vertex Format
{
    m_pDevice->SetVertexFormat(m_eVertexFormat);
}

void RtgGeometry::Render(RTGPrimitive primitive)
{
    if (!m_iNumIndices)
        return;

    SetFVF();
    m_pDevice->DrawIndexedPrimitive(
        RtGetRender()->GetD3DPT(primitive),
        m_Vertices.GetBuffer(),
        m_pDevice->GetCurrentVertexStride(),
        m_Indices.GetBuffer(),
        m_iNumFaces);


    /*
    SetFVF();
	switch(m_eMType)
	{
	case RTG_SYSTEM_MEMORY:
		if ( m_iNumIndices )
		{
	        if(!m_pDevice->DrawIndexPrimitive(primitive, m_Vertices.GetBuffer(), m_iNumVertices, m_Indices.GetBuffer(), m_iNumIndices, m_iNumFaces))
            {
                CHECKEX("DrawIndexPrimitive失败");
            }
		}else
		{
	        if (!m_pDevice->DrawPrimitive(primitive, m_Vertices.GetBuffer(), m_iNumVertices, m_iNumFaces))
            {
                CHECKEX("DrawPrimitive失败");
            }
		}
		break;
	case RTG_VIDEO_MEMORY:
		if ( m_iNumIndices )
        {
            if (!m_pDevice->DrawIndexPrimitiveVB(primitive, m_Vertices.GetBuffer(), 0, m_iNumVertices, m_Indices.GetBuffer(), 0, m_iNumIndices, m_iNumFaces))
            {
                CHECKEX("DrawIndexPrimitiveVB失败");
            }
        }else
        {
	        if (!m_pDevice->DrawPrimitiveVB(primitive, m_Vertices.GetBuffer(), 0, m_iNumVertices, m_iNumFaces))
            {
                CHECKEX("DrawPrimitiveVB失败");
            }
        }
		break;
	}*/
}

void RtgGeometry::DrawIndexPrimitive(RTGPrimitive primitive, int vstart, int vcount, int istart, int icount, int pcount)
{
    RTASSERT(false);
    /*
	switch(m_eMType)
	{
	case RTG_SYSTEM_MEMORY:
		{
			unsigned char *p = (unsigned char *)m_Vertices.GetBuffer();
			short *i = (short *)m_Indices.GetBuffer();
			m_pDevice->DrawIndexPrimitive(primitive, p + vstart * m_VertexSpec.iNumVertexSize, vcount, i+istart, icount, pcount);
			break;
		}
	case RTG_VIDEO_MEMORY:
        m_pDevice->DrawIndexPrimitiveVB(primitive, m_Vertices.GetBuffer(), vstart, vcount, m_Indices.GetBuffer(), istart, icount, pcount);
		break;
	} */
}

int RtgGeometry::CreateRect(float w, float h)
{
	if ( !CreateResource(4, 2, 6) )
		return 0;

	float x_ext_div = w/2.0f;
	float y_ext_div = h/2.0f;

	m_Vertices.LockBuffer(m_pDevice);
	// vertex 0
	m_Vertices.SetVertex  (0, -x_ext_div, -y_ext_div, 0.0f);
	m_Vertices.SetNormal  (0, 0.0f, 0.0f, 1.0f);
	m_Vertices.SetColor   (0, 0xffffffff);
	m_Vertices.SetTexCoord(0, 0.0f, 0.0f);
	// vertex 1
	m_Vertices.SetVertex  (1, x_ext_div, -y_ext_div, 0.0f);
	m_Vertices.SetNormal  (1, 0.0f, 0.0f, 1.0f);
	m_Vertices.SetColor   (1, 0xffffffff);
	m_Vertices.SetTexCoord(1, 1.0f, 0.0f);
	// vertex 2
	m_Vertices.SetVertex  (2, x_ext_div, y_ext_div, 0.0f);
	m_Vertices.SetNormal  (2, 0.0f, 0.0f, 1.0f);
	m_Vertices.SetColor   (2, 0xffffffff);
	m_Vertices.SetTexCoord(2, 1.0f, 1.0f);
	// vertex 3
	m_Vertices.SetVertex  (3,-x_ext_div, y_ext_div, 0.0f);
	m_Vertices.SetNormal  (3, 0.0f, 0.0f, 1.0f);
	m_Vertices.SetColor   (3, 0xffffffff);
	m_Vertices.SetTexCoord(3, 0.0f, 1.0f);
	// end
	m_Vertices.UnLockBuffer(m_pDevice);

	m_Indices.LockBuffer(m_pDevice);
	// face 0
	m_Indices.SetIndex(0, 0);
	m_Indices.SetIndex(1, 1);
	m_Indices.SetIndex(2, 2);
	// face 1
	m_Indices.SetIndex(3, 0);
	m_Indices.SetIndex(4, 2);
	m_Indices.SetIndex(5, 3);
	// end
	m_Indices.UnLockBuffer(m_pDevice);
	return 1;
}

int RtgGeometry::CreateRect(float w, float h, int iSegmentX, int iSegmentY)
{
    if (iSegmentX<=0) iSegmentX = 1;
    if (iSegmentY<=0) iSegmentY = 1;
	if ( !CreateResource((iSegmentX+1)*(iSegmentY+1), iSegmentX*iSegmentY*2, iSegmentX*iSegmentY*6) )
		return 0;

	float x_ext_div = w/2.0f;
	float y_ext_div = h/2.0f;
    float x_len = w/iSegmentX;
    float y_len = h/iSegmentY;
    int x, y, idx, v0, v1, v2, v3;

	// 顶点
    idx = 0;
    m_Vertices.LockBuffer(m_pDevice);
    for (y=0; y<=iSegmentY; y++)
    {
        for (x=0; x<=iSegmentX; x++)
        {
            m_Vertices.SetVertex  (idx, -x_ext_div + x*x_len, -y_ext_div + y*y_len, 0.0f);
            m_Vertices.SetNormal  (idx, 0.0f, 0.0f, 1.0f);
            m_Vertices.SetColor   (idx, 0xffffffff);
            m_Vertices.SetTexCoord(idx, 0.0f+x/iSegmentX, 0.0f+y/iSegmentY);
            idx ++;
        }
    }
	m_Vertices.UnLockBuffer(m_pDevice);

    // 索引
    idx = 0;
	m_Indices.LockBuffer(m_pDevice);
    for (y=0; y<=iSegmentY; y++)
    {
        for (x=0; x<=iSegmentX; x++)
        {
            v0 = (y  )*(iSegmentX+1)+(x  );
            v1 = (y  )*(iSegmentX+1)+(x+1);
            v2 = (y+1)*(iSegmentX+1)+(x  );
            v3 = (y+1)*(iSegmentX+1)+(x+1);

            m_Indices.SetIndex(idx++, v0);
            m_Indices.SetIndex(idx++, v1);
            m_Indices.SetIndex(idx++, v2);
            m_Indices.SetIndex(idx++, v0);
            m_Indices.SetIndex(idx++, v2);
            m_Indices.SetIndex(idx++, v3);
        }
    }
	m_Indices.UnLockBuffer(m_pDevice);
	return 1;
}

int RtgGeometry::CreateTrape(float t, float b, float h)
{
	if ( !CreateResource(4, 2, 6) )
		return 0;

	float x_top_div = t/2.0f;
	float x_bottom_div = b/2.0f;
	float y_ext_div = h/2.0f;

	m_Vertices.LockBuffer(m_pDevice);
	// vertex 0
	m_Vertices.SetVertex  (0, -x_bottom_div, -y_ext_div, 0.0f);
	m_Vertices.SetNormal  (0, 0.0f, 0.0f, 1.0f);
	m_Vertices.SetColor   (0, 0xffffffff);
	m_Vertices.SetTexCoord(0, 0.0f, 0.0f);
	// vertex 1
	m_Vertices.SetVertex  (1, x_bottom_div, -y_ext_div, 0.0f);
	m_Vertices.SetNormal  (1, 0.0f, 0.0f, 1.0f);
	m_Vertices.SetColor   (1, 0xffffffff);
	m_Vertices.SetTexCoord(1, 1.0f, 0.0f);
	// vertex 2
	m_Vertices.SetVertex  (2, x_top_div, y_ext_div, 0.0f);
	m_Vertices.SetNormal  (2, 0.0f, 0.0f, 1.0f);
	m_Vertices.SetColor   (2, 0xffffffff);
	m_Vertices.SetTexCoord(2, 1.0f, 1.0f);
	// vertex 3
	m_Vertices.SetVertex  (3, -x_top_div, y_ext_div, 0.0f);
	m_Vertices.SetNormal  (3, 0.0f, 0.0f, 1.0f);
	m_Vertices.SetColor   (3, 0xffffffff);
	m_Vertices.SetTexCoord(3, 0.0f, 1.0f);
	// end 
	m_Vertices.UnLockBuffer(m_pDevice);

	m_Indices.LockBuffer(m_pDevice);
	// face 0
	m_Indices.SetIndex(0, 0);
	m_Indices.SetIndex(1, 1);
	m_Indices.SetIndex(2, 2);
	// face 1
	m_Indices.SetIndex(3, 0);
	m_Indices.SetIndex(4, 2);
	m_Indices.SetIndex(5, 3);
	// end
	m_Indices.UnLockBuffer(m_pDevice);
	return 1;
}

int RtgGeometry::CreateBox(float w, float h, float d)
{
	if ( !CreateResource(24, 12, 36) )
		return 0;

	float x_ext_div = w/2.0f;
	float y_ext_div = h/2.0f;
	float z_ext_div = d/2.0f;

	// front face
	m_Vertices.LockBuffer(m_pDevice);
	// vertex 0
	m_Vertices.SetVertex  ( 0, -x_ext_div, -y_ext_div, z_ext_div);
	m_Vertices.SetNormal  ( 0, 0.0f, 0.0f, 1.0f);
	m_Vertices.SetColor   ( 0, 0xffffffff);
	m_Vertices.SetTexCoord( 0, 0.0f, 0.0f);
	// vertex 1
	m_Vertices.SetVertex  ( 1, x_ext_div, -y_ext_div, z_ext_div);
	m_Vertices.SetNormal  ( 1, 0.0f, 0.0f, 1.0f);
	m_Vertices.SetColor   ( 1, 0xffffffff);
	m_Vertices.SetTexCoord( 1, 1.0f, 0.0f);
	// vertex 2
	m_Vertices.SetVertex  ( 2, x_ext_div, y_ext_div, z_ext_div);
	m_Vertices.SetNormal  ( 2, 0.0f, 0.0f, 1.0f);
	m_Vertices.SetColor   ( 2, 0xffffffff);
	m_Vertices.SetTexCoord( 2, 1.0f, 1.0f);
	// vertex 3
	m_Vertices.SetVertex  ( 3, -x_ext_div, y_ext_div, z_ext_div);
	m_Vertices.SetNormal  ( 3, 0.0f, 0.0f, 1.0f);
	m_Vertices.SetColor   ( 3, 0xffffffff);
	m_Vertices.SetTexCoord( 3, 0.0f, 1.0f);
	// back face
	// vertex 4
	m_Vertices.SetVertex  ( 4, -x_ext_div, -y_ext_div, -z_ext_div);
	m_Vertices.SetNormal  ( 4, 0.0f, 0.0f, -1.0f);
	m_Vertices.SetColor   ( 4, 0xffffffff);
	m_Vertices.SetTexCoord( 4, 1.0f, 0.0f);
	// vertex 5
	m_Vertices.SetVertex  ( 5, x_ext_div, -y_ext_div, -z_ext_div);
	m_Vertices.SetNormal  ( 5, 0.0f, 0.0f, -1.0f);
	m_Vertices.SetColor   ( 5, 0xffffffff);
	m_Vertices.SetTexCoord( 5, 0.0f, 0.0f);
	// vertex 6
	m_Vertices.SetVertex  ( 6, x_ext_div, y_ext_div, -z_ext_div);
	m_Vertices.SetNormal  ( 6, 0.0f, 0.0f, -1.0f);
	m_Vertices.SetColor   ( 6, 0xffffffff);
	m_Vertices.SetTexCoord( 6, 0.0f, 1.0f);
	// vertex 7
	m_Vertices.SetVertex  ( 7, -x_ext_div, y_ext_div, -z_ext_div);
	m_Vertices.SetNormal  ( 7, 0.0f, 0.0f, -1.0f);
	m_Vertices.SetColor   ( 7, 0xffffffff);
	m_Vertices.SetTexCoord( 7, 1.0f, 1.0f);
	// top face
	// vertex 8
	m_Vertices.SetVertex  ( 8, -x_ext_div, y_ext_div, -z_ext_div);
	m_Vertices.SetNormal  ( 8, 0.0f, 1.0f, 0.0f);
	m_Vertices.SetColor   ( 8, 0xffffffff);
	m_Vertices.SetTexCoord( 8, 0.0f, 1.0f);
	// vertex 9
	m_Vertices.SetVertex  ( 9, x_ext_div, y_ext_div, -z_ext_div);
	m_Vertices.SetNormal  ( 9, 0.0f, 1.0f, 0.0f);
	m_Vertices.SetColor   ( 9, 0xffffffff);
	m_Vertices.SetTexCoord( 9, 1.0f, 1.0f);
	// vertex 10
	m_Vertices.SetVertex  (10, x_ext_div, y_ext_div, z_ext_div);
	m_Vertices.SetNormal  (10, 0.0f, 1.0f, 0.0f);
	m_Vertices.SetColor   (10, 0xffffffff);
	m_Vertices.SetTexCoord(10, 1.0f, 0.0f);
	// vertex 11
	m_Vertices.SetVertex  (11, -x_ext_div, y_ext_div, z_ext_div);
	m_Vertices.SetNormal  (11, 0.0f, 1.0f, 0.0f);
	m_Vertices.SetColor   (11, 0xffffffff);
	m_Vertices.SetTexCoord(11, 0.0f, 0.0f);
	// bottom face
	// vertex 12
	m_Vertices.SetVertex  (12, -x_ext_div, -y_ext_div, -z_ext_div);
	m_Vertices.SetNormal  (12, 0.0f, -1.0f, 0.0f);
	m_Vertices.SetColor   (12, 0xffffffff);
	m_Vertices.SetTexCoord(12, 0.0f, 0.0f);
	// vertex 13
	m_Vertices.SetVertex  (13, x_ext_div, -y_ext_div, -z_ext_div);
	m_Vertices.SetNormal  (13, 0.0f, -1.0f, 0.0f);
	m_Vertices.SetColor   (13, 0xffffffff);
	m_Vertices.SetTexCoord(13, 1.0f, 0.0f);
	// vertex 14
	m_Vertices.SetVertex  (14, x_ext_div, -y_ext_div, z_ext_div);
	m_Vertices.SetNormal  (14, 0.0f, -1.0f, 0.0f);
	m_Vertices.SetColor   (14, 0xffffffff);
	m_Vertices.SetTexCoord(14, 1.0f, 1.0f);
	// vertex 15
	m_Vertices.SetVertex  (15, -x_ext_div, -y_ext_div, z_ext_div);
	m_Vertices.SetNormal  (15, 0.0f, -1.0f, 0.0f);
	m_Vertices.SetColor   (15, 0xffffffff);
	m_Vertices.SetTexCoord(15, 0.0f, 1.0f);
	// left face
	// vertex 16
	m_Vertices.SetVertex  (16, -x_ext_div, -y_ext_div, -z_ext_div);
	m_Vertices.SetNormal  (16, -1.0, 0.0f, 0.0f);
	m_Vertices.SetColor   (16, 0xffffffff);
	m_Vertices.SetTexCoord(16, 0.0f, 0.0f);
	// vertex 17
	m_Vertices.SetVertex  (17, -x_ext_div, -y_ext_div, z_ext_div);
	m_Vertices.SetNormal  (17, -1.0, 0.0f, 0.0f);
	m_Vertices.SetColor   (17, 0xffffffff);
	m_Vertices.SetTexCoord(17, 1.0f, 0.0f);
	// vertex 18
	m_Vertices.SetVertex  (18, -x_ext_div, y_ext_div, z_ext_div);
	m_Vertices.SetNormal  (18, -1.0, 0.0f, 0.0f);
	m_Vertices.SetColor   (18, 0xffffffff);
	m_Vertices.SetTexCoord(18, 1.0f, 1.0f);
	// vertex 19
	m_Vertices.SetVertex  (19, -x_ext_div, y_ext_div, -z_ext_div);
	m_Vertices.SetNormal  (19, -1.0, 0.0f, 0.0f);
	m_Vertices.SetColor   (19, 0xffffffff);
	m_Vertices.SetTexCoord(19, 0.0f, 1.0f);
	// right face
	// vertex 20
	m_Vertices.SetVertex  (20, x_ext_div, -y_ext_div, -z_ext_div);
	m_Vertices.SetNormal  (20, 1.0, 0.0f, 0.0f);
	m_Vertices.SetColor   (20, 0xffffffff);
	m_Vertices.SetTexCoord(20, 0.0f, 0.0f);
	// vertex 21
	m_Vertices.SetVertex  (21, x_ext_div, -y_ext_div, z_ext_div);
	m_Vertices.SetNormal  (21, 1.0, 0.0f, 0.0f);
	m_Vertices.SetColor   (21, 0xffffffff);
	m_Vertices.SetTexCoord(21, 0.0f, 1.0f);
	// vertex 22
	m_Vertices.SetVertex  (22, x_ext_div, y_ext_div, z_ext_div);
	m_Vertices.SetNormal  (22, 1.0, 0.0f, 0.0f);
	m_Vertices.SetColor   (22, 0xffffffff);
	m_Vertices.SetTexCoord(22, 1.0f, 1.0f);
	// vertex 23
	m_Vertices.SetVertex  (23, x_ext_div, y_ext_div, -z_ext_div);
	m_Vertices.SetNormal  (23, 1.0, 0.0f, 0.0f);
	m_Vertices.SetColor   (23, 0xffffffff);
	m_Vertices.SetTexCoord(23, 1.0f, 0.0f);
	// end
	m_Vertices.UnLockBuffer(m_pDevice);

	m_Indices.LockBuffer(m_pDevice);
	// front face
	m_Indices.SetIndex(0, 0); m_Indices.SetIndex(1, 1); m_Indices.SetIndex(2, 2);
	m_Indices.SetIndex(3, 0); m_Indices.SetIndex(4, 2); m_Indices.SetIndex(5, 3);
	// back face
	m_Indices.SetIndex(6, 4); m_Indices.SetIndex(7, 6); m_Indices.SetIndex(8, 5);
	m_Indices.SetIndex(9, 4); m_Indices.SetIndex(10, 7); m_Indices.SetIndex(11, 6);
	// top face
	m_Indices.SetIndex(12, 8); m_Indices.SetIndex(13, 10); m_Indices.SetIndex(14, 9);
	m_Indices.SetIndex(15, 8); m_Indices.SetIndex(16, 11); m_Indices.SetIndex(17, 10);
	// bottom face
	m_Indices.SetIndex(18, 12); m_Indices.SetIndex(19, 13); m_Indices.SetIndex(20, 14);
	m_Indices.SetIndex(21, 12); m_Indices.SetIndex(22, 14); m_Indices.SetIndex(23, 15);
	// left face
	m_Indices.SetIndex(24, 16); m_Indices.SetIndex(25, 17); m_Indices.SetIndex(26, 18);
	m_Indices.SetIndex(27, 16); m_Indices.SetIndex(28, 18); m_Indices.SetIndex(29, 19);
	// right face
	m_Indices.SetIndex(30, 20); m_Indices.SetIndex(31, 22); m_Indices.SetIndex(32, 21);
	m_Indices.SetIndex(33, 20); m_Indices.SetIndex(34, 23); m_Indices.SetIndex(35, 22);
	// end
	m_Indices.UnLockBuffer(m_pDevice);

	return 1;
};

int RtgGeometry::CreateParticalCircle(float radius, float ts, float te)
{
	float theta_total = DegreeToRadian(te-ts);
	float theta_inc = (theta_total)/m_iPieces;
	float theta;
	int num_vertices = m_iPieces + 2;
	int num_triangles = m_iPieces;
	int i;

	if ( !CreateResource(num_vertices, num_triangles, num_triangles*3) )
		return 0;

	m_Vertices.LockBuffer(m_pDevice);

	m_Vertices.SetVertex  (0, 0.0f, 0.0f, 0.0f);
	m_Vertices.SetNormal  (0, 0.0f, 0.0f, 1.0f);
	m_Vertices.SetColor   (0, 0xffffffff);
	m_Vertices.SetTexCoord(0, 0.5f, 0.5f);

	theta = DegreeToRadian(ts);
	for ( i=1; i<num_vertices; i++ )
	{
		m_Vertices.SetVertex  (i, radius * fcos(theta), radius * fsin(theta), 0.0f);
		m_Vertices.SetColor   (i, 0xffffffff);
		m_Vertices.SetNormal  (i, 0.0f, 0.0f, 1.0f);
		m_Vertices.SetTexCoord(i, (fcos(theta)+1.0f)*0.5f, (fsin(theta)+1.0f)*0.5f);
		theta += theta_inc;
	}
	m_Vertices.UnLockBuffer(m_pDevice);

    int index = 0;
	m_Indices.LockBuffer(m_pDevice);
	for ( i=1; i<=m_iPieces; i++ )
	{
		m_Indices.SetIndex(index++, 0);
		m_Indices.SetIndex(index++, i);
		m_Indices.SetIndex(index++, i+1);
	}
	m_Indices.UnLockBuffer(m_pDevice);

    return 1;
}

int RtgGeometry::CreateCircle(float radius)
{
	return CreateParticalCircle(radius, 90.0f, 450.0f);
}

int RtgGeometry::CreateCone(float r, float h)
{
	CreateCylinder(r, 0, h);
	return 0;
}

int RtgGeometry::CreateParticalCylinder(float ts, float te, float radius1, float radius2, float height)
{
	int num_vertices = (m_iPieces+1)*2;
	int num_triangles = m_iPieces*2;
	
	if ( !CreateResource(num_vertices, num_triangles, num_triangles*3) )
		return 0;

	float theta_total = DegreeToRadian(te-ts);
	float theta_inc = (theta_total)/m_iPieces;
	float theta = DegreeToRadian(ts);

	int index = 0;
	m_Vertices.LockBuffer(m_pDevice);
	for ( int i=0; i<=m_iPieces; i++, theta += theta_inc )
	{
		// bottom vertex
		m_Vertices.SetVertex( index, radius1 * fcos(theta), radius1 * fsin(theta), -height/2.0f);
		// bottom normal
		m_Vertices.SetNormal( index, fcos(theta), fsin(theta), 0.0f);
		// bottom color
		m_Vertices.SetColor ( index, 0xffffffff);
		// bottom vertex tex coord
		m_Vertices.SetTexCoord( index, (float)i/(float)m_iPieces, 1.0f);
		index++;

		// top vertex position
		m_Vertices.SetVertex( index, radius2 * fcos(theta), radius2 * fsin(theta), height/2.0f);
		// top vertex normal
		m_Vertices.SetNormal( index, fcos(theta), fsin(theta), 0.0f );
		// top vertex tex coord
		m_Vertices.SetTexCoord( index, (float)i/(float)m_iPieces, 0.0f);
		index++;
	}
	m_Vertices.UnLockBuffer(m_pDevice);

	int base = 0;
	index = 0;
	m_Indices.LockBuffer(m_pDevice);
	for ( int i=0; i<m_iPieces; i++ )
	{
		m_Indices.SetIndex(index++, base);
		m_Indices.SetIndex(index++, base+2);
		m_Indices.SetIndex(index++, base+1);
		base++;

		m_Indices.SetIndex(index++, base);
		m_Indices.SetIndex(index++, base+1);
		m_Indices.SetIndex(index++, base+2);
		base++;
	}
	m_Indices.UnLockBuffer(m_pDevice);

	return 1;
}

int RtgGeometry::CreateCylinder(float r1, float r2, float h)
{
	return CreateParticalCylinder(90.0f, 450.0f, r1, r2, h);
}

int RtgGeometry::CreateParticalSphere(float radius, float ts, float te, float ps, float pe)
{
	int num_vertices = (m_iStacks+1)*(m_iPieces+1);
	int num_triangles = m_iStacks*m_iPieces*2;
	if ( !CreateResource(num_vertices, num_triangles, num_triangles*3) )
		return 0;

	float theta_total = DegreeToRadian(te - ts);
	float phi_total = DegreeToRadian(pe - ps);
	float theta_inc = (theta_total)/m_iStacks;
	float phi_inc = (phi_total)/m_iPieces;

	int i,j;
	int index = 0;
	float theta = DegreeToRadian(ts);

	m_Vertices.LockBuffer(m_pDevice);
	for ( i=0; i<=m_iStacks; i++ )
	{
		float phi = DegreeToRadian(ps);
		float cos_theta = fcos(theta);
		float sin_theta = fsin(theta);
		for ( j=0; j<=m_iPieces; j++, index++ )
		{
			float cos_phi = fcos(phi);
			float sin_phi = fsin(phi);
			// vertex
			m_Vertices.SetVertex( index, (radius * cos_theta) * cos_phi, (radius * cos_theta) * sin_phi, radius * sin_theta );
			// normal
			RtgVertex3 n( (radius * cos_theta) * cos_phi, (radius * cos_theta) * sin_phi, radius * sin_theta );
			n.Normalize();
			m_Vertices.SetNormal( index, n);
			// color
			m_Vertices.SetColor( index, 0xffffffff);
			// texcoord
			m_Vertices.SetTexCoord( index, (float)j/(float)(m_iPieces), (float)i/(float)(m_iStacks) );
			phi += phi_inc;
		}
		theta += theta_inc;
	}
	m_Vertices.UnLockBuffer(m_pDevice);

	int base = 0;
	index = 0;
	m_Indices.LockBuffer(m_pDevice);
	for ( i=0; i<m_iStacks; i++ )
	{
		for ( j=0; j<m_iPieces; j++ )
		{
			m_Indices.SetIndex(index++, base);
			m_Indices.SetIndex(index++, base+1);
			m_Indices.SetIndex(index++, base+m_iPieces+1);

			m_Indices.SetIndex(index++, base+1);
			m_Indices.SetIndex(index++, base+m_iPieces+1+1);
			m_Indices.SetIndex(index++, base+m_iPieces+1);
			
			base++;
		}
		base++;
	}
	m_Indices.UnLockBuffer(m_pDevice);

	return 1;
}

int RtgGeometry::CreateSphere(float radius)
{
	return CreateParticalSphere(radius, -90.0f, 90.0f, 90.0f, 450.0f);
}

void RtgGeometry::CreateOneStack(int stack, float InnerRadius, float OuterRadius, float ts, float te, 
					float ps, float pe, int vbase, int ibase)
{
	float phi_total = DegreeToRadian(pe-ps);
	float phi_inc = phi_total/m_iPieces;
	float phi = DegreeToRadian(pe);

	RtgVertex3 center0, center1;
	RtgVertex3 vec0( (float) cos(ts), (float) sin(ts), 0.0f );
	RtgVertex3 vec1( (float) cos(te), (float) sin(te), 0.0f );
	RtgVertex3 vecy(0.0f, 0.0f, 1.0f);
	RtgVertex3 vecz;

	float ir = (OuterRadius - InnerRadius)/2.0f;
	float radius = (InnerRadius+OuterRadius)/2.0f;
	center0.Set( (float)(radius * cos(ts)), (float)(radius *sin(ts)), 0.0f );
	center1.Set( (float)(radius * cos(te)), (float)(radius *sin(te)), 0.0f );

	RtgMatrix16 matrix0, matrix1;
	matrix0.Unit();
	matrix1.Unit();

    rtgVectorCross(vecz, vec0, vecy);
	matrix0.SetRow(0, vec0);
	matrix0.SetRow(1, vecy);
	matrix0.SetRow(2, vecz);
	matrix0.SetRow(3, center0);

    rtgVectorCross(vecz, vec1, vecy);
	matrix1.SetRow(0, vec1);
	matrix1.SetRow(1, vecy);
	matrix1.SetRow(2, vecz);
	matrix1.SetRow(3, center1);

	RtgVertex3 vector;
	RtgVertex3 output;
	RtgVertex3 normal;

	int index = vbase;
	m_Vertices.LockBuffer(m_pDevice);
	for ( int i=0; i<=m_iPieces; i++ )
	{
		vector.Set( (float)cos(phi), (float)sin(phi), 0.0f);
		output = (vector * ir) * matrix0;
		normal = output - center0;
		normal.Normalize();

		m_Vertices.SetVertex( index, output );
		m_Vertices.SetNormal( index, normal);
		m_Vertices.SetColor ( index, 0xffffffff);
		m_Vertices.SetTexCoord( index, (float)stack/(float)m_iStacks, (float)i/(float)m_iPieces );
		index++;

		vector.Set( (float)cos(phi), (float)sin(phi), 0.0f);
		output = (vector*ir) * matrix1;
		normal = output - center1;
		normal.Normalize();

		m_Vertices.SetVertex( index, output );
		m_Vertices.SetNormal( index, normal );
		m_Vertices.SetColor ( index, 0xffffffff );
		m_Vertices.SetTexCoord( index, (float)(stack+1)/(float)m_iStacks, (float)i/(float)m_iPieces );
		index++;

		phi += phi_inc;
	}
	m_Vertices.UnLockBuffer(m_pDevice);

	index = ibase;
	int base = stack * m_iPieces * 2;
	m_Indices.LockBuffer(m_pDevice);
	for ( int i=0; i<m_iPieces; i++ )
	{
		m_Indices.SetIndex(index++, base);
		m_Indices.SetIndex(index++, base+1);
		m_Indices.SetIndex(index++, base+2);
		base++;

		m_Indices.SetIndex(index++, base);
		m_Indices.SetIndex(index++, base+2);
		m_Indices.SetIndex(index++, base+1);
		base++;
	}
	m_Indices.UnLockBuffer(m_pDevice);
}

int RtgGeometry::CreateParticalDonut(float InnerRadius, float OuterRadius, 
								   float ts, float te, float ps, float pe)
{
	int num_vertices = m_iStacks*(m_iPieces+1)*2;
	int num_triangles = m_iStacks*m_iPieces*2;
	if ( !CreateResource(num_vertices, num_triangles, num_triangles*3) )
		return 0;

	float theta_total = DegreeToRadian(te - ts);
	float theta = DegreeToRadian(ts);
	float theta_inc = (theta_total)/(m_iStacks-1);

	int vbase = 0;
	int ibase = 0;
	for ( int i=0; i<m_iStacks; i++ )
	{
		CreateOneStack(i, InnerRadius, OuterRadius, theta, theta+theta_inc, ps, pe, vbase, ibase);
		vbase += (m_iPieces+1)*2;
		ibase += m_iPieces*2*3;
		theta += theta_inc;
	}
	return 1;
}

int RtgGeometry::CreateDonut(float ir, float or)
{
	return CreateParticalDonut(ir, or, 0.0f, 360.0f, 0.0f, 360.0f);
}

void RtgGeometry::FadeSegment(int r, int g, int b)
{
	int grids = m_iNumVertices/4;
	int vbase = grids * 2;
	m_Vertices.LockBuffer(m_pDevice);
	for ( int i=0; i<grids; i++ )
	{
		int base = i*4;
		int a0 = 255*(grids-i)/vbase;
		int c0 = RtgGetColor(r,g,b,a0);
		int a1 = 255*(grids-i-1)/vbase;
		int c1 = RtgGetColor(r,g,b,a1);

		m_Vertices.SetColor(base, c0);
		m_Vertices.SetColor(base+1, c1);
		m_Vertices.SetColor(base+2, c1);
		m_Vertices.SetColor(base+3, c0);
	}
	m_Vertices.UnLockBuffer(m_pDevice);
}

void RtgGeometry::FadeSegment(float r, float g, float b)
{
	FadeSegment( (int) (r*255), (int) (g*255), (int) (b*255) );
}

int RtgGeometry::CreateSegment(int grids, float x_ext, float y_ext)
{
	float xinc = x_ext/grids;
	float y_grid = y_ext/2.0f;

	float x = -x_ext/2.0f;
	float nx = x+xinc;
	
	float t = 0.0f;
	float tinc = 1.0f/grids;
	float nt = t+tinc;

	if ( !CreateResource(grids*4, grids*2, grids*2*3) )
		return 0;

	int index = 0;
	int iindex = 0;
	m_Vertices.LockBuffer(m_pDevice);
	m_Indices.LockBuffer(m_pDevice);
	for ( int i=0; i<grids; i++ )
	{
		// vertex a
		m_Vertices.SetVertex(index, x, -y_grid, 0.0f);
		m_Vertices.SetNormal(index, 0.0f, 0.0f, 1.0f);
		m_Vertices.SetColor(index, 0xffffffff);
		m_Vertices.SetTexCoord(index, t, 0.0f);
		index++;
		// vertex b
		m_Vertices.SetVertex(index, nx, -y_grid, 0.0f);
		m_Vertices.SetNormal(index, 0.0f, 0.0f, 1.0f);
		m_Vertices.SetColor(index, 0xffffffff);
		m_Vertices.SetTexCoord(index, nt, 0.0f);
		index++;
		// vertex c
		m_Vertices.SetVertex(index, nx, y_grid, 0.0f);
		m_Vertices.SetNormal(index, 0.0f, 0.0f, 1.0f);
		m_Vertices.SetColor(index, 0xffffffff);
		m_Vertices.SetTexCoord(index, nt, 1.0f);
		index++;
		// vertex d
		m_Vertices.SetVertex(index, x, y_grid, 0.0f);
		m_Vertices.SetNormal(index, 0.0f, 0.0f, 1.0f);
		m_Vertices.SetColor(index, 0xffffffff);
		m_Vertices.SetTexCoord(index, t, 1.0f);
		index++;
		// increment
		x = nx;
		nx += xinc;
		t = nt;
		nt += tinc;
		// topology
		int vbase = i*4;

		m_Indices.SetIndex(iindex++, vbase);
		m_Indices.SetIndex(iindex++, vbase+1);
		m_Indices.SetIndex(iindex++, vbase+2);

		m_Indices.SetIndex(iindex++, vbase);
		m_Indices.SetIndex(iindex++, vbase+2);
		m_Indices.SetIndex(iindex++, vbase+3);
	}
	m_Vertices.UnLockBuffer(m_pDevice);
	m_Indices.UnLockBuffer(m_pDevice);
	return 1;
}

void RtgGeometry::ChangeVertexColor(int r, int g, int b, int a)
{
	int color = RtgGetColor(r,g,b,a);

	m_Vertices.LockBuffer(m_pDevice);
	for ( int i=0; i<m_iNumVertices; i++ )
	{
		m_Vertices.SetColor(i, color);
	}
	m_Vertices.UnLockBuffer(m_pDevice);
}

void RtgGeometry::ChangeVertexColor(float r, float g, float b, float a)
{
	int color = RtgGetColor(r,g,b,a);

	m_Vertices.LockBuffer(m_pDevice);
	for ( int i=0; i<m_iNumVertices; i++ )
	{
		m_Vertices.SetColor(i, color);
	}
	m_Vertices.UnLockBuffer(m_pDevice);
}

void RtgGeometry::ChangeTexcoord(float s, float t, int index)
{
	m_Vertices.LockBuffer(m_pDevice);
	for ( int i=0; i<m_iNumVertices; i++ )
	{
		float t0, t1;
		m_Vertices.GetTexCoord(i, t0, t1, index);
		m_Vertices.SetTexCoord(i, t0+s, t1+t, index);
	}
	m_Vertices.UnLockBuffer(m_pDevice);
}

void RtgGeometry::ChangeVertexPosition(RtgMatrix16 matrix)
{
	unsigned char *p = (unsigned char *)m_Vertices.LockBuffer(m_pDevice);
	for ( int i=0; i<m_iNumVertices; i++ )
	{
		RtgVertex3 *v = (RtgVertex3 *) p;
		*v = *v * matrix;
		p += m_VertexSpec.iNumVertexSize;
	}
	m_Vertices.UnLockBuffer(m_pDevice);
}

void RtgGeometry::UpdateNode(int node, int mask)
{
}

void RtgGeometry::SetBumpmapLight(RtgVertex3 &light, RTGLightMode mode)
{
	if ( m_VertexSpec.iColorOffset==-1 )
		return;

	unsigned char *buffer = (unsigned char *)m_Vertices.LockBuffer(m_pDevice);
	for (int i=0; i<m_iNumVertices; i++, buffer+=m_VertexSpec.iNumVertexSize )
	{
		RtgVertex3 ref;
		RtgMatrix16 matrix; matrix.Identity();
		RtgVertex3 tang[3];
		GetTangentSpace(i, tang);

		switch(mode)
		{
		case RTG_LIGHT_POINT:
			{
				RtgVertex3 *vector = (RtgVertex3 *)buffer;
				ref = light - *vector;
				ref.Normalize();
				break;
			}
		case RTG_LIGHT_DIRECTION:
			ref = light;
			break;
		}

		matrix.SetCol(0, tang[0]);
		matrix.SetCol(1, tang[1]);
		matrix.SetCol(2, tang[2]);
		
		RtgVertex3 vec = ref * matrix;

		int *color = (int *) (buffer + m_VertexSpec.iColorOffset);
		*color = RtgVectorToColor(vec, 0.0f);
	}
	m_Vertices.UnLockBuffer(m_pDevice);
}

void RtgGeometry::Copy(RtgGeometry &src)
{
	src.ReleaseResource();
	src.SetVertexFormat(m_eVertexFormat);
	src.SetResourceType(m_eMType);
	src.CreateResource(m_iNumVertices, m_iNumFaces, m_iNumIndices);
	
	void *psrc = m_Vertices.LockBuffer(m_pDevice);
	void *pdst = src.m_Vertices.LockBuffer(m_pDevice);
	  memcpy(pdst, psrc, sizeof(m_VertexSpec.iNumVertexSize * m_iNumVertices) );
	m_Vertices.UnLockBuffer(m_pDevice);
	src.m_Vertices.UnLockBuffer(m_pDevice);

	psrc = m_Indices.LockBuffer(m_pDevice);
	pdst = src.m_Indices.LockBuffer(m_pDevice);
	  memcpy(pdst, psrc, sizeof(short)*m_iNumIndices);
	m_Indices.UnLockBuffer(m_pDevice);
	src.m_Indices.UnLockBuffer(m_pDevice);
}


} // namespace rt_graph
/*----------------------------------------------------------------------------
-   The End.
----------------------------------------------------------------------------*/
