
//////////////////////////////////////////////////////////////////////////
//
//   FileName : rtg_buffer_allocator.cpp
//     Author : zxy
// CreateTime : 2009.12.20
//       Desc :
//
//////////////////////////////////////////////////////////////////////////

#include "graph/rtg_buffer_allocator.h"
#include "graph/rt_graph.h"

namespace rt_graph {

    namespace {
        
        DWORD _bufferId = 0;
    }

    RtgBufferAllocator::RtgBufferAllocator(IDirect3DDevice9* pDevice)
        : m_pDevice(pDevice)
    {
    }

    RtgBufferAllocator::~RtgBufferAllocator()
    {
        Clear();
#ifdef _DEBUG
        if (!m_setBuffer.empty())
        {
            string str = "buffer not release : \n";
            for (set<RtgBufferItem*>::iterator i = m_setBuffer.begin(); 
                 i != m_setBuffer.end(); ++i)
            {
                RtCoreLog().Error("buffer unrelease : %s(%s) id:%u\n", 
                    (*i)->szFunc.c_str(), 
                    (*i)->szLine.c_str(),
                    (*i)->dwId);
            }
        }
#endif
    }

    bool RtgBufferAllocator::Init()
    {
        RTASSERT(m_pDevice);
        RTASSERT(m_vShare_vb.empty());
        RTASSERT(m_vShare_ib.empty());

        RtgSetCreateBufferDebugInfo();
        RtgBufferItem* vb = CreateBuffer(
            Init_vb_Size,
            0,
            D3DUSAGE_DYNAMIC | D3DUSAGE_WRITEONLY,
            D3DPOOL_DEFAULT,
            NULL);

        RtgSetCreateBufferDebugInfo();
        RtgBufferItem* ib = CreateBuffer(
            Init_ib_Size,
            D3DFMT_INDEX16,
            D3DUSAGE_DYNAMIC | D3DUSAGE_WRITEONLY,
            D3DPOOL_DEFAULT,
            NULL);

        m_vShare_vb.push_back(RtgShareBuffer(vb));
        m_vShare_ib.push_back(RtgShareBuffer(ib));

        return true;
    }

    void RtgBufferAllocator::Clear()
    {
        for (size_t i = 0; i < m_vShare_vb.size(); ++i)
            ReleaseBuffer(m_vShare_vb[i].bufItem);
        for (size_t i = 0; i < m_vShare_ib.size(); ++i)
            ReleaseBuffer(m_vShare_ib[i].bufItem);
    }

    void RtgBufferAllocator::PrepareFotRender()
    {
        for (size_t i = 0; i < m_vShare_vb.size(); ++i)
            m_vShare_vb[i].Reset();
        for (size_t i = 0; i < m_vShare_ib.size(); ++i)
            m_vShare_ib[i].Reset();
    }

    void RtgBufferAllocator::FrameMove()
    {
    }

    RtgBufferItem* RtgBufferAllocator::CreateBuffer(UINT _Size, 
                                                    DWORD _Fmt /* = 0 */, 
                                                    DWORD _Usage /* = 0 */, 
                                                    D3DPOOL _Poolt /* = D3DPOOL_MANAGED */, 
                                                    void* _Data /* = NULL */
                                                    )
    {
        RtgBufferItem* bufItem = RT_NEW RtgBufferItem;
        
        bufItem->dwSize   = _Size;
        bufItem->Usage    = _Usage;
        bufItem->Format   = _Fmt;
        bufItem->Locked   = false;
        bufItem->Pooltype = _Poolt;
        bufItem->pBuffer = NULL;

#ifdef _DEBUG
        bufItem->szFunc = _call_create_buffer_func_name;
        bufItem->szLine = _call_create_buffer_func_line;
        bufItem->dwId = _bufferId++;
        _call_create_buffer_func_name.clear();
        _call_create_buffer_func_line.clear();
        m_setBuffer.insert(bufItem);
#endif

        if (GetCurrentThreadId() != RtGetRender()->GetDeviceThreadId())
        {
            RTASSERT(false);
            RtCoreLog().Error("create device buffer mush in device thread : \n( %s(%s)\n",
                _call_create_buffer_func_name.c_str(),
                _call_create_buffer_func_line.c_str()
                );
            return NULL;
        }

        if (CreateBufferImpl(bufItem))
        {
            if (_Data)
                UpdateBuffer(bufItem, _Data, _Size);

        }
        else
        {
            DEL_ONE(bufItem);
            RtCoreLog().Error("create device buffer fail : \n( %s(%s)\nsize : %u\nformat : %u\npool : %u\n",
                _call_create_buffer_func_name.c_str(),
                _call_create_buffer_func_line.c_str(),
                _Size,
                _Fmt,
                _Poolt);
        }

        return bufItem;
    }

