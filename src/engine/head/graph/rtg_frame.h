
namespace rt_graph {

// 这个类是图形库的应用程序框架


class CRtgAppFrame : public RtgDeviceEvent
{
public:
    class CConsole
    {
    public:
        CConsole(CRtgAppFrame* pAppFrame)       { m_pAppFrame=pAppFrame; }
        virtual ~CConsole() {}
        virtual void Init() {}
        virtual void Close(RtgDevice* pDevice) {}

        virtual void AddConsoleString(const char* szString) {}
        virtual void ClearConsole() {}

        virtual void ShowConsole(RtgDevice* pDevice, bool bShowConsole) {}
        virtual void DoCommand(const char* szCommand) {}

        virtual void OnKeyChar(int iChar, bool bAltDown) {}
        virtual void ShowConsole(bool bShow){}
    protected:
        CRtgAppFrame*   m_pAppFrame;
    };
public:
    friend class RtgDevice;
    RT_DECLARE_DYNCREATE(CRtgAppFrame, RtgDeviceEvent, 0, "RtGraph")
    CRtgAppFrame();
    virtual ~CRtgAppFrame();

    bool Init(RtgDeviceD3D9* pDevice);
    void Close();
    int  Run(); // 执行应用程序,比如消息处理之类
    void Render();

    // Set
    void EnableNotifyOnMouseMove(bool bEnable)  { m_bNotifyOnMouseMove = bEnable; }
    void EnableCaptureMouse(bool bCapture)      { m_bCaptureMouse = bCapture; }
    void EnableConsole(bool bCan)               { m_bEnableConsole = bCan; }
    void ShowConsole(bool bShow);

    // Get
    HINSTANCE GetInstance();
    void* GetWndHandle();
    inline bool GetNotifyOnMouseMove()          { return m_bNotifyOnMouseMove; }
    inline bool GetCaptureMouse()               { return m_bCaptureMouse; }
    inline bool GetEnableConsole()              { return m_bEnableConsole; }
    inline bool GetShowConsole()                { return m_bShowConsole; }
    inline CConsole* GetConsole()               { return m_pConsole; }

    // Console
    bool DoConsoleCommand(const char* szCommand);

protected:
    // Device Event
    virtual bool OnPreCreate3DDevice();
    virtual bool OnAfterCreate3DDevice();
    virtual bool OnDeviceInit();
    virtual void OnDeviceClose();
    virtual bool OnRestoreDevice();
    virtual bool OnInvalidateDevice();
    virtual void OnBeginRender();
    virtual void OnEndRender();
    virtual void OnRender() {}
	virtual void OnRender2D() {}
    virtual void OnFrameMove(float fDifTime) {}

    // Other
    virtual bool OnFrameInit() { return true; }
    virtual void OnFrameClose(){}

    virtual bool OnConsoleCommand(const char* szCommand);

public:
    // Keyboard Event -------------------------------------------------------------------------
    virtual void OnKeyDown(int iChar, bool bAltDown);
    virtual void OnKeyUp(int iChar, bool bAltDown);
    virtual void OnKeyChar(int iChar, bool bAltDown);

    // Mouse Event ----------------------------------------------------------------------------
    virtual void OnMouseMove(int x, int y, int increaseX, int increaseY);
	virtual void OnMouseWheel(long vDelta,int x,int y);
	virtual void MouseWheel( long vDelta,int x,int y);

    virtual void OnMouseLDown(int x, int y);
    virtual void OnMouseLUp(int x, int y);
    virtual void OnMouseLDrag(int x, int y, int increaseX, int increaseY);
    virtual void OnMouseLDClick(int x, int y);

    virtual void OnMouseMDown(int x, int y);
    virtual void OnMouseMUp(int x, int y);
    virtual void OnMouseMDrag(int x, int y, int increaseX, int increaseY);
    virtual void OnMouseMDClick(int x, int y);

    virtual void OnMouseRDown(int x, int y);
    virtual void OnMouseRUp(int x, int y);
    virtual void OnMouseRDrag(int x, int y, int increaseX, int increaseY);
    virtual void OnMouseRDClick(int x, int y);

    // Windows message
    virtual void OnWindowActive(RTHWND hWnd, bool bActive){}
    virtual void OnExitSizeMove(RTHWND hWnd){}
	virtual long DefaultWindowProc(RTHWND hWnd,unsigned int Msg,unsigned int wParam,unsigned int lParam){return 0;}
	virtual	void OnKillFocus(){};
	virtual	void OnSetFocus(){};

protected:
    bool CheckInit(); // 检查是否能初始化

protected:
    // Console
    CConsole*       m_pConsole;
    bool            m_bEnableConsole;
    bool            m_bShowConsole;

public:
    // Keyboard & Mouse
    bool            m_bKeys[256];           // array of key state
    bool            m_bMouseButtons[3];     // array of mouse states [left, middle, right]
    bool            m_bNotifyOnMouseMove;
    bool            m_bCaptureMouse;
    bool            m_bUseDefaultWindowProc;
    int             m_iLastMousePos[2];

};

} // namespace rt_graph

/*----------------------------------------------------------------------------
-   The End.
----------------------------------------------------------------------------*/
