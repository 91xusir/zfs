
#include "rtg_graph_inter.h"

namespace rt_graph {
/*----------------------------------------------------------------------------
-   class RtgDynamicStreamManager.
----------------------------------------------------------------------------*/

RtgDynamicStreamManager::RtgDynamicStreamManager()
{
    m_pBuffers = NULL;      // 缓冲区
    m_dwTotalStream = 0;    // 总共的缓冲区数
    m_dwTotalMemory = 0;
    m_dwUsedMemory = 0;
    m_dwActiveEntry = 0;
    m_dwLastestStream = 0;
}

RtgDynamicStreamManager::~RtgDynamicStreamManager()
{
    DEL_ARRAY(m_pBuffers);
}

void RtgDynamicStreamManager::InitStreamManager(DWORD dwTotalStream, DWORD* pdwSize)
{
    CHECK(dwTotalStream>0);
    CHECK(pdwSize!=NULL);

    m_dwTotalStream = dwTotalStream;

    m_pBuffers = RT_NEW SDynamicStream[m_dwTotalStream];

    for (DWORD i=0; i<m_dwTotalStream; i++)
    {
        m_pBuffers[i].dwTotalSize   = pdwSize[i];
        m_pBuffers[i].dwFreeSize    = pdwSize[i];
        m_pBuffers[i].dwActiveEntry = 0;

        m_dwTotalMemory += pdwSize[i];
    }
}

RtgDynamicStreamManager::SDynamicStream* RtgDynamicStreamManager::GetBuffer(DWORD dwBufferID)
{
    CHECK(dwBufferID<=m_dwTotalStream && dwBufferID>0);
    return m_pBuffers + dwBufferID - 1;
}

BOOL RtgDynamicStreamManager::QuerySpace(DWORD dwInSize, DWORD& dwOutBufferID, DWORD& dwOutOffset)
{
    DWORD i, iIdx = m_dwLastestStream;
    for (i=0; i<m_dwTotalStream; i++)
    {
        if (m_pBuffers[iIdx].dwFreeSize>=dwInSize)
        {
            dwOutBufferID = m_dwLastestStream + 1;
            dwOutOffset = m_pBuffers[iIdx].dwTotalSize - m_pBuffers[iIdx].dwFreeSize;
            return TRUE;
        }
        iIdx ++;
        if (iIdx==m_dwTotalStream) iIdx = 0;
    }
    return FALSE;
}

DWORD RtgDynamicStreamManager::ClearOneBuffer()
{
    if (m_dwLastestStream==0) m_dwLastestStream = m_dwTotalStream-1;
    m_dwUsedMemory -= (m_pBuffers[m_dwLastestStream].dwTotalSize-m_pBuffers[m_dwLastestStream].dwFreeSize);
    m_dwActiveEntry -= m_pBuffers[m_dwLastestStream].dwActiveEntry;
    m_pBuffers[m_dwLastestStream].dwFreeSize = m_pBuffers[m_dwLastestStream].dwTotalSize;
    m_pBuffers[m_dwLastestStream].dwActiveEntry = 0;
    return m_dwLastestStream+1;
}

DWORD RtgDynamicStreamManager::ClearSizeBuffer(DWORD dwMinSize)
{
    DWORD dwBufID, i=0;
    for (dwBufID=m_dwLastestStream, i=0; i<m_dwTotalStream; i++, dwBufID--)
    {
        if (dwBufID==0) dwBufID = m_dwTotalStream-1;
        if (m_pBuffers[dwBufID].dwTotalSize >= dwMinSize)
        {
            break;
        }
    }
    if (i>=m_dwTotalStream)
    {
        return 0;
    }
    if (m_dwLastestStream==0) m_dwLastestStream = m_dwTotalStream-1;
    m_dwUsedMemory -= (m_pBuffers[m_dwLastestStream].dwTotalSize-m_pBuffers[m_dwLastestStream].dwFreeSize);
    m_dwActiveEntry -= m_pBuffers[m_dwLastestStream].dwActiveEntry;
    m_pBuffers[m_dwLastestStream].dwFreeSize = m_pBuffers[m_dwLastestStream].dwTotalSize;
    m_pBuffers[m_dwLastestStream].dwActiveEntry = 0;
    return m_dwLastestStream+1;
}

void RtgDynamicStreamManager::ClearAllBuffer()
{
    for (DWORD i=0; i<m_dwTotalStream; i++)
    {
        m_pBuffers[i].dwFreeSize = m_pBuffers[i].dwTotalSize;
        m_pBuffers[i].dwActiveEntry = 0;
    }
    m_dwLastestStream = 0;
    m_dwUsedMemory = 0;
    m_dwActiveEntry = 0;
}

BOOL RtgDynamicStreamManager::AddEntry(DWORD dwBufferID, DWORD dwSize, DWORD& dwInOutOffset, DWORD dwStride)
{
    DWORD dwPaddingSize = dwSize, dwPaddingOffset = dwInOutOffset;

    // padding
    if (dwStride>=4)
    {
        dwPaddingOffset = (dwInOutOffset / dwStride + 1) * dwStride;
        dwPaddingSize += (dwPaddingOffset - dwInOutOffset);
    }

    CHECK(m_dwLastestStream<m_dwTotalStream && m_dwLastestStream>=0);
    CHECK(dwBufferID<=m_dwTotalStream && dwBufferID>0);
    CHECK(dwPaddingSize<=m_pBuffers[dwBufferID-1].dwFreeSize);
    CHECK(dwPaddingOffset+dwSize<=m_pBuffers[dwBufferID-1].dwTotalSize);

    SDynamicStream* pBuffer = m_pBuffers+(dwBufferID-1);

    pBuffer->dwActiveEntry ++;
    pBuffer->dwFreeSize -= dwPaddingSize;
    dwInOutOffset = dwPaddingOffset;

    m_dwUsedMemory += dwPaddingSize;
    m_dwActiveEntry ++;

    return TRUE;
}

//===================================================
//  RtgRenderStreamManager
//===================================================

RtgRenderStreamManager::RtgRenderStreamManager(RtgDevice* pDevice)
{
    m_dwLastUpdateBuffferID = 0;
    m_dwLastUpdateStride = 0;
    m_dwLastUpdateOffset = 0;

    m_dwStatDynamicSend = 0;
    m_dwStatDynamicClear = 0;
    m_dwStatDynamicSwap = 0;

    m_pDevice = pDevice;
    //m_pStreamMgr = new RtgDynamicStreamManager();
    m_pStreamMgr = &m_streamMgr;
}

RtgRenderStreamManager::~RtgRenderStreamManager()
{
    //delete m_pStreamMgr;
}

BOOL RtgRenderStreamManager::InitStreamManager(DWORD nTotalDynamicStream, DWORD* nDynamicSize, DWORD nTotalStaticStream, DWORD* nStaticSize, DWORD nTotalDynamicEntry, DWORD nTotalStaticEntry)
{
    m_pStreamMgr->InitStreamManager(nTotalDynamicStream, nDynamicSize);
    return TRUE;
}

BOOL RtgRenderStreamManager::Upload(DWORD& dwResID, BYTE* pResource, DWORD dwSize, BOOL bResident, DWORD dwStride)
{
    void* pBuf = BeginUpload(dwResID, dwSize, bResident, dwStride);
    if (pBuf==NULL) return FALSE;
    memcpy(pBuf, pResource, dwSize);
    return EndUpload(dwResID);
}

void* RtgRenderStreamManager::BeginUpload(DWORD& dwResID, DWORD dwSize, BOOL bResident, DWORD dwStride)
{
    DWORD dwOffset;

    m_dwStatDynamicSend ++;

    if (m_pStreamMgr->QuerySpace(dwSize+dwStride, m_dwLastUpdateBuffferID, dwOffset))
    {
        m_pStreamMgr->AddEntry(m_dwLastUpdateBuffferID, dwSize, dwOffset, dwStride);
        dwResID = m_dwLastUpdateBuffferID;
        m_dwLastUpdateStride = dwStride;
        m_dwLastUpdateOffset = dwOffset;
        return LockStream(m_dwLastUpdateBuffferID, dwOffset, dwSize, FALSE);
    }else
    {
        dwOffset = 0;
        m_dwLastUpdateBuffferID = m_pStreamMgr->ClearSizeBuffer((dwSize+dwStride+4)&0xFFFFFFFC);
        if (m_dwLastUpdateBuffferID==0)
        {
            m_dwLastUpdateBuffferID = m_pStreamMgr->ClearOneBuffer();
            OnResizeStream(m_dwLastUpdateBuffferID, (dwSize+dwStride+4)&0xFFFFFFFC);
        }
        if (!m_pStreamMgr->AddEntry(m_dwLastUpdateBuffferID, dwSize, dwOffset, dwStride))
        {
            CHECKEX("缓冲区不足");
            return NULL;
        }
        m_dwStatDynamicClear ++;
        dwResID = m_dwLastUpdateBuffferID;
        m_dwLastUpdateStride = dwStride;
        m_dwLastUpdateOffset = dwOffset;
        return LockStream(m_dwLastUpdateBuffferID, dwOffset, dwSize, TRUE);
    }
}

BOOL RtgRenderStreamManager::EndUpload(DWORD& dwResID)
{
    CHECK(m_dwLastUpdateBuffferID==dwResID);
    return UnlockStream(dwResID);
}

void RtgRenderStreamManager::Clear(DWORD dwResID)
{
}

void RtgRenderStreamManager::ClearAll()
{
    m_pStreamMgr->ClearAllBuffer();
}

void RtgRenderStreamManager::OnBeginRender()
{
}

void RtgRenderStreamManager::OnEndRender()
{
}

DWORD RtgRenderStreamManager::GetTotalActiveEntry()    
{ 
    return m_pStreamMgr->GetActiveEntry();
}
} // namespace rt_graph

/*----------------------------------------------------------------------------
-   The End.
----------------------------------------------------------------------------*/
