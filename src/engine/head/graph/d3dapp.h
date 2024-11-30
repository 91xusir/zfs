
//-----------------------------------------------------------------------------
// File: D3DApp.h
//
// Desc: Application class for the Direct3D samples framework library.
//
// Copyright (c) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------
#ifndef D3DAPP_H
#define D3DAPP_H

#include "d3denumeration.h"
#include "d3dsettings.h"
#include <windef.h>

#define WM_USER_QUIT (WM_USER + 886)  // �Զ�����Ϣ�Ķ���

namespace rt_graph {
class CRtgAppFrame;
}

namespace rt_graph_dx9 {

//-----------------------------------------------------------------------------
// Error codes
//-----------------------------------------------------------------------------
enum APPMSGTYPE { MSG_NONE, MSGERR_APPMUSTEXIT, MSGWARN_SWITCHEDTOREF };

#define D3DAPPERR_NODIRECT3D 0x82000001
#define D3DAPPERR_NOWINDOW 0x82000002
#define D3DAPPERR_NOCOMPATIBLEDEVICES 0x82000003
#define D3DAPPERR_NOWINDOWABLEDEVICES 0x82000004
#define D3DAPPERR_NOHARDWAREDEVICE 0x82000005
#define D3DAPPERR_HALNOTCOMPATIBLE 0x82000006
#define D3DAPPERR_NOWINDOWEDHAL 0x82000007
#define D3DAPPERR_NODESKTOPHAL 0x82000008
#define D3DAPPERR_NOHALTHISMODE 0x82000009
#define D3DAPPERR_NONZEROREFCOUNT 0x8200000a
#define D3DAPPERR_MEDIANOTFOUND 0x8200000b
#define D3DAPPERR_RESETFAILED 0x8200000c
#define D3DAPPERR_NULLREFDEVICE 0x8200000d

//-----------------------------------------------------------------------------
// Name: class CD3DApplication
// Desc: A base class for creating sample D3D9 applications. To create a simple
//       Direct3D application, simply derive this class into a class (such as
//       class CMyD3DApplication) and override the following functions, as
//       needed:
//          OneTimeSceneInit()    - To initialize app data (alloc mem, etc.)
//          InitDeviceObjects()   - To initialize the 3D scene objects
//          FrameMove()           - To animate the scene
//          Render()              - To render the scene
//          DeleteDeviceObjects() - To cleanup the 3D scene objects
//          FinalCleanup()        - To cleanup app data (for exitting the app)
//          MsgProc()             - To handle Windows messages
//-----------------------------------------------------------------------------
class CD3DApplication {
   protected:
    CD3DEnumeration m_d3dEnumeration;
    CD3DSettings    m_d3dSettings;

    // Internal variables for the state of the app
    bool m_bWindowed;
    bool m_bActive;
    bool m_bReady;
    bool m_bHasFocus;
    bool m_bDeviceLost;
    bool m_bMinimized;
    bool m_bMaximized;
    bool m_bIgnoreSizeChange;
    bool m_bDeviceObjectsInited;
    bool m_bDeviceObjectsRestored;

    // Internal variables used for timing
    bool m_bFrameMoving;
    bool m_bSingleStep;

    // Internal error handling function
    HRESULT DisplayErrorMsg(HRESULT hr, DWORD dwType);

    // Internal functions to manage and render the 3D scene
    static bool     ConfirmDeviceHelper(D3DCAPS9* pCaps, VertexProcessingType vertexProcessingType,
                                        D3DFORMAT adapterFormat, D3DFORMAT backBufferFormat);
    void            BuildPresentParamsFromSettings();
    bool            FindBestWindowedMode(bool bRequireHAL, bool bRequireREF);
    bool            FindBestFullscreenMode(bool bRequireHAL, bool bRequireREF);
    HRESULT         ChooseInitialD3DSettings();
    HRESULT         Initialize3DEnvironment();
    HRESULT         HandlePossibleSizeChange();
    HRESULT         Reset3DEnvironment();
    HRESULT         ToggleFullscreen();
    HRESULT         ForceWindowed();
    HRESULT         UserSelectNewDevice();
    void            Cleanup3DEnvironment();
    HRESULT         Render3DEnvironment();
    virtual HRESULT AdjustWindowForChange();
    virtual void    UpdateStats();
    virtual void    OnClose(){};

   protected:
    // ���ڴ�������Ⱦ3D��������Ҫ����
    D3DPRESENT_PARAMETERS m_d3dpp;  // Parameters for CreateDevice/Reset

