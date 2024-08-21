

#pragma message("=======> Graphics Lib Use D3D9")

#include <d3d9.h>
#include "D3dx9math.h"
#include "rtg_graph_inter.h"
#include "graph\dxutil.h"
#include "graph\d3denumeration.h"
#include "graph\d3dsettings.h"
#include "graph\d3dapp.h"
#include "graph\d3dfont.h"
using namespace rt_graph_dx9;
#include "graph\rtg_device_d3d9.h"
#include "graph\d3dutil.h"
#include "rtg_i_util.h"
#include "graph/rtg_effect_manager.h"
#include "graph/rtg_render_target_manager.h"
#include "character/actor.h"
#include "rtg_buffer_manager_d3d8.h"

namespace rt_graph {

#include "rtg_enum_mapping.h"

#define LOG_FUNNAME 0

#if LOG_FUNNAME
#define LOG_FUNCTION LOG1("%s\n", __FUNCTION__)
#else
#define LOG_FUNCTION
#endif

enum {
    EUSE_DEFAULT_STATE = 0,
    EUSE_USER_STATE = 1,

};

/*----------------------------------------------------------------------
    class RtgDeviceD3D9
----------------------------------------------------------------------*/
DWORD dwT1, dwT2;

RtgDeviceD3D9* g_pDevice = NULL;
RtgDevice*     g_pDeviceBase = NULL;

//double g_RenderUseTime = 0.0;
//double g_RenderUiUseTime = 0.0;
//double g_UpdateUiUseTime = 0.0;

bool rtCreateDeviceD3D9() {
    RTASSERT(!g_pDevice);
    g_pDevice = RT_NEW RtgDeviceD3D9;
    g_pDeviceBase = g_pDevice;
    return !!g_pDevice;
}

void rtDestroyDevice() {
    if (g_pDevice)
        g_pDevice->Close();
    DEL_ONE(g_pDevice);
    g_pDeviceBase = NULL;
}

RtgDeviceD3D9::RtgDeviceD3D9()
    : m_ActDecl_vnt(NULL), m_ActDecl_vnct(NULL), m_bDrawPerf(false), m_fPerfx(5.f), m_fPerfy(5.f),
      m_bloomTexW(0), m_bloomTexH(0) {
    m_eDeviceType = RTG_DEVICE_D3D9;
    m_iCurrentZBufferState = EUSE_DEFAULT_STATE;
    m_pEffectManager = NULL;
    m_pTextMamager = NULL;
    m_pBufferManager = NULL;
    m_pRTManager = NULL;
    for (int i = 0; i < RTGRS_MAX; i++) {
        m_listRenderState[i].bChanged = FALSE;
        m_listRenderState[i].dwValue = 0;
    }

    // 使用的坐标系
#ifdef RTG_LEFT_HANDED
    m_bLeftHanded = TRUE;
#else
    m_bLeftHanded = FALSE;
#endif
    if (m_bLeftHanded)
        m_dwCullDefault = D3DCULL_CCW;
    else
        m_dwCullDefault = D3DCULL_CW;

    // Font
    m_pFont = RT_NEW CD3DFont(_T("Courier New"), 10, 0);

    m_dwVertexFVF = D3DFVF_XYZ;
    m_d3dEnumeration.AppUsesDepthBuffer = TRUE;

    m_bCustomToggleFullscreen = false;
    m_hRenderWnd = NULL;
    m_hRenderFullWnd = NULL;
    m_pDefaultRenderTarget = NULL;
    m_pDefaultDepthStencilSurface = NULL;
    m_pRenderTargetDepthStencilSurface = NULL;
    m_texPostProcess = NULL;
    m_texPostDown0 = NULL;
    m_texPostDown1 = NULL;

    memset(m_bloomTexOffset_h, 0, sizeof(m_bloomTexOffset_h));
    memset(m_bloomTexOffset_v, 0, sizeof(m_bloomTexOffset_v));
}

RtgDeviceD3D9::~RtgDeviceD3D9() {
    DEL_ONE(m_pFont);
    if (m_pDefaultRenderTarget) {
        m_pDefaultRenderTarget->Release();
        m_pDefaultRenderTarget = NULL;
    }
    if (m_pRenderTargetDepthStencilSurface) {
        m_pRenderTargetDepthStencilSurface->Release();
        m_pRenderTargetDepthStencilSurface = NULL;
    }
}

void RtgDeviceD3D9::ChangeDrawPref() {
    m_bDrawPerf = !m_bDrawPerf;
    m_fPerfx = 5.f;
    m_fPerfy = 120.f;
}

ID3DXEffect* RtgDeviceD3D9::GetEffect(RtgEffectType _Type) {
    return m_pEffectManager->GetEffect(_Type);
}

bool RtgDeviceD3D9::FullscreenToggle() {
    if (FAILED(this->ToggleFullscreen())) {
        return false;
    }
    return true;
}

bool RtgDeviceD3D9::SetFullScreenInfo(DWORD dwWidth, DWORD dwHeight, DWORD dwDepth) {
    return SetFullScreenMode(dwWidth, dwHeight, dwDepth);
}

bool RtgDeviceD3D9::ResetDevice(BOOL bFullscreen, DWORD dwWidth, DWORD dwHeight, DWORD dwDepth) {
    DWORD idc, idm, iBestDeviceCombo, iBitDepth;
    DWORD iBestRefreshRate, iMaxRefreshRate, iHighRefreshRate, iBestDisplayMode, iHighDisplayMode;
    D3DAdapterInfo* pAdapterInfo;
    D3DDeviceInfo*  pDeviceInfo;
    D3DDeviceCombo* pDeviceCombo;
    D3DDISPLAYMODE* pdm;
    D3DDISPLAYMODE  dm;
    D3DFORMAT       bitFormat;

    // 检查是否改变
    pDeviceCombo = m_d3dSettings.PDeviceCombo();
    if (pDeviceCombo) {
        dm = m_d3dSettings.DisplayMode();
        if (m_d3dSettings.IsWindowed) {
            dm.Width = m_d3dSettings.Windowed_Width;
            dm.Height = m_d3dSettings.Windowed_Height;
        }
        iBitDepth = 16;
        if (dm.Format == D3DFMT_X8R8G8B8 || dm.Format == D3DFMT_A8R8G8B8 ||
            dm.Format == D3DFMT_R8G8B8) {
            iBitDepth = 32;
        }
        if ((BOOL)m_d3dSettings.IsWindowed != bFullscreen && dm.Width == dwWidth &&
            dm.Height == dwHeight && iBitDepth == dwDepth) {
            return true;  // 如果没有改变就直接返回
        }
        if (m_d3dSettings.IsWindowed && !bFullscreen && dm.Width == dwWidth &&
            dm.Height == dwHeight) {
            return true;  // 如果只是窗口模式就不比较颜色
        }
    }
    RtCoreLog().Info("RtgDeviceD3D9: ResetDevice [%s] (%dx%dx%d)\n",
                     bFullscreen ? "Fullscreen" : "Window", dwWidth, dwHeight, dwDepth);

    // 检查是否有符合的颜色深度
    iBestDeviceCombo = -1;
    D3DFORMAT bestFormat = dwDepth == 32 ? D3DFMT_A8R8G8B8 : D3DFMT_R5G6B5;
    //if (!bFullscreen) bitFormat = m_d3dSettings.PAdapterInfo()->d3ddmDesktop.Format;
    pDeviceInfo = m_d3dSettings.PDeviceInfo();
    for (idc = 0; idc < pDeviceInfo->pDeviceComboList->Count(); idc++) {
        pDeviceCombo = (D3DDeviceCombo*)pDeviceInfo->pDeviceComboList->GetPtr(idc);
        if ((BOOL)pDeviceCombo->IsWindowed == bFullscreen)
            continue;
        if (pDeviceCombo->BackBufferFormat != pDeviceCombo->AdapterFormat)
            continue;
        /*if (bFullscreen)*/ bitFormat = pDeviceCombo->AdapterFormat;

        if (bestFormat == bitFormat) {
            iBestDeviceCombo = idc;
            break;
        } else if ((bitFormat == D3DFMT_X8R8G8B8 || bitFormat == D3DFMT_A8R8G8B8 ||
                    bitFormat == D3DFMT_R8G8B8) &&
                   dwDepth == 32) {
            iBestDeviceCombo = idc;
        }
    }
    if (iBestDeviceCombo == -1)  // 没有找到对应的颜色深度
    {
        RtCoreLog().Error("RtgDeviceD3D9: Device不支持指定的颜色深度 [%d], 在[%s]模式下\n", dwDepth,
                          bFullscreen ? "全屏" : "窗口");
        return false;
    }
    pDeviceCombo = (D3DDeviceCombo*)pDeviceInfo->pDeviceComboList->GetPtr(iBestDeviceCombo);

    // 检查是否有符合的显示大小
    iBestDisplayMode = -1;
    iHighDisplayMode = -1;
    iMaxRefreshRate = 75;
    iHighRefreshRate = iBestRefreshRate = 0;
    pAdapterInfo = m_d3dSettings.PAdapterInfo();
    for (idm = 0; idm < pAdapterInfo->pDisplayModeList->Count(); idm++) {
        pdm = (D3DDISPLAYMODE*)pAdapterInfo->pDisplayModeList->GetPtr(idm);
        if (pdm->Format != pDeviceCombo->AdapterFormat)
            continue;
        if (pdm->Width == dwWidth && pdm->Height == dwHeight &&
            pdm->RefreshRate == iMaxRefreshRate) {
            // 找到最佳的显示方式了
            iBestDisplayMode = idm;
            break;
        } else if (pdm->Width == dwWidth && pdm->Height == dwHeight &&
                   pdm->RefreshRate > iBestRefreshRate) {
            if (pdm->RefreshRate < iMaxRefreshRate) {
                iBestDisplayMode = idm;
                iBestRefreshRate = pdm->RefreshRate;
            } else {
                iHighDisplayMode = idm;
                iHighRefreshRate = pdm->RefreshRate;
            }
        }
    }

    if (iBestDisplayMode == -1 && iHighDisplayMode == -1) {
        RtCoreLog().Error("RtgDeviceD3D9: Device不支持指定的窗口大小\n");
        return false;
    }
    if (iBestDisplayMode == -1) {
        iBestDisplayMode = iHighDisplayMode;
    }

    // 赋值给变量
    m_d3dSettings.IsWindowed = !bFullscreen;
    m_bWindowed = m_d3dSettings.IsWindowed;
    if (bFullscreen) {
        m_d3dSettings.pFullscreen_DeviceCombo = pDeviceCombo;
        m_d3dSettings.Fullscreen_DisplayMode =
            *((D3DDISPLAYMODE*)pAdapterInfo->pDisplayModeList->GetPtr(iBestDisplayMode));
    } else {
        //m_d3dSettings.pWindowed_DeviceCombo = pDeviceCombo;
        //m_d3dSettings.Windowed_DisplayMode = *((D3DDISPLAYMODE*)pAdapterInfo->pDisplayModeList->GetPtr(iBestDisplayMode));
    }

    // 重新设置3D环境

    AdjustWindowForChange();

    if (!bFullscreen) {
        RECT rc;
        GetClientRect(m_hWnd, &rc);
        ClientToScreen(m_hWnd, (LPPOINT)&rc);
        rc.right = rc.left + dwWidth;
        rc.bottom = rc.top + dwHeight;
        AdjustWindowRect(&rc, m_dwWindowStyle, FALSE);
        if (rc.left < 0) {
            rc.right -= rc.left;
            rc.left = 0;
        }
        if (rc.top < 0) {
            rc.bottom -= rc.top;
            rc.top = 0;
        }
        MoveWindow(m_hWnd, rc.left, rc.top, rc.right - rc.left, rc.bottom - rc.top, true);

        // Update window properties
        GetWindowRect(m_hWnd, &m_rcWindowBounds);
        GetClientRect(m_hWnd, &m_rcWindowClient);

        m_d3dSettings.Windowed_Width = m_rcWindowClient.right - m_rcWindowClient.left;
        m_d3dSettings.Windowed_Height = m_rcWindowClient.bottom - m_rcWindowClient.top;
    }

    BuildPresentParamsFromSettings();
    Reset3DEnvironment();
    return true;
}

void RtgDeviceD3D9::OnWindowSizeChanged() {
    HandlePossibleSizeChange();
}

bool RtgDeviceD3D9::OnPreCreate3DDevice() {
    m_pAppFrame = (CRtgAppFrame*)m_pEvent;
    RtCoreLog().Info("RtgDeviceD3D9::OnPreCreate3DDevice\n");
    m_pVB = RT_NEW D3DVertexStreamManager(this);
    //m_pIB = RT_NEW D3DIndexStreamManager(this);
    RtgDevice::OnPreCreate3DDevice();
    return true;
}

bool RtgDeviceD3D9::OnCreate3DDevice() {
    RtCoreLog().Info("RtgDeviceD3D9::OnCreate3DDevice\n");
    // 如果没有应用的实例，就取得当前应用的实例
    if (m_hAppInstance == NULL) {
        m_hAppInstance = (HINSTANCE)GetModuleHandle(NULL);
    }

    m_hRenderWnd = (HWND)m_ViewWnd[RTGVWM_WINDOWED].hWnd;
    m_hRenderFullWnd = (HWND)m_ViewWnd[RTGVWM_FULLSCREEN].hWnd;

    CD3DApplication::m_hWnd = m_hRenderWnd;
    CD3DApplication::m_hWndFocus = m_hRenderFullWnd;

    // 默认创建窗口的参数，只有创建默认窗口才有效
    CD3DApplication::m_dwCreationWidth = m_userConfig.lWndWidth;
    CD3DApplication::m_dwCreationHeight = m_userConfig.lWndHeight;
    /*CD3DApplication::m_iDefaultStartX = m_config.iDefaultStartX;
	CD3DApplication::m_iDefaultStartY = m_config.iDefaultStartY;*/
    CD3DApplication::m_strWindowTitle = m_config.szWindowTitle;
    CD3DApplication::m_bvsync = m_userConfig.bvsync;

    if (m_hRenderFullWnd)
        m_bCustomToggleFullscreen = true;
    //lyymark 1.device创建主窗口在这边进去
    if (FAILED(CD3DApplication::Create((HINSTANCE)m_hAppInstance))) {
        RtCoreLog().Error("RtgDeviceD3D9错误: 创建D3D设备出错.\n");
        return false;
    }

    m_hRenderWnd = CD3DApplication::m_hWnd;
    m_hRenderFullWnd = CD3DApplication::m_hWndFocus;
    m_ViewWnd[RTGVWM_WINDOWED].hWnd = m_hRenderWnd;
    m_ViewWnd[RTGVWM_TOPWINDOW].hWnd = m_hRenderWnd;
    m_ViewWnd[RTGVWM_FULLSCREEN].hWnd = m_hRenderFullWnd;
    return true;
}

void RtgDeviceD3D9::OnGetDeviceCaps() {
    m_Ability.bFullScreneAntialiasing = true;  // 这个函数已经不用, 并且需要每次切换驱动器的时候检查
    m_Ability.bVertexShader = m_d3dCaps.VertexShaderVersion >= D3DVS_VERSION(1, 1);
    m_Ability.bPixelShader = m_d3dCaps.PixelShaderVersion >= D3DPS_VERSION(1, 1);
    m_Ability.iMaxTextureWidth = m_d3dCaps.MaxTextureWidth;
    m_Ability.iMaxTextureHeight = m_d3dCaps.MaxTextureHeight;
    m_Ability.bSquareTexture = (m_d3dCaps.TextureCaps & D3DPTEXTURECAPS_SQUAREONLY) ? 1 : 0;
    m_Ability.iMaxMultiTextures = m_d3dCaps.MaxSimultaneousTextures;
    m_Ability.iMaxTextureStages = m_d3dCaps.MaxTextureBlendStages;
    m_Ability.bEnvBumpmap = (m_d3dCaps.TextureOpCaps & D3DTOP_BUMPENVMAP) ? true : false;
    m_Ability.bDot3ProductBumpmap =
        (m_d3dCaps.TextureOpCaps & D3DTEXOPCAPS_DOTPRODUCT3) ? true : false;
    m_Ability.bTextureOpMultiplyAdd =
        (m_d3dCaps.TextureOpCaps & D3DTEXOPCAPS_MULTIPLYADD) ? true : false;
    m_Ability.bDynamicTextures = (m_d3dCaps.Caps2 & D3DCAPS2_DYNAMICTEXTURES) ? true : false;

    m_Ability.iMaxPrimitiveCount = m_d3dCaps.MaxPrimitiveCount;
    m_Ability.iMaxVertexIndex = m_d3dCaps.MaxVertexIndex;
    m_Ability.iMaxStreams = m_d3dCaps.MaxStreams;
    m_Ability.iMaxStreamStride = m_d3dCaps.MaxStreamStride;

    RtgImage::m_iMaxWidth = m_Ability.iMaxTextureWidth;
    RtgImage::m_iMaxHeight = m_Ability.iMaxTextureHeight;
    RtgImage::m_bSquare = m_Ability.bSquareTexture;
    RtgTexture::m_iMaxMultiTextures = m_Ability.iMaxMultiTextures;

    int i;
    for (i = 0; i < m_Ability.iMaxMultiTextures; i++) {
        m_bTexture[i] = TRUE;
    }
    for (; i < 8; i++) {
        m_bTexture[i] = FALSE;
    }

    // 输出
    RtCoreLog().Info(
        "----- Device D3D9 ----------------------------------------------------------\n");
    RtCoreLog().Info("  设备状态： %s\n", m_strDeviceStats);
    RtCoreLog().Info("  检查设备能力.\n");
    RtCoreLog().Info("    最大贴图大小 (%dx%d).\n", m_Ability.iMaxTextureWidth,
                     m_Ability.iMaxTextureHeight);
    RtCoreLog().Info("    最多多层贴图数 %d.\n", m_Ability.iMaxMultiTextures);
    if (!m_Ability.bFullScreneAntialiasing)
        RtCoreLog().Info("    不支持全屏幕抗锯齿.\n");
    if (!m_Ability.bVertexShader)
        RtCoreLog().Info("    不支持 Vertex Shader.\n");
    if (!m_Ability.bPixelShader)
        RtCoreLog().Info("    不支持 Pixel Shader.\n");
    if (!m_Ability.bEnvBumpmap)
        RtCoreLog().Info("    不支持 D3DTOP_BUMPENVMAP.\n");
    if (!m_Ability.bDot3ProductBumpmap)
        RtCoreLog().Info("    不支持 D3DTEXOPCAPS_DOTPRODUCT3.\n");
    if (!m_Ability.bTextureOpMultiplyAdd)
        RtCoreLog().Info("    不支持 D3DTEXOPCAPS_MULTIPLYADD.\n");
    if (!m_Ability.bDynamicTextures)
        RtCoreLog().Info("    不支持 动态贴图.\n");
    if (m_Ability.bSquareTexture)
        RtCoreLog().Info("    贴图必须是方形的.\n");
    if (!(m_d3dCaps.TextureAddressCaps & D3DPTADDRESSCAPS_BORDER))
        RtCoreLog().Info("    不支持贴图Border模式.\n");
    if (!(m_d3dCaps.TextureAddressCaps & D3DPTADDRESSCAPS_CLAMP))
        RtCoreLog().Info("    不支持贴图Clamp模式.\n");
    if (!(m_d3dCaps.TextureAddressCaps & D3DPTADDRESSCAPS_WRAP))
        RtCoreLog().Info("    不支持贴图Wrap模式.\n");
    if (!(m_d3dCaps.RasterCaps & D3DPRASTERCAPS_FOGRANGE))
        RtCoreLog().Info("    不支持 Fog Range.\n");
    if (!(m_d3dCaps.RasterCaps & D3DPRASTERCAPS_FOGTABLE))
        RtCoreLog().Info("    不支持 Fog Table.\n");
    if (!(m_d3dCaps.RasterCaps & D3DPRASTERCAPS_FOGVERTEX))
        RtCoreLog().Info("    不支持 Fog Vertex.\n");
    if (!(m_d3dCaps.TextureCaps & D3DPTEXTURECAPS_MIPMAP))
        RtCoreLog().Info("    不支持 D3DPTEXTURECAPS_MIPMAP.\n");

    if (!(m_d3dCaps.TextureFilterCaps & D3DPTFILTERCAPS_MAGFLINEAR)) {
        RtCoreLog().Info("    不支持 D3DPTFILTERCAPS_MAGFLINEAR.\n");
        this->m_eTextureFilterMag = RTGTEXF_POINT;
        if (!(m_d3dCaps.TextureFilterCaps & D3DPTFILTERCAPS_MAGFPOINT)) {
            RtCoreLog().Info("    不支持 D3DPTFILTERCAPS_MAGFPOINT.\n");
            this->m_eTextureFilterMag = RTGTEXF_NONE;
        }
    }
    if (!(m_d3dCaps.TextureFilterCaps & D3DPTFILTERCAPS_MINFLINEAR)) {
        RtCoreLog().Info("    不支持 D3DPTFILTERCAPS_MINFLINEAR.\n");
        this->m_eTextureFilterMin = RTGTEXF_POINT;
        if (!(m_d3dCaps.TextureFilterCaps & D3DPTFILTERCAPS_MINFPOINT)) {
            RtCoreLog().Info("    不支持 D3DPTFILTERCAPS_MINFPOINT.\n");
            this->m_eTextureFilterMin = RTGTEXF_NONE;
        }
    }
    if (!(m_d3dCaps.TextureFilterCaps & D3DPTFILTERCAPS_MIPFLINEAR)) {
        RtCoreLog().Info("    不支持 D3DPTFILTERCAPS_MIPFLINEAR.\n");
        this->m_eTextureFilterMip = RTGTEXF_POINT;
        if (!(m_d3dCaps.TextureFilterCaps & D3DPTFILTERCAPS_MIPFPOINT)) {
            RtCoreLog().Info("    不支持 D3DPTFILTERCAPS_MIPFPOINT.\n");
            this->m_eTextureFilterMip = RTGTEXF_NONE;
        }
    }
    // 输出结束
}

bool RtgDeviceD3D9::OnAfterCreate3DDevice() {
    int i;

    // Texture Format
    TextureFormat texFormats[] = {
        RTG_TEXTURE_A8R8G8B8, 0, 32, 8, 8, 8, 8, Rtgimg_Fill_A8R8G8B8,  // 32 bits
        RTG_TEXTURE_X8R8G8B8, 0, 32, 0, 8, 8, 8, Rtgimg_Fill_X8R8G8B8,  // 32 bits
        RTG_TEXTURE_R8G8B8,   0, 24, 0, 8, 8, 8, Rtgimg_Fill_R8G8B8,    // 24 bits
        RTG_TEXTURE_A4R4G4B4, 0, 16, 4, 4, 4, 4, Rtgimg_Fill_A4R4G4B4,  // 16 bits
        RTG_TEXTURE_A1R5G5B5, 0, 16, 1, 5, 5, 5, Rtgimg_Fill_A8R3G3B2,  // 16 bits
        RTG_TEXTURE_R5G6B5,   0, 16, 0, 5, 6, 5, Rtgimg_Fill_A1R5G5B5,  // 16 bits
        RTG_TEXTURE_X1R5G5B5, 0, 16, 0, 5, 5, 5, Rtgimg_Fill_R5G6B5,    // 16 bits
        RTG_TEXTURE_X4R4G4B4, 0, 16, 0, 4, 4, 4, Rtgimg_Fill_X1R5G5B5,  // 16 bits
    };

    D3DFORMAT d3dPixelFormat = m_d3dSettings.BackBufferFormat();

    m_iTexNumFormats = 0;
    if (m_d3dSettings.PDeviceInfo()->DevType == D3DDEVTYPE_HAL) {  // 硬件
        for (i = 0; i < sizeof(texFormats) / sizeof(TextureFormat); i++) {
            if (SUCCEEDED(m_pD3D->CheckDeviceFormat(
                    m_d3dSettings.AdapterOrdinal(), D3DDEVTYPE_HAL, d3dPixelFormat, 0,
                    D3DRTYPE_TEXTURE, enumRTGTextureFormat[texFormats[i].m_eFormat]))) {
                texFormats[i].m_bSupported = 1;
                m_TexFormat[m_iTexNumFormats++] = texFormats[i];
            }
        }
        if (SUCCEEDED(m_pD3D->CheckDeviceFormat(m_d3dSettings.AdapterOrdinal(), D3DDEVTYPE_HAL,
                                                d3dPixelFormat, 0, D3DRTYPE_TEXTURE,
                                                D3DFMT_DXT5))) {
            RtgImage::m_bSupportDXT = true;
        }
    } else {  // 软件
        for (i = 0; i < sizeof(texFormats) / sizeof(TextureFormat); i++) {
            texFormats[i].m_bSupported = 1;
            m_TexFormat[m_iTexNumFormats++] = texFormats[i];
        }
    }

    if (m_iTexNumFormats == 0) {
        RtCoreLog().Warn("RtgDeviceD3D9错误: 硬件不支持任何贴图格式.");
        return false;
    }
    m_pTexColorFormat = m_TexFormat;
    for (i = 0; i < m_iTexNumFormats; i++) {
        if (m_TexFormat[i].m_iAlphaBits) {
            m_pTexAlphaFormat = m_TexFormat + i;
            break;
        }
    }
    if (i == m_iTexNumFormats) {
        RtCoreLog().Warn("RtgDeviceD3D9错误: 硬件不支持任何Alpha贴图.");
        return false;
    }

#ifndef NO_DEBUG_INFO
    char* pTexFormatsName[] = {"A8R8G8B8", "X8R8G8B8", "R8G8B8  ", "A4R4G4B4",
                               "A1R5G5B5", "R5G6B5  ", "X1R5G5B5", "X4R4G4B4"};
    RtCoreLog().Info("RtgDeviceD3D9: 支持图形格式列表 (Backbuffer格式为[%s]):\n",
                     D3DUtil_D3DFormatToString(d3dPixelFormat, false));
    for (i = 0; i < m_iTexNumFormats; i++) {
        RtCoreLog().Info("    %2d: [%s] %d Bits, %d Alpha", i, pTexFormatsName[i],
                         m_TexFormat[i].m_iBits, m_TexFormat[i].m_iAlphaBits);
        if (m_TexFormat[i].m_eFormat == m_pTexColorFormat->m_eFormat) {
            RtCoreLog().Info(" [默认格式]");
        }
        if (m_TexFormat[i].m_eFormat == m_pTexAlphaFormat->m_eFormat) {
            RtCoreLog().Info(" [默认Alpha格式]");
        }
        RtCoreLog().Info("\n");
    }
#endif

    // 取得所有的RenderState默认值
    for (i = 0; i < RTGRS_MAX; i++) {
        m_pd3dDevice->GetRenderState(enumRTGRenderState[i], &(m_listRenderState[i].dwValue));
    }

    RtgDevice::OnAfterCreate3DDevice();

    return true;
}

bool RtgDeviceD3D9::OnInit() {
    ((CRtgAppFrame*)m_pEvent)->Init(this);

    DEVMODE devMode;
    memset(&devMode, 0, sizeof(DEVMODE));
    devMode.dmSize = sizeof(DEVMODE);
    EnumDisplaySettings(NULL, ENUM_CURRENT_SETTINGS, &devMode);
    m_iWndWidth = m_d3dsdBackBuffer.Width;
    m_iWndHeight = m_d3dsdBackBuffer.Height;
    m_iWndColorBits = devMode.dmBitsPerPel;
    m_dwRefreshRate = devMode.dmDisplayFrequency;
    switch (m_d3dsdBackBuffer.Format) {
        case D3DFMT_A8R8G8B8:
            m_eBackbufferFormat = RTG_TEXTURE_A8R8G8B8;
            break;
        case D3DFMT_X8R8G8B8:
            m_eBackbufferFormat = RTG_TEXTURE_X8R8G8B8;
            break;
        case D3DFMT_R8G8B8:
            m_eBackbufferFormat = RTG_TEXTURE_R8G8B8;
            break;
        case D3DFMT_A4R4G4B4:
            m_eBackbufferFormat = RTG_TEXTURE_A4R4G4B4;
            break;
        case D3DFMT_A1R5G5B5:
            m_eBackbufferFormat = RTG_TEXTURE_A1R5G5B5;
            break;
        case D3DFMT_R5G6B5:
            m_eBackbufferFormat = RTG_TEXTURE_R5G6B5;
            break;
        case D3DFMT_X1R5G5B5:
            m_eBackbufferFormat = RTG_TEXTURE_X1R5G5B5;
            break;
        case D3DFMT_X4R4G4B4:
            m_eBackbufferFormat = RTG_TEXTURE_X4R4G4B4;
            break;
        default:
            RtCoreLog().Warn("RtgDeviceD3D8: 不支持的Backbuffer格式\n");
            break;
    }

    /*  m_Texture.InitTexture();
    m_Texture.SetTextureQuality(m_userConfig.lTextureQuality);
    m_Texture.SelectTextureFormat(m_iWndColorBits);*/

    m_pBufferManager = RT_NEW RtgBufferAllocator(m_pd3dDevice);
    m_pBufferManager->Init();

    m_pEffectManager = RT_NEW RtgEffectManager(m_pd3dDevice);
    m_pEffectManager->Init();

    m_pTextMamager = RT_NEW RtgTextureManager(m_pd3dDevice);
    m_pTextMamager->Init();

    m_pRTManager = RT_NEW RtgRenderTargetManager(m_pd3dDevice);
    m_pRTManager->Init();

    D3DVERTEXELEMENT9 veVnt[] = {
        {0, 0, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0},
        {0, 12, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_NORMAL, 0},
        {0, 24, D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 0},
        D3DDECL_END()};

    D3DVERTEXELEMENT9 veVnct[] = {
        {0, 0, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0},
        {0, 12, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_NORMAL, 0},
        {0, 24, D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 0},
        {1, 0, D3DDECLTYPE_D3DCOLOR, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_COLOR, 0},
        D3DDECL_END()};

    m_pd3dDevice->CreateVertexDeclaration(veVnt, &m_ActDecl_vnt);
    m_pd3dDevice->CreateVertexDeclaration(veVnct, &m_ActDecl_vnct);

    return RtgDevice::OnInit();
}

void RtgDeviceD3D9::OnClose() {

    RtgDevice::OnClose();
    CD3DApplication::Cleanup3DEnvironment();

    //if (m_pEffectManager)
    //    delete m_pEffectManager;

    //if (m_pTextMamager)
    //    delete m_pTextMamager;

    DEL_ONE(m_pVB);
    //DEL_ONE(m_pIB);
}

int RtgDeviceD3D9::OnRun() {
    return CD3DApplication::Run();
}

void RtgDeviceD3D9::RenderScene() {

    if (FAILED(Render3DEnvironment())) {
        RtCoreLog().Warn("RtgDeviceD3D9错误: 渲染错误.\n");
        SendMessage(m_hWnd, WM_CLOSE, 0, 0);
    }
}

BOOL RtgDeviceD3D9::SetRenderState(RTGRenderState eState, DWORD dwValue) {
    LOG_FUNCTION;
    CHECK(m_pd3dDevice != 0);
    m_listRenderState[eState].bChanged = TRUE;
    m_listRenderState[eState].dwValue = dwValue;
    switch (eState) {
        case RTGRS_COLOR_WRITE:
            if (dwValue)
                dwValue = D3DCOLORWRITEENABLE_ALPHA | D3DCOLORWRITEENABLE_BLUE |
                          D3DCOLORWRITEENABLE_GREEN | D3DCOLORWRITEENABLE_RED;
            else
                dwValue = 0x00;
            break;
    }
    return m_pd3dDevice->SetRenderState(enumRTGRenderState[eState], dwValue) == D3D_OK;
}

DWORD RtgDeviceD3D9::GetRenderState(RTGRenderState eState) {
    return m_listRenderState[eState].dwValue;
}

BOOL RtgDeviceD3D9::SetLight(int iIdx, RtgLight* pLight) {
    LOG_FUNCTION;
    /*if (iIdx<0 || iIdx>=(int)m_d3dCaps.MaxActiveLights) return FALSE;*/
    if (pLight == NULL) {
        return m_pd3dDevice->LightEnable(iIdx, FALSE) == D3D_OK;
    }

    D3DLIGHT9 d3dLight;
    memset(&d3dLight, 0, sizeof(d3dLight));

    // Rotate through the various light types
    d3dLight.Type = enumRTGLightMode[pLight->eMode];

    // Make sure the light type is supported by the device.  If
    // D3DVTXPCAPS_POSITIONALLIGHTS is not set, the device does not support
    // point or spot lights, so change light #2's type to a directional light.
    DWORD dwCaps = m_d3dCaps.VertexProcessingCaps;
    if (0 == (dwCaps & D3DVTXPCAPS_POSITIONALLIGHTS)) {
        if (d3dLight.Type == D3DLIGHT_POINT || d3dLight.Type == D3DLIGHT_SPOT)
            d3dLight.Type = D3DLIGHT_DIRECTIONAL;
    }

    d3dLight.Diffuse.r = pLight->vDiffuse.x;
    d3dLight.Diffuse.g = pLight->vDiffuse.y;
    d3dLight.Diffuse.b = pLight->vDiffuse.z;
    d3dLight.Specular.r = pLight->vSpecular.x;
    d3dLight.Specular.g = pLight->vSpecular.y;
    d3dLight.Specular.b = pLight->vSpecular.z;
    d3dLight.Range = pLight->fRange;
    d3dLight.Attenuation0 = pLight->fConstantAttenuation;
    d3dLight.Attenuation1 = pLight->fLinearAttenuation;
    d3dLight.Attenuation2 = pLight->fQuadraticAttenuation;

    switch (d3dLight.Type) {
        case D3DLIGHT_POINT:
            d3dLight.Position.x = pLight->vPosition.x;
            d3dLight.Position.y = pLight->vPosition.y;
            d3dLight.Position.z = pLight->vPosition.z;
            break;
        case D3DLIGHT_DIRECTIONAL:
            d3dLight.Direction.x = pLight->vDirection.x;
            d3dLight.Direction.y = pLight->vDirection.y;
            d3dLight.Direction.z = pLight->vDirection.z;
            break;
        case D3DLIGHT_SPOT:
            d3dLight.Position.x = pLight->vPosition.x;
            d3dLight.Position.y = pLight->vPosition.y;
            d3dLight.Position.z = pLight->vPosition.z;
            d3dLight.Direction.x = pLight->vDirection.x;
            d3dLight.Direction.y = pLight->vDirection.y;
            d3dLight.Direction.z = pLight->vDirection.z;
            d3dLight.Theta = pLight->fTheta;
            d3dLight.Phi = pLight->fPhi;
            d3dLight.Falloff = pLight->fFallOff;
            break;
    }

    m_pd3dDevice->LightEnable(iIdx, TRUE);
    return m_pd3dDevice->SetLight(iIdx, &d3dLight) == D3D_OK;
}

BOOL RtgDeviceD3D9::OnSetFVF() {
    LOG_FUNCTION;
    m_dwVertexFVF = D3DFVF_XYZ;
    switch (m_eVertexFormat) {
        case RTG_VNCT:
            m_dwVertexFVF |= D3DFVF_NORMAL | D3DFVF_DIFFUSE | D3DFVF_TEX1 | D3DFVF_TEXCOORDSIZE2(0);
            break;
        case RTG_VNCT2:
            m_dwVertexFVF |= D3DFVF_NORMAL | D3DFVF_DIFFUSE | D3DFVF_TEX2 |
                             D3DFVF_TEXCOORDSIZE2(0) | D3DFVF_TEXCOORDSIZE2(1);
            break;
        case RTG_VNCT3:
            m_dwVertexFVF |= D3DFVF_NORMAL | D3DFVF_DIFFUSE | D3DFVF_TEX3 | D3DFVF_TEXCOORDSIZE2(0);
            break;
        case RTG_VNCT4:
            m_dwVertexFVF |= D3DFVF_NORMAL | D3DFVF_DIFFUSE | D3DFVF_TEX4 | D3DFVF_TEXCOORDSIZE2(0);
            break;
        case RTG_VNT:
            m_dwVertexFVF |= D3DFVF_NORMAL | D3DFVF_TEX1 | D3DFVF_TEXCOORDSIZE2(0);
            break;
        case RTG_VNT2:
            m_dwVertexFVF |= D3DFVF_NORMAL | D3DFVF_TEX2 | D3DFVF_TEXCOORDSIZE2(0);
            break;
        case RTG_VNT3:
            m_dwVertexFVF |= D3DFVF_NORMAL | D3DFVF_TEX3 | D3DFVF_TEXCOORDSIZE2(0);
            break;
        case RTG_VNT4:
            m_dwVertexFVF |= D3DFVF_NORMAL | D3DFVF_TEX4 | D3DFVF_TEXCOORDSIZE2(0);
            break;
        case RTG_VCT:
            m_dwVertexFVF |= D3DFVF_DIFFUSE | D3DFVF_TEX1 | D3DFVF_TEXCOORDSIZE2(0);
            break;
        case RTG_VCT2:
            m_dwVertexFVF |= D3DFVF_DIFFUSE | D3DFVF_TEX2 | D3DFVF_TEXCOORDSIZE2(0);
            break;
        case RTG_VCT3:
            m_dwVertexFVF |= D3DFVF_DIFFUSE | D3DFVF_TEX3 | D3DFVF_TEXCOORDSIZE2(0);
            break;
        case RTG_VCT4:
            m_dwVertexFVF |= D3DFVF_DIFFUSE | D3DFVF_TEX4 | D3DFVF_TEXCOORDSIZE2(0);
            break;
        case RTG_VN:
            m_dwVertexFVF |= D3DFVF_NORMAL;
            break;
        case RTG_VC:
            m_dwVertexFVF |= D3DFVF_DIFFUSE;
            break;
        case RTG_VT:
            m_dwVertexFVF |= D3DFVF_TEX1 | D3DFVF_TEXCOORDSIZE2(0);
            break;
        case RTG_VT2:
            m_dwVertexFVF |= D3DFVF_TEX2 | D3DFVF_TEXCOORDSIZE2(0);
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
            CHECKEX("RtgDeviceD3D9: 目前不支持VertexShader");
            break;
    }
    m_dwVertexStride = 0;
    if (m_dwVertexFVF & D3DFVF_XYZ)
        m_dwVertexStride += sizeof(float) * 3;
    if (m_dwVertexFVF & D3DFVF_XYZRHW)
        m_dwVertexStride += sizeof(float) * 4;
    if (m_dwVertexFVF & D3DFVF_NORMAL)
        m_dwVertexStride += sizeof(float) * 3;
    if (m_dwVertexFVF & D3DFVF_DIFFUSE)
        m_dwVertexStride += sizeof(DWORD);

    if ((m_dwVertexFVF & 0x0F00) == D3DFVF_TEX1)
        m_dwVertexStride += sizeof(float) * 2 * 1;
    else if ((m_dwVertexFVF & 0x0F00) == D3DFVF_TEX2)
        m_dwVertexStride += sizeof(float) * 2 * 2;
    else if ((m_dwVertexFVF & 0x0F00) == D3DFVF_TEX3)
        m_dwVertexStride += sizeof(float) * 2 * 3;
    else if ((m_dwVertexFVF & 0x0F00) == D3DFVF_TEX4)
        m_dwVertexStride += sizeof(float) * 2 * 4;

    return SetFVF(m_dwVertexFVF);
}

void RtgDeviceD3D9::SetMatrix(RTGMatrix eMode, RtgMatrix16* pM16) {
    CHECK(eMode != RTGTS_TEXTURE);
    m_pd3dDevice->SetTransform(enumRTGMatrix[eMode], (D3DMATRIX*)pM16);
}

void RtgDeviceD3D9::GetMatrix(RTGMatrix eMode, RtgMatrix16* pM16) {
    CHECK(eMode != RTGTS_TEXTURE);
    m_pd3dDevice->GetTransform(enumRTGMatrix[eMode], (D3DMATRIX*)pM16);
}

void RtgDeviceD3D9::SetTextureMatrix(int iIdx, RtgMatrix16* pM16) {
    CHECK(iIdx >= 0 && iIdx < m_Ability.iMaxMultiTextures);
    m_pd3dDevice->SetTransform(enumRTGMatrix[RTGTS_TEXTURE + iIdx], (D3DMATRIX*)pM16);
}

void RtgDeviceD3D9::GetTextureMatrix(int iIdx, RtgMatrix16* pM16) {
    CHECK(iIdx > 0 && iIdx < m_Ability.iMaxMultiTextures);
    m_pd3dDevice->GetTransform(enumRTGMatrix[RTGTS_TEXTURE + iIdx], (D3DMATRIX*)pM16);
}

D3DPRIMITIVETYPE RtgDeviceD3D9::GetD3DPT(RTGPrimitive pt) const {
    return enumRTGPrimitive[pt];
}

//
//BOOL RtgDeviceD3D9::DrawPrimitive(RTGPrimitive eType, void *pVertices, UINT dwVerticesCount, UINT dwPrimitiveCount)
//{
//    LOG_FUNCTION;
//    STAT(m_iLastRenderNumFaces+=dwPrimitiveCount);
//    STAT(m_iLastRenderDrawPrimitive++);
//    CHECK(pVertices!=NULL && dwVerticesCount!=0);
//    return m_pd3dDevice->DrawPrimitiveUP(enumRTGPrimitive[eType], dwPrimitiveCount, pVertices, m_dwVertexStride)==D3D_OK;
//}
//
BOOL RtgDeviceD3D9::DrawPrimitiveVB(RTGPrimitive eType, void* pVertices, UINT dwVertexStart,
                                    UINT dwVerticesCount, UINT dwPrimitiveCount) {
    LOG_FUNCTION;
    STAT(m_iLastRenderNumFaces += dwPrimitiveCount);
    STAT(m_iLastRenderDrawPrimitiveVB++);
    CHECK(pVertices != NULL && dwVerticesCount != 0);
    m_pd3dDevice->SetStreamSource(0, (IDirect3DVertexBuffer9*)pVertices,
                                  dwVertexStart * m_dwVertexStride, m_dwVertexStride);
    return m_pd3dDevice->DrawPrimitive(enumRTGPrimitive[eType], dwVertexStart, dwPrimitiveCount) ==
           D3D_OK;
}

BOOL RtgDeviceD3D9::DrawPrimitiveVB(RTGPrimitive eType, UINT dwVertexStart, UINT dwVerticesCount,
                                    UINT dwPrimitiveCount) {
    LOG_FUNCTION;
    STAT(m_iLastRenderNumFaces += dwPrimitiveCount);
    STAT(m_iLastRenderDrawPrimitiveVB++);
    CHECK(dwVerticesCount != 0);
    return m_pd3dDevice->DrawPrimitive(enumRTGPrimitive[eType],
                                       dwVertexStart + m_dwVertexBufferStartOffset,
                                       dwPrimitiveCount) == D3D_OK;
}

//
//BOOL RtgDeviceD3D9::DrawIndexPrimitive(RTGPrimitive eType, void *pVertices, UINT dwVerticesCount, void *pIndices, UINT dwIndicesCount, UINT dwPrimitiveCount)
//{
//    LOG_FUNCTION;
//    STAT(m_iLastRenderNumFaces+=dwPrimitiveCount);
//    STAT(m_iLastRenderDrawIndexPrimitive++);
//	if (dwPrimitiveCount==-1) dwPrimitiveCount = dwIndicesCount/3;
//    CHECK(pVertices!=NULL && dwVerticesCount!=0 && dwIndicesCount!=0);
//    return m_pd3dDevice->DrawIndexedPrimitiveUP(enumRTGPrimitive[eType], 0, dwVerticesCount, dwPrimitiveCount, (unsigned short*)pIndices, D3DFMT_INDEX16, pVertices, m_dwVertexStride)==D3D_OK;
//}
//
//BOOL RtgDeviceD3D9::DrawIndexPrimitiveVB(RTGPrimitive eType, void *pVertices, UINT dwVerticesCount, void *pIndices, UINT dwIndicesCount, UINT dwPrimitiveCount)
//{
//    LOG_FUNCTION;
//    STAT(m_iLastRenderNumFaces+=dwPrimitiveCount);
//    STAT(m_iLastRenderDrawIndexPrimitiveVB++);
//    CHECK(pVertices!=NULL && dwVerticesCount!=0 && dwIndicesCount!=0);
//	m_pd3dDevice->SetStreamSource(0, (IDirect3DVertexBuffer9*)pVertices, 0, m_dwVertexStride);
//	m_pd3dDevice->SetIndices((IDirect3DIndexBuffer9*)pIndices);
//	return m_pd3dDevice->DrawIndexedPrimitive(enumRTGPrimitive[eType], 0, 0, dwVerticesCount, 0, dwPrimitiveCount)==D3D_OK;
//}
//
//BOOL RtgDeviceD3D9::DrawIndexPrimitiveVB(RTGPrimitive eType, void *pVertices, UINT dwVertexStart, UINT dwVerticesCount, void *pIndices, UINT dwIndexStart, UINT dwIndicesCount, UINT dwPrimitiveCount)
//{
//    LOG_FUNCTION;
//    STAT(m_iLastRenderNumFaces+=dwPrimitiveCount);
//    STAT(m_iLastRenderDrawIndexPrimitiveVB++);
//    CHECK(pVertices!=NULL && dwVerticesCount!=0 && dwIndicesCount!=0);
//	m_pd3dDevice->SetStreamSource(0, (IDirect3DVertexBuffer9*)pVertices, dwVertexStart*m_dwVertexStride, m_dwVertexStride);
//	m_pd3dDevice->SetIndices((IDirect3DIndexBuffer9*)pIndices);
//	return m_pd3dDevice->DrawIndexedPrimitive(enumRTGPrimitive[eType], dwVertexStart, 0, dwVerticesCount, dwIndexStart, dwPrimitiveCount)==D3D_OK;
//}
//
//BOOL RtgDeviceD3D9::DrawIndexPrimitiveVB(RTGPrimitive eType, UINT dwVertexStart, UINT dwVerticesCount, UINT dwIndexStart, UINT dwPrimitiveCount)
//{
//    LOG_FUNCTION;
//    STAT(m_iLastRenderNumFaces+=dwPrimitiveCount);
//    STAT(m_iLastRenderDrawIndexPrimitiveVB++);
//    CHECK(dwVerticesCount!=0);
//	return m_pd3dDevice->DrawIndexedPrimitive(enumRTGPrimitive[eType], dwVertexStart+m_dwVertexBufferStartOffset, 0, dwVerticesCount, dwIndexStart+m_dwIndexBufferStartOffset, dwPrimitiveCount)==D3D_OK;
//}
//
//BOOL RtgDeviceD3D9::SetStreamSource(UINT _StreamIndex, RtgBufferItem* vb, UINT _Stride)
//{
//    RTASSERT(vb);
//    RTASSERT(vb->Format != D3DFMT_INDEX16 && vb->Format != D3DFMT_INDEX32);
//
//    if (SUCCEEDED(m_pd3dDevice->SetStreamSource(_StreamIndex, (IDirect3DVertexBuffer9*)vb->pBuffer, 0, _Stride)))
//        return TRUE;
//    return FALSE;
//}
//
////BOOL RtgDeviceD3D9::SetIndices(RtgBufferItem* ib)
////{
////    RTASSERT(ib);
////    RTASSERT(ib->Format == D3DFMT_INDEX16 || ib->Format == D3DFMT_INDEX32);
////
////    if (SUCCEEDED(m_pd3dDevice->SetIndices((IDirect3DIndexBuffer9*)ib->pBuffer)))
////        return TRUE;
////    return FALSE;
////}
//
BOOL RtgDeviceD3D9::SetStreamSource(UINT dwStreamNumber, void* pStreamData, UINT dwStride) {
    return m_pd3dDevice->SetStreamSource(dwStreamNumber, (IDirect3DVertexBuffer9*)pStreamData, 0,
                                         dwStride) == D3D_OK;
}

BOOL RtgDeviceD3D9::SetIndices(void* pIndexData) {
    return m_pd3dDevice->SetIndices((IDirect3DIndexBuffer9*)pIndexData) == D3D_OK;
}

//void RtgDeviceD3D9::DisableStreamSource(UINT _StreamIndex)
//{
//    m_pd3dDevice->SetStreamSource(_StreamIndex, NULL, 0, 0);
//}
//
BOOL RtgDeviceD3D9::CreateVertexBuffer(UINT dwLength, void** ppVertexBuffer, DWORD dwUsage) {
    LOG_FUNCTION;
    DWORD dwD3DUsage = 0;
    if (dwUsage & RTGU_RENDERTARGET)
        dwD3DUsage |= D3DUSAGE_RENDERTARGET;
    if (dwUsage & RTGU_DEPTHSTENCIL)
        dwD3DUsage |= D3DUSAGE_DEPTHSTENCIL;
    if (dwUsage & RTGU_WRITEONLY)
        dwD3DUsage |= D3DUSAGE_WRITEONLY;
    if (dwUsage & RTGU_SOFTWAREPROCESSING)
        dwD3DUsage |= D3DUSAGE_SOFTWAREPROCESSING;
    if (dwUsage & RTGU_DONOTCLIP)
        dwD3DUsage |= D3DUSAGE_DONOTCLIP;
    if (dwUsage & RTGU_POINTS)
        dwD3DUsage |= D3DUSAGE_POINTS;
    if (dwUsage & RTGU_RTPATCHES)
        dwD3DUsage |= D3DUSAGE_RTPATCHES;
    if (dwUsage & RTGU_NPATCHES)
        dwD3DUsage |= D3DUSAGE_NPATCHES;
    if (dwUsage & RTGU_DYNAMIC)
        dwD3DUsage |= D3DUSAGE_DYNAMIC;
    return m_pd3dDevice->CreateVertexBuffer(dwLength, dwD3DUsage, 0, D3DPOOL_DEFAULT,
                                            (IDirect3DVertexBuffer9**)ppVertexBuffer,
                                            NULL) == D3D_OK;
}

BOOL RtgDeviceD3D9::LockVertexBuffer(void* pVertexBuffer, void** ppData, DWORD dwOffset,
                                     DWORD dwSize, RTGLock eLockFlag) {
    LOG_FUNCTION;
    return ((LPDIRECT3DVERTEXBUFFER9)pVertexBuffer)
               ->Lock(dwOffset, dwSize, ppData, enumRTGLock[eLockFlag]) == D3D_OK;
}

BOOL RtgDeviceD3D9::UnLockVertexBuffer(void* pVertexBuffer) {
    LOG_FUNCTION;
    return (((LPDIRECT3DVERTEXBUFFER9)pVertexBuffer)->Unlock() == D3D_OK);
}

BOOL RtgDeviceD3D9::ReleaseVertexBuffer(void* pVertexBuffer) {
    LOG_FUNCTION;
    return (((LPDIRECT3DVERTEXBUFFER9)pVertexBuffer)->Release() == D3D_OK);
}

BOOL RtgDeviceD3D9::CreateIndexBuffer(UINT dwLength, void** ppIndexBuffer, DWORD dwUsage) {
    LOG_FUNCTION;
    DWORD dwD3DUsage = 0;
    if (dwUsage & RTGU_RENDERTARGET)
        dwD3DUsage |= RTGU_RENDERTARGET;
    if (dwUsage & RTGU_DEPTHSTENCIL)
        dwD3DUsage |= RTGU_DEPTHSTENCIL;
    if (dwUsage & RTGU_WRITEONLY)
        dwD3DUsage |= RTGU_WRITEONLY;
    if (dwUsage & RTGU_SOFTWAREPROCESSING)
        dwD3DUsage |= RTGU_SOFTWAREPROCESSING;
    if (dwUsage & RTGU_DONOTCLIP)
        dwD3DUsage |= RTGU_DONOTCLIP;
    if (dwUsage & RTGU_POINTS)
        dwD3DUsage |= RTGU_POINTS;
    if (dwUsage & RTGU_RTPATCHES)
        dwD3DUsage |= RTGU_RTPATCHES;
    if (dwUsage & RTGU_NPATCHES)
        dwD3DUsage |= RTGU_NPATCHES;
    if (dwUsage & RTGU_DYNAMIC)
        dwD3DUsage |= RTGU_DYNAMIC;
    return m_pd3dDevice->CreateIndexBuffer(dwLength, dwD3DUsage, D3DFMT_INDEX16, D3DPOOL_DEFAULT,
                                           (IDirect3DIndexBuffer9**)ppIndexBuffer, NULL) == D3D_OK;
}

BOOL RtgDeviceD3D9::LockIndexBuffer(void* pIndexBuffer, void** ppData, DWORD dwOffset, DWORD dwSize,
                                    RTGLock eLockFlag) {
    LOG_FUNCTION;
    return ((LPDIRECT3DINDEXBUFFER9)pIndexBuffer)
               ->Lock(dwOffset, dwSize, ppData, enumRTGLock[eLockFlag]) == D3D_OK;
}

BOOL RtgDeviceD3D9::UnLockIndexBuffer(void* pIndexBuffer) {
    LOG_FUNCTION;
    return (((LPDIRECT3DINDEXBUFFER9)pIndexBuffer)->Unlock() == D3D_OK);
}

BOOL RtgDeviceD3D9::ReleaseIndexBuffer(void* pIndexBuffer) {
    LOG_FUNCTION;
    return (((LPDIRECT3DINDEXBUFFER9)pIndexBuffer)->Release() == D3D_OK);
}

BOOL RtgDeviceD3D9::SelectTextureFormat(int iBits) {

    if (m_iTexNumFormats == 0) {
        RtCoreLog().Warn("ERROR: 硬件不支持任何贴图格式.");
        return FALSE;
    }
    if (iBits >= 24) {
        m_iTexNumBits = 32;
    } else {
        m_iTexNumBits = 16;
    }
    for (int i = 0; i < m_iTexNumFormats; i++) {
        if (m_iTexNumBits == m_TexFormat[i].m_iBits && m_TexFormat[i].m_iAlphaBits) {
            m_pTexAlphaFormat = m_pTexColorFormat = m_TexFormat + i;
            return TRUE;
        }
    }
    return FALSE;
}

void RtgDeviceD3D9::SetTextureFilterMip(RTGTextureFilter eType) {
    m_eTextureFilterMip = eType;
    for (int i = 0; i < this->m_Ability.iMaxMultiTextures; i++) {
        m_pd3dDevice->SetSamplerState(i, D3DSAMP_MIPFILTER,
                                      enumRTGTextureFilter[m_eTextureFilterMip]);
    }
}

void RtgDeviceD3D9::SetTextureFilterMin(RTGTextureFilter eType) {
    m_eTextureFilterMin = eType;
    for (int i = 0; i < this->m_Ability.iMaxMultiTextures; i++) {
        m_pd3dDevice->SetSamplerState(i, D3DSAMP_MINFILTER,
                                      enumRTGTextureFilter[m_eTextureFilterMin]);
    }
}

void RtgDeviceD3D9::SetTextureFilterMag(RTGTextureFilter eType) {
    m_eTextureFilterMag = eType;
    for (int i = 0; i < this->m_Ability.iMaxMultiTextures; i++) {
        m_pd3dDevice->SetSamplerState(i, D3DSAMP_MAGFILTER,
                                      enumRTGTextureFilter[m_eTextureFilterMag]);
    }
}

void RtgDeviceD3D9::SetTextureAddress(RTGTextureAddress eType) {
    m_eTextureAddress = eType;
    for (int i = 0; i < this->m_Ability.iMaxMultiTextures; i++) {
        m_pd3dDevice->SetSamplerState(i, D3DSAMP_ADDRESSU,
                                      enumRTGTextureAddress[m_eTextureAddress]);
        m_pd3dDevice->SetSamplerState(i, D3DSAMP_ADDRESSV,
                                      enumRTGTextureAddress[m_eTextureAddress]);
    }
}

void RtgDeviceD3D9::SetTextureBorderColor(DWORD dwColor) {
    m_dwTextureBorderColor = dwColor;
    for (int i = 0; i < this->m_Ability.iMaxMultiTextures; i++) {
        m_pd3dDevice->SetSamplerState(i, D3DSAMP_BORDERCOLOR, dwColor);
    }
}

void RtgDeviceD3D9::SetShader(RtgShader* pShader) {
    LOG_FUNCTION;
    m_pLastShader = pShader;
#ifdef _DEBUG
    CHECK(pShader->iNumStage <= m_Ability.iMaxMultiTextures);
#endif
    if (pShader->bColorKey) {
        if (m_bAlphaTest != TRUE) {
            m_bAlphaTest = TRUE;
            m_pd3dDevice->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);
            m_pd3dDevice->SetRenderState(D3DRS_ALPHAREF, (DWORD)pShader->alphaRef);
            m_pd3dDevice->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATEREQUAL);
        }
    } else {
        if (m_bAlphaTest != FALSE) {
            m_bAlphaTest = FALSE;
            m_pd3dDevice->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);
        }
    }

    if (pShader->bTwoSide) {
        if (m_bCullFace != FALSE) {
            m_bCullFace = FALSE;
            m_pd3dDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
        }
    } else {
        if (m_bCullFace != TRUE) {
            m_bCullFace = TRUE;
            m_pd3dDevice->SetRenderState(D3DRS_CULLMODE, m_dwCullDefault);
        }
    }

    if (pShader->bEnableBlendFunc) {
        if (m_bBlend != TRUE) {
            m_bBlend = TRUE;
            m_pd3dDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
        }
        if (m_eBlendSrcFunc != pShader->eBlendSrcFunc) {
            m_eBlendSrcFunc = pShader->eBlendSrcFunc;
            m_pd3dDevice->SetRenderState(D3DRS_SRCBLEND, enumRTGBlendFunc[pShader->eBlendSrcFunc]);
        }
        if (m_eBlendDestFunc != pShader->eBlendDstFunc) {
            m_eBlendDestFunc = pShader->eBlendDstFunc;
            m_pd3dDevice->SetRenderState(D3DRS_DESTBLEND, enumRTGBlendFunc[pShader->eBlendDstFunc]);
        }
        if (m_dwBlendFactor != pShader->dwBlendFactor) {
            m_dwBlendFactor = pShader->dwBlendFactor;
            m_pd3dDevice->SetRenderState(D3DRS_BLENDFACTOR, pShader->dwBlendFactor);
        }
    } else {
        if (m_bBlend != FALSE) {
            m_bBlend = FALSE;
            m_pd3dDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
        }
    }

    if (pShader->dwTextureFactor != m_dwTextureFactor) {
        m_dwTextureFactor = pShader->dwTextureFactor;
        m_pd3dDevice->SetRenderState(D3DRS_TEXTUREFACTOR, m_dwTextureFactor);
    }

    int i, iPasses;
