
//////////////////////////////////////////////////////////////////////////
//
//   FileName : rtg_effect_code.cpp
//     Author : zxy
// CreateTime : 2009.12.20
//       Desc :
//
//////////////////////////////////////////////////////////////////////////

#include "graph\rtg_effect_code.h"

namespace rt_graph {

const char* GetEffectCodeFile(RtgEffectType _Type)
{
    switch (_Type)
    {
    case Rtg_Effect_Terrain :
        return "shader\\terrain.fx";
    case Rtg_Effect_Bloom :
        return "shader\\bloom.fx";
    default :
        break;
    }

    return 0;
}

}