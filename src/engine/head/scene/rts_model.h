
class RtsActor : public RtsObject
{
    RT_DECLARE_DYNAMIC(RtsActor, RtsObject, 0, "scene")
public:
    RtsActor(){}
    virtual ~RtsActor(){}
    virtual CRT_ActorInstance* GetActor()=0;
    virtual void FrameMove(float _es_time)
    {
        if (GetActor())
            GetActor()->Tick(_es_time);
    }

    enum { EADL_MAX_DYNAMIC_LIGHT = 2 };
    struct SDynamicLights
    {
        float               fDistance; // 灯光和物件的距离
        RtsDynamicLight*    pLight;    // 灯光的指针
    };
    SDynamicLights  m_pLights[EADL_MAX_DYNAMIC_LIGHT]; // 最多受EADL_MAX_DYNAMIC_LIGHT盏灯影响


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
};

class RtsModel : public RtsActor
{
    RT_DECLARE_DYNCREATE(RtsModel, RtsActor, 0, "scene")
protected:
    RtsModel();
    virtual ~RtsModel();

public:

    virtual bool CreateResource(const char fileName[], RtsSGrid* _grid, bool bForceLoad = false);
    virtual void ReleaseResource();

    virtual void Render(RtgCamera& inCamera, RtgDevice& inDevice, RTGRenderMask eMask);
    virtual void RenderHighLight(RtgCamera& inCamera, RtgDevice& inDevice, RtgVertex3 &color,RtgVertex3 &scale);
    virtual long Serialize(RtArchive& ar);
	static void UpdateThreadInstance();

    virtual const RtgAABB* Get_AABBox() const
    {
        if (m_pChar && m_pChar->ResourceReady())
            return m_pChar->GetBoundingBox();
        
        static RtgAABB _aabbox = RtgAABB(RtgVertex3(0.f, 0.f, 0.f), RtgVertex3(10.f, 10.f, 10.f));
        return &_aabbox;
    }

    virtual const RtgAABB* Get_World_AABBox() const
    {
        if (m_pChar && m_pChar->ResourceReady())
            return m_pChar->Get_WorldBoundBox();

        static RtgAABB _aabbox = RtgAABB(RtgVertex3(0.f, 0.f, 0.f), RtgVertex3(1.f, 1.f, 1.f));
        _aabbox.vPos = *(RtgVertex3*)&m_Matrix._30;
        return &_aabbox;
    }

     float GetModelScale()
     { 
         return ((RtgVertex3*)&(GetMatrix()._00))->Length(); 
     }

     virtual void FrameMove(float _timeMill) 
     {
         if (m_pChar)
             m_pChar->Tick(_timeMill);
     }

     virtual bool DataVaild() const
     {
         return !m_fileName.empty();
     }

public:
   virtual void OnMatrixChanged();

public:
    CRT_ActorInstance*  GetActor()  { return m_pChar; }
    farray<DWORD>       m_aryVc;
public:
    CRT_ActorInstance*  m_pChar;
    float               m_fRenderTime;

};

/*------------------------------------------------------------------------
-   The End.
------------------------------------------------------------------------*/
