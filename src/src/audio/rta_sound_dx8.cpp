
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

LRESULT CALLBACK rta_core_MMIOProc(LPSTR lpmmioinfo, UINT uMsg, LPARAM lParam1, LPARAM lParam2);

RtaSoundDx8::RtaSoundDx8()
{
    m_pSoundManager     = NULL;

    m_bInit             = false;
    m_fWholeVolumeIdx   = 1.f;

    m_lVolumeLevels[0]  = -10000;
    for(int i=1; i<=100; ++i)
    {
        m_lVolumeLevels[i] = -2000.00f * log10(100.0f/i);
    }
}

RtaSoundDx8::~RtaSoundDx8()
{
    Cleanup();
}

bool RtaSoundDx8::Initialize(HWND hWnd)
{
    HRESULT hr;
    if(m_pSoundManager)
    {
        DEL_ONE(m_pSoundManager);
    }

    m_pSoundManager = RT_NEW CSoundManager();
    if( FAILED( hr = m_pSoundManager->Initialize(hWnd, DSSCL_PRIORITY, 2, 22050, 16) ) )
    {
        RtCoreLog().Error("声音错误: 初始化声音系统失败 [%d].\n", hr );
        DEL_ONE(m_pSoundManager);
        m_bInit = false;
        return false;
    }
    // mmio I/O proc
    mmioInstallIOProc(mmioFOURCC('W', 'A', 'V', ' '), &rta_core_MMIOProc, MMIO_INSTALLPROC|MMIO_GLOBALPROC);
    m_bInit = true;
    return true;
}

void RtaSoundDx8::Cleanup()
{
    if (!m_bInit) return;
    ClearAll();
    DEL_ONE(m_pSoundManager);
    mmioInstallIOProc(mmioFOURCC('W', 'A', 'V', ' '), NULL, MMIO_REMOVEPROC);
    m_bInit             = false;
    m_fWholeVolumeIdx   = 0.f;
    m_iUseAudioCnt      = 0;
    m_dwUseAudioMem     = 0;
}

DWORD RtaSoundDx8::Play(const char* szFilename, float fVolumeOffset, bool bLoop, bool b3D, Rta3DSoundParam* p3DParam)
{
    if (!m_bInit) return 0;

    DWORD dwFlag = bLoop ? DSBPLAY_LOOPING : 0;

    RtaSoundItem* pItem = FindAndRemoveStop(szFilename);
    if (pItem) //找到了
    {
        pItem->bLoop = bLoop;
        pItem->dwID  = GetNewID();
		pItem->iFadeOutFrame = 0;
    }else //加载新的音乐
    {
        pItem = AddNew(szFilename, bLoop, b3D);
        if (pItem==NULL)
        {
            return 0;
        }
    }

    float fVolume = m_fWholeVolumeIdx + fVolumeOffset;
    fVolume = Clamp(fVolume, 0.f, 1.f);
    //RtCoreLog().Info("    RtaSoundDx8::Play [%s]\n", szFilename);
    pItem->sound.Play(0, dwFlag, (LPDS3DBUFFER)p3DParam, m_lVolumeLevels[Clamp((int)(fVolume*100), 0, 100)], b3D);
    return pItem->dwID;
}

bool RtaSoundDx8::Stop(DWORD dwID, int iFadeOutFrame)
{
    if (!m_bInit) return false;
    RtaSoundItem* pItem = FindPlayingAudio(dwID);
    if (pItem)
    {
        if ( pItem->iFadeOutFrame==0 && pItem->sound.IsSoundPlaying()) // 如果还在播放
        {
            pItem->iFadeOutFrame = iFadeOutFrame;

            // 加入停止链表中，用于FadeOut
            m_listPlaying.remove(pItem);
            m_listStopping.push_back(pItem);
           // RtCoreLog().Info("    RtaSoundDx8::MoveToStopping [%s]\n", pItem->szFilename);
		}
		else
		{
			//RtCoreLog().Info("    RtaSoundDx8::NoStopping[%s]\n",pItem->szFilename);
		}
    }
    return true;
}