#ifdef _DEBUG
    m_iTextureRequiredPasses =
        (pShader->iNumStage + m_Ability.iMaxMultiTextures - 1) / m_Ability.iMaxMultiTextures;
    CHECK(m_iTextureRequiredPasses <= 1);
#endif

    iPasses = (m_Ability.iMaxMultiTextures > pShader->iNumStage) ? pShader->iNumStage
                                                                 : m_Ability.iMaxMultiTextures;
    for (i = 0; i < iPasses; i++) {
        UseTextureStage(i, pShader->Shaders + i);
    }
    m_iTextureCurPass = i;
    for (; i < m_Ability.iMaxMultiTextures; i++) {
        m_pd3dDevice->SetTextureStageState(i, D3DTSS_COLOROP, D3DTOP_DISABLE);
    }
}

void RtgDeviceD3D9::ResetTextMat() {
    for (int i = 0; i < m_Ability.iMaxMultiTextures; i++)
        SetTextureMatrix(i, &m_mTextureDefaultMatrix);
}

void RtgDeviceD3D9::UseTextureStage(int iLevel, RtgTexStage* pStage) {
    if (pStage->bUseTextureMatrix) {
        if (m_bUseTextureMatrix[iLevel] != TRUE ||
            m_dwTexTransformFlag[iLevel] != pStage->dwTexTransformFlag) {
            m_bUseTextureMatrix[iLevel] = TRUE;
            m_dwTexTransformFlag[iLevel] = pStage->dwTexTransformFlag;

            //m_pd3dDevice->SetTextureStageState(iLevel, D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_COUNT2);
        }
        m_pd3dDevice->SetTextureStageState(iLevel, D3DTSS_TEXTURETRANSFORMFLAGS,
                                           pStage->dwTexTransformFlag);
        //pStage->mMatrix.SaveTo4x4(m16);
        SetTextureMatrix(iLevel, &pStage->mMatrix16);
    } else {
        if (m_bUseTextureMatrix[iLevel]) {
            m_bUseTextureMatrix[iLevel] = FALSE;
            m_pd3dDevice->SetTextureStageState(iLevel, D3DTSS_TEXTURETRANSFORMFLAGS,
                                               D3DTTFF_DISABLE);
            SetTextureMatrix(iLevel, &m_mTextureDefaultMatrix);
        }
    }

    UseTexture(iLevel, pStage->GetTexture());

    /*m_Texture.UseTexture(iLevel, pStage->GetTexture());*/
    if (pStage->eColorOp == RTGTOP_DISABLE) {
        m_pd3dDevice->SetTextureStageState(iLevel, D3DTSS_COLOROP, D3DTOP_DISABLE);
    } else {
        m_pd3dDevice->SetTextureStageState(iLevel, D3DTSS_COLOROP,
                                           enumRTGTextureOp[pStage->eColorOp]);
        m_pd3dDevice->SetTextureStageState(iLevel, D3DTSS_COLORARG1,
                                           enumRTGTexArg[pStage->eColorArg1]);
        m_pd3dDevice->SetTextureStageState(iLevel, D3DTSS_COLORARG2,
                                           enumRTGTexArg[pStage->eColorArg2]);
        m_pd3dDevice->SetTextureStageState(iLevel, D3DTSS_COLORARG0,
                                           enumRTGTexArg[pStage->eColorArg0]);
        if (pStage->eAlphaOp == RTGTOP_DISABLE) {
            m_pd3dDevice->SetTextureStageState(iLevel, D3DTSS_ALPHAOP, D3DTOP_DISABLE);
        } else {
            m_pd3dDevice->SetTextureStageState(iLevel, D3DTSS_ALPHAOP,
                                               enumRTGTextureOp[pStage->eAlphaOp]);
            m_pd3dDevice->SetTextureStageState(iLevel, D3DTSS_ALPHAARG1,
                                               enumRTGTexArg[pStage->eAlphaArg1]);
            m_pd3dDevice->SetTextureStageState(iLevel, D3DTSS_ALPHAARG2,
                                               enumRTGTexArg[pStage->eAlphaArg2]);
            m_pd3dDevice->SetTextureStageState(iLevel, D3DTSS_ALPHAARG0,
                                               enumRTGTexArg[pStage->eAlphaArg0]);
        }

        m_pd3dDevice->SetTextureStageState(iLevel, D3DTSS_RESULTARG,
                                           enumRTGTexArg[pStage->eResultArg]);

        if (pStage->eTexCoordIndex != RTGTC_PASSTHRU) {
            m_pd3dDevice->SetTextureStageState(iLevel, D3DTSS_TEXCOORDINDEX,
                                               enumRTGTexCoor[pStage->eTexCoordIndex]);
        } else {
            m_pd3dDevice->SetTextureStageState(iLevel, D3DTSS_TEXCOORDINDEX, iLevel);
        }
    }
}

