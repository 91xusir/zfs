
#ifdef USE_D3D8
#pragma message( "=======> Graphics Lib Use D3D8" )

#include <d3d8.h>
#include <MSDX8/D3dx8math.h>
#include "rtg_graph_inter.h"
#include "dx8/dxutil.h"
#include "dx8/d3dapp.h"
#include "dx8/d3dfont.h"
using namespace rt_graph_dx8;

#include "rtg_refreshrate.h"
#include "rtg_device_d3d8.h"
#include "rtg_buffer_manager_d3d8.h"
#include "dx8/d3dutil.h"
#include "rtg_i_util.h"

// USAGE     -> D3DUSAGE_WRITEONLY
// D3DFORMAT -> D3DFMT_INDEX16
// D3DPOOL   -> D3DPOOL_DEFAULT

namespace rt_graph {

#include "rtg_enum_mapping.h"

#define LOG_FUNNAME 0

#if LOG_FUNNAME
#   define  LOG_FUNCTION    RtCoreLog().Test("%s\n", __FUNCTION__)
#else
#   define  LOG_FUNCTION
#endif

/*----------------------------------------------------------------------
    class RtgDeviceD3D8
----------------------------------------------------------------------*/

RtgDevice* rtCreateDeviceD3D8()
{
    return RT_NEW RtgDeviceD3D8;
}

RtgDeviceD3D8::RtgDeviceD3D8()
{
    m_eDeviceType = RTG_DEVICE_D3D8;

    for (int i=0; i<RTGRS_MAX; i++)
    {
        m_listRenderState[i].bChanged = FALSE;
        m_listRenderState[i].dwValue = 0;
    }

    // 使用的坐标系
#ifdef RTG_LEFT_HANDED
    m_bLeftHanded = TRUE;
#else
    m_bLeftHanded = FALSE;
#endif
    if (m_bLeftHanded)  m_dwCullDefault                 = D3DCULL_CCW;
    else                m_dwCullDefault                 = D3DCULL_CW;

    // Depth Test
    m_bUseDepthBuffer = TRUE;

    // Font
    m_pFont = RT_NEW CD3DFont( _T("Courier New"), 10, D3DFONT_BOLD );

    m_dwVertexFVF = D3DFVF_XYZ;
    m_eLastVertexFormat = RTGVF_NULL;

    m_bCustomToggleFullscreen = false;
    m_hRenderWnd = NULL;
    m_hRenderFullWnd = NULL;
    m_pDefaultRenderTarget = NULL;
    m_texPostProcessID = 0;
    m_texPostProcessRenderTarget = 0;
    m_pDefaultDepthStencilSurface = NULL;
}

RtgDeviceD3D8::~RtgDeviceD3D8()
{
}

bool RtgDeviceD3D8::FullscreenToggle()
{
    if (FAILED(this->ToggleFullscreen()))
    {
        return false;
    }
    return true;
}

bool RtgDeviceD3D8::SetFullScreenInfo(DWORD dwWidth, DWORD dwHeight, DWORD dwDepth)
{
    return SetFullScreenMode(dwWidth, dwHeight, dwDepth);
}

bool RtgDeviceD3D8::ResetDevice(BOOL bFullscreen, DWORD dwWidth, DWORD dwHeight, DWORD dwDepth)
{
    RtCoreLog().Info("RtgDeviceD3D8: ResetDevice [%s] (%dx%dx%d)\n", bFullscreen?"Fullscreen":"Window", dwWidth, dwHeight, dwDepth);
    // Get access to current adapter, device, and mode
    D3DAdapterInfo* pAdapterInfo = &m_Adapters[m_dwAdapter];
    D3DDeviceInfo*  pDeviceInfo  = &pAdapterInfo->devices[pAdapterInfo->dwCurrentDevice];

    if (m_bWindowed!=bFullscreen && m_d3dpp.BackBufferWidth==dwWidth && m_d3dpp.BackBufferHeight==dwHeight)
    {
        if (!m_bWindowed)
        {
            DWORD BitDepth = 16;
            if( m_d3dpp.BackBufferFormat == D3DFMT_X8R8G8B8 || m_d3dpp.BackBufferFormat == D3DFMT_A8R8G8B8 || m_d3dpp.BackBufferFormat == D3DFMT_R8G8B8 )
            {
                BitDepth = 32;
            }
            if (BitDepth==dwDepth)
            {
                return true;
            }
        }else
        {
            return true;
        }
    }

    if (bFullscreen)
    {
        if (!this->SetFullScreenMode(dwWidth, dwHeight, dwDepth))
        {
            RtCoreLog().Warn("RtgDeviceD3D8: 试图切换到不支持的全屏模式(%dx%dx%d).\n", dwWidth, dwHeight, dwDepth);
            return false;
        }
    }else
    {
        // Need device change if going windowed and the current device
        // can only be fullscreen
        // 如果在全屏幕状态，并且不能切换到窗口状态，就强制切换
        if( !m_bWindowed && !pDeviceInfo->bCanDoWindowed )
        {
            return ForceWindowed()==S_OK;
        }
    }

    D3DModeInfo*    pModeInfo    = &pDeviceInfo->modes[pDeviceInfo->dwCurrentMode];
    m_bReady = FALSE;

    // Toggle the windowed state
    m_bWindowed = !bFullscreen;
    pDeviceInfo->bWindowed = m_bWindowed;

    // Prepare window for windowed/fullscreen change
    AdjustWindowForChange();

    if (!bFullscreen)
    {
        RECT rc;
        GetClientRect(m_hWnd, &rc);
        ClientToScreen(m_hWnd, (LPPOINT)&rc);
        rc.right = rc.left + dwWidth;
        rc.bottom = rc.top + dwHeight;
        AdjustWindowRect( &rc, m_dwWindowStyle, FALSE );
        if (rc.left<0)
        {
            rc.right -= rc.left;
            rc.left = 0;
        }
        if (rc.top<0)
        {
            rc.bottom -= rc.top;
            rc.top = 0;
        }
        MoveWindow(m_hWnd, rc.left, rc.top, rc.right - rc.left, rc.bottom - rc.top, true);

        // Update window properties
        GetWindowRect( m_hWnd, &m_rcWindowBounds );
        GetClientRect( m_hWnd, &m_rcWindowClient );
    }

    // Set up the presentation parameters
    m_d3dpp.Windowed               = pDeviceInfo->bWindowed;
    if( m_bWindowed )
        m_d3dpp.MultiSampleType    = pDeviceInfo->MultiSampleTypeWindowed;
    else
        m_d3dpp.MultiSampleType    = pDeviceInfo->MultiSampleTypeFullscreen;
    m_d3dpp.AutoDepthStencilFormat = pModeInfo->DepthStencilFormat;
    m_d3dpp.hDeviceWindow          = m_hWnd;
    if( m_bWindowed )
    {
        m_d3dpp.BackBufferWidth  = m_rcWindowClient.right - m_rcWindowClient.left;
        m_d3dpp.BackBufferHeight = m_rcWindowClient.bottom - m_rcWindowClient.top;
        m_d3dpp.BackBufferFormat = pAdapterInfo->d3ddmDesktop.Format;
    }
    else
    {
        m_d3dpp.BackBufferWidth  = pModeInfo->Width;
        m_d3dpp.BackBufferHeight = pModeInfo->Height;
        m_d3dpp.BackBufferFormat = pModeInfo->Format;
    }

    // Resize the 3D device
    if( FAILED( Resize3DEnvironment() ) )
    {
        if( m_bWindowed )
        {
            return ForceWindowed()==S_OK;
        }else
        {
            return false;
        }
    }

    // When moving from fullscreen to windowed mode, it is important to
    // adjust the window size after resetting the device rather than
    // beforehand to ensure that you get the window size you want.  For
    // example, when switching from 640x480 fullscreen to windowed with
    // a 1000x600 window on a 1024x768 desktop, it is impossible to set
    // the window size to 1000x600 until after the display mode has
    // changed to 1024x768, because windows cannot be larger than the
    // desktop.
    if( m_bWindowed )
    {
        SetWindowPos( m_hWnd, HWND_NOTOPMOST,
            m_rcWindowBounds.left, m_rcWindowBounds.top,
            ( m_rcWindowBounds.right - m_rcWindowBounds.left ),
            ( m_rcWindowBounds.bottom - m_rcWindowBounds.top ),
            SWP_SHOWWINDOW );
    }

    m_bReady = TRUE;

    return true;
}

void RtgDeviceD3D8::OnWindowSizeChanged()
{
    RtCoreLog().Info("RtgDeviceD3D8::OnWindowSizeChanged\n");
    HRESULT hr;
    RECT rcClientOld;
    rcClientOld = m_rcWindowClient;

    // Update window properties
    GetWindowRect( m_hWnd, &m_rcWindowBounds );
    GetClientRect( m_hWnd, &m_rcWindowClient );

    if( rcClientOld.right - rcClientOld.left !=
        m_rcWindowClient.right - m_rcWindowClient.left ||
        rcClientOld.bottom - rcClientOld.top !=
        m_rcWindowClient.bottom - m_rcWindowClient.top)
    {
        // A new window size will require a new backbuffer
        // size, so the 3D structures must be changed accordingly.
        m_bReady = FALSE;

        m_d3dpp.BackBufferWidth  = m_rcWindowClient.right - m_rcWindowClient.left;
        m_d3dpp.BackBufferHeight = m_rcWindowClient.bottom - m_rcWindowClient.top;

        D3DAdapterInfo* pAdapterInfo = &m_Adapters[m_dwAdapter];
        m_pD3D->GetAdapterDisplayMode( m_dwAdapter, &pAdapterInfo->d3ddmDesktop );
        m_d3dpp.BackBufferFormat = pAdapterInfo->d3ddmDesktop.Format;

        // Resize the 3D environment
        if( FAILED( hr = Resize3DEnvironment() ) )
        {
            DisplayErrorMsg( D3DAPPERR_RESIZEFAILED, MSGERR_APPMUSTEXIT );
            return;
        }

        m_bReady = TRUE;
    }
}

bool RtgDeviceD3D8::OnPreCreate3DDevice()
{
    m_pAppFrame = (CRtgAppFrame*)m_pEvent;
    RtCoreLog().Info("RtgDeviceD3D8::OnPreCreate3DDevice\n");
    m_eMaxMultiSampleTypeFullscreen = m_userConfig.dwMaxMultiSampleTypeFullscreen;
    m_eMaxMultiSampleTypeWindowed = m_userConfig.dwMaxMultiSampleTypeWindowed;
    m_pVB = RT_NEW D3DVertexStreamManager(this);
    m_pIB = RT_NEW D3DIndexStreamManager(this);
    //m_enumDisplayMode.EnumAll();
    RtgDevice::OnPreCreate3DDevice();
    return true;
}

bool RtgDeviceD3D8::OnCreate3DDevice()
{
    RtCoreLog().Info("RtgDeviceD3D8::OnCreate3DDevice\n");

    // 如果没有应用的实例，就取得当前应用的实例
    if( m_hAppInstance == NULL )
    {
        m_hAppInstance = (HINSTANCE)GetModuleHandle(NULL);
    }

    m_hRenderWnd = (HWND)m_ViewWnd[RTGVWM_WINDOWED].hWnd;
    m_hRenderFullWnd = (HWND)m_ViewWnd[RTGVWM_FULLSCREEN].hWnd;;

    CD3DApplication::m_hWnd = m_hRenderWnd;
    CD3DApplication::m_hWndFocus = m_hRenderFullWnd;

    // 默认创建窗口的参数，只有创建默认窗口才有效
    CD3DApplication::m_dwCreationWidth = m_userConfig.lWndWidth;
    CD3DApplication::m_dwCreationHeight = m_userConfig.lWndHeight;
    CD3DApplication::m_iDefaultStartX = m_config.iDefaultStartX;
    CD3DApplication::m_iDefaultStartY = m_config.iDefaultStartY;
    CD3DApplication::m_strWindowTitle = m_config.szWindowTitle;

    if (m_hRenderFullWnd)
    {
        m_bCustomToggleFullscreen = true;
    }
    // 如果前面没有窗口，就在这里创建窗口
    if (FAILED(CD3DApplication::Create((HINSTANCE)m_hAppInstance)))
    {
        RtCoreLog().Warn("ERROR: 创建D3D设备出错.\n");
        return false;
    }
    m_hRenderWnd = CD3DApplication::m_hWnd;
    m_hRenderFullWnd = CD3DApplication::m_hWndFocus;;
    m_ViewWnd[RTGVWM_WINDOWED].hWnd = m_hRenderWnd;
    m_ViewWnd[RTGVWM_TOPWINDOW].hWnd = m_hRenderWnd;
    m_ViewWnd[RTGVWM_FULLSCREEN].hWnd = m_hRenderFullWnd;
    return true;
}

void RtgDeviceD3D8::OnGetDeviceCaps()
{
    RtCoreLog().Info("RtgDeviceD3D8::OnGetDeviceCaps\n");

    m_Ability.bFullScreneAntialiasing   = !(m_d3dCaps.RasterCaps & D3DPRASTERCAPS_STRETCHBLTMULTISAMPLE);
    m_Ability.bVertexShader             = m_d3dCaps.VertexShaderVersion >= D3DVS_VERSION(1,1);
    m_Ability.bPixelShader              = m_d3dCaps.PixelShaderVersion >= D3DPS_VERSION(1,1);
    m_Ability.iMaxTextureWidth          = m_d3dCaps.MaxTextureWidth;
    m_Ability.iMaxTextureHeight         = m_d3dCaps.MaxTextureHeight;
    m_Ability.bSquareTexture            = (m_d3dCaps.TextureCaps & D3DPTEXTURECAPS_SQUAREONLY) ? true : false;
    m_Ability.iMaxMultiTextures         = m_d3dCaps.MaxSimultaneousTextures;
    m_Ability.iMaxTextureStages         = m_d3dCaps.MaxTextureBlendStages;
    m_Ability.bEnvBumpmap               = (m_d3dCaps.TextureOpCaps & D3DTOP_BUMPENVMAP) ? true : false;
    m_Ability.bDot3ProductBumpmap       = (m_d3dCaps.TextureOpCaps & D3DTEXOPCAPS_DOTPRODUCT3) ? true : false;
    m_Ability.bTextureOpMultiplyAdd     = (m_d3dCaps.TextureOpCaps & D3DTEXOPCAPS_MULTIPLYADD) ? true : false;
    m_Ability.bDynamicTextures          = (m_d3dCaps.Caps2 & D3DCAPS2_DYNAMICTEXTURES) ? true : false;

    m_Ability.iMaxPrimitiveCount        = m_d3dCaps.MaxPrimitiveCount;
    m_Ability.iMaxVertexIndex           = m_d3dCaps.MaxVertexIndex;
    m_Ability.iMaxStreams               = m_d3dCaps.MaxStreams;
    m_Ability.iMaxStreamStride          = m_d3dCaps.MaxStreamStride;

    m_Ability.bTextureAddressBorder     = (m_d3dCaps.TextureAddressCaps & D3DPTADDRESSCAPS_BORDER)?true:false;
    m_Ability.bTextureAddressClamp      = (m_d3dCaps.TextureAddressCaps & D3DPTADDRESSCAPS_CLAMP)?true:false;
    m_Ability.bTextureAddressWrap       = (m_d3dCaps.TextureAddressCaps & D3DPTADDRESSCAPS_WRAP)?true:false;
    m_Ability.bTextureMipmap            = (m_d3dCaps.TextureCaps & D3DPTEXTURECAPS_MIPMAP)?true:false;

    RtgImage::m_iMaxWidth = m_Ability.iMaxTextureWidth;
    RtgImage::m_iMaxHeight = m_Ability.iMaxTextureHeight;
    RtgImage::m_bSquare = m_Ability.bSquareTexture;
    RtgTexture::m_iMaxMultiTextures = m_Ability.iMaxMultiTextures;

    int i;
    for (i=0; i<m_Ability.iMaxMultiTextures; i++)
    {
        m_bTexture[i] = TRUE;
    }
    for (; i<8; i++)
    {
        m_bTexture[i] = FALSE;
    }

    // 输出
    RtCoreLog().Info("----- Device ------------------------------------------------------------\n");
    RtCoreLog().Info("  设备状态： %s\n", m_strDeviceStats);
    RtCoreLog().Info("  检查设备能力.\n");
    RtCoreLog().Info("    最大贴图大小 (%dx%d).\n", m_Ability.iMaxTextureWidth, m_Ability.iMaxTextureHeight);
    RtCoreLog().Info("    最多多层贴图数 %d.\n", m_Ability.iMaxMultiTextures);
    if (m_Ability.bSquareTexture)           RtCoreLog().Info("    贴图必须是方形的.\n");
    if (!m_Ability.bFullScreneAntialiasing) RtCoreLog().Info("    不支持全屏幕抗锯齿.\n");
    if (!m_Ability.bVertexShader)           RtCoreLog().Info("    不支持 Vertex Shader.\n");
    if (!m_Ability.bPixelShader)            RtCoreLog().Info("    不支持 Pixel Shader.\n");
    if (!m_Ability.bEnvBumpmap)             RtCoreLog().Info("    不支持 D3DTOP_BUMPENVMAP.\n");
    if (!m_Ability.bDot3ProductBumpmap)     RtCoreLog().Info("    不支持 D3DTEXOPCAPS_DOTPRODUCT3.\n");
    if (!m_Ability.bTextureOpMultiplyAdd)   RtCoreLog().Info("    不支持 D3DTEXOPCAPS_MULTIPLYADD.\n");
    if (!m_Ability.bDynamicTextures)        RtCoreLog().Info("    不支持 动态贴图.\n");
    if (!(m_Ability.bTextureAddressBorder)) RtCoreLog().Info("    不支持贴图Border模式.\n");
    if (!(m_Ability.bTextureAddressClamp))  RtCoreLog().Info("    不支持贴图Clamp模式.\n");
    if (!(m_Ability.bTextureAddressWrap))   RtCoreLog().Info("    不支持贴图Wrap模式.\n");
    if (!(m_Ability.bTextureMipmap))        RtCoreLog().Info("    不支持 D3DPTEXTURECAPS_MIPMAP.\n");
    if (!(m_d3dCaps.RasterCaps & D3DPRASTERCAPS_FOGRANGE))          RtCoreLog().Info("    不支持 Fog Range.\n");
    if (!(m_d3dCaps.RasterCaps & D3DPRASTERCAPS_FOGTABLE))          RtCoreLog().Info("    不支持 Fog Table.\n");
    if (!(m_d3dCaps.RasterCaps & D3DPRASTERCAPS_FOGVERTEX))         RtCoreLog().Info("    不支持 Fog Vertex.\n");

    if (!(m_d3dCaps.TextureFilterCaps & D3DPTFILTERCAPS_MAGFLINEAR))
    {
        RtCoreLog().Info("    不支持 D3DPTFILTERCAPS_MAGFLINEAR.\n");
        this->m_eTextureFilterMag = RTGTEXF_POINT;
        if (!(m_d3dCaps.TextureFilterCaps & D3DPTFILTERCAPS_MAGFPOINT))
        {
            RtCoreLog().Info("    不支持 D3DPTFILTERCAPS_MAGFPOINT.\n");
            this->m_eTextureFilterMag = RTGTEXF_NONE;
        }
    }
    if (!(m_d3dCaps.TextureFilterCaps & D3DPTFILTERCAPS_MINFLINEAR))
    {
        RtCoreLog().Info("    不支持 D3DPTFILTERCAPS_MINFLINEAR.\n");
        this->m_eTextureFilterMin = RTGTEXF_POINT;
        if (!(m_d3dCaps.TextureFilterCaps & D3DPTFILTERCAPS_MINFPOINT))
        {
            RtCoreLog().Info("    不支持 D3DPTFILTERCAPS_MINFPOINT.\n");
            this->m_eTextureFilterMin = RTGTEXF_NONE;
        }
    }
    if (!(m_d3dCaps.TextureFilterCaps & D3DPTFILTERCAPS_MIPFLINEAR))
    {
        RtCoreLog().Info("    不支持 D3DPTFILTERCAPS_MIPFLINEAR.\n");
        this->m_eTextureFilterMip = RTGTEXF_POINT;
        if (!(m_d3dCaps.TextureFilterCaps & D3DPTFILTERCAPS_MIPFPOINT))
        {
            RtCoreLog().Info("    不支持 D3DPTFILTERCAPS_MIPFPOINT.\n");
            this->m_eTextureFilterMip = RTGTEXF_NONE;
        }
    }
    // 输出结束
}

//-----------------------------------------------------------------------------
// Name: D3DFormatToString
// Desc: Returns the string for the given D3DFORMAT.
//-----------------------------------------------------------------------------
TCHAR* D3D8_D3DFormatToString( D3DFORMAT format, bool bWithPrefix )
{
    TCHAR* pstr = NULL;
    switch( format )
    {
    case D3DFMT_UNKNOWN:         pstr = TEXT("D3DFMT_UNKNOWN"); break;
    case D3DFMT_R8G8B8:          pstr = TEXT("D3DFMT_R8G8B8"); break;
    case D3DFMT_A8R8G8B8:        pstr = TEXT("D3DFMT_A8R8G8B8"); break;
    case D3DFMT_X8R8G8B8:        pstr = TEXT("D3DFMT_X8R8G8B8"); break;
    case D3DFMT_R5G6B5:          pstr = TEXT("D3DFMT_R5G6B5"); break;
    case D3DFMT_X1R5G5B5:        pstr = TEXT("D3DFMT_X1R5G5B5"); break;
    case D3DFMT_A1R5G5B5:        pstr = TEXT("D3DFMT_A1R5G5B5"); break;
    case D3DFMT_A4R4G4B4:        pstr = TEXT("D3DFMT_A4R4G4B4"); break;
    case D3DFMT_R3G3B2:          pstr = TEXT("D3DFMT_R3G3B2"); break;
    case D3DFMT_A8:              pstr = TEXT("D3DFMT_A8"); break;
    case D3DFMT_A8R3G3B2:        pstr = TEXT("D3DFMT_A8R3G3B2"); break;
    case D3DFMT_X4R4G4B4:        pstr = TEXT("D3DFMT_X4R4G4B4"); break;
    case D3DFMT_A2B10G10R10:     pstr = TEXT("D3DFMT_A2B10G10R10"); break;
    case D3DFMT_G16R16:          pstr = TEXT("D3DFMT_G16R16"); break;
    case D3DFMT_A8P8:            pstr = TEXT("D3DFMT_A8P8"); break;
    case D3DFMT_P8:              pstr = TEXT("D3DFMT_P8"); break;
    case D3DFMT_L8:              pstr = TEXT("D3DFMT_L8"); break;
    case D3DFMT_A8L8:            pstr = TEXT("D3DFMT_A8L8"); break;
    case D3DFMT_A4L4:            pstr = TEXT("D3DFMT_A4L4"); break;
    case D3DFMT_V8U8:            pstr = TEXT("D3DFMT_V8U8"); break;
    case D3DFMT_L6V5U5:          pstr = TEXT("D3DFMT_L6V5U5"); break;
    case D3DFMT_X8L8V8U8:        pstr = TEXT("D3DFMT_X8L8V8U8"); break;
    case D3DFMT_Q8W8V8U8:        pstr = TEXT("D3DFMT_Q8W8V8U8"); break;
    case D3DFMT_V16U16:          pstr = TEXT("D3DFMT_V16U16"); break;
    case D3DFMT_A2W10V10U10:     pstr = TEXT("D3DFMT_A2W10V10U10"); break;
    case D3DFMT_UYVY:            pstr = TEXT("D3DFMT_UYVY"); break;
    case D3DFMT_YUY2:            pstr = TEXT("D3DFMT_YUY2"); break;
    case D3DFMT_DXT1:            pstr = TEXT("D3DFMT_DXT1"); break;
    case D3DFMT_DXT2:            pstr = TEXT("D3DFMT_DXT2"); break;
    case D3DFMT_DXT3:            pstr = TEXT("D3DFMT_DXT3"); break;
    case D3DFMT_DXT4:            pstr = TEXT("D3DFMT_DXT4"); break;
    case D3DFMT_DXT5:            pstr = TEXT("D3DFMT_DXT5"); break;
    case D3DFMT_D16_LOCKABLE:    pstr = TEXT("D3DFMT_D16_LOCKABLE"); break;
    case D3DFMT_D32:             pstr = TEXT("D3DFMT_D32"); break;
    case D3DFMT_D15S1:           pstr = TEXT("D3DFMT_D15S1"); break;
    case D3DFMT_D24S8:           pstr = TEXT("D3DFMT_D24S8"); break;
    case D3DFMT_D24X8:           pstr = TEXT("D3DFMT_D24X8"); break;
    case D3DFMT_D24X4S4:         pstr = TEXT("D3DFMT_D24X4S4"); break;
    case D3DFMT_D16:             pstr = TEXT("D3DFMT_D16"); break;
    case D3DFMT_VERTEXDATA:      pstr = TEXT("D3DFMT_VERTEXDATA"); break;
    case D3DFMT_INDEX16:         pstr = TEXT("D3DFMT_INDEX16"); break;
    case D3DFMT_INDEX32:         pstr = TEXT("D3DFMT_INDEX32"); break;
    default:                     pstr = TEXT("Unknown format"); break;
    }
    if( bWithPrefix || _tcsstr( pstr, TEXT("D3DFMT_") )== NULL )
        return pstr;
    else
        return pstr + lstrlen( TEXT("D3DFMT_") );
}

bool RtgDeviceD3D8::OnAfterCreate3DDevice()
{
    RtCoreLog().Info("RtgDeviceD3D8::OnAfterCreate3DDevice\n");

    // 取得所有的RenderState默认值
    for (int i=0; i<RTGRS_MAX; i++)
    {
        m_pd3dDevice->GetRenderState(enumRTGRenderState[i], &(m_listRenderState[i].dwValue));
    }

    RtgDevice::OnAfterCreate3DDevice();
    return true;
}

bool RtgDeviceD3D8::OnInit()
{
    RtCoreLog().Info("RtgDeviceD3D8::OnInit\n");

    m_Texture.InitTexture();
    m_Texture.SetTextureQuality(m_userConfig.lTextureQuality);

    return RtgDevice::OnInit();
}

void RtgDeviceD3D8::OnClose()
{
    RtCoreLog().Info("RtgDeviceD3D8::OnClose\n");
    CD3DApplication::Cleanup3DEnvironment();
    DEL_ONE(m_pVB);
    DEL_ONE(m_pIB);
    RtgDevice::OnClose();
}

int RtgDeviceD3D8::OnRun()
{
    return CD3DApplication::Run();
}

void RtgDeviceD3D8::RenderScene()
{
    if( FAILED( Render3DEnvironment() ) )
    {
        RtCoreLog().Warn("RtgDeviceD3D8错误: 渲染错误.\n");
        SendMessage( m_hWnd, WM_CLOSE, 0, 0 );
    }
}

BOOL RtgDeviceD3D8::SetRenderState(RTGRenderState eState, DWORD dwValue)
{
	funguard;
    LOG_FUNCTION;
    CHECK(m_pd3dDevice!=0);
    if (m_listRenderState[eState].bChanged==TRUE && m_listRenderState[eState].dwValue==dwValue)
    {
        return TRUE;
    }
    m_listRenderState[eState].bChanged = TRUE;
    m_listRenderState[eState].dwValue = dwValue;
    switch (eState)
    {
    case RTGRS_COLOR_WRITE:
        if (dwValue) dwValue = D3DCOLORWRITEENABLE_ALPHA | D3DCOLORWRITEENABLE_BLUE | D3DCOLORWRITEENABLE_GREEN | D3DCOLORWRITEENABLE_RED;
        else         dwValue = 0x00;
        break;
    }
    return m_pd3dDevice->SetRenderState(enumRTGRenderState[eState], dwValue)==D3D_OK;
	fununguard;
}

DWORD RtgDeviceD3D8::GetRenderState(RTGRenderState eState)
{
	funguard;
    return m_listRenderState[eState].dwValue;
	fununguard;
}

BOOL RtgDeviceD3D8::SetLight(int iIdx, RtgLight* pLight)
{
	funguard;
    LOG_FUNCTION;
    if (iIdx<0 || iIdx>=(int)m_d3dCaps.MaxActiveLights) return FALSE;
    if (pLight==NULL)
    {
        return m_pd3dDevice->LightEnable( iIdx, FALSE )==D3D_OK;
    }

    D3DLIGHT8 d3dLight;
    memset( &d3dLight, 0, sizeof(d3dLight) );

    // Rotate through the various light types
    d3dLight.Type = enumRTGLightMode[pLight->eMode];

    // Make sure the light type is supported by the device.  If 
    // D3DVTXPCAPS_POSITIONALLIGHTS is not set, the device does not support 
    // point or spot lights, so change light #2's type to a directional light.
    DWORD dwCaps = m_d3dCaps.VertexProcessingCaps;
    if( 0 == ( dwCaps & D3DVTXPCAPS_POSITIONALLIGHTS ) )
    {
        if( d3dLight.Type == D3DLIGHT_POINT || d3dLight.Type == D3DLIGHT_SPOT )
            d3dLight.Type = D3DLIGHT_DIRECTIONAL;
    }

    d3dLight.Diffuse.r      = pLight->vDiffuse.x;
    d3dLight.Diffuse.g      = pLight->vDiffuse.y;
    d3dLight.Diffuse.b      = pLight->vDiffuse.z;
    d3dLight.Specular.r     = pLight->vSpecular.x;
    d3dLight.Specular.g     = pLight->vSpecular.y;
    d3dLight.Specular.b     = pLight->vSpecular.z;
    d3dLight.Range          = pLight->fRange;
    d3dLight.Attenuation0   = pLight->fConstantAttenuation;
    d3dLight.Attenuation1   = pLight->fLinearAttenuation;
    d3dLight.Attenuation2   = pLight->fQuadraticAttenuation;

    switch( d3dLight.Type )
    {
    case D3DLIGHT_POINT:
        d3dLight.Position.x   = pLight->vPosition.x;
        d3dLight.Position.y   = pLight->vPosition.y;
        d3dLight.Position.z   = pLight->vPosition.z;
        break;
    case D3DLIGHT_DIRECTIONAL:
        d3dLight.Direction.x  = pLight->vDirection.x;
        d3dLight.Direction.y  = pLight->vDirection.y;
        d3dLight.Direction.z  = pLight->vDirection.z;
        break;
    case D3DLIGHT_SPOT:
        d3dLight.Position.x   = pLight->vPosition.x;
        d3dLight.Position.y   = pLight->vPosition.y;
        d3dLight.Position.z   = pLight->vPosition.z;
        d3dLight.Direction.x  = pLight->vDirection.x;
        d3dLight.Direction.y  = pLight->vDirection.y;
        d3dLight.Direction.z  = pLight->vDirection.z;
        d3dLight.Theta        = pLight->fTheta;
        d3dLight.Phi          = pLight->fPhi;
        d3dLight.Falloff      = pLight->fFallOff;
        break;
    }

    m_pd3dDevice->LightEnable( iIdx, TRUE );
    return m_pd3dDevice->SetLight( iIdx, &d3dLight )==D3D_OK;
	fununguard;
}

BOOL RtgDeviceD3D8::OnSetFVF()
{
	funguard;
    LOG_FUNCTION;
    if (m_eLastVertexFormat==m_eVertexFormat)
    {
        return TRUE;
    }
    m_dwVertexFVF = D3DFVF_XYZ;
    switch (m_eVertexFormat)
    {
    case RTG_VNCT:
        m_dwVertexFVF |= D3DFVF_NORMAL|D3DFVF_DIFFUSE|D3DFVF_TEX1|D3DFVF_TEXCOORDSIZE2(0);
        break;
    case RTG_VNC2T:
        m_dwVertexFVF |= D3DFVF_NORMAL|D3DFVF_DIFFUSE|D3DFVF_SPECULAR|D3DFVF_TEX1|D3DFVF_TEXCOORDSIZE2(0);
        break;
    case RTG_VNCT2:
        m_dwVertexFVF |= D3DFVF_NORMAL|D3DFVF_DIFFUSE|D3DFVF_TEX2|D3DFVF_TEXCOORDSIZE2(0);
        break;
    case RTG_VNCT3:
        m_dwVertexFVF |= D3DFVF_NORMAL|D3DFVF_DIFFUSE|D3DFVF_TEX3|D3DFVF_TEXCOORDSIZE2(0);
        break;
    case RTG_VNCT4:
        m_dwVertexFVF |= D3DFVF_NORMAL|D3DFVF_DIFFUSE|D3DFVF_TEX4|D3DFVF_TEXCOORDSIZE2(0);
        break;
    case RTG_VNT:
        m_dwVertexFVF |= D3DFVF_NORMAL|D3DFVF_TEX1|D3DFVF_TEXCOORDSIZE2(0);
        break;
    case RTG_VNT2:
        m_dwVertexFVF |= D3DFVF_NORMAL|D3DFVF_TEX2|D3DFVF_TEXCOORDSIZE2(0);
        break;
    case RTG_VNT3:
        m_dwVertexFVF |= D3DFVF_NORMAL|D3DFVF_TEX3|D3DFVF_TEXCOORDSIZE2(0);
        break;
    case RTG_VNT4:
        m_dwVertexFVF |= D3DFVF_NORMAL|D3DFVF_TEX4|D3DFVF_TEXCOORDSIZE2(0);
        break;
    case RTG_VCT:
        m_dwVertexFVF |= D3DFVF_DIFFUSE|D3DFVF_TEX1|D3DFVF_TEXCOORDSIZE2(0);
        break;
    case RTG_VCT2:
        m_dwVertexFVF |= D3DFVF_DIFFUSE|D3DFVF_TEX2|D3DFVF_TEXCOORDSIZE2(0);
        break;
    case RTG_VCT3:
        m_dwVertexFVF |= D3DFVF_DIFFUSE|D3DFVF_TEX3|D3DFVF_TEXCOORDSIZE2(0);
        break;
    case RTG_VCT4:
        m_dwVertexFVF |= D3DFVF_DIFFUSE|D3DFVF_TEX4|D3DFVF_TEXCOORDSIZE2(0);
        break;
    case RTG_VN:
        m_dwVertexFVF |= D3DFVF_NORMAL;
        break;
    case RTG_VC:
        m_dwVertexFVF |= D3DFVF_DIFFUSE;
        break;
    case RTG_VT:
        m_dwVertexFVF |= D3DFVF_TEX1|D3DFVF_TEXCOORDSIZE2(0);
        break;
    case RTG_VT2:
        m_dwVertexFVF |= D3DFVF_TEX2|D3DFVF_TEXCOORDSIZE2(0);
        break;
    case RTG_TRANSFORMED_VC:
        m_dwVertexFVF = D3DFVF_XYZRHW | D3DFVF_DIFFUSE;
        break;
    case RTG_TRANSFORMED_VT:
        m_dwVertexFVF = D3DFVF_XYZRHW | D3DFVF_TEX1 | D3DFVF_TEXCOORDSIZE2(0);
        break;
    case RTG_TRANSFORMED_VT2:
        m_dwVertexFVF = D3DFVF_XYZRHW | D3DFVF_TEX2 | D3DFVF_TEXCOORDSIZE2(0);
        break;
    case RTG_TRANSFORMED_VCT:
        m_dwVertexFVF = D3DFVF_XYZRHW | D3DFVF_DIFFUSE | D3DFVF_TEX1 | D3DFVF_TEXCOORDSIZE2(0);
        break;
    case RTG_TRANSFORMED_VCT2:
        m_dwVertexFVF = D3DFVF_XYZRHW | D3DFVF_DIFFUSE | D3DFVF_TEX2 | D3DFVF_TEXCOORDSIZE2(0);
        break;
    case RTG_VERTEX_SHADER:
        CHECKEX("RtgDeviceD3D8: 目前不支持VertexShader");
        break;
    }
    m_dwVertexStride = 0;
    if (m_dwVertexFVF&D3DFVF_XYZ)     m_dwVertexStride += sizeof(float)*3;
    if (m_dwVertexFVF&D3DFVF_XYZRHW)  m_dwVertexStride += sizeof(float)*4;
    if (m_dwVertexFVF&D3DFVF_NORMAL)  m_dwVertexStride += sizeof(float)*3;
    if (m_dwVertexFVF&D3DFVF_DIFFUSE) m_dwVertexStride += sizeof(DWORD);
    if (m_dwVertexFVF&D3DFVF_SPECULAR)m_dwVertexStride += sizeof(DWORD);

    if ((m_dwVertexFVF&0x0F00)==D3DFVF_TEX1)      m_dwVertexStride += sizeof(float)*2*1;
    else if ((m_dwVertexFVF&0x0F00)==D3DFVF_TEX2) m_dwVertexStride += sizeof(float)*2*2;
    else if ((m_dwVertexFVF&0x0F00)==D3DFVF_TEX3) m_dwVertexStride += sizeof(float)*2*3;
    else if ((m_dwVertexFVF&0x0F00)==D3DFVF_TEX4) m_dwVertexStride += sizeof(float)*2*4;

    m_eLastVertexFormat = m_eVertexFormat;
    return m_pd3dDevice->SetVertexShader(m_dwVertexFVF)==D3D_OK;
	fununguard;
}

void RtgDeviceD3D8::SetMatrix(RTGMatrix eMode, RtgMatrix16 *pM16)
{
	funguard;
    CHECK(eMode!=RTGTS_TEXTURE);
    m_pd3dDevice->SetTransform( enumRTGMatrix[eMode], (D3DMATRIX*)pM16 );
	fununguard;
}

void RtgDeviceD3D8::GetMatrix(RTGMatrix eMode, RtgMatrix16 *pM16)
{
    CHECK(eMode!=RTGTS_TEXTURE);
    m_pd3dDevice->GetTransform( enumRTGMatrix[eMode], (D3DMATRIX*)pM16 );
}

void RtgDeviceD3D8::SetTextureMatrix(int iIdx, RtgMatrix16 *pM16)
{
    CHECK(iIdx>=0 && iIdx<m_Ability.iMaxMultiTextures);
    m_pd3dDevice->SetTransform( enumRTGMatrix[RTGTS_TEXTURE+iIdx], (D3DMATRIX*)pM16 );
}

void RtgDeviceD3D8::GetTextureMatrix(int iIdx, RtgMatrix16 *pM16)
{
    CHECK(iIdx>0 && iIdx<m_Ability.iMaxMultiTextures);
    m_pd3dDevice->GetTransform( enumRTGMatrix[RTGTS_TEXTURE+iIdx], (D3DMATRIX*)pM16 );
}

BOOL RtgDeviceD3D8::DrawPrimitive(RTGPrimitive eType, void *pVertices, UINT dwVerticesCount, UINT dwPrimitiveCount)
{
    LOG_FUNCTION;
    STAT(m_iLastRenderNumFaces+=dwPrimitiveCount);
    STAT(m_iLastRenderDrawPrimitive++);
    CHECK(pVertices!=NULL && dwVerticesCount!=0);
    return m_pd3dDevice->DrawPrimitiveUP(enumRTGPrimitive[eType], dwPrimitiveCount, pVertices, m_dwVertexStride)==D3D_OK;
}

BOOL RtgDeviceD3D8::DrawPrimitiveVB(RTGPrimitive eType, void *pVertices, UINT dwVertexStart, UINT dwVerticesCount, UINT dwPrimitiveCount)
{
    LOG_FUNCTION;
    STAT(m_iLastRenderNumFaces+=dwPrimitiveCount);
    STAT(m_iLastRenderDrawPrimitiveVB++);
    CHECK(pVertices!=NULL && dwVerticesCount!=0);
    m_pd3dDevice->SetStreamSource(0, (IDirect3DVertexBuffer8*)pVertices, m_dwVertexStride);
	return m_pd3dDevice->DrawPrimitive(enumRTGPrimitive[eType], dwVertexStart, dwPrimitiveCount)==D3D_OK;
}

BOOL RtgDeviceD3D8::DrawPrimitiveVB(RTGPrimitive eType, UINT dwVertexStart, UINT dwVerticesCount, UINT dwPrimitiveCount)
{
    LOG_FUNCTION;
    STAT(m_iLastRenderNumFaces+=dwPrimitiveCount);
    STAT(m_iLastRenderDrawPrimitiveVB++);
    CHECK(dwVerticesCount!=0);
    return m_pd3dDevice->DrawPrimitive(enumRTGPrimitive[eType], dwVertexStart+m_dwVertexBufferStartOffset, dwPrimitiveCount)==D3D_OK;
}

BOOL RtgDeviceD3D8::DrawIndexPrimitive(RTGPrimitive eType, void *pVertices, UINT dwVerticesCount, void *pIndices, UINT dwIndicesCount, UINT dwPrimitiveCount)
{
    LOG_FUNCTION;
    STAT(m_iLastRenderNumFaces+=dwPrimitiveCount);
    STAT(m_iLastRenderDrawIndexPrimitive++);
	if (dwPrimitiveCount==-1) dwPrimitiveCount = dwIndicesCount/3;
    CHECK(pVertices!=NULL && dwVerticesCount!=0 && dwIndicesCount!=0);
    return m_pd3dDevice->DrawIndexedPrimitiveUP(enumRTGPrimitive[eType], 0, dwVerticesCount, dwPrimitiveCount, (unsigned short*)pIndices, D3DFMT_INDEX16, pVertices, m_dwVertexStride)==D3D_OK;
}

BOOL RtgDeviceD3D8::DrawIndexPrimitiveVB(RTGPrimitive eType, void *pVertices, UINT dwVerticesCount, void *pIndices, UINT dwIndicesCount, UINT dwPrimitiveCount)
{
    LOG_FUNCTION;
    STAT(m_iLastRenderNumFaces+=dwPrimitiveCount);
    STAT(m_iLastRenderDrawIndexPrimitiveVB++);
    CHECK(pVertices!=NULL && dwVerticesCount!=0 && dwIndicesCount!=0);
	m_pd3dDevice->SetStreamSource(0, (IDirect3DVertexBuffer8*)pVertices, m_dwVertexStride);
	m_pd3dDevice->SetIndices((IDirect3DIndexBuffer8*)pIndices, 0);
	return m_pd3dDevice->DrawIndexedPrimitive(enumRTGPrimitive[eType], 0, dwVerticesCount, 0, dwPrimitiveCount)==D3D_OK;
}

BOOL RtgDeviceD3D8::DrawIndexPrimitiveVB(RTGPrimitive eType, void *pVertices, UINT dwVertexStart, UINT dwVerticesCount, void *pIndices, UINT dwIndexStart, UINT dwIndicesCount, UINT dwPrimitiveCount)
{
    LOG_FUNCTION;
    STAT(m_iLastRenderNumFaces+=dwPrimitiveCount);
    STAT(m_iLastRenderDrawIndexPrimitiveVB++);
    CHECK(pVertices!=NULL && dwVerticesCount!=0 && dwIndicesCount!=0);
	m_pd3dDevice->SetStreamSource(0, (IDirect3DVertexBuffer8*)pVertices, m_dwVertexStride);
	m_pd3dDevice->SetIndices((IDirect3DIndexBuffer8*)pIndices, 0);
	return m_pd3dDevice->DrawIndexedPrimitive(enumRTGPrimitive[eType], dwVertexStart, dwVerticesCount, dwIndexStart, dwPrimitiveCount)==D3D_OK;
}

BOOL RtgDeviceD3D8::DrawIndexPrimitiveVB(RTGPrimitive eType, UINT dwVertexStart, UINT dwVerticesCount, UINT dwIndexStart, UINT dwPrimitiveCount)
{
	funguard;
    LOG_FUNCTION;
    STAT(m_iLastRenderNumFaces+=dwPrimitiveCount);
    STAT(m_iLastRenderDrawIndexPrimitiveVB++);
    CHECK(dwVerticesCount!=0);
	return m_pd3dDevice->DrawIndexedPrimitive(enumRTGPrimitive[eType], 0/*dwVertexStart+m_dwVertexBufferStartOffset*/, dwVerticesCount, dwIndexStart+m_dwIndexBufferStartOffset, dwPrimitiveCount)==D3D_OK;
	fununguard;
}

BOOL RtgDeviceD3D8::SetStreamSource(UINT dwStreamNumber, void* pStreamData, UINT dwStride)
{
	return m_pd3dDevice->SetStreamSource(dwStreamNumber, (IDirect3DVertexBuffer8*)pStreamData, dwStride)==D3D_OK;
}

BOOL RtgDeviceD3D8::SetIndices(void* pIndexData)
{
    // 这里会使用 m_dwVertexBufferStartOffset, 所以在调用本函数之前一定要设置m_dwVertexBufferStartOffset的值
	return m_pd3dDevice->SetIndices((IDirect3DIndexBuffer8*)pIndexData, m_dwVertexBufferStartOffset)==D3D_OK;
}

BOOL RtgDeviceD3D8::CreateVertexBuffer(UINT dwLength, void** ppVertexBuffer, DWORD dwUsage)
{
    LOG_FUNCTION;
    DWORD dwD3DUsage = 0;
    if (dwUsage&RTGU_RENDERTARGET      ) dwD3DUsage |= D3DUSAGE_RENDERTARGET      ;
    if (dwUsage&RTGU_DEPTHSTENCIL      ) dwD3DUsage |= D3DUSAGE_DEPTHSTENCIL      ;
    if (dwUsage&RTGU_WRITEONLY         ) dwD3DUsage |= D3DUSAGE_WRITEONLY         ;
    if (dwUsage&RTGU_SOFTWAREPROCESSING) dwD3DUsage |= D3DUSAGE_SOFTWAREPROCESSING;
    if (dwUsage&RTGU_DONOTCLIP         ) dwD3DUsage |= D3DUSAGE_DONOTCLIP         ;
    if (dwUsage&RTGU_POINTS            ) dwD3DUsage |= D3DUSAGE_POINTS            ;
    if (dwUsage&RTGU_RTPATCHES         ) dwD3DUsage |= D3DUSAGE_RTPATCHES         ;
    if (dwUsage&RTGU_NPATCHES          ) dwD3DUsage |= D3DUSAGE_NPATCHES          ;
    if (dwUsage&RTGU_DYNAMIC           ) dwD3DUsage |= D3DUSAGE_DYNAMIC           ;
    return m_pd3dDevice->CreateVertexBuffer(dwLength, dwD3DUsage, 0, D3DPOOL_DEFAULT, (IDirect3DVertexBuffer8**)ppVertexBuffer)==D3D_OK;
}

BOOL RtgDeviceD3D8::LockVertexBuffer(void* pVertexBuffer, void** ppData, DWORD dwOffset, DWORD dwSize, RTGLock eLockFlag)
{
    LOG_FUNCTION;
    return ((LPDIRECT3DVERTEXBUFFER8)pVertexBuffer)->Lock(dwOffset, dwSize, (unsigned char**)ppData, enumRTGLock[eLockFlag])==D3D_OK;
}

BOOL RtgDeviceD3D8::UnLockVertexBuffer(void* pVertexBuffer)
{
    LOG_FUNCTION;
    return (((LPDIRECT3DVERTEXBUFFER8)pVertexBuffer)->Unlock()==D3D_OK);
}

BOOL RtgDeviceD3D8::ReleaseVertexBuffer(void* pVertexBuffer)
{
    LOG_FUNCTION;
    return (((LPDIRECT3DVERTEXBUFFER8)pVertexBuffer)->Release()==D3D_OK);
}

BOOL RtgDeviceD3D8::CreateIndexBuffer(UINT dwLength, void** ppIndexBuffer, DWORD dwUsage)
{
    LOG_FUNCTION;
    DWORD dwD3DUsage = 0;
    if (dwUsage&RTGU_RENDERTARGET      ) dwD3DUsage |= RTGU_RENDERTARGET      ;
    if (dwUsage&RTGU_DEPTHSTENCIL      ) dwD3DUsage |= RTGU_DEPTHSTENCIL      ;
    if (dwUsage&RTGU_WRITEONLY         ) dwD3DUsage |= RTGU_WRITEONLY         ;
    if (dwUsage&RTGU_SOFTWAREPROCESSING) dwD3DUsage |= RTGU_SOFTWAREPROCESSING;
    if (dwUsage&RTGU_DONOTCLIP         ) dwD3DUsage |= RTGU_DONOTCLIP         ;
    if (dwUsage&RTGU_POINTS            ) dwD3DUsage |= RTGU_POINTS            ;
    if (dwUsage&RTGU_RTPATCHES         ) dwD3DUsage |= RTGU_RTPATCHES         ;
    if (dwUsage&RTGU_NPATCHES          ) dwD3DUsage |= RTGU_NPATCHES          ;
    if (dwUsage&RTGU_DYNAMIC           ) dwD3DUsage |= RTGU_DYNAMIC           ;
    return m_pd3dDevice->CreateIndexBuffer(dwLength, dwD3DUsage, D3DFMT_INDEX16, D3DPOOL_DEFAULT, (IDirect3DIndexBuffer8**)ppIndexBuffer)==D3D_OK;
}

BOOL RtgDeviceD3D8::LockIndexBuffer(void* pIndexBuffer, void** ppData, DWORD dwOffset, DWORD dwSize, RTGLock eLockFlag)
{
    LOG_FUNCTION;
    return ((LPDIRECT3DINDEXBUFFER8)pIndexBuffer)->Lock(dwOffset, dwSize, (unsigned char**)ppData, enumRTGLock[eLockFlag])==D3D_OK;
}

BOOL RtgDeviceD3D8::UnLockIndexBuffer(void* pIndexBuffer)
{
    LOG_FUNCTION;
    return (((LPDIRECT3DINDEXBUFFER8)pIndexBuffer)->Unlock()==D3D_OK);
}

BOOL RtgDeviceD3D8::ReleaseIndexBuffer(void* pIndexBuffer)
{
    LOG_FUNCTION;
    return (((LPDIRECT3DINDEXBUFFER8)pIndexBuffer)->Release()==D3D_OK);
}

BOOL RtgDeviceD3D8::SelectTextureFormat(int iBits)
{
    int i;

    if (m_iTexNumFormats==0)
    {
        RtCoreLog().Warn("ERROR: 硬件不支持任何贴图格式.");
        return FALSE;
    }

    if (iBits>=24)
    {
        m_iTexNumBits = 32;
    }else
    {
        m_iTexNumBits = 16;
    }

    m_pTexAlphaFormat = NULL;
    m_pTexColorFormat = NULL;
    m_pTexKeyFormat   = NULL;

    // 查找Alpha的
    for (i=0; i<m_iTexNumFormats; i++)
    {
        if (m_iTexNumBits==m_TexFormat[i].m_iBits && m_TexFormat[i].m_iAlphaBits>1)
        {
            m_pTexAlphaFormat = m_TexFormat+i;
            break;
        }
    }

    // 查找ColorKey的
	for (i=0; i<m_iTexNumFormats; i++)
	{
		if (m_iTexNumBits==m_TexFormat[i].m_iBits && m_TexFormat[i].m_iAlphaBits==1)
        {
			m_pTexKeyFormat = m_TexFormat+i;
			break;
		}
	}

    // 查找Color的
	for (i=0; i<m_iTexNumFormats; i++)
	{
		if (m_iTexNumBits==m_TexFormat[i].m_iBits && m_TexFormat[i].m_iAlphaBits==0)
        {
			m_pTexColorFormat = m_TexFormat+i;
			break;
		}
	}

    // 如果没有任何Alpha贴图支持
    if (m_pTexAlphaFormat==NULL)
    {
        if (m_iTexNumBits==32)
        {
            return SelectTextureFormat(16);
        }
        RtCoreLog().Warn("ERROR: 硬件不支持任何Alpha贴图.");
        m_pTexKeyFormat = m_pTexColorFormat = m_pTexAlphaFormat = m_TexFormat;
        return FALSE;
    }

    // 如果没有颜色的，就用Alpha的替代
    if (m_pTexColorFormat==NULL)
    {
        m_pTexColorFormat = m_pTexAlphaFormat;
    }

    // 如果没有ColorKey的，就用Alpha的替代
    if (m_pTexKeyFormat==NULL)
    {
        m_pTexKeyFormat = m_pTexAlphaFormat;
    }
	return TRUE;
}

void RtgDeviceD3D8::SetTextureFilterMip(RTGTextureFilter eType)
{
    if (m_eTextureFilterMip!=eType)
    {
        m_eTextureFilterMip = eType;
        for (int i=0; i<this->m_Ability.iMaxMultiTextures; i++)
        {
            m_pd3dDevice->SetTextureStageState( i, D3DTSS_MIPFILTER, enumRTGTextureFilter[m_eTextureFilterMip]);
        }
    }
}

void RtgDeviceD3D8::SetTextureFilterMin(RTGTextureFilter eType)
{
    if (m_eTextureFilterMin!=eType)
    {
        m_eTextureFilterMin = eType;
        for (int i=0; i<this->m_Ability.iMaxMultiTextures; i++)
        {
            m_pd3dDevice->SetTextureStageState( i, D3DTSS_MINFILTER, enumRTGTextureFilter[m_eTextureFilterMin]);
        }
    }
}

void RtgDeviceD3D8::SetTextureFilterMag(RTGTextureFilter eType)
{
    if (m_eTextureFilterMag!=eType)
    {
        m_eTextureFilterMag = eType;
        for (int i=0; i<this->m_Ability.iMaxMultiTextures; i++)
        {
            m_pd3dDevice->SetTextureStageState( i, D3DTSS_MAGFILTER, enumRTGTextureFilter[m_eTextureFilterMag]);
        }
    }
}

void RtgDeviceD3D8::SetTextureAddress(RTGTextureAddress eType)
{
    if (m_eTextureAddress!=eType)
    {
        m_eTextureAddress = eType;
        for (int i=0; i<this->m_Ability.iMaxMultiTextures; i++)
        {
            m_pd3dDevice->SetTextureStageState( i, D3DTSS_ADDRESSU,  enumRTGTextureAddress[m_eTextureAddress]);
            m_pd3dDevice->SetTextureStageState( i, D3DTSS_ADDRESSV,  enumRTGTextureAddress[m_eTextureAddress] );
        }
    }
}

void RtgDeviceD3D8::SetTextureBorderColor(DWORD dwColor)
{
    if (m_dwTextureBorderColor!=dwColor)
    {
        m_dwTextureBorderColor = dwColor;
        for (int i=0; i<this->m_Ability.iMaxMultiTextures; i++)
        {
            m_pd3dDevice->SetTextureStageState( i, D3DTSS_BORDERCOLOR, dwColor );
        }
    }
}

void RtgDeviceD3D8::SetShader(RtgShader* pShader)
{
    LOG_FUNCTION;
    m_pLastShader = pShader;
    CHECK(pShader->iNumStage<=m_Ability.iMaxMultiTextures);
    if (pShader->bColorKey)
    {
        if (m_bAlphaTest!=TRUE)
        {
            m_bAlphaTest = TRUE;
	        m_pd3dDevice->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);
	        m_pd3dDevice->SetRenderState(D3DRS_ALPHAREF, (DWORD)0x00000001); // 0x00000080
            m_pd3dDevice->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATEREQUAL);
        }
    }else
    {
        if (m_bAlphaTest!=FALSE)
        {
            m_bAlphaTest = FALSE;
	        m_pd3dDevice->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);
	        //m_pd3dDevice->SetRenderState(D3DRS_ALPHAREF, (DWORD)0x00000001); // 0x00000001
        }
    }

    if ( pShader->bTwoSide )
    {
        if (m_bCullFace!=FALSE)
        {
            m_bCullFace = FALSE;
	        m_pd3dDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
        }
    }else
    {
        if (m_bCullFace!=TRUE)
        {
            m_bCullFace = TRUE;
            m_pd3dDevice->SetRenderState(D3DRS_CULLMODE, m_dwCullDefault);
        }
    }

    if ( pShader->bEnableBlendFunc )
    {
        if (m_bBlend!=TRUE)
        {
            m_bBlend = TRUE;
	        m_pd3dDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
        }
        if (m_eBlendSrcFunc!=pShader->eBlendSrcFunc)
        {
            m_eBlendSrcFunc = pShader->eBlendSrcFunc;
	        m_pd3dDevice->SetRenderState(D3DRS_SRCBLEND, enumRTGBlendFunc[pShader->eBlendSrcFunc]);
        }
        if (m_eBlendDestFunc!=pShader->eBlendDstFunc)
        {
            m_eBlendDestFunc = pShader->eBlendDstFunc;
	        m_pd3dDevice->SetRenderState(D3DRS_DESTBLEND, enumRTGBlendFunc[pShader->eBlendDstFunc]);
        }
    }else
    {
        if (m_bBlend!=FALSE)
        {
            m_bBlend = FALSE;
	        m_pd3dDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
        }
    }

    if ( pShader->dwTextureFactor!=m_dwTextureFactor )
    {
        m_dwTextureFactor = pShader->dwTextureFactor;
        m_pd3dDevice->SetRenderState(D3DRS_TEXTUREFACTOR, m_dwTextureFactor);
    }

    int i, iPasses;
    m_iTextureRequiredPasses = (pShader->iNumStage+m_Ability.iMaxMultiTextures-1)/m_Ability.iMaxMultiTextures;
    CHECK(m_iTextureRequiredPasses<=1);
    iPasses = (m_Ability.iMaxMultiTextures>pShader->iNumStage)?pShader->iNumStage:m_Ability.iMaxMultiTextures;
    for (i=0; i<iPasses; i++)
    {
        UseTextureStage(i, pShader->Shaders+i);
    }
    m_iTextureCurPass = i;
    for (; i<m_Ability.iMaxMultiTextures; i++)
    {
		m_pd3dDevice->SetTextureStageState(i, D3DTSS_COLOROP, D3DTOP_DISABLE);
    }
}

