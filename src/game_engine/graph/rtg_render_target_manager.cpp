
//////////////////////////////////////////////////////////////////////////
//
//   FileName : rtg_render_target_manager.cpp
//     Author : zxy
// CreateTime : 2010.03.15
//       Desc :
//
//////////////////////////////////////////////////////////////////////////

#include "graph\rtg_render_target_manager.h"
#include "graph\rt_graph.h"

namespace rt_graph {

    RtgRenderTargetManager::RtgRenderTargetManager(IDirect3DDevice9* pDevice)
        : m_pDevice(pDevice)
    {
        m_lastSetFrame = NULL;
        m_lastSetDepth = NULL;
        m_dfFrame = NULL;
        m_dfDepth = NULL;
    }

    RtgRenderTargetManager::~RtgRenderTargetManager()
    {
        RTASSERT(!m_dfFrame);
        RTASSERT(!m_dfDepth);
        RTASSERT(m_skTarget.empty());
    }

    bool RtgRenderTargetManager::Init()
    {
        SafeRelease(m_dfFrame);
        SafeRelease(m_dfDepth);
        m_pDevice->GetRenderTarget(0, &m_dfFrame);
        m_pDevice->GetDepthStencilSurface(&m_dfDepth);

        return true;
    }

    bool RtgRenderTargetManager::Push_RenderTarget(RtgTextItem* frameTarget, RtgTextItem* depthTarget)
    {
        RTASSERT(m_pDevice);
        RTASSERT(m_skTarget.size() <= Max_RenderTarget_Num);

        if (m_skTarget.size() > Max_RenderTarget_Num)
            return false;

        RtgRenderTarget rt =
        { 
            frameTarget, 
            depthTarget 
        };

        if (!rt.frame && !m_skTarget.empty())
            rt.frame = m_skTarget.top().frame;

        if (!rt.depth && !m_skTarget.empty())
            rt.depth = m_skTarget.top().depth;

        if (!Set_RenderTarget(rt))
            return false;

        m_skTarget.push(rt);

        return true;
    }

    bool RtgRenderTargetManager::Pop_RenderTarget()
    {
        RTASSERT(m_pDevice);
        RTASSERT(!m_skTarget.empty());

        if (m_skTarget.empty())
            return false;

        m_skTarget.pop();

        if (m_skTarget.empty())
        {
            m_pDevice->SetRenderTarget(0, m_dfFrame);
            m_pDevice->SetDepthStencilSurface(m_dfDepth);
            m_lastSetFrame = m_dfFrame;
            m_lastSetDepth = m_dfDepth;
            return true;
        }
        else
        {
            return Set_RenderTarget(m_skTarget.top());
        }

        return false;
    }

    bool RtgRenderTargetManager::Set_RenderTarget(const RtgRenderTarget& rt)
    {
        IDirect3DSurface9* oldFrame = NULL;
        IDirect3DSurface9* frame = NULL;
        IDirect3DSurface9* depth = NULL;

        if (rt.frame)
            frame = RtGetRender()->GetTextMgr()->GetFrameTargt(rt.frame);
        else
            frame = m_dfFrame;

        if (!frame)
            return false;

        m_pDevice->GetRenderTarget(0, &oldFrame);
        oldFrame->Release();
        
        if (frame != m_lastSetFrame)
            m_pDevice->SetRenderTarget(0, frame);
        m_lastSetFrame = frame;

        if (rt.depth)
            depth = RtGetRender()->GetTextMgr()->GetDepthTargt(rt.depth);
        else
            depth = m_dfDepth;

        if (!depth)
        {
            m_pDevice->SetRenderTarget(0, oldFrame);
            return false;
        }

        if (depth != m_lastSetDepth)
            m_pDevice->SetDepthStencilSurface(depth);
        m_lastSetDepth = depth;

        return true;
    }

    void RtgRenderTargetManager::OnLostDevice()
    {
        RTASSERT(m_pDevice);
        SafeRelease(m_dfFrame);
        SafeRelease(m_dfDepth);
        m_lastSetFrame = NULL;
        m_lastSetDepth = NULL;
    }

    void RtgRenderTargetManager::OnResetDevice()
    {
        RTASSERT(m_pDevice);
        RTASSERT(!m_dfFrame);
        RTASSERT(!m_dfDepth);
        m_pDevice->GetRenderTarget(0, &m_dfFrame);
        m_pDevice->GetDepthStencilSurface(&m_dfDepth);
    }

}