void RtgDeviceD3D9::SetTextureCoord(int texId, int coordId) {
    m_pd3dDevice->SetTextureStageState(texId, D3DTSS_TEXCOORDINDEX, coordId);
}

void RtgDeviceD3D9::SetPostProcessEffectOnoff(bool bOn) {
    RtgDevice::SetPostProcessEffectOnoff(bOn);

    if (IsUsePostProcessEffectOn()) {
        if (!CreatePostProcessResource() || !GetEffect(Rtg_Effect_Bloom))
            SetPostProcessEffectOnoff(false);
    } else {
        DestoryPostProcessResource(false);
    }
}

void RtgDeviceD3D9::Gray(bool bOn) {
    RtgDevice::Gray(bOn);

    if (IsGray()) {
        if (!CreateGrayResource() || !GetEffect(Rtg_Effect_Bloom))
            Gray(false);
    } else {
        DestoryGrayResource(false);
    }

    m_dwGrayBegin = rtMilliseconds();
}

bool RtgDeviceD3D9::CreatePostProcessResource() {
    if (!m_texPostProcess)
        m_texPostProcess = GetTextMgr()->CreateTexture(
            m_iWndWidth, m_iWndHeight, 1, D3DFMT_A8R8G8B8, D3DPOOL_DEFAULT, D3DUSAGE_RENDERTARGET);

    const int down_scal = 4;

    m_bloomTexW = m_iWndWidth / down_scal;
    m_bloomTexH = m_iWndHeight / down_scal;

    float sh = (float)down_scal / (float)m_iWndWidth;
    float sv = (float)down_scal / (float)m_iWndHeight;
    int   c = Bloom_Kernel_Size / 2;

    for (int i = 0; i < Bloom_Kernel_Size; ++i) {
        m_bloomTexOffset_h[i] = (i - c) * sh;
        m_bloomTexOffset_v[i] = (i - c) * sv;
    }

    if (!m_texPostDown0)
        m_texPostDown0 = GetTextMgr()->CreateTexture(m_bloomTexW, m_bloomTexH, 1, D3DFMT_A8R8G8B8,
                                                     D3DPOOL_DEFAULT, D3DUSAGE_RENDERTARGET);

    if (!m_texPostDown1)
        m_texPostDown1 = GetTextMgr()->CreateTexture(m_bloomTexW, m_bloomTexH, 1, D3DFMT_A8R8G8B8,
                                                     D3DPOOL_DEFAULT, D3DUSAGE_RENDERTARGET);

    if (!m_texPostProcess || !m_texPostDown0 || !m_texPostDown1) {
        DestoryPostProcessResource(m_texPostProcess ? false : true);
        return false;
    }

    return true;
}