void RtgDeviceD3D8::UseTextureStage(int iLevel, RtgTexStage* pStage)
{
    if (pStage->bUseTextureMatrix)
    {
        if (m_bUseTextureMatrix[iLevel]!=TRUE || m_dwTexTransformFlag[iLevel]!=pStage->dwTexTransformFlag)
        {
            m_bUseTextureMatrix[iLevel] = TRUE;
            m_dwTexTransformFlag[iLevel] = pStage->dwTexTransformFlag;
            m_pd3dDevice->SetTextureStageState(iLevel, D3DTSS_TEXTURETRANSFORMFLAGS, pStage->dwTexTransformFlag);
            //m_pd3dDevice->SetTextureStageState(iLevel, D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_COUNT2);
        }
        SetTextureMatrix(iLevel, &pStage->mMatrix16);
    }else
    {
        if (m_bUseTextureMatrix[iLevel])
        {
            m_bUseTextureMatrix[iLevel] = FALSE;
            m_pd3dDevice->SetTextureStageState(iLevel, D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_DISABLE);
            SetTextureMatrix(iLevel, &m_mTextureDefaultMatrix);
        }
    }
    m_Texture.UseTexture(iLevel, pStage->GetTexture());
    if (pStage->eColorOp==RTGTOP_DISABLE)
    {
        m_pd3dDevice->SetTextureStageState(iLevel, D3DTSS_COLOROP, D3DTOP_DISABLE );
    }else
    {
        m_pd3dDevice->SetTextureStageState(iLevel, D3DTSS_COLOROP, enumRTGTextureOp[pStage->eColorOp]);
        m_pd3dDevice->SetTextureStageState(iLevel, D3DTSS_COLORARG1, enumRTGTexArg[pStage->eColorArg1]);
        m_pd3dDevice->SetTextureStageState(iLevel, D3DTSS_COLORARG2, enumRTGTexArg[pStage->eColorArg2]);
        m_pd3dDevice->SetTextureStageState(iLevel, D3DTSS_COLORARG0, enumRTGTexArg[pStage->eColorArg0]);

        if (pStage->eAlphaOp==RTGTOP_DISABLE)
        {
            m_pd3dDevice->SetTextureStageState(iLevel, D3DTSS_ALPHAOP, D3DTOP_DISABLE );
        }else
        {
            m_pd3dDevice->SetTextureStageState(iLevel, D3DTSS_ALPHAOP, enumRTGTextureOp[pStage->eAlphaOp]);
            m_pd3dDevice->SetTextureStageState(iLevel, D3DTSS_ALPHAARG1, enumRTGTexArg[pStage->eAlphaArg1]);
            m_pd3dDevice->SetTextureStageState(iLevel, D3DTSS_ALPHAARG2, enumRTGTexArg[pStage->eAlphaArg2]);
            m_pd3dDevice->SetTextureStageState(iLevel, D3DTSS_ALPHAARG0, enumRTGTexArg[pStage->eAlphaArg0]);
        }

        m_pd3dDevice->SetTextureStageState(iLevel, D3DTSS_RESULTARG, enumRTGTexArg[pStage->eResultArg]);

        if (pStage->eTexCoordIndex!=RTGTC_PASSTHRU)
        {
            m_pd3dDevice->SetTextureStageState(iLevel, D3DTSS_TEXCOORDINDEX, enumRTGTexCoor[pStage->eTexCoordIndex]);
        }else
        {
            m_pd3dDevice->SetTextureStageState(iLevel, D3DTSS_TEXCOORDINDEX, iLevel);
        }
    }
    //<< for test begin, must delete
    //m_pd3dDevice->SetTextureStageState( iLevel, D3DTSS_MINFILTER, D3DTEXF_ANISOTROPIC );
    //m_pd3dDevice->SetTextureStageState( iLevel, D3DTSS_MAGFILTER, D3DTEXF_ANISOTROPIC );
    //m_pd3dDevice->SetTextureStageState( iLevel, D3DTSS_MIPFILTER, D3DTEXF_ANISOTROPIC );
    //>> for test end
}

