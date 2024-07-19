
namespace rt_graph {

    /*
static RtgiDefaultWnd* GetDefWnd(RtgiDefaultWnd* pWnd)
{
    static RtgiDefaultWnd* s_pWnd = NULL;
    if (pWnd)
    {
        s_pWnd = pWnd;
    }
    return s_pWnd;
}

RtgiDefaultWnd::RtgiDefaultWnd()
{
    GetDefWnd(this);
}

bool RtgiDefaultWnd::rtgiCreateWindow( const char* strWindowTitle, HINSTANCE hInstance, HICON hIcon, HMENU hMenu, int x, int y )
{
    if( hInstance == NULL ) 
        hInstance = (HINSTANCE)GetModuleHandle(NULL);

    char szExePath[MAX_PATH];
    GetModuleFileName( NULL, szExePath, MAX_PATH );
    if( hIcon == NULL ) // If the icon is NULL, then use the first one found in the exe
    {
        hIcon = ExtractIcon( hInstance, szExePath, 0 );
    }

    // Register the windows class
    WNDCLASS wndClass;
    wndClass.style = CS_DBLCLKS;
    wndClass.lpfnWndProc = rtgiStaticWndProc;
    wndClass.cbClsExtra = 0;
    wndClass.cbWndExtra = 0;
    wndClass.hInstance = hInstance;
    wndClass.hIcon = hIcon;
    wndClass.hCursor = LoadCursor( NULL, IDC_ARROW );
    wndClass.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
    wndClass.lpszMenuName = NULL;
    wndClass.lpszClassName = L"rtg3DWndClass";

    if( !RegisterClass( &wndClass ) )
    {
        DWORD dwError = GetLastError();
        if( dwError != ERROR_CLASS_ALREADY_EXISTS )
        {
            return false;
        }
    }

    // Set the window's initial style.  It is invisible initially since it might be resized later
    DWORD dwWindowStyle = WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_THICKFRAME | WS_MINIMIZEBOX | WS_MAXIMIZEBOX;
    m_dwWindowStyle = dwWindowStyle;

    RECT rc;

    // Override the window's initial & size position if there were cmd line args
    if( m_iDefaultStartX != -1 )
        x = m_iDefaultStartX;
    if( m_iDefaultStartY != -1 )
        y = m_iDefaultStartY;

    m_bWindowCreatedWithDefaultPositions = false;
    if( x == CW_USEDEFAULT && y == CW_USEDEFAULT )
        m_bWindowCreatedWithDefaultPositions = true;

    // Find the window's initial size, but it might be changed later
    int nDefaultWidth = 640;
    int nDefaultHeight = 480;
    if( m_iDefaultWidth != 0 )
        nDefaultWidth = m_iDefaultWidth;
    if( m_iDefaultHeight != 0 )
        nDefaultHeight = m_iDefaultHeight;
    SetRect( &rc, 0, 0, nDefaultWidth, nDefaultHeight );
    AdjustWindowRect( &rc, m_dwWindowStyle, ( hMenu != NULL ) ? true : false );

    char* strCachedWindowTitle = m_szWindowTitle;
    rt2_strncpy( strCachedWindowTitle, strWindowTitle, 256 );
    strCachedWindowTitle[255] = 0;

    // Create the render window
    HWND m_hWnd = CreateWindow( L"rtg3DWndClass", strWindowTitle, m_dwWindowStyle, x, y, (rc.right-rc.left), (rc.bottom-rc.top), 0, hMenu, hInstance, 0 );
    if( m_hWnd == NULL )
    {
        DWORD dwError = GetLastError();
        return false;
    }

    // Record the window's client & window rect
    RECT rcWindowClient;
    GetClientRect( m_hWnd, &rcWindowClient );
    m_rcWindowClient = rcWindowClient;

    RECT rcWindowBounds;
    GetWindowRect( m_hWnd, &rcWindowBounds );
    m_rcWindowBounds = rcWindowBounds;

    return true;
}

RtgiDefaultWnd* RtgiDefaultWnd::GetRtWnd(HWND hWnd)
{
    RtgiDefaultWnd* pWnd = pGetDefWnd(NULL);
    if (pWnd->m_hWnd==hWnd)
        return pWnd;
    return NULL;
}

/*
//--------------------------------------------------------------------------------------
// Handles window messages 
//--------------------------------------------------------------------------------------
LRESULT CALLBACK rtgiStaticWndProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
    RtgiDefaultWnd* pWnd = GetRtWnd(hWnd);
    if (pWnd==NULL)
    {
        CHECKEX("´íÎó");
        return DefWindowProc( hWnd, uMsg, wParam, lParam );
    }
    // Consolidate the keyboard messages and pass them to the app's keyboard callback
    if( uMsg == WM_KEYDOWN ||
        uMsg == WM_SYSKEYDOWN || 
        uMsg == WM_KEYUP ||
        uMsg == WM_SYSKEYUP )
    {
        bool bKeyDown = (uMsg == WM_KEYDOWN || uMsg == WM_SYSKEYDOWN);
        DWORD dwMask = (1 << 29);
        bool bAltDown = ( (lParam & dwMask) != 0 );

        pWnd->m_Keys[ (BYTE) (wParam & 0xFF) ] = bKeyDown;

        LPDXUTCALLBACKKEYBOARD pCallbackKeyboard = GetDXUTState().GetKeyboardFunc();
        if( pCallbackKeyboard )
            pCallbackKeyboard( (UINT)wParam, bKeyDown, bAltDown );
    }

    // Consolidate the mouse button messages and pass them to the app's mouse callback
    if( uMsg == WM_LBUTTONDOWN ||
        uMsg == WM_LBUTTONUP ||
        uMsg == WM_LBUTTONDBLCLK ||
        uMsg == WM_MBUTTONDOWN ||
        uMsg == WM_MBUTTONUP ||
        uMsg == WM_MBUTTONDBLCLK ||
        uMsg == WM_RBUTTONDOWN ||
        uMsg == WM_RBUTTONUP ||
        uMsg == WM_RBUTTONDBLCLK ||
        uMsg == WM_XBUTTONDOWN ||
        uMsg == WM_XBUTTONUP ||
        uMsg == WM_XBUTTONDBLCLK ||
        uMsg == WM_MOUSEWHEEL || 
        (GetDXUTState().GetNotifyOnMouseMove() && uMsg == WM_MOUSEMOVE) )
    {
        int xPos = (short)LOWORD(lParam);
        int yPos = (short)HIWORD(lParam);

        if( uMsg == WM_MOUSEWHEEL )
        {
            // WM_MOUSEWHEEL passes screen mouse coords
            // so convert them to client coords
            POINT pt;
            pt.x = xPos; pt.y = yPos;
            ScreenToClient( hWnd, &pt );
            xPos = pt.x; yPos = pt.y;
        }

        int nMouseWheelDelta = 0;
        if( uMsg == WM_MOUSEWHEEL ) 
            nMouseWheelDelta = (short) HIWORD(wParam);

        int nMouseButtonState = LOWORD(wParam);
        bool bLeftButton  = ((nMouseButtonState & MK_LBUTTON) != 0);
        bool bRightButton = ((nMouseButtonState & MK_RBUTTON) != 0);
        bool bMiddleButton = ((nMouseButtonState & MK_MBUTTON) != 0);
        bool bSideButton1 = ((nMouseButtonState & MK_XBUTTON1) != 0);
        bool bSideButton2 = ((nMouseButtonState & MK_XBUTTON2) != 0);

        bool* bMouseButtons = GetDXUTState().GetMouseButtons();
        bMouseButtons[0] = bLeftButton;
        bMouseButtons[1] = bMiddleButton;
        bMouseButtons[2] = bRightButton;
        bMouseButtons[3] = bSideButton1;
        bMouseButtons[4] = bSideButton2;

        LPDXUTCALLBACKMOUSE pCallbackMouse = GetDXUTState().GetMouseFunc();
        if( pCallbackMouse )
            pCallbackMouse( bLeftButton, bRightButton, bMiddleButton, bSideButton1, bSideButton2, nMouseWheelDelta, xPos, yPos );
    }

    // Pass all messages to the app's MsgProc callback, and don't 
    // process further messages if the apps says not to.
    LPDXUTCALLBACKMSGPROC pCallbackMsgProc = GetDXUTState().GetWindowMsgFunc();
    if( pCallbackMsgProc )
    {
        bool bNoFurtherProcessing = false;
        LRESULT nResult = pCallbackMsgProc( hWnd, uMsg, wParam, lParam, &bNoFurtherProcessing );
        if( bNoFurtherProcessing )
            return nResult;
    }

    switch( uMsg )
    {
        case WM_PAINT:
        {
            IDirect3DDevice9* pd3dDevice = DXUTGetD3DDevice();

            // Handle paint messages when the app is paused
            if( pd3dDevice && DXUTIsRenderingPaused() && 
                GetDXUTState().GetDeviceObjectsCreated() && GetDXUTState().GetDeviceObjectsReset() )
            {
                HRESULT hr;
                double fTime = DXUTGetTime();
                float fElapsedTime = DXUTGetElapsedTime();

                if( pD3DSettingsDlg && DXUTGetShowSettingsDialog() )
                {
                    // Clear the render target and the zbuffer 
                    pd3dDevice->Clear(0, NULL, D3DCLEAR_TARGET|D3DCLEAR_ZBUFFER, 0x00003F3F, 1.0f, 0);

                    // Render the scene
                    if( SUCCEEDED( pd3dDevice->BeginScene() ) )
                    {
                        pD3DSettingsDlg->OnRender( fElapsedTime );
                        pd3dDevice->EndScene();
                    }
                }
                else
                {
                    LPDXUTCALLBACKFRAMERENDER pCallbackFrameRender = GetDXUTState().GetFrameRenderFunc();
                    if( pCallbackFrameRender != NULL )
                        pCallbackFrameRender( pd3dDevice, fTime, fElapsedTime );
                }

                hr = pd3dDevice->Present( NULL, NULL, NULL, NULL );
                if( D3DERR_DEVICELOST == hr )
                {
                    GetDXUTState().SetDeviceLost( true );
                }
                else if( D3DERR_DRIVERINTERNALERROR == hr )
                {
                    // When D3DERR_DRIVERINTERNALERROR is returned from Present(),
                    // the application can do one of the following:
                    // 
                    // - End, with the pop-up window saying that the application cannot continue 
                    //   because of problems in the display adapter and that the user should 
                    //   contact the adapter manufacturer.
                    //
                    // - Attempt to restart by calling IDirect3DDevice9::Reset, which is essentially the same 
                    //   path as recovering from a lost device. If IDirect3DDevice9::Reset fails with 
                    //   D3DERR_DRIVERINTERNALERROR, the application should end immediately with the message 
                    //   that the user should contact the adapter manufacturer.
                    // 
                    // The framework attempts the path of resetting the device
                    // 
                    GetDXUTState().SetDeviceLost( true );
                }
            }
            break;
        }

        case WM_SIZE:
            // Pick up possible changes to window style due to maximize, etc.
            if( DXUTIsWindowed() && DXUTGetHWND() != NULL )
                GetDXUTState().SetWinStyle( GetWindowLong( DXUTGetHWND(), GWL_STYLE ) );

            if( SIZE_MINIMIZED == wParam )
            {
                if( GetDXUTState().GetClipCursorWhenFullScreen() && !DXUTIsWindowed() )
                    ClipCursor( NULL );
                DXUTPause( true, true ); // Pause while we're minimized
                GetDXUTState().SetMinimized( true );
                GetDXUTState().SetMaximized( false );
            }
            else if( SIZE_MAXIMIZED == wParam )
            {
                if( GetDXUTState().GetMinimized() )
                    DXUTPause( false, false ); // Unpause since we're no longer minimized
                GetDXUTState().SetMinimized( false );
                GetDXUTState().SetMaximized( true );
                DXUTHandlePossibleSizeChange();
            }
            else if( SIZE_RESTORED == wParam )
            {
                if( GetDXUTState().GetMaximized() )
                {
                    GetDXUTState().SetMaximized( false );
                    DXUTHandlePossibleSizeChange();
                }
                else if( GetDXUTState().GetMinimized() )
                {
                    DXUTPause( false, false ); // Unpause since we're no longer minimized
                    GetDXUTState().SetMinimized( false );
                    DXUTHandlePossibleSizeChange();
                }
                else
                {
                    // If we're neither maximized nor minimized, the window size 
                    // is changing by the user dragging the wifndow edges.  In this 
                    // case, we don't reset the device yet -- we wait until the 
                    // user stops dragging, and a WM_EXITSIZEMOVE message comes.
                }
            }
            break;

        case WM_GETMINMAXINFO:
            ((MINMAXINFO*)lParam)->ptMinTrackSize.x = MIN_WINDOW_SIZE_X;
            ((MINMAXINFO*)lParam)->ptMinTrackSize.y = MIN_WINDOW_SIZE_Y;
            break;

        case WM_ENTERSIZEMOVE:
            // Halt frame movement while the app is sizing or moving
            DXUTPause( true, true );
            break;

        case WM_EXITSIZEMOVE:
            DXUTPause( false, false );
            DXUTHandlePossibleSizeChange();
            break;

        case WM_SETCURSOR:
            // Turn off Windows cursor in full screen mode
            if( !DXUTIsRenderingPaused() && !DXUTIsWindowed() )
            {
                SetCursor( NULL );
                IDirect3DDevice9* pd3dDevice = DXUTGetD3DDevice();
                if( pd3dDevice && GetDXUTState().GetShowCursorWhenFullScreen() )
                    pd3dDevice->ShowCursor( true );
                return true; // prevent Windows from setting cursor to window class cursor
            }
            break;

         case WM_MOUSEMOVE:
            if( !DXUTIsRenderingPaused() && !DXUTIsWindowed() )
            {
                IDirect3DDevice9* pd3dDevice = DXUTGetD3DDevice();
                if( pd3dDevice )
                {
                    POINT ptCursor;
                    GetCursorPos( &ptCursor );
                    pd3dDevice->SetCursorPosition( ptCursor.x, ptCursor.y, 0 );
                }
            }
            break;

       case WM_ACTIVATEAPP:
            if( wParam == TRUE )
                GetDXUTState().SetActive( true );
            else 
                GetDXUTState().SetActive( false );
            break;

       case WM_ENTERMENULOOP:
            // Pause the app when menus are displayed
            DXUTPause( true, true );
            break;

        case WM_EXITMENULOOP:
            DXUTPause( false, false );
            break;

        case WM_NCHITTEST:
            // Prevent the user from selecting the menu in full screen mode
            if( !DXUTIsWindowed() )
                return HTCLIENT;
            break;

        case WM_POWERBROADCAST:
            switch( wParam )
            {
                #ifndef PBT_APMQUERYSUSPEND
                    #define PBT_APMQUERYSUSPEND 0x0000
                #endif
                case PBT_APMQUERYSUSPEND:
                    // At this point, the app should save any data for open
                    // network connections, files, etc., and prepare to go into
                    // a suspended mode.  The app can use the MsgProc callback
                    // to handle this if desired.
                    return true;

                #ifndef PBT_APMRESUMESUSPEND
                    #define PBT_APMRESUMESUSPEND 0x0007
                #endif
                case PBT_APMRESUMESUSPEND:
                    // At this point, the app should recover any data, network
                    // connections, files, etc., and resume running from when
                    // the app was suspended. The app can use the MsgProc callback
                    // to handle this if desired.
                    return true;
            }
            break;

        case WM_SYSCOMMAND:
            // Prevent moving/sizing and power loss in full screen mode
            switch( wParam )
            {
                case SC_MOVE:
                case SC_SIZE:
                case SC_MAXIMIZE:
                case SC_KEYMENU:
                case SC_MONITORPOWER:
                    if( !DXUTIsWindowed() )
                        return 1;
                    break;
            }
            break;

        case WM_CLOSE:
        {
            HMENU hMenu;
            hMenu = GetMenu(hWnd);
            if( hMenu != NULL )
                DestroyMenu( hMenu );
            DestroyWindow( hWnd );
            UnregisterClass( L"rtg3DWndClass", NULL );
            GetDXUTState().SetHWNDFocus( NULL );
            GetDXUTState().SetHWNDDeviceFullScreen( NULL );
            GetDXUTState().SetHWNDDeviceWindowed( NULL );
            return 0;
        }

        case WM_DESTROY:
            PostQuitMessage(0);
            break;

        default:
            // At this point the message is still not handled.  We let the
            // CDXUTIMEEditBox's static message proc handle the msg.
            // This is because some IME messages must be handled to ensure
            // proper functionalities and the static msg proc ensures that
            // this happens even if no control has the input focus.
            if( CDXUTIMEEditBox::StaticMsgProc( uMsg, wParam, lParam ) )
                return 0;
    }

    return DefWindowProc( hWnd, uMsg, wParam, lParam );
}
*/

} // namespace rt_graph

/*----------------------------------------------------------------------------
-   The End.
----------------------------------------------------------------------------*/