void RtgDeviceD3D9::DestoryPostProcessResource(bool bForce) {
    if (m_texPostProcess) {
        if (bForce || !IsGray()) {
            GetTextMgr()->ReleaseTexture(m_texPostProcess);
            m_texPostProcess = NULL;
        }
    }

    if (m_texPostDown0) {
        GetTextMgr()->ReleaseTexture(m_texPostDown0);
        m_texPostDown0 = NULL;
    }

    if (m_texPostDown1) {
        GetTextMgr()->ReleaseTexture(m_texPostDown1);
        m_texPostDown1 = NULL;
    }
}

bool RtgDeviceD3D9::CreateGrayResource() {
    if (!m_texPostProcess)
        m_texPostProcess = GetTextMgr()->CreateTexture(
            m_iWndWidth, m_iWndHeight, 1, D3DFMT_A8R8G8B8, D3DPOOL_DEFAULT, D3DUSAGE_RENDERTARGET);

    if (!m_texPostProcess)
        return false;

    return true;
}

void RtgDeviceD3D9::DestoryGrayResource(bool bForce) {
    if (m_texPostProcess) {
        if (bForce || !IsUsePostProcessEffectOn()) {
            GetTextMgr()->ReleaseTexture(m_texPostProcess);
            m_texPostProcess = NULL;
        }
    }
}