void RtgDeviceD3D8::RestoreShader(RtgShader* pShader)
{
    LOG_FUNCTION;
}

void RtgDeviceD3D8::DeleteShader(RtgShader* pShader)
{
    LOG_FUNCTION;
    for (int i=0; i<pShader->iNumStage; i++ )
    {
        m_Texture.DeleteTexture(pShader->Shaders[i].GetTexture());
        pShader->Shaders[i].SetTexture(0);
    }
}

BOOL RtgDeviceD3D8::CreateTextureByFile(_TexItem* pItem, const char* szTexFileName, int iMipMap)
{
    LOG_FUNCTION;
    RtgImage img;
    if (!img.Load(szTexFileName))
    {
        char szFileName[MAX_PATH];
        GetCurrentDirectory(MAX_PATH, szFileName);
        RtCoreLog().Warn("RtgDeviceD3D8: Can't open Texture [%s][%s]", szTexFileName, szFileName);
        return FALSE;
    }

    strncpy(pItem->szFileName, szTexFileName, MAX_TEX_FILE_NAME-1);
#ifdef _DEBUG
    if (strlen(szTexFileName)>=MAX_TEX_FILE_NAME)
    {
        RtCoreLog().Warn("RtgDeviceD3D8: [WARNING] 贴图名字过长(>=%d)，可能会丢失一些信息 (%s)\n", MAX_TEX_FILE_NAME, szTexFileName);
    }
#endif

    img.FixTextureQuality(pItem->bQualityManage);

    // Create Texture
    LPDIRECT3DTEXTURE8 pTex;
    int iWidth, iHeight;
    pItem->iWidth = iWidth = img.info.iWidth;
    pItem->iHeight = iHeight = img.info.iHeight;
    if (iMipMap==0) iMipMap = img.GetMipMapCount();
    pItem->iMipMapCount = iMipMap;
    if (m_pd3dDevice->CreateTexture( iWidth, iHeight, iMipMap, 0, enumRTGTextureFormat[m_pTexAlphaFormat->m_eFormat], D3DPOOL_MANAGED, &pTex)!=D3D_OK)
    {
        RtCoreLog().Warn("RtgDeviceD3D8错误: CreateTextureByFile调用CreateTexture失败.\n");
        return FALSE;
    }

    D3DLOCKED_RECT desc;
    for ( int i=0; i<iMipMap; i++ )
    {
        img.Resize(iWidth,iHeight);
        pTex->LockRect(i, &desc, NULL, D3DLOCK_NOSYSLOCK);
        m_pTexAlphaFormat->fill(iWidth, iHeight, desc.pBits, desc.Pitch, img.GetRawData(), iWidth*4);
        pTex->UnlockRect(i);
        if (iWidth>1) iWidth/=2;
        if (iHeight>1) iHeight/=2;
    }

    pItem->pTexture = pTex;
    return TRUE;
}

