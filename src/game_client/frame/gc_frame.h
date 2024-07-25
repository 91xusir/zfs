
class GcLogin;
// add [3/14/2009 tooth.shi]
// ��ȡ��Ⱦ�豸
RtgDeviceD3D9	*GetDevice();
// ��ȡ��ʱ��
GcTimer		*GetTimer();

//ldr123 
//�Ƿ���logģʽ(debug,releaseģʽ�µ�showlog)
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

    //virtual bool OnPreCreate3DDevice();		//��ʱ����Ҫ
    //virtual bool OnAfterCreate3DDevice();		//��ʱ����Ҫ

    virtual bool OnDeviceInit();				//Device��ʼ��
	//virtual void OnDeviceClose();				//�����˳�

    virtual bool OnRestoreDevice();				//�ָ�����
    virtual bool OnInvalidateDevice();			//����ʧЧ

	virtual void OnBeginRender();
    virtual void OnEndRender();

	virtual void OnRender();					//��Ⱦ
    virtual void OnRender2D();
    virtual void OnFrameMove(float fDifTime);	//֡ѭ��

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
    // �����¼�߼�
    bool OnEnterLogin();
    // �뿪��¼�߼�
    void OnLeaveLogin();
    // ������Ϸ����
    bool OnEnterGame();
    // �뿪��Ϸ����
    void OnLeaveGame();

#if DO_CONSOLE
    virtual bool OnConsoleCommand(const char* szCommand);
#endif

public:
    GcUserInput*    m_pCurrentProcess;          // ָ��ǰ������� Login|World
    BOOL            m_bChangeGameFlowNextFrame; // ��־�Ƿ�����һ֡������Ϸ����
    BOOL            m_bNextGameFlow;            // ��־�Ƿ������һ����Ϸ����

    BOOL            m_bUIInit;

    GcLogin*        m_pLogin;
    GcWorld*        m_pWorld;
	CItemManager*	m_pItemManager;
	// begin gc_frame.h [3/15/2009 tooth.shi]
	//RtwFont		    mFont;
	// end gc_frame.h [3/15/2009 tooth.shi]

    // �ػ������Ϣ
    //     ���ڽػ�״̬�͵ȴ���갴��������������ʾȷ�ϣ����Ҽ������м���ʾȡ��
    bool            m_bMouseCapture;        // �Ƿ�ػ������Ϣ
    int             m_iMouseCaptureID;      // �Զ����ID��������ʾ��˭�ػ�����Ϣ
    void*           m_pMouseCaptureData;    // �Զ��������
    std::string     m_szMouseCaptureMsg;    // ��ʾ����Ϣ

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