void RtgDeviceD3D9::RestoreShader(RtgShader* pShader) {
    LOG_FUNCTION;
}

void RtgDeviceD3D9::DeleteShader(RtgShader* pShader) {
    LOG_FUNCTION;
    for (int i = 0; i < pShader->iNumStage; i++) {
        GetTextMgr()->ReleaseTexture(pShader->Shaders[i].GetTexture());
        //m_Texture.DeleteTexture(pShader->Shaders[i].GetTexture());
        pShader->Shaders[i].SetTexture(0);
    }
}

BOOL RtgDeviceD3D9::CreateTextureByFile(_TexItem* pItem, const char* szTexFileName, int iMipMap) {
    LOG_FUNCTION;
    RtgImage img;
    if (!img.Load(szTexFileName)) {
        RtCoreLog().Warn("RtgDeviceD3D9: Can't open Texture [%s]", szTexFileName);
        char szFileName[MAX_PATH];
        GetCurrentDirectory(MAX_PATH, szFileName);
        RtCoreLog().Warn(" [%s].\n", szFileName);
        return FALSE;
    }

    rt2_strncpy(pItem->szFileName, szTexFileName, MAX_TEX_FILE_NAME - 1);
#ifdef _DEBUG
    if (strlen(szTexFileName) >= MAX_TEX_FILE_NAME) {
        RtCoreLog().Warn("RtgDeviceD3D9: [WARNING] 贴图名字过长(>=%d)，可能会丢失一些信息 (%s)\n",
                         MAX_TEX_FILE_NAME, szTexFileName);
    }
#endif

    img.FixTextureQuality(pItem->bQualityManage);

    // Create Texture
    LPDIRECT3DTEXTURE9 pTex;
    int                iWidth, iHeight;
    pItem->iWidth = iWidth = img.info.iWidth;
    pItem->iHeight = iHeight = img.info.iHeight;
    if (iMipMap == 0)
        iMipMap = img.GetMipMapCount();
    pItem->iMipMapCount = iMipMap;
    if (m_pd3dDevice->CreateTexture(iWidth, iHeight, iMipMap, 0,
                                    enumRTGTextureFormat[m_pTexColorFormat->m_eFormat],
                                    D3DPOOL_MANAGED, &pTex, NULL) != D3D_OK) {
        RtCoreLog().Warn("RtgDeviceD3D9错误: CreateTextureByFile调用CreateTexture失败.\n");
        return FALSE;
    }

    D3DLOCKED_RECT desc;
    for (int i = 0; i < iMipMap; i++) {
        img.Resize(iWidth, iHeight);
        pTex->LockRect(i, &desc, NULL, D3DLOCK_NOSYSLOCK);
        m_pTexColorFormat->fill(iWidth, iHeight, desc.pBits, desc.Pitch, img.GetRawData(),
                                iWidth * 4);
        pTex->UnlockRect(i);
        if (iWidth > 1)
            iWidth /= 2;
        if (iHeight > 1)
            iHeight /= 2;
    }

    pItem->pTexture = pTex;
    return TRUE;
}

BOOL RtgDeviceD3D9::CreateTextureByImage(_TexItem* pItem, RtgImage& img) {
    LOG_FUNCTION;
    LPDIRECT3DTEXTURE9 pTex;
    D3DFORMAT          dwD3dFormat;
    int                iWidth;
    int                iHeight;
    D3DLOCKED_RECT     desc;

    if (img.info.bDDS) {
        switch (img.info.iDxtcFormat) {
            case RtgImage::EPF_DXT1:
                dwD3dFormat = D3DFMT_DXT1;
                break;
            case RtgImage::EPF_DXT2:
                dwD3dFormat = D3DFMT_DXT2;
                break;
            case RtgImage::EPF_DXT3:
                dwD3dFormat = D3DFMT_DXT3;
                break;
            case RtgImage::EPF_DXT4:
                dwD3dFormat = D3DFMT_DXT4;
                break;
            case RtgImage::EPF_L8:
                dwD3dFormat = D3DFMT_L8;
                break;
            case RtgImage::EPF_ARGB:
                dwD3dFormat = D3DFMT_A8R8G8B8;
                break;
            default:
            case RtgImage::EPF_DXT5:
                dwD3dFormat = D3DFMT_DXT5;
                break;
        }
        if (m_pd3dDevice->CreateTexture(pItem->iWidth, pItem->iHeight, pItem->iMipMapCount, 0,
                                        dwD3dFormat, D3DPOOL_MANAGED, &pTex, NULL) != D3D_OK) {
            RtCoreLog().Warn("RtgDeviceD3D8错误: CreateTextureByImage调用CreateTexture失败.\n");
            return FALSE;
        }

        CHECK(pTex->GetLevelCount() == pItem->iMipMapCount);
        pItem->pTexture = pTex;

        iWidth = pItem->iWidth;
        iHeight = pItem->iHeight;
        for (int i = 0; i < pItem->iMipMapCount; i++) {
            img.Resize(iWidth, iHeight);
            pTex->LockRect(i, &desc, NULL, D3DLOCK_NOSYSLOCK);
            memcpy(desc.pBits, img.GetRawData(), img.GetDataSize());
            pTex->UnlockRect(i);
            if (iWidth > 1)
                iWidth /= 2;
            if (iHeight > 1)
                iHeight /= 2;
        }
    } else {
        switch (pItem->cFormat) {
            case 1:
                dwD3dFormat = D3DFMT_A8R8G8B8;
                break;
            case 2:
                dwD3dFormat = D3DFMT_A4R4G4B4;
                break;
            case 3:
                dwD3dFormat = D3DFMT_A1R5G5B5;
                break;
            case 8:
                dwD3dFormat = D3DFMT_L8;
                break;
            default:
                dwD3dFormat = enumRTGTextureFormat[m_pTexColorFormat->m_eFormat];
                break;
        }
        if (m_pd3dDevice->CreateTexture(pItem->iWidth, pItem->iHeight, pItem->iMipMapCount, 0,
                                        dwD3dFormat, D3DPOOL_MANAGED, &pTex, NULL) != D3D_OK) {
            RtCoreLog().Warn("RtgDeviceD3D9错误: CreateTextureByImage[%s]调用CreateTexture失败.\n",
                             pItem->szFileName);
            return FALSE;
        }
        CHECK(pTex->GetLevelCount() == pItem->iMipMapCount);
        pItem->pTexture = pTex;

        int            iWidth = pItem->iWidth;
        int            iHeight = pItem->iHeight;
        D3DLOCKED_RECT desc;
        for (int i = 0; i < pItem->iMipMapCount; i++) {
            img.Resize(iWidth, iHeight);
            pTex->LockRect(i, &desc, NULL, D3DLOCK_NOSYSLOCK);
            m_pTexColorFormat->fill(iWidth, iHeight, desc.pBits, desc.Pitch, img.GetRawData(),
                                    iWidth * 4);
            pTex->UnlockRect(i);
            if (iWidth > 1)
                iWidth /= 2;
            if (iHeight > 1)
                iHeight /= 2;
        }
    }
    return TRUE;
}

BOOL RtgDeviceD3D9::CreateTextureByMemoryCpy(_TexItem* pItem) {
    CHECK(0);
    return TRUE;
}

BOOL RtgDeviceD3D9::CreateTexture(_TexItem* pItem, BOOL bDxManaged) {
    LOG_FUNCTION;
    LPDIRECT3DTEXTURE9 pTex;
    D3DFORMAT          dwD3dFormat;
    switch (pItem->cFormat) {
        case 1:
            dwD3dFormat = D3DFMT_A8R8G8B8;
            break;
        case 2:
            dwD3dFormat = D3DFMT_A4R4G4B4;
            break;
        case 3:
            dwD3dFormat = D3DFMT_A1R5G5B5;
            break;
        case 8:
            dwD3dFormat = D3DFMT_L8;
            break;
        default:
            dwD3dFormat = enumRTGTextureFormat[m_pTexColorFormat->m_eFormat];
            break;
    }
    if (m_pd3dDevice->CreateTexture(pItem->iWidth, pItem->iHeight, pItem->iMipMapCount, 0,
                                    dwD3dFormat, bDxManaged ? D3DPOOL_MANAGED : D3DPOOL_DEFAULT,
                                    &pTex, NULL) != D3D_OK) {
        RtCoreLog().Warn("RtgDeviceD3D9错误: CreateTexture调用失败.\n");
        return FALSE;
    }
    CHECK(pTex->GetLevelCount() == pItem->iMipMapCount);
    pItem->pTexture = pTex;
    return TRUE;
}

BOOL RtgDeviceD3D9::CreateTextureDynamic(_TexItem* pItem) {
    if (!m_Ability.bDynamicTextures) {
        return CreateTexture(pItem, TRUE);
    }
    LOG_FUNCTION;
    LPDIRECT3DTEXTURE9 pTex;
    D3DFORMAT          dwD3dFormat;
    switch (pItem->cFormat) {
        case 1:
            dwD3dFormat = D3DFMT_A8R8G8B8;
            break;
        case 2:
            dwD3dFormat = D3DFMT_A4R4G4B4;
            break;
        case 3:
            dwD3dFormat = D3DFMT_A1R5G5B5;
            break;
        case 8:
            dwD3dFormat = D3DFMT_L8;
            break;
        default:
            dwD3dFormat = enumRTGTextureFormat[m_pTexColorFormat->m_eFormat];
            break;
    }
    CHECK(pItem->iMipMapCount == 1);  // 动态贴图只能为1
    if (m_pd3dDevice->CreateTexture(pItem->iWidth, pItem->iHeight, 1, D3DUSAGE_DYNAMIC, dwD3dFormat,
                                    D3DPOOL_DEFAULT, &pTex, NULL) != D3D_OK) {
        RtCoreLog().Warn("RtgDeviceD3D9错误: 创建动态贴图出错, 贴图格式为：%d, %d, %d\n",
                         pItem->iWidth, pItem->iHeight, dwD3dFormat);
        return FALSE;
    }
    CHECK(pTex->GetLevelCount() == 1);
    pItem->pTexture = pTex;
    return TRUE;
}

BOOL RtgDeviceD3D9::CreateTextureRenderTarget(_TexItem* pItem) {
    LOG_FUNCTION;
    LPDIRECT3DTEXTURE9 pTex;
    CHECK(pItem->iMipMapCount == 1);  // RenderTarget只能为1

    D3DFORMAT kRenderTargetFormat = D3DFMT_A8R8G8B8;

    ///!用两百来表示使用 D3DFMT_X8R8G8B8
    switch (pItem->cFormat) {
        case 4: {
            kRenderTargetFormat = D3DFMT_X8R8G8B8;
        } break;
        case 8: {
            kRenderTargetFormat = D3DFMT_L8;
        } break;
        default: {
            kRenderTargetFormat = D3DFMT_A8R8G8B8;
        } break;
    }

    if (m_pd3dDevice->CreateTexture(pItem->iWidth, pItem->iHeight, 1, D3DUSAGE_RENDERTARGET,
                                    kRenderTargetFormat, D3DPOOL_DEFAULT, &pTex, NULL) != D3D_OK) {
        RtCoreLog().Warn("RtgDeviceD3D9错误: CreateTextureRenderTarget调用CreateTexture失败.\n");
        return FALSE;
    }
    if (pItem->bUseOffScreen) {
        if (m_pd3dDevice->CreateOffscreenPlainSurface(
                pItem->iWidth, pItem->iHeight, kRenderTargetFormat, D3DPOOL_SYSTEMMEM,
                (LPDIRECT3DSURFACE9*)&pItem->pOffScreen, NULL) != D3D_OK) {
            RtCoreLog().Warn("RtgDeviceD3D9错误: "
                             "CreateTextureRenderTarget调用CreateOffscreenPlainSurface失败.\n");
            return FALSE;
        }
    }
    CHECK(pTex->GetLevelCount() == 1);
    pItem->pTexture = pTex;
    return TRUE;
}

BOOL RtgDeviceD3D9::CreateTextureDepthStencil(_TexItem* pItem) {
    LOG_FUNCTION;
    LPDIRECT3DTEXTURE9 pTex;
    CHECK(pItem->iMipMapCount == 1);  // RenderTarget只能为1
    if (m_pd3dDevice->CreateTexture(pItem->iWidth, pItem->iHeight, 1, D3DUSAGE_DEPTHSTENCIL,
                                    enumRTGTextureFormat[pItem->cFormat], D3DPOOL_DEFAULT, &pTex,
                                    NULL) != D3D_OK) {
        RtCoreLog().Warn("RtgDeviceD3D9错误: CreateTextureDepthStencil调用CreateTexture失败.\n");
        return FALSE;
    }
    CHECK(pTex->GetLevelCount() == 1);
    pItem->pTexture = pTex;
    return TRUE;
}