BOOL RtgDeviceD3D8::CreateTextureByImage(_TexItem* pItem, RtgImage& img)
{
    LOG_FUNCTION;
    LPDIRECT3DTEXTURE8 pTex;
    D3DFORMAT dwD3dFormat;
    int iWidth;
    int iHeight;
    D3DLOCKED_RECT desc;

    if (img.info.bDDS)
    {
        switch (img.info.iDxtcFormat)
        {
        case RtgImage::EPF_DXT1: dwD3dFormat = D3DFMT_DXT1; break;
        case RtgImage::EPF_DXT2: dwD3dFormat = D3DFMT_DXT2; break;
        case RtgImage::EPF_DXT3: dwD3dFormat = D3DFMT_DXT3; break;
        case RtgImage::EPF_DXT4: dwD3dFormat = D3DFMT_DXT4; break;
        default:
        case RtgImage::EPF_DXT5: dwD3dFormat = D3DFMT_DXT5; break;
        }
        if (m_pd3dDevice->CreateTexture( pItem->iWidth, pItem->iHeight, pItem->iMipMapCount, 0, dwD3dFormat, D3DPOOL_MANAGED, &pTex)!=D3D_OK)
        {
            RtCoreLog().Warn("RtgDeviceD3D8错误: CreateTextureByImage调用CreateTexture失败.\n");
            return FALSE;
        }

        CHECK(pTex->GetLevelCount()==pItem->iMipMapCount);
        pItem->pTexture = pTex;

        iWidth = pItem->iWidth;
        iHeight = pItem->iHeight;
        for ( int i=0; i<pItem->iMipMapCount; i++ )
        {
            img.Resize(iWidth,iHeight);
            pTex->LockRect(i, &desc, NULL, D3DLOCK_NOSYSLOCK);
            memcpy(desc.pBits, img.GetRawData(), img.GetDataSize());
            pTex->UnlockRect(i);
            if (iWidth>1) iWidth/=2;
            if (iHeight>1) iHeight/=2;
        }
    }else
    {
        TextureFormat* pTexFormat;
        switch (pItem->cFormat)
        {
        case 1:  pTexFormat = m_TexFormat+RTG_TEXTURE_A8R8G8B8; break;
        case 2:  pTexFormat = m_TexFormat+RTG_TEXTURE_A4R4G4B4; break;
        case 3:  pTexFormat = m_TexFormat+RTG_TEXTURE_A1R5G5B5; break;
        default:
            switch (pItem->cTexBitType)
            {
            default:
            case RTGTBT_ALPHA:    pTexFormat = m_pTexAlphaFormat; break;
            case RTGTBT_COLOR:    pTexFormat = m_pTexColorFormat; break;
            case RTGTBT_COLORKEY: pTexFormat = m_pTexKeyFormat;   break;
            }
            break;
        }
        dwD3dFormat = enumRTGTextureFormat[pTexFormat->m_eFormat];

        if (m_pd3dDevice->CreateTexture( pItem->iWidth, pItem->iHeight, pItem->iMipMapCount, 0, dwD3dFormat, D3DPOOL_MANAGED, &pTex)!=D3D_OK)
        {
            RtCoreLog().Warn("RtgDeviceD3D8错误: CreateTextureByImage调用CreateTexture失败.\n");
            return FALSE;
        }
        CHECK(pTex->GetLevelCount()==pItem->iMipMapCount);
        pItem->pTexture = pTex;

        iWidth = pItem->iWidth;
        iHeight = pItem->iHeight;
        for ( int i=0; i<pItem->iMipMapCount; i++ )
        {
            img.Resize(iWidth,iHeight);
            pTex->LockRect(i, &desc, NULL, D3DLOCK_NOSYSLOCK);
            pTexFormat->fill(iWidth, iHeight, desc.pBits, desc.Pitch, img.GetRawData(), iWidth*4);
            pTex->UnlockRect(i);
            if (iWidth>1) iWidth/=2;
            if (iHeight>1) iHeight/=2;
        }
    }
    return TRUE;
}

