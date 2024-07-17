
namespace rt_graph {

/*----------------------------------------------------------------------------
-   CConsoleDefault
----------------------------------------------------------------------------*/

class CConsoleDefault : public CRtgAppFrame::CConsole
{
public:
    enum { CONSOLE_LINE   = 20 };
    enum { CONSOLE_LENGTH = 100};
    CConsoleDefault(CRtgAppFrame* pAppFrame);
	CConsoleDefault(const CConsoleDefault &object);
    virtual ~CConsoleDefault();
    virtual void Init();
    virtual void Close(RtgDevice* pDevice);
    virtual void AddConsoleString(const char* szString);
    virtual void ClearConsole();
    virtual void ShowConsole(RtgDevice* pDevice, bool bShowConsole);
    virtual void OnKeyChar(int iChar, bool bAltDown);
    virtual void ShowConsole(bool bShow);

protected:
    //char        m_szConsoleString[CONSOLE_LINE][CONSOLE_LENGTH];
    //char        m_szConsoleInput[CONSOLE_LENGTH];
	CM_MEMDEF2(m_szConsoleString, CONSOLE_LINE, CONSOLE_LENGTH)
	CM_MEMDEF(m_szConsoleInput, CONSOLE_LENGTH)
    int         m_iConsoleLineCur;
    int         m_iConsoleFadeCur;
    int         m_iConsoleFadeFrame;
    RtgShader   m_ConsoleShader;
};

} // namespace rt_graph

/*----------------------------------------------------------------------------
-   The End.
----------------------------------------------------------------------------*/
