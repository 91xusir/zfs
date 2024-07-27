//-----------------------------------------------------------------------------
// File: D3DApp.cpp
//
// Desc: Application class for the Direct3D samples framework library.
//
// Copyright (c) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------
#define STRICT
#include <windows.h>
#include <windowsx.h>
#include <mmsystem.h>
#include <stdio.h>
#include <tchar.h>
#include <D3D9.h>
#include "graph\DXUtil.h"
#include "graph\D3DUtil.h"
#include "graph\D3DEnumeration.h"
#include "graph\D3DSettings.h"
#include "graph\D3DApp.h"
#include "graph\D3DRes.h"

#include "core/rt2_core.h"
#include "rtg_graph_inter.h"
#include "rtg_console.h"

namespace rt_graph_dx9 {
//-----------------------------------------------------------------------------
// Global access to the app (needed for the global WndProc())
//-----------------------------------------------------------------------------
static CD3DApplication* g_pD3DApp = NULL;

//-----------------------------------------------------------------------------
// Name: CD3DApplication()
// Desc: Constructor
//-----------------------------------------------------------------------------
CD3DApplication::CD3DApplication() {
    g_pD3DApp = this;
    m_pAppFrame = NULL;

    m_pD3D = NULL;
    m_pd3dDevice = NULL;
    m_hWnd = NULL;
    m_hWndFocus = NULL;
    m_hMenu = NULL;
    m_bWindowed = true;
    m_bActive = false;
    m_bReady = true;
    m_bHasFocus = true;  // default=false, janhail change
    m_bDeviceLost = false;
    m_bMinimized = false;
    m_bMaximized = false;
    m_bIgnoreSizeChange = false;
    m_bDeviceObjectsInited = false;
    m_bDeviceObjectsRestored = false;
    m_dwCreateFlags = 0;

    m_bFrameMoving = true;
    m_bSingleStep = false;
    m_fTime = 0.0f;
    m_fElapsedTime = 0.0f;
    m_dwElapsedMS = 0;
    m_fFPS = 0.0f;
    m_strDeviceStats[0] = _T('\0');
    m_strFrameStats[0] = _T('\0');

    m_strWindowTitle = _T("D3D9 Application");
    m_dwCreationWidth = 400;
    m_dwCreationHeight = 300;
    m_bShowCursorWhenFullscreen = false;
    m_bStartFullscreen = false;
    m_isAdapter16 = false;
    m_deviceThread = 0;
    m_uRenderFrame = 0;
    m_bvsync = false;

    Pause(true);  // Pause until we're ready to render

    // When m_bClipCursorWhenFullscreen is true, the cursor is limited to
    // the device window when the app goes fullscreen.  This prevents users
    // from accidentally clicking outside the app window on a multimon system.
    // This flag is turned off by default for debug builds, since it makes
    // multimon debugging difficult.
#if defined(_DEBUG) || defined(DEBUG)
    m_bClipCursorWhenFullscreen = false;
#else
    m_bClipCursorWhenFullscreen = true;
#endif
}

//-----------------------------------------------------------------------------
// Name: WndProc()
// Desc: Static msg handler which passes messages to the application class.
//-----------------------------------------------------------------------------
LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    return g_pD3DApp->MsgProc(hWnd, uMsg, wParam, lParam);
}

//-----------------------------------------------------------------------------
// Name: ConfirmDeviceHelper()
// Desc: Static function used by D3DEnumeration
//-----------------------------------------------------------------------------
bool CD3DApplication::ConfirmDeviceHelper(D3DCAPS9*            pCaps,
                                          VertexProcessingType vertexProcessingType,
                                          D3DFORMAT adapterFormat, D3DFORMAT backBufferFormat) {
    DWORD dwBehavior;

    if (vertexProcessingType == SOFTWARE_VP)
        dwBehavior = D3DCREATE_SOFTWARE_VERTEXPROCESSING;
    else if (vertexProcessingType == MIXED_VP)
        dwBehavior = D3DCREATE_MIXED_VERTEXPROCESSING;
    else if (vertexProcessingType == HARDWARE_VP)
        dwBehavior = D3DCREATE_HARDWARE_VERTEXPROCESSING;
    else if (vertexProcessingType == PURE_HARDWARE_VP)
        dwBehavior = D3DCREATE_HARDWARE_VERTEXPROCESSING | D3DCREATE_PUREDEVICE;
    else
        dwBehavior = 0;  // TODO: throw exception

    return SUCCEEDED(g_pD3DApp->ConfirmDevice(pCaps, dwBehavior, adapterFormat, backBufferFormat));
}

//-----------------------------------------------------------------------------
// Name: Create()
// Desc:
//-----------------------------------------------------------------------------
HRESULT CD3DApplication::Create(HINSTANCE hInstance) {
    HRESULT hr;

    // 创建 Direct3D 设备
    m_pD3D = ::Direct3DCreate9(D3D_SDK_VERSION);
    if (m_pD3D == NULL)  // 如果创建失败，显示错误消息并退出应用程序
        return DisplayErrorMsg(D3DAPPERR_NODIRECT3D, MSGERR_APPMUSTEXIT);

    // 设置 D3D 枚举并指定回调函数
    m_d3dEnumeration.SetD3D(m_pD3D);
    m_d3dEnumeration.ConfirmDeviceCallback = ConfirmDeviceHelper;
    // 执行设备枚举
    if (FAILED(
            hr = m_d3dEnumeration.Enumerate())) {  // 如果枚举失败，释放 Direct3D 对象并显示错误消息
        SAFE_RELEASE(m_pD3D);
        return DisplayErrorMsg(hr, MSGERR_APPMUSTEXIT);
    }
    //tim.yang  在显卡支持的情况下，如果显卡模式是16位的，弹出警告对话框
    D3DDISPLAYMODE primaryDesktopDisplayMode;
    m_pD3D->GetAdapterDisplayMode(0, &primaryDesktopDisplayMode);
    if (primaryDesktopDisplayMode.Format == D3DFMT_R5G6B5 ||
        primaryDesktopDisplayMode.Format == D3DFMT_A1R5G5B5 ||
        primaryDesktopDisplayMode.Format == D3DFMT_X1R5G5B5) {
        m_isAdapter16 = true;
    } else
        m_isAdapter16 = false;
    if (m_isAdapter16) {
        MessageBox(
            NULL,
            "客户端检测到您现在的桌面颜色配置是16位色的，这样进入游戏可能会造成客户端的异常\n"
            "\n"
            " 如果客户端发生异常， 请手动设置为32位色\n",
            "错误", MB_OK | MB_ICONERROR);
    }
    //除非已经指定了一个替代的窗口句柄（hWnd），否则创建一个窗口来渲染到其中
    if (m_hWnd == NULL) {
        // Register the windows class
        //WNDCLASSEX wndClass = { 0, WndProc, 0, 0, hInstance,
        //                      LoadIcon( hInstance, MAKEINTRESOURCE(IDI_MAIN_ICON) ),
        //                      NULL,/*LoadCursor( NULL, IDC_ARROW )*/
        //                      (HBRUSH)GetStockObject(WHITE_BRUSH),
        //                      NULL, _T("D3D Window"),LoadIcon( hInstance, MAKEINTRESOURCE(IDI_MAIN_ICON))};
        WNDCLASSEX wndClass = {
            sizeof(WNDCLASSEX),  // cbSize: 结构体的大小，用于确保版本兼容性
            CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS,  // style: 窗口类的样式标志
            // CS_HREDRAW: 窗口宽度变化时重绘整个窗口
            // CS_VREDRAW: 窗口高度变化时重绘整个窗口
            // CS_DBLCLKS: 允许双击消息
            (WNDPROC)WndProc,  // lpfnWndProc: 窗口过程函数的地址，处理窗口消息
            0,                 // cbClsExtra: 窗口类的额外字节数，通常设为 0
            0,                 // cbWndExtra: 窗口实例的额外字节数，通常设为 0
            hInstance,         // hInstance: 窗口类的应用程序实例句柄
            LoadIcon(hInstance,
                     (LPCTSTR)IDI_MAIN_ICON),  // hIcon: 窗口的图标句柄（大图标），从资源中加载
            NULL,  // hCursor: 窗口的光标句柄，默认为 NULL 使用系统光标
            (HBRUSH)(COLOR_WINDOW + 1),  // hbrBackground: 窗口的背景画刷，使用系统窗口颜色
            NULL,                        // lpszMenuName: 窗口的菜单名称，默认为 NULL
            _T("D3D Window"),            // lpszClassName: 窗口类名，用于标识窗口类
            LoadIcon(hInstance,
                     (LPCTSTR)IDI_SMALL_ICON)  // hIconSm: 窗口的小图标句柄，通常用于任务栏
        };
        // 注册窗口类
        RegisterClassEx(&wndClass);

        // 设置窗口样式 //alter by tim.yang  20080725  login对话框会缩小
        m_dwWindowStyle = WS_OVERLAPPED | WS_CAPTION | WS_MINIMIZEBOX |
                          WS_SYSMENU /*| WS_THICKFRAME */ | WS_VISIBLE;

        // 设置窗口大小
        /*	RECT rc;
			SetRect(&rc, 0, 0, m_dwCreationWidth, m_dwCreationHeight);
			AdjustWindowRect(&rc, m_dwWindowStyle, false);*/

        // lyymark 窗口居中
        RECT screenRect;
        GetWindowRect(GetDesktopWindow(), &screenRect);
        const int screenWidth = screenRect.right - screenRect.left;
        const int screenHeight = screenRect.bottom - screenRect.top;
        RECT      windowRect;
        SetRect(&windowRect, 0, 0, m_dwCreationWidth, m_dwCreationHeight);
        AdjustWindowRect(&windowRect, m_dwWindowStyle, false);
        const int     windowWidth = windowRect.right - windowRect.left;
        const int     windowHeight = windowRect.bottom - windowRect.top;
        const int     x = (screenWidth - windowWidth) / 2;
        constexpr int y = 1;

        // lyymark 1.Device.CreateWindow 创建窗口
        m_hWnd = CreateWindow(
            _T("D3D Window"),  // lpClassName: 窗口类名，必须与 RegisterClassEx 中的 lpszClassName 匹配
            m_strWindowTitle,  // lpWindowName: 窗口标题字符串，显示在窗口的标题栏上
            m_dwWindowStyle,  // dwStyle: 窗口样式，定义窗口的外观和行为
            x,  // x: 窗口左上角的 x 坐标，CW_USEDEFAULT 表示使用系统默认值
            y,  // y: 窗口左上角的 y 坐标，CW_USEDEFAULT 表示使用系统默认值
            windowWidth,   // nWidth: 窗口的宽度，计算自 RECT 结构体中设置的大小
            windowHeight,  // nHeight: 窗口的高度，计算自 RECT 结构体中设置的大小
            NULL,  // hWndParent: 父窗口的句柄，如果窗口是顶级窗口则为 NULL
            NULL,  // hMenu: 窗口的菜单句柄，默认为 NULL，或使用 LoadMenu 函数加载菜单
            hInstance,  // hInstance: 应用程序的实例句柄
            0L          // lpParam: 额外的创建参数，通常设为 0
        );
    }
    typedef HRESULT(WINAPI * DwmSetWindowAttributeProc)(HWND, DWORD, LPCVOID, DWORD);
    static DwmSetWindowAttributeProc DwmSetWindowAttribute =
        (DwmSetWindowAttributeProc)GetProcAddress(GetModuleHandle(TEXT("dwmapi.dll")),
                                                  "DwmSetWindowAttribute");

    if (DwmSetWindowAttribute) {
        constexpr DWORD dwAttribute = 20;  // 设置暗色模式属性
        constexpr BOOL  bValue = TRUE;     // 启用暗色模式
        DwmSetWindowAttribute(m_hWnd, dwAttribute, &bValue, sizeof(bValue));  // 设置窗口属性
    }

    // 如果没有设置焦点窗口，则将主窗口设置为焦点窗口
    if (m_hWndFocus == NULL)
        m_hWndFocus = m_hWnd;

    // 获取当前窗口的样式、边界和客户区大小
    m_dwWindowStyle = GetWindowLong(m_hWnd, GWL_STYLE);
    GetWindowRect(m_hWnd, &m_rcWindowBounds);
    GetClientRect(m_hWnd, &m_rcWindowClient);
    // 选择初始的 D3D 设置
    if (FAILED(
            hr =
                ChooseInitialD3DSettings())) {  // 如果选择设置失败，释放 Direct3D 对象并显示错误消息
        SAFE_RELEASE(m_pD3D);
        return DisplayErrorMsg(hr, MSGERR_APPMUSTEXIT);
    }

    // 启动计时器
    DXUtil_Timer(TIMER_START);

    // 初始化场景
    if (FAILED(hr = OneTimeSceneInit())) {
        SAFE_RELEASE(m_pD3D);
        return DisplayErrorMsg(hr, MSGERR_APPMUSTEXIT);
    }

    // 初始化 3D 环境
    if (FAILED(hr = Initialize3DEnvironment())) {
        // 如果初始化失败，释放 Direct3D 对象并显示错误消息
        return DisplayErrorMsg(hr, MSGERR_APPMUSTEXIT);
    }

    // 恢复游戏状态
    Pause(false);

    return S_OK;
}