BOOL RtgDeviceD3D8::CreateTextureByMemoryCpy(_TexItem* pItem)
{
    CHECK(0);
    return TRUE;
}

BOOL RtgDeviceD3D8::CreateTexture(_TexItem* pItem, BOOL bDxManaged)
{
    LOG_FUNCTION;
    LPDIRECT3DTEXTURE8 pTex;
    D3DFORMAT dwD3dFormat;
    TextureFormat* pTexFormat;
    switch (pItem->cFormat)
    {
    case 1:  pTexFormat = m_TexFormat+RTG_TEXTURE_A8R8G8B8; break;
    case 2:  pTexFormat = m_TexFormat+RTG_TEXTURE_A4R4G4B4; break;
    case 3:  pTexFormat = m_TexFormat+RTG_TEXTURE_A1R5G5B5; break;
    default:
        switch (pItem->cTexBitType)
        {
        default:
        case RTGTBT_ALPHA:    pTexFormat = m_pTexAlphaFormat; break;
        case RTGTBT_COLOR:    pTexFormat = m_pTexColorFormat; break;
        case RTGTBT_COLORKEY: pTexFormat = m_pTexKeyFormat;   break;
        }
        break;
    }
    dwD3dFormat = enumRTGTextureFormat[pTexFormat->m_eFormat];
    if (m_pd3dDevice->CreateTexture( pItem->iWidth, pItem->iHeight, pItem->iMipMapCount, 0, dwD3dFormat, bDxManaged?D3DPOOL_MANAGED:D3DPOOL_DEFAULT, &pTex)!=D3D_OK)
    {
        RtCoreLog().Warn("RtgDeviceD3D8错误: CreateTexture调用失败.\n");
        return FALSE;
    }
    CHECK(pTex->GetLevelCount()==pItem->iMipMapCount);
    pItem->pTexture = pTex;
    return TRUE;
}

BOOL RtgDeviceD3D8::CreateTextureDynamic(_TexItem* pItem)
{
    if (!m_Ability.bDynamicTextures)
    {
        return CreateTexture(pItem, TRUE);
    }
    LOG_FUNCTION;
    LPDIRECT3DTEXTURE8 pTex;
    D3DFORMAT dwD3dFormat;
    TextureFormat* pTexFormat;
    switch (pItem->cFormat)
    {
    case 1:  pTexFormat = m_TexFormat+RTG_TEXTURE_A8R8G8B8; break;
    case 2:  pTexFormat = m_TexFormat+RTG_TEXTURE_A4R4G4B4; break;
    case 3:  pTexFormat = m_TexFormat+RTG_TEXTURE_A1R5G5B5; break;
    default:
        switch (pItem->cTexBitType)
        {
        default:
        case RTGTBT_ALPHA:    pTexFormat = m_pTexAlphaFormat; break;
        case RTGTBT_COLOR:    pTexFormat = m_pTexColorFormat; break;
        case RTGTBT_COLORKEY: pTexFormat = m_pTexKeyFormat;   break;
        }
        break;
    }
    dwD3dFormat = enumRTGTextureFormat[pTexFormat->m_eFormat];
    CHECK(pItem->iMipMapCount==1); // 动态贴图只能为1
    if (m_pd3dDevice->CreateTexture( pItem->iWidth, pItem->iHeight, 1, D3DUSAGE_DYNAMIC, dwD3dFormat, D3DPOOL_DEFAULT, &pTex)!=D3D_OK)
    {
        RtCoreLog().Warn("RtgDeviceD3D8错误: 创建动态贴图出错, 贴图格式为：%d, %d, %d\n", pItem->iWidth, pItem->iHeight, dwD3dFormat);
        return FALSE;
    }
    CHECK(pTex->GetLevelCount()==1);
    pItem->pTexture = pTex;
    return TRUE;
}

BOOL RtgDeviceD3D8::CreateTextureRenderTarget(_TexItem* pItem)
{
    LOG_FUNCTION;
    LPDIRECT3DTEXTURE8 pTex;
    CHECK(pItem->iMipMapCount==1); // RenderTarget只能为1
    //D3DFORMAT dwD3dFormat = enumRTGTextureFormat[m_pTexColorFormat->m_eFormat]; // RenderTarget只能为 D3DFMT_X1R5G5B5, D3DFMT_R5G6B5, D3DFMT_X8R8G8B8, and D3DFMT_A8R8G8B8
    D3DFORMAT dwD3dFormat = m_d3dsdBackBuffer.Format; // RenderTarget只能为 D3DFMT_X1R5G5B5, D3DFMT_R5G6B5, D3DFMT_X8R8G8B8, and D3DFMT_A8R8G8B8
	if (m_pd3dDevice->CreateTexture( pItem->iWidth, pItem->iHeight, 1, D3DUSAGE_RENDERTARGET, dwD3dFormat, D3DPOOL_DEFAULT, &pTex)!=D3D_OK)
    {
        RtCoreLog().Warn("RtgDeviceD3D8错误: CreateTextureRenderTarget调用CreateTexture失败.\n");
	    return FALSE;
    }
    if (pItem->bUseOffScreen)
    {
        //if (m_pd3dDevice->CreateOffscreenPlainSurface(pItem->iWidth, pItem->iHeight, dwD3dFormat, D3DPOOL_SYSTEMMEM, 
        //    (LPDIRECT3DSURFACE8*)&pItem->pOffScreen, NULL)!=D3D_OK)
        if (m_pd3dDevice->CreateImageSurface(pItem->iWidth, pItem->iHeight, dwD3dFormat, (LPDIRECT3DSURFACE8*)&pItem->pOffScreen))
        {
            RtCoreLog().Warn("RtgDeviceD3D8错误: CreateTextureRenderTarget调用CreateImageSurface失败.\n");
            return FALSE;
        }
    }
	CHECK(pTex->GetLevelCount()==1);
    pItem->pTexture = pTex;
    return TRUE;
}

BOOL RtgDeviceD3D8::CreateTextureDepthStencil(_TexItem* pItem)
{
    LOG_FUNCTION;
    LPDIRECT3DTEXTURE8 pTex;
    CHECK(pItem->iMipMapCount==1); // RenderTarget只能为1
    if (m_pd3dDevice->CreateTexture( pItem->iWidth, pItem->iHeight, 1, D3DUSAGE_DEPTHSTENCIL, enumRTGTextureFormat[m_pTexAlphaFormat->m_eFormat], D3DPOOL_DEFAULT, &pTex)!=D3D_OK)
    {
        RtCoreLog().Warn("RtgDeviceD3D8错误: CreateTextureDepthStencil调用CreateTexture失败.\n");
        return FALSE;
    }
    CHECK(pTex->GetLevelCount()==1);
    pItem->pTexture = pTex;
    return TRUE;
}

BOOL RtgDeviceD3D8::SetRenderTarget(_TexItem* pRenderTarget, _TexItem* pZStencil)
{
    if (pRenderTarget->iPathIdx!=-2)
    {
        CHECKEX("RtgDeviceD3D8::SetRenderTarget 传入的贴图不是RenderTarget.");
        return FALSE;
    }
    if (m_pDefaultRenderTarget==NULL)
    {
        if (m_pd3dDevice->GetRenderTarget(&m_pDefaultRenderTarget)!=D3D_OK)
        {
            CHECKEX("RtgDeviceD3D8: 不能取得RenderTarget.");
            return FALSE;
        }
        if (m_pd3dDevice->GetDepthStencilSurface(&m_pDefaultDepthStencilSurface)!=D3D_OK)
        {
            CHECKEX("RtgDeviceD3D8: 不能取得DepthStencilSurface.");
            return FALSE;
        }
        //if (pZStencil)
        //{
        //    CHECKEX("RtgDeviceD3D8: 目前不支持设置Depth Stencil Buffer.");
        //    return FALSE;
        //}
    }
    HRESULT hResult;
    IDirect3DSurface8* pNewZStencil;
    IDirect3DSurface8* pNewRenderTarget;
    if (pRenderTarget) hResult = ((LPDIRECT3DTEXTURE8)(pRenderTarget->pTexture))->GetSurfaceLevel(0, &pNewRenderTarget);
    else pNewRenderTarget = NULL;
    if (pZStencil==(_TexItem*)-1)
	{
		pNewZStencil = m_pDefaultDepthStencilSurface;
		m_pDefaultDepthStencilSurface->AddRef();
		/*
		if (m_pd3dDevice->GetDepthStencilSurface(&pNewZStencil)!=D3D_OK)
		{
            CHECKEX("RtgDeviceD3D8: 不能取得DepthStencilSurface.");
		}
		*/
	}else if (pZStencil)
	{
		 hResult = ((LPDIRECT3DTEXTURE8)(pZStencil->pTexture))->GetSurfaceLevel(0, &pNewZStencil);
	}else
	{
		pNewZStencil = NULL;
	}
    hResult = m_pd3dDevice->SetRenderTarget(pNewRenderTarget, pNewZStencil);
    if (hResult==D3D_OK)
    {
        if (pNewRenderTarget)
        {
            pNewRenderTarget->Release();
        }
        if (pNewZStencil)
        {
            pNewZStencil->Release();
        }
    }
    return hResult==D3D_OK;
    //return m_pd3dDevice->SetRenderTarget(pNewRenderTarget, pNewZStencil)==D3D_OK;
    // D3DERR_INVALIDCALL is returned if pRenderTarget or pNewZStencil are not NULL and invalid,
    //        or if the new depth buffer is smaller than the new or retained color buffer.
}

BOOL RtgDeviceD3D8::ResetDefaultRenderTarget()
{
    if (m_pDefaultRenderTarget)
    {
        BOOL bResult = m_pd3dDevice->SetRenderTarget(m_pDefaultRenderTarget, m_pDefaultDepthStencilSurface)==D3D_OK;
        if (!bResult)
        {
            CHECKEX("RtgDeviceD3D8: 不能恢复RenderTarget.");
        }else
        {
            m_pDefaultRenderTarget->Release();
            m_pDefaultDepthStencilSurface->Release();
        }
        return bResult;
    }
    return FALSE;
}

BOOL RtgDeviceD3D8::DeleteTexture(_TexItem* pItem)
{
    if (pItem)
    {
        if (pItem->pTexture)
        {
            LPDIRECT3DTEXTURE8 pTexture = (LPDIRECT3DTEXTURE8)pItem->pTexture;
            SAFE_RELEASE(pTexture);
            pItem->pTexture = NULL;
        }
        if (pItem->pOffScreen)
        {
            LPDIRECT3DSURFACE8 pOffScreen = (LPDIRECT3DSURFACE8)pItem->pOffScreen;
            SAFE_RELEASE(pOffScreen);
            pItem->pOffScreen = NULL;
        }
        return TRUE;
    }
    RtCoreLog().Warn("RtgDeviceD3D8错误: DeleteTexture调用失败.\n");
	return FALSE;
}

_TexItem* pLastTextureLockItem = NULL;
LPDIRECT3DSURFACE8 pTextureLockSurface = NULL;

BOOL RtgDeviceD3D8::TextureLock(_TexItem* pItem, RtgTexture::TexLock& texLock)
{
    D3DLOCKED_RECT desc;
    LPDIRECT3DTEXTURE8 pTex = (LPDIRECT3DTEXTURE8)pItem->pTexture;
    if (pItem->iPathIdx==-2)
    {
        if (!pItem->bUseOffScreen) return FALSE;
        LPDIRECT3DSURFACE8 pOffScreen = (LPDIRECT3DSURFACE8)pItem->pOffScreen;
        pLastTextureLockItem = pItem;
        pTex->GetSurfaceLevel(0,&pTextureLockSurface);
        int ret = m_pd3dDevice->CopyRects(pTextureLockSurface, NULL, 0, pOffScreen, NULL); // m_pd3dDevice->GetRenderTargetData(pTextureLockSurface, pOffScreen);
  		pOffScreen->LockRect(&desc,NULL,NULL);
    }
	else
    {
        if (pTex->LockRect(0, &desc, NULL, D3DLOCK_NOSYSLOCK)!=D3D_OK)
        {
            RtCoreLog().Warn("RtgDeviceD3D8错误: TextureLock调用LockRect失败.\n");
            return FALSE;
        }
    }
    texLock.dwTexID = pItem->dwIdx;
    texLock.iWidth  = pItem->iWidth;
    texLock.iHeight = pItem->iHeight;
    texLock.iPitch  = desc.Pitch;
    texLock.pBits   = desc.pBits;
    return TRUE;
}

void RtgDeviceD3D8::TextureUnlock(_TexItem* pItem, RtgTexture::TexLock& texLock)
{
    LPDIRECT3DTEXTURE8 pTex = (LPDIRECT3DTEXTURE8)pItem->pTexture;
    if (pItem->iPathIdx==-2)
    {
        if(!pItem->bUseOffScreen) return;
        LPDIRECT3DSURFACE8 pOffScreen = (LPDIRECT3DSURFACE8)pItem->pOffScreen;
        pOffScreen->UnlockRect();
        CHECK(pLastTextureLockItem==pItem);
        int ret = m_pd3dDevice->CopyRects(pOffScreen, NULL, 0, pTextureLockSurface, NULL);//m_pd3dDevice->UpdateSurface(pOffScreen,NULL,pTextureLockSurface,NULL);
   		pTextureLockSurface->Release();
    }else
    {
        pTex->UnlockRect(0);
    }
}

BOOL RtgDeviceD3D8::UpdateTexture(_TexItem* pItem, void *pBuffer)
{
    LOG_FUNCTION;
	D3DLOCKED_RECT desc;
	LPDIRECT3DTEXTURE8 pTex = (LPDIRECT3DTEXTURE8)pItem->pTexture;
	if (pTex->LockRect(0, &desc, NULL, D3DLOCK_NOSYSLOCK)!=D3D_OK)
    {
        RtCoreLog().Warn("RtgDeviceD3D8错误: UpdateTexture调用LockRect失败.\n");
        return FALSE;
    }
    TextureFormat* pTexFormat;
    switch (pItem->cFormat)
    {
    case 1:  pTexFormat = m_TexFormat+RTG_TEXTURE_A8R8G8B8; break;
    case 2:  pTexFormat = m_TexFormat+RTG_TEXTURE_A4R4G4B4; break;
    case 3:  pTexFormat = m_TexFormat+RTG_TEXTURE_A1R5G5B5; break;
    default:
        switch (pItem->cTexBitType)
        {
        default:
        case RTGTBT_ALPHA:    pTexFormat = m_pTexAlphaFormat; break;
        case RTGTBT_COLOR:    pTexFormat = m_pTexColorFormat; break;
        case RTGTBT_COLORKEY: pTexFormat = m_pTexKeyFormat;   break;
        }
        break;
    }
    //RtCoreLog().Info("RtgDeviceD3D8::UpdateTexture\n");
	pTexFormat->fill(pItem->iWidth, pItem->iHeight, desc.pBits, desc.Pitch, pBuffer, pItem->iWidth*4);
	pTex->UnlockRect(0);
	return TRUE;
}

BOOL RtgDeviceD3D8::UpdateTextureDynamic(_TexItem* pItem, void *pBuffer)
{
    if (!m_Ability.bDynamicTextures)
    {
        return UpdateTexture(pItem, pBuffer);
    }
    LOG_FUNCTION;
    D3DLOCKED_RECT desc;
    LPDIRECT3DTEXTURE8 pTex = (LPDIRECT3DTEXTURE8)pItem->pTexture;
    if (pTex->LockRect(0, &desc, NULL, D3DLOCK_DISCARD)!=D3D_OK)
    {
        RtCoreLog().Warn("RtgDeviceD3D8错误: UpdateTextureDynamic调用LockRect失败.\n");
        return FALSE;
    }
    TextureFormat* pTexFormat;
    switch (pItem->cFormat)
    {
    case 1:  pTexFormat = m_TexFormat+RTG_TEXTURE_A8R8G8B8; break;
    case 2:  pTexFormat = m_TexFormat+RTG_TEXTURE_A4R4G4B4; break;
    case 3:  pTexFormat = m_TexFormat+RTG_TEXTURE_A1R5G5B5; break;
    default:
        switch (pItem->cTexBitType)
        {
        default:
        case RTGTBT_ALPHA:    pTexFormat = m_pTexAlphaFormat; break;
        case RTGTBT_COLOR:    pTexFormat = m_pTexColorFormat; break;
        case RTGTBT_COLORKEY: pTexFormat = m_pTexKeyFormat;   break;
        }
        break;
    }
    //RtCoreLog().Info("RtgDeviceD3D8::UpdateTextureDynamic\n");
    pTexFormat->fill(pItem->iWidth, pItem->iHeight, desc.pBits, desc.Pitch, pBuffer, pItem->iWidth*4);
    pTex->UnlockRect(0);
    return TRUE;
}

