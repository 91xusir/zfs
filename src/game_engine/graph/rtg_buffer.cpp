
#include "rtg_graph_inter.h"
namespace rt_graph {

RtgBuffer::RtgBuffer()
{
    m_pDevice = NULL;
    m_eMType = RTG_SYSTEM_MEMORY;
    m_eVType = RTGBT_VERTEX_BUFFER;
    m_pBuffer = NULL;
    m_pLockedBuffer = NULL;
    m_iNumElements = 0;
    SetVertexFormat(RTG_VCT);
}

RtgBuffer::~RtgBuffer()
{
    ReleaseBuffer(m_pDevice);
}

int RtgBuffer::SetVertexFormat(int vsize, int normal, int color, int numtex, int *texcoords)
{
    m_VertexSpec.iNumVertexCoords = vsize;
    m_VertexSpec.iNumVertexSize = vsize * sizeof(float);
    m_VertexSpec.iVertexOffset = 0;

    if ( normal )
    {
        m_VertexSpec.iNormalOffset = m_VertexSpec.iNumVertexSize;
        m_VertexSpec.iNumVertexSize += 3 * sizeof(float);
    }else
    {
        m_VertexSpec.iNormalOffset = -1;
    }

    if ( color )
    {
        m_VertexSpec.iColorOffset = m_VertexSpec.iNumVertexSize;
        m_VertexSpec.iNumVertexSize += 4;
    }else
    {
        m_VertexSpec.iColorOffset = -1;
    }

    m_VertexSpec.iNumTexCoords = numtex;
    if ( numtex )
    {
        m_VertexSpec.iTexOffset = m_VertexSpec.iNumVertexSize;
        if ( texcoords )
        {
            for ( int i=0; i<numtex; i++ )
            {
                m_VertexSpec.iTexCoordList[i] = texcoords[i];
                m_VertexSpec.iNumVertexSize += sizeof(float) * m_VertexSpec.iTexCoordList[i];
            }
        }else
        {
            for ( int i=0; i<numtex; i++ )
            {
                m_VertexSpec.iTexCoordList[i] = 2;
                m_VertexSpec.iNumVertexSize += sizeof(float) * m_VertexSpec.iTexCoordList[i];
            }
        }
        m_VertexSpec.TexOffset[0] = m_VertexSpec.iTexOffset;
		int i = 1;
        for ( ; i<numtex; i++ )
        {
            m_VertexSpec.TexOffset[i] = m_VertexSpec.TexOffset[i-1] + m_VertexSpec.iTexCoordList[i-1] * 4;
        }
#ifdef _DEBUG
        for ( ; i<RTG_MAX_MULTITEXTURES; i++ )
        {
            m_VertexSpec.TexOffset[i] = -1;
        }
#endif
    }else
    {
        m_VertexSpec.iTexOffset = -1;
    }
    return 1;
}

void RtgBuffer::SetVertexFormat(RTGVertexFormat fmt)
{
    static int texlist[] = { 2,2,2,2,2,2,2,2 };
    switch(fmt)
    {
    case RTG_VNCT:
        SetVertexFormat(3, 1, 1, 1, texlist);
        break;
    case RTG_VNCT2:
        SetVertexFormat(3, 1, 1, 2, texlist);
        break;
    case RTG_VNCT3:
        SetVertexFormat(3, 1, 1, 3, texlist);
        break;
    case RTG_VNCT4:
        SetVertexFormat(3, 1, 1, 4, texlist);
        break;
    case RTG_VNT:
        SetVertexFormat(3, 1, 0, 1, texlist);
        break;
    case RTG_VNT2:
        SetVertexFormat(3, 1, 0, 2, texlist);
        break;
    case RTG_VNT3:
        SetVertexFormat(3, 1, 0, 3, texlist);
        break;
    case RTG_VNT4:
        SetVertexFormat(3, 1, 0, 4, texlist);
        break;
    case RTG_VCT:
        SetVertexFormat(3, 0, 1, 1, texlist);
        break;
    case RTG_VCT2:
        SetVertexFormat(3, 0, 1, 2, texlist);
        break;
    case RTG_VCT3:
        SetVertexFormat(3, 0, 1, 3, texlist);
        break;
    case RTG_VCT4:
        SetVertexFormat(3, 0, 1, 4, texlist);
        break;
    case RTG_VN:
        SetVertexFormat(3, 1, 0, 0, texlist);
        break;
    case RTG_VC:
        SetVertexFormat(3, 0, 1, 0, texlist);
        break;
    case RTG_VT:
        SetVertexFormat(3, 0, 0, 1, texlist);
        break;
    case RTG_VT2:
        SetVertexFormat(3, 0, 0, 2, texlist);
        break;
    case RTG_TRANSFORMED_VC:
        SetVertexFormat(4, 0, 1, 0, texlist);
        break;
    case RTG_TRANSFORMED_VT:
        SetVertexFormat(4, 0, 0, 1, texlist);
        break;
    case RTG_TRANSFORMED_VT2:
        SetVertexFormat(4, 0, 0, 2, texlist);
        break;
    case RTG_TRANSFORMED_VCT:
        SetVertexFormat(4, 0, 1, 1, texlist);
        break;
    case RTG_TRANSFORMED_VCT2:
        SetVertexFormat(4, 0, 1, 2, texlist);
        break;
    }
}

void RtgBuffer::SetVertexFormat(RtgVertexSpec fmt)
{
    m_VertexSpec = fmt;
}

void RtgBuffer::SetResourceType(RTGMemoryType mtype, RTGBufferType vtype)
{
    m_eMType = mtype;
    m_eVType = vtype;
}

void *RtgBuffer::AllocateElements(RtgDevice* pDevice, int items)
{
    int dwSize;
    m_iNumElements = items;
    switch(m_eVType)
    {
    case RTGBT_VERTEX_BUFFER:
        dwSize = m_VertexSpec.iNumVertexSize;
        break;
    case RTGBT_INDEX_BUFFER:
        dwSize = sizeof(short);
        break;
    }
    return AllocateBuffer( pDevice, items*dwSize );
}

void *RtgBuffer::AllocateBuffer(RtgDevice* pDevice, int dwSize)
{
    ReleaseBuffer(m_pDevice);
    m_pDevice = pDevice;
    switch(m_eVType)
    {
    case RTGBT_VERTEX_BUFFER:
        switch(m_eMType)
        {
        case RTG_VIDEO_MEMORY:
        case RTG_SYSTEM_MEMORY:
            RtgSetCreateBufferDebugInfo();
            m_pBuffer = RtGetRender()->GetBuffMgr()->CreateBuffer(dwSize);
            break;
        }
        break;
    case RTGBT_INDEX_BUFFER:
        switch(m_eMType)
        {
        case RTG_VIDEO_MEMORY:
        case RTG_SYSTEM_MEMORY:
            RtgSetCreateBufferDebugInfo();
            m_pBuffer = RtGetRender()->GetBuffMgr()->CreateBuffer(dwSize, D3DFMT_INDEX16);
            break;
        }
    }
    return m_pBuffer;
}

int RtgBuffer::ReleaseBuffer(RtgDevice* pDevice)
{
    RtgReleaseBuffer(m_pBuffer);
   /* if ( m_pBuffer==NULL )
        return 1;
    CHECK(pDevice!=NULL);

    switch(m_eVType)
    {
    case RTGBT_VERTEX_BUFFER:
        switch(m_eMType)
        {
        case RTG_VIDEO_MEMORY:
            if (pDevice->ReleaseVertexBuffer(m_pBuffer))
            {
                RtCoreLog().Warn("ERROR: Release Vertex Buffer Error.\n");
            }
            m_pBuffer = NULL;
            break;
        case RTG_SYSTEM_MEMORY:
            RtCoreMem().Free(m_pBuffer, __FILE__, __LINE__);
			m_pBuffer = NULL;
            break;
        }
        break;
    case RTGBT_INDEX_BUFFER:
        switch(m_eMType)
        {
        case RTG_VIDEO_MEMORY:
            if (pDevice->ReleaseIndexBuffer(m_pBuffer))
            {
                RtCoreLog().Warn("ERROR: Release Index Buffer Error.\n");
            }
            m_pBuffer = NULL;
            break;
        case RTG_SYSTEM_MEMORY:
            RtCoreMem().Free(m_pBuffer, __FILE__, __LINE__);
			m_pBuffer = NULL;
            break;
        }
        break;
    }*/
    return 1;
}

void *RtgBuffer::LockBuffer(RtgDevice* pDevice)
{
    if (!m_pBuffer)
        return NULL;

    m_pLockedBuffer = (unsigned char*)RtGetRender()->GetBuffMgr()->Lock(m_pBuffer);
    return m_pLockedBuffer;

  /*  if (m_pBuffer==NULL) return NULL;
    m_pLockedBuffer = NULL;
    switch(m_eVType)
    {
    case RTGBT_VERTEX_BUFFER:
        switch(m_eMType)
        {
        case RTG_VIDEO_MEMORY:
            pDevice->LockVertexBuffer(m_pBuffer, (void**)(&m_pLockedBuffer));
            break;
        case RTG_SYSTEM_MEMORY:
            m_pLockedBuffer = (unsigned char *)m_pBuffer;
            break;
        }
        break;
    case RTGBT_INDEX_BUFFER:
        switch(m_eMType)
        {
        case RTG_VIDEO_MEMORY:
            pDevice->LockIndexBuffer(m_pBuffer, (void**)(&m_pLockedBuffer));
            break;
        case RTG_SYSTEM_MEMORY:
            m_pLockedBuffer = (unsigned char *)m_pBuffer;
            break;
        }
        break;
    }
    return m_pLockedBuffer;*/
}

int RtgBuffer::UnLockBuffer(RtgDevice* pDevice)
{
    RtGetRender()->GetBuffMgr()->Unlock(m_pBuffer);
    //if ( m_pBuffer==NULL ) return FALSE;
    //m_pLockedBuffer = NULL;
    //switch(m_eVType)
    //{
    //case RTGBT_VERTEX_BUFFER:
    //    switch(m_eMType)
    //    {
    //    case RTG_VIDEO_MEMORY:
    //        return pDevice->UnLockVertexBuffer(m_pBuffer);
    //    case RTG_SYSTEM_MEMORY:
    //        break;
    //    }
    //    break;
    //case RTGBT_INDEX_BUFFER:
    //    switch(m_eMType)
    //    {
    //    case RTG_VIDEO_MEMORY:
    //        return pDevice->UnLockIndexBuffer(m_pBuffer);
    //    case RTG_SYSTEM_MEMORY:
    //        break;
    //    }
    //    break;
    //}
    return TRUE;
}

int RtgBuffer::GetVertex(int index, RtgVertex3 &vertex)
{
    if ( m_pLockedBuffer==NULL || index<0 || index>=m_iNumElements )
        return FALSE;

    float *v = (float *) (m_pLockedBuffer + m_VertexSpec.iNumVertexSize * index);
    vertex.Set(v[0], v[1], v[2]);

    return TRUE;
}

RtgVertex3 *RtgBuffer::GetVertex(int index)
{
    if ( m_pLockedBuffer==NULL || index<0 || index>=m_iNumElements )
        return NULL;

    return (RtgVertex3 *)(m_pLockedBuffer + m_VertexSpec.iNumVertexSize * index);
}

int RtgBuffer::SetVertex(int index, RtgVertex3 &vertex)
{
    return SetVertex(index, vertex.x, vertex.y, vertex.z);
}

int RtgBuffer::SetVertex(int index, float x, float y, float z)
{
    if ( m_pLockedBuffer==NULL || index<0 || index>=m_iNumElements )
        return 0;
    
    float *v = (float *) (m_pLockedBuffer + m_VertexSpec.iNumVertexSize * index);
    v[0] = x; v[1] = y; v[2] = z;

    return 1;
}

int RtgBuffer::GetNormal(int index, RtgVertex3 &normal)
{
    if ( m_pLockedBuffer==NULL || index<0 || index>=m_iNumElements || m_VertexSpec.iNormalOffset==-1 )
        return 0;
    
    float *n = (float *) (m_pLockedBuffer + m_VertexSpec.iNumVertexSize * index + m_VertexSpec.iNormalOffset);
    normal.Set(n[0], n[1], n[2]);

    return 1;
}

RtgVertex3 *RtgBuffer::GetNormal(int index)
{
    if ( m_pLockedBuffer==NULL || index<0 || index>=m_iNumElements || m_VertexSpec.iNormalOffset==-1 )
        return NULL;
    else
        return (RtgVertex3 *) (m_pLockedBuffer + m_VertexSpec.iNumVertexSize * index + m_VertexSpec.iNormalOffset);
}

int RtgBuffer::SetNormal(int index, RtgVertex3 &normal)
{
    if ( m_pLockedBuffer==NULL || index<0 || index>=m_iNumElements || m_VertexSpec.iNormalOffset==-1 )
        return 0;

    RtgVertex3 *n = (RtgVertex3 *) (m_pLockedBuffer + m_VertexSpec.iNumVertexSize * index + m_VertexSpec.iNormalOffset);
    *n = normal;

    return 1;
}

int RtgBuffer::SetNormal(int index, float x, float y, float z)
{
    if ( m_pLockedBuffer==NULL || index<0 || index>=m_iNumElements || m_VertexSpec.iNormalOffset==-1 )
        return 0;

    float *n = (float *) (m_pLockedBuffer + m_VertexSpec.iNumVertexSize * index + m_VertexSpec.iNormalOffset);
    n[0] = x; n[1] = y; n[2] = z;

    return 1;
}

int RtgBuffer::SetColor(int index, int color)
{
    if ( m_pLockedBuffer==NULL || index<0 || index>=m_iNumElements || m_VertexSpec.iColorOffset==-1 )
        return 0;

    memcpy(m_pLockedBuffer + m_VertexSpec.iNumVertexSize * index + m_VertexSpec.iColorOffset, &color, 4);

    return 1;
}

int RtgBuffer::SetColor(int index, float r, float g, float b, float a)
{
    return SetColor(index, RtgGetColor(r*255.f,g*255.f,b*255.f,a*255.f) );
}

void *RtgBuffer::GetColor(int index)
{
    if ( m_pLockedBuffer==NULL || index<0 || index>=m_iNumElements || m_VertexSpec.iColorOffset==-1 )
        return NULL;
    else
        return m_pLockedBuffer + m_VertexSpec.iNumVertexSize * index + m_VertexSpec.iColorOffset;
}

int RtgBuffer::SetTexCoord(int index, float x, float y, int channel)
{
    return SetTexCoord(index, x, y, 0.0f, channel);
}

int RtgBuffer::SetTexCoord(int index, float x, float y, float z, int channel)
{
    if ( m_pLockedBuffer==NULL || index<0 || index>=m_iNumElements || m_VertexSpec.iTexOffset==-1 )
        return 0;

    unsigned char *base = (m_pLockedBuffer + m_VertexSpec.iNumVertexSize * index + m_VertexSpec.iTexOffset);
    float *t = (float *) (base);

    if ( channel==-1 )
    {
        for ( int i=0; i<m_VertexSpec.iNumTexCoords; i++ )
        {
            if ( m_VertexSpec.iTexCoordList[i]>=1 )
            {
                *t = x; t++;
            }
            if ( m_VertexSpec.iTexCoordList[i]>=2 )
            {
                *t = y; t++;
            }
            if ( m_VertexSpec.iTexCoordList[i]>=3 )
            {
                *t = z; t++;
            }
            if ( m_VertexSpec.iTexCoordList[i]>=4 )
            {
                *t = 1.0f; t++;
            }
        }
    }
    else
    {
        for ( int i=1; i<=channel; i++ )
        {
            t += m_VertexSpec.iTexCoordList[i-1];
        }

        if ( m_VertexSpec.iTexCoordList[channel]>=1 )
        {
            *t = x; t++;
        }
        if ( m_VertexSpec.iTexCoordList[channel]>=2 )
        {
            *t = y; t++;
        }
        if ( m_VertexSpec.iTexCoordList[channel]>=3 )
        {
            *t = z; t++;
        }
        if ( m_VertexSpec.iTexCoordList[channel]>=4 )
        {
            *t = 1.0f; t++;
        }
    }

    return 1;
}

int RtgBuffer::GetTexCoord(int index, float &s, float &t, int channel)
{
    if ( m_pLockedBuffer==NULL || index<0 || index>=m_iNumElements || m_VertexSpec.iTexOffset==-1 )
        return 0;

    float *sp = (float *)m_pLockedBuffer;
    if ( channel >= 1 )
    {
        sp = (float *) (m_pLockedBuffer + index * m_VertexSpec.iNumVertexSize + m_VertexSpec.iTexOffset + m_VertexSpec.iTexCoordList[channel-1] * sizeof(float) );
    }
    else
    {
        sp = (float *) (m_pLockedBuffer + index * m_VertexSpec.iNumVertexSize + m_VertexSpec.iTexOffset);
    }
    s = sp[0];
    t = sp[1];

    return 1;
}

void *RtgBuffer::GetTexCoord(int index)
{
    if ( m_pLockedBuffer==NULL || index<0 || index>=m_iNumElements || m_VertexSpec.iTexOffset==-1 )
        return NULL;
    else
        return (m_pLockedBuffer + index * m_VertexSpec.iNumVertexSize + m_VertexSpec.iTexOffset);
}

int RtgBuffer::SetIndex(int index, int value)
{
    if ( m_pLockedBuffer==NULL || index<0 || index>=m_iNumElements ) return 0;
    *(short *) (m_pLockedBuffer + index*sizeof(short) ) = value;
    return 1;
}

int RtgBuffer::GetIndex(int index, int &value)
{
    if ( m_pLockedBuffer==NULL || index<0 || index>=m_iNumElements ) return 0;
    value = *(short *) (m_pLockedBuffer + index*sizeof(short) );
    return 1;
}

} // namespace rt_graph
/*----------------------------------------------------------------------------
-   The End.
----------------------------------------------------------------------------*/
