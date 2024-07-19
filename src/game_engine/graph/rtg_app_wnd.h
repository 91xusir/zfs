
namespace rt_graph {

class RtgiDefaultWnd
{
public:
    RtgiDefaultWnd()
	{
		CM_MEMPROTECTOR(m_szWindowTitle, 256)
		//GetDefWnd(this);
	}
	~RtgiDefaultWnd()
	{
		CM_MEMUNPROTECTOR(m_szWindowTitle)
	}
    bool rtgiCreateWindow( const char* strWindowTitle, HINSTANCE hInstance, HICON hIcon, HMENU hMenu, int x, int y );

public:
    static RtgiDefaultWnd* GetRtWnd(HWND hWnd);

private:
    int     m_iDefaultStartX;       // ��ʼ���ڵ�Xλ�ã�Ĭ��Ϊ -1
    int     m_iDefaultStartY;       // ��ʼ���ڵ�Yλ�ã�Ĭ��Ϊ -1
    int     m_iDefaultWidth;        // Ĭ�Ͽ�ȣ�Ĭ��Ϊ 0;
    int     m_iDefaultHeight;       // Ĭ�ϸ߶ȣ�Ĭ��Ϊ 0;
    DWORD   m_dwWindowStyle;        // Ĭ�ϴ��ڷ��
    bool    m_bWindowCreatedWithDefaultPositions;
    //char    m_szWindowTitle[256];
	CM_MEMDEF(m_szWindowTitle, 256)
    RECT    m_rcWindowClient;
    RECT    m_rcWindowBounds;
    HWND    m_hWnd;
    bool    m_Keys[256];                       // array of key state
    bool    m_MouseButtons[5];                 // array of mouse states
};

LRESULT CALLBACK rtgiStaticWndProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );

} // namespace rt_graph

/*----------------------------------------------------------------------------
-   The End.
----------------------------------------------------------------------------*/