void RtaSoundDx8::StopAll(int iFadeOutFrame)
{
    if (!m_bInit) return;
    RtaSoundItem* pItem;
    std::list<RtaSoundItem*>::iterator it;
    for (it=m_listPlaying.begin(); it!=m_listPlaying.end(); it++)
    {
        pItem = *it;
        pItem->iFadeOutFrame = iFadeOutFrame;
        m_listStopping.push_back(*it);
    }
    m_listPlaying.clear();
}

void RtaSoundDx8::ClearAll()
{
    if (!m_bInit) return;
    while (DeleteMinCostAudio())
    {
    }
    CHECK(m_listPlaying.size()==0 && m_listStopping.size()==0 && m_listStoped.size()==0);
}

void RtaSoundDx8::PauseAll()
{
    if (!m_bInit) return;
}

void RtaSoundDx8::RestartAll()
{
    if (!m_bInit) return;
}

void RtaSoundDx8::Set3DListener(Rta3DListener& in3DListener)
{
    if (!m_bInit) return;
    DS3DLISTENER ds3d;
    ds3d.dwSize = sizeof(DS3DLISTENER);
    ds3d.flDistanceFactor   = in3DListener.fDistanceFactor;
    ds3d.flDopplerFactor    = in3DListener.fDopplerFactor;
    ds3d.flRolloffFactor    = in3DListener.fRolloffFactor;
    ds3d.vOrientFront       = D3DXVECTOR3(in3DListener.vOrientFront.x, in3DListener.vOrientFront.y, in3DListener.vOrientFront.z);
    ds3d.vOrientTop         = D3DXVECTOR3(in3DListener.vOrientTop.x, in3DListener.vOrientTop.y, in3DListener.vOrientTop.z);
    ds3d.vPosition          = D3DXVECTOR3(in3DListener.vPosition.x, in3DListener.vPosition.y, in3DListener.vPosition.z);
    ds3d.vVelocity          = D3DXVECTOR3(in3DListener.vVelocity.x, in3DListener.vVelocity.y, in3DListener.vVelocity.z);
    m_pSoundManager->Set3DListener(ds3d);
}

void RtaSoundDx8::Set3DParameters(DWORD dwID, Rta3DSoundParam& inParam)
{
    if (!m_bInit) return;
    RtaSoundItem* pItem = FindPlayingAudio(dwID);
    if (pItem)
    {
        pItem->sound.Set3dParameters((LPDS3DBUFFER)&inParam);
    }
}

void RtaSoundDx8::SetVolume(DWORD dwID, int iVolume)
{
    if (!m_bInit) return;
    RtaSoundItem* pItem = FindPlayingAudio(dwID);
    if (pItem)
    {
        iVolume = Clamp(iVolume, 0, 100);
        pItem->sound.SetVolume(m_lVolumeLevels[iVolume]);
    }
}

void RtaSoundDx8::SetVolumeOffset(DWORD dwID, float fVolumeOffset)
{
    if (!m_bInit) return;
    RtaSoundItem* pItem = FindPlayingAudio(dwID);
    if (pItem)
    {
        float fVolume = m_fWholeVolumeIdx + fVolumeOffset;
        pItem->sound.SetVolume(m_lVolumeLevels[Clamp((int)(fVolume*100), 0, 100)]);
    }
}

bool RtaSoundDx8::IsPlaying(DWORD dwID)
{
    if (!m_bInit) return false;
    RtaSoundItem* pItem = FindPlayingAudio(dwID);
    if (pItem && pItem->sound.IsSoundPlaying())
    {
        return true;
    }
    return false;
}

void RtaSoundDx8::SetWholeVolume(float fVolume)
{
    m_fWholeVolumeIdx = Clamp(fVolume, 0.f, 1.f);
}

float RtaSoundDx8::GetWholeVolume()
{
    return m_fWholeVolumeIdx;
}

