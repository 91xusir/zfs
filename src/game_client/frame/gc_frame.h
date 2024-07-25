
class GcLogin;
// add [3/14/2009 tooth.shi]
// 获取渲染设备
RtgDeviceD3D9	*GetDevice();
// 获取计时器
GcTimer		*GetTimer();

//ldr123 
//是否是log模式(debug,release模式下的showlog)
extern bool	g_bLogEnable;

class CGameClientFrame : public CRtgAppFrame
{
public:
	RT_DECLARE_DYNCREATE(CGameClientFrame, CRtgAppFrame, 0, "")// add [3/16/2009 tooth.shi]
    CGameClientFrame();
    virtual ~CGameClientFrame();

    bool SetMouseCapture(int iID, enum GameCursor eCursor, void* pData, const char* szMsg);
    void CancelMouseCapture();

    // Device Event

    //virtual bool OnPreCreate3DDevice();		//暂时不需要
    //virtual bool OnAfterCreate3DDevice();		//暂时不需要

    virtual bool OnDeviceInit();				//Device初始化
	//virtual void OnDeviceClose();				//驱动退出

    virtual bool OnRestoreDevice();				//恢复驱动
    virtual bool OnInvalidateDevice();			//驱动失效

	virtual void OnBeginRender();
    virtual void OnEndRender();

	virtual void OnRender();					//渲染
    virtual void OnRender2D();
    virtual void OnFrameMove(float fDifTime);	//帧循环

    // Mouse Event
	virtual void OnMouseMove(int x, int y, int increaseX, int increaseY);
	virtual void OnMouseWheel(/*int iButton, */long vDelta,int x,int y);

    virtual void OnMouseLDown(/*int iButton, */int x, int y);
    virtual void OnMouseLUp(/*int iButton, */int x, int y);
    virtual void OnMouseLDrag(/*int iButton, */int x, int y, int increaseX, int increaseY);
    virtual void OnMouseLDClick(/*int iButton, */int x, int y);

    virtual void OnMouseMDown(/*int iButton,*/ int x, int y);
    virtual void OnMouseMUp(/*int iButton, */int x, int y);
    virtual void OnMouseMDrag(/*int iButton, */int x, int y, int increaseX, int increaseY);
    virtual void OnMouseMDClick(/*int iButton,*/ int x, int y);

    virtual void OnMouseRDown(/*int iButton, */int x, int y);
    virtual void OnMouseRUp(/*int iButton,*/ int x, int y);
    virtual void OnMouseRDrag(/*int iButton, */int x, int y, int increaseX, int increaseY);
    virtual void OnMouseRDClick(/*int iButton, */int x, int y);

	// Keyboard Event
	virtual void OnKeyDown(/*int iButton,*/ int iKey, bool bAltDown);
	virtual void OnKeyUp(/*int iButton,*/ int iKey, bool bAltDown);
	virtual void OnKeyChar(/*int iButton,*/ int iChar, bool bAltDown);

	// Window Event
	virtual	void OnKillFocus();
	virtual	void OnSetFocus();

    // Other
    virtual bool OnFrameInit();
    virtual void OnFrameClose();
    // 进入登录逻辑
    bool OnEnterLogin();
    // 离开登录逻辑
    void OnLeaveLogin();
    // 进入游戏世界
    bool OnEnterGame();
    // 离开游戏世界
    void OnLeaveGame();

#if DO_CONSOLE
    virtual bool OnConsoleCommand(const char* szCommand);
#endif

public:
    GcUserInput*    m_pCurrentProcess;          // 指向当前处理过程 Login|World
    BOOL            m_bChangeGameFlowNextFrame; // 标志是否在下一帧更改游戏流程
    BOOL            m_bNextGameFlow;            // 标志是否存在下一个游戏流程

    BOOL            m_bUIInit;

    GcLogin*        m_pLogin;
    GcWorld*        m_pWorld;
	CItemManager*	m_pItemManager;
	// begin gc_frame.h [3/15/2009 tooth.shi]
	//RtwFont		    mFont;
	// end gc_frame.h [3/15/2009 tooth.shi]

    // 截获鼠标消息
    //     处于截获状态就等待鼠标按键，如果按左键表示确认，按右键或者中键表示取消
    bool            m_bMouseCapture;        // 是否截获鼠标消息
    int             m_iMouseCaptureID;      // 自定义的ID，用来标示是谁截获了消息
    void*           m_pMouseCaptureData;    // 自定义的数据
    std::string     m_szMouseCaptureMsg;    // 显示的消息

    bool m_bCanCaptureVideo;
    bool m_bShowStatScene;
    bool m_bShowStatChar;
    bool m_bShowStatClient;
    bool m_bShowStatFile;
    bool m_bShowStatAudio;
    bool m_bShowStatNet;

    static DWORD m_sTimerEvent;
    static DWORD m_sTimerRender;
    static DWORD m_sTimerRun;
    static DWORD m_sTimerUI;
    void ResetTimer();

};

/*----------------------------------------------------------------------------
    The End.
----------------------------------------------------------------------------*/

class CSceneMusicApp : public RtThread
{
public:
	CM_MEMDEF(m_szMusicName, 40)
	bool m_bChangeMusic;
	bool m_bloop;
	int m_nLoopTime;
	long m_lNextLoopTime;
	CSceneMusicApp();
	~CSceneMusicApp();
	void Play(const char* filename,bool loop = false,int looptime = 0);
	void Stop();
protected:
	virtual int OnRun();
};