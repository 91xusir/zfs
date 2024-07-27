
#include "rtg_graph_inter.h"

namespace rt_graph {

RtPerformance g_GraphPref;
RtgRenderOption g_RenderOption;
string _call_create_buffer_func_name;
string _call_create_buffer_func_line;

//lyymark 1.Core.RtgDevice 默认配置
RtgDevice::RtgDevice()
{
    int i;

    m_bDeviceReady = FALSE;
    m_pEvent = NULL;
    m_pCamera = NULL;
    m_pLastShader = NULL;

    m_pVB = NULL;
    //m_pIB = NULL;

    // 是否背面剔除
    m_bCullFace = TRUE;

    // 是否进行可见检查
    m_bEnableViewCheck = FALSE;

    // 是否清除Z缓冲区后调用OnClearZRender函数(通常用在编辑器)
    m_bClearZRender = FALSE;

    // 填充模式
    m_eFillMode = RTG_FILLSOLID;

    // 时间设置
    m_fDesireFPS = 30.f;

    // 顶点
    m_bVertexProcessing = FALSE;

    m_dwVertexBufferStartOffset = 0;
    m_dwIndexBufferStartOffset = 0;

    // 颜色
    m_dwClearColor = 0x3030A030;
    m_dwCurrentColor = ~m_dwClearColor;

    // 当前的Draw的次数
    m_iTextureCurPass = 0;
    // 需要Draw的次数 (1为只有1次)
    m_iTextureRequiredPasses = 1;
    m_eTextureFilterMip = RTGTEXF_LINEAR;
    m_eTextureFilterMin = RTGTEXF_LINEAR;
    m_eTextureFilterMag = RTGTEXF_LINEAR;
    m_eTextureAddress = RTGTADD_WRAP;
    m_dwTextureBorderColor = 0x00000000;

    // Fog
    m_bFogEnable = FALSE;
    m_dwFogColor = 0xFFFFFFFF;
    m_fFogStart = 50.f;
    m_fFogEnd = 500.f;

    // AlphaTest & AlphaBlend
    m_bAlphaTest = FALSE;
    m_iAlphaRef = 0;
    m_eAlphaFunc = RTGCMP_GREATER;
    m_bBlend = FALSE;
    m_eBlendSrcFunc = RTGBLEND_ONE;
    m_eBlendDestFunc = RTGBLEND_ZERO;
    m_dwTextureFactor = 0xFFFFFFFF;

    // Material
    m_vMaterialDiffuse.Set(1.f);
    m_vMaterialAmbient.Set(1.f);
    m_vMaterialSpecular.Set(0.f);
    m_vMaterialEmissive.Set(0.f);
    m_fMaterialPower = 0.f;

    // Depth Test
    m_bZTest = TRUE;
    m_bZWrite = TRUE;
    m_eZFunc = RTGCMP_GREATER;

    m_bGray = false;
    m_bUsePostProcessEffectOnoff = false;
    m_dwPostProcessObjectHLColorSpe = 0x00FFFFFF;
	m_fPostProcessObjectHLColorSpePower = 8;
    m_fPostProcessObjectHLScale = 1.f;
	m_dwPostProcessObjectHLColorEmi = 0x00222222;
	m_dwPostProcessObjectHLColorDif = 0x00555555;

    m_bUsePostProcessObjectHighLight = false;
    m_bUsePostProcessObjectHighLightOnoff = false;
    m_iPostProcessEffectType = 0;
    m_fPostProcessEffectContrast = 0;
    m_fPostProcessEffectIntensity = 0.5f;

   // m_iRenderTargetStackSize = 0;

    for (i=0; i<8; i++)
    {
        m_bTexture[0] = FALSE;
        m_bUseTextureMatrix[i] = FALSE;
        m_dwTexTransformFlag[i] = RTGTTF_COUNT2;
    }
    m_mTextureDefaultMatrix.Identity();

    m_eCurrentViewWindowMode = RTGVWM_WINDOWED;
    for (i=0; i<RTGVWM_MAX; i++)
    {
        m_ViewWnd[i].bEnable = FALSE;
        m_ViewWnd[i].hWnd = NULL;
    }

    m_Ability.bFullScreneAntialiasing = false;  // 全屏反锯齿
    m_Ability.bSquareTexture = false;           // 是否需要方形的贴图
    m_Ability.bEnvBumpmap = false;              // 是否支持Bumpmap
    m_Ability.bDynamicTextures = false;         // 是否支持动态贴图
    m_Ability.bDot3ProductBumpmap = false;      // 是否支持Dot3ProductBumpmap
    m_Ability.bTextureOpMultiplyAdd = false;    // 
    m_Ability.iMaxTextureWidth = 0;             // 最大贴图宽度
    m_Ability.iMaxTextureHeight = 0;            // 最大贴图高度
    m_Ability.iMaxMultiTextures = 0;            // 一次渲染最多的贴图层数
    m_Ability.iMaxTextureStages = 0;            // 一次渲染最多的Stage数
    m_Ability.iMaxPrimitiveCount = 0;           // 最多的三角形数
    m_Ability.iMaxVertexIndex = 0;              // 最多的索引数
    m_Ability.iMaxStreams = 0;                  // 最多的
    m_Ability.iMaxStreamStride = 0;             // 最大的
    m_Ability.bVertexBuffer = false;            // 是否支持VertexBuffer
    m_Ability.bVertexShader = false;            // 是否支持VertexShader
    m_Ability.bPixelShader = false;             // 是否支持PixelShader

    m_userConfig.bFullscreen            = false;        // 全屏模式   关
    m_userConfig.bFSAA                  = false;        // 全屏抗锯齿 关
    m_userConfig.bEdgeAntialias         = false;        // 全屏抗锯齿 关
    m_userConfig.bEnableConsole         = false;
    m_userConfig.lWndWidth              = 1024;
    m_userConfig.lWndHeight             = 768;
    m_userConfig.lWndColorDepth         = 32;
    m_userConfig.lTextureQuality        = 0;            // 贴图质量   高
    m_userConfig.lGeometryQuality       = 0;            // 贴图质量   高
    m_userConfig.bPosteffect            = 1;            // 后期处理   关
    m_userConfig.bObjEffect             = 1;            // 后期加强   关
    m_userConfig.dwMaxMultiSampleTypeWindowed   = 0;
    m_userConfig.dwMaxMultiSampleTypeFullscreen = 0;
    m_userConfig.lMillisecondPerFrame   = 16;           // 1000/16 = 60
    m_userConfig.bvsync = false;                        // 垂直同步   关

    rt2_strcpy(m_config.szWindowTitle, "rtgGraph 3D App");
    m_config.iDefaultStartX          = CW_USEDEFAULT;   // 开始窗口的X位置
    m_config.iDefaultStartY          = CW_USEDEFAULT;   // 开始窗口的Y位置
    m_config.hCustomWndHandle        = 0;               // 开始窗口的Y位置
    m_config.fCameraFOV              = 45;              //
    m_config.fCameraAspect           = 1.333333333;     // 4/3, 16/9
    m_config.fCameraNear             = 3;               //
    m_config.fCameraFar              = 5000;            //
    m_config.bEnableFog              = true;

}

RtgDevice::~RtgDevice()
{
}

struct SResumeDefaultDisplayMode
{
    DWORD dwColorDepth;
    DWORD dwScreenWidth;
    DWORD dwScreenHeight;
    DWORD dwRefreshRate;
};

SResumeDefaultDisplayMode   s_ResumeDefaultDisplayMode;

//lyymark 1.Device.Init Device初始化
bool RtgDevice::Init(RTHINSTANCE hInst, RtRuntimeClass* pEventClass, RtRuntimeClass* pCameraClass, const char* szGraphIni, const char* szUserIni)
{
    RtIni graphIni;
    if (!graphIni.OpenFile(szGraphIni))
        return false;
    if (szUserIni)
    {
        RtIni userIni;
		if (!userIni.OpenFile(szUserIni))
		{
			if (!graphIni.OpenFile(szGraphIni))
				return false;
		}
		else
		{
			if (Init(hInst, pEventClass, pCameraClass, &graphIni, &userIni))
			{
				return true;
			}
			else
			{
				if (!graphIni.OpenFile(szGraphIni))
					return false;
			}
		}
    }
    return Init(hInst, pEventClass, pCameraClass, &graphIni, NULL);
}

bool RtgDevice::Init(RTHINSTANCE hInst, RtRuntimeClass* pEventClass, RtRuntimeClass* pCameraClass, RtIni* pGraphIni, RtIni* pUserIni)
{
    RTASSERT(pEventClass);

    if (!pEventClass->IsDerivedFrom(RT_RUNTIME_CLASS(CRtgAppFrame)))
        return false;

    m_hAppInstance = hInst;

    m_pEvent = (RtgDeviceEvent*)(pEventClass->CreateObject());
    SetCamera((RtgCamera*)(pCameraClass->CreateObject()));


    DEVMODE devMode;
    memset(&devMode, 0, sizeof(DEVMODE));
    devMode.dmSize = sizeof(DEVMODE);
    EnumDisplaySettings(NULL, ENUM_CURRENT_SETTINGS, &devMode);
    s_ResumeDefaultDisplayMode.dwScreenWidth  = devMode.dmPelsWidth;
    s_ResumeDefaultDisplayMode.dwScreenHeight = devMode.dmPelsHeight;
    s_ResumeDefaultDisplayMode.dwColorDepth   = devMode.dmBitsPerPel;
    s_ResumeDefaultDisplayMode.dwRefreshRate  = devMode.dmDisplayFrequency;

    ReadGraphIni(pGraphIni);
    ReadUserIni(pUserIni);
// lyymark 搞不懂为什么不让生效 先放着
#if defined(_MT) || defined(_DLL)
    m_userConfig.bFullscreen = false;
#endif

    ApplyConfig();

    RtgImage::StaticInit();

    if (!OnPreCreate3DDevice())
        return false;

    if (!OnCreate3DDevice())
        return false;

    if (!OnAfterCreate3DDevice())
        return false;

    if (m_userConfig.bFullscreen)
        SetViewMode(RTGVWM_TOPWINDOW, true);

    if (!OnInit())
        return false;

    if (!((CRtgAppFrame*)m_pEvent)->OnFrameInit())
        return false;

    m_bDeviceReady = TRUE;

    return true;
}

RTHWND RtgDevice::GetWndHandle(RTGViewWindowMode eMode)
{
    if (eMode<RTGVWM_WINDOWED || eMode>=RTGVWM_MAX)
        eMode = RTGVWM_WINDOWED;
    return m_ViewWnd[eMode].hWnd;
}


//lyymark 1.Core.ReadGraphIni加载Graph.ini配置
void RtgDevice::ReadGraphIni(RtIni* pGraphIni)
{
    long lTotalTextureMem = 8*1024*1024;

    const char* szWndSection = "Window";
    pGraphIni->GetEntry(szWndSection, "Fullscreen",          &(m_userConfig.bFullscreen));
    pGraphIni->GetEntry(szWndSection, "WndWidth",            &(m_userConfig.lWndWidth));
    pGraphIni->GetEntry(szWndSection, "WndHeight",           &(m_userConfig.lWndHeight));
    pGraphIni->GetEntry(szWndSection, "ColorDepth",          &(m_userConfig.lWndColorDepth));
    pGraphIni->GetEntry(szWndSection, "StartX",              &(m_config.iDefaultStartX));
    pGraphIni->GetEntry(szWndSection, "StartY",              &(m_config.iDefaultStartY));
    pGraphIni->GetEntry(szWndSection, "Title",               m_config.szWindowTitle, 256);

    const char* szGraphSection = "Graph";
    pGraphIni->GetEntry(szGraphSection, "EnableConsole",     &(m_userConfig.bEnableConsole));
    pGraphIni->GetEntry(szGraphSection, "FSAA",              &(m_userConfig.bFSAA));
    pGraphIni->GetEntry(szGraphSection, "EdgeAntialias",     &(m_userConfig.bEdgeAntialias));
    pGraphIni->GetEntry(szGraphSection, "PostEffect",        &(m_userConfig.bPosteffect));
    pGraphIni->GetEntry(szGraphSection, "ObjEffect",         &(m_userConfig.bObjEffect));
    pGraphIni->GetEntry(szGraphSection, "MultiSampleWnd",    &(m_userConfig.dwMaxMultiSampleTypeWindowed));
    pGraphIni->GetEntry(szGraphSection, "MultiSampleFsc",    &(m_userConfig.dwMaxMultiSampleTypeFullscreen));
    pGraphIni->GetEntry(szGraphSection, "LockFps",           &(m_userConfig.lMillisecondPerFrame));
    pGraphIni->GetEntry(szGraphSection, "Vsync",             &(m_userConfig.bvsync));

    const char* szTextureSection = "Texture";
    pGraphIni->GetEntry(szTextureSection, "TotalTextureMem", &(lTotalTextureMem));

    const char* szQualitySection = "Quality";
    pGraphIni->GetEntry(szQualitySection, "TextureQuality",  &(m_userConfig.lTextureQuality));
    pGraphIni->GetEntry(szQualitySection, "GeometryQuality", &(m_userConfig.lGeometryQuality));

    const char* szCameraSection = "Camera";
    pGraphIni->GetEntry(szCameraSection, "FOV",              &(m_config.fCameraFOV));
    pGraphIni->GetEntry(szCameraSection, "Aspect",           &(m_config.fCameraAspect));
    pGraphIni->GetEntry(szCameraSection, "Near",             &(m_config.fCameraNear));
    pGraphIni->GetEntry(szCameraSection, "Far",              &(m_config.fCameraFar));
    pGraphIni->GetEntry(szCameraSection, "EnableFog",        &(m_config.bEnableFog));

    if (m_config.iDefaultStartX==-1) m_config.iDefaultStartX = CW_USEDEFAULT;
    if (m_config.iDefaultStartY==-1) m_config.iDefaultStartY = CW_USEDEFAULT;
}
//lyymark 1.Core.ReadGraphIni加载User.ini配置
void RtgDevice::ReadUserIni(RtIni* pUserIni)
{
//Heten
    const char* szUserSection = "graph";
    pUserIni->GetEntry(szUserSection, "fullscreen",       &(m_userConfig.bFullscreen));
    pUserIni->GetEntry(szUserSection, "width",            &(m_userConfig.lWndWidth));
    pUserIni->GetEntry(szUserSection, "height",           &(m_userConfig.lWndHeight));
    pUserIni->GetEntry(szUserSection, "color",            &(m_userConfig.lWndColorDepth));
//end
    pUserIni->GetEntry(szUserSection, "FSAA",             &(m_userConfig.bFSAA));
    pUserIni->GetEntry(szUserSection, "EdgeAntialias",    &(m_userConfig.bEdgeAntialias));
    pUserIni->GetEntry(szUserSection, "PostEffect",       &(m_userConfig.bPosteffect));
    pUserIni->GetEntry(szUserSection, "ObjEffect",        &(m_userConfig.bObjEffect));
    pUserIni->GetEntry(szUserSection, "MultiSampleWnd",   &(m_userConfig.dwMaxMultiSampleTypeWindowed));
    pUserIni->GetEntry(szUserSection, "MultiSampleFsc",   &(m_userConfig.dwMaxMultiSampleTypeFullscreen));
    pUserIni->GetEntry(szUserSection, "LockFps",          &(m_userConfig.lMillisecondPerFrame));
    pUserIni->GetEntry(szUserSection, "Vsync",           &(m_userConfig.bvsync));
    pUserIni->GetEntry(szUserSection, "TextureQuality",   &(m_userConfig.lTextureQuality));
    pUserIni->GetEntry(szUserSection, "GeometryQuality",  &(m_userConfig.lGeometryQuality));
}

void RtgDevice::ApplyConfig()
{
    DWORD dwScreenW = (DWORD)::GetSystemMetrics(SM_CXSCREEN);
    DWORD dwScreenH = (DWORD)::GetSystemMetrics(SM_CYSCREEN);

    // 绑定窗口如果为NULL，就是自己创建窗口
    BindViewWindow(RTGVWM_WINDOWED, m_config.hCustomWndHandle);
    BindViewWindow(RTGVWM_TOPWINDOW, m_config.hCustomWndHandle);
    SetViewWindowInfo(RTGVWM_WINDOWED, m_userConfig.lWndWidth, m_userConfig.lWndHeight, m_userConfig.lWndColorDepth, 0);
    SetViewWindowInfo(RTGVWM_TOPWINDOW, m_userConfig.lWndWidth, m_userConfig.lWndHeight, m_userConfig.lWndColorDepth, 0);
    LockFps(m_userConfig.lMillisecondPerFrame);


    //m_pEvent->EnableConsole(m_config.bEnableConsole);
}

void RtgDevice::SetCustomWndHandle(HWND hWnd)
{
    m_config.hCustomWndHandle = hWnd;
}

void RtgDevice::Close()
{
    if (m_pEvent && m_pCamera)
    {
        //((CRtgAppFrame*)m_pEvent)->Close();
        this->OnClose();
        ResumeDefaultDisplayMode();
        DEL_ONE(m_pCamera);
       // DEL_ONE(m_pEvent);
    }
}

void RtgDevice::ResumeDefaultDisplayMode()
{
    /*
    DEVMODE devMode;
    memset(&devMode, 0, sizeof(DEVMODE));
    devMode.dmSize = sizeof(DEVMODE);
    EnumDisplaySettings(NULL, ENUM_CURRENT_SETTINGS, &devMode);
    if (   s_ResumeDefaultDisplayMode.dwScreenWidth==devMode.dmPelsWidth
        && s_ResumeDefaultDisplayMode.dwScreenHeight==devMode.dmPelsHeight
        && s_ResumeDefaultDisplayMode.dwColorDepth==devMode.dmBitsPerPel
        && s_ResumeDefaultDisplayMode.dwRefreshRate==devMode.dmDisplayFrequency)
        return;

    devMode.dmFields = DM_BITSPERPEL | DM_PELSWIDTH | DM_PELSHEIGHT | (s_ResumeDefaultDisplayMode.dwRefreshRate?DM_DISPLAYFREQUENCY:0);
    devMode.dmPelsWidth         = s_ResumeDefaultDisplayMode.dwScreenWidth;
    devMode.dmPelsHeight        = s_ResumeDefaultDisplayMode.dwScreenHeight;
    devMode.dmBitsPerPel        = s_ResumeDefaultDisplayMode.dwColorDepth;
    devMode.dmDisplayFrequency  = s_ResumeDefaultDisplayMode.dwRefreshRate;
    if (ChangeDisplaySettings(&devMode, CDS_RESET)!=DISP_CHANGE_SUCCESSFUL)
    {
        LOG("RtgDevice: 切换分辨率失败.\n");
    }
    */
    RtCoreLog().Info("ResumeDefaultDisplayMode.\n");
    if (ChangeDisplaySettings(NULL, 0)!=DISP_CHANGE_SUCCESSFUL)
    {
        RtCoreLog().Info("RtgDevice: 切换分辨率失败.\n");
    }
}

int RtgDevice::RunEngine()
{
    return this->OnRun();
}

void RtgDevice::BindViewWindow(RTGViewWindowMode eMode, RTHWND hWnd, bool bResize)
{
    m_ViewWnd[eMode].eViewMode      = eMode;
    m_ViewWnd[eMode].bEnable        = true;
    m_ViewWnd[eMode].bResize        = bResize;
    m_ViewWnd[eMode].bFullscreen    = false;
    m_ViewWnd[eMode].bTopwindow     = false;
    m_ViewWnd[eMode].iPosX          = 0;
    m_ViewWnd[eMode].iPosY          = 0;
    m_ViewWnd[eMode].dwWidth        = 800;
    m_ViewWnd[eMode].dwHeight       = 600;
    m_ViewWnd[eMode].dwDepth        = 16;
    m_ViewWnd[eMode].dwRefreshRate  = 60;
    m_ViewWnd[eMode].hWnd           = hWnd;
    m_ViewWnd[eMode].dwWndStyle     = GetWindowLong((HWND)hWnd, GWL_STYLE);


	switch (eMode)
    {
    case RTGVWM_TOPWINDOW:
        m_ViewWnd[eMode].dwWndStyle = WS_POPUP | WS_BORDER | WS_VISIBLE;
        m_ViewWnd[eMode].bTopwindow = true;
        break;
    case RTGVWM_WINDOWED://alter by tim.yang  20080724注释掉可以让鼠标拖拽窗口的参数
		m_ViewWnd[eMode].dwWndStyle = WS_OVERLAPPED| WS_CAPTION | WS_MINIMIZEBOX| WS_SYSMENU /*| WS_THICKFRAME */| WS_VISIBLE;//WS_CAPTION |WS_MINIMIZEBOX| WS_VISIBLE;
        break;
    case RTGVWM_FULLSCREEN:
        m_ViewWnd[eMode].bFullscreen = true;
        break;
    }
}

void RtgDevice::SetViewWindowInfo(RTGViewWindowMode eMode, DWORD dwWidth, DWORD dwHeight, DWORD dwDepth, DWORD dwRefreshRate)
{
    if (!m_ViewWnd[eMode].bEnable) return;
    m_ViewWnd[eMode].dwWidth        = dwWidth;
    m_ViewWnd[eMode].dwHeight       = dwHeight;
    m_ViewWnd[eMode].dwDepth        = dwDepth;
    m_ViewWnd[eMode].dwRefreshRate  = dwRefreshRate;
}

bool RtgDevice::SetViewMode(RTGViewWindowMode eMode, bool bStype)
{
    DWORD dwWidth = m_ViewWnd[eMode].dwWidth;
    DWORD dwHeight = m_ViewWnd[eMode].dwHeight;
    DWORD dwDepth = m_ViewWnd[eMode].dwDepth;
    DWORD dwRefreshRate = m_ViewWnd[eMode].dwRefreshRate;

    if (eMode == RTGVWM_TOPWINDOW)
    {
        dwWidth = (DWORD)::GetSystemMetrics(SM_CXSCREEN);
        dwHeight = (DWORD)::GetSystemMetrics(SM_CYSCREEN);
    }

    bool bResult = false;

    if (m_eCurrentViewWindowMode == eMode
        && m_iWndWidth      == dwWidth
        && m_iWndHeight     == dwHeight
        && m_iWndColorBits  == dwDepth
        && m_dwRefreshRate  == dwRefreshRate
        )
        return true;

    // 保存最后一次窗口的位置信息
    RECT rc;
    HWND hWnd = (HWND)m_ViewWnd[m_eCurrentViewWindowMode].hWnd;
    GetClientRect(hWnd, &rc);
    ClientToScreen(hWnd, (LPPOINT)&rc);
    m_ViewWnd[m_eCurrentViewWindowMode].iPosX = rc.left;
    m_ViewWnd[m_eCurrentViewWindowMode].iPosY = rc.top;

    // 切换分辨率
    //if (m_ViewWnd[eMode].bTopwindow || m_ViewWnd[m_eCurrentViewWindowMode].bTopwindow)
    //{
    //    if (m_ViewWnd[eMode].bTopwindow)
    //    {
    //        DEVMODE devMode;
    //        memset(&devMode, 0, sizeof(DEVMODE));
    //        devMode.dmSize = sizeof(DEVMODE);
    //        devMode.dmFields = DM_BITSPERPEL | DM_PELSWIDTH | DM_PELSHEIGHT | (m_ViewWnd[eMode].dwRefreshRate?DM_DISPLAYFREQUENCY:0);
    //        devMode.dmPelsWidth         = dwWidth;
    //        devMode.dmPelsHeight        = dwHeight;
    //        devMode.dmBitsPerPel        = dwDepth;
    //        devMode.dmDisplayFrequency  = dwRefreshRate;
    //        //if (ChangeDisplaySettings(&devMode, CDS_RESET)!=DISP_CHANGE_SUCCESSFUL)
    //        //RtCoreLog().Info("RtgDevice: 切换分辨率 %dx%dx%dx%d.\n", m_ViewWnd[eMode].dwWidth, m_ViewWnd[eMode].dwHeight, m_ViewWnd[eMode].dwDepth,m_ViewWnd[eMode].dwRefreshRate);
    //       /* if (ChangeDisplaySettings(&devMode, 0)!=DISP_CHANGE_SUCCESSFUL)
    //        {
    //            RtCoreLog().Warn("RtgDevice: 切换分辨率失败.\n");
    //            return false;
    //        }*/
    //        //SetWindowPos(hWnd, HWND_TOPMOST, 0, 0, 10, 10, SWP_NOMOVE);
    //        SetWindowPos(hWnd, HWND_TOP, 0, 0, 10, 10, SWP_NOMOVE);
    //    }else
    //    {
    //        //SetWindowPos(hWnd, HWND_NOTOPMOST, 0, 0, 10, 10, SWP_NOMOVE);
    //        ResumeDefaultDisplayMode();
    //    }
    //}

    // 设置当前的窗口风格
    if (bStype && m_ViewWnd[eMode].dwWndStyle!=GetWindowLong((HWND)m_ViewWnd[eMode].hWnd, GWL_STYLE))
    {
        SetWindowLong((HWND)m_ViewWnd[eMode].hWnd, GWL_STYLE, m_ViewWnd[eMode].dwWndStyle);
    }
    else
    {
        m_ViewWnd[eMode].dwWndStyle = GetWindowLong((HWND)m_ViewWnd[eMode].hWnd, GWL_STYLE);
    }

    // 取得窗口位置
    if (m_ViewWnd[eMode].bTopwindow)
    {
        rc.left = 0;
        rc.top  = 0;
    }else
    {
        rc.left = m_ViewWnd[eMode].iPosX;
        rc.top  = m_ViewWnd[eMode].iPosY;
    }

    // 如果这次或者上次有一次是全屏幕模式，就要切换全屏/窗口模式
    if (eMode==RTGVWM_FULLSCREEN || m_eCurrentViewWindowMode==RTGVWM_FULLSCREEN)
    {
        // 如果两个模式不同，就需要切换，否则只需要调整
        if (eMode!=m_eCurrentViewWindowMode)
        {
            if (eMode==RTGVWM_FULLSCREEN)
            {
                SetFullScreenInfo(dwWidth, dwHeight, dwDepth);
            }
            bResult = FullscreenToggle();
        }else
        {
            bResult = ResetDevice(TRUE, dwWidth, dwHeight, dwDepth);
        }
    }else
    {
        if (m_ViewWnd[eMode].bResize)
        {
            rc.right = rc.left + dwWidth;
            rc.bottom = rc.top + dwHeight;
            AdjustWindowRect(&rc, m_ViewWnd[eMode].dwWndStyle, FALSE);
            if (GetParent(hWnd))
            {
                RECT prc;
                GetClientRect(GetParent(hWnd), &prc);
                ClientToScreen(GetParent(hWnd), (LPPOINT)&prc);
                rc.left     -= prc.left;
                rc.top      -= prc.top;
                rc.right    -= prc.left;
                rc.bottom   -= prc.top;
            }
            MoveWindow(hWnd, rc.left, rc.top, rc.right - rc.left, rc.bottom - rc.top, true);
        }
        OnWindowSizeChanged();
    }
    m_eCurrentViewWindowMode = eMode;
    return bResult;
}

int RtgDevice::GetAvailableTextureMem()
{
    return 0;
}

BOOL RtgDevice::SaveRenderTargetToFile(const char* szFileName, bool bJPG)
{
    RtCoreLog().Warn("错误: 当前设备不支持保存渲染对象到文件的功能.\n");
    return FALSE;
}

BOOL RtgDevice::SaveRenderTargetToArchive(RtArchive* pArc, bool bJPG)
{
    RtCoreLog().Warn("错误: 当前设备不支持保存渲染对象到流的功能.\n");
    return FALSE;
}

bool RtgDevice::OnErrorMessage(const char* szMsg, int iType)
{
    CHECK(m_pEvent!=NULL);
    return m_pEvent->OnErrorMessage(szMsg, iType);
}

void RtgDevice::RenderScene()
{
    CHECKEX("没有初始化图形引擎");
}

bool RtgDevice::OnPreCreate3DDevice()
{
	CHECK(m_pEvent!=NULL);

	const DWORD dwVBDTotal = 1;
	DWORD dwVBDSize[dwVBDTotal] = {500*1024};
	const DWORD dwVBSTotal = 0;
	//DWORD dwVBSSize[dwVBSTotal] = {1*1024*1024};

	if (!m_pVB->InitStreamManager(dwVBDTotal, dwVBDSize, dwVBSTotal, NULL, 1000, 0))
	{
		RtCoreLog().Warn("ERROR: 创建顶点缓冲区失败.\n");
	}

	//const DWORD dwIBDTotal = 1;
	//DWORD dwIBDSize[dwIBDTotal] = {100*1024};
	//const DWORD dwIBSTotal = 0;
	////DWORD dwIBSSize[dwIBSTotal] = {1*1024*1024};
	//if (!m_pIB->InitStreamManager(dwIBDTotal, dwIBDSize, dwIBSTotal, NULL, 3000, 0))
	//{
	//	RtCoreLog().Warn("ERROR: 创建索引缓冲区失败.\n");
	//}

    RTASSERT(m_pEvent);
    return m_pEvent->OnPreCreate3DDevice();
}

bool RtgDevice::OnAfterCreate3DDevice()
{
    RTASSERT(m_pEvent);
    return m_pEvent->OnAfterCreate3DDevice();
}

bool RtgDevice::OnInit()
{
    RTASSERT(m_pEvent);
    return m_pEvent->OnDeviceInit();
}

void RtgDevice::OnClose()
{
    RTASSERT(m_pEvent);
    m_pEvent->OnDeviceClose();
}

RtgDeviceEvent* RtgDevice::GetEvent()
{
    return m_pEvent;
}

bool RtgDevice::OnRestoreDevice()
{
    CHECK(m_pEvent!=NULL);
    m_pVB->OnCreateStreamManager();
    //m_pIB->OnCreateStreamManager();
    //m_Texture.OnRestoreDevice();
    return m_pEvent->OnRestoreDevice();
}

bool RtgDevice::OnInvalidateDevice()
{
    CHECK(m_pEvent!=NULL);
    m_pVB->OnReleaseStreamManager();
    //m_pIB->OnReleaseStreamManager();
    //m_Texture.OnInvalidateDevice();
    return m_pEvent->OnInvalidateDevice();
}

void RtgDevice::SetMatrixFromCamera()
{
    // Camera
    CHECK(m_pCamera!=NULL);
    SetMatrix(RTGTS_VIEW, &(m_pCamera->m_matView));
    SetMatrix(RTGTS_PROJECTION, &(m_pCamera->m_matProj));
}

void RtgDevice::OnBeginRender()
{
    CHECK(m_pEvent!=NULL);
    //m_Texture.OnBeginRender();
    m_pVB->OnBeginRender();
    //m_pIB->OnBeginRender();
    m_pEvent->OnBeginRender();
}

void RtgDevice::OnEndRender()
{
    CHECK(m_pEvent!=NULL);
    m_pEvent->OnEndRender();
    m_pVB->OnEndRender();
    //m_pIB->OnEndRender();
    //m_Texture.OnEndRender();
}

void RtgDevice::OnRender()
{
    if (m_pEvent)
        m_pEvent->OnRender();
}

void RtgDevice::OnRender2D()
{
    if (m_pEvent)
	    m_pEvent->OnRender2D();
}

void RtgDevice::OnClearZRender()
{
    m_pEvent->OnClearZRender();
}

void RtgDevice::OnFrameMove(float fDifTime)
{
    m_pCamera->UpdateMatrix();
    SetMatrixFromCamera();

    if (m_pEvent)
        m_pEvent->OnFrameMove(fDifTime);
}

// 设置Camera，这里只是设置，没有负责删除
void RtgDevice::SetCamera(RtgCamera* pCamera)
{
    m_pCamera = pCamera;
  /*  CHECK(m_pCamera!=NULL);
    if (m_pCamera)
    {
        m_pCamera->SetDevice(this);
    }*/
}

void RtgDevice::SetEvent(RtgDeviceEvent* pEvent)
{
    m_pEvent = pEvent;
}

void RtgDevice::DrawPoint( RtgVertex3&  vPos, DWORD *pColor )
{
    RtgVertexVCT p;
    p.v = vPos;
    if ( pColor ) p.c = *pColor;
    else          p.c = m_dwCurrentColor;
    p.t.Set(0.0f,0.0f);

    this->SetShader(&m_nullShader);
    RTGVertexFormat eFVF = GetVertexFormat();
    SetVertexFormat(RTG_VCT);
    DrawPrimitiveUP(D3DPT_POINTLIST, &p, sizeof(RtgVertexVCT), 1);
    SetVertexFormat(eFVF);
    this->RestoreShader(&m_nullShader);
}

void RtgDevice::DrawLine( RtgVertex3&  vStart, RtgVertex3 &vEnd, DWORD *pColor )
{
    RtgVertexVC p[2];
    p[0].v = vStart;
    if (pColor) p[0].c = *pColor;
    else        p[0].c = m_dwCurrentColor;
    p[1] = p[0];
    p[1].v = vEnd;

	DWORD dwLighting = GetRenderState(RTGRS_LIGHTING);
	SetRenderState(RTGRS_LIGHTING, FALSE);
    this->SetShader(&m_nullShader);
    RTGVertexFormat eFVF = GetVertexFormat();
    SetVertexFormat(RTG_VC);
    DrawPrimitiveUP(D3DPT_LINELIST, &p, sizeof(RtgVertexVC), 1);
    SetVertexFormat(eFVF);
    this->RestoreShader(&m_nullShader);
	SetRenderState(RTGRS_LIGHTING, dwLighting);
}

void RtgDevice::DrawAxis( RtgMatrix16& m16, float fLen )
{
    RtgVertexVC p[6];
    p[0].v = p[2].v = p[4].v = m16.Position();
    p[0].c = RtgGetColor(255, 0, 0, 0);
    p[2].c = RtgGetColor(0, 255, 0, 0);
    p[4].c = RtgGetColor(0, 0, 255, 0);
    p[1] = p[0];
    p[3] = p[2];
    p[5] = p[4];

    RtgMatrix16 mm;
    mm = m16; mm.TranslateX(fLen);
    p[1].v  = mm.Position();
    mm = m16; mm.TranslateY(fLen);
    p[3].v  = mm.Position();
    mm = m16; mm.TranslateZ(fLen);
    p[5].v  = mm.Position();

    this->SetShader(&m_nullShader);
    RTGVertexFormat eFVF = GetVertexFormat();
    SetVertexFormat(RTG_VC);
    DrawPrimitiveUP(D3DPT_LINELIST, &p, sizeof(RtgVertexVC), 3);
    SetVertexFormat(eFVF);
    this->RestoreShader(&m_nullShader);
}

void RtgDevice::DrawAABBox(const RtgAABB* _aabbox, DWORD _color)
{
    RTASSERT(_aabbox);

    RtgVertexVC v[] = 
    {
        {RtgVertex3(_aabbox->vPos.x - _aabbox->vExt.x, _aabbox->vPos.y - _aabbox->vExt.y, _aabbox->vPos.z - _aabbox->vExt.z), _color},
        {RtgVertex3(_aabbox->vPos.x + _aabbox->vExt.x, _aabbox->vPos.y - _aabbox->vExt.y, _aabbox->vPos.z - _aabbox->vExt.z), _color},
        {RtgVertex3(_aabbox->vPos.x + _aabbox->vExt.x, _aabbox->vPos.y - _aabbox->vExt.y, _aabbox->vPos.z - _aabbox->vExt.z), _color},
        {RtgVertex3(_aabbox->vPos.x + _aabbox->vExt.x, _aabbox->vPos.y + _aabbox->vExt.y, _aabbox->vPos.z - _aabbox->vExt.z), _color},
        {RtgVertex3(_aabbox->vPos.x + _aabbox->vExt.x, _aabbox->vPos.y + _aabbox->vExt.y, _aabbox->vPos.z - _aabbox->vExt.z), _color},
        {RtgVertex3(_aabbox->vPos.x - _aabbox->vExt.x, _aabbox->vPos.y + _aabbox->vExt.y, _aabbox->vPos.z - _aabbox->vExt.z), _color},
        {RtgVertex3(_aabbox->vPos.x - _aabbox->vExt.x, _aabbox->vPos.y + _aabbox->vExt.y, _aabbox->vPos.z - _aabbox->vExt.z), _color},
        {RtgVertex3(_aabbox->vPos.x - _aabbox->vExt.x, _aabbox->vPos.y - _aabbox->vExt.y, _aabbox->vPos.z - _aabbox->vExt.z), _color},
        {RtgVertex3(_aabbox->vPos.x - _aabbox->vExt.x, _aabbox->vPos.y - _aabbox->vExt.y, _aabbox->vPos.z + _aabbox->vExt.z), _color},
        {RtgVertex3(_aabbox->vPos.x + _aabbox->vExt.x, _aabbox->vPos.y - _aabbox->vExt.y, _aabbox->vPos.z + _aabbox->vExt.z), _color},
        {RtgVertex3(_aabbox->vPos.x + _aabbox->vExt.x, _aabbox->vPos.y - _aabbox->vExt.y, _aabbox->vPos.z + _aabbox->vExt.z), _color},
        {RtgVertex3(_aabbox->vPos.x + _aabbox->vExt.x, _aabbox->vPos.y + _aabbox->vExt.y, _aabbox->vPos.z + _aabbox->vExt.z), _color},
        {RtgVertex3(_aabbox->vPos.x + _aabbox->vExt.x, _aabbox->vPos.y + _aabbox->vExt.y, _aabbox->vPos.z + _aabbox->vExt.z), _color},
        {RtgVertex3(_aabbox->vPos.x - _aabbox->vExt.x, _aabbox->vPos.y + _aabbox->vExt.y, _aabbox->vPos.z + _aabbox->vExt.z), _color},
        {RtgVertex3(_aabbox->vPos.x - _aabbox->vExt.x, _aabbox->vPos.y + _aabbox->vExt.y, _aabbox->vPos.z + _aabbox->vExt.z), _color},
        {RtgVertex3(_aabbox->vPos.x - _aabbox->vExt.x, _aabbox->vPos.y - _aabbox->vExt.y, _aabbox->vPos.z + _aabbox->vExt.z), _color},
        {RtgVertex3(_aabbox->vPos.x - _aabbox->vExt.x, _aabbox->vPos.y - _aabbox->vExt.y, _aabbox->vPos.z - _aabbox->vExt.z), _color},
        {RtgVertex3(_aabbox->vPos.x - _aabbox->vExt.x, _aabbox->vPos.y - _aabbox->vExt.y, _aabbox->vPos.z + _aabbox->vExt.z), _color},
        {RtgVertex3(_aabbox->vPos.x + _aabbox->vExt.x, _aabbox->vPos.y - _aabbox->vExt.y, _aabbox->vPos.z - _aabbox->vExt.z), _color},
        {RtgVertex3(_aabbox->vPos.x + _aabbox->vExt.x, _aabbox->vPos.y - _aabbox->vExt.y, _aabbox->vPos.z + _aabbox->vExt.z), _color},
        {RtgVertex3(_aabbox->vPos.x - _aabbox->vExt.x, _aabbox->vPos.y + _aabbox->vExt.y, _aabbox->vPos.z - _aabbox->vExt.z), _color},
        {RtgVertex3(_aabbox->vPos.x - _aabbox->vExt.x, _aabbox->vPos.y + _aabbox->vExt.y, _aabbox->vPos.z + _aabbox->vExt.z), _color},
        {RtgVertex3(_aabbox->vPos.x + _aabbox->vExt.x, _aabbox->vPos.y + _aabbox->vExt.y, _aabbox->vPos.z - _aabbox->vExt.z), _color},
        {RtgVertex3(_aabbox->vPos.x + _aabbox->vExt.x, _aabbox->vPos.y + _aabbox->vExt.y, _aabbox->vPos.z + _aabbox->vExt.z), _color}
    };

    DWORD dwLighting = GetRenderState(RTGRS_LIGHTING);
    SetRenderState(RTGRS_LIGHTING, FALSE);
    SetShader(&m_nullShader);
    SetFVF(D3DFVF_XYZ | D3DFVF_DIFFUSE);
    DrawPrimitiveUP(D3DPT_LINELIST, v, sizeof(RtgVertexVC), sizeof(v) / sizeof(RtgVertexVC) / 2);
    RestoreShader(&m_nullShader);
    SetRenderState(RTGRS_LIGHTING, dwLighting);

}

void RtgDevice::DrawBox( RtgVertex3& vHalfExt, RtgMatrix16& m16, DWORD *pColor )
{
    DWORD c;
    RtgVertex3 p[8];
    RtgVertexVC v[24];

    rtgGetBox(vHalfExt, m16, p);
    if (pColor) c = *pColor;
    else        c = m_dwCurrentColor;
    for (int i=0; i<24; i++)
    {
        v[i].c = c;
    }
    v[ 0].v = p[0];  v[ 1].v = p[1];
    v[ 2].v = p[1];  v[ 3].v = p[2];
    v[ 4].v = p[2];  v[ 5].v = p[3];
    v[ 6].v = p[3];  v[ 7].v = p[0];

    v[ 8].v = p[4];  v[ 9].v = p[5];
    v[10].v = p[5];  v[11].v = p[6];
    v[12].v = p[6];  v[13].v = p[7];
    v[14].v = p[7];  v[15].v = p[4];

    v[16].v = p[0];  v[17].v = p[4];
    v[18].v = p[1];  v[19].v = p[5];
    v[20].v = p[2];  v[21].v = p[6];
    v[22].v = p[3];  v[23].v = p[7];


    DWORD dwLighting = GetRenderState(RTGRS_LIGHTING);
    SetRenderState(RTGRS_LIGHTING, FALSE);
    SetShader(&m_nullShader);
    SetFVF(D3DFVF_XYZ | D3DFVF_DIFFUSE);
    DrawPrimitiveUP(D3DPT_LINELIST, &p, sizeof(RtgVertexVC), 12);
    RestoreShader(&m_nullShader);
    SetRenderState(RTGRS_LIGHTING, dwLighting);

    /*
    DrawLine(p[0], p[1], pColor);
    DrawLine(p[1], p[2], pColor);
    DrawLine(p[2], p[3], pColor);
    DrawLine(p[3], p[0], pColor);

    DrawLine(p[4], p[5], pColor);
    DrawLine(p[5], p[6], pColor);
    DrawLine(p[6], p[7], pColor);
    DrawLine(p[7], p[4], pColor);

    DrawLine(p[0], p[4], pColor);
    DrawLine(p[1], p[5], pColor);
    DrawLine(p[2], p[6], pColor);
    DrawLine(p[3], p[7], pColor);
    */
}

void RtgDevice::DrawBox( RtgVertex3& mMin, RtgVertex3& mMax, DWORD *pColor )
{
    RtgVertex3 center = 0.5f * (mMin + mMax);
    RtgVertex3 halfext = mMax - center;
    RtgMatrix16 ma; ma.Identity();
    ma.SetRow(3, center);
    DrawBox(halfext, ma, pColor);
}

void RtgDevice::DrawTriangle( RtgVertex3&  v1, RtgVertex3& v2, RtgVertex3& v3, DWORD *pColor )
{
    RtgVertexVCT p[3];
    p[0].v = v1;
    if (pColor) p[0].c = *pColor;
    else        p[0].c = m_dwCurrentColor;
    p[0].t.Set(0.0f,0.0f);
    p[2] = p[1] = p[0];
    p[1].v = v2;
    p[2].v = v3;

    RTGVertexFormat eFVF = GetVertexFormat();
    SetVertexFormat(RTG_VCT);
    DrawPrimitiveUP(D3DPT_TRIANGLELIST, &p, sizeof(RtgVertexVCT), 1);
    SetVertexFormat(eFVF);
}

RTGVertexFormat RtgDevice::GetVertexFormat()
{
    return m_eVertexFormat;
}

DWORD RtgDevice::GetVertexStride()
{
    return m_dwVertexStride;
}

void RtgDevice::SetVertexFormat(RTGVertexFormat eVF, const char* pVertexShader)
{
    m_eVertexFormat = eVF;
    OnSetFVF();
}

BOOL RtgDevice::SetDefaultMaterial(float fR, float fG, float fB, float fA)
{
    m_vMaterialDiffuse.Set(fR, fG, fB, fA);
    m_vMaterialAmbient.Set(fR, fG, fB, fA);
    m_vMaterialSpecular.Set(0.f);
    m_vMaterialEmissive.Set(0.f);
    return SetMaterial(m_vMaterialDiffuse, m_vMaterialAmbient, m_vMaterialSpecular, m_vMaterialEmissive, 0.f);
}

BOOL RtgDevice::SetHighlightPower(float fPower)
{
    return SetMaterial(m_vMaterialDiffuse, m_vMaterialAmbient, m_vMaterialSpecular, m_vMaterialEmissive, fPower);
}/*
BOOL RtgDevice::GetBackBufferRenderTarget2(DWORD dwRenderTargetRes,DWORD dwZStencilRes)
{
	unsigned int dwRederTargetIdx = m_Texture.FindIdx(dwRenderTargetRes);
	if (dwRederTargetIdx==0)
	{
#ifdef _DEBUG
		RtCoreLog().Warn("RtgDevice::SetRenderTarget 找不到ID为%d的贴图\n", dwRenderTargetRes);
#endif
		return FALSE;
	}

	if (dwRederTargetIdx < RtgTexture::IFL_TAG)
	{
		if ( dwRederTargetIdx<=0 || dwRederTargetIdx>RtgTexture::MAX_TEXTURES )
		{
			if (dwRederTargetIdx!=0)
			{
				RtCoreLog().Warn("错误: 贴图ID错误，不能找到相应的贴图.\n");
			}
			return FALSE;
		}else
		{
			 m_Texture.ActiveTexture(&(m_Texture.m_TextureList[dwRederTargetIdx-1]));
			if (!GetBackBufferRenderTarget(&(m_Texture.m_TextureList[dwRederTargetIdx-1]), NULL))
			{
				RtCoreLog().Warn("错误: GetBackBufferRenderTarget错误.可能是RenderTarget没有创建或者无效或者没有正确的创建.\n");
			}
		}
	}else
	{
		RtCoreLog().Warn("错误: GetBackBufferRenderTarget的贴图不能是IFL连续贴图.\n");
	}
	return TRUE;

}*//*
BOOL RtgDevice::SetRenderTarget2(DWORD dwRenderTargetRes, DWORD dwZStencilRes)
{
    unsigned int dwRederTargetIdx = m_Texture.FindIdx(dwRenderTargetRes);
    if (dwRederTargetIdx==0)
    {
#ifdef _DEBUG
        RtCoreLog().Warn("RtgDevice::SetRenderTarget 找不到ID为%d的贴图\n", dwRenderTargetRes);
#endif
        return FALSE;
    }

    if (dwRederTargetIdx < RtgTexture::IFL_TAG)
    {
        if ( dwRederTargetIdx<=0 || dwRederTargetIdx>RtgTexture::MAX_TEXTURES )
        {
            if (dwRederTargetIdx!=0)
            {
                RtCoreLog().Warn("错误: 贴图ID错误，不能找到相应的贴图.\n");
            }
            return FALSE;
        }else
        {
			m_ZStencilStack[m_iRenderTargetStackSize] = dwZStencilRes;
            m_RenderTargetStack[m_iRenderTargetStackSize] = dwRenderTargetRes;
			m_iRenderTargetStackSize ++;
            m_Texture.ActiveTexture(&(m_Texture.m_TextureList[dwRederTargetIdx-1]));
			_TexItem* pZStencil;
			if (dwZStencilRes==0 || dwZStencilRes==-1)
			{
				pZStencil = (_TexItem*)dwZStencilRes;
			}else
			{
                unsigned int dwZStencilIdx = m_Texture.FindIdx(dwZStencilRes);
                if (dwZStencilIdx)
                {
                    pZStencil = &(m_Texture.m_TextureList[dwZStencilIdx-1]);
                }else
                {
                    pZStencil = NULL;
                }
			}
            if (!SetRenderTarget(&(m_Texture.m_TextureList[dwRederTargetIdx-1]), pZStencil))
            {
                RtCoreLog().Warn("错误: SetRenderTarget错误.可能是RenderTarget没有创建或者无效或者没有正确的创建.\n");
            }
        }
    }else
    {
        RtCoreLog().Warn("错误: SetRenderTarget的贴图不能是IFL连续贴图.\n");
    }
    return TRUE;
}*/

//BOOL RtgDevice::RestoreRenderTarget()
//{
//	/*m_iRenderTargetStackSize--;
//	if(m_iRenderTargetStackSize > 0)
//    {
//        BOOL ret = SetRenderTarget2(m_RenderTargetStack[m_iRenderTargetStackSize-1], m_ZStencilStack[m_iRenderTargetStackSize-1]);
//		m_iRenderTargetStackSize--;
//		return ret;
//	}
//	else
//    {
//        return ResetDefaultRenderTarget();
//    }*/
//}
void RtgDevice::GetScreenPos(RtgVertex3 &vWorldPos, int &nOutX, int &nOutY, float &nOutZ)
{
	RtgVertex4 vPos(vWorldPos.x, vWorldPos.y, vWorldPos.z, 1.f);
	RtgVertex4 vOut = vPos * m_pCamera->m_matViewProj;
	nOutX = int( (float) m_iWndWidth   * (vOut.x / vOut.w*0.5f+0.5f));
	nOutY = int(((float)(m_iWndHeight))* (0.5f - vOut.y / vOut.w * 0.5f));
	nOutZ = vOut.z / vOut.w;
}
void RtgDevice::GetScreenPos(RtgVertex3 &vWorldPos, int &nOutX, int &nOutY)
{
    RtgVertex4 vPos(vWorldPos.x, vWorldPos.y, vWorldPos.z, 1.f);
    RtgVertex4 vOut = vPos * m_pCamera->m_matViewProj;
    nOutX = int( (float) m_iWndWidth   * (vOut.x / vOut.w*0.5f+0.5f));
    nOutY = int(((float)(m_iWndHeight))* (0.5f - vOut.y / vOut.w * 0.5f));
}

void RtgDevice::GetPickRayFromScreen(int sx, int sy, RtgVertex3 &vOrig, RtgVertex3 &vDir)
{
#if 1
    RtgMatrix16 matProj = m_pCamera->GetProjMatrix();

    // Compute the vector of the pick ray in screen space
    RtgVertex3 v;
    v.x =  ( ( ( 2.f * sx ) / m_iWndWidth  ) - 1.f ) / matProj._00;
    v.y = -( ( ( 2.f * sy ) / m_iWndHeight ) - 1.f ) / matProj._11;

#ifdef RTG_LEFT_HANDED
    v.z =  1.0f;
#else
    v.z =  -1.0f;
#endif

    // Get the inverse view matrix
    RtgMatrix16 m = m_pCamera->GetViewMatrix().Inverse();

    // Transform the screen space pick ray into 3D space
    vDir.x  = v.x*m._00 + v.y*m._10 + v.z*m._20;
    vDir.y  = v.x*m._01 + v.y*m._11 + v.z*m._21;
    vDir.z  = v.x*m._02 + v.y*m._12 + v.z*m._22;
    vDir.Normalize();
    vOrig.x = m._30;
    vOrig.y = m._31;
    vOrig.z = m._32;
#else
    RtgVertex3 vOut, v;
    RtgMatrix16 matInvProj;
    matInvProj = m_pCamera->m_matViewProj.Inverse();

    v.x = float(sx)/(float)m_iWndWidth-0.5f;
    v.x += v.x;
    v.y = 0.5f-float(sy)/((float)(m_iWndHeight));
    v.y += v.y;
    v.z = 0.0f;

    vOut = v * matInvProj;
    vOrig.Set(vOut.x/vOut.w,vOut.y/vOut.w,vOut.z/vOut.w);

    v.z = 1.0f;
    vOut = v * matInvProj;
    vDir.Set(vOut.x/vOut.w,vOut.y/vOut.w,vOut.z/vOut.w);

    // get dir
    vDir = vDir-vOrig;
    vDir.Normalize();
#endif
}

void Ava(BYTE *b1,BYTE *b2,BYTE *b3,BYTE *b4,BYTE *b5,
         BYTE *b6,BYTE *b7,BYTE *b8,BYTE *b9,BYTE *dest)
{
    for(int i=0; i<3; i++)
    {
        dest[i] = (b1[i] + b2[i] + b3[i] + b4[i] + b5[i]
        + b6[i] + b7[i] + b8[i] + b9[i])/9;
        // if(Abs(dest[i] - b1[i]) < 10)
        //	dest[i] = b1[i];
        long ret = dest[i]*1.1;
        if(ret > 255)
            ret = 255;
        dest[i] = ret;
        /*
        long ret = dest[i] + (dest[i]-128)*m_fPostProcessEffectContrast;
        if(ret > 255)
        ret = 255;
        if(ret < 0)
        ret = 0;
        dest[i] = ret;
        */
    }
}

void GetFinalPix(int i,int j,int w,int h,BYTE *b,BYTE *dest)
{
    BYTE *b1, *b2, *b3, *b4, *b5, *b6, *b7, *b8, *b9, *d;
    int o = i*w, c, ret;
    if(i==0 || j==0 || i==w-1 || j==h-1)
    {
        dest[(o+j)*4]   = b[(o+j)*4];
        dest[(o+j)*4+1] = b[(o+j)*4+1];
        dest[(o+j)*4+2] = b[(o+j)*4+2];
        dest[(o+j)*4+3] = b[(o+j)*4+3];
        return;
    }

    b1 = &b[(o+j)*4];
    b2 = &b[(o-w+j)*4];
    b3 = &b[(o+w+j)*4];
    b4 = &b[(i*w+j-1)*4];
    b5 = &b[(o+j+1)*4];
    b6 = &b[(o-w+j+1)*4];
    b7 = &b[(o-w+j-1)*4];
    b8 = &b[(o+w+j+1)*4];
    b9 = &b[(o+w+j-1)*4];
    d  = &dest[(o+j)*4];

    for(c=0; c<3; c++)
    {
        d[c] = (b1[c] + b2[c] + b3[c] + b4[c] + b5[c] + b6[c] + b7[c] + b8[c] + b9[c])/9;
        // if(Abs(dest[c] - b1[c]) < 10)
        //	d[c] = b1[c];
        ret = d[c]*1.1;
        d[c] = (ret>255) ? 255 : ret;
        /*
        ret = d[c] + (d[c]-128)*m_fPostProcessEffectContrast;
        if(ret > 255)
        ret = 255;
        if(ret < 0)
        ret = 0;
        d[c] = ret;
        */
    }
}

// ----------------------------------------------------------------------------
// gaussian blur
// take from dx9 sdk
// ----------------------------------------------------------------------------
inline float GaussianDistribution( float x, float y, float rho )
{
    float g = 1.0f / sqrtf( 2.0f * PI * rho * rho );
    g *= expf( -(x*x + y*y)/(2*rho*rho) );

    return g;
}

bool GetSampleOffsets_Bloom(DWORD dwD3DTexSize,float *afTexCoordOffset,
                            float *avColorWeight,float fDeviation, float fMultiplier )
{
    int i=0;
    float tu = 1.0f / (float)dwD3DTexSize;
    tu = 1.0f;

    // Fill the center texel
    float weight = 1.0f * GaussianDistribution( 0, 0, fDeviation );
    avColorWeight[0] = weight;

    afTexCoordOffset[0] = 0.0f;

    // Fill the right side
    for( i=1; i < 8; i++ )
    {
        weight = fMultiplier * GaussianDistribution( (float)i, 0, fDeviation );
        afTexCoordOffset[i] = i * tu;
        avColorWeight[i] = weight;
    }

    // Copy to the left side
    for( i=8; i < 15; i++ )
    {
        avColorWeight[i] = avColorWeight[i-7];
        afTexCoordOffset[i] = -afTexCoordOffset[i-7];
    }

    return true;
}

inline void GetPix(BYTE *src,int u,int v,BYTE *dest)
{
    if(u < 0) u = 0;
    if(v < 0) v = 0;
    if(u > 255) u = 255;
    if(v > 255) v = 255;

    dest[0] = src[(v*256 + u)*4+0];
    dest[1] = src[(v*256 + u)*4+1];
    dest[2] = src[(v*256 + u)*4+2];
    dest[3] = src[(v*256 + u)*4+3];
}

inline void GaussianBlur(BYTE *src,int u,int v,BYTE *d,float *offset,float *weight)
{
    BYTE tmp[4];
    float final[4];
    memset(tmp,0,4*sizeof(char));
    memset(final,0,4*sizeof(float));
    for(int i=0; i<15; i++)
    {
        GetPix(src, u+offset[i], v, tmp);
        final[0] += weight[i]*tmp[0];
        final[1] += weight[i]*tmp[1];
        final[2] += weight[i]*tmp[2];
        final[3] += weight[i]*tmp[3];
    }
    if(final[0]>255) final[0] = 255;
    if(final[0]<0) final[0] = 0;

    if(final[1]>255) final[1] = 255;
    if(final[1]<0) final[1] = 0;

    if(final[2]>255) final[2] = 255;
    if(final[2]<0) final[2] = 0;

    if(final[3]>255) final[3] = 255;
    if(final[3]<0) final[3] = 0;

    d[0] = final[0];
    d[1] = final[1];
    d[2] = final[2];
    d[3] = final[3];

    /*
    d[0] = src[(v*256 + u)*4 + 0];
    d[1] = src[(v*256 + u)*4 + 1];
    d[2] = src[(v*256 + u)*4 + 2];
    d[3] = src[(v*256 + u)*4 + 3];
    */
}





} // namespace rt_graph
/*----------------------------------------------------------------------------
-   The End.
----------------------------------------------------------------------------*/