//-----------------------------------------------------------------------------
// Name: FindBestWindowedMode()
// Desc: Sets up m_d3dSettings with best available windowed mode, subject to
//       the bRequireHAL and bRequireREF constraints.  Returns false if no such
//       mode can be found.
//-----------------------------------------------------------------------------
bool CD3DApplication::FindBestWindowedMode(bool bRequireHAL, bool bRequireREF) {
    // Get display mode of primary adapter (which is assumed to be where the window
    // will appear)
    D3DDISPLAYMODE primaryDesktopDisplayMode;
    m_pD3D->GetAdapterDisplayMode(0, &primaryDesktopDisplayMode);

    D3DAdapterInfo* pBestAdapterInfo = NULL;
    D3DDeviceInfo*  pBestDeviceInfo = NULL;
    D3DDeviceCombo* pBestDeviceCombo = NULL;

    for (UINT iai = 0; iai < m_d3dEnumeration.m_pAdapterInfoList->Count(); iai++) {
        D3DAdapterInfo* pAdapterInfo =
            (D3DAdapterInfo*)m_d3dEnumeration.m_pAdapterInfoList->GetPtr(iai);
        for (UINT idi = 0; idi < pAdapterInfo->pDeviceInfoList->Count(); idi++) {
            D3DDeviceInfo* pDeviceInfo = (D3DDeviceInfo*)pAdapterInfo->pDeviceInfoList->GetPtr(idi);
            if (bRequireHAL && pDeviceInfo->DevType != D3DDEVTYPE_HAL)
                continue;
            if (bRequireREF && pDeviceInfo->DevType != D3DDEVTYPE_REF)
                continue;
            for (UINT idc = 0; idc < pDeviceInfo->pDeviceComboList->Count(); idc++) {
                D3DDeviceCombo* pDeviceCombo =
                    (D3DDeviceCombo*)pDeviceInfo->pDeviceComboList->GetPtr(idc);
                bool bAdapterMatchesBB =
                    (pDeviceCombo->BackBufferFormat == pDeviceCombo->AdapterFormat);
                if (!pDeviceCombo->IsWindowed)
                    continue;
                if (pDeviceCombo->AdapterFormat != primaryDesktopDisplayMode.Format)
                    continue;
                // If we haven't found a compatible DeviceCombo yet, or if this set
                // is better (because it's a HAL, and/or because formats match better),
                // save it
                if (pBestDeviceCombo == NULL ||
                    pBestDeviceCombo->DevType != D3DDEVTYPE_HAL &&
                        pDeviceCombo->DevType == D3DDEVTYPE_HAL ||
                    pDeviceCombo->DevType == D3DDEVTYPE_HAL && bAdapterMatchesBB) {
                    pBestAdapterInfo = pAdapterInfo;
                    pBestDeviceInfo = pDeviceInfo;
                    pBestDeviceCombo = pDeviceCombo;
                    if (pDeviceCombo->DevType == D3DDEVTYPE_HAL && bAdapterMatchesBB) {
                        // This windowed device combo looks great -- take it
                        goto EndWindowedDeviceComboSearch;
                    }
                    // Otherwise keep looking for a better windowed device combo
                }
            }
        }
    }
EndWindowedDeviceComboSearch:
    if (pBestDeviceCombo == NULL)
        return false;

    /*int  iSamples;
			iSamples = isWindowed?m_eMaxMultiSampleTypeFullscreen:m_eMaxMultiSampleTypeWindowed;
			for(; iSamples>=D3DMULTISAMPLE_2_SAMPLES;iSamples--)
			{
				D3DFORMAT fmt;
				fmt = pBestAdapterInfo->d3ddmDesktop.Format;

				if(SUCCEEDED(m_pD3D->CheckDeviceMultiSampleType(m_dwAdapter,
					pDeviceInfo->DeviceType, fmt, true,(D3DMULTISAMPLE_TYPE)iSamples)))
				{
					pDeviceInfo->MultiSampleTypeWindowed = (D3DMULTISAMPLE_TYPE)iSamples;
					RtCoreLog().Info("D3D: [%s] multisampling support,use %d samples.\n", (isWindowed?"Window":"Fullscreen"), iSamples);
					break;
				}
			}*/

    m_d3dSettings.pWindowed_AdapterInfo = pBestAdapterInfo;
    m_d3dSettings.pWindowed_DeviceInfo = pBestDeviceInfo;
    m_d3dSettings.pWindowed_DeviceCombo = pBestDeviceCombo;
    m_d3dSettings.IsWindowed = true;
    m_d3dSettings.Windowed_DisplayMode = primaryDesktopDisplayMode;
    m_d3dSettings.Windowed_Width = m_rcWindowClient.right - m_rcWindowClient.left;
    m_d3dSettings.Windowed_Height = m_rcWindowClient.bottom - m_rcWindowClient.top;
    if (m_d3dEnumeration.AppUsesDepthBuffer)
        m_d3dSettings.Windowed_DepthStencilBufferFormat =
            *(D3DFORMAT*)pBestDeviceCombo->pDepthStencilFormatList->GetPtr(0);
    m_d3dSettings.Windowed_MultisampleType =
        *(D3DMULTISAMPLE_TYPE*)pBestDeviceCombo->pMultiSampleTypeList->GetPtr(0);
    m_d3dSettings.Windowed_MultisampleQuality =
        0; /**(D3DMULTISAMPLE_QUAL*) pBestDeviceCombo->pMultiSampleQualityList->GetPtr(0);*/
    m_d3dSettings.Windowed_VertexProcessingType =
        *(VertexProcessingType*)pBestDeviceCombo->pVertexProcessingTypeList->GetPtr(0);
    m_d3dSettings.Windowed_PresentInterval =
        *(UINT*)pBestDeviceCombo->pPresentIntervalList->GetPtr(0);
    return true;
}