    HWND              m_hWnd;             // Ӧ�ó����������
    HWND              m_hWndFocus;        // D3D ���㴰�ڣ�ͨ���� m_hWnd ��ͬ��
    HMENU             m_hMenu;            // Ӧ�ó���˵�����ȫ��ʱ���������
    LPDIRECT3D9       m_pD3D;             // �� D3D ����
    LPDIRECT3DDEVICE9 m_pd3dDevice;       // D3D ��Ⱦ�豸
    D3DCAPS9          m_d3dCaps;          // �豸�Ĺ��ܲ���
    D3DSURFACE_DESC   m_d3dsdBackBuffer;  // ��̨�������ı�������
    DWORD             m_dwCreateFlags;    // ָʾ�����Ӳ�����㴦��
    DWORD             m_dwWindowStyle;    // ����ģʽ�л����ѱ��洰����ʽ
    DWORD             m_dwFpsLocked;      // ����֡��
    RECT              m_rcWindowBounds;   // ����ģʽ�л����ѱ��洰�ڱ߽�
    RECT              m_rcWindowClient;   // ����ģʽ�л����ѱ���ͻ�����С

    // ���ʱ��صı���
    FLOAT         m_fTime;               // ��ǰʱ�䣨����Ϊ��λ��
    FLOAT         m_fElapsedTime;        // ����һ֡����������ʱ��
    unsigned long m_dwElapsedMS;         // ����һ֡���������ĺ�����
    FLOAT         m_fFPS;                // ˲ʱ֡��
    TCHAR         m_strDeviceStats[90];  // ���� D3D �豸״̬���ַ���
    //lyymark ֡��Ϣ
    TCHAR m_strFrameStats[90];  // ����֡״̬���ַ���

    // Ӧ�ó���Ŀ���д����
    TCHAR* m_strWindowTitle;             // Ӧ�ó��򴰿ڵı���
    DWORD  m_dwCreationWidth;            // ���ڴ������ڵĿ��
    DWORD  m_dwCreationHeight;           // ���ڴ������ڵĸ߶�
    bool   m_bShowCursorWhenFullscreen;  // �Ƿ���ȫ��ģʽ����ʾ���
    bool   m_bClipCursorWhenFullscreen;  // �Ƿ���ȫ��ģʽ�����ƹ��λ��
    bool   m_bStartFullscreen;           // �Ƿ�������ʱ��ȫ��ģʽ����Ӧ�ó���

    DWORD m_deviceThread;  // �豸�̵߳ı�ʶ��
    UINT  m_uRenderFrame;  // ��Ⱦ֡�ļ�����
    bool  m_bvsync;        // �Ƿ����ô�ֱͬ�� (V-Sync)

    // Overridable functions for the 3D scene created by the app
    virtual HRESULT ConfirmDevice(D3DCAPS9*, DWORD, D3DFORMAT, D3DFORMAT) { return S_OK; }

    virtual HRESULT OneTimeSceneInit() { return S_OK; }

    virtual HRESULT InitDeviceObjects() { return S_OK; }

    virtual HRESULT RestoreDeviceObjects() { return S_OK; }

    virtual HRESULT FrameMove() { return S_OK; }

    virtual HRESULT Render() { return S_OK; }

    virtual HRESULT InvalidateDeviceObjects() { return S_OK; }

    virtual HRESULT DeleteDeviceObjects() { return S_OK; }

    virtual HRESULT FinalCleanup() { return S_OK; }

    virtual void OnGetDeviceCaps(){};

    bool SetFullScreenMode(DWORD dwWidth, DWORD dwHeight, DWORD dwDepth);

   public:
    // Functions to create, run, pause, and clean up the application
    virtual HRESULT Create(HINSTANCE hInstance);
 
    virtual INT     Run();
    virtual LRESULT MsgProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
    virtual void    Pause(bool bPause);

    virtual ~CD3DApplication() {
        if (m_pD3D)
            m_pD3D->Release();
        m_pD3D = NULL;
    }

    void IncreaseRenderFrame() { ++m_uRenderFrame; }

    UINT GetRenderFrame() const { return m_uRenderFrame; }

    inline HWND& GetHWND() { return m_hWnd; }  // The main app window



    void LockFpsPerFrame(DWORD dwMillisecondPerFrame) { m_dwFpsLocked = dwMillisecondPerFrame; }

    float GetFps() const { return m_fFPS; }

    TCHAR* GetFrameStats() { return m_strFrameStats; }

    DWORD GetDeviceThreadId() const { return m_deviceThread; }

    // Internal constructor
    CD3DApplication();

    rt_graph::CRtgAppFrame* m_pAppFrame;

    void BeginRenderViewPort(DWORD dwX, DWORD dwY, DWORD dwWeight, DWORD dwHeight, float fMin,
                             float fMax);
    void EndRenderViewPort();
    D3DVIEWPORT9 m_d3dLastViewPort;
};

}  // namespace rt_graph_dx9

#endif

using namespace rt_graph_dx9;
