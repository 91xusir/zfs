
/*----------------------------------------------------------------------------
-   class RtaSoundDx8.
----------------------------------------------------------------------------*/

struct RtaSoundItem
{
    RtString        szFilename;
    DWORD		    dwID;           // 0 ��ʾû��ʹ��
    CSound          sound;          // dx Sound
    int 		    iFadeOutFrame;  // ÿ iFadeOutFrame �Ͱ���������һ��,һֱ��û������
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

    virtual void SetWholeVolume(float fVolume); // fVolume��ֵΪ0.f��1.f
    virtual float GetWholeVolume(); // fVolume��ֵΪ0��100

protected:
    void DeleteAudio(RtaSoundItem* pItem);

protected:
    RtaSoundItem* FindPlayingAudio(DWORD dwID);
    RtaSoundItem* FindAndRemoveStop(const char* szFilename); // �ҵ�һ���Ѿ����벢���Ѿ�ֹͣ���ŵ�Audio
    RtaSoundItem* AddNew(const char* szFilename, bool bLoop, bool b3D);
    bool DeleteMinCostAudio();
    DWORD GetNewID();

protected:
    CSoundManager*              m_pSoundManager;
    std::list<RtaSoundItem*>    m_listPlaying;
    std::list<RtaSoundItem*>    m_listStopping;
    std::list<RtaSoundItem*>    m_listStoped;

    float           m_fWholeVolumeIdx;      // ��������
    bool            m_bInit;
    long            m_lVolumeLevels[101];   // ��ͬ�����������Ӧ��ֵ

};

/*----------------------------------------------------------------------------
-   The End.
----------------------------------------------------------------------------*/