//-----------------------------------------------------------------------------
// Name: FindBestFullscreenMode()
// Desc: Sets up m_d3dSettings with best available fullscreen mode, subject to
//       the bRequireHAL and bRequireREF constraints.  Returns false if no such
//       mode can be found.
//-----------------------------------------------------------------------------
bool CD3DApplication::FindBestFullscreenMode(bool bRequireHAL, bool bRequireREF) {
    // For fullscreen, default to first HAL DeviceCombo that supports the current desktop
    // display mode, or any display mode if HAL is not compatible with the desktop mode, or
    // non-HAL if no HAL is available
    D3DDISPLAYMODE adapterDesktopDisplayMode;
    D3DDISPLAYMODE bestAdapterDesktopDisplayMode;
    D3DDISPLAYMODE bestDisplayMode;
    bestAdapterDesktopDisplayMode.Width = 0;
    bestAdapterDesktopDisplayMode.Height = 0;
    bestAdapterDesktopDisplayMode.Format = D3DFMT_UNKNOWN;
    bestAdapterDesktopDisplayMode.RefreshRate = 0;

    D3DAdapterInfo* pBestAdapterInfo = NULL;
    D3DDeviceInfo*  pBestDeviceInfo = NULL;
    D3DDeviceCombo* pBestDeviceCombo = NULL;

    for (UINT iai = 0; iai < m_d3dEnumeration.m_pAdapterInfoList->Count(); iai++) {
        D3DAdapterInfo* pAdapterInfo =
            (D3DAdapterInfo*)m_d3dEnumeration.m_pAdapterInfoList->GetPtr(iai);
        m_pD3D->GetAdapterDisplayMode(pAdapterInfo->AdapterOrdinal, &adapterDesktopDisplayMode);
        for (UINT idi = 0; idi < pAdapterInfo->pDeviceInfoList->Count(); idi++) {
            D3DDeviceInfo* pDeviceInfo = (D3DDeviceInfo*)pAdapterInfo->pDeviceInfoList->GetPtr(idi);
            if (bRequireHAL && pDeviceInfo->DevType != D3DDEVTYPE_HAL)
                continue;
            if (bRequireREF && pDeviceInfo->DevType != D3DDEVTYPE_REF)
                continue;
            for (UINT idc = 0; idc < pDeviceInfo->pDeviceComboList->Count(); idc++) {
                D3DDeviceCombo* pDeviceCombo =
                    (D3DDeviceCombo*)pDeviceInfo->pDeviceComboList->GetPtr(idc);
                bool bAdapterMatchesBB =
                    (pDeviceCombo->BackBufferFormat == pDeviceCombo->AdapterFormat);
                bool bAdapterMatchesDesktop =
                    (pDeviceCombo->AdapterFormat == adapterDesktopDisplayMode.Format);
                if (pDeviceCombo->IsWindowed)
                    continue;
                // If we haven't found a compatible set yet, or if this set
                // is better (because it's a HAL, and/or because formats match better),
                // save it
                if (pBestDeviceCombo == NULL ||
                    pBestDeviceCombo->DevType != D3DDEVTYPE_HAL &&
                        pDeviceInfo->DevType == D3DDEVTYPE_HAL ||
                    pDeviceCombo->DevType == D3DDEVTYPE_HAL &&
                        pBestDeviceCombo->AdapterFormat != adapterDesktopDisplayMode.Format &&
                        bAdapterMatchesDesktop ||
                    pDeviceCombo->DevType == D3DDEVTYPE_HAL && bAdapterMatchesDesktop &&
                        bAdapterMatchesBB) {
                    bestAdapterDesktopDisplayMode = adapterDesktopDisplayMode;
                    pBestAdapterInfo = pAdapterInfo;
                    pBestDeviceInfo = pDeviceInfo;
                    pBestDeviceCombo = pDeviceCombo;
                    if (pDeviceInfo->DevType == D3DDEVTYPE_HAL && bAdapterMatchesDesktop &&
                        bAdapterMatchesBB) {
                        // This fullscreen device combo looks great -- take it
                        goto EndFullscreenDeviceComboSearch;
                    }
                    // Otherwise keep looking for a better fullscreen device combo
                }
            }
        }
    }
EndFullscreenDeviceComboSearch:
    if (pBestDeviceCombo == NULL)
        return false;

    // Need to find a display mode on the best adapter that uses pBestDeviceCombo->AdapterFormat
    // and is as close to bestAdapterDesktopDisplayMode's res as possible
    bestDisplayMode.Width = 0;
    bestDisplayMode.Height = 0;
    bestDisplayMode.Format = D3DFMT_UNKNOWN;
    bestDisplayMode.RefreshRate = 0;
    for (UINT idm = 0; idm < pBestAdapterInfo->pDisplayModeList->Count(); idm++) {
        D3DDISPLAYMODE* pdm = (D3DDISPLAYMODE*)pBestAdapterInfo->pDisplayModeList->GetPtr(idm);
        if (pdm->Format != pBestDeviceCombo->AdapterFormat)
            continue;
        if (pdm->Width == bestAdapterDesktopDisplayMode.Width &&
            pdm->Height == bestAdapterDesktopDisplayMode.Height &&
            pdm->RefreshRate == bestAdapterDesktopDisplayMode.RefreshRate) {
            // found a perfect match, so stop
            bestDisplayMode = *pdm;
            break;
        } else if (pdm->Width == bestAdapterDesktopDisplayMode.Width &&
                   pdm->Height == bestAdapterDesktopDisplayMode.Height &&
                   pdm->RefreshRate > bestDisplayMode.RefreshRate) {
            // refresh rate doesn't match, but width/height match, so keep this
            // and keep looking
            bestDisplayMode = *pdm;
        } else if (pdm->Width == bestAdapterDesktopDisplayMode.Width) {
            // width matches, so keep this and keep looking
            bestDisplayMode = *pdm;
        } else if (bestDisplayMode.Width == 0) {
            // we don't have anything better yet, so keep this and keep looking
            bestDisplayMode = *pdm;
        }
    }

    m_d3dSettings.pFullscreen_AdapterInfo = pBestAdapterInfo;
    m_d3dSettings.pFullscreen_DeviceInfo = pBestDeviceInfo;
    m_d3dSettings.pFullscreen_DeviceCombo = pBestDeviceCombo;
    m_d3dSettings.IsWindowed = false;
    m_d3dSettings.Fullscreen_DisplayMode = bestDisplayMode;
    if (m_d3dEnumeration.AppUsesDepthBuffer)
        m_d3dSettings.Fullscreen_DepthStencilBufferFormat =
            *(D3DFORMAT*)pBestDeviceCombo->pDepthStencilFormatList->GetPtr(0);
    m_d3dSettings.Fullscreen_MultisampleType =
        *(D3DMULTISAMPLE_TYPE*)pBestDeviceCombo->pMultiSampleTypeList->GetPtr(0);
    m_d3dSettings.Fullscreen_MultisampleQuality = 0;
    m_d3dSettings.Fullscreen_VertexProcessingType =
        *(VertexProcessingType*)pBestDeviceCombo->pVertexProcessingTypeList->GetPtr(0);
    m_d3dSettings.Fullscreen_PresentInterval = D3DPRESENT_INTERVAL_DEFAULT;
    return true;
}

//-----------------------------------------------------------------------------
// Name: ChooseInitialD3DSettings()
// Desc:
//-----------------------------------------------------------------------------
HRESULT CD3DApplication::ChooseInitialD3DSettings() {
    bool bFoundFullscreen = FindBestFullscreenMode(false, false);
    bool bFoundWindowed = FindBestWindowedMode(false, false);

    if (m_bStartFullscreen && bFoundFullscreen)
        m_d3dSettings.IsWindowed = false;
    if (!bFoundWindowed && bFoundFullscreen)
        m_d3dSettings.IsWindowed = false;

    if (!bFoundFullscreen && !bFoundWindowed)
        return D3DAPPERR_NOCOMPATIBLEDEVICES;

    return S_OK;
}

#ifndef WM_MOUSEWHEEL
#define WM_MOUSEWHEEL 0x020A
#endif
#ifndef WHEEL_DELTA
#define WHEEL_DELTA 120
#endif

