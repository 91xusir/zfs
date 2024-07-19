
namespace rt_audio {

enum ERTA3dMode
{
    RTA3DMODE_NORMAL         = 0, // 默认
    RTA3DMODE_HEADRELATIVE      , //
    RTA3DMODE_DISABLE           , // 不进行3D计算, 没有3D效果
};

/*----------------------------------------------------------------------------
-   struct Rta3DSoundParam.
----------------------------------------------------------------------------*/
struct Rta3DSoundParam
{
    DWORD       dwSize;
    RtgVertex3  vPosition;
    RtgVertex3  vVelocity;
    DWORD       dwInsideConeAngle;  // 音锥内角度
    DWORD       dwOutsideConeAngle; // 音锥外角度
    RtgVertex3  vConeOrientation;   // 音锥内部音量
    long        lConeOutsideVolume; // 音锥外部音量
    float       fMinDistance;       // 最小距离, 从这一点开始随着距离的增大声音会减小。每多一个最小距离
                                    //   声音减小fRolloffFactor倍，当fRolloffFactor为1的时候变成1/2的音量
    float       fMaxDistance;       // 最大距离, 从这一点开始声音不再变化。需要设置DSBCAPS_MUTE3DATMAXDISTANCE
    DWORD       dwMode;             // ERTA3dMode 的值 默认 RTA3DMODE_NORMAL
};

/*----------------------------------------------------------------------------
-   struct Rta3DListener.
----------------------------------------------------------------------------*/
struct Rta3DListener
{
    DWORD       dwSize;             // 缓冲区大小
    RtgVertex3  vPosition;          // 耳朵的位置
    RtgVertex3  vVelocity;          // 速度
    RtgVertex3  vOrientFront;       // 向前方向
    RtgVertex3  vOrientTop;         // 向上方向
    float       fDistanceFactor;    // 距离因子, 默认为1.f (他只影响多普勒效果和ITD)
    float       fRolloffFactor;     // 越远越小声, 默认为1.f [0.f-10.f]
    float       fDopplerFactor;     // 多普勒效应, 默认为1.f [0.f-10.f]
};

/*----------------------------------------------------------------------------
-   class RtaSound.
----------------------------------------------------------------------------*/

class RtaSound
{
public:
    RtaSound();
    virtual ~RtaSound(){}
    virtual bool Initialize(HWND hwnd)=0;
    virtual void Cleanup()=0;

public:
    virtual DWORD Play(const char* szFilename, float fVolumeOffset, bool bLoop, bool b3D, Rta3DSoundParam* p3DParam=0)=0;
    virtual bool Stop(DWORD dwID, int iFadeOutFrame=0)=0;
    virtual void StopAll(int iFadeOutFrame)=0;
    virtual void ClearAll()=0;
    virtual void PauseAll()=0;
    virtual void RestartAll()=0;
    virtual void FadeOut()=0;

    virtual void Set3DListener(Rta3DListener& in3DListener)=0;
    virtual void Set3DParameters(DWORD dwID, Rta3DSoundParam& inParam)=0;
    virtual void SetVolume(DWORD dwID, int iVolume)=0;
    virtual void SetVolumeOffset(DWORD dwID, float fVolumeOffset)=0;

    virtual bool IsPlaying(DWORD dwID)=0;

    virtual void SetWholeVolume(float fVolume)=0; // fVolume的值为0到100
    virtual float GetWholeVolume()=0; // fVolume的值为0到100

public:

    // 设置声音最大数量和占用的最大内存
    //     这个设置必须在Initialize之前调用才有效
    void SetupSound(int iMaxSoundNum, DWORD dwMaxSoundMem)
    {
        m_iMaxSoundNumber = iMaxSoundNum;
        m_dwMaxSoundMemory = dwMaxSoundMem;
    }

protected:
    int     m_iMaxSoundNumber;
    DWORD   m_dwMaxSoundMemory;

public:
    // 统计信息
    int             m_iUseAudioCnt;
    DWORD           m_dwUseAudioMem;

};
} // namespace rt_audio

/*----------------------------------------------------------------------------
-   The End.
----------------------------------------------------------------------------*/