// 删除声音，这里只是删除内容，并没有删除列表
void RtaSoundDx8::DeleteAudio(RtaSoundItem* pItem)
{
    if (pItem==NULL) return;
    m_iUseAudioCnt --;
    m_dwUseAudioMem -= pItem->sound.GetBufferSize();
    pItem->sound.Stop();
    pItem->sound.Clear();
    pItem->Reset();
    DEL_ONE(pItem);
}

RtaSoundItem* RtaSoundDx8::FindAndRemoveStop(const char* szFilename)
{
    RtaSoundItem *pItem;
    std::list<RtaSoundItem*>::iterator it;
    for (it=m_listStoped.begin(); it!=m_listStoped.end(); it++)
    {
        pItem = (*it);
        if (pItem->szFilename==szFilename)
        {
            m_listPlaying.push_back(pItem);
            m_listStoped.erase(it);
            return pItem;
        }
    }

    for (it=m_listStopping.begin(); it!=m_listStopping.end(); it++)
    {
        pItem = (*it);
        if (pItem->szFilename==szFilename)
        {
            m_listPlaying.push_back(pItem);
            m_listStopping.erase(it);
            return pItem;
        }
    }

    return NULL; // 没有找到
}

bool RtaSoundDx8::DeleteMinCostAudio()
{
    RtaSoundItem* pItem;
    if (m_listStoped.size())
    {
        pItem = *(m_listStoped.begin());
        DeleteAudio(pItem);
        m_listStoped.pop_front();
        return true;
    }

    if (m_listStopping.size())
    {
        pItem = *(m_listStopping.begin());
        DeleteAudio(pItem);
        m_listStopping.pop_front();
        return true;
    }

    if (m_listPlaying.size())
    {
        pItem = *(m_listPlaying.begin());
        DeleteAudio(pItem);
        m_listPlaying.pop_front();
        return true;
    }
    return false;
}

RtaSoundItem* RtaSoundDx8::AddNew(const char* szFilename, bool bLoop, bool b3D)
{
    if (m_iUseAudioCnt>=m_iMaxSoundNumber)
    {
        if (!DeleteMinCostAudio())
        {
            return NULL;
        }
    }

    // 创建新的声音:

    RtaSoundItem* pNewItem = RT_NEW RtaSoundItem;
    pNewItem->szFilename   = szFilename;
    pNewItem->dwID         = GetNewID();
    pNewItem->bLoop        = bLoop;
    char strFilename[MAX_PATH];
	rt2_strcpy(strFilename, szFilename);
    rt2_strcat(strFilename, "+");
    if( FAILED(m_pSoundManager->Create(&(pNewItem->sound), (char*)strFilename, b3D ? (DSBCAPS_CTRL3D|DSBCAPS_MUTE3DATMAXDISTANCE|DSBCAPS_CTRLVOLUME) : DSBCAPS_CTRLVOLUME) ))
    {
        DEL_ONE(pNewItem);
        return NULL;
    }
    m_listPlaying.push_back(pNewItem);

    m_dwUseAudioMem += pNewItem->sound.GetBufferSize();
    m_iUseAudioCnt ++;

    // 如果内存使用过大，则删除已停止播放的声音:
    while (m_dwUseAudioMem>m_dwMaxSoundMemory && m_listPlaying.size()>1)
    {
        if (!DeleteMinCostAudio())
        {
            break;
        }
    }
    return pNewItem;
}

RtaSoundItem* RtaSoundDx8::FindPlayingAudio(DWORD dwID)
{
    std::list<RtaSoundItem*>::iterator it;
    for (it=m_listPlaying.begin(); it!=m_listPlaying.end(); it++)
    {
        if ((*it)->dwID==dwID)
        {
            return (*it);
        }
    }
    return NULL;
}

DWORD RtaSoundDx8::GetNewID()
{
    static DWORD dwNewID = 0;
    if (dwNewID==0) dwNewID ++;
    return dwNewID++;
}

