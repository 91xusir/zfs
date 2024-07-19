

// 场景的3D音效
class RtsAudio3D : public RtsObject
{
    RT_DECLARE_DYNCREATE(RtsAudio3D, RtsObject, 0, "scene")

protected:
    RtsAudio3D();
    virtual ~RtsAudio3D();
    void OnMatrixChanged();

public:
    virtual long Serialize(RtArchive& ar);
    virtual void Render(RtgCamera& inCamera, RtgDevice& inDevice, RTGRenderMask eMask);
    void ResetPosition();
    void Play();
    void Stop();
    
    virtual const RtgAABB* Get_AABBox() const
    {
        static RtgAABB _aabox = RtgAABB(RtgVertex3(0.f, 0.f, 0.f), RtgVertex3(10.f, 10.f, 10.f));
        return &_aabox;
    }

    virtual const RtgAABB* Get_World_AABBox() const 
    {
        static RtgAABB _aabox = RtgAABB(RtgVertex3(0.f, 0.f, 0.f), RtgVertex3(10.f, 10.f, 10.f));
        _aabox.vPos = *(RtgVertex3*)&m_Matrix._30;
        return &_aabox;
    }

    virtual bool DataVaild() const
    {
        return !m_szFileName.IsEmpty();
    }

public:
    bool            m_bEnable;          // 是否使用
    RtString        m_szFileName;       // 文件名

    bool            m_bLoop;            // 是否循环
    bool	        m_b3D;              // 是否使用3D
    int             m_iVolume;          // 音量
    int             m_iFadeOutFrame;    // 淡出
    float           m_fMinDistance;     // 最近距离
    float           m_fMaxDistance;     // 最远距离

    RtaSoundNode*   m_pSound;
    RtgAABB         m_aaboxWorld;

    /*
    RtgGeometry     m_gMin;
    RtgGeometry     m_gMax;
    RtgShader       m_ShaderSphere;
    */
};

/*------------------------------------------------------------------------
  The End.
------------------------------------------------------------------------*/