//-----------------------------------------------------------------------------
// Name: MsgProc()
// Desc: Message handling function.
//-----------------------------------------------------------------------------
LRESULT CD3DApplication::MsgProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    HRESULT hr;
    bool    bAltDown;
    int     xPos, yPos;

    switch (uMsg) {
        case WM_SETFOCUS:
            m_pAppFrame->OnSetFocus();
            break;
        case WM_KILLFOCUS:
            m_pAppFrame->OnKillFocus();
            break;
            // 键盘事件的处理 -----------------------------------------------------------------------
        case WM_CHAR:
            bAltDown = ((lParam & (1 << 29)) != 0);
            if (m_pAppFrame->GetEnableConsole() && m_pAppFrame->GetShowConsole()) {
                m_pAppFrame->GetConsole()->OnKeyChar((int)wParam, bAltDown);
            } else {
                m_pAppFrame->OnKeyChar((int)wParam, bAltDown);
            }
            break;
        case WM_KEYDOWN:
        case WM_SYSKEYDOWN:
            bAltDown = ((lParam & (1 << 29)) != 0);
            m_pAppFrame->m_bKeys[(BYTE)(wParam & 0xFF)] = true;
            if (m_pAppFrame->GetEnableConsole()) {
                if (wParam == VK_OEM_3 && m_pAppFrame->m_bKeys[VK_CONTROL]) {
                    m_pAppFrame->ShowConsole(!m_pAppFrame->GetShowConsole());
                } else {
                    // 如果允许Console，那么只有在不显示Console的情况下才允许发送KeyDown消息出去
                    if (!m_pAppFrame->GetShowConsole()) {
                        m_pAppFrame->OnKeyDown((int)wParam, bAltDown);
                    }
                }
            } else {
                m_pAppFrame->OnKeyDown((int)wParam, bAltDown);
            }
            break;
        case WM_KEYUP:
        case WM_SYSKEYUP:
            bAltDown = ((lParam & (1 << 29)) != 0);
            m_pAppFrame->m_bKeys[(BYTE)(wParam & 0xFF)] = false;
            m_pAppFrame->OnKeyUp((int)wParam, bAltDown);  // ( int iChar, bool bAltDown )
            break;

            // 鼠标事件的处理 -----------------------------------------------------------------------
        case WM_LBUTTONDOWN:
        case WM_LBUTTONUP:
        case WM_LBUTTONDBLCLK:
        case WM_MBUTTONDOWN:
        case WM_MBUTTONUP:
        case WM_MBUTTONDBLCLK:
        case WM_RBUTTONDOWN:
        case WM_RBUTTONUP:
        case WM_RBUTTONDBLCLK:
        case WM_MOUSEMOVE: {
            xPos = (short)LOWORD(lParam);
            yPos = (short)HIWORD(lParam);
            int nMouseButtonState = LOWORD(wParam);
            m_pAppFrame->m_bMouseButtons[0] = ((nMouseButtonState & MK_LBUTTON) != 0);
            m_pAppFrame->m_bMouseButtons[1] = ((nMouseButtonState & MK_MBUTTON) != 0);
            m_pAppFrame->m_bMouseButtons[2] = ((nMouseButtonState & MK_RBUTTON) != 0);

            switch (uMsg) {
                case WM_LBUTTONDOWN:
                    m_pAppFrame->OnMouseLDown(xPos, yPos);
                    break;
                case WM_LBUTTONUP:
                    m_pAppFrame->OnMouseLUp(xPos, yPos);
                    if (m_pAppFrame->m_bCaptureMouse && GetCapture())
                        ReleaseCapture();
                    break;
                case WM_LBUTTONDBLCLK:
                    m_pAppFrame->OnMouseLDClick(xPos, yPos);
                    break;
                case WM_MBUTTONDOWN:
                    m_pAppFrame->OnMouseMDown(xPos, yPos);
                    break;
                case WM_MBUTTONUP:
                    m_pAppFrame->OnMouseMUp(xPos, yPos);
                    if (m_pAppFrame->m_bCaptureMouse && GetCapture())
                        ReleaseCapture();
                    break;
                case WM_MBUTTONDBLCLK:
                    m_pAppFrame->OnMouseMDClick(xPos, yPos);
                    break;
                case WM_RBUTTONDOWN:
                    m_pAppFrame->OnMouseRDown(xPos, yPos);
                    break;
                case WM_RBUTTONUP:
                    m_pAppFrame->OnMouseRUp(xPos, yPos);
                    if (m_pAppFrame->m_bCaptureMouse && GetCapture())
                        ReleaseCapture();
                    break;
                case WM_RBUTTONDBLCLK:
                    m_pAppFrame->OnMouseRDClick(xPos, yPos);
                    break;
                case WM_MOUSEMOVE:
                    if (m_pAppFrame->m_bNotifyOnMouseMove) {
                        if (m_pAppFrame->m_bMouseButtons[0]) {
                            m_pAppFrame->OnMouseLDrag(xPos, yPos,
                                                      xPos - m_pAppFrame->m_iLastMousePos[0],
                                                      yPos - m_pAppFrame->m_iLastMousePos[1]);
                            if (m_pAppFrame->m_bCaptureMouse)
                                SetCapture(hWnd);
                        } else if (m_pAppFrame->m_bMouseButtons[1]) {
                            m_pAppFrame->OnMouseMDrag(xPos, yPos,
                                                      xPos - m_pAppFrame->m_iLastMousePos[0],
                                                      yPos - m_pAppFrame->m_iLastMousePos[1]);
                            if (m_pAppFrame->m_bCaptureMouse)
                                SetCapture(hWnd);
                        } else if (m_pAppFrame->m_bMouseButtons[2]) {
                            m_pAppFrame->OnMouseRDrag(xPos, yPos,
                                                      xPos - m_pAppFrame->m_iLastMousePos[0],
                                                      yPos - m_pAppFrame->m_iLastMousePos[1]);
                            if (m_pAppFrame->m_bCaptureMouse)
                                SetCapture(hWnd);
                        } else {
                            m_pAppFrame->OnMouseMove(xPos, yPos,
                                                     xPos - m_pAppFrame->m_iLastMousePos[0],
                                                     yPos - m_pAppFrame->m_iLastMousePos[1]);
                        }
                    }
                    if (m_bActive && m_bReady && m_pd3dDevice != NULL) {
                        POINT ptCursor;
                        GetCursorPos(&ptCursor);
                        if (!m_bWindowed)
                            ScreenToClient(m_hWnd, &ptCursor);
                        m_pd3dDevice->SetCursorPosition(ptCursor.x, ptCursor.y, 0L);
                    }
                    break;
            }
            m_pAppFrame->m_iLastMousePos[0] = xPos;
            m_pAppFrame->m_iLastMousePos[1] = yPos;
        } break;
        case WM_MOUSEWHEEL: {
            // WM_MOUSEWHEEL passes screen mouse coords so convert them to client coords
            xPos = (short)LOWORD(lParam);
            yPos = (short)HIWORD(lParam);
            POINT pt;
            pt.x = xPos;
            pt.y = yPos;
            ScreenToClient(hWnd, &pt);
            xPos = pt.x;
            yPos = pt.y;
            int nMouseWheelDelta = (short)HIWORD(wParam);
            m_pAppFrame->OnMouseWheel(nMouseWheelDelta / WHEEL_DELTA, xPos, yPos);
            m_pAppFrame->m_iLastMousePos[0] = xPos;
            m_pAppFrame->m_iLastMousePos[1] = yPos;
        } break;

            ////////////////////////////////////////////////////////////////////////////////////////
            /*
		case WM_SYSKEYDOWN:
		case WM_SYSKEYUP:
		case WM_SYSCHAR:
		case WM_SYSDEADCHAR:
		return 0; // Janhail add
		*/

        case WM_PAINT:
            // Handle paint messages when the app is not ready
            /*
			if( m_pd3dDevice && !m_bReady )
			{
			if( m_bWindowed )
			{
			Render();
			m_pd3dDevice->Present( NULL, NULL, NULL, NULL );
			}
			}
			*/
            ValidateRect(hWnd, NULL);
            return 0;
            break;

        case WM_SIZING: {
            int   wmId = LOWORD(wParam);
            int   wmEvent = HIWORD(wParam);
            RECT* pkRect = (RECT FAR*)lParam;
            pkRect->right = pkRect->left + m_dwCreationWidth;
            pkRect->bottom = pkRect->top + m_dwCreationHeight;
        } break;

        case WM_ACTIVATEAPP:
            m_bHasFocus = !!wParam;
            //m_bActive = (BOOL)wParam;
            break;

        case WM_GETMINMAXINFO:
            ((MINMAXINFO*)lParam)->ptMinTrackSize.x = 100;
            ((MINMAXINFO*)lParam)->ptMinTrackSize.y = 100;
            break;

        case WM_ENTERSIZEMOVE:
            // Halt frame movement while the app is sizing or moving
            // Pause( TRUE );
            break;

        case WM_SIZE:
            // 检查窗口是否被隐藏或最小化
            if (SIZE_MAXHIDE == wParam ||
                SIZE_MINIMIZED == wParam) {  // 如果当前是全屏模式且需要剪裁光标，解除光标剪裁
                if (m_bClipCursorWhenFullscreen && !m_bWindowed)
                    ClipCursor(NULL);
                // 设置应用程序为不活动状态
                m_bActive = FALSE;
                // 通知应用程序框架窗口状态变化
                m_pAppFrame->OnWindowActive(hWnd, false);
            } else {
                m_bActive = TRUE;
                m_pAppFrame->OnWindowActive(hWnd, true);
            }
            break;

        case WM_EXITSIZEMOVE:
            // Pause( FALSE );

            if (m_bActive && m_bWindowed) {
                RECT rcClientOld;
                rcClientOld = m_rcWindowClient;

                // Update window properties
                GetWindowRect(m_hWnd, &m_rcWindowBounds);
                GetClientRect(m_hWnd, &m_rcWindowClient);

                if (rcClientOld.right - rcClientOld.left !=
                        m_rcWindowClient.right - m_rcWindowClient.left ||
                    rcClientOld.bottom - rcClientOld.top !=
                        m_rcWindowClient.bottom - m_rcWindowClient.top) {
                    // A new window size will require a new backbuffer
                    // size, so the 3D structures must be changed accordingly.
                    m_bReady = FALSE;

                    m_d3dpp.BackBufferWidth = m_rcWindowClient.right - m_rcWindowClient.left;
                    m_d3dpp.BackBufferHeight = m_rcWindowClient.bottom - m_rcWindowClient.top;

                    // Resize the 3D environment
                    if (FAILED(hr = Reset3DEnvironment())) {
                        DisplayErrorMsg(D3DAPPERR_RESETFAILED, MSGERR_APPMUSTEXIT);
                        return 0;
                    }

                    m_bReady = TRUE;
                }
                m_pAppFrame->OnExitSizeMove(hWnd);
            }

            break;

        case WM_SETCURSOR:
            // Turn off Windows cursor in fullscreen mode
            if (m_bActive && m_bReady && !m_bWindowed) {
                SetCursor(NULL);
                if (m_bShowCursorWhenFullscreen)
                    m_pd3dDevice->ShowCursor(TRUE);
                return TRUE;  // prevent Windows from setting cursor to window class cursor
            }
            break;

        case WM_NCHITTEST:
            // Prevent the user from selecting the menu in fullscreen mode
            if (!m_bWindowed)
                return HTCLIENT;

            break;

        case WM_POWERBROADCAST:
            switch (wParam) {
#ifndef PBT_APMQUERYSUSPEND
#define PBT_APMQUERYSUSPEND 0x0000
#endif
                case PBT_APMQUERYSUSPEND:
                    // At this point, the app should save any data for open
                    // network connections, files, etc., and prepare to go into
                    // a suspended mode.
                    return TRUE;

#ifndef PBT_APMRESUMESUSPEND
#define PBT_APMRESUMESUSPEND 0x0007
#endif
                case PBT_APMRESUMESUSPEND:
                    // At this point, the app should recover any data, network
                    // connections, files, etc., and resume running from when
                    // the app was suspended.
                    return TRUE;
            }
            break;

        case WM_SYSCOMMAND:
            // Prevent moving/sizing and power loss in fullscreen mode
            switch (wParam) {
                case SC_MOVE:
                case SC_SIZE:
                case SC_MAXIMIZE:
                case SC_KEYMENU:
                case SC_MONITORPOWER:
                    if (FALSE == m_bWindowed)
                        return 1;
                    break;
            }
            break;

            /*
			case WM_ENTERMENULOOP:
			// Pause the app when menus are displayed
			// Pause(TRUE);
			break;

			case WM_EXITMENULOOP:
			// Pause(FALSE);
			break;

			case WM_CONTEXTMENU:
			// No context menus allowed in fullscreen mode
			if( m_bWindowed == FALSE )
			break;

			// Handle the app's context menu (via right mouse click)
			TrackPopupMenuEx( GetSubMenu( LoadMenu( 0, MAKEINTRESOURCE(IDR_POPUP) ), 0 ),
			TPM_VERTICAL, LOWORD(lParam), HIWORD(lParam), hWnd, NULL );
			break;

			case WM_COMMAND:
			switch( LOWORD(wParam) )
			{
			case IDM_TOGGLESTART:
			// Toggle frame movement
			m_bFrameMoving = !m_bFrameMoving;
			DXUtil_Timer( m_bFrameMoving ? TIMER_START : TIMER_STOP );
			break;

			case IDM_SINGLESTEP:
			// Single-step frame movement
			if( FALSE == m_bFrameMoving )
			DXUtil_Timer( TIMER_ADVANCE );
			else
			DXUtil_Timer( TIMER_STOP );
			m_bFrameMoving = FALSE;
			m_bSingleStep  = TRUE;
			break;

			case IDM_CHANGEDEVICE:
			// Prompt the user to select a new device or mode
			if( m_bActive && m_bReady )
			{
			Pause(TRUE);

			if( FAILED( hr = UserSelectNewDevice() ) )
			return 0;

			Pause(FALSE);
			}
			return 0;

			case IDM_TOGGLEFULLSCREEN:
			// Toggle the fullscreen/window mode
			if( m_bActive && m_bReady )
			{
			Pause( TRUE );

			if( FAILED( ToggleFullscreen() ) )
			{
			DisplayErrorMsg( D3DAPPERR_RESIZEFAILED, MSGERR_APPMUSTEXIT );
			return 0;
			}

			Pause( FALSE );
			}
			return 0;

			case IDM_EXIT:
			// Recieved key/menu command to exit app
			SendMessage( hWnd, WM_CLOSE, 0, 0 );
			return 0;
			}
			break;
			*/

        case WM_CLOSE:
            //Cleanup3DEnvironment();
            DestroyMenu(GetMenu(hWnd));
            DestroyWindow(hWnd);
            PostQuitMessage(0);
            return 0;
    }

    /* if (m_pAppFrame->m_bUseDefaultWindowProc)
		{
		m_pAppFrame->DefaultWindowProc(hWnd, uMsg, wParam, lParam);
		}*/
    return DefWindowProc(hWnd, uMsg, wParam, lParam);
}

