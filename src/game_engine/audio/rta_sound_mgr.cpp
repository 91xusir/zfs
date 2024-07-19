
#include "graph/rt_graph.h"
#include "audio/rt_audio.h"
#include "dx8\dsutil.h"
#include "dx8\dxutil.h"
#ifdef RT_DEBUG_NEW
#   undef RT_NEW
#endif
#include "D3dx9math.h"
#ifdef RT_DEBUG_NEW
#   define RT_NEW RT_DEBUG_NEW
#endif
#include "rta_sound_dx8.h"

/*----------------------------------------------------------------------------
-   class RtaSoundNode.
----------------------------------------------------------------------------*/

RtaSoundNode::RtaSoundNode()
{
    m_dwID          = 0;
    m_cLoopType     = 0;
    m_b3D           = false;
    m_bPlayEnd      = true;
    m_fVolume       = 0;
    m_iFadeOutFrame = 1;

    m_fDelaySecond      = 0.f;
    m_fCfgDelaySecond   = 0.f;
    m_fCfgDelayOffset   = 0.f;

    memset(&m_param, 0, sizeof(Rta3DSoundParam));
    m_param.dwSize = sizeof(Rta3DSoundParam);
    m_param.fMinDistance = 200.f;
    m_param.fMaxDistance = 600.f;
    m_param.dwInsideConeAngle   = 360;
    m_param.dwOutsideConeAngle  = 360;
    m_param.vConeOrientation.z  = 1.f;
    m_param.lConeOutsideVolume  = 0;
    m_param.dwMode = RTA3DMODE_NORMAL;
}

RtaSoundNode::~RtaSoundNode()
{
    Stop();
}

void RtaSoundNode::Play()
{
    //RtCoreLog().Info("RtaSoundNode::Play [%s]\n", m_szFilename);
    m_dwID = g_pSoundMgr->m_pSound->Play(m_szFilename, m_fVolume, m_cLoopType==1, m_b3D, &m_param);
    m_bPlayEnd = false;
}

void RtaSoundNode::Stop()
{
    if (m_dwID==0) return;
    //RtCoreLog().Info("RtaSoundNode::Stop [%s]\n", m_szFilename);
    g_pSoundMgr->m_pSound->Stop(m_dwID, m_iFadeOutFrame);
    m_dwID = 0;
    m_bPlayEnd = true;
}

void RtaSoundNode::SetPosition(const RtgVertex3& vPos)
{
    m_param.vPosition = vPos;
}

void RtaSoundNode::SetSpeed(const RtgVertex3& vSpeed)
{
    m_param.vVelocity = vSpeed;
}

void RtaSoundNode::Update(float fCurSecond)
{
    if (m_dwID==0) return;
    if (m_cLoopType!=1)
    {
        if (!m_bPlayEnd)
        {
            m_bPlayEnd = !g_pSoundMgr->m_pSound->IsPlaying(m_dwID);
            if (m_cLoopType==2)
            {
                m_fDelaySecond = fCurSecond + m_fCfgDelaySecond + ((rand()%2000)-1000)/1000.f * m_fCfgDelayOffset;
            }
        }else
        {
            if (m_cLoopType==2)
            {
                if (fCurSecond>m_fDelaySecond)
                {
                    Play();
                }
            }
        }
    }
    if (!m_bPlayEnd)
    {
        g_pSoundMgr->m_pSound->Set3DParameters(m_dwID, m_param);
    }
}

void RtaSoundNode::SetVolume(float fVolume)
{
    m_fVolume = fVolume;
    m_fVolume = Clamp(m_fVolume, 0.f, 1.f);
    g_pSoundMgr->m_pSound->SetVolume(m_dwID, fVolume);
}

void RtaSoundNode::SetVolumeOffset(float fVolumeOffset)
{
    m_fVolume += fVolumeOffset;
    m_fVolume = Clamp(m_fVolume, 0.f, 1.f);
    g_pSoundMgr->m_pSound->SetVolumeOffset(m_dwID, fVolumeOffset);
}