BOOL RtgDeviceD3D9::GetBackBufferRenderTarget(_TexItem* pRenderTarget, _TexItem* pZStencil) {
    HRESULT            hResult;
    IDirect3DSurface9 *src, *dest;

    if (m_pd3dDevice->GetRenderTarget(0, &src) != D3D_OK) {
        RtCoreLog().Error("获取backbuffer失败");
        return false;
    }

    if (pRenderTarget) {
        hResult = ((LPDIRECT3DTEXTURE9)(pRenderTarget->pTexture))->GetSurfaceLevel(0, &dest);
        if (hResult != D3D_OK) {
            RtCoreLog().Error("获取target失败");
            return false;
        }
    }

    if (m_pd3dDevice->StretchRect(src, NULL, dest, NULL, D3DTEXF_NONE) != D3D_OK) {
        RtCoreLog().Error("出错拉");
    }
    src->Release();
    dest->Release();

    return true;
}

BOOL RtgDeviceD3D9::SetRenderTarget(RtgTextItem* frameTarget, RtgTextItem* depthTarget) {
    if (m_pRTManager->Push_RenderTarget(frameTarget, depthTarget))
        return TRUE;
    return FALSE;
}

BOOL RtgDeviceD3D9::RestoreRenderTarget() {
    if (m_pRTManager->Pop_RenderTarget())
        return TRUE;
    return FALSE;
}

BOOL RtgDeviceD3D9::DeleteTexture(_TexItem* pItem) {
    if (pItem) {
        if (pItem->pTexture) {
            LPDIRECT3DTEXTURE9 pTexture = (LPDIRECT3DTEXTURE9)pItem->pTexture;
            SAFE_RELEASE(pTexture);
            pItem->pTexture = NULL;
        }
        if (pItem->pOffScreen) {
            LPDIRECT3DSURFACE9 pOffScreen = (LPDIRECT3DSURFACE9)pItem->pOffScreen;
            SAFE_RELEASE(pOffScreen);
            pItem->pOffScreen = NULL;
        }
        return TRUE;
    }
    RtCoreLog().Warn("RtgDeviceD3D9错误: DeleteTexture调用失败.\n");
    return FALSE;
}

_TexItem*          pLastTextureLockItem = NULL;
LPDIRECT3DSURFACE9 pTextureLockSurface = NULL;

BOOL RtgDeviceD3D9::TextureLock(_TexItem* pItem, RtgTexture::TexLock& texLock) {
    D3DLOCKED_RECT     desc;
    LPDIRECT3DTEXTURE9 pTex = (LPDIRECT3DTEXTURE9)pItem->pTexture;
    if (pItem->iPathIdx == -2) {
        if (!pItem->bUseOffScreen)
            return FALSE;
        LPDIRECT3DSURFACE9 pOffScreen = (LPDIRECT3DSURFACE9)pItem->pOffScreen;
        pLastTextureLockItem = pItem;
        pTex->GetSurfaceLevel(0, &pTextureLockSurface);
        m_pd3dDevice->GetRenderTargetData(pTextureLockSurface, pOffScreen);
        pOffScreen->LockRect(&desc, NULL, NULL);
    } else {
        if (pTex->LockRect(0, &desc, NULL, D3DLOCK_NOSYSLOCK) != D3D_OK) {
            RtCoreLog().Warn("RtgDeviceD3D9错误: TextureLock调用LockRect失败.\n");
            return FALSE;
        }
    }
    texLock.dwTexID = pItem->dwIdx;
    texLock.iWidth = pItem->iWidth;
    texLock.iHeight = pItem->iHeight;
    texLock.iPitch = desc.Pitch;
    texLock.pBits = desc.pBits;
    return TRUE;
}

void RtgDeviceD3D9::TextureUnlock(_TexItem* pItem, RtgTexture::TexLock& texLock) {
    LPDIRECT3DTEXTURE9 pTex = (LPDIRECT3DTEXTURE9)pItem->pTexture;
    if (pItem->iPathIdx == -2) {
        if (!pItem->bUseOffScreen)
            return;
        LPDIRECT3DSURFACE9 pOffScreen = (LPDIRECT3DSURFACE9)pItem->pOffScreen;
        pOffScreen->UnlockRect();
        CHECK(pLastTextureLockItem == pItem);
        m_pd3dDevice->UpdateSurface(pOffScreen, NULL, pTextureLockSurface, NULL);
        pTextureLockSurface->Release();
    } else {
        pTex->UnlockRect(0);
    }
}

BOOL RtgDeviceD3D9::UpdateTexture(_TexItem* pItem, void* pBuffer) {
    LOG_FUNCTION;
    D3DLOCKED_RECT     desc;
    LPDIRECT3DTEXTURE9 pTex = (LPDIRECT3DTEXTURE9)pItem->pTexture;
    if (pTex->LockRect(0, &desc, NULL, D3DLOCK_NOSYSLOCK) != D3D_OK) {
        RtCoreLog().Warn("RtgDeviceD3D9错误: UpdateTexture调用LockRect失败.\n");
        return FALSE;
    }
    m_pTexColorFormat->fill(pItem->iWidth, pItem->iHeight, desc.pBits, desc.Pitch, pBuffer,
                            pItem->iWidth * 4);
    pTex->UnlockRect(0);
    return TRUE;
}

BOOL RtgDeviceD3D9::UpdateTextureDynamic(_TexItem* pItem, void* pBuffer) {
    if (!m_Ability.bDynamicTextures) {
        return UpdateTexture(pItem, pBuffer);
    }
    LOG_FUNCTION;
    D3DLOCKED_RECT     desc;
    LPDIRECT3DTEXTURE9 pTex = (LPDIRECT3DTEXTURE9)pItem->pTexture;
    if (pTex->LockRect(0, &desc, NULL, D3DLOCK_DISCARD) != D3D_OK) {
        RtCoreLog().Warn("RtgDeviceD3D9错误: UpdateTextureDynamic调用LockRect失败.\n");
        return FALSE;
    }
    m_pTexColorFormat->fill(pItem->iWidth, pItem->iHeight, desc.pBits, desc.Pitch, pBuffer,
                            pItem->iWidth * 4);
    pTex->UnlockRect(0);
    return TRUE;
}

BOOL RtgDeviceD3D9::UseTexture(int stage, RtgTextItem* texItem) {
    if (FAILED(m_pd3dDevice->SetTexture(stage, GetTextMgr()->GetD3dTexture(texItem))))
        return FALSE;
    return TRUE;
}

BOOL RtgDeviceD3D9::ClearRenderTarget(bool bStencil, bool bTarget, bool bZBuffer, DWORD dwColor,
                                      float fNewZ, DWORD dwStencil) {
    HRESULT hr;
    DWORD   dwFlag = 0;
    if (bStencil)
        dwFlag |= D3DCLEAR_STENCIL;
    if (bTarget)
        dwFlag |= D3DCLEAR_TARGET;
    if (bZBuffer)
        dwFlag |= D3DCLEAR_ZBUFFER;
    if (FAILED(hr = m_pd3dDevice->Clear(0L, NULL, dwFlag, dwColor, fNewZ, dwStencil))) {
        return DisplayErrorMsg(hr, MSGERR_APPMUSTEXIT);
    }
    return TRUE;
}

void RtgDeviceD3D9::LockFps(DWORD dwMillisecondPerFrame) {
    LockFpsPerFrame(dwMillisecondPerFrame);
}

HRESULT RtgDeviceD3D9::FrameMove() {
    RtGetPref()->Time_Update = rtMilliseconds();
    m_pBufferManager->FrameMove();
    m_pTextMamager->FrameMove();
    RtcGetActorManager()->FrameMove();
    this->OnFrameMove(m_fElapsedTime);
    RtGetPref()->Time_Update = rtMilliseconds() - RtGetPref()->Time_Update;
    return S_OK;
}

void RtgDeviceD3D9::UpdateAppTime() {
    UpdateStats();
}

BOOL RtgDeviceD3D9::BeginScene() {
    return SUCCEEDED(m_pd3dDevice->BeginScene());
}

void RtgDeviceD3D9::EndScene() {
    m_pd3dDevice->EndScene();
}

HRESULT RtgDeviceD3D9::Render() {
    HRESULT hResult = E_FAIL;  // 初始化返回结果为失败

    RtGetPref()->Time_Render = rtMilliseconds();  // 获取当前渲染时间
    m_pBufferManager->PrepareFotRender();         // 准备渲染缓冲区

    if (FAILED(m_pd3dDevice->BeginScene()))  // 开始场景渲染
        return E_FAIL;

    ID3DXEffect* _effect = GetEffect(Rtg_Effect_Bloom);  // 获取效果
    RTASSERT(!IsUsePostProcessEffectOn() || _effect);    // 断言检查效果是否可用

    if (IsUsePostProcessEffectOn() || IsGray())  // 使用后处理效果或灰度
        SetRenderTarget(m_texPostProcess, 0);    // 设置渲染目标

    // 清除渲染目标
    if (FAILED(hResult = m_pd3dDevice->Clear(0L, NULL, D3DCLEAR_ZBUFFER | D3DCLEAR_TARGET,
                                             m_dwClearColor, 1.0f, 0L)))
        return DisplayErrorMsg(hResult, MSGERR_APPMUSTEXIT);

    SetTextureFilterMag(RTGTEXF_LINEAR);  // 设置纹理放大过滤
    SetTextureFilterMin(RTGTEXF_LINEAR);  // 设置纹理缩小过滤

    OnBeginRender();  // 开始渲染
    OnRender();       // 渲染
    OnEndRender();    // 结束渲染

    if (IsUsePostProcessEffectOn()) {                       // 如果使用后处理效果
        RtgShader         shader;                           // 着色器
        RECT              _rect;                            // 矩形
        RtgNVertexVT      _vrect[4];                        // 顶点
        BOOL              bfog = GetFogEnable();            // 获取雾效状态
        UINT              _pass = 0;                        // 渲染通道
        RTGTextureAddress _taddress = GetTextureAddress();  // 纹理地址

        _rect.left = 0;
        _rect.top = 0;
        _rect.bottom = m_iWndHeight - 1;
        _rect.right = m_iWndWidth - 1;

        SetTextureFilterMag(RTGTEXF_POINT);   // 设置纹理放大过滤
        SetTextureFilterMin(RTGTEXF_POINT);   // 设置纹理缩小过滤
        SetFogEnable(FALSE);                  // 关闭雾效
        SetFVF(D3DFVF_XYZRHW | D3DFVF_TEX1);  // 设置顶点格式
        SetRenderTarget(m_texPostDown0, 0);   // 设置渲染目标

        shader.iNumStage = 1;
        shader.Shaders[0].SetTexture(m_texPostProcess);  // 设置纹理
        SetShader(&shader);                              // 设置着色器

        _vrect[0] = RtgNVertexVT(0, 0, 0, 0);
        _vrect[1] = RtgNVertexVT(0, m_bloomTexH, 0, 1);
        _vrect[2] = RtgNVertexVT(m_bloomTexW, m_bloomTexH, 1, 1);
        _vrect[3] = RtgNVertexVT(m_bloomTexW, 0, 1, 0);

        DrawPrimitiveUP(D3DPT_TRIANGLEFAN, &_vrect, sizeof(RtgNVertexVT), 2);  // 绘制原始几何图形

        SetTextureAddress(RTGTADD_CLAMP);  // 设置纹理地址模式
        _effect->SetFloat("g_fIntensity", m_fPostProcessEffectIntensity);  // 设置效果强度
        _effect->SetFloatArray("TexelOffset_h", m_bloomTexOffset_h,
                               Bloom_Kernel_Size);  // 设置水平偏移
        _effect->SetFloatArray("TexelOffset_v", m_bloomTexOffset_v,
                               Bloom_Kernel_Size);  // 设置垂直偏移

        SetRenderTarget(m_texPostDown1, 0);  // 设置渲染目标
        shader.iNumStage = 1;
        shader.Shaders[0].SetTexture(m_texPostDown0);  // 设置纹理
        SetShader(&shader);                            // 设置着色器

        if (SUCCEEDED(_effect->SetTechnique("tec_gauss_h")) &&
            SUCCEEDED(_effect->Begin(&_pass, 0))) {  // 设置水平高斯模糊技术
            if (SUCCEEDED(_effect->BeginPass(0))) {  // 开始通道
                DrawPrimitiveUP(D3DPT_TRIANGLEFAN, &_vrect, sizeof(RtgNVertexVT),
                                2);  // 绘制原始几何图形
                _effect->EndPass();  // 结束通道
            }
            _effect->End();  // 结束效果
        }

        RestoreRenderTarget();  // 恢复渲染目标
        shader.iNumStage = 1;
        shader.Shaders[0].SetTexture(m_texPostDown1);  // 设置纹理
        SetShader(&shader);                            // 设置着色器

        if (SUCCEEDED(_effect->SetTechnique("tec_gauss_v")) &&
            SUCCEEDED(_effect->Begin(&_pass, 0))) {  // 设置垂直高斯模糊技术
            if (SUCCEEDED(_effect->BeginPass(0))) {  // 开始通道
                DrawPrimitiveUP(D3DPT_TRIANGLEFAN, &_vrect, sizeof(RtgNVertexVT),
                                2);  // 绘制原始几何图形
                _effect->EndPass();  // 结束通道
            }
            _effect->End();  // 结束效果
        }

        RestoreRenderTarget();  // 恢复渲染目标
        RestoreRenderTarget();  // 再次恢复渲染目标

        shader.iNumStage = 2;
        shader.Shaders[0].SetTexture(m_texPostProcess);  // 设置纹理
        shader.Shaders[1].SetTexture(m_texPostDown0);    // 设置纹理
        SetShader(&shader);                              // 设置着色器

        SetTextureFilterMag(RTGTEXF_LINEAR);  // 设置纹理放大过滤
        SetTextureFilterMin(RTGTEXF_LINEAR);  // 设置纹理缩小过滤

        _vrect[0] = RtgNVertexVT(_rect.left, _rect.top, 0, 0);
        _vrect[1] = RtgNVertexVT(_rect.left, _rect.bottom, 0, 1);
        _vrect[2] = RtgNVertexVT(_rect.right, _rect.bottom, 1, 1);
        _vrect[3] = RtgNVertexVT(_rect.right, _rect.top, 1, 0);

        float _coeff = min(((rtMilliseconds() - m_dwGrayBegin) / 1000.f), 1.f);  // 计算灰度系数
        _effect->SetFloat("g_fGrayCoeff", _coeff);  // 设置灰度系数

        const char* tec = IsGray() ? "tec_gray" : "tec";  // 选择技术
        if (SUCCEEDED(_effect->SetTechnique(tec)) &&
            SUCCEEDED(_effect->Begin(&_pass, 0))) {  // 设置技术
            if (SUCCEEDED(_effect->BeginPass(0))) {  // 开始通道
                DrawPrimitiveUP(D3DPT_TRIANGLEFAN, &_vrect, sizeof(RtgNVertexVT),
                                2);  // 绘制原始几何图形
                _effect->EndPass();  // 结束通道
            }
            _effect->End();  // 结束效果
        }

        SetFogEnable(bfog);            // 恢复雾效
        SetTextureAddress(_taddress);  // 恢复纹理地址模式
    } else if (IsGray()) {             // 如果使用灰度效果
        RtgShader    shader;
        RECT         _rect;
        RtgNVertexVT _vrect[4];
        BOOL         bfog = GetFogEnable();
        UINT         _pass = 0;

        _rect.left = 0;
        _rect.top = 0;
        _rect.bottom = m_iWndHeight - 1;
        _rect.right = m_iWndWidth - 1;

        SetFogEnable(FALSE);                  // 关闭雾效
        SetFVF(D3DFVF_XYZRHW | D3DFVF_TEX1);  // 设置顶点格式

        shader.iNumStage = 1;
        shader.Shaders[0].SetTexture(m_texPostProcess);  // 设置纹理
        SetShader(&shader);                              // 设置着色器

        _vrect[0] = RtgNVertexVT(_rect.left, _rect.top, 0, 0);
        _vrect[1] = RtgNVertexVT(_rect.left, _rect.bottom, 0, 1);
        _vrect[2] = RtgNVertexVT(_rect.right, _rect.bottom, 1, 1);
        _vrect[3] = RtgNVertexVT(_rect.right, _rect.top, 1, 0);

        RestoreRenderTarget();  // 恢复渲染目标

        float _coeff = min(((rtMilliseconds() - m_dwGrayBegin) / 1000.f), 1.f);  // 计算灰度系数
        _effect->SetFloat("g_fGrayCoeff", _coeff);  // 设置灰度系数

        if (SUCCEEDED(_effect->SetTechnique("tec_gray2")) &&
            SUCCEEDED(_effect->Begin(&_pass, 0))) {  // 设置灰度效果技术
            if (SUCCEEDED(_effect->BeginPass(0))) {  // 开始通道
                DrawPrimitiveUP(D3DPT_TRIANGLEFAN, &_vrect, sizeof(RtgNVertexVT),
                                2);  // 绘制原始几何图形
                _effect->EndPass();  // 结束通道
            }
            _effect->End();  // 结束效果
        }

        SetFogEnable(bfog);  // 恢复雾效
    }

    SetTextureFilterMag(RTGTEXF_POINT);  // 设置纹理放大过滤
    SetTextureFilterMin(RTGTEXF_POINT);  // 设置纹理缩小过滤
    OnRender2D();                        // 渲染 2D 内容

    RtGetPref()->Time_Render = rtMilliseconds() - RtGetPref()->Time_Render;  // 计算渲染时间
    if (m_bDrawPerf)
        DrawPerfInformation();  // 绘制性能信息

    RtGetPref()->Reset();  // 重置首选项

    m_pd3dDevice->EndScene();  // 结束场景渲染

    return S_OK;  // 返回成功
}