    bool RtgBufferAllocator::CreateBufferImpl(RtgBufferItem* bufItem)
    {
        RTASSERT(bufItem);
        RTASSERT(!bufItem->pBuffer);

        switch (bufItem->Format)
        {
        case D3DFMT_INDEX16 :
        case D3DFMT_INDEX32 :
            {
                if (FAILED(m_pDevice->CreateIndexBuffer(
                    bufItem->dwSize, 
                    bufItem->Usage, 
                    (D3DFORMAT)bufItem->Format, 
                    bufItem->Pooltype, 
                    (IDirect3DIndexBuffer9**)&bufItem->pBuffer, 
                    NULL))
                    )
                    return false;
            }
            break;

        default :
            {
                if (FAILED(m_pDevice->CreateVertexBuffer(
                    bufItem->dwSize, 
                    bufItem->Usage, 
                    bufItem->Format, 
                    bufItem->Pooltype, 
                    (IDirect3DVertexBuffer9**)&bufItem->pBuffer, 
                    NULL))
                    )
                    return false;
            }
            break;
        }

        RtGetPref()->Used_BuffSize += bufItem->dwSize;

        if (bufItem->Pooltype == D3DPOOL_DEFAULT)
            m_setHWbuffer.insert(bufItem);

        return true;
    }

    void RtgBufferAllocator::ReleaseBuffer(RtgBufferItem* bufItem)
    {
        if (!bufItem)
            return;
        ReleaseBufferImpl(bufItem);
        if (bufItem->Pooltype == D3DPOOL_DEFAULT)
            m_setHWbuffer.erase(bufItem);
#ifdef _DEBUG
        m_setBuffer.erase(bufItem);
#endif
        DEL_ONE(bufItem);
    }

    void RtgBufferAllocator::ReleaseBufferImpl(RtgBufferItem* bufItem)
    {
        if (bufItem->pBuffer)
        {
            switch (bufItem->Format)
            {
            case D3DFMT_INDEX16 :
            case D3DFMT_INDEX32 :
                ((IDirect3DIndexBuffer9*)bufItem->pBuffer)->Release();
                break;

            default :
                ((IDirect3DVertexBuffer9*)bufItem->pBuffer)->Release();
                break;
            }
        }

        bufItem->pBuffer = NULL;
        RtGetPref()->Used_BuffSize -= bufItem->dwSize;
    }

    bool RtgBufferAllocator::Resize(RtgBufferItem* bufItem, UINT _Size)
    {
        RTASSERT(bufItem);
        RTASSERT(_Size);

        ReleaseBufferImpl(bufItem);
        bufItem->dwSize = _Size;
        return CreateBufferImpl(bufItem);

    }

    void* RtgBufferAllocator::Lock(RtgBufferItem* bufItem, UINT _Offset, UINT _Size)
    {
        RTASSERT(bufItem);
        RTASSERT(!bufItem->Locked);
        if (bufItem->Locked || !bufItem->pBuffer)
            return NULL;

        DWORD _Lockf = 0;
        void* data = NULL;

        if ((bufItem->Usage & D3DUSAGE_DYNAMIC) && bufItem->Pooltype == D3DPOOL_DEFAULT)
        {
            if (_Offset)
                _Lockf = D3DLOCK_NOOVERWRITE;
            else
                _Lockf = D3DLOCK_DISCARD;
        }

        switch (bufItem->Format)
        {
        case D3DFMT_INDEX16 :
        case D3DFMT_INDEX32 :
            {
                IDirect3DIndexBuffer9* ib = (IDirect3DIndexBuffer9*)bufItem->pBuffer;
                if (FAILED(ib->Lock(_Offset, _Size, &data, _Lockf)))
                    return NULL;
            }
            break;

        default :
            {
                IDirect3DVertexBuffer9* vb = (IDirect3DVertexBuffer9*)bufItem->pBuffer;
                if (FAILED(vb->Lock(_Offset, _Size, &data, _Lockf)))
                    return NULL;
            }
            break;
        }

        bufItem->Locked = true;
        return data;
    }

    void RtgBufferAllocator::Unlock(RtgBufferItem* bufItem)
    {
        RTASSERT(bufItem);
        if (!bufItem->Locked)
            return;

        switch (bufItem->Format)
        {
        case D3DFMT_INDEX16 :
        case D3DFMT_INDEX32 :
            {
                IDirect3DIndexBuffer9* ib = (IDirect3DIndexBuffer9*)bufItem->pBuffer;
                ib->Unlock();
            }
            break;

        default :
            {
                IDirect3DVertexBuffer9* vb = (IDirect3DVertexBuffer9*)bufItem->pBuffer;
                vb->Unlock();
            }
            break;
        }

        bufItem->Locked = false;
    }

    bool RtgBufferAllocator::Request_vb(RtgShareBufferItem* shareItem, UINT _Size)
    {
        for (size_t i = 0; i < m_vShare_vb.size(); ++i)
        {
            RtgShareBuffer& _sbuf = m_vShare_vb[i];

            if (_sbuf.freeSize >= _Size)
            {
                DWORD _Offset = _sbuf.bufItem->dwSize - _sbuf.freeSize;
                _sbuf.freeSize -= _Size;
                shareItem->pShareBuf = &_sbuf;
                shareItem->Offset = _Offset;
                shareItem->Size = _Size;
                return true;
            }
        }

        RtgSetCreateBufferDebugInfo();
        RtgBufferItem* vb = CreateBuffer(
            (_Size > Init_vb_Size ? _Size : Init_vb_Size),
            0,
            D3DUSAGE_DYNAMIC | D3DUSAGE_WRITEONLY,
            D3DPOOL_DEFAULT,
            NULL);

        m_vShare_vb.push_back(RtgShareBuffer(vb));
        m_setHWbuffer.insert(vb);
        shareItem->pShareBuf = &m_vShare_vb.back();
        shareItem->Offset = 0;
        shareItem->Size = _Size;
        return true;
    }