void RtaSoundDx8::FadeOut()
{
    if (!m_bInit) return;

    static DWORD dwFrameCount = 0;
    dwFrameCount++;

    long lVolume;
    RtaSoundItem* pItem;
    std::list<RtaSoundItem*>::iterator it, itNext;
    for (it=m_listStopping.begin(); it!=m_listStopping.end(); it=itNext)
    {
        itNext = it;
        itNext ++;
        pItem = (*it);
        if ((dwFrameCount%pItem->iFadeOutFrame)==0)
        {
            lVolume = pItem->sound.DescendVolume(); // 减少音量
            if (lVolume<=-6000) // 小到听不见了
            {
                //RtCoreLog().Info("    RtaSoundDx8::FadeOut Stop [%s]\n", pItem->szFilename);
                pItem->sound.Stop();      // 停止
                pItem->iFadeOutFrame = 0;
                m_listStoped.push_back(pItem);
                m_listStopping.erase(it);
            }
        }
    }

    for (it=m_listPlaying.begin(); it!=m_listPlaying.end(); it=itNext)
    {
        itNext = it;
        itNext ++;
        pItem = (*it);
        if (!pItem->sound.IsSoundPlaying())
        {
            //RtCoreLog().Info("    RtaSoundDx8::Play End [%s]\n", pItem->szFilename);
            m_listStoped.push_back(pItem);
            m_listPlaying.erase(it);
        }
    }

}

LRESULT CALLBACK rta_core_MMIOProc(LPSTR lpmmioinfo, UINT uMsg, LPARAM lParam1, LPARAM lParam2)
{
    MMIOINFO* pInfo = (MMIOINFO*)lpmmioinfo;
    switch (uMsg)
    {
    case MMIOM_OPEN:
        {
            char strFilename[MAX_PATH];
            rt2_strcpy(strFilename, (const char*)lParam1);
            strFilename[strlen(strFilename)-1] = 0;
            pInfo->dwReserved1 = (DWORD)(DWORD_PTR)RtCoreFile().CreateFileReader(strFilename, 0);
            if (pInfo->dwReserved1==0)
            {
                return MMIOERR_CANNOTOPEN;
            }
        }
        break;
    case MMIOM_CLOSE:
        if (pInfo->dwReserved1)
        {
            RtCoreFile().CloseFile((RtArchive*)(DWORD_PTR)pInfo->dwReserved1);
        }
        break;
    case MMIOM_READ:
        if (pInfo->dwReserved1)
        {
            return ((RtArchive*)(DWORD_PTR)pInfo->dwReserved1)->Serialize((void*)lParam1, lParam2);
        }else
        {
            return 0;
        }
    case MMIOM_SEEK:
        if (pInfo->dwReserved1)
        {
            RtArchive::SeekMode eMode;
            switch (lParam2)
            {
            case SEEK_SET:  eMode = RtArchive::SeekBegin;   break;
            case SEEK_END:  eMode = RtArchive::SeekEnd;     break;
            case SEEK_CUR:  eMode = RtArchive::SeekCurrent; break;
            }
            ((RtArchive*)(DWORD_PTR)pInfo->dwReserved1)->Seek(lParam1, eMode);
            return ((RtArchive*)(DWORD_PTR)pInfo->dwReserved1)->Tell();
        }else
        {
            return 0;
        }
    case MMIOM_WRITE:
        if (pInfo->dwReserved1)
        {
            return ((RtArchive*)(DWORD_PTR)pInfo->dwReserved1)->Serialize((void*)lParam1, lParam2);
        }else
        {
            return 0;
        }
    case MMIOM_WRITEFLUSH:
        if (pInfo->dwReserved1)
        {
            int iWrote = ((RtArchive*)(DWORD_PTR)pInfo->dwReserved1)->Serialize((void*)lParam1, lParam2);
            ((RtArchive*)(DWORD_PTR)pInfo->dwReserved1)->Flush();
            return iWrote;
        }else
        {
            return 0;
        }
    }
    return MMSYSERR_NOERROR;
}

/*----------------------------------------------------------------------------
-   The End.
----------------------------------------------------------------------------*/
