
/*------------------------------------------------------------------------
-   本文件里面都是一使场景有动态效果的东西.
------------------------------------------------------------------------*/

/*------------------------------------------------------------------------
-   RtsInnervation. 动态基类，用于定义动态的操作
------------------------------------------------------------------------*/

typedef void (*TInnervationMovementCB)(RtgVertex3& vLocalPos, float& fSpeed, float& fParamL1, float& fParamL2);

class RtsInnervation : public RtsObject
{
    RT_DECLARE_DYNCREATE(RtsInnervation, RtsObject, 0, "scene")
public:
    class SParticle : public CRT_PoseNotify
    {
    public:
        RtgVertex3  vGlobalPos;          // 当前的位置
        RtgVertex3  vDestinationPos;     // 当前要移动到的目的位置
        RtgVertex3  vLocalPos;           // 相对于当前位置的偏移位置
        float       fParamG1, fParamG2;  // 全局位置的参数
        float       fParamL1, fParamL2;  // 相对位置的参数
        TInnervationMovementCB  pCall;  // 回调函数
        float       fGlobalSpeed;
        float       fLocalSpeed;
        SParticle*  pNext;
        CRT_ActorInstance*  pChar;
        RtsInnervation*     pParent;

        virtual void OnPoseEnd(SRT_Pose *pose);
    };

protected:
    RtsInnervation();
    virtual ~RtsInnervation();

public:

    bool Init(int iParticleNum, RtgVertex3 vRangeHalf, float fGlobalSpeed, float fLocalSpeed);
    void Move(float fOffsetSecond);

    virtual bool CreateResource(const char fileName[], RtsSGrid* _grid, bool bForceLoad = false);
    virtual void ReleaseResource();

    virtual bool OnReload(RtgDevice* pDevice);
    virtual void Render(RtgCamera& inCamera, RtgDevice& inDevice, RTGRenderMask eMask);
    virtual long Serialize(RtArchive& ar);

    virtual void FrameMove(float _timeMill) 
    {
        Move(_timeMill / 1000.f);
    }


    // 当初始化粒子的时候
    virtual void OnInitParticle(){}
    // 当创建一个粒子的时候
    virtual void OnCreateParticle(SParticle* pParticle){}
    // 当一个粒子全局位置到达目的地的时候(返回true表示要删除该物体)
    virtual bool OnGlobalParticleDone(SParticle* pParticle){return false;}

    virtual void OnMatrixChanged()
    {
        Update_WorldBoundBox();
    }

    virtual const RtgAABB* Get_AABBox() const
    {
        return &m_AABB;
    }

    virtual const RtgAABB* Get_World_AABBox() const 
    {
        return &m_aabboxWorld;
    }

    void Update_WorldBoundBox()
    {
        RtgVertex3 _min = m_AABB.vPos - m_AABB.vExt;
        RtgVertex3 _max = m_AABB.vPos + m_AABB.vExt;

        RtgVertex3 _box[] = {
            RtgVertex3(_min.x, _min.y, _min.z),
            RtgVertex3(_max.x, _min.y, _min.z),
            RtgVertex3(_max.x, _max.y, _min.z),
            RtgVertex3(_min.x, _max.y, _min.z),
            RtgVertex3(_min.x, _min.y, _max.z),
            RtgVertex3(_max.x, _min.y, _max.z),
            RtgVertex3(_max.x, _max.y, _max.z),
            RtgVertex3(_min.x, _max.y, _max.z),
        };

        _min.Set(+1e10);
        _max.Set(-1e10);

        for (size_t i = 0; i < sizeof(_box) / sizeof(RtgVertex3); ++i)
        {
            RtgVertex3 _v = _box[i] * m_Matrix;

            if (_v.x < _min.x)
                _min.x = _v.x;
            if (_v.x > _max.x)
                _max.x = _v.x;
            if (_v.y < _min.y)
                _min.y = _v.y;
            if (_v.y > _max.y)
                _max.y = _v.y;
            if (_v.z < _min.z)
                _min.z = _v.z;
            if (_v.z > _max.z)
                _max.z = _v.z;
        }

        m_aabboxWorld.vPos.Set((_min.x + _max.x) * 0.5f, (_min.y + _max.y) * 0.5f, (_min.z + _max.z) * 0.5f);
        m_aabboxWorld.vExt.Set((_max.x - _min.x) * 0.5f, (_max.y - _min.y) * 0.5f, (_max.z - _min.z) * 0.5f);
    }

    virtual bool DataVaild() const
    {
        return ((!m_fileName.empty()) && (!m_szCharFileName.empty()));
    }

public:
    string      m_szCharFileName;       // 模型文件名

    unsigned char m_iParticleNum;         // 粒子总数
    bool        m_bPlaySecond;
    char        m_cPlaySecondRate;
    bool        m_bValid;
    RtgVertex3  m_vRangeHalf;           // 范围(一半)
    float       m_fGlobalSpeed;         // 全局移动的速度
    float       m_fLocalSpeed;          // 相对位置的移动速度

    float       m_fSecondRate;

    float       m_fLastCreateTime;
    float       m_fRenderTime;

    SParticle*  m_pParticles;
    SParticle*  m_pUseParticles;
    SParticle*  m_pUnuseParticles;

private :
    RtgAABB     m_aabboxWorld;
    RtgAABB     m_AABB;

public:
    static bool InitInnervation();
    static bool ClearInnervation();
    static bool RegisterMovement(const char* szName, TInnervationMovementCB pCallBack);

    struct SMovement
    {
        RtString                szName;
        TInnervationMovementCB  pCallBack;
        SMovement*              pNext;
    };
    static SMovement*           pMovementFunction;

};

/*------------------------------------------------------------------------
-   RtsInnerveFireFly. 萤火虫
------------------------------------------------------------------------*/

class RtsInnerveFireFly : public RtsInnervation
{
    RT_DECLARE_DYNCREATE(RtsInnerveFireFly, RtsInnervation, 0, "scene")
protected:
    RtsInnerveFireFly();
    virtual ~RtsInnerveFireFly();
    virtual bool OnReload(RtgDevice* pDevice);

public:
    virtual void Render(RtgCamera& inCamera, RtgDevice& inDevice, RTGRenderMask eMask);
    virtual void RenderHighLight(RtgCamera& inCamera, RtgDevice& inDevice, RtgVertex3 &color,RtgVertex3 &scale);
    virtual long Serialize(RtArchive& ar);

    virtual void OnInitParticle();
    virtual void OnCreateParticle(SParticle* pParticle);
    virtual bool OnGlobalParticleDone(SParticle* pParticle);

};

/*------------------------------------------------------------------------
-   The End.
------------------------------------------------------------------------*/