bool RtaSoundNode::IsPlaying()
{
    if (m_dwID==0) return false;
    return g_pSoundMgr->m_pSound->IsPlaying(m_dwID);
}

void RtaSoundNode::SetDistance(float fMin, float fMax)
{
    m_param.fMinDistance = fMin;
    m_param.fMaxDistance = fMax;
    if (m_dwID)
    {
        g_pSoundMgr->m_pSound->Set3DParameters(m_dwID, m_param);
    }
}

/*----------------------------------------------------------------------------
-   class RtaSoundManager.
----------------------------------------------------------------------------*/

RtaSoundManager::RtaSoundManager()
{
    m_iMaxChannel       = 4;
    m_bUse3D            = TRUE;

    m_3dListener.fDistanceFactor    = 1.0f;
    m_3dListener.fDopplerFactor     = 1.0f;
    m_3dListener.fRolloffFactor     = 1.0f;

    m_3dListener.vOrientFront.Set(0.0f,0.0f,1.0f);
    m_3dListener.vOrientTop.Set(0.0f,1.0f,0.0f);

    m_3dListener.vPosition.Set(0.0f,0.0f,0.0f);
    m_3dListener.vVelocity.Set(1.0f,0.0f,0.0f);

    m_iLastUpdateChannel = 0;
}

RtaSoundManager::~RtaSoundManager()
{
}

bool RtaSoundManager::Initialize(HWND hWnd)
{
    m_pSound = RT_NEW RtaSoundDx8;
    if (m_pSound==0)
    {
        rtThrowf("ERROR: RtaSoundManager::Initialize() 内存分配失败");
        return false;
    }
    if (!m_pSound->Initialize(hWnd))
    {
        RtCoreLog().Error("RtAudio: Audio init fail!\n");
        return false;
    }
    return true;
}

void RtaSoundManager::Cleanup()
{
    if (m_pSound)
    {
        m_pSound->Cleanup();
        DEL_ONE(m_pSound);
    }
}

void RtaSoundManager::ClearAll()
{
    //RtCoreLog().Info("RtaSoundManager::ClearAll\n");
    m_pSound->ClearAll();
}

void RtaSoundManager::StopAll(int iFadeOutFrame)
{
    //RtCoreLog().Info("RtaSoundManager::StopAll\n");
    m_pSound->StopAll(iFadeOutFrame);
}

// 设置目录，所有的音效文件都要在这个目录里面，(eg. "audio/")
void RtaSoundManager::SetAudioPath(const char* szPathname)
{
    m_strPathname = szPathname;
}

void RtaSoundManager::BeginRender()
{
}

void RtaSoundManager::Render(RtaSoundNode* pSoundNode)
{
    m_listActive.insert(pSoundNode);
}

void RtaSoundManager::EndRender()
{
}

RtaSoundNode* RtaSoundManager::CreateSound(const char* szFilename, BYTE cLoopType, bool b3D, float fVolume)
{
    //RtCoreLog().Info("RtaSoundManager::CreateSound [%s]\n", szFilename);
    RtaSoundNode* pNewNode  = RT_NEW RtaSoundNode;

    pNewNode->m_szFilename  = m_strPathname * szFilename;
    pNewNode->m_cLoopType   = cLoopType;
    pNewNode->m_b3D         = b3D;
    pNewNode->m_fVolume     = fVolume;

    return pNewNode;
}

void RtaSoundManager::DestroySound(RtaSoundNode* pSoundNode)
{
    //RtCoreLog().Info("RtaSoundManager::DestroySound [%s]\n", pSoundNode->m_szFilename);
    if (pSoundNode==NULL) return;
    if (m_listActive.find(pSoundNode)!=m_listActive.end())
    {
        m_listActive.erase(pSoundNode);
    }
    DEL_ONE(pSoundNode);
}

