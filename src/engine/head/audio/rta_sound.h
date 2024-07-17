
namespace rt_audio {

enum ERTA3dMode
{
    RTA3DMODE_NORMAL         = 0, // Ĭ��
    RTA3DMODE_HEADRELATIVE      , //
    RTA3DMODE_DISABLE           , // ������3D����, û��3DЧ��
};

/*----------------------------------------------------------------------------
-   struct Rta3DSoundParam.
----------------------------------------------------------------------------*/
struct Rta3DSoundParam
{
    DWORD       dwSize;
    RtgVertex3  vPosition;
    RtgVertex3  vVelocity;
    DWORD       dwInsideConeAngle;  // ��׶�ڽǶ�
    DWORD       dwOutsideConeAngle; // ��׶��Ƕ�
    RtgVertex3  vConeOrientation;   // ��׶�ڲ�����
    long        lConeOutsideVolume; // ��׶�ⲿ����
    float       fMinDistance;       // ��С����, ����һ�㿪ʼ���ž���������������С��ÿ��һ����С����
                                    //   ������СfRolloffFactor������fRolloffFactorΪ1��ʱ����1/2������
    float       fMaxDistance;       // ������, ����һ�㿪ʼ�������ٱ仯����Ҫ����DSBCAPS_MUTE3DATMAXDISTANCE
    DWORD       dwMode;             // ERTA3dMode ��ֵ Ĭ�� RTA3DMODE_NORMAL
};

/*----------------------------------------------------------------------------
-   struct Rta3DListener.
----------------------------------------------------------------------------*/
struct Rta3DListener
{
    DWORD       dwSize;             // ��������С
    RtgVertex3  vPosition;          // �����λ��
    RtgVertex3  vVelocity;          // �ٶ�
    RtgVertex3  vOrientFront;       // ��ǰ����
    RtgVertex3  vOrientTop;         // ���Ϸ���
    float       fDistanceFactor;    // ��������, Ĭ��Ϊ1.f (��ֻӰ�������Ч����ITD)
    float       fRolloffFactor;     // ԽԶԽС��, Ĭ��Ϊ1.f [0.f-10.f]
    float       fDopplerFactor;     // ������ЧӦ, Ĭ��Ϊ1.f [0.f-10.f]
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

    virtual void SetWholeVolume(float fVolume)=0; // fVolume��ֵΪ0��100
    virtual float GetWholeVolume()=0; // fVolume��ֵΪ0��100

public:

    // �����������������ռ�õ�����ڴ�
    //     ������ñ�����Initialize֮ǰ���ò���Ч
    void SetupSound(int iMaxSoundNum, DWORD dwMaxSoundMem)
    {
        m_iMaxSoundNumber = iMaxSoundNum;
        m_dwMaxSoundMemory = dwMaxSoundMem;
    }

protected:
    int     m_iMaxSoundNumber;
    DWORD   m_dwMaxSoundMemory;

public:
    // ͳ����Ϣ
    int             m_iUseAudioCnt;
    DWORD           m_dwUseAudioMem;

};
} // namespace rt_audio

/*----------------------------------------------------------------------------
-   The End.
----------------------------------------------------------------------------*/