void RtgDeviceD3D9::DrawPerfInformation() {
    //static char textPerf[1024];
    S_MEMDEF(textPerf, 1024)
    S_MEMPROTECTOR(textPerf, 1024, bMP)
    static DWORD prev = 0;
    DWORD        curr = rtMilliseconds();

    if (curr - prev >= 1000) {
        RtPerformance* pref = RtGetPref();
        rt2_sprintf(textPerf,
                    "           FPS : %.2f (%u mspf)\n"
                    "        Render : %u ms\n"
                    "        Update : %u ms\n"
                    "    Used VB/IB : %.2f M\n"
                    "   Free VM/AGP : %.2f M\n"
                    "    Proc Faces : %u\n"
                    "     Draw Call : %u\n"
                    "\n"
                    "Terrain Render : %u ms\n"
                    "  Actor Render : %u ms\n"
                    "  Actor Update : %u ms\n"
                    "   Skin Render : %u ms\n"
                    "   Skin Update : %u ms\n"
                    " Effect Render : %u ms\n"
                    " Effect Update : %u ms\n"
                    "     UI Render : %u ms\n"
                    "     UI Update : %u ms\n"
                    " Logic Process : %u ms\n"
                    "\n"
                    "     Actor Num : %u\n"
                    "  Particle Num : %u\n"
                    "\n"
                    " Actor Loading : %u\n"
                    "Actor Creating : %u\n"
                    "  Text Loading : %u\n"
                    " Text Creating : %u\n"
                    "\n"
                    "   Render Tile : %u\n"
                    " Render Object : %u\n",

                    RtGetRender()->GetFps(), (UINT)(1000.f / RtGetRender()->GetFps()),
                    pref->Time_Render, pref->Time_Update, pref->Used_BuffSize / 1048576.f,
                    RtGetRender()->GetAvailableTextureMem() / 1048576.f, pref->Proc_FaceNum,
                    pref->Draw_Call,

                    pref->TerrRender, pref->ActrRender, pref->ActrUpdate, pref->skinRender,
                    pref->skinUpdate, pref->efftRender, pref->efftUpdate, pref->Ui_Render,
                    pref->Ui_Update, pref->LogicProc,

                    pref->ActorNum, pref->partNum,

                    RtcGetActorManager()->GetLoadQueueSize(),
                    RtcGetActorManager()->GetCratQueueSize(), GetTextMgr()->GetLoadQueueSize(),
                    GetTextMgr()->GetCratQueueSize(),

                    pref->RenderTile_Num, pref->RenderObj_Num);
        prev = curr;
    }

    DrawString(m_fPerfx, m_fPerfy, 0xff00ffff, "Performance :");
    DrawString(m_fPerfx, m_fPerfy + 20, 0xff00ff00, textPerf);
}

static BYTE* g_pBlurDestBuffer = NULL;
static int   g_iBlurLastWidth = 0, g_iBlurLastHeight = 0;

template <class T>
void ImageBlur(BYTE* pBits, int iWidth, int iHeight, DWORD fA, DWORD fR, DWORD fG, DWORD fB) {
    int   i, j, w, h, o00, o01, o02, o10, o11, o12, c;
    int   b1, b2, b3, b4, b5, b6, b7, b8, b9;
    BYTE* b;

    b = pBits;
    if (g_pBlurDestBuffer == NULL || g_iBlurLastWidth < iWidth || g_iBlurLastHeight < iHeight) {
        if (g_pBlurDestBuffer)
            RtCoreMem().Free(g_pBlurDestBuffer);
        g_pBlurDestBuffer =
            (BYTE*)RtCoreMem().Alloc(iWidth * iHeight * sizeof(T), __FILE__, __LINE__);
        g_iBlurLastWidth = iWidth;
        g_iBlurLastHeight = iHeight;
    }
    w = iWidth;
    h = iHeight;
    for (i = 0; i < w; i++) {
        o00 = i * w;
        o01 = o00 - w;
        o02 = o00 + w;

        if (i == 0 || i == w - 1) {
            for (j = 0; j < h; j++) {
                c = o00 + j;
                ((T*)g_pBlurDestBuffer)[c] = ((T*)b)[c];
            }
            continue;
        }

        c = o00 + 0;
        ((T*)g_pBlurDestBuffer)[c] = ((T*)b)[c];
        c = o00 + (h - 1);
        ((T*)g_pBlurDestBuffer)[c] = ((T*)b)[c];

        for (j = 1; j < h - 1; j++) {
            o10 = j;
            o11 = o10 - 1;
            o12 = o10 + 1;

            b1 = o00 + o10;
            b2 = o01 + o10;
            b3 = o02 + o10;
            b4 = o00 + o11;
            b5 = o00 + o12;
            b6 = o01 + o12;
            b7 = o01 + o11;
            b8 = o02 + o12;
            b9 = o02 + o11;

            T cA = ((((T*)b)[b1] & fA) + (((T*)b)[b2] & fA) + (((T*)b)[b3] & fA) +
                    (((T*)b)[b4] & fA) + (((T*)b)[b5] & fA) + (((T*)b)[b6] & fA) +
                    (((T*)b)[b7] & fA) + (((T*)b)[b8] & fA) + (((T*)b)[b9] & fA)) /
                   9;
            T cR = ((((T*)b)[b1] & fR) + (((T*)b)[b2] & fR) + (((T*)b)[b3] & fR) +
                    (((T*)b)[b4] & fR) + (((T*)b)[b5] & fR) + (((T*)b)[b6] & fR) +
                    (((T*)b)[b7] & fR) + (((T*)b)[b8] & fR) + (((T*)b)[b9] & fR)) /
                   9;
            T cG = ((((T*)b)[b1] & fG) + (((T*)b)[b2] & fG) + (((T*)b)[b3] & fG) +
                    (((T*)b)[b4] & fG) + (((T*)b)[b5] & fG) + (((T*)b)[b6] & fG) +
                    (((T*)b)[b7] & fG) + (((T*)b)[b8] & fG) + (((T*)b)[b9] & fG)) /
                   9;
            T cB = ((((T*)b)[b1] & fB) + (((T*)b)[b2] & fB) + (((T*)b)[b3] & fB) +
                    (((T*)b)[b4] & fB) + (((T*)b)[b5] & fB) + (((T*)b)[b6] & fB) +
                    (((T*)b)[b7] & fB) + (((T*)b)[b8] & fB) + (((T*)b)[b9] & fB)) /
                   9;

            ((T*)g_pBlurDestBuffer)[b1] = (cA & fA) | (cR & fR) | (cG & fG) | (cB & fB);
        }
    }

    memcpy(b, g_pBlurDestBuffer, w * h * sizeof(T));
}

void RtgDeviceD3D9::Blur(RtgTextItem* texItem) {
    RTASSERT(texItem);
    RtgTextLock texLock;
    texLock.flag = Text_Lock_All;

    switch (texItem->format) {
        case D3DFMT_A8R8G8B8:
        case D3DFMT_X8R8G8B8: {
            if (GetTextMgr()->Lock(&texLock, texItem)) {
                ImageBlur<DWORD>((BYTE*)texLock.data, texLock.width, texLock.height, 0xff000000,
                                 0x00ff0000, 0x0000ff00, 0x000000ff);
                GetTextMgr()->Unlock(&texLock);
            }
        } break;

        case D3DFMT_X1R5G5B5: {
            if (GetTextMgr()->Lock(&texLock, texItem)) {
                ImageBlur<WORD>((BYTE*)texLock.data, texLock.width, texLock.pitch, 0x1000, 0x7C00,
                                0x003E, 0x001F);
                GetTextMgr()->Unlock(&texLock);
            }
        } break;

        case D3DFMT_R5G6B5: {
            if (GetTextMgr()->Lock(&texLock, texItem)) {
                ImageBlur<WORD>((BYTE*)texLock.data, texLock.width, texLock.pitch, 0x0000, 0xF800,
                                0x07E0, 0x001F);
                GetTextMgr()->Unlock(&texLock);
            }
        } break;

        case D3DFMT_A8:
        case D3DFMT_L8: {
            if (GetTextMgr()->Lock(&texLock, texItem)) {
                ImageBlur<WORD>((BYTE*)texLock.data, texLock.width, texLock.pitch, 0x0000, 0xF800,
                                0x07E0, 0x001F);
                GetTextMgr()->Unlock(&texLock);
            }
        } break;

        default:
            break;
    }
}

void RtgDeviceD3D9::Gray(RtgTextItem* texItem, bool bFirst) {
    RTASSERT(false);
    RTASSERT(texItem);
    //RtgTextLock texLock;

    /*  static BYTE *dest = NULL;
    static BYTE *dest2 = NULL;
    static int iLastWidth = 0, iLastHeight = 0;
    int i, j, k, w, h, c, o;
    BYTE g;
    BYTE *b;

    if(m_Texture.LockTexture(Tex,Lock))
    {
        b = (BYTE*)Lock.pBits;
        w = Lock.iWidth;
        h = Lock.iHeight;
        if(dest==NULL || iLastWidth<w || iLastHeight<h)
        {
            if (dest) RtCoreMem().Free(dest);
            dest = (BYTE*)RtCoreMem().Alloc(w*h*4, __FILE__, __LINE__);
            if (dest2) RtCoreMem().Free(dest2);
            dest2 = (BYTE*)RtCoreMem().Alloc(w*h*4, __FILE__, __LINE__);
            iLastWidth = w;
            iLastHeight = h;
        }
        if (bFirst)
        {
            for(i=0;i<w;i++)
            {
                for(j=0; j<h; j++)
                {
                    c = i*w*4+j*4;
                    g = (BYTE)(((int)(b[c]*1.6f + b[c+1]*9.6f + b[c+2]*4.8f))>>4);
                    for (k=0; k<3; k++)
                    {
                        o = c+k;
                        if (g>b[o])
                        {
                            dest [o] = g - b[o];
                            dest2[o] = 0;
                        }else
                        {
                            dest [o] = 0;
                            dest2[o] = b[o] - g;
                        }
                    }
                }
            }
            memcpy(b,dest,w*h*4);
        }else
        {
            memcpy(b,dest2,w*h*4);
        }
        m_Texture.UnlockTexture(Lock);
    }*/
}

void RtgDeviceD3D9::DrawString(float fX, float fY, DWORD dwColor, const char* pString,
                               DWORD dwFlags) {
    m_pFont->DrawText(fX, fY, dwColor, (char*)pString, dwFlags);
}

HRESULT RtgDeviceD3D9::OneTimeSceneInit() {
    return S_OK;
}

HRESULT RtgDeviceD3D9::InitDeviceObjects() {
    m_pFont->InitDeviceObjects(m_pd3dDevice);
    return S_OK;
}

bool RtgDeviceD3D9::SaveTexture(void* p, const char* fileName) {
    if (!p || !fileName)
        return false;

    D3DXIMAGE_FILEFORMAT fmt = D3DXIFF_BMP;

    const char* ext = strrchr(fileName, '.');

    if (ext) {
        ++ext;

        if (stricmp(ext, "jpg") == 0)
            fmt = D3DXIFF_JPG;
        else if (stricmp(ext, "tga") == 0)
            fmt = D3DXIFF_TGA;
        else if (stricmp(ext, "dds") == 0)
            fmt = D3DXIFF_DDS;
    }

    return SUCCEEDED(D3DXSaveTextureToFile(fileName, fmt, (LPDIRECT3DTEXTURE9)p, NULL));
}

BOOL RtgDeviceD3D9::SetMaterial(RtgVertex4& vDiffuse, RtgVertex4& vAmbient, RtgVertex4& vSpecular,
                                RtgVertex4& vEmissive, float fPower) {
    m_vMaterialDiffuse = vDiffuse;
    m_vMaterialAmbient = vAmbient;
    m_vMaterialSpecular = vSpecular;
    m_vMaterialEmissive = vEmissive;
    m_fMaterialPower = fPower;

    D3DMATERIAL9 mtrl;
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
    return m_pd3dDevice->SetMaterial(&mtrl) == D3D_OK;
}