//-----------------------------------------------------------------------------
// Name: HandlePossibleSizeChange()
// Desc: Reset the device if the client area size has changed.
//-----------------------------------------------------------------------------
HRESULT CD3DApplication::HandlePossibleSizeChange() {
    HRESULT hr = S_OK;
    RECT    rcClientOld;
    rcClientOld = m_rcWindowClient;

    if (m_bIgnoreSizeChange)
        return S_OK;

    // Update window properties
    GetWindowRect(m_hWnd, &m_rcWindowBounds);
    GetClientRect(m_hWnd, &m_rcWindowClient);

    /* if( rcClientOld.right - rcClientOld.left !=
			 m_rcWindowClient.right - m_rcWindowClient.left ||
			 rcClientOld.bottom - rcClientOld.top !=
			 m_rcWindowClient.bottom - m_rcWindowClient.top) */
    {
        // A new window size will require a new backbuffer
        // size, so the 3D structures must be changed accordingly.
        Pause(true);

        m_d3dpp.BackBufferWidth = m_rcWindowClient.right - m_rcWindowClient.left;
        m_d3dpp.BackBufferHeight = m_rcWindowClient.bottom - m_rcWindowClient.top;

        if (m_pd3dDevice != NULL) {
            // Reset the 3D environment
            if (FAILED(hr = Reset3DEnvironment())) {
                if (hr != D3DERR_OUTOFVIDEOMEMORY)
                    hr = D3DAPPERR_RESETFAILED;
                DisplayErrorMsg(hr, MSGERR_APPMUSTEXIT);
            }
        }
        Pause(false);
    }
    return hr;
}

//-----------------------------------------------------------------------------
// Name: Initialize3DEnvironment()
// Desc:
//-----------------------------------------------------------------------------
HRESULT CD3DApplication::Initialize3DEnvironment() {
    HRESULT hr;

    D3DAdapterInfo* pAdapterInfo = m_d3dSettings.PAdapterInfo();
    D3DDeviceInfo*  pDeviceInfo = m_d3dSettings.PDeviceInfo();

    m_bWindowed = m_d3dSettings.IsWindowed;
    AdjustWindowForChange();
    BuildPresentParamsFromSettings();

    if (pDeviceInfo->Caps.PrimitiveMiscCaps & D3DPMISCCAPS_NULLREFERENCE)
        DisplayErrorMsg(D3DAPPERR_NULLREFDEVICE, 0);

    DWORD behaviorFlags = 0;
    if (m_d3dSettings.GetVertexProcessingType() == SOFTWARE_VP)
        behaviorFlags = D3DCREATE_SOFTWARE_VERTEXPROCESSING;
    else if (m_d3dSettings.GetVertexProcessingType() == MIXED_VP)
        behaviorFlags = D3DCREATE_MIXED_VERTEXPROCESSING;
    else if (m_d3dSettings.GetVertexProcessingType() == HARDWARE_VP)
        behaviorFlags = D3DCREATE_HARDWARE_VERTEXPROCESSING;
    else if (m_d3dSettings.GetVertexProcessingType() == PURE_HARDWARE_VP)
        behaviorFlags = D3DCREATE_HARDWARE_VERTEXPROCESSING | D3DCREATE_PUREDEVICE;
    else
        behaviorFlags = 0;
    UINT       Adapter = m_d3dSettings.AdapterOrdinal();
    D3DDEVTYPE devType = pDeviceInfo->DevType;

#ifdef _PerfHUD_
    ++Adapter;
    devType = D3DDEVTYPE_REF;
#endif

    hr =
        m_pD3D->CreateDevice(Adapter, devType, m_hWndFocus, behaviorFlags, &m_d3dpp, &m_pd3dDevice);
    m_deviceThread = GetCurrentThreadId();

    if (SUCCEEDED(hr)) {
        // When moving from fullscreen to windowed mode, it is important to
        // adjust the window size after recreating the device rather than
        // beforehand to ensure that you get the window size you want.  For
        // example, when switching from 640x480 fullscreen to windowed with
        // a 1000x600 window on a 1024x768 desktop, it is impossible to set
        // the window size to 1000x600 until after the display mode has
        // changed to 1024x768, because windows cannot be larger than the
        // desktop.
        if (m_bWindowed) {
            SetWindowPos(m_hWnd, HWND_NOTOPMOST, m_rcWindowBounds.left, m_rcWindowBounds.top,
                         (m_rcWindowBounds.right - m_rcWindowBounds.left),
                         (m_rcWindowBounds.bottom - m_rcWindowBounds.top), SWP_SHOWWINDOW);
        }

        // Store device Caps
        m_pd3dDevice->GetDeviceCaps(&m_d3dCaps);
        m_dwCreateFlags = behaviorFlags;

        // Store device description
        if (pDeviceInfo->DevType == D3DDEVTYPE_REF)
            lstrcpy(m_strDeviceStats, TEXT("REF"));
        else if (pDeviceInfo->DevType == D3DDEVTYPE_HAL)
            lstrcpy(m_strDeviceStats, TEXT("HAL"));
        else if (pDeviceInfo->DevType == D3DDEVTYPE_SW)
            lstrcpy(m_strDeviceStats, TEXT("SW"));

        if (behaviorFlags & D3DCREATE_HARDWARE_VERTEXPROCESSING &&
            behaviorFlags & D3DCREATE_PUREDEVICE) {
            if (pDeviceInfo->DevType == D3DDEVTYPE_HAL)
                lstrcat(m_strDeviceStats, TEXT(" (pure hw vp)"));
            else
                lstrcat(m_strDeviceStats, TEXT(" (simulated pure hw vp)"));
        } else if (behaviorFlags & D3DCREATE_HARDWARE_VERTEXPROCESSING) {
            if (pDeviceInfo->DevType == D3DDEVTYPE_HAL)
                lstrcat(m_strDeviceStats, TEXT(" (hw vp)"));
            else
                lstrcat(m_strDeviceStats, TEXT(" (simulated hw vp)"));
        } else if (behaviorFlags & D3DCREATE_MIXED_VERTEXPROCESSING) {
            if (pDeviceInfo->DevType == D3DDEVTYPE_HAL)
                lstrcat(m_strDeviceStats, TEXT(" (mixed vp)"));
            else
                lstrcat(m_strDeviceStats, TEXT(" (simulated mixed vp)"));
        } else if (behaviorFlags & D3DCREATE_SOFTWARE_VERTEXPROCESSING) {
            lstrcat(m_strDeviceStats, TEXT(" (sw vp)"));
        }

        if (pDeviceInfo->DevType == D3DDEVTYPE_HAL) {
            // Be sure not to overflow m_strDeviceStats when appending the adapter
            // description, since it can be long.  Note that the adapter description
            // is initially CHAR and must be converted to TCHAR.
            lstrcat(m_strDeviceStats, TEXT(": "));
            const int cchDesc = sizeof(pAdapterInfo->AdapterIdentifier.Description);
            TCHAR     szDescription[cchDesc];
            DXUtil_ConvertAnsiStringToGenericCch(
                szDescription, pAdapterInfo->AdapterIdentifier.Description, cchDesc);
            int maxAppend =
                sizeof(m_strDeviceStats) / sizeof(TCHAR) - lstrlen(m_strDeviceStats) - 1;
            _tcsncat(m_strDeviceStats, szDescription, maxAppend);
        }

        // Store render target surface desc
        LPDIRECT3DSURFACE9 pBackBuffer = NULL;
        m_pd3dDevice->GetBackBuffer(0, 0, D3DBACKBUFFER_TYPE_MONO, &pBackBuffer);
        pBackBuffer->GetDesc(&m_d3dsdBackBuffer);
        pBackBuffer->Release();

        OnGetDeviceCaps();

        // Set up the fullscreen cursor
        if (m_bShowCursorWhenFullscreen && !m_bWindowed) {
            HCURSOR hCursor;
#ifdef _WIN64
            hCursor = (HCURSOR)GetClassLongPtr(m_hWnd, GCLP_HCURSOR);
#else
            hCursor = (HCURSOR)ULongToHandle(GetClassLong(m_hWnd, GCL_HCURSOR));
#endif
            D3DUtil_SetDeviceCursor(m_pd3dDevice, hCursor, true);
            m_pd3dDevice->ShowCursor(true);
        }

        // Confine cursor to fullscreen window
        if (m_bClipCursorWhenFullscreen) {
            if (!m_bWindowed) {
                RECT rcWindow;
                GetWindowRect(m_hWnd, &rcWindow);
                ClipCursor(&rcWindow);
            } else {
                ClipCursor(NULL);
            }
        }

        // Initialize the app's device-dependent objects
        hr = InitDeviceObjects();
        if (FAILED(hr)) {
            DeleteDeviceObjects();
        } else {
            m_bDeviceObjectsInited = true;
            hr = RestoreDeviceObjects();
            if (FAILED(hr)) {
                InvalidateDeviceObjects();
            } else {
                m_bDeviceObjectsRestored = true;
                return S_OK;
            }
        }

        // Cleanup before we try again
        Cleanup3DEnvironment();
    }

    // If that failed, fall back to the reference rasterizer
    if (hr != D3DAPPERR_MEDIANOTFOUND && hr != HRESULT_FROM_WIN32(ERROR_FILE_NOT_FOUND) &&
        pDeviceInfo->DevType == D3DDEVTYPE_HAL) {
        if (FindBestWindowedMode(false, true)) {
            m_bWindowed = true;
            AdjustWindowForChange();
            // Make sure main window isn't topmost, so error message is visible
            SetWindowPos(m_hWnd, HWND_NOTOPMOST, m_rcWindowBounds.left, m_rcWindowBounds.top,
                         (m_rcWindowBounds.right - m_rcWindowBounds.left),
                         (m_rcWindowBounds.bottom - m_rcWindowBounds.top), SWP_SHOWWINDOW);

            // Let the user know we are switching from HAL to the reference rasterizer
            DisplayErrorMsg(hr, MSGWARN_SWITCHEDTOREF);

            hr = Initialize3DEnvironment();
        }
    }
    return hr;
}

