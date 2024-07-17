
//////////////////////////////////////////////////////////////////////////
//
//   FileName : rtg_buffer_allocator.h
//     Author : zxy
// CreateTime : 2009.12.20
//       Desc :
//
//////////////////////////////////////////////////////////////////////////

#ifndef _RtgBufferAllocator_H_
#define _RtgBufferAllocator_H_

#include "core/rt2_core.h"
#include "graph/rtg_thread_lock.h"
#include "d3d9.h"
#include "d3dx9.h"

namespace rt_graph {

    typedef struct _RtgBufferItem
    {
        DWORD   dwSize;
        DWORD   Format;
        DWORD   Usage;
        D3DPOOL Pooltype;
        bool    Locked;
        void*   pBuffer;   
#ifdef _DEBUG
        string  szFunc;
        string  szLine;
        DWORD   dwId;
#endif
    } RtgBufferItem;

    struct RtgShareBuffer
    {
        RtgBufferItem* bufItem;
        DWORD freeSize;

        RtgShareBuffer(RtgBufferItem* _bufItem)
            : bufItem(_bufItem),
              freeSize(_bufItem->dwSize)
        {
        }

        void Reset()
        {
            freeSize = bufItem->dwSize;
        }
    };

    struct RtgShareBufferItem
    {
        RtgShareBuffer* pShareBuf;
        DWORD Offset;
        DWORD Size;

        RtgShareBufferItem()
            : pShareBuf(NULL),
              Offset(0),
              Size(0)
        {
        }

        RtgShareBufferItem(RtgShareBuffer* _shareBuf, DWORD _Offset, DWORD _Size)
            : pShareBuf(_shareBuf),
              Offset(_Offset),
              Size(_Size)
        {
        }

        RtgShareBufferItem(const RtgShareBufferItem& _shareItem)
        {
            pShareBuf = _shareItem.pShareBuf;
            Offset = _shareItem.Offset;
            Size = _shareItem.Size;
        }

        bool Vaild() const
        {
            return (pShareBuf != NULL) && (Size > 0);
        }
    };

    class RtgBufferAllocator
    {
        enum 
        {
            Init_vb_Size = 120000,
            Init_ib_Size = 20000,   
        };

    public :

        bool Init();
        void Clear();
        void FrameMove();
        void PrepareFotRender();

        RtgBufferItem* CreateBuffer(UINT _Size, 
                                    DWORD _Fmt = 0, 
                                    DWORD _Usage = 0, 
                                    D3DPOOL _Poolt = D3DPOOL_MANAGED,
                                    void* _Data = NULL
                                    );
        void ReleaseBuffer(RtgBufferItem* bufItem);
        bool UpdateBuffer(RtgBufferItem* bufItem, 
                          void* pData, 
                          UINT _Size, 
                          UINT _Offset = 0,
                          bool bAutoResize = false);
        bool Resize(RtgBufferItem* bufItem, UINT _Size);
        
        void* Lock(RtgBufferItem* bufItem, UINT _Offset = 0, UINT _Size = 0);
        void  Unlock(RtgBufferItem* bufItem);

        bool Request_vb(RtgShareBufferItem* shareItem, UINT _Size);
        bool Request_ib(RtgShareBufferItem* shareItem, UINT _Size);

        void* Lock(RtgShareBufferItem* shareItem);
        void  Unlock(RtgShareBufferItem* shareItem);

        void* Lock_Shared_vb(UINT _Size, RtgShareBufferItem* shareItem);
        void* Lock_Shared_ib(UINT _Size, RtgShareBufferItem* shareItem);

        void OnLostDevice();
        void OnResetDevice();

        RtgBufferAllocator(IDirect3DDevice9* pDevice);
        virtual ~RtgBufferAllocator();

    protected :
        bool CreateBufferImpl(RtgBufferItem* bufItem);
        void ReleaseBufferImpl(RtgBufferItem* bufItem);

    protected :
        IDirect3DDevice9*       m_pDevice;
        vector<RtgShareBuffer>  m_vShare_vb;
        vector<RtgShareBuffer>  m_vShare_ib;
        set<RtgBufferItem*>     m_setHWbuffer;
#ifdef _DEBUG
        set<RtgBufferItem*>     m_setBuffer;
#endif

    };
}

#ifdef _DEBUG
    #define RtgSetCreateBufferDebugInfo()   \
        _call_create_buffer_func_name = __FUNCTION__;   \
        _call_create_buffer_func_line = __LINE__;
#else
    #define RtgSetCreateBufferDebugInfo() 
#endif
#define RtgReleaseBuffer(p) if (p) RtGetRender()->GetBuffMgr()->ReleaseBuffer(p); (p) = NULL

#endif