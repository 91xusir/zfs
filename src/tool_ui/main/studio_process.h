#pragma once

class CUiStudioProcess
{
public:
	CUiStudioProcess(void);
	virtual ~CUiStudioProcess(void);

public:
    bool Create();
    bool Destroy();
    void Run(DWORD dwDelta);
    void Render();

    bool MouseDown(int button);
    bool MouseUp(int button);
    bool MouseDClick(int button);
    bool MouseMove(int x, int y);
    bool MouseWheel(int delta, int x, int y);
    bool KeyChar(int ch);
    bool KeyDown(int key);
    bool KeyUp(int key);

    void SetOperateWidget(RtwWidget* pWidget);

    RtwWidget* getOperateWidget() {/*LOG("");*/ return m_pOperateWidget; }
	void SetMoveWidgetAttrib(RtwWidget* pWidget, RtwTree::Item* pSelectTreeItem, SSize offset = SSize(0,0));
    bool                m_bShowWidgetsLine;

	RtwWidget*          m_pOperateWidget;
	list<RtwWidget*>	m_SelectedWidgetList;
private:
	bool				m_bStretching;
	bool				m_bCtrl;
    UiLayerList         m_ExcludeLayers;
    RtwWidget*          m_pDragWidget;
	list<RtwWidget*>::iterator m_lri;
	IUiSound*			m_pSound;
};
