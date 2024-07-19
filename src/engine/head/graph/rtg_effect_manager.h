
//////////////////////////////////////////////////////////////////////////
//
//   FileName : rtg_effect_manager.h
//     Author : zxy
// CreateTime : 2009.12.20
//       Desc :
//
//////////////////////////////////////////////////////////////////////////

#ifndef _RtgEffectManager_H_
#define _RtgEffectManager_H_

#include "rtg_effect_code.h"
#include "d3d9.h"
#include "d3dx9.h"

namespace rt_graph {

    class RtgEffectManager
    {
    public :
        bool Init();
        void Clear();
        ID3DXEffect* LoadEffect(const char fileName[], RtgEffectType _Type);

        void OnLostDevice();
        void OnResetDevice();

    public :
        RtgEffectManager(IDirect3DDevice9* pd3d9Device);
        virtual ~RtgEffectManager();

    public :
        ID3DXEffect* GetEffect(RtgEffectType _Type)
        {
            return m_ppEffect[_Type];
        }

    private :
        IDirect3DDevice9* m_pDevice;
        ID3DXEffect* m_ppEffect[Rtg_Effect_Count];
    };

}

#endif