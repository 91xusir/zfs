
//////////////////////////////////////////////////////////////////////////
//
//   FileName : rtg_render_target_manager.h
//     Author : zxy
// CreateTime : 2010.03.15
//       Desc :
//
//////////////////////////////////////////////////////////////////////////

#ifndef _RtgRenderTargetManager_H_
#define _RtgRenderTargetManager_H_

#include "core/rt2_core.h"
#include "d3d9.h"
#include "d3dx9.h"

namespace rt_graph {

    struct _RtgTextItem;
    typedef _RtgTextItem RtgTextItem;

    class RtgRenderTargetManager
    {
        enum
        {
            Max_RenderTarget_Num = 4
        };

        typedef struct _RtgRenderTarget
        {
            RtgTextItem* frame;
            RtgTextItem* depth;
        } RtgRenderTarget;

    public :

        bool Init();

        bool Push_RenderTarget(RtgTextItem* frameTarget, RtgTextItem* depthTarget);
        bool Pop_RenderTarget();

        void OnLostDevice();
        void OnResetDevice();

        RtgRenderTargetManager(IDirect3DDevice9* pd3dDevice);
        virtual ~RtgRenderTargetManager();

    protected :
        bool Set_RenderTarget(const RtgRenderTarget& rt);

    protected :
        IDirect3DDevice9*      m_pDevice;
        stack<RtgRenderTarget> m_skTarget;
        IDirect3DSurface9*     m_lastSetFrame;
        IDirect3DSurface9*     m_lastSetDepth;
        IDirect3DSurface9*     m_dfFrame;
        IDirect3DSurface9*     m_dfDepth;

    };

}

#endif
