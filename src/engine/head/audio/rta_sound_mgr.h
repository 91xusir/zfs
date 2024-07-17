
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
    void Play();  // Play之前应该先更新位置(Position)，否则默认是0,0,0位置
    void Stop();

    void SetPosition(const RtgVertex3& vPos);
    void SetSpeed(const RtgVertex3& vSpeed);

    // Volume 的值域为 0.f - 1.f
    void SetVolume(float fVolume);
    void SetVolumeOffset(float fVolumeOffset);
    float GetVolume()                           { return m_fVolume; }

    void SetFadeOutFrame(int iFadeOutFrame)     { m_iFadeOutFrame = iFadeOutFrame; }
    int  GetFadeOutFrame()                      { return m_iFadeOutFrame; }

    void SetDistance(float fMin=10.f, float fMax=500.f);

    bool IsPlaying();

    void Update(float fCurSecond); // fCurSecond是当前的时间

private:
    RtString        m_szFilename;
    DWORD           m_dwID;
    Rta3DSoundParam m_param;

    BYTE	        m_cLoopType;        // 循环类型: 0 只播放一次, 1 循环播放, 2 根据Delay时间播放
    bool            m_b3D;              // 是否是3D音效
    bool            m_bPlayEnd;

    float           m_fVolume;
    int             m_iFadeOutFrame;

    float           m_fDelaySecond;     // Delay的时间
    float           m_fCfgDelaySecond;  // 配置Delay的时间
    float           m_fCfgDelayOffset;  // Delay的偏移，总的Delay时间是 (fRandomRange是-1到1之间的值)
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
    void SetAudioPath(const char* szPathname); // 设置目录，所有的音效文件都要在这个目录里面，(eg. "audio/")

    // cLoopType 循环类型: 0 只播放一次, 1 循环播放, 2 根据Delay时间播放
    //     Volume 的值域为 0.f - 1.f
    RtaSoundNode* CreateSound(const char* szFilename, BYTE cLoopType, bool b3D, float fVolume);
    void DestroySound(RtaSoundNode* pSoundNode);

    void ClearAll();
    void StopAll(int iFadeOutFrame=10);

    void PlayOnce(const char* szFilename, bool b3D, float fVolumeOffset, const RtgVertex3& vPos);

    void UpdateAll(RtgVertex3* pListenerPos, float fCurSecond);

    void BeginRender();
    void Render(RtaSoundNode* pSoundNode);
    void EndRender();

    void SetWholeVolume(float fVolume); // fVolume的值为0.f到1.f
    float GetWholeVolume(); // fVolume的值为0.f到1.f

    void SetUse3dAudio(bool b3D)    { m_bUse3D = b3D; }
    bool GetUse3DAudio()            { return m_bUse3D!=0; }

    int GetLastChannel()    { return m_iLastUpdateChannel; }

public: // 统计
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
