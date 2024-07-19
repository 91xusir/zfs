#pragma once

class CUiStudioWorld;
class CUiStudioProcess;
class CEventFrame;
class UiMain;
#include "driver_system.h"

extern HWND g_hWndRenderWindow;

CEventFrame* getMainFrame();
CUiStudioProcess* getStudioProcess();
UiMain* getUiMain();

enum EStudioState
{
    ss_Edit,
    ss_Preview
};

class CEventFrame : public CRtgAppFrame
{
    RT_DECLARE_DYNCREATE(CEventFrame, CRtgAppFrame, 0, "")

public:
	CEventFrame(void);
	virtual ~CEventFrame(void);

public:
	virtual bool OnDeviceInit();
	virtual bool OnRestoreDevice();
	virtual bool OnInvalidateDevice();
	virtual bool OnFrameInit();
	virtual void OnFrameClose();
	virtual void OnFrameMove(float fDifTime);
	virtual void OnRender();
	virtual bool OnErrorMessage(const char* szMsg, int iType); // iType 0 错误, 1 必须退出的错误

	virtual void OnMouseLDown(int x, int y);
	virtual void OnMouseLDrag(int x, int y, int increaseX, int increaseY);
	virtual void OnMouseLUp(int x, int y);

	virtual void OnMouseRDown(int x, int y);
	virtual void OnMouseRDrag(int x, int y, int increaseX, int increaseY);
	virtual void OnMouseRUp(int x, int y);

	virtual void OnMouseMDown(int x, int y);
	virtual void OnMouseMDrag(int x, int y, int increaseX, int increaseY);
	virtual void OnMouseMUp(int x, int y);
	virtual void OnMouseMove(int x, int y, int increaseX, int increaseY);
	virtual void OnMouseWheel(long vDelta, int x, int y);

	virtual void OnKeyDown(int iChar, bool bAltDown);
    virtual void OnKeyUp(int iChar, bool bAltDown);
    virtual void OnKeyChar(int iChar, bool bAltDown);

    EStudioState getState() {/*LOG("");*/ return m_StudioState; }
    void SetState(EStudioState _state) {/*LOG("");*/ m_StudioState = _state; }

#if DO_CONSOLE
    virtual bool OnConsoleCommand(const char* szCommand);
#endif

public:
    RtgDevice* getRenderDevice() {/*LOG("");*/ return m_pDevice; }

    void ClearPreviewWidgets();

public:
    EStudioState            m_StudioState;
    CUiStudioProcess*       m_pStudioProcess;

    bool                    m_bMouseLDown;
    bool                    m_bMouseRDown;

    std::list<DWORD>        m_PreviewWidgets;   // 预览时生成的Widgets，预览结束时应该删除掉
	CDriverSystem			mDriverSystem;
};

class CUiStudioCamera : public RtgCameraGame
{
public:
    RT_DECLARE_DYNCREATE(CUiStudioCamera, RtgCameraGame, 0, "")
    CUiStudioCamera();
    virtual ~CUiStudioCamera();
};