HRESULT RtgDeviceD3D9::RestoreDeviceObjects() {
    m_iWndWidth = m_d3dsdBackBuffer.Width;
    m_iWndHeight = m_d3dsdBackBuffer.Height;

    switch (m_d3dsdBackBuffer.Format) {
        case D3DFMT_A8R8G8B8:
            m_eBackbufferFormat = RTG_TEXTURE_A8R8G8B8;
            break;
        case D3DFMT_X8R8G8B8:
            m_eBackbufferFormat = RTG_TEXTURE_X8R8G8B8;
            break;
        case D3DFMT_R8G8B8:
            m_eBackbufferFormat = RTG_TEXTURE_R8G8B8;
            break;
        case D3DFMT_A4R4G4B4:
            m_eBackbufferFormat = RTG_TEXTURE_A4R4G4B4;
            break;
        case D3DFMT_A1R5G5B5:
            m_eBackbufferFormat = RTG_TEXTURE_A1R5G5B5;
            break;
        case D3DFMT_R5G6B5:
            m_eBackbufferFormat = RTG_TEXTURE_R5G6B5;
            break;
        case D3DFMT_X1R5G5B5:
            m_eBackbufferFormat = RTG_TEXTURE_X1R5G5B5;
            break;
        case D3DFMT_X4R4G4B4:
            m_eBackbufferFormat = RTG_TEXTURE_X4R4G4B4;
            break;
        default:
            RtCoreLog().Warn("RtgDeviceD3D8: 不支持的Backbuffer格式\n");
            break;
    }

    if (m_d3dsdBackBuffer.Format == D3DFMT_X8R8G8B8 ||
        m_d3dsdBackBuffer.Format == D3DFMT_A8R8G8B8 || m_d3dsdBackBuffer.Format == D3DFMT_R8G8B8)
        m_iWndColorBits = 32;
    else
        m_iWndColorBits = 16;

    RtCoreLog().Info("RtgDeviceD3D9: 重新设置窗口大小为： %dx%dx%d\n", m_iWndWidth, m_iWndHeight,
                     m_iWndColorBits);

    D3DMATERIAL9 mtrl;
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
    m_pd3dDevice->SetMaterial(&mtrl);

    if (m_pRTManager)
        m_pRTManager->OnResetDevice();

    if (m_pEffectManager)
        m_pEffectManager->OnResetDevice();

    if (m_pTextMamager)
        m_pTextMamager->OnResetDevice();

    if (m_pBufferManager)
        m_pBufferManager->OnResetDevice();

    if (!OnRestoreDevice())
        return S_FALSE;

    m_pFont->RestoreDeviceObjects();
    m_pCamera->SetAspect(m_d3dsdBackBuffer.Width / (float)m_d3dsdBackBuffer.Height);
    m_pCamera->RestoreDevice();

    this->SetTextureFilterMip(m_eTextureFilterMip);
    this->SetTextureFilterMin(m_eTextureFilterMin);
    this->SetTextureFilterMag(m_eTextureFilterMag);
    this->SetTextureAddress(m_eTextureAddress);
    this->SetTextureBorderColor(m_dwTextureBorderColor);

    for (int i = 0; i < RTGRS_MAX; i++)
        if (m_listRenderState[i].bChanged)
            SetRenderState((RTGRenderState)(i), m_listRenderState[i].dwValue);

    m_pd3dDevice->SetRenderState(D3DRS_ALPHATESTENABLE, m_bAlphaTest);
    m_pd3dDevice->SetRenderState(D3DRS_ALPHAREF, (DWORD)0x00000001);
    m_pd3dDevice->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATEREQUAL);
    m_pd3dDevice->SetRenderState(D3DRS_ZFUNC, D3DCMP_LESSEQUAL);

    m_pd3dDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, m_bBlend);
    m_pd3dDevice->SetRenderState(D3DRS_SRCBLEND, enumRTGBlendFunc[m_eBlendSrcFunc]);
    m_pd3dDevice->SetRenderState(D3DRS_DESTBLEND, enumRTGBlendFunc[m_eBlendDestFunc]);

    m_pd3dDevice->SetRenderState(D3DRS_FOGENABLE, m_bFogEnable);
    m_pd3dDevice->SetRenderState(D3DRS_FOGCOLOR, m_dwFogColor);
    m_pd3dDevice->SetRenderState(D3DRS_FOGVERTEXMODE, D3DFOG_LINEAR);
    m_pd3dDevice->SetRenderState(D3DRS_FOGSTART, *(DWORD*)(&m_fFogStart));
    m_pd3dDevice->SetRenderState(D3DRS_FOGEND, *(DWORD*)(&m_fFogEnd));

    if (m_bCullFace)
        m_pd3dDevice->SetRenderState(D3DRS_CULLMODE, m_dwCullDefault);
    else
        m_pd3dDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);

    m_pd3dDevice->SetRenderState(D3DRS_TEXTUREFACTOR, m_dwTextureFactor);
    m_pd3dDevice->SetFVF(m_dwVertexFVF);

    if (GetTextMgr()) {
        if (IsUsePostProcessEffectOn()) {
            DestoryPostProcessResource(true);
            CreatePostProcessResource();
        } else if (IsGray()) {
            DestoryGrayResource(true);
            CreateGrayResource();
        }
    }

    return S_OK;
}

void RtgDeviceD3D9::SetFogEnable(BOOL bEnable) {
    m_bFogEnable = bEnable;
    m_pd3dDevice->SetRenderState(D3DRS_FOGENABLE, m_bFogEnable);
    if (bEnable) {
        m_pd3dDevice->SetRenderState(D3DRS_FOGVERTEXMODE, D3DFOG_LINEAR);
        m_pd3dDevice->SetRenderState(D3DRS_RANGEFOGENABLE, TRUE);
    }
}

void RtgDeviceD3D9::SetFogParameter(DWORD dwColor, float fNear, float fFar) {
    SetFogColor(dwColor);
    SetFogNear(fNear);
    SetFogFar(fFar);
}

void RtgDeviceD3D9::SetFogColor(DWORD dwColor) {
    m_dwFogColor = dwColor;
    m_pd3dDevice->SetRenderState(D3DRS_FOGCOLOR, m_dwFogColor);
}

void RtgDeviceD3D9::SetFogNear(float fNear) {
    m_fFogStart = fNear;
    m_pd3dDevice->SetRenderState(D3DRS_FOGSTART, *(DWORD*)(&m_fFogStart));
}

void RtgDeviceD3D9::SetFogFar(float fFar) {
    m_fFogEnd = fFar;
    m_pd3dDevice->SetRenderState(D3DRS_FOGEND, *(DWORD*)(&m_fFogEnd));
}

HRESULT RtgDeviceD3D9::InvalidateDeviceObjects() {
    if (!OnInvalidateDevice())
        return S_FALSE;

    if (m_pDefaultRenderTarget) {
        m_pDefaultRenderTarget->Release();
        m_pDefaultRenderTarget = NULL;
    }

    DestoryPostProcessResource(true);
    DestoryGrayResource(true);

    if (m_pRenderTargetDepthStencilSurface) {
        m_pRenderTargetDepthStencilSurface->Release();
        m_pRenderTargetDepthStencilSurface = NULL;
    }

    m_pDefaultDepthStencilSurface = NULL;
    m_pFont->InvalidateDeviceObjects();

    if (m_pRTManager)
        m_pRTManager->OnLostDevice();

    if (m_pEffectManager)
        m_pEffectManager->OnLostDevice();

    if (m_pTextMamager)
        m_pTextMamager->OnLostDevice();

    if (m_pBufferManager)
        m_pBufferManager->OnLostDevice();

    return S_OK;
}

HRESULT RtgDeviceD3D9::DeleteDeviceObjects() {
    if (m_pDefaultRenderTarget) {
        m_pDefaultRenderTarget->Release();
        m_pDefaultRenderTarget = NULL;
    }
    if (m_pRenderTargetDepthStencilSurface) {
        m_pRenderTargetDepthStencilSurface->Release();
        m_pRenderTargetDepthStencilSurface = NULL;
    }

    DestoryPostProcessResource(true);
    DestoryGrayResource(true);

    m_pDefaultDepthStencilSurface = NULL;
    m_pFont->DeleteDeviceObjects();

    SafeRelease(m_ActDecl_vnt);
    SafeRelease(m_ActDecl_vnct);

    if (m_pRTManager)
        DEL_ONE(m_pRTManager);

    if (m_pEffectManager)
        DEL_ONE(m_pEffectManager);

    if (m_pTextMamager)
        DEL_ONE(m_pTextMamager);

    if (m_pBufferManager)
        DEL_ONE(m_pBufferManager);

    return S_OK;
}

HRESULT RtgDeviceD3D9::FinalCleanup() {
    DEL_ONE(m_pFont);
    return S_OK;
}

HRESULT RtgDeviceD3D9::ConfirmDevice(D3DCAPS9* pCaps, DWORD dwBehavior, D3DFORMAT eFormat,
                                     D3DFORMAT eF2) {
    do {
        if (dwBehavior & D3DCREATE_PUREDEVICE)
            break;
        if (!(pCaps->TextureCaps & (D3DPTEXTURECAPS_ALPHAPALETTE | D3DPTEXTURECAPS_ALPHA)))
            break;
        //if(!(pCaps->RasterCaps & D3DPRASTERCAPS_ZTEST))         break;
        if (!(pCaps->AlphaCmpCaps & D3DPCMPCAPS_GREATEREQUAL))
            break;
        if (!(pCaps->ZCmpCaps & D3DPCMPCAPS_LESSEQUAL))
            break;
        if (!(pCaps->SrcBlendCaps & D3DPBLENDCAPS_DESTCOLOR))
            break;
        if (!(pCaps->DestBlendCaps & D3DPBLENDCAPS_DESTCOLOR))
            break;
        if (!(pCaps->TextureOpCaps & D3DTEXOPCAPS_DISABLE))
            break;
        return S_OK;
    } while (0);
    //RtCoreLog().Warn("RtgDeviceD3D9::ConfirmDevice 检查设备能力出错.\n");
    return E_FAIL;
}

HRESULT RtgDeviceD3D9::AdjustWindowForChange() {
    if (m_bCustomToggleFullscreen) {
        if (m_bWindowed) {
            ::ShowWindow(m_hRenderFullWnd, SW_HIDE);
            CD3DApplication::m_hWnd = m_hRenderWnd;
        } else {
            if (::IsIconic(m_hRenderFullWnd))
                ::ShowWindow(m_hRenderFullWnd, SW_RESTORE);
            ::ShowWindow(m_hRenderFullWnd, SW_SHOW);
            CD3DApplication::m_hWnd = m_hRenderFullWnd;
        }
        return S_OK;
    } else {
        return CD3DApplication::AdjustWindowForChange();
    }
}

long RtgDeviceD3D9::DeviceMsgProc(RTHWND hWnd, UINT uMsg, UINT wParam, UINT lParam) {
    return CD3DApplication::MsgProc((HWND)hWnd, uMsg, wParam, lParam);
}

int RtgDeviceD3D9::GetAvailableTextureMem() {
    return m_pd3dDevice->GetAvailableTextureMem();
}

bool RtgDeviceD3D9::OnD3D9ErrorMessage(const char* szMsg, int iType) {
    return OnErrorMessage(szMsg, iType);
}

BOOL RtgDeviceD3D9::SaveRenderTargetToFile(const char* szFileName, bool bJPG /*= ture*/) {
    BOOL               bResult = FALSE;
    LPDIRECT3DSURFACE9 pSrcSurface;
    D3DSURFACE_DESC    BackBufferDes;
    if (m_pd3dDevice->GetRenderTarget(0, &pSrcSurface) != D3D_OK) {
        return bResult;
    }
    pSrcSurface->GetDesc(&BackBufferDes);

    RtCoreLog().Info("BackBufferDesc::%d,%d\n", BackBufferDes.Width, BackBufferDes.Height);

    if (bJPG)
        bResult = D3DXSaveSurfaceToFile(szFileName, D3DXIFF_JPG, pSrcSurface, NULL, NULL) == D3D_OK;
    else
        bResult = D3DXSaveSurfaceToFile(szFileName, D3DXIFF_DDS, pSrcSurface, NULL, NULL) == D3D_OK;
    pSrcSurface->Release();
    return bResult;
}

bool RtgDeviceD3D9::CreateUIStateBlock(DWORD* dwToken) {
    if (m_pd3dDevice->BeginStateBlock() != D3D_OK)
        return false;
    m_pd3dDevice->SetRenderState(D3DRS_ZENABLE, FALSE);
    m_pd3dDevice->SetRenderState(D3DRS_FOGENABLE, FALSE);
    m_pd3dDevice->SetRenderState(D3DRS_MULTISAMPLEANTIALIAS, FALSE);
    m_pd3dDevice->SetRenderState(D3DRS_LIGHTING, FALSE);
    m_pd3dDevice->SetRenderState(D3DRS_ZFUNC, D3DCMP_EQUAL);
    m_pd3dDevice->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_POINT);
    m_pd3dDevice->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_POINT);
    m_pd3dDevice->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_NONE);
    return m_pd3dDevice->EndStateBlock((IDirect3DStateBlock9**)dwToken) == D3D_OK;
}

bool RtgDeviceD3D9::BeginRecordStateBlock() {
    return m_pd3dDevice->BeginStateBlock() == D3D_OK;
}

bool RtgDeviceD3D9::EndRecordStateBlock(DWORD* dwToken) {
    return m_pd3dDevice->EndStateBlock((IDirect3DStateBlock9**)dwToken) == D3D_OK;
}

bool RtgDeviceD3D9::DeleteStateBlock(DWORD dwToken) {
    IDirect3DStateBlock9* pStateBlock = (IDirect3DStateBlock9*)(DWORD_PTR)dwToken;
    SAFE_RELEASE(pStateBlock);
    return true;
}

bool RtgDeviceD3D9::ApplyStateBlock(DWORD dwToken) {
    return ((IDirect3DStateBlock9*)(DWORD_PTR)dwToken)->Apply() == D3D_OK;
}

bool RtgDeviceD3D9::CaptureStateBlock(DWORD dwToken) {
    return ((IDirect3DStateBlock9*)(DWORD_PTR)dwToken)->Capture() == D3D_OK;
}

BOOL RtgDeviceD3D9::IsWindowed() {
    return m_bWindowed;
}

BOOL RtgDeviceD3D9::HasFocus() {
    return m_bHasFocus;
}

BOOL RtgDeviceD3D9::IsActive() {
    return m_bActive;
}

DWORD RtgDeviceD3D9::GetDisplayMode(RtgDisplayMode* pModeBuf, DWORD dwBufSize) {
    D3DDeviceInfo*  pDeviceInfo = m_d3dSettings.PDeviceInfo();
    D3DAdapterInfo* pAdapterInfo = m_d3dSettings.PAdapterInfo();
    D3DDeviceCombo* pDeviceCombo;
    D3DDISPLAYMODE  displayMode;
    D3DFORMAT       adapterFormat;
    DWORD           idc, idm, idx, iBitDepth, dwR;
    std::set<DWORD> dmMap;

    if (dwBufSize == 0)
        dwBufSize = 0x0FFFFFFF;
    for (idc = 0; idc < pDeviceInfo->pDeviceComboList->Count(); idc++) {
        pDeviceCombo = (D3DDeviceCombo*)pDeviceInfo->pDeviceComboList->GetPtr(idc);
        if (pDeviceCombo) {
            //if( pDeviceCombo->IsWindowed )  HasWindowedDeviceCombo = true;
            //else                            HasFullscreenDeviceCombo = true;
            adapterFormat = pDeviceCombo->AdapterFormat;
            iBitDepth = 16;
            if (adapterFormat == D3DFMT_X8R8G8B8 || adapterFormat == D3DFMT_A8R8G8B8 ||
                adapterFormat == D3DFMT_R8G8B8) {
                iBitDepth = 32;
            }

            for (idm = 0; idm < pAdapterInfo->pDisplayModeList->Count(); idm++) {
                displayMode = *(D3DDISPLAYMODE*)pAdapterInfo->pDisplayModeList->GetPtr(idm);
                if (displayMode.Format == adapterFormat) {
                    dwR = ((iBitDepth & 0xFF) << 24) | ((displayMode.Width & 0x00000FFF) << 12) |
                          (displayMode.Height & 0x00000FFF);
                    dmMap.insert(dwR);
                }
            }
        }
    }
    if (pModeBuf) {
        std::set<DWORD>::iterator it = dmMap.begin();
        for (idx = 0; it != dmMap.end(); it++, idx++) {
            dwR = (*it);
            pModeBuf[idx].dwDepth = (dwR & 0xFF000000) >> 24;
            pModeBuf[idx].dwWidth = (dwR & 0x00FFF000) >> 12;
            pModeBuf[idx].dwHeight = (dwR & 0x00000FFF);
        }
    }
    return (DWORD)dmMap.size();
}

bool RtgDeviceD3D9::GetCanDoWindowed() {
    D3DDeviceInfo*  pDeviceInfo = m_d3dSettings.PDeviceInfo();
    D3DDeviceCombo* pDeviceCombo;
    DWORD           idc;
    bool            bCanDoWindowed = false;
    for (idc = 0; idc < pDeviceInfo->pDeviceComboList->Count(); idc++) {
        pDeviceCombo = (D3DDeviceCombo*)pDeviceInfo->pDeviceComboList->GetPtr(idc);
        if (pDeviceCombo) {
            if (pDeviceCombo->IsWindowed)
                bCanDoWindowed = true;
        }
    }
    return bCanDoWindowed;
}

const char* RtgDeviceD3D9::GetDeviceDesc() {
    D3DAdapterInfo* pAdapterInfo;
    for (UINT iai = 0; iai < m_d3dEnumeration.m_pAdapterInfoList->Count(); iai++) {
        pAdapterInfo = (D3DAdapterInfo*)m_d3dEnumeration.m_pAdapterInfoList->GetPtr(iai);
        if (pAdapterInfo->AdapterOrdinal == m_d3dSettings.AdapterOrdinal()) {
            return pAdapterInfo->AdapterIdentifier.Description;
        }
    }
    return "UnknowDevice";
}

}  // namespace rt_graph

/*------------------------------------------------------------------------
-   The End.
------------------------------------------------------------------------*/