BOOL RtgDeviceD3D8::UseTexture(int iTextureStage, _TexItem* pItem)
{
    LOG_FUNCTION;
    if (pItem==NULL) return m_pd3dDevice->SetTexture(iTextureStage, NULL)==D3D_OK;
    STAT(m_iLastRenderUseTexture++);
    return m_pd3dDevice->SetTexture(iTextureStage, (LPDIRECT3DTEXTURE8)pItem->pTexture)==D3D_OK;
}

BOOL RtgDeviceD3D8::ClearRenderTarget(bool bStencil, bool bTarget, bool bZBuffer, DWORD dwColor, float fNewZ, DWORD dwStencil)
{
    HRESULT hr;
    DWORD dwFlag = 0;
    if (bStencil) dwFlag |= D3DCLEAR_STENCIL;
    if (bTarget)  dwFlag |= D3DCLEAR_TARGET;
    if (bZBuffer)
	{
		if (m_iRenderTargetStackSize>0)
		{
			if (m_ZStencilStack[m_iRenderTargetStackSize-1]!=0)
			{
				dwFlag |= D3DCLEAR_ZBUFFER;
			}
		}else
		{
			dwFlag |= D3DCLEAR_ZBUFFER;
		}
	}
    if (dwFlag)
    {
        if(FAILED( hr=m_pd3dDevice->Clear( 0L, NULL, dwFlag, dwColor, fNewZ, dwStencil)))
        {
            return DisplayErrorMsg(hr, MSGERR_APPMUSTEXIT);
        }
    }
    return TRUE;
}

void RtgDeviceD3D8::LockFps(DWORD dwMillisecondPerFrame)
{
    LockFpsPerFrame(dwMillisecondPerFrame);
}

HRESULT RtgDeviceD3D8::FrameMove()
{
    this->OnFrameMove(m_fElapsedTime);
    return S_OK;
}

void RtgDeviceD3D8::UpdateAppTime()
{
    IUpdateAppTime();
}

float RtgDeviceD3D8::GetElapsedTime()
{
    return m_fElapsedTime;
}

float RtgDeviceD3D8::GetAppTime()
{
    return m_fTime;
}

BOOL RtgDeviceD3D8::BeginScene()
{
    return SUCCEEDED( m_pd3dDevice->BeginScene() );
}

void RtgDeviceD3D8::EndScene()
{
    m_pd3dDevice->EndScene();
}

void RtgDeviceD3D8::EndPostProcess()
{
    if (m_bCurFrameUsePostProcessEffect)
    {
        RtgShader shader;
        shader.bTwoSide = true;
        shader.iNumStage = 1;
        shader.Shaders[0].SetTexture(m_texPostProcessID);

        RECT rt;
        rt.left   = 0;
        rt.top    = 0;
        rt.bottom = m_d3dsdBackBuffer.Height-1;
        rt.right  = m_d3dsdBackBuffer.Width-1;

        //////////////////////////////////////////////////////////////////////////
        // 恢复RenderTarget，然后把小图渲染上去

        RtgNVertexVCT p[4];
        DWORD dwColor = 0xFFFFFFFF;
        BOOL bBakFog = this->GetFogEnable();
        this->SetFogEnable(FALSE);
        this->SetShader(&shader);
        this->SetVertexFormat(RTG_TRANSFORMED_VCT);

        // RestoreRenderTarget(); 
        ResetDefaultRenderTarget();

        p[0] = RtgNVertexVCT( rt.left,  rt.top,    dwColor, 0, 0);
        p[1] = RtgNVertexVCT( rt.left,  rt.bottom, dwColor, 0, 1);
        p[2] = RtgNVertexVCT( rt.right, rt.bottom, dwColor, 1, 1);
        p[3] = RtgNVertexVCT( rt.right, rt.top,    dwColor, 1, 0);

        this->SetTextureFilterMag(RTGTEXF_POINT);
        this->SetTextureFilterMin(RTGTEXF_POINT);
        this->DrawPrimitive(RTG_TRIANGLE_FAN, &p, 4, 2);
        /*
        IDirect3DSurface8 *src,*dest;
        if (((IDirect3DTexture8 *)(m_Texture.m_TextureList[m_texPostProcessID-1]).pTexture)->GetSurfaceLevel(0,&src)!=D3D_OK)
        {
        int iiiiiiiiiiii =0 ;
        }
        if (m_pd3dDevice->GetRenderTarget(&dest)!=D3D_OK)
        {
        int iiiiiiiiiiii =0 ;
        }
        // StretchRect(src,NULL,dest,NULL,D3DTEXF_NONE);
        HRESULT hrCopyRects = m_pd3dDevice->CopyRects(src, NULL, 1, dest, NULL);
        if (hrCopyRects!=D3D_OK)
        {
        int iiiiiiiiiiii =0 ;
        if (hrCopyRects==D3DERR_DEVICELOST)
        {
        iiiiiiiiiiii = 1;
        }else if (hrCopyRects==D3DERR_INVALIDCALL)
        {
        iiiiiiiiiiii = 2;
        }
        }
        src->Release();
        dest->Release();
        */

        //////////////////////////////////////////////////////////////////////////
        // 渲染整张图到小的图上，然后取出来模糊化

        RtgDevice::SetRenderTarget(m_texPostProcessRenderTarget, 0);
        p[0] = RtgNVertexVCT( 0,  0,    dwColor, 0, 0);
        p[1] = RtgNVertexVCT( 0,  255,  dwColor, 0, 1);
        p[2] = RtgNVertexVCT( 255, 255, dwColor, 1, 1);
        p[3] = RtgNVertexVCT( 255, 0,   dwColor, 1, 0);

        this->SetTextureFilterMag(RTGTEXF_LINEAR);
        this->SetTextureFilterMin(RTGTEXF_LINEAR);
        this->DrawPrimitive(RTG_TRIANGLE_FAN, &p, 4, 2);

        p[0] = RtgNVertexVCT( rt.left,  rt.top,    dwColor, 0, 0);
        p[1] = RtgNVertexVCT( rt.left,  rt.bottom, dwColor, 0, 1);
        p[2] = RtgNVertexVCT( rt.right, rt.bottom, dwColor, 1, 1);
        p[3] = RtgNVertexVCT( rt.right, rt.top,    dwColor, 1, 0);

        //////////////////////////////////////////////////////////////////////////
        // 恢复RenderTarget，然后把小图渲染上去

        ResetDefaultRenderTarget();

        switch (m_iPostProcessEffectType)
        {
        case 0:
            {
                // blur and update
                //RtgDevice::SetRenderTarget(m_texPostProcessRenderTarget, 0);
                //SaveRenderTargetToFile("d:\\t1.bmp");
                //ResetDefaultRenderTarget();
                Blur(m_texPostProcessRenderTarget);
                //RtgDevice::SetRenderTarget(m_texPostProcessRenderTarget, 0);
                //SaveRenderTargetToFile("d:\\t2.bmp");
                //ResetDefaultRenderTarget();
                shader.Shaders[0].SetTexture(m_texPostProcessRenderTarget);
                shader.bEnableBlendFunc = true;
                shader.eBlendDstFunc = RTGBLEND_ONE;
                shader.eBlendSrcFunc = RTGBLEND_SRCALPHA;
                shader.dwTextureFactor = RtgVectorToColor(RtgVertex3(0.5,0.5,0.5),m_fPostProcessEffectIntensity);
                shader.Shaders[0].eColorOp = RTGTOP_SELECTARG1;
                shader.Shaders[0].eColorArg1 = RTGTA_TEXTURE;
                shader.Shaders[0].eAlphaOp = RTGTOP_SELECTARG1;
                shader.Shaders[0].eAlphaArg1 = RTGTA_TFACTOR;
                this->SetShader(&shader);
                this->DrawPrimitive(RTG_TRIANGLE_FAN, &p, 4, 2);
            }
            break;
        case 1:
            {
                // Gray
                Gray(m_texPostProcessRenderTarget, true);
                shader.Shaders[0].SetTexture(m_texPostProcessRenderTarget);
                shader.bEnableBlendFunc = true;
                shader.eBlendDstFunc = RTGBLEND_ONE;
                shader.eBlendSrcFunc = RTGBLEND_ONE;
                shader.Shaders[0].eColorOp = RTGTOP_SELECTARG1;
                shader.Shaders[0].eColorArg1 = RTGTA_TEXTURE;
                this->SetShader(&shader);
                this->DrawPrimitive(RTG_TRIANGLE_FAN, &p, 4, 2);

                Gray(m_texPostProcessRenderTarget, false);
                this->SetRenderState(RTGRS_BLENDOP, RTGBOP_REVSUBTRACT);
                this->SetShader(&shader);
                this->DrawPrimitive(RTG_TRIANGLE_FAN, &p, 4, 2);
                this->SetRenderState(RTGRS_BLENDOP, RTGBOP_ADD);
            }
            break;
        }
        this->SetFogEnable(bBakFog);
        RestoreRenderTarget(); 
        RestoreRenderTarget(); 
    }
}

HRESULT RtgDeviceD3D8::Render()
{
#if LOG_FUNNAME
    RtCoreLog().Info("\n------- Begin Render\n");
#endif
    HRESULT hr;

    // Begin the scene
    if( SUCCEEDED( m_pd3dDevice->BeginScene() ) )
    {
        STAT(m_iLastRenderNumFaces=0);
        STAT(m_iLastRenderNumClusters=0);
        STAT(m_iLastRenderUseTexture=0);
        STAT(m_iLastRenderDrawPrimitive=0);
        STAT(m_iLastRenderDrawPrimitiveVB=0);
        STAT(m_iLastRenderDrawIndexPrimitive=0);
        STAT(m_iLastRenderDrawIndexPrimitiveVB=0);
        STAT(m_pVB->m_dwStatDynamicSend=0);
        STAT(m_pVB->m_dwStatDynamicClear=0);
        STAT(m_pIB->m_dwStatDynamicSend=0);
        STAT(m_pIB->m_dwStatDynamicClear=0);

        m_bCurFrameUsePostProcessEffect = (m_bUsePostProcessEffect && m_bUsePostProcessEffectOnoff);

        if (m_bCurFrameUsePostProcessEffect)
        {
            if (m_texPostProcessID==0)
            {
                m_texPostProcessID = m_Texture.CreateTextureRenderTarget(m_d3dsdBackBuffer.Width, m_d3dsdBackBuffer.Height, true);
                RtCoreLog().Info("创建后期处理贴图1,ID=%d\n", m_texPostProcessID);
            }
            if (m_texPostProcessRenderTarget==0)
            {
                m_texPostProcessRenderTarget = m_Texture.CreateTextureRenderTarget(256, 256, true);
                RtCoreLog().Info("创建后期处理贴图2,ID=%d\n", m_texPostProcessRenderTarget);
            }
            RtgDevice::SetRenderTarget(m_texPostProcessID, -1);
        }

        // Clear the viewport m_dwClearColor

        DWORD dwFlag = D3DCLEAR_TARGET;
		if (m_iRenderTargetStackSize>0)
		{
			if (m_ZStencilStack[m_iRenderTargetStackSize-1]!=0)
			{
				dwFlag |= D3DCLEAR_ZBUFFER;
			}
		}else
		{
			dwFlag |= D3DCLEAR_ZBUFFER;
		}
        if(FAILED( hr=m_pd3dDevice->Clear( 0L, NULL, dwFlag, m_dwClearColor, 1.0f, 0L )))
        {
            return DisplayErrorMsg(hr, MSGERR_APPMUSTEXIT);
        }

        this->OnBeginRender();
        this->OnRender();
        this->OnEndRender();

        if (m_bClearZRender)
        {
            m_pd3dDevice->Clear( 0L, NULL, D3DCLEAR_ZBUFFER, 0x00000000, 1.0f, 0L );
            this->OnClearZRender();
        }

#if DO_STAT
        if (m_iRenderTargetStackSize==0)
        {
            // Output statistics
            static char szStatMsg[400];
            if (s_bShowStatFPS)
            {
                sprintf(szStatMsg, "%s F: %d", m_strFrameStats, m_iLastRenderNumFaces);
                //strcat(szStatMsg, m_strDeviceStats);
                m_eLastVertexFormat = RTGVF_NULL;
                m_pFont->DrawText( 2,  0, 0xFFFF00FF, szStatMsg );
                sprintf(szStatMsg, "F: %d", m_iLastRenderNumFaces);
            }
            if (s_bShowStatMem)
            {
                strcpy(szStatMsg, "Available: ");
                rtEncodeIntegerNumber(szStatMsg+strlen(szStatMsg), 0, this->GetAvailableTextureMem());
                strcat(szStatMsg, ", TextureUsed: ");
                rtEncodeIntegerNumber(szStatMsg+strlen(szStatMsg), 0, this->m_Texture.TotalUsedTextureSize());
                m_eLastVertexFormat = RTGVF_NULL;
                m_pFont->DrawText( 340, 0, 0xFFFF00FF, szStatMsg );
            }
            if (s_bShowStatBuffer)
            {
                sprintf(szStatMsg, "VB [Send:%d Clear:%d Used:%2.f%%] IB [Send:%d Clear:%d Used:%2.f%%]",
                    m_pVB->m_dwStatDynamicSend,
                    m_pVB->m_dwStatDynamicClear,
                    m_pVB->GetUsedDynamicMemory()*100.f/m_pVB->GetTotalDynamicMemory(),
                    m_pIB->m_dwStatDynamicSend,
                    m_pIB->m_dwStatDynamicClear,
                    m_pIB->GetUsedDynamicMemory()*100.f/m_pIB->GetTotalDynamicMemory()
                    );
                this->DrawString(2, 13, 0xFFFF00FF, szStatMsg, 0);
            }
            if (s_bShowStatRender)
            {
                sprintf(szStatMsg,
                    "D3D8Render: %.3f(ms)\n"
                    "Run: %.3f(ms)\n"
                    "Present: %.3f(ms)\n"
                    "UseTexture: %d\n"
                    "DrawPrimitive: %d\n"
                    "DrawPrimitiveVB: %d\n"
                    "DrawIndexPrimitive: %d\n"
                    "DrawIndexPrimitiveVB: %d",
                    rtClockSeconds(m_dwStatTimerRender),
                    rtClockSeconds(m_dwStatTimerRun),
                    rtClockSeconds(m_dwStatTimerPresent),
                    m_iLastRenderUseTexture,
                    m_iLastRenderDrawPrimitive,
                    m_iLastRenderDrawPrimitiveVB,
                    m_iLastRenderDrawIndexPrimitive,
                    m_iLastRenderDrawIndexPrimitiveVB
                    );
                this->DrawString(2, 26, 0xFFFF00FF, szStatMsg, 0);
            }
            if (s_bShowStatTexture)
            {
            }
            //if (s_bShowStatCollision)
            //{
            //    static char szTxt[100];
            //    sprintf(szTxt, "%d, %d, %d, %d",
            //        azmuGetCntAABBIntersectTriangle(),
            //        azmuGetCntLineIntersectTriangle(),
            //        azmuGetCntOBBIntersectOBB(),
            //        azmuGetCntRayHitTriangle());
            //    m_pFont->DrawText(2, 40, 0xFFFF00FF, szTxt);
            //    sprintf(szTxt, "C: %d, F: %d, M: %.2fm, T: %.2fm", m_iLastRenderNumClusters, m_iLastRenderNumFaces, m_pd3dDevice->GetAvailableTextureMem()/1000000.f, m_Texture.TotalTextureSize()/1000000.f);
            //    m_pFont->DrawText(2, 60, 0xFFFF00FF, szTxt);
            //}
        }
#endif

        // End the scene.
        m_pd3dDevice->EndScene();
    }
#if LOG_FUNNAME
    RtCoreLog().Info("------- End Render\n");
#endif
    return S_OK;
}

void RtgDeviceD3D8::DrawString(float fX, float fY, DWORD dwColor, const char* pString, DWORD dwFlags)
{
    m_eLastVertexFormat = RTGVF_NULL;
    m_pFont->DrawText( fX, fY, dwColor, (char*)pString, dwFlags );
}

HRESULT RtgDeviceD3D8::OneTimeSceneInit()
{
    return S_OK;
}

HRESULT RtgDeviceD3D8::InitDeviceObjects()
{
    m_pFont->InitDeviceObjects( m_pd3dDevice );
    return S_OK;
}

