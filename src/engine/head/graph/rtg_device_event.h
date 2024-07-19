
namespace rt_graph {

class RtgDeviceD3D9;
 
class RtgDeviceEvent : public RtObject
{
public:
    RT_DECLARE_DYNCREATE(RtgDeviceEvent, RtObject, 0, "RtGraph")
    RtgDeviceEvent(){ m_pDevice = NULL; }
    virtual ~RtgDeviceEvent(){}
    RtgDeviceD3D9* GetDevice()              { return m_pDevice; }

public:
    virtual bool OnPreCreate3DDevice() { return true; }
    virtual bool OnAfterCreate3DDevice() { return true; }
    virtual bool OnDeviceInit()  { return true; }
    virtual void OnDeviceClose() {}
    virtual bool OnRestoreDevice() { return true; }
    virtual bool OnInvalidateDevice() { return true; }
    virtual void OnBeginRender(){}
    virtual void OnRender(){}
	virtual void OnRender2D(){}
    virtual void OnEndRender(){}
    virtual void OnClearZRender(){}
    virtual void OnFrameMove(float fDifTime){}
    virtual bool OnErrorMessage(const char* szMsg, int iType){ return false; }

protected:
    RtgDeviceD3D9* m_pDevice;
};

} // namespace rt_graph

/*----------------------------------------------------------------------------
-   The End.
----------------------------------------------------------------------------*/