//-----------------------------------------------------------------------------
// Name: BuildPresentParamsFromSettings()
// Desc:
//-----------------------------------------------------------------------------
void CD3DApplication::BuildPresentParamsFromSettings() {
    m_d3dpp.Windowed = m_d3dSettings.IsWindowed;
    m_d3dpp.BackBufferCount = 1;
    m_d3dpp.MultiSampleType = m_d3dSettings.MultisampleType();
    m_d3dpp.MultiSampleQuality = m_d3dSettings.MultisampleQuality();
    m_d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
    m_d3dpp.EnableAutoDepthStencil = m_d3dEnumeration.AppUsesDepthBuffer;
    m_d3dpp.hDeviceWindow = m_hWnd;
    m_d3dpp.Flags = 0;

    if (m_d3dEnumeration.AppUsesDepthBuffer)
        m_d3dpp.AutoDepthStencilFormat = m_d3dSettings.DepthStencilBufferFormat();

    if (m_bWindowed) {
        m_d3dpp.BackBufferWidth = m_rcWindowClient.right - m_rcWindowClient.left;
        m_d3dpp.BackBufferHeight = m_rcWindowClient.bottom - m_rcWindowClient.top;
        m_d3dpp.BackBufferFormat = m_d3dSettings.PDeviceCombo()->BackBufferFormat;
        m_d3dpp.FullScreen_RefreshRateInHz = 0;
        if (m_bvsync)
            m_d3dpp.PresentationInterval = D3DPRESENT_INTERVAL_DEFAULT;
        else
            m_d3dpp.PresentationInterval = D3DPRESENT_INTERVAL_IMMEDIATE;
    } else {
        m_d3dpp.BackBufferWidth = m_d3dSettings.DisplayMode().Width;
        m_d3dpp.BackBufferHeight = m_d3dSettings.DisplayMode().Height;
        m_d3dpp.BackBufferFormat = m_d3dSettings.PDeviceCombo()->BackBufferFormat;
        m_d3dpp.FullScreen_RefreshRateInHz = m_d3dSettings.Fullscreen_DisplayMode.RefreshRate;
        m_d3dpp.PresentationInterval = m_d3dSettings.PresentInterval();
    }
}

//-----------------------------------------------------------------------------
// Name: Reset3DEnvironment()
// Desc:
//-----------------------------------------------------------------------------
HRESULT CD3DApplication::Reset3DEnvironment() {
    HRESULT hr;

    // Release all vidmem objects
    if (m_bDeviceObjectsRestored) {
        m_bDeviceObjectsRestored = false;
        InvalidateDeviceObjects();
    }
    // Reset the device
    if (FAILED(hr = m_pd3dDevice->Reset(&m_d3dpp)))
        return hr;

    // Store render target surface desc
    LPDIRECT3DSURFACE9 pBackBuffer;
    m_pd3dDevice->GetBackBuffer(0, 0, D3DBACKBUFFER_TYPE_MONO, &pBackBuffer);
    pBackBuffer->GetDesc(&m_d3dsdBackBuffer);
    pBackBuffer->Release();

    // Set up the fullscreen cursor
    if (m_bShowCursorWhenFullscreen && !m_bWindowed) {
        HCURSOR hCursor;
#ifdef _WIN64
        hCursor = (HCURSOR)GetClassLongPtr(m_hWnd, GCLP_HCURSOR);
#else
        hCursor = (HCURSOR)ULongToHandle(GetClassLong(m_hWnd, GCL_HCURSOR));
#endif
        D3DUtil_SetDeviceCursor(m_pd3dDevice, hCursor, true);
        m_pd3dDevice->ShowCursor(true);
    }

    // Confine cursor to fullscreen window
    if (m_bClipCursorWhenFullscreen) {
        if (!m_bWindowed) {
            RECT rcWindow;
            GetWindowRect(m_hWnd, &rcWindow);
            ClipCursor(&rcWindow);
        } else {
            ClipCursor(NULL);
        }
    }

    // Initialize the app's device-dependent objects
    hr = RestoreDeviceObjects();
    if (FAILED(hr)) {
        InvalidateDeviceObjects();
        return hr;
    }
    m_bDeviceObjectsRestored = true;

    // If the app is paused, trigger the rendering of the current frame
    if (false == m_bFrameMoving) {
        m_bSingleStep = true;
        DXUtil_Timer(TIMER_START);
        DXUtil_Timer(TIMER_STOP);
    }

    return S_OK;
}

//-----------------------------------------------------------------------------
// Name: ToggleFullScreen()
// Desc: Called when user toggles between fullscreen mode and windowed mode
//-----------------------------------------------------------------------------
HRESULT CD3DApplication::ToggleFullscreen() {
    HRESULT    hr;
    int        AdapterOrdinalOld = m_d3dSettings.AdapterOrdinal();
    D3DDEVTYPE DevTypeOld = m_d3dSettings.DevType();

    Pause(true);
    m_bIgnoreSizeChange = true;

    // Toggle the windowed state
    m_bWindowed = !m_bWindowed;
    m_d3dSettings.IsWindowed = m_bWindowed;

    // Prepare window for windowed/fullscreen change
    AdjustWindowForChange();

    // If AdapterOrdinal and DevType are the same, we can just do a Reset().
    // If they've changed, we need to do a complete device teardown/rebuild.
    if (m_d3dSettings.AdapterOrdinal() == AdapterOrdinalOld &&
        m_d3dSettings.DevType() == DevTypeOld) {
        // Reset the 3D device
        BuildPresentParamsFromSettings();
        // added by carl,used to test performance
        if (!m_bWindowed) {
            // m_d3dpp.FullScreen_PresentationInterval = D3DPRESENT_INTERVAL_ONE;
            m_d3dpp.PresentationInterval = D3DPRESENT_INTERVAL_IMMEDIATE;
            // m_d3dpp.SwapEffect = D3DSWAPEFFECT_FLIP;
            // m_d3dpp.SwapEffect = D3DSWAPEFFECT_COPY;
        }
        hr = Reset3DEnvironment();
    } else {
        Cleanup3DEnvironment();
        hr = Initialize3DEnvironment();
    }
    if (FAILED(hr)) {
        if (hr != D3DERR_OUTOFVIDEOMEMORY)
            hr = D3DAPPERR_RESETFAILED;
        m_bIgnoreSizeChange = false;
        if (!m_bWindowed) {
            // Restore window type to windowed mode
            m_bWindowed = !m_bWindowed;
            m_d3dSettings.IsWindowed = m_bWindowed;
            AdjustWindowForChange();
            SetWindowPos(m_hWnd, HWND_NOTOPMOST, m_rcWindowBounds.left, m_rcWindowBounds.top,
                         (m_rcWindowBounds.right - m_rcWindowBounds.left),
                         (m_rcWindowBounds.bottom - m_rcWindowBounds.top), SWP_SHOWWINDOW);
        }
        return DisplayErrorMsg(hr, MSGERR_APPMUSTEXIT);
    }

    m_bIgnoreSizeChange = false;

    // When moving from fullscreen to windowed mode, it is important to
    // adjust the window size after resetting the device rather than
    // beforehand to ensure that you get the window size you want.  For
    // example, when switching from 640x480 fullscreen to windowed with
    // a 1000x600 window on a 1024x768 desktop, it is impossible to set
    // the window size to 1000x600 until after the display mode has
    // changed to 1024x768, because windows cannot be larger than the
    // desktop.
    if (m_bWindowed) {
        SetWindowPos(m_hWnd, HWND_NOTOPMOST, m_rcWindowBounds.left, m_rcWindowBounds.top,
                     (m_rcWindowBounds.right - m_rcWindowBounds.left),
                     (m_rcWindowBounds.bottom - m_rcWindowBounds.top), SWP_SHOWWINDOW);
    }

    Pause(false);
    return S_OK;
}

//-----------------------------------------------------------------------------
// Name: ForceWindowed()
// Desc: Switch to a windowed mode, even if that means picking a new device
//       and/or adapter
//-----------------------------------------------------------------------------
HRESULT CD3DApplication::ForceWindowed() {
    HRESULT hr;

    if (m_bWindowed)
        return S_OK;

    if (!FindBestWindowedMode(false, false)) {
        return E_FAIL;
    }
    m_bWindowed = true;

    // Now destroy the current 3D device objects, then reinitialize

    Pause(true);

    // Release all scene objects that will be re-created for the new device
    Cleanup3DEnvironment();

    // Create the new device
    if (FAILED(hr = Initialize3DEnvironment()))
        return DisplayErrorMsg(hr, MSGERR_APPMUSTEXIT);

    Pause(false);
    return S_OK;
}

