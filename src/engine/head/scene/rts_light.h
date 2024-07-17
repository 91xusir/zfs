

// 场景的灯光 (主要是点光源)
class RtsLight : public RtsObject, public RtgLightItem
{
    RT_DECLARE_DYNCREATE(RtsLight, RtsObject, 0, "scene")
protected:
    RtsLight();
    virtual ~RtsLight();

public:
    virtual long Serialize(RtArchive& ar);
    void  ResetPosition();
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
public:
    BOOL        m_bEnable;          // 是否使用灯光

};

/*------------------------------------------------------------------------
-   The End.
------------------------------------------------------------------------*/
