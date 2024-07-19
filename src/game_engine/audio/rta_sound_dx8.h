
/*----------------------------------------------------------------------------
-   class RtaSoundDx8.
----------------------------------------------------------------------------*/

struct RtaSoundItem
{
    RtString        szFilename;
    DWORD		    dwID;           // 0 表示没有使用
    CSound          sound;          // dx Sound
    int 		    iFadeOutFrame;  // 每 iFadeOutFrame 就把声音减弱一次,一直到没有声音
    bool		    bLoop;

public:
    RtaSoundItem() { Reset(); }
    void Reset()
    {
        dwID = 0;
        iFadeOutFrame = 0;
        bLoop = false;
    }
};

/*----------------------------------------------------------------------------
-   class RtaSoundDx8.
----------------------------------------------------------------------------*/

class RtaSoundDx8 : public RtaSound
{
public:
    RtaSoundDx8();
    virtual ~RtaSoundDx8();
    virtual bool Initialize(HWND hWnd);
    virtual void Cleanup();

public:
    virtual DWORD Play(const char* szFilename, float fVolumeOffset, bool bLoop, bool b3D, Rta3DSoundParam* p3DParam);
    virtual bool Stop(DWORD dwID, int iFadeOutFrame);
    virtual void StopAll(int iFadeOutFrame);
    virtual void ClearAll();
    virtual void PauseAll();
    virtual void RestartAll();
    virtual void FadeOut();

    virtual void Set3DListener(Rta3DListener& in3DListener);
    virtual void Set3DParameters(DWORD dwID, Rta3DSoundParam& inParam);
    virtual void SetVolume(DWORD dwID, int iVolume);
    virtual void SetVolumeOffset(DWORD dwID, float fVolumeOffset);

    virtual bool IsPlaying(DWORD dwID);

    virtual void SetWholeVolume(float fVolume); // fVolume的值为0.f到1.f
    virtual float GetWholeVolume(); // fVolume的值为0到100

protected:
    void DeleteAudio(RtaSoundItem* pItem);

protected:
    RtaSoundItem* FindPlayingAudio(DWORD dwID);
    RtaSoundItem* FindAndRemoveStop(const char* szFilename); // 找到一个已经载入并且已经停止播放的Audio
    RtaSoundItem* AddNew(const char* szFilename, bool bLoop, bool b3D);
    bool DeleteMinCostAudio();
    DWORD GetNewID();

protected:
    CSoundManager*              m_pSoundManager;
    std::list<RtaSoundItem*>    m_listPlaying;
    std::list<RtaSoundItem*>    m_listStopping;
    std::list<RtaSoundItem*>    m_listStoped;

    float           m_fWholeVolumeIdx;      // 总体音量
    bool            m_bInit;
    long            m_lVolumeLevels[101];   // 不同级别的音量对应的值

};

/*----------------------------------------------------------------------------
-   The End.
----------------------------------------------------------------------------*/
