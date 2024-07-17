#pragma once

class UiLayerStudio;
class UiForm_Attributes;
class UiForm_WidgetTree;
class UiForm_MainMenu;
class UiForm_ToolBar;

class UiMain
{
public:
	UiMain();
	~UiMain();

	void Run(DWORD dwDelta);
    bool Init();

public:
    UiLayerStudio*          m_pLay_Studio;
    UiForm_Attributes*      m_pFrm_Attributes;
    UiForm_WidgetTree*      m_pFrm_WidgetTree;
    UiForm_MainMenu*        m_pFrm_MainMenu;
    UiForm_ToolBar*         m_pFrm_Toolbar;

public:
};
 
extern UiMain *g_pUiMain;


class IUiGameWidget
{
public:
    IUiGameWidget() {/*LOG("");*/ }
    ~IUiGameWidget() {/*LOG("");*/ }

public:
    virtual bool Init() = 0;
    virtual void Refresh() = 0;
    virtual void Show() = 0;
    virtual void Hide() = 0;
    virtual bool IsVisible() = 0;
};

#include "uilayer_studio.h"
#include "uiform_widget_hint.h"
#include "uiform_attributes.h"
#include "uiform_widgettree.h"
#include "uiform_mainmenu.h"
#include "uiform_toolbar.h"

