
// 场景的灯光 (主要是点光源)
class RtsDynamicLight : public RtsLight
{
    RT_DECLARE_DYNCREATE(RtsDynamicLight, RtsLight, 0, "scene")
protected:
    RtsDynamicLight();
    virtual ~RtsDynamicLight();

public:
    virtual long Serialize(RtArchive& ar);

};

/*------------------------------------------------------------------------
  The End.
------------------------------------------------------------------------*/