//-----------------------------------------------------------------------------
// Name: AdjustWindowForChange()
// Desc: Prepare the window for a possible change between windowed mode and
//       fullscreen mode.  This function is virtual and thus can be overridden
//       to provide different behavior, such as switching to an entirely
//       different window for fullscreen mode (as in the MFC sample apps).
//-----------------------------------------------------------------------------
HRESULT CD3DApplication::AdjustWindowForChange() {
    if (m_bWindowed) {
        // Set windowed-mode style
        SetWindowLong(m_hWnd, GWL_STYLE, m_dwWindowStyle);
        if (m_hMenu != NULL) {
            SetMenu(m_hWnd, m_hMenu);
            m_hMenu = NULL;
        }
    } else {
        // Set fullscreen-mode style
        SetWindowLong(m_hWnd, GWL_STYLE, WS_POPUP | WS_SYSMENU | WS_VISIBLE);
        if (m_hMenu == NULL) {
            m_hMenu = GetMenu(m_hWnd);
            SetMenu(m_hWnd, NULL);
        }
    }
    return S_OK;
}

//-----------------------------------------------------------------------------
// Name: UserSelectNewDevice()
// Desc: Displays a dialog so the user can select a new adapter, device, or
//       display mode, and then recreates the 3D environment if needed
//-----------------------------------------------------------------------------
HRESULT CD3DApplication::UserSelectNewDevice() {
    HRESULT hr;

    // Can't display dialogs in fullscreen mode
    if (m_bWindowed == false) {
        if (FAILED(ToggleFullscreen())) {
            DisplayErrorMsg(D3DAPPERR_RESETFAILED, MSGERR_APPMUSTEXIT);
            return E_FAIL;
        }
    }

    CD3DSettingsDialog settingsDialog(&m_d3dEnumeration, &m_d3dSettings);
    if (settingsDialog.ShowDialog(m_hWnd) != IDOK)
        return S_OK;
    settingsDialog.GetFinalSettings(&m_d3dSettings);

    m_bWindowed = m_d3dSettings.IsWindowed;

    // Release all scene objects that will be re-created for the new device
    Cleanup3DEnvironment();

    // Inform the display class of the change. It will internally
    // re-create valid surfaces, a d3ddevice, etc.
    if (FAILED(hr = Initialize3DEnvironment())) {
        if (hr != D3DERR_OUTOFVIDEOMEMORY)
            hr = D3DAPPERR_RESETFAILED;
        if (!m_bWindowed) {
            // Restore window type to windowed mode
            m_bWindowed = !m_bWindowed;
            m_d3dSettings.IsWindowed = m_bWindowed;
            AdjustWindowForChange();
            SetWindowPos(m_hWnd, HWND_NOTOPMOST, m_rcWindowBounds.left, m_rcWindowBounds.top,
                         (m_rcWindowBounds.right - m_rcWindowBounds.left),
                         (m_rcWindowBounds.bottom - m_rcWindowBounds.top), SWP_SHOWWINDOW);
        }
        return DisplayErrorMsg(hr, MSGERR_APPMUSTEXIT);
    }

    // If the app is paused, trigger the rendering of the current frame
    if (false == m_bFrameMoving) {
        m_bSingleStep = true;
        DXUtil_Timer(TIMER_START);
        DXUtil_Timer(TIMER_STOP);
    }
    return S_OK;
}

//-----------------------------------------------------------------------------
// Name: Run()
//lyymark 1.d3dapp.CD3DApplication 渲染主循环
//-----------------------------------------------------------------------------
INT CD3DApplication::Run() {
    //lyymark fps limit
    //LockFpsPerFrame(1000 / 60);
    // 加载加速器表，用于处理键盘快捷键
    HACCEL hAccel = LoadAccelerators(NULL, MAKEINTRESOURCE(IDR_MAIN_ACCEL));
    // 用于消息处理的变量 Now we're ready to recieve and process Windows messages.
    int bGotMsg;
    MSG msg;
    msg.message = WM_NULL;
    // 预取消息，以确保消息队列为空时不会阻塞
    PeekMessage(&msg, NULL, 0U, 0U, PM_NOREMOVE);
    // 消息循环
    while (WM_QUIT != msg.message)  // 当消息不是退出消息时
    {
        // 检索消息，如果有消息则从消息队列中移除
        bGotMsg = PeekMessage(&msg, NULL, 0U, 0U, PM_REMOVE);
        /*if (!m_bActive)
			{
				Sleep(m_dwFpsLocked);
			}*/
        if (bGotMsg)  // 如果有消息
        {
            // 如果消息不是加速器消息，则进行翻译和分发
            if (0 == TranslateAccelerator(m_hWnd, hAccel, &msg)) {
                TranslateMessage(&msg);  // 翻译虚拟键消息
                DispatchMessage(&msg);   // 将消息发送到窗口过程
            }
        } else  // 如果没有消息

        {
            // Render a frame during idle time (no messages are waiting)
            if (m_bReady)  // 如果应用程序已准备好
            {
                // Temporarily removed
                if (m_dwFpsLocked) {
                    DWORD dSTime = timeGetTime();
                    if (FAILED(Render3DEnvironment()))
                        SendMessage(m_hWnd, WM_CLOSE, 0, 0);
                    DWORD dDTime = timeGetTime() - dSTime;
                    if (dDTime < m_dwFpsLocked) {
                        Sleep(m_dwFpsLocked - dDTime);
                    }
                } else {  // 渲染3D环境，如果渲染失败则发送关闭消息
                    if (FAILED(Render3DEnvironment()))
                        SendMessage(m_hWnd, WM_CLOSE, 0, 0);
                }
            }
        }
    }
    // 销毁加速器表
    if (hAccel != NULL)
        DestroyAcceleratorTable(hAccel);
    // 返回退出消息的参数
    return (INT)msg.wParam;
}

// lyymark 1.d3dapp.Render3DEnvironment 3d环境渲染
HRESULT CD3DApplication::Render3DEnvironment() {
    // 初始化 hr 为失败状态
    HRESULT hr = E_FAIL;

    // 增加渲染帧数
    IncreaseRenderFrame();

    // 检查设备是否丢失
    if (m_bDeviceLost) {
        // 测试设备的合作级别
        if (FAILED(hr = m_pd3dDevice->TestCooperativeLevel())) {
            // 如果设备丢失，返回 S_OK
            if (D3DERR_DEVICELOST == hr)
                return S_OK;

            // 如果设备无法重置
            if (D3DERR_DEVICENOTRESET == hr) {
                // 如果窗口化模式
                if (m_bWindowed) {
                    // 获取适配器信息
                    D3DAdapterInfo* pAdapterInfo = m_d3dSettings.PAdapterInfo();
                    // 获取显示模式
                    m_pD3D->GetAdapterDisplayMode(pAdapterInfo->AdapterOrdinal,
                                                  &m_d3dSettings.Windowed_DisplayMode);
                    // 设置后台缓冲区格式
                    m_d3dpp.BackBufferFormat = m_d3dSettings.Windowed_DisplayMode.Format;
                }

                // 尝试重置 3D 环境
                if (FAILED(hr = Reset3DEnvironment()))
                    return hr;
            }

            // 返回 hr 错误代码
            return hr;
        }

        // 设备不再丢失
        m_bDeviceLost = false;
    }

    // 获取应用时间
    FLOAT fAppTime = DXUtil_Timer(TIMER_GETAPPTIME);
    // 获取已用时间
    FLOAT fElapsedAppTime = DXUtil_Timer(TIMER_GETELAPSEDTIME);

    // 如果已用时间为 0 且帧在移动，返回 S_OK
    if ((0.0f == fElapsedAppTime) && m_bFrameMoving)
        return S_OK;

    // 如果帧在移动或单步模式
    if (m_bFrameMoving || m_bSingleStep) {
        // 更新时间变量
        m_fTime = fAppTime;
        m_fElapsedTime = fElapsedAppTime;
        m_dwElapsedMS = (unsigned long)(fElapsedAppTime * 1000.f);

        // 调用 FrameMove 并检查返回值
        if (FAILED(hr = FrameMove()))
            return hr;
        // 重置单步标志
        m_bSingleStep = false;
    }

    // 如果应用不活跃，返回 S_OK
    if (!m_bActive)
        return S_OK;

    // 调用 Render 并检查返回值
    if (FAILED(hr = Render()))
        return hr;

    // 更新统计信息
    UpdateStats();
    // 显示绘制结果
    hr = m_pd3dDevice->Present(NULL, NULL, NULL, NULL);

    // 如果设备丢失，设置设备丢失标志
    if (D3DERR_DEVICELOST == hr)
        m_bDeviceLost = true;

    // 返回 S_OK
    return S_OK;
}

