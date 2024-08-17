
//////////////////////////////////////////////////////////////////////////
//
//   FileName : rtg_effect_code.h
//     Author : zxy
// CreateTime : 2009.12.20
//       Desc :
//
//////////////////////////////////////////////////////////////////////////

#ifndef _RtgEffectCode_H_
#define _RtgEffectCode_H_


namespace rt_graph {

    typedef enum _RtgEffectType 
    {
        Rtg_Effect_Terrain = 0,
        Rtg_Effect_Bloom,
        Rtg_Effect_Count
    } RtgEffectType;

    struct RtgEffectCode
    {
        RtgEffectCode()
            : m_Code(0), m_CodeSize(0)
        {
        }

        RtgEffectCode(const char* _Code, size_t _CodeSize)
            : m_Code(_Code), m_CodeSize(_CodeSize)
        {
        }

        bool Vaild() const
        {
            return m_Code != 0;
        }

        const char*  m_Code;
        const size_t m_CodeSize;
    };

    const char* GetEffectCodeFile(RtgEffectType _Type);
    const char* GetEffectCodeChars(RtgEffectType _Type);

}

#endif