BOOL RtgDeviceD3D8::SetMaterial(RtgVertex4& vDiffuse, RtgVertex4& vAmbient, RtgVertex4& vSpecular, RtgVertex4& vEmissive, float fPower)
{
    m_vMaterialDiffuse = vDiffuse;
    m_vMaterialAmbient = vAmbient;
    m_vMaterialSpecular = vSpecular;
    m_vMaterialEmissive = vEmissive;
    m_fMaterialPower = fPower;

    D3DMATERIAL8 mtrl;
    mtrl.Diffuse.r = m_vMaterialDiffuse.x;
    mtrl.Diffuse.g = m_vMaterialDiffuse.y;
    mtrl.Diffuse.b = m_vMaterialDiffuse.z;
    mtrl.Diffuse.a = m_vMaterialDiffuse.w;
    mtrl.Ambient.r = m_vMaterialAmbient.x;
    mtrl.Ambient.g = m_vMaterialAmbient.y;
    mtrl.Ambient.b = m_vMaterialAmbient.z;
    mtrl.Ambient.a = m_vMaterialAmbient.w;
    mtrl.Specular.r = m_vMaterialSpecular.x;
    mtrl.Specular.g = m_vMaterialSpecular.y;
    mtrl.Specular.b = m_vMaterialSpecular.z;
    mtrl.Specular.a = m_vMaterialSpecular.w;
    mtrl.Emissive.r = m_vMaterialEmissive.x;
    mtrl.Emissive.g = m_vMaterialEmissive.y;
    mtrl.Emissive.b = m_vMaterialEmissive.z;
    mtrl.Emissive.a = m_vMaterialEmissive.w;
    mtrl.Power = m_fMaterialPower;
    return m_pd3dDevice->SetMaterial( &mtrl )==D3D_OK;
}

void RtgDeviceD3D8::RestoreTextureCaps()
{
    RTGTextureFilter  eTextureFilterMip    = m_eTextureFilterMip   ;
    RTGTextureFilter  eTextureFilterMin    = m_eTextureFilterMin   ;
    RTGTextureFilter  eTextureFilterMag    = m_eTextureFilterMag   ;
    RTGTextureAddress eTextureAddress      = m_eTextureAddress     ;
    DWORD             dwTextureBorderColor = m_dwTextureBorderColor;

    m_eTextureFilterMip    = RTGTEXF_MAX;
    m_eTextureFilterMin    = RTGTEXF_MAX;
    m_eTextureFilterMag    = RTGTEXF_MAX;
    m_eTextureAddress      = RTGTADD_MAX;
    m_dwTextureBorderColor = 0x00000000;

    this->SetTextureFilterMip   (eTextureFilterMip   );
    this->SetTextureFilterMin   (eTextureFilterMin   );
    this->SetTextureFilterMag   (eTextureFilterMag   );
    this->SetTextureAddress     (eTextureAddress     );
    this->SetTextureBorderColor (dwTextureBorderColor);
}

HRESULT RtgDeviceD3D8::RestoreDeviceObjects()
{
    int i;

    DEVMODE devMode;
    memset(&devMode, 0, sizeof(DEVMODE));
    devMode.dmSize = sizeof(DEVMODE);
    EnumDisplaySettings(NULL, ENUM_CURRENT_SETTINGS, &devMode);
    m_iWndWidth     = m_d3dsdBackBuffer.Width;
    m_iWndHeight    = m_d3dsdBackBuffer.Height;
    m_iWndColorBits = devMode.dmBitsPerPel;
    m_dwRefreshRate = devMode.dmDisplayFrequency;

    // 设置窗口
    m_iWndWidth = m_d3dsdBackBuffer.Width;
    m_iWndHeight = m_d3dsdBackBuffer.Height;
    switch (m_d3dsdBackBuffer.Format)
    {
    case D3DFMT_A8R8G8B8: m_eBackbufferFormat = RTG_TEXTURE_A8R8G8B8; break;
    case D3DFMT_X8R8G8B8: m_eBackbufferFormat = RTG_TEXTURE_X8R8G8B8; break;
    case D3DFMT_R8G8B8  : m_eBackbufferFormat = RTG_TEXTURE_R8G8B8  ; break;
    case D3DFMT_A4R4G4B4: m_eBackbufferFormat = RTG_TEXTURE_A4R4G4B4; break;
    case D3DFMT_A1R5G5B5: m_eBackbufferFormat = RTG_TEXTURE_A1R5G5B5; break;
    case D3DFMT_R5G6B5  : m_eBackbufferFormat = RTG_TEXTURE_R5G6B5  ; break;
    case D3DFMT_X1R5G5B5: m_eBackbufferFormat = RTG_TEXTURE_X1R5G5B5; break;
    case D3DFMT_X4R4G4B4: m_eBackbufferFormat = RTG_TEXTURE_X4R4G4B4; break;
    default: RtCoreLog().Warn("RtgDeviceD3D8: 不支持的Backbuffer格式\n"); break;
    }
    if( m_d3dsdBackBuffer.Format == D3DFMT_X8R8G8B8 || m_d3dsdBackBuffer.Format == D3DFMT_A8R8G8B8 || m_d3dsdBackBuffer.Format == D3DFMT_R8G8B8 )
    {
        m_iWndColorBits = 32;
    }else
    {
        m_iWndColorBits = 16;
    }

    // 设置材质
    D3DMATERIAL8 mtrl;
    mtrl.Diffuse.r = m_vMaterialDiffuse.x;
    mtrl.Diffuse.g = m_vMaterialDiffuse.y;
    mtrl.Diffuse.b = m_vMaterialDiffuse.z;
    mtrl.Diffuse.a = m_vMaterialDiffuse.w;
    mtrl.Ambient.r = m_vMaterialDiffuse.x;
    mtrl.Ambient.g = m_vMaterialDiffuse.y;
    mtrl.Ambient.b = m_vMaterialDiffuse.z;
    mtrl.Ambient.a = m_vMaterialDiffuse.w;
    mtrl.Specular.r = m_vMaterialSpecular.x;
    mtrl.Specular.g = m_vMaterialSpecular.y;
    mtrl.Specular.b = m_vMaterialSpecular.z;
    mtrl.Specular.a = m_vMaterialSpecular.w;
    mtrl.Emissive.r = m_vMaterialEmissive.x;
    mtrl.Emissive.g = m_vMaterialEmissive.y;
    mtrl.Emissive.b = m_vMaterialEmissive.z;
    mtrl.Emissive.a = m_vMaterialEmissive.w;
    mtrl.Power = m_fMaterialPower;
    m_pd3dDevice->SetMaterial( &mtrl );

    // 设置贴图

    TextureFormat texFormats[] =
    {
        RTG_TEXTURE_A8R8G8B8, 0, 32, 8, 8, 8, 8, Rtgimg_Fill_A8R8G8B8,    // 32 bits
        RTG_TEXTURE_X8R8G8B8, 0, 32, 0, 8, 8, 8, Rtgimg_Fill_X8R8G8B8,    // 32 bits
        RTG_TEXTURE_R8G8B8  , 0, 32, 0, 8, 8, 8, Rtgimg_Fill_R8G8B8  ,    // 24 bits
        RTG_TEXTURE_A4R4G4B4, 0, 16, 4, 4, 4, 4, Rtgimg_Fill_A4R4G4B4,    // 16 bits
        RTG_TEXTURE_A1R5G5B5, 0, 16, 1, 5, 5, 5, Rtgimg_Fill_A1R5G5B5,    // 16 bits
        RTG_TEXTURE_R5G6B5  , 0, 16, 0, 5, 6, 5, Rtgimg_Fill_R5G6B5  ,    // 16 bits
        RTG_TEXTURE_X1R5G5B5, 0, 16, 0, 5, 5, 5, Rtgimg_Fill_X1R5G5B5,    // 16 bits
        RTG_TEXTURE_X4R4G4B4, 0, 16, 0, 4, 4, 4, Rtgimg_Fill_X4R4G4B4,    // 16 bits
    };

    D3DFORMAT d3dPixelFormat;
    D3DAdapterInfo* pAdapterInfo = &m_Adapters[m_dwAdapter];
    D3DDeviceInfo* pDeviceInfo = &pAdapterInfo->devices[pAdapterInfo->dwCurrentDevice];
    if (m_bWindowed)
    {
        d3dPixelFormat = pAdapterInfo->d3ddmDesktop.Format;
    }else
    {
        D3DModeInfo* pModeInfo = &pDeviceInfo->modes[pDeviceInfo->dwCurrentMode];
        d3dPixelFormat = pModeInfo->Format;
    }

    RtgImage::m_bSupportDXT = false;
    m_iTexNumFormats = 0;
    if (pDeviceInfo->DeviceType==D3DDEVTYPE_HAL)
    {   // 硬件
        for ( i=0; i<sizeof(texFormats)/sizeof(TextureFormat); i++ )
        {
            if (SUCCEEDED(m_pD3D->CheckDeviceFormat( m_dwAdapter, D3DDEVTYPE_HAL, d3dPixelFormat, 0, D3DRTYPE_TEXTURE, enumRTGTextureFormat[texFormats[i].m_eFormat])))
            {
                texFormats[i].m_bSupported = 1;
                m_TexFormat[m_iTexNumFormats++] = texFormats[i];
            }
        }
        if (SUCCEEDED(m_pD3D->CheckDeviceFormat( m_dwAdapter, D3DDEVTYPE_HAL, d3dPixelFormat, 0, D3DRTYPE_TEXTURE, D3DFMT_DXT5)))
        {
            RtgImage::m_bSupportDXT = true;
        }
    }else
    {
        for ( i=0; i<sizeof(texFormats)/sizeof(TextureFormat); i++ )
        {   // 软件
            texFormats[i].m_bSupported = 1;
            m_TexFormat[m_iTexNumFormats++] = texFormats[i];
        }
    }
    if (!m_Texture.SelectTextureFormat(m_iWndColorBits))
    {
        return FALSE;
    }
    if (!( m_pTexColorFormat->m_eFormat==RTG_TEXTURE_X1R5G5B5
        || m_pTexColorFormat->m_eFormat==RTG_TEXTURE_R5G6B5
        || m_pTexColorFormat->m_eFormat==RTG_TEXTURE_X8R8G8B8
        || m_pTexColorFormat->m_eFormat==RTG_TEXTURE_A8R8G8B8))
    {
        RtCoreLog().Warn("RtgDeviceD3D8: 显卡没有支持任何RenderTarget需要的格式，创建RenderTarget将失败\n");
    }

#ifndef NO_DEBUG_INFO
    char* pTexFormatsName[] =
    { "A8R8G8B8", "X8R8G8B8", "R8G8B8  ", "A4R4G4B4", "A1R5G5B5", "R5G6B5  ", "X1R5G5B5", "X4R4G4B4" };

    RtCoreLog().Info("RtgDeviceD3D8: 重新设置窗口为： %dx%dx%dx%d (Backbuffer格式为[%s])\n",
        m_iWndWidth, m_iWndHeight, m_iWndColorBits, m_dwRefreshRate, D3D8_D3DFormatToString(d3dPixelFormat, false));
    for ( i=0; i<sizeof(texFormats)/sizeof(TextureFormat); i++ )
    {
        if (texFormats[i].m_bSupported)
        {
            if (texFormats[i].m_eFormat==m_pTexColorFormat->m_eFormat)
            {
                RtCoreLog().Info("    %2d: [%s] %d Bits, %d Alpha [默认Color格式]\n", i, pTexFormatsName[i], texFormats[i].m_iBits, texFormats[i].m_iAlphaBits);
            }
            if (texFormats[i].m_eFormat==m_pTexAlphaFormat->m_eFormat)
            {
                RtCoreLog().Info("    %2d: [%s] %d Bits, %d Alpha [默认Alpha格式]\n", i, pTexFormatsName[i], texFormats[i].m_iBits, texFormats[i].m_iAlphaBits);
            }
            if (texFormats[i].m_eFormat==m_pTexKeyFormat->m_eFormat)
            {
                RtCoreLog().Info("    %2d: [%s] %d Bits, %d Alpha [默认ColorKey格式]\n", i, pTexFormatsName[i], texFormats[i].m_iBits, texFormats[i].m_iAlphaBits);
            }
        }
    }
#endif

    //////////////////////////////////////////////////////////////////////////

    if (!OnRestoreDevice())
    {
        return S_FALSE;
    }

    // Restore the device objects for the meshes and fonts
    m_pFont->RestoreDeviceObjects();
    // Set the transform matrices (view and world are updated per frame)
    m_pCamera->SetAspect(m_d3dsdBackBuffer.Width / (float)m_d3dsdBackBuffer.Height);
    m_pCamera->RestoreDevice();

    /*
    // Set up the default texture states
    m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_SELECTARG1 );
    m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
    m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG2, D3DTA_DIFFUSE );
    m_pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP,   D3DTOP_SELECTARG1 );
    m_pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE );

    m_pd3dDevice->SetRenderState( D3DRS_DITHERENABLE, TRUE );
    m_pd3dDevice->SetRenderState( D3DRS_ZENABLE,      TRUE );
    m_pd3dDevice->SetRenderState( D3DRS_LIGHTING,     TRUE );
    */

    this->RestoreTextureCaps();

    // 以下为强制设置的状态

    // Alpha Test
    m_listRenderState[RTGRS_ALPHA_TEST].bChanged    = TRUE;
    m_listRenderState[RTGRS_ALPHA_REF].bChanged     = TRUE;
    m_listRenderState[RTGRS_ALPHA_FUNC].bChanged    = TRUE;
    m_listRenderState[RTGRS_Z_FUNC].bChanged        = TRUE;

    m_listRenderState[RTGRS_ALPHA_TEST].dwValue     = m_bAlphaTest;
    m_listRenderState[RTGRS_ALPHA_REF].dwValue      = (DWORD)0x00000080; // 默认应该是0x00000001
    m_listRenderState[RTGRS_ALPHA_FUNC].dwValue     = D3DCMP_GREATEREQUAL;
    m_listRenderState[RTGRS_Z_FUNC].dwValue         = D3DCMP_LESSEQUAL;

    // blend
    m_listRenderState[RTGRS_ALPHA_BLEND].bChanged   = TRUE;
    m_listRenderState[RTGRS_BLEND_SRC].bChanged     = TRUE;
    m_listRenderState[RTGRS_BLEND_DEST].bChanged    = TRUE;

    m_listRenderState[RTGRS_ALPHA_BLEND].dwValue    = m_bBlend;
    m_listRenderState[RTGRS_BLEND_SRC].dwValue      = enumRTGBlendFunc[m_eBlendSrcFunc];
    m_listRenderState[RTGRS_BLEND_DEST].dwValue     = enumRTGBlendFunc[m_eBlendDestFunc];

    // cull face
    m_listRenderState[RTGRS_CULLMODE].bChanged      = TRUE;
    if (m_bCullFace) m_listRenderState[RTGRS_CULLMODE].dwValue = m_dwCullDefault;
    else             m_listRenderState[RTGRS_CULLMODE].dwValue = D3DCULL_NONE;

    // FSAA
    SetRenderState(RTGRS_FULLSCRENE_ANTIALIASING, m_userConfig.bFSAA);
    SetRenderState(RTGRS_EDGE_ANTIALIAS, m_userConfig.bEdgeAntialias);

    // 贴图矩阵
    for (i=0; i<8; i++)
    {
        m_bUseTextureMatrix[i] = FALSE;
        m_dwTexTransformFlag[i] = RTGTTF_MAX; // 强制设置成与其他都不同
    }

    // 最后一次状态
    for (i=0; i<RTGRS_MAX; i++)
    {
        if (m_listRenderState[i].bChanged)
        {
            m_listRenderState[i].bChanged = FALSE;
            SetRenderState((RTGRenderState)(i), m_listRenderState[i].dwValue);
        }
    }

    /*
    // 以下为强制设置的状态

    // Alpha Test
	m_pd3dDevice->SetRenderState(D3DRS_ALPHATESTENABLE, m_bAlphaTest);
    m_pd3dDevice->SetRenderState(D3DRS_ALPHAREF, (DWORD)0x00000001);
    m_pd3dDevice->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATEREQUAL);
    m_pd3dDevice->SetRenderState(D3DRS_ZFUNC, D3DCMP_LESSEQUAL);

    // blend
	m_pd3dDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, m_bBlend);
	m_pd3dDevice->SetRenderState(D3DRS_SRCBLEND, enumRTGBlendFunc[m_eBlendSrcFunc]);
	m_pd3dDevice->SetRenderState(D3DRS_DESTBLEND, enumRTGBlendFunc[m_eBlendDestFunc]);

    // cull face
    if (m_bCullFace) m_pd3dDevice->SetRenderState(D3DRS_CULLMODE, m_dwCullDefault);
    else             m_pd3dDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
    */

    // Fog
    m_pd3dDevice->SetRenderState(D3DRS_FOGENABLE, m_bFogEnable);
    m_pd3dDevice->SetRenderState(D3DRS_FOGCOLOR, m_dwFogColor);
    m_pd3dDevice->SetRenderState(D3DRS_FOGVERTEXMODE, D3DFOG_LINEAR);
    m_pd3dDevice->SetRenderState(D3DRS_FOGSTART, *(DWORD*)(&m_fFogStart));
    m_pd3dDevice->SetRenderState(D3DRS_FOGEND,   *(DWORD*)(&m_fFogEnd));
    //m_pd3dDevice->SetRenderState(D3DRS_RANGEFOGENABLE, TRUE);

    // texture factor
    m_pd3dDevice->SetRenderState(D3DRS_TEXTUREFACTOR, m_dwTextureFactor);

    // Render Shader
    m_pd3dDevice->SetVertexShader(m_dwVertexFVF);

    return S_OK;
}

