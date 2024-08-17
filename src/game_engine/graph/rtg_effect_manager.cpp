
//////////////////////////////////////////////////////////////////////////
//
//   FileName : rtg_effect_manager.h
//     Author : zxy
// CreateTime : 2009.12.20
//       Desc :
//
//////////////////////////////////////////////////////////////////////////

#include "graph/rtg_effect_manager.h"
#include "core/rt2_core.h"

namespace rt_graph {

RtgEffectManager::RtgEffectManager(IDirect3DDevice9* pd3d9Device) : m_pDevice(pd3d9Device) {
    memset(m_ppEffect, 0, sizeof(m_ppEffect));
}

RtgEffectManager::~RtgEffectManager() {
    Clear();
}

bool RtgEffectManager::Init() {
    for (int i = 0; i < Rtg_Effect_Count; ++i)
        // m_ppEffect[i] = LoadEffect(GetEffectCodeChars((RtgEffectType)i), (RtgEffectType)i,false);
        m_ppEffect[i] =
            LoadEffectFromCode(GetEffectCodeChars((RtgEffectType)i), (RtgEffectType)i);

    return true;
}

void RtgEffectManager::Clear() {
    for (int i = 0; i < Rtg_Effect_Count; ++i)
        if (m_ppEffect[i])
            m_ppEffect[i]->Release();
}

ID3DXEffect* RtgEffectManager::LoadEffect(const char fileName[], RtgEffectType _Type) {
    RTASSERT(_Type >= 0 && _Type < Rtg_Effect_Count);

    if (!fileName)
        return NULL;

    ID3DXEffect* p = m_ppEffect[_Type];

    if (p)
        p->Release();

    RtArchive* pFile = RtCoreFile().CreateFileReader(fileName, 0);

    if (!pFile)
        return NULL;

    int   _Size   = pFile->TotalSize();
    char* pBuffer = RT_NEW char[_Size];

    pFile->Serialize(pBuffer, _Size);
    pFile->Close();
    DEL_ONE(pFile);

    if (SUCCEEDED(D3DXCreateEffect(m_pDevice, pBuffer, _Size, 0, 0, 0, 0, &p, 0))) {
        DEL_ARRAY(pBuffer);
        m_ppEffect[_Type] = p;
        return p;
    }

    DEL_ARRAY(pBuffer);
    RtCoreLog().Error("load effect : %s fail~\n", fileName);

    return NULL;
}

ID3DXEffect* RtgEffectManager::LoadEffectFromCode(const char* shaderCode, RtgEffectType _Type) {
    RTASSERT(_Type >= 0 && _Type < Rtg_Effect_Count);
    if (!shaderCode)
        return NULL;
    ID3DXEffect* p = m_ppEffect[_Type];
    if (p)
        p->Release();
    // 硬编码的着色器代码
    const char* pBuffer = shaderCode;
    int         _Size   = strlen(pBuffer);
    if (SUCCEEDED(D3DXCreateEffect(m_pDevice, pBuffer, _Size, 0, 0, 0, 0, &p, 0))) {
        m_ppEffect[_Type] = p;
        return p;
    }
    RtCoreLog().Error("load effect fail~\n");
    return nullptr;
}

void RtgEffectManager::OnLostDevice() {
    for (int i = 0; i < Rtg_Effect_Count; ++i)
        if (m_ppEffect[i])
            m_ppEffect[i]->OnLostDevice();
}

void RtgEffectManager::OnResetDevice() {
    for (int i = 0; i < Rtg_Effect_Count; ++i)
        if (m_ppEffect[i])
            m_ppEffect[i]->OnResetDevice();
}

}  // namespace rt_graph
