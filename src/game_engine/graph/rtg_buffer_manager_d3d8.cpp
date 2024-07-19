
#include "rtg_graph_inter.h"
#include "rtg_buffer_manager_d3d8.h"
namespace rt_graph {

/*----------------------------------------------------------------------------
-   class D3DVertexStreamManager.
----------------------------------------------------------------------------*/

D3DVertexStreamManager::~D3DVertexStreamManager()
{
    OnReleaseStreamManager();
}

BOOL D3DVertexStreamManager::OnCreateStreamManager()
{
    DWORD dwTotalBuffer  = m_pStreamMgr->GetTotalBuffer();
    //DWORD totalDBuffer = m_pStreamMgr->GetTotalDynamicBuffer();
    //DWORD totalSBuffer = m_pStreamMgr->GetTotalStaticBuffer();

    CHECK(dwTotalBuffer>0);

    OnReleaseStreamManager();

    m_pVertexBuffer = (void**)RtCoreMem().Alloc(sizeof(void*)*(dwTotalBuffer+1), __FILE__, __LINE__);

    DWORD dwUsage = RTGU_DYNAMIC | RTGU_WRITEONLY | (m_pDevice->m_bVertexProcessing?RTGU_SOFTWAREPROCESSING:0);

    DWORD dwBufferID;
    for (dwBufferID=1; dwBufferID<=dwTotalBuffer; dwBufferID++)
    {
        RtgDynamicStreamManager::SDynamicStream* pBuffer = m_pStreamMgr->GetBuffer(dwBufferID);
        CHECK(pBuffer!=NULL);
        if (!m_pDevice->CreateVertexBuffer(pBuffer->dwTotalSize, &(m_pVertexBuffer[dwBufferID]), dwUsage))
        {
            return FALSE;
        }
    }
    return TRUE;
}

BOOL D3DVertexStreamManager::OnReleaseStreamManager()
{
    if (m_pVertexBuffer)
    {
        DWORD dwBufferID;
        DWORD dwTotalBuffer  = m_pStreamMgr->GetTotalBuffer();
        for (dwBufferID=1; dwBufferID<=dwTotalBuffer; dwBufferID++)
        {
            m_pDevice->ReleaseVertexBuffer(m_pVertexBuffer[dwBufferID]);
        }
        RtCoreMem().Free(m_pVertexBuffer);
        m_pVertexBuffer = NULL;
    }
    return TRUE;
}

BOOL D3DVertexStreamManager::OnResizeStream(DWORD dwResID, DWORD dwSize)
{
    RtCoreLog().Warn("警告: 顶点缓冲区不足，重新设置缓冲区[%d]的大小为[%d]\n", dwResID, dwSize);
    RtgDynamicStreamManager::SDynamicStream* pBuffer = m_pStreamMgr->GetBuffer(dwResID);
    CHECK(pBuffer!=NULL);
    CHECK(pBuffer->dwActiveEntry==0);
    CHECK(pBuffer->dwTotalSize==pBuffer->dwFreeSize);
    m_pStreamMgr->m_dwTotalMemory += dwSize - pBuffer->dwTotalSize;
    pBuffer->dwFreeSize = pBuffer->dwTotalSize = dwSize;
    m_pDevice->ReleaseVertexBuffer(m_pVertexBuffer[dwResID]);
    DWORD dwUsage = RTGU_DYNAMIC | RTGU_WRITEONLY | (m_pDevice->m_bVertexProcessing?RTGU_SOFTWAREPROCESSING:0);
    if (!m_pDevice->CreateVertexBuffer(pBuffer->dwTotalSize, &(m_pVertexBuffer[dwResID]), dwUsage))
    {
        return FALSE;
    }
    return TRUE;
}

BOOL D3DVertexStreamManager::Bind(DWORD dwChannel, DWORD dwResID)
{
    if (dwResID)
    {
        CHECK (m_dwLastUpdateBuffferID==dwResID);
    }
    m_pDevice->SetStreamSource(dwChannel, m_pVertexBuffer[m_dwLastUpdateBuffferID], m_dwLastUpdateStride); 
    m_pDevice->SetVertexBufferStart(m_dwLastUpdateOffset/m_dwLastUpdateStride);
    return TRUE;
}

void* D3DVertexStreamManager::LockStream(DWORD dwBufferID, DWORD dwOffset, DWORD dwSize, DWORD bDiscard)
{
    CHECK(dwSize!=0);
    CHECK(dwBufferID<=m_pStreamMgr->GetTotalBuffer());
    void* pBuf;
    VERIFYEXP(m_pDevice->LockVertexBuffer(m_pVertexBuffer[dwBufferID], &pBuf, dwOffset, dwSize, bDiscard?RTGLK_DISCARD:RTGLK_NOOVERWRITE));
    return pBuf;
}

BOOL D3DVertexStreamManager::UnlockStream(DWORD dwBufferID)
{
    VERIFYEXP(m_pDevice->UnLockVertexBuffer(m_pVertexBuffer[dwBufferID]));
    return TRUE;
}

/*----------------------------------------------------------------------------
-   class D3DIndexStreamManager.
----------------------------------------------------------------------------*/

D3DIndexStreamManager::~D3DIndexStreamManager()
{
    OnReleaseStreamManager();
}

BOOL D3DIndexStreamManager::OnCreateStreamManager()
{
    DWORD dwTotalBuffer  = m_pStreamMgr->GetTotalBuffer();
    //DWORD totalDBuffer = m_pStreamMgr->GetTotalDynamicBuffer();
    //DWORD totalSBuffer = m_pStreamMgr->GetTotalStaticBuffer();

    CHECK(dwTotalBuffer > 0);

    OnReleaseStreamManager();

    m_pIndexBuffer = (void**)RtCoreMem().Alloc(sizeof(void*)*(dwTotalBuffer+1), __FILE__, __LINE__);
    
    DWORD dwUsage = RTGU_DYNAMIC | RTGU_WRITEONLY | (m_pDevice->m_bVertexProcessing?RTGU_SOFTWAREPROCESSING:0);

    DWORD dwBufferID;
    for (dwBufferID=1; dwBufferID<=dwTotalBuffer; dwBufferID++)
    {
        RtgDynamicStreamManager::SDynamicStream* pBuffer = m_pStreamMgr->GetBuffer(dwBufferID);

        CHECK(pBuffer!=NULL);

        if (!m_pDevice->CreateIndexBuffer(pBuffer->dwTotalSize, &(m_pIndexBuffer[dwBufferID]), dwUsage))
        {
            return FALSE;
        }
    }

    return TRUE;
}

BOOL D3DIndexStreamManager::OnReleaseStreamManager()
{
    if (m_pIndexBuffer)
    {
        DWORD dwBufferID;
        DWORD dwTotalBuffer  = m_pStreamMgr->GetTotalBuffer();

        for (dwBufferID=1; dwBufferID<=dwTotalBuffer; dwBufferID++)
        {
            m_pDevice->ReleaseIndexBuffer(m_pIndexBuffer[dwBufferID]);
        }

        RtCoreMem().Free(m_pIndexBuffer);
        m_pIndexBuffer = NULL;
    }
    return TRUE;
}

BOOL D3DIndexStreamManager::OnResizeStream(DWORD dwResID, DWORD dwSize)
{
    RtCoreLog().Warn("警告: 索引缓冲区不足，重新设置缓冲区[%d]的大小为[%d]\n", dwResID, dwSize);
    RtgDynamicStreamManager::SDynamicStream* pBuffer = m_pStreamMgr->GetBuffer(dwResID);
    CHECK(pBuffer!=NULL);
    CHECK(pBuffer->dwActiveEntry==0);
    CHECK(pBuffer->dwTotalSize==pBuffer->dwFreeSize);
    m_pStreamMgr->m_dwTotalMemory += dwSize - pBuffer->dwTotalSize;
    pBuffer->dwFreeSize = pBuffer->dwTotalSize = dwSize;
    m_pDevice->ReleaseIndexBuffer(m_pIndexBuffer[dwResID]);
    DWORD dwUsage = RTGU_DYNAMIC | RTGU_WRITEONLY | (m_pDevice->m_bVertexProcessing?RTGU_SOFTWAREPROCESSING:0);
    if (!m_pDevice->CreateIndexBuffer(pBuffer->dwTotalSize, &(m_pIndexBuffer[dwResID]), dwUsage))
    {
        return FALSE;
    }
    return TRUE;
}

BOOL D3DIndexStreamManager::Bind(DWORD dwVBResID, DWORD dwIBResID)
{
    if (dwIBResID)
    {
        CHECK (m_dwLastUpdateBuffferID==dwIBResID);
    }
    RtgRenderStreamManager* pVertexStreamManager = m_pDevice->m_pVB;
    CHECK(pVertexStreamManager!=NULL);
    VERIFYEXP(m_pDevice->SetIndices(m_pIndexBuffer[m_dwLastUpdateBuffferID]));
    m_pDevice->SetIndexBufferStart(m_dwLastUpdateOffset/sizeof(short));
    return TRUE;
}

void* D3DIndexStreamManager::LockStream(DWORD dwBufferID, DWORD dwOffset, DWORD dwSize, DWORD bDiscard)
{
    CHECK(dwSize!=0);
    CHECK(dwBufferID<=m_pStreamMgr->GetTotalBuffer());
    void* pBuf;
    VERIFYEXP(m_pDevice->LockIndexBuffer(m_pIndexBuffer[dwBufferID], &pBuf, dwOffset, dwSize, bDiscard?RTGLK_DISCARD:RTGLK_NOOVERWRITE));
    return pBuf;
}

BOOL D3DIndexStreamManager::UnlockStream(DWORD dwBufferID)
{
    VERIFYEXP(m_pDevice->UnLockIndexBuffer(m_pIndexBuffer[dwBufferID]));
    return TRUE;
}
} // namespace rt_graph

/*----------------------------------------------------------------------------
-   The End.
----------------------------------------------------------------------------*/