void RtgDeviceD3D8::SetFogEnable(BOOL bEnable)
{
    if (m_bFogEnable!=bEnable)
    {
        m_bFogEnable = bEnable;
        m_pd3dDevice->SetRenderState(D3DRS_FOGENABLE, m_bFogEnable);
        if (bEnable)
        {
            m_pd3dDevice->SetRenderState(D3DRS_FOGVERTEXMODE, D3DFOG_LINEAR);
            m_pd3dDevice->SetRenderState(D3DRS_RANGEFOGENABLE, TRUE);
        }
    }
}

void RtgDeviceD3D8::SetFogParameter(DWORD dwColor, float fNear, float fFar)
{
    SetFogColor(dwColor);
    SetFogNear(fNear);
    SetFogFar(fFar);
}

void RtgDeviceD3D8::SetFogColor(DWORD dwColor)
{
    if (m_dwFogColor!=dwColor)
    {
        m_dwFogColor = dwColor;
        m_pd3dDevice->SetRenderState(D3DRS_FOGCOLOR, m_dwFogColor);
    }
}

void RtgDeviceD3D8::SetFogNear(float fNear)
{
    if (m_fFogStart!=fNear)
    {
        m_fFogStart = fNear;
        m_pd3dDevice->SetRenderState(D3DRS_FOGSTART, *(DWORD*)(&m_fFogStart));
    }
}

void RtgDeviceD3D8::SetFogFar(float fFar)
{
    if (m_fFogEnd!=fFar)
    {
        m_fFogEnd = fFar;
        m_pd3dDevice->SetRenderState(D3DRS_FOGEND,   *(DWORD*)(&m_fFogEnd));
    }
}

HRESULT RtgDeviceD3D8::InvalidateDeviceObjects()
{
    if (!OnInvalidateDevice())
    {
        return S_FALSE;
    }

    if (m_pDefaultRenderTarget)
    {
        m_pDefaultRenderTarget->Release();
        m_pDefaultRenderTarget = NULL;
    }
    if (m_texPostProcessID)
    {
        m_Texture.DeleteTexture(m_texPostProcessID);
        RtCoreLog().Info("销毁后期处理贴图1,ID=%d\n", m_texPostProcessID);
        m_texPostProcessID = 0;
    }
    if (m_texPostProcessRenderTarget)
    {
        m_Texture.DeleteTexture(m_texPostProcessRenderTarget);
        RtCoreLog().Info("销毁后期处理贴图2,ID=%d\n", m_texPostProcessRenderTarget);
        m_texPostProcessRenderTarget = 0;
    }

    m_pFont->InvalidateDeviceObjects();
    return S_OK;
}

HRESULT RtgDeviceD3D8::DeleteDeviceObjects()
{
    if (m_pDefaultRenderTarget)
    {
        m_pDefaultRenderTarget->Release();
        m_pDefaultRenderTarget = NULL;
    }
    if (m_texPostProcessID)
    {
        m_Texture.DeleteTexture(m_texPostProcessID);
        RtCoreLog().Info("销毁后期处理贴图1,ID=%d\n", m_texPostProcessID);
        m_texPostProcessID = 0;
    }
    if (m_texPostProcessRenderTarget)
    {
        m_Texture.DeleteTexture(m_texPostProcessRenderTarget);
        RtCoreLog().Info("销毁后期处理贴图2,ID=%d\n", m_texPostProcessRenderTarget);
        m_texPostProcessRenderTarget = 0;
    }

    m_pFont->DeleteDeviceObjects();
    return S_OK;
}

HRESULT RtgDeviceD3D8::FinalCleanup()
{
    //delete m_pFont;
	//m_pFont = NULL;
	DEL_ONE( m_pFont );
    
    return S_OK;
}

HRESULT RtgDeviceD3D8::ConfirmDevice(D3DCAPS8* pCaps, DWORD dwBehavior, D3DFORMAT eFormat)
{
    if( dwBehavior & D3DCREATE_PUREDEVICE )
        return E_FAIL; // GetTransform doesn't work on PUREDEVICE

    // This sample uses alpha textures and/or straight alpha. Make sure the
    // device supports them
    if( pCaps->TextureCaps & D3DPTEXTURECAPS_ALPHAPALETTE )
        return S_OK;
    if( pCaps->TextureCaps & D3DPTEXTURECAPS_ALPHA )
        return S_OK;

    return E_FAIL;
}

HRESULT RtgDeviceD3D8::AdjustWindowForChange()
{
    if (m_bCustomToggleFullscreen)
    {
        if( m_bWindowed )
        {
            ::ShowWindow( m_hRenderFullWnd, SW_HIDE );
            CD3DApplication::m_hWnd = m_hRenderWnd;
        }
        else
        {
            if( ::IsIconic( m_hRenderFullWnd ) )
                ::ShowWindow( m_hRenderFullWnd, SW_RESTORE );
            ::ShowWindow( m_hRenderFullWnd, SW_SHOW );
            CD3DApplication::m_hWnd = m_hRenderFullWnd;
        }
        return S_OK;
    }else
    {
        return CD3DApplication::AdjustWindowForChange();
    }
}

long RtgDeviceD3D8::DeviceMsgProc( RTHWND hWnd, UINT uMsg, UINT wParam, UINT lParam )
{
    return CD3DApplication::MsgProc((HWND)hWnd, uMsg, wParam, lParam);
}

int RtgDeviceD3D8::GetAvailableTextureMem()
{
    return m_pd3dDevice->GetAvailableTextureMem();
}

bool RtgDeviceD3D8::OnD3D8ErrorMessage(const char* szMsg, int iType)
{
    return OnErrorMessage(szMsg, iType);
}

BOOL RtgDeviceD3D8::SaveRenderTargetToFile(const char* szFileName, bool bJPG)
{
    /*
    BOOL bResult = FALSE;
    LPDIRECT3DSURFACE8 pSrcSurface;
    if (m_pd3dDevice->GetRenderTarget(&pSrcSurface)!=D3D_OK)
    {
        return bResult;
    }
    bResult = D3DXSaveSurfaceToFile(szFileName, D3DXIFF_BMP, pSrcSurface, NULL, NULL)==D3D_OK;
    pSrcSurface->Release();
    return bResult;
    */
    RtArchive* pArc = RtCoreFile().CreateFileWriter(NULL, szFileName, 0);
    if (pArc)
    {
        SaveRenderTargetToArchive(pArc, bJPG);
        RtCoreFile().CloseFile(pArc);
        return TRUE;
    }else
    {
        return FALSE;
    }
}

BOOL RtgDeviceD3D8::SaveRenderTargetToArchive(RtArchive* pArc, bool bJPG)
{
    BOOL bResult = FALSE;
    /*
    LPDIRECT3DSURFACE8 pSrcSurface;
    if (m_pd3dDevice->GetRenderTarget(&pSrcSurface)!=D3D_OK)
    {
        return bResult;
    }

    D3DLOCKED_RECT desc;
    if (pSrcSurface->LockRect(&desc, NULL, NULL)==D3D_OK)
    {
        //pTexFormat->fill(pItem->iWidth, pItem->iHeight, desc.pBits, desc.Pitch, pBuffer, pItem->iWidth*4);
        LOG1("LocalRect: %d\n", desc.Pitch);
        pSrcSurface->UnlockRect();
    }else
    {
        bResult = FALSE;
    }

    //bResult = D3DXSaveSurfaceToFile(szFileName, D3DXIFF_BMP, pSrcSurface, NULL, NULL)==D3D_OK;
    pSrcSurface->Release();
    */

    HRESULT				hr;
    D3DSURFACE_DESC		d3dsd;
    LPDIRECT3DSURFACE8	SurfaceCopy;
    D3DLOCKED_RECT		d3dLR;
    unsigned char       *pImage, *pImageAux, *pData;
    UINT                x, y;
    unsigned char       wColor;
    RtgImage            img;

    LPDIRECT3DSURFACE8 pSrcSurface;
    if (m_pd3dDevice->GetRenderTarget(&pSrcSurface)!=D3D_OK)
    {
        return bResult;
    }

    pSrcSurface->GetDesc(&d3dsd);

    if (!img.Create(d3dsd.Width, d3dsd.Height, false))
    {
        pSrcSurface->Release();
        return FALSE;
    }

    hr = m_pd3dDevice->CreateImageSurface(d3dsd.Width, d3dsd.Height, d3dsd.Format, &SurfaceCopy);
    if (SUCCEEDED(hr))
    {
        hr = m_pd3dDevice->CopyRects(pSrcSurface, NULL, 0, SurfaceCopy, NULL);
        if (SUCCEEDED(hr))
        {
            hr = SurfaceCopy->LockRect(&d3dLR, NULL, D3DLOCK_NO_DIRTY_UPDATE | D3DLOCK_NOSYSLOCK | D3DLOCK_READONLY);
            if (SUCCEEDED(hr))
            {
                pImage = (unsigned char*)d3dLR.pBits;
                pData = img.data.pData;

                for (y = 0; y < d3dsd.Height; y++)
                {
                    if (d3dsd.Format == D3DFMT_X8R8G8B8)
                    {
                        pImageAux = pImage;
                        for (x = 0; x < d3dsd.Width; x++)
                        {
                            pData[2] = pImageAux[0];
                            pData[1] = pImageAux[1];
                            pData[0] = pImageAux[2];
                            pData[3] = 0;

                            pData += 4;
                            pImageAux += 4;
                        }
                    }else if (d3dsd.Format == D3DFMT_A8R8G8B8)
                    {
                        pImageAux = pImage;
                        for (x = 0; x < d3dsd.Width; x++)
                        {
                            pData[2] = pImageAux[0];
                            pData[1] = pImageAux[1];
                            pData[0] = pImageAux[2];
                            pData[3] = pImageAux[3];

                            pData += 4;
                            pImageAux += 4;
                        }
                    }else if (d3dsd.Format == D3DFMT_R5G6B5)
                    {
                        pImageAux = pImage;
                        for (x = 0; x < d3dsd.Width; x++)
                        {
                            wColor = *((unsigned short*)pImageAux);

                            pData[2] = (unsigned char)((wColor&0x001f)<<3);
                            pData[1] = (unsigned char)(((wColor&0x7e0)>>5)<<2);
                            pData[0] = (unsigned char)(((wColor&0xf800)>>11)<<3);
                            pData[3] = 0;

                            pData += 4;
                            pImageAux += 2;
                        }
                    }else if (d3dsd.Format == D3DFMT_X1R5G5B5)
                    {
                        pImageAux = pImage;
                        for (x = 0; x < d3dsd.Width; x++)
                        {
                            wColor = *((unsigned short*)pImageAux);

                            pData[2] = (unsigned char)((wColor&0x001f)<<3);
                            pData[1] = (unsigned char)(((wColor&0x3e0)>>5)<<3);
                            pData[0] = (unsigned char)(((wColor&0x7c00)>>10)<<3);
                            pData[3] = 0;

                            pData += 4;
                            pImageAux += 2;
                        }
                    }else if (d3dsd.Format == D3DFMT_A1R5G5B5)
                    {
                        pImageAux = pImage;
                        for (x = 0; x < d3dsd.Width; x++)
                        {
                            wColor = *((unsigned short*)pImageAux);

                            pData[2] = (unsigned char)((wColor&0x001f)<<3);
                            pData[1] = (unsigned char)(((wColor&0x3e0)>>5)<<3);
                            pData[0] = (unsigned char)(((wColor&0x7c00)>>10)<<3);
                            pData[3] = (unsigned char)(((wColor&0x8000)>>15)*255);

                            pData += 4;
                            pImageAux += 2;
                        }
                    }
                    pImage += d3dLR.Pitch;
                }
                SurfaceCopy->UnlockRect();

                img.Flip();

                if (bJPG)
                {
                    bResult = img.SaveToArchiveJPG(pArc);
                }else
                {
                    bResult = img.SaveToArchiveBMP(pArc);
                }
            }
        }
        SurfaceCopy->Release();
    }
    pSrcSurface->Release();

    return bResult;
}

bool RtgDeviceD3D8::CreateUIStateBlock(DWORD* dwToken)
{
    if (m_pd3dDevice->BeginStateBlock()!=D3D_OK)
        return false;
    m_pd3dDevice->SetRenderState( D3DRS_ZENABLE,                   FALSE );
    m_pd3dDevice->SetRenderState( D3DRS_FOGENABLE,                 FALSE );
    m_pd3dDevice->SetRenderState( D3DRS_MULTISAMPLEANTIALIAS,      FALSE );
    m_pd3dDevice->SetRenderState( D3DRS_LIGHTING,                  FALSE );
	m_pd3dDevice->SetRenderState( D3DRS_ZFUNC,					   D3DCMP_EQUAL );

	m_pd3dDevice->SetTextureStageState( 0, D3DTSS_MINFILTER, D3DTEXF_POINT );
    m_pd3dDevice->SetTextureStageState( 0, D3DTSS_MAGFILTER, D3DTEXF_POINT );
    m_pd3dDevice->SetTextureStageState( 0, D3DTSS_MIPFILTER, D3DTEXF_NONE );
    return m_pd3dDevice->EndStateBlock( dwToken )==D3D_OK;
}

bool RtgDeviceD3D8::BeginRecordStateBlock()
{
    return m_pd3dDevice->BeginStateBlock()==D3D_OK;
}

bool RtgDeviceD3D8::EndRecordStateBlock(DWORD* dwToken)
{
    return m_pd3dDevice->EndStateBlock(dwToken)==D3D_OK;
}

bool RtgDeviceD3D8::DeleteStateBlock(DWORD dwToken)
{
    return m_pd3dDevice->DeleteStateBlock(dwToken)==D3D_OK;
}

bool RtgDeviceD3D8::ApplyStateBlock(DWORD dwToken)
{
    return m_pd3dDevice->ApplyStateBlock(dwToken)==D3D_OK;
}

bool RtgDeviceD3D8::CaptureStateBlock(DWORD dwToken)
{
    return m_pd3dDevice->CaptureStateBlock(dwToken)==D3D_OK;
}

BOOL RtgDeviceD3D8::IsWindowed()
{
    return m_bWindowed;
}

BOOL RtgDeviceD3D8::HasFocus()
{
    return m_bHasFocus;
}

BOOL RtgDeviceD3D8::IsActive()
{
    return m_bActive;
}

DWORD RtgDeviceD3D8::GetDisplayMode(RtgDisplayMode* pModeBuf, DWORD dwBufSize)
{
    D3DAdapterInfo* pAdapterInfo = &m_Adapters[m_dwAdapter];
    D3DDeviceInfo*  pDeviceInfo  = &pAdapterInfo->devices[pAdapterInfo->dwCurrentDevice];

    DWORD i, BitDepth;
    int iCount = 0;
    D3DDISPLAYMODE* pDisplayMode;

	if (dwBufSize==0) dwBufSize = 0x0FFFFFFF;
    for (i=0; i<pDeviceInfo->dwNumModes && i<dwBufSize; i++)
    {
        BitDepth = 16;
        if( pDeviceInfo->modes[i].Format == D3DFMT_X8R8G8B8 ||
            pDeviceInfo->modes[i].Format == D3DFMT_A8R8G8B8 ||
            pDeviceInfo->modes[i].Format == D3DFMT_R8G8B8 )
        {
            BitDepth = 32;
        }

        D3DDISPLAYMODE DisplayMode;
        DWORD dwNumAdapterModes = m_pD3D->GetAdapterModeCount( m_dwAdapter );
        for( UINT iMode = 0; iMode < dwNumAdapterModes; iMode++ )
        {
            m_pD3D->EnumAdapterModes( m_dwAdapter, iMode, &DisplayMode );

            pDisplayMode = &DisplayMode;

            if (   pDisplayMode->Width==pDeviceInfo->modes[i].Width
                && pDisplayMode->Height==pDeviceInfo->modes[i].Height
                && pDisplayMode->Format==pDeviceInfo->modes[i].Format)
            {
                if (pModeBuf)
                {
                    if ((DWORD)iCount<dwBufSize)
                    {
                        pModeBuf[iCount].dwWidth        = pDeviceInfo->modes[i].Width;
                        pModeBuf[iCount].dwHeight       = pDeviceInfo->modes[i].Height;
                        pModeBuf[iCount].dwDepth        = BitDepth;
                        pModeBuf[iCount].dwRefreshRate  = pDisplayMode->RefreshRate;
                        iCount ++;
                    }
                }else
                {
                    iCount ++;
                }
            }
        }
    }
    return iCount;
}

bool RtgDeviceD3D8::GetCanDoWindowed()
{
    D3DAdapterInfo* pAdapterInfo = &m_Adapters[m_dwAdapter];
    D3DDeviceInfo*  pDeviceInfo  = &pAdapterInfo->devices[pAdapterInfo->dwCurrentDevice];
    return pDeviceInfo->bCanDoWindowed==TRUE;
}

const char* RtgDeviceD3D8::GetDeviceDesc()
{
    D3DAdapterInfo* pAdapterInfo = &m_Adapters[m_dwAdapter];
    D3DDeviceInfo*  pDeviceInfo  = &pAdapterInfo->devices[pAdapterInfo->dwCurrentDevice];
    return pDeviceInfo->strDesc;
}
} // namespace rt_graph

#endif // USE_D3D8

/*------------------------------------------------------------------------
-   The End.
------------------------------------------------------------------------*/
