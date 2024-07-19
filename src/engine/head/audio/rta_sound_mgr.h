
namespace rt_audio {

/*----------------------------------------------------------------------------
-   class RtaSoundNode.
----------------------------------------------------------------------------*/
class RtaSoundNode
{
    RtaSoundNode();
    ~RtaSoundNode();

public:
    friend class RtaSoundManager;
    void Play();  // Play֮ǰӦ���ȸ���λ��(Position)������Ĭ����0,0,0λ��
    void Stop();

    void SetPosition(const RtgVertex3& vPos);
    void SetSpeed(const RtgVertex3& vSpeed);

    // Volume ��ֵ��Ϊ 0.f - 1.f
    void SetVolume(float fVolume);
    void SetVolumeOffset(float fVolumeOffset);
    float GetVolume()                           { return m_fVolume; }

    void SetFadeOutFrame(int iFadeOutFrame)     { m_iFadeOutFrame = iFadeOutFrame; }
    int  GetFadeOutFrame()                      { return m_iFadeOutFrame; }

    void SetDistance(float fMin=10.f, float fMax=500.f);

    bool IsPlaying();

    void Update(float fCurSecond); // fCurSecond�ǵ�ǰ��ʱ��

private:
    RtString        m_szFilename;
    DWORD           m_dwID;
    Rta3DSoundParam m_param;

    BYTE	        m_cLoopType;        // ѭ������: 0 ֻ����һ��, 1 ѭ������, 2 ����Delayʱ�䲥��
    bool            m_b3D;              // �Ƿ���3D��Ч
    bool            m_bPlayEnd;

    float           m_fVolume;
    int             m_iFadeOutFrame;

    float           m_fDelaySecond;     // Delay��ʱ��
    float           m_fCfgDelaySecond;  // ����Delay��ʱ��
    float           m_fCfgDelayOffset;  // Delay��ƫ�ƣ��ܵ�Delayʱ���� (fRandomRange��-1��1֮���ֵ)
                                        //     m_fDelaySecond = m_fCfgDelaySecond + fRandomRange * m_fCfgDelayOffset

};

/*----------------------------------------------------------------------------
-   class RtaSoundManager.
----------------------------------------------------------------------------*/
class RtaSoundManager
{
public:
    friend class RtaSoundNode;
    RtaSoundManager();
    virtual ~RtaSoundManager();

    bool Initialize(HWND hwnd);
    void Cleanup();

public:
    void SetAudioPath(const char* szPathname); // ����Ŀ¼�����е���Ч�ļ���Ҫ�����Ŀ¼���棬(eg. "audio/")

    // cLoopType ѭ������: 0 ֻ����һ��, 1 ѭ������, 2 ����Delayʱ�䲥��
    //     Volume ��ֵ��Ϊ 0.f - 1.f
    RtaSoundNode* CreateSound(const char* szFilename, BYTE cLoopType, bool b3D, float fVolume);
    void DestroySound(RtaSoundNode* pSoundNode);

    void ClearAll();
    void StopAll(int iFadeOutFrame=10);

    void PlayOnce(const char* szFilename, bool b3D, float fVolumeOffset, const RtgVertex3& vPos);

    void UpdateAll(RtgVertex3* pListenerPos, float fCurSecond);

    void BeginRender();
    void Render(RtaSoundNode* pSoundNode);
    void EndRender();

    void SetWholeVolume(float fVolume); // fVolume��ֵΪ0.f��1.f
    float GetWholeVolume(); // fVolume��ֵΪ0.f��1.f

    void SetUse3dAudio(bool b3D)    { m_bUse3D = b3D; }
    bool GetUse3DAudio()            { return m_bUse3D!=0; }

    int GetLastChannel()    { return m_iLastUpdateChannel; }

public: // ͳ��
    int GetUseAudioCnt();
    DWORD GetUseAudioMem();

protected:
    RtaSound*       m_pSound;

    Rta3DListener   m_3dListener;

    int             m_iMaxChannel;
    int             m_bUse3D;

    int             m_iLastUpdateChannel;
    RtString        m_strPathname;
    std::set<RtaSoundNode*> m_listActive;

};
} // namespace rt_audio

/*----------------------------------------------------------------------------
-   The End.
----------------------------------------------------------------------------*/