void RtaSoundManager::UpdateAll(RtgVertex3* pListenerPos, float fCurSecond)
{
    if (pListenerPos)
    {
        m_3dListener.vPosition = *pListenerPos;
    }
    m_pSound->Set3DListener(m_3dListener);
    m_pSound->FadeOut(); // 处理声音渐隐

    // 计算优先级别
    float fPriority;
    RtgVertex3 vVector;
    RtaSoundNode* pNode;
    std::multimap<float, RtaSoundNode*> mapPriority;
    std::pair<float, RtaSoundNode*> pairPriority;
    std::list<RtaSoundNode*> listStop;
    std::set<RtaSoundNode*>::iterator itSet;
    for (itSet=m_listActive.begin(); itSet!=m_listActive.end(); itSet++)
    {
        pNode = *itSet;
        vVector = pNode->m_param.vPosition - m_3dListener.vPosition; // 取得向量
        fPriority = vVector.Length2(); // 取得距离 (重要度)

        if (fPriority<(pNode->m_param.fMaxDistance*pNode->m_param.fMaxDistance))
        {
            // 插入重要度队列，最小的排到最前面
            pairPriority.first = fPriority;
            pairPriority.second = pNode;
            mapPriority.insert(pairPriority);
        }else
        {
            listStop.push_back(pNode);
        }
    }

    // 根据优先级别播放
    m_iLastUpdateChannel = 0;
    std::multimap<float, RtaSoundNode*>::iterator itMap = mapPriority.begin();
    for (; itMap!=mapPriority.end(); itMap++)
    {
        pNode = (*itMap).second;
        if (m_iLastUpdateChannel<m_iMaxChannel && m_bUse3D)
        {
            if (pNode->m_dwID==0)
            {
                pNode->Play();
            }
            pNode->Update(fCurSecond);
        }else
        {
            m_listActive.erase(pNode);
            pNode->Stop(); // 如果超出播放的限制，就停止播放
        }
        m_iLastUpdateChannel ++;
    }

    std::list<RtaSoundNode*>::iterator itList = listStop.begin();
    for (; itList!=listStop.end(); itList++)
    {
        pNode = (*itList);
        m_listActive.erase(pNode);
        pNode->Stop();
    }
}

void RtaSoundManager::SetWholeVolume(float fVolume)
{
    m_pSound->SetWholeVolume(fVolume);
}

float RtaSoundManager::GetWholeVolume()
{
    return m_pSound->GetWholeVolume();
}

void RtaSoundManager::PlayOnce(const char* szFilename, bool b3D, float fVolumeOffset, const RtgVertex3& vPos)
{
    //RtCoreLog().Info("RtaSoundManager::PlayOnce [%s]\n", szFilename);
    Rta3DSoundParam param;
    memset(&param, 0, sizeof(Rta3DSoundParam));
    param.dwSize = sizeof(Rta3DSoundParam);
    param.fMinDistance = 10.f;
    param.fMaxDistance = 500.f;
    param.dwInsideConeAngle   = 360;
    param.dwOutsideConeAngle  = 360;
    param.vConeOrientation.z  = 1.f;
    param.lConeOutsideVolume  = 0;
    param.dwMode = RTA3DMODE_NORMAL;
	if (vPos.x != 0 || vPos.y != 0 || vPos.z != 0)
	{
		param.vPosition = vPos;
	}
    m_pSound->Play(m_strPathname * szFilename, GetWholeVolume(), false, b3D, &param);
}

int RtaSoundManager::GetUseAudioCnt()
{
    return m_pSound->m_iUseAudioCnt;
}

DWORD RtaSoundManager::GetUseAudioMem()
{
    return m_pSound->m_dwUseAudioMem;
}

/*----------------------------------------------------------------------------
-   The End.
----------------------------------------------------------------------------*/
