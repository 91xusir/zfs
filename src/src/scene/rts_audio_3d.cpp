#include "scene/rt_scene.h"

RT_IMPLEMENT_DYNCREATE(RtsAudio3D, RtsObject, 0, "scene")

RtsAudio3D::RtsAudio3D()
{
    m_bEnable       = false;        // ÊÇ·ñÊ¹ÓÃ
    m_bLoop         = true;
    m_b3D           = true;
    m_iVolume       = 0;
    m_iFadeOutFrame = 1;
    m_fMinDistance  = 200.f;
    m_fMaxDistance  = 600.f;
    m_pSound = NULL;
//#if RTS_DRAW_HELPER
//    SetShowType(RtGetRender(),SO_SPRITE);
//#endif
}

RtsAudio3D::~RtsAudio3D()
{
    if (m_pSound)
    {
        g_pSoundMgr->DestroySound(m_pSound);
    }
}

long RtsAudio3D::Serialize(RtArchive& ar)
{
    if (!RtsObject::Serialize(ar)) return FALSE;
    ar << m_bEnable;
    ar << m_szFileName;
    ar << m_bLoop;
    ar << m_b3D;
    ar << m_iVolume;
    ar << m_iFadeOutFrame;
    ar << m_fMinDistance;
    ar << m_fMaxDistance;
    m_fMinDistance  = 200.f;
    m_fMaxDistance  = 600.f;
    return TRUE;
}

void RtsAudio3D::Render(RtgCamera& inCamera, RtgDevice& inDevice, RTGRenderMask eMask)
{
    RtsObject::Render(inCamera, inDevice, eMask);
    if (m_pSound==NULL)
    {
        m_pSound = g_pSoundMgr->CreateSound(m_szFileName, m_bLoop, m_b3D, m_iVolume);
        m_pSound->SetDistance(m_fMinDistance, m_fMaxDistance);
        m_pSound->SetFadeOutFrame(m_iFadeOutFrame);
        ResetPosition();
    }
}

void RtsAudio3D::OnMatrixChanged()
{
    ResetPosition();
}

void RtsAudio3D::ResetPosition()
{
    if (m_pSound)
    {
        RtgVertex3 vPos(GetMatrix().Position());
        m_pSound->SetPosition(vPos);
    }
}

void RtsAudio3D::Play()
{
    ResetPosition();
    if (m_pSound) m_pSound->Play();
}

void RtsAudio3D::Stop()
{
    if (m_pSound)
    {
        m_pSound->Stop();
    }
}

/*------------------------------------------------------------------------
  The End.
------------------------------------------------------------------------*/