//-----------------------------------------------------------------------------
// Name: UpdateStats()
// Desc:
//-----------------------------------------------------------------------------
void CD3DApplication::UpdateStats() {
    // Keep track of the frame count
    static FLOAT fLastTime = 0.0f;
    static DWORD dwFrames = 0;
    FLOAT        fTime = DXUtil_Timer(TIMER_GETABSOLUTETIME);
    ++dwFrames;

    // Update the scene stats once per second
    if (fTime - fLastTime > 1.0f) {
        m_fFPS = dwFrames / (fTime - fLastTime);
        fLastTime = fTime;
        dwFrames = 0;

        TCHAR     strFmt[100];
        D3DFORMAT fmtAdapter = m_d3dSettings.DisplayMode().Format;
        if (fmtAdapter == m_d3dsdBackBuffer.Format) {
            lstrcpyn(strFmt, D3DUtil_D3DFormatToString(fmtAdapter, false), 100);
        } else {
            _sntprintf(strFmt, 100, TEXT("backbuf %s, adapter %s"),
                       D3DUtil_D3DFormatToString(m_d3dsdBackBuffer.Format, false),
                       D3DUtil_D3DFormatToString(fmtAdapter, false));
        }
        strFmt[99] = TEXT('\0');

        TCHAR strDepthFmt[100];
        if (m_d3dEnumeration.AppUsesDepthBuffer) {
            _sntprintf(strDepthFmt, 100, TEXT(" (%s)"),
                       D3DUtil_D3DFormatToString(m_d3dSettings.DepthStencilBufferFormat(), false));
            strDepthFmt[99] = TEXT('\0');
        } else {
            // No depth buffer
            strDepthFmt[0] = TEXT('\0');
        }

        TCHAR* pstrMultiSample;
        switch (m_d3dSettings.MultisampleType()) {
            case D3DMULTISAMPLE_NONMASKABLE:
                pstrMultiSample = TEXT(" (Nonmaskable Multisample)");
                break;
            case D3DMULTISAMPLE_2_SAMPLES:
                pstrMultiSample = TEXT(" (2x Multisample)");
                break;
            case D3DMULTISAMPLE_3_SAMPLES:
                pstrMultiSample = TEXT(" (3x Multisample)");
                break;
            case D3DMULTISAMPLE_4_SAMPLES:
                pstrMultiSample = TEXT(" (4x Multisample)");
                break;
            case D3DMULTISAMPLE_5_SAMPLES:
                pstrMultiSample = TEXT(" (5x Multisample)");
                break;
            case D3DMULTISAMPLE_6_SAMPLES:
                pstrMultiSample = TEXT(" (6x Multisample)");
                break;
            case D3DMULTISAMPLE_7_SAMPLES:
                pstrMultiSample = TEXT(" (7x Multisample)");
                break;
            case D3DMULTISAMPLE_8_SAMPLES:
                pstrMultiSample = TEXT(" (8x Multisample)");
                break;
            case D3DMULTISAMPLE_9_SAMPLES:
                pstrMultiSample = TEXT(" (9x Multisample)");
                break;
            case D3DMULTISAMPLE_10_SAMPLES:
                pstrMultiSample = TEXT(" (10x Multisample)");
                break;
            case D3DMULTISAMPLE_11_SAMPLES:
                pstrMultiSample = TEXT(" (11x Multisample)");
                break;
            case D3DMULTISAMPLE_12_SAMPLES:
                pstrMultiSample = TEXT(" (12x Multisample)");
                break;
            case D3DMULTISAMPLE_13_SAMPLES:
                pstrMultiSample = TEXT(" (13x Multisample)");
                break;
            case D3DMULTISAMPLE_14_SAMPLES:
                pstrMultiSample = TEXT(" (14x Multisample)");
                break;
            case D3DMULTISAMPLE_15_SAMPLES:
                pstrMultiSample = TEXT(" (15x Multisample)");
                break;
            case D3DMULTISAMPLE_16_SAMPLES:
                pstrMultiSample = TEXT(" (16x Multisample)");
                break;
            default:
                pstrMultiSample = TEXT("");
                break;
        }

        const int cchMaxFrameStats = sizeof(m_strFrameStats) / sizeof(TCHAR);
        _sntprintf(m_strFrameStats, cchMaxFrameStats, _T("%.02f fps (%dx%d), %s%s%s"), m_fFPS,
                   m_d3dsdBackBuffer.Width, m_d3dsdBackBuffer.Height, strFmt, strDepthFmt,
                   pstrMultiSample);
        m_strFrameStats[cchMaxFrameStats - 1] = TEXT('\0');
    }
}

//-----------------------------------------------------------------------------
// Name: Pause()
// Desc: Called in to toggle the pause state of the app.
//-----------------------------------------------------------------------------
void CD3DApplication::Pause(bool bPause) {
    static DWORD dwAppPausedCount = 0;

    dwAppPausedCount += (bPause ? +1 : -1);
    m_bActive = (dwAppPausedCount ? false : true);

    // Handle the first pause request (of many, nestable pause requests)
    if (bPause && (1 == dwAppPausedCount)) {
        // Stop the scene from animating
        if (m_bFrameMoving)
            DXUtil_Timer(TIMER_STOP);
    }

    if (0 == dwAppPausedCount) {
        // Restart the timers
        if (m_bFrameMoving)
            DXUtil_Timer(TIMER_START);
    }
}

//-----------------------------------------------------------------------------
// Name: Cleanup3DEnvironment()
// Desc: Cleanup scene objects
//-----------------------------------------------------------------------------
void CD3DApplication::Cleanup3DEnvironment() {
    if (m_pd3dDevice != NULL) {
        if (m_bDeviceObjectsRestored) {
            m_bDeviceObjectsRestored = false;
            InvalidateDeviceObjects();
        }
        if (m_bDeviceObjectsInited) {
            m_bDeviceObjectsInited = false;
            DeleteDeviceObjects();
        }

        if (m_pd3dDevice->Release() > 0)
            DisplayErrorMsg(D3DAPPERR_NONZEROREFCOUNT, MSGERR_APPMUSTEXIT);
        m_pd3dDevice = NULL;
    }
}

//-----------------------------------------------------------------------------
// Name: DisplayErrorMsg()
// Desc: Displays error messages in a message box
//-----------------------------------------------------------------------------
HRESULT CD3DApplication::DisplayErrorMsg(HRESULT hr, DWORD dwType) {
    static bool s_bFatalErrorReported = false;
    TCHAR       strMsg[512];

    // If a fatal error message has already been reported, the app
    // is already shutting down, so don't show more error messages.
    if (s_bFatalErrorReported)
        return hr;

    switch (hr) {
        case D3DAPPERR_NODIRECT3D:
            _tcscpy(strMsg, _T("Could not initialize Direct3D. You may\n")
                            _T("want to check that the latest version of\n")
                            _T("DirectX is correctly installed on your\n")
                            _T("system.  Also make sure that this program\n")
                            _T("was compiled with header files that match\n")
                            _T("the installed DirectX DLLs."));
            break;

        case D3DAPPERR_NOCOMPATIBLEDEVICES:
            _tcscpy(strMsg, _T("Could not find any compatible Direct3D\n")
                            _T("devices."));
            break;

        case D3DAPPERR_NOWINDOWABLEDEVICES:
            _tcscpy(strMsg, _T("This sample cannot run in a desktop\n")
                            _T("window with the current display settings.\n")
                            _T("Please change your desktop settings to a\n")
                            _T("16- or 32-bit display mode and re-run this\n")
                            _T("sample."));
            break;

        case D3DAPPERR_NOHARDWAREDEVICE:
            _tcscpy(strMsg, _T("No hardware-accelerated Direct3D devices\n")
                            _T("were found."));
            break;

        case D3DAPPERR_HALNOTCOMPATIBLE:
            _tcscpy(strMsg, _T("This sample requires functionality that is\n")
                            _T("not available on your Direct3D hardware\n")
                            _T("accelerator."));
            break;

        case D3DAPPERR_NOWINDOWEDHAL:
            _tcscpy(strMsg, _T("Your Direct3D hardware accelerator cannot\n")
                            _T("render into a window.\n")
                            _T("Press F2 while the app is running to see a\n")
                            _T("list of available devices and modes."));
            break;

        case D3DAPPERR_NODESKTOPHAL:
            _tcscpy(strMsg, _T("Your Direct3D hardware accelerator cannot\n")
                            _T("render into a window with the current\n")
                            _T("desktop display settings.\n")
                            _T("Press F2 while the app is running to see a\n")
                            _T("list of available devices and modes."));
            break;

        case D3DAPPERR_NOHALTHISMODE:
            _tcscpy(strMsg, _T("This sample requires functionality that is\n")
                            _T("not available on your Direct3D hardware\n")
                            _T("accelerator with the current desktop display\n")
                            _T("settings.\n")
                            _T("Press F2 while the app is running to see a\n")
                            _T("list of available devices and modes."));
            break;

        case D3DAPPERR_MEDIANOTFOUND:
            //lyytodo 这里之前注释了，不知道为啥，先还原看看
            // case HRESULT_FROM_WIN32( ERROR_FILE_NOT_FOUND ):
        case HRESULT_FROM_WIN32(ERROR_FILE_NOT_FOUND):
            _tcscpy(strMsg, _T("Could not load required media."));
            break;

        case D3DAPPERR_RESETFAILED:
            _tcscpy(strMsg, _T("Could not reset the Direct3D device."));
            break;

        case D3DAPPERR_NONZEROREFCOUNT:
            _tcscpy(strMsg, _T("A D3D object has a non-zero reference\n")
                            _T("count (meaning things were not properly\n")
                            _T("cleaned up)."));
            break;

        case D3DAPPERR_NULLREFDEVICE:
            _tcscpy(strMsg, _T("Warning: Nothing will be rendered.\n")
                            _T("The reference rendering device was selected, but your\n")
                            _T("computer only has a reduced-functionality reference device\n")
                            _T("installed.  Install the DirectX SDK to get the full\n")
                            _T("reference device.\n"));
            break;

        case E_OUTOFMEMORY:
            _tcscpy(strMsg, _T("Not enough memory."));
            break;

        case D3DERR_OUTOFVIDEOMEMORY:
            _tcscpy(strMsg, _T("Not enough video memory."));
            break;

        case D3DERR_DRIVERINTERNALERROR:
            _tcscpy(strMsg, _T("A serious problem occured inside the display driver."));
            dwType = MSGERR_APPMUSTEXIT;
            break;

        default:
            _tcscpy(strMsg, _T("Generic application error. Enable\n")
                            _T("debug output for detailed information."));
    }

#ifdef _DEBUG

    if (MSGERR_APPMUSTEXIT == dwType) {
        s_bFatalErrorReported = true;
        _tcscat(strMsg, _T("\n\nThis sample will now exit."));
        MessageBox(NULL, strMsg, m_strWindowTitle, MB_ICONERROR | MB_OK);

        // Close the window, which shuts down the app
        /*if( m_hWnd )
				SendMessage( m_hWnd, WM_CLOSE, 0, 0 );*/
    } else {
        if (MSGWARN_SWITCHEDTOREF == dwType)
            _tcscat(strMsg, _T("\n\nSwitching to the reference rasterizer,\n")
                            _T("a software device that implements the entire\n")
                            _T("Direct3D feature set, but runs very slowly."));
        MessageBox(NULL, strMsg, m_strWindowTitle, MB_ICONWARNING | MB_OK);
    }

#endif

    return hr;
}

bool CD3DApplication::SetFullScreenMode(DWORD dwWidth, DWORD dwHeight, DWORD dwDepth) {
    // ERR("CD3D9Application: 不支持函数[SetFullScreenMode]\n");
    return false;
}
}  // namespace rt_graph_dx9

void CD3DApplication::BeginRenderViewPort(DWORD dwX, DWORD dwY, DWORD dwWeight, DWORD dwHeight,
                                          float fMin, float fMax) {
    D3DVIEWPORT9 d3dvp = {dwX, dwY, dwWeight, dwHeight, fMin, fMax};
    m_pd3dDevice->GetViewport(&m_d3dLastViewPort);
    m_pd3dDevice->SetViewport(&d3dvp);
}

void CD3DApplication::EndRenderViewPort() {
    m_pd3dDevice->SetViewport(&m_d3dLastViewPort);
}