    bool RtgBufferAllocator::Request_ib(RtgShareBufferItem* shareItem, UINT _Size)
    {
        for (size_t i = 0; i < m_vShare_ib.size(); ++i)
        {
            RtgShareBuffer& _sbuf = m_vShare_ib[i];

            if (_sbuf.freeSize >= _Size)
            {
                DWORD _Offset = _sbuf.bufItem->dwSize - _sbuf.freeSize;
                _sbuf.freeSize -= _Size;
                shareItem->pShareBuf = &_sbuf;
                shareItem->Offset = _Offset;
                shareItem->Size = _Size;
                return true;
            }
        }

        RtgSetCreateBufferDebugInfo();
        RtgBufferItem* ib = CreateBuffer(
            (_Size > Init_vb_Size ? _Size : Init_vb_Size),
            D3DFMT_INDEX16,
            D3DUSAGE_DYNAMIC | D3DUSAGE_WRITEONLY,
            D3DPOOL_DEFAULT,
            NULL);

        m_vShare_ib.push_back(RtgShareBuffer(ib));
        m_setHWbuffer.insert(ib);
        shareItem->pShareBuf = &m_vShare_ib.back();
        shareItem->Offset = 0;
        shareItem->Size = _Size;
        return true;
    }

    void* RtgBufferAllocator::Lock(RtgShareBufferItem* shareItem)
    {
        RTASSERT(shareItem);
        return Lock(shareItem->pShareBuf->bufItem, shareItem->Offset, shareItem->Size);
    }

    void RtgBufferAllocator::Unlock(RtgShareBufferItem* shareItem)
    {
        RTASSERT(shareItem);
        return Unlock(shareItem->pShareBuf->bufItem);
    }

    void* RtgBufferAllocator::Lock_Shared_vb(UINT _Size, RtgShareBufferItem* shareItem)
    {
        RTASSERT(shareItem);
        Request_vb(shareItem, _Size);
        return Lock(shareItem);
    }

    void* RtgBufferAllocator::Lock_Shared_ib(UINT _Size, RtgShareBufferItem* shareItem)
    {
        RTASSERT(shareItem);
        Request_ib(shareItem, _Size);
        return Lock(shareItem);
    }

    bool RtgBufferAllocator::UpdateBuffer(RtgBufferItem* bufItem, 
                                          void* pData, 
                                          UINT _Size, 
                                          UINT _Offset /* = 0 */, 
                                          bool bAutoResize /* = false */
                                          )
    {
        RTASSERT(bufItem);
        RTASSERT(bufItem->pBuffer);
        RTASSERT(pData);

        if (bAutoResize && _Offset + _Size > bufItem->dwSize)
            if (!Resize(bufItem, _Offset + _Size))
                return false;

        DWORD _Lockf = 0;

        if ((bufItem->Usage & D3DUSAGE_DYNAMIC) && bufItem->Pooltype == D3DPOOL_DEFAULT)
        {
            if (_Offset)
                _Lockf = D3DLOCK_NOOVERWRITE;
            else
                _Lockf = D3DLOCK_DISCARD;
        }

        switch (bufItem->Format)
        {
        case D3DFMT_INDEX16 :
        case D3DFMT_INDEX32 :
            {
                IDirect3DIndexBuffer9* ib = (IDirect3DIndexBuffer9*)bufItem->pBuffer;
                void* dst = NULL;
                if (FAILED(ib->Lock(_Offset, _Size, &dst, _Lockf)))
                    return false;
                memcpy(dst, pData, _Size);
                ib->Unlock();
            }
            break;
        default :
            {
                IDirect3DVertexBuffer9* vb = (IDirect3DVertexBuffer9*)bufItem->pBuffer;
                void* dst = NULL;
                if (FAILED(vb->Lock(_Offset, _Size, &dst, _Lockf)))
                    return false;
                memcpy(dst, pData, _Size);
                vb->Unlock();
            }
            break;
        }

        return true;
    }

    void RtgBufferAllocator::OnLostDevice()
    {
        for (set<RtgBufferItem*>::iterator i = m_setHWbuffer.begin();
            i != m_setHWbuffer.end(); ++i)
        {
            ReleaseBufferImpl(*i);
        }
    }

    void RtgBufferAllocator::OnResetDevice()
    {
        for (set<RtgBufferItem*>::iterator i = m_setHWbuffer.begin();
            i != m_setHWbuffer.end(); ++i)
        {
            CreateBufferImpl(*i);
        }
    }

}