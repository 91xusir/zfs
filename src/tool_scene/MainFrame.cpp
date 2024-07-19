// MainFrame.cpp : CMainFrame 类的实现
//

#include "stdafx.h"
#include "tool_scene.h"
#include "resource.h"

#include "BrushMapView.h"
#include "BrushVertexView.h"
#include "BrushColorView.h"
#include "area_list_view.h"
//tim.yang  包含路点列表头文件
#include "pathpoint_list_view.h"
#include "MainFrame.h"

#include "scene/rt_scene.h"
#include "rts_scene_tool.h"
#include "brush_tool.h"
#include "EditorFrame.h"
#include "SceneEditorDoc.h"
#include "SceneEditorView.h"
#include "brush_tool_object.h"
#include "DlgHeightMap.h"
#include <stack>
#include "DlgNewBlock.h"
#include "GridItems.h"

#ifdef RT_DEBUG_NEW
#   undef new
#endif

#define COLOR_EXCHANGE(x) (((x&0x000000FF)<<16)|(x&0x0000FF00)|((x&0x00FF0000)>>16))

// CMainFrame

IMPLEMENT_DYNCREATE(CMainFrame, CFrameWnd)

BEGIN_MESSAGE_MAP(CMainFrame, CFrameWnd)
    //{{AFX_MSG_MAP(CMainFrame)
	ON_WM_CREATE()
	ON_COMMAND_RANGE(ID_VIEW_BRUSH_PROPERTY, ID_VIEW_AREA_LIST, OnShowView)
    ON_UPDATE_COMMAND_UI_RANGE(ID_VIEW_BRUSH_PROPERTY, ID_VIEW_AREA_LIST, OnUpdateUIBrushView)
	//tim.yang  添加路点列表的消息映射
	ON_COMMAND_RANGE(ID_VIEW_BRUSH_PROPERTY, ID_VIEW_PATHPOINT_LIST, OnShowView)
	ON_UPDATE_COMMAND_UI_RANGE(ID_VIEW_BRUSH_PROPERTY, ID_VIEW_PATHPOINT_LIST, OnUpdateUIBrushView)
	//end
	ON_COMMAND(ID_MENU_THEMES_OFFICE2000, OnThemesOffice2000)
	ON_UPDATE_COMMAND_UI(ID_MENU_THEMES_OFFICE2000, OnUpdateThemesOffice2000)
	ON_COMMAND(ID_MENU_THEMES_OFFICEXP, OnThemesOfficexp)
	ON_UPDATE_COMMAND_UI(ID_MENU_THEMES_OFFICEXP, OnUpdateThemesOfficexp)
	ON_COMMAND(ID_MENU_THEMES_OFFICEVISIO, OnThemesOfficeVisio)
	ON_UPDATE_COMMAND_UI(ID_MENU_THEMES_OFFICEVISIO, OnUpdateThemesOfficeVisio)
	ON_COMMAND(ID_MENU_THEMES_OFFICE2003, OnThemesOffice2003)
	ON_UPDATE_COMMAND_UI(ID_MENU_THEMES_OFFICE2003, OnUpdateThemesOffice2003)
	ON_COMMAND(ID_MENU_THEMES_NATIVEOFFICE, OnThemesNativexptheme)
	ON_UPDATE_COMMAND_UI(ID_MENU_THEMES_NATIVEOFFICE, OnUpdateThemesNativexptheme)
	ON_MESSAGE(XTPWM_DOCKINGPANE_NOTIFY, OnDockingPaneNotify)
    ON_MESSAGE(XTPWM_PROPERTYGRID_NOTIFY, OnGridNotify)
    ON_NOTIFY(TVN_SELCHANGED, ID_TREE_OBJ_VIEW, OnTreeCtrlSelectChanged)
	ON_XTP_CREATECOMMANDBAR()
    ON_COMMAND(ID_NEW_BLOCK, OnNewBlock)
	//ON_COMMAND(ID_FILE_BACK, OnFileBack)
    ON_COMMAND(ID_GAME_PLAY, OnLockGameView)
    ON_COMMAND(ID_GAME_HEIGHTMAP, OnHeightmapView)
    ON_COMMAND(ID_TOOL_TGA_REVERSE, OnToolTgaReverse)
    ON_COMMAND(ID_TOOL_RECOMPUTE_OBJECT_LIGHT, OnToolRecomputeObjectLight)
    ON_COMMAND(ID_TOOL_RECOMPUTE_OBJECT_BELONG, OnToolRecomputeObjectBelong)
    ON_COMMAND(ID_TOOL_CHECK_SAME_OBJECT, OnToolCheckSameObject)
    ON_COMMAND(ID_TOOL_DELETE_SAME_OBJECT, OnToolDeleteSameObject)
	ON_COMMAND(ID_VIEW_CENTER, OnViewCenter)
    ON_COMMAND(ID_GAME_SNAPSHOT, OnToolSnapShotView)
    ON_COMMAND(ID_GAME_MOVEOBJECT, OnToolMoveObject)
    ON_COMMAND(ID_GAME_NORMALIZABLE, OnToolOriginallyObjectSize)
    ON_COMMAND(ID_GAME_ADJUSTOBJECT, OnToolAdjustObject)
    ON_COMMAND(ID_TOOL_LOAD_ENV, OnToolLoadEnv)
    ON_COMMAND(ID_TOOL_SAVE_ENV, OnToolSaveEnv)
    ON_COMMAND(ID_TOOL_ADD_AREA_FROM_FILE, OnToolAddAreaFromFile)
	ON_COMMAND(ID_OUTPUT_NPC_INFO,OnToolOutPutNpcInfo)
    ON_COMMAND_RANGE(ID_BTN_SHOW_LINE, ID_BTN_SHOW_MINIMAP, OnShowMask)
    ON_COMMAND(ID_BUILD_SM, OnBuildSM)
    ON_COMMAND(ID_BUILD_SM2, OnBuildSM2)
    ON_COMMAND(ID_SHOWNORMAL, OnShowNormal)
    ON_COMMAND(ID_ENABLE_POSEEFFECT, OnEnablePE)
    ON_COMMAND(ID_SAVECURRBLOCK, OnSaveCurrBlock)
    ON_UPDATE_COMMAND_UI_RANGE(ID_BTN_SHOW_LINE, ID_BTN_SHOW_MINIMAP, OnUpdateShowMask)
    ON_WM_CLOSE()
    //}}AFX_MSG_MAP
    ON_XTP_CREATECOMMANDBAR()
    ON_XTP_CREATECONTROL()
END_MESSAGE_MAP()

static UINT indicators[] =
{
	ID_SEPARATOR,           // 状态行指示器
	ID_INDICATOR_CAPS,
	ID_INDICATOR_NUM,
	ID_INDICATOR_SCRL,
};


// CMainFrame 构造/析构

CMainFrame::CMainFrame()
{
    m_pWndBrushMapView = NULL;
    m_pPaneLayout = NULL;
}

CMainFrame::~CMainFrame()
{
	if (m_pWndBrushMapView)
	{
		delete m_pWndBrushMapView;
	}
}

BOOL CMainFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	if( !CFrameWnd::PreCreateWindow(cs) )
		return FALSE;
	return TRUE;
}

int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
//    guard;
	if (CFrameWnd::OnCreate(lpCreateStruct) == -1)
		return -1;

	/*
	if (!m_wndToolBar.CreateEx(this, TBSTYLE_FLAT, WS_CHILD | WS_VISIBLE | CBRS_TOP
		| CBRS_GRIPPER | CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC) ||
		!m_wndToolBar.LoadToolBar(IDR_MAINFRAME))
	{
		TRACE0("未能创建工具栏\n");
		return -1;      // 未能创建
	}
	*/

	if (!m_wndStatusBar.Create(this) ||
		!m_wndStatusBar.SetIndicators(indicators,
		  sizeof(indicators)/sizeof(UINT)))
	{
		TRACE0("未能创建状态栏\n");
		return -1;      // 未能创建
	}

	if (!InitCommandBars())
		return -1;

	CXTPPaintManager::SetTheme(xtpThemeOfficeXP);

	CXTPCommandBars* pCommandBars = GetCommandBars();
	
	pCommandBars->SetMenu(_T("菜单栏"), IDR_MAINFRAME); 

	CXTPToolBar* pStandardBar = (CXTPToolBar*)pCommandBars->Add(_T("标准"), xtpBarTop);
	if (!pStandardBar || !pStandardBar->LoadToolBar(IDR_MAINFRAME))
	{
		TRACE0("未能创建[标准]工具栏\n");
		return -1;      // 未能创建
	}

    CXTPToolBar* pShowBar = (CXTPToolBar*)pCommandBars->Add(_T("显示"), xtpBarTop); 
    if (!pShowBar || !pShowBar->LoadToolBar(IDR_TOOLBAR_SHOW))
    {
        TRACE0("未能创建[显示]工具栏\n");
        return -1;
    }
    DockRightOf(pShowBar, pStandardBar);

	XTPImageManager()->SetMaskColor(RGB(255, 0, 255));
	XTPImageManager()->SetIcons(IDR_TOOLBAR_BRUSHS);

	m_paneManager.InstallDockingPanes(this);
	m_paneManager.SetTheme(xtpPaneThemeOffice);
    //tim.yang  在右边栏中添加路点列表的图标
	int nIDIcons[] = {
        ID_VIEW_BRUSH_MAP, 
        ID_VIEW_BRUSH_OBJECT, 
        ID_VIEW_BRUSH_GRASS,
        ID_VIEW_BRUSH_PROPERTY, 
        ID_VIEW_BRUSH_VERTEX, 
        ID_VIEW_BRUSH_COLOR, 
        6, 
        ID_VIEW_AREA_LIST,
        ID_VIEW_PATHPOINT_LIST};
	m_paneManager.SetIcons(IDB_BITMAP_BRUSH_ICONS, nIDIcons, sizeof(nIDIcons)/sizeof(nIDIcons[0]), RGB(255, 0, 255));
	//end
	XTPPaintManager()->RefreshMetrics();

	// Create docking panes.

    // 包括顶点刷子, 贴图(选择贴图)刷子
	CXTPDockingPane* paneMap = m_paneManager.CreatePane(ID_VIEW_BRUSH_MAP, CRect(0, 0,200, 120), xtpPaneDockRight);

    // 包括放置、移动(旋转)、删除，选择物件
    CXTPDockingPane* paneObject = m_paneManager.CreatePane(ID_VIEW_BRUSH_OBJECT, CRect(0, 0,200, 120), xtpPaneDockRight);


    CXTPDockingPane* paneGrass = m_paneManager.CreatePane(ID_VIEW_BRUSH_GRASS, CRect(0, 0,200, 120), xtpPaneDockRight);

    // 显示当前选中内容的属性
    CXTPDockingPane* paneAttribute = m_paneManager.CreatePane(ID_VIEW_BRUSH_PROPERTY, CRect(0, 0,200, 120), xtpPaneDockRight);

    // 显示当前选中内容的属性
    CXTPDockingPane* paneVertex = m_paneManager.CreatePane(ID_VIEW_BRUSH_VERTEX, CRect(0, 0,200, 120), xtpPaneDockRight);

    // 显示当前选中内容的属性
    //CXTPDockingPane* paneColor = m_paneManager.CreatePane(ID_VIEW_BRUSH_COLOR, CRect(0, 0,200, 120), xtpPaneDockRight);

    // 显示当前选中内容的属性
    CXTPDockingPane* paneArea = m_paneManager.CreatePane(ID_VIEW_AREA_LIST, CRect(0, 0,200, 120), xtpPaneDockRight);
    //tim.yang  创建路点列表的LIST
	CXTPDockingPane* panePathPoint = m_paneManager.CreatePane(ID_VIEW_PATHPOINT_LIST, CRect(0, 0,200, 120), xtpPaneDockRight);
	//end
	m_paneManager.AttachPane(paneObject, paneMap);
	m_paneManager.AttachPane(paneAttribute, paneMap);
    m_paneManager.AttachPane(paneGrass, paneMap);
	m_paneManager.AttachPane(paneVertex, paneMap);
	//m_paneManager.AttachPane(paneColor, paneMap);
	m_paneManager.AttachPane(paneArea, paneMap);
	//tim.yang   添加路点列表的LIST
	m_paneManager.AttachPane(panePathPoint, paneMap);


    // paneArea->Close();
    paneAttribute->Select();


    // If "RtEngineSceneEditorLayout" layout does not exist, save the current configuration 
    // as "RtEngineSceneEditorLayout", otherwise load previous state and set it as the active
    // layout.
    /**
    m_pPaneLayout = m_paneManager.CreateLayout();

    if (m_pPaneLayout->LoadFromFile(_T("tool_scene_layout.xml"), _T("RtEngineSceneEditorLayout")))
    {
        m_paneManager.SetLayout(m_pPaneLayout);
    }else
    {
        m_paneManager.GetLayout(m_pPaneLayout);
    }

    LoadCommandBars(_T("RtEngineSceneEditorSave"));
    /**/

    // Core init
#ifdef _DEBUG
    rtCoreInit("tool_scene.ini");
    //rtCoreInit("tool_scene", 0, (RtOutputDevice*)-1, (RtOutputDevice*)-1, 0);
#else
    rtCoreInit("tool_scene.ini");
#endif

	// TODO: 如果不需要工具栏可停靠，则删除这三行
	//m_wndToolBar.EnableDocking(CBRS_ALIGN_ANY);
	//EnableDocking(CBRS_ALIGN_ANY);
	//DockControlBar(&m_wndToolBar);

	return 0;
 //   guard_end_ex;
}

void CMainFrame::OnClose() 
{
    /*
    SaveCommandBars(_T("RtEngineSceneEditorSave"));
    m_paneManager.GetLayout(m_pPaneLayout);
    m_pPaneLayout->SaveToFile(_T("tool_scene_layout.xml"), _T("RtEngineSceneEditorLayout"));
    delete m_pPaneLayout;
    */
	if(MessageBox("退出","确认",MB_OKCANCEL) == IDCANCEL)return;
    CFrameWnd::OnClose();
}

BOOL CMainFrame::DestroyWindow() 
{
   // guard;

    if (m_pWndBrushMapView->GetSafeHwnd())
    {
        m_pWndBrushMapView->DestroyWindow();
        DEL_ONE(m_pWndBrushMapView);
    }
    if (m_wndBrushObjectView.GetSafeHwnd())
    {
        m_ilBrushObjectView.DeleteImageList();
        m_wndBrushObjectView.DestroyWindow();
    }
    if (m_wndBrushVertexView.GetSafeHwnd())
    {
        m_wndBrushVertexView.DestroyWindow();
    }
    if (m_wndAreaView.GetSafeHwnd())
    {
        m_wndAreaView.DestroyWindow();
    }
	//tim.yang 销毁路点列表窗口
	if (m_wndPathPointView.GetSafeHwnd())
	{
		m_wndPathPointView.DestroyWindow();
	}
    m_paneManager.DestroyAll();

    BOOL bResult = CFrameWnd::DestroyWindow();

    // Exit Core
    rtCoreExit();

	return bResult;
    //guard_end_ex;
}

// CMainFrame 诊断

bool CEditorFrame::OnErrorMessage(const char* szMsg, int iType)
{
    //guard;
    ((CSceneEditorView*)m_pView)->Pause();
    //ERR1("RtGraph错误: %s.\n", szMsg);
    CHECKEX(0);
    return false;
    //unguard;
}

#ifdef _DEBUG
void CMainFrame::AssertValid() const
{
	CFrameWnd::AssertValid();
}

void CMainFrame::Dump(CDumpContext& dc) const
{
	CFrameWnd::Dump(dc);
}

#endif //_DEBUG


// CMainFrame 消息处理程序

void CMainFrame::OnShowView(UINT nID)
{
	m_paneManager.ShowPane(nID);
}

void CMainFrame::OnUpdateUIBrushView(CCmdUI* pCmdUI)
{
    pCmdUI->SetCheck(m_paneManager.IsPaneSelected(m_paneManager.FindPane(pCmdUI->m_nID))? TRUE: FALSE);
}

LRESULT CMainFrame::OnDockingPaneNotify(WPARAM wParam, LPARAM lParam)
{
	if (wParam == XTP_DPN_SHOWWINDOW)
	{
		CXTPDockingPane* pPane = (CXTPDockingPane*)lParam;

		if (!pPane->IsValid())
		{
			switch (pPane->GetID())
			{
			case ID_VIEW_BRUSH_MAP:         pPane->Attach(CreateBrushMapView()); break;
			case ID_VIEW_BRUSH_OBJECT:      pPane->Attach(CreateBrushObjectView()); break;
            case ID_VIEW_BRUSH_GRASS :      pPane->Attach(CreateBrushGrassView());break;
			case ID_VIEW_BRUSH_PROPERTY:    pPane->Attach(CreateBrushPropertyView()); break;				
			case ID_VIEW_BRUSH_VERTEX:      pPane->Attach(CreateBrushVertexView()); break;				
			//case ID_VIEW_BRUSH_COLOR:       pPane->Attach(CreateBrushColorView()); break;				
			case ID_VIEW_AREA_LIST:         pPane->Attach(CreateAreaListView()); break;	
			//tim.yang  路点列表的创建
			case ID_VIEW_PATHPOINT_LIST:    pPane->Attach(CreatePathPointListView());break;
			}
		}
		return 1;
	}
	return 0;
}

CWnd* CMainFrame::CreateBrushMapView()
{
	if (m_pWndBrushMapView->GetSafeHwnd() == 0)
	{
        m_pWndBrushMapView = new CBrushMapForm();
        m_pWndBrushMapView->Create(_T("STATIC"), NULL, WS_CHILD, CRect(0, 0, 0, 0), this, 0);
        ((CBrushMapForm*)m_pWndBrushMapView)->OnInitialUpdate();
	}
	return m_pWndBrushMapView;
}

class CBrushObjectXmlFile : public RtScriptXML
{
public:
    CBrushObjectXmlFile(CMainFrame* pFrame)
    {
        m_pFrame=pFrame;
        m_iNodeType = 0;
        m_nodes.push(NULL);
    }
    virtual ~CBrushObjectXmlFile()
    {
        if (m_nodes.size()!=1)
        {
           // ERR("XML ERROR: XML File Error.\n");
            CHECKEX("XML ERROR");
        }
    }
    virtual void OnError(const char* szFilename, const char* szErrMsg, long lLineNo)
    {
        //ERR3("XML ERROR: \"%s\", File '%s', line %d\n", szErrMsg, szFilename, lLineNo);
        CHECKEX("XML ERROR");
    }
    virtual void OnTagStart(const char *pTag)
    {   // <pTag
       // guard;
        if (stricmp(pTag, "node")==0)
        {
            m_iNodeType = 1;
        }else if (stricmp(pTag, "object")==0)
        {
            m_iNodeType = 2;
        }else if (stricmp(pTag, "light")==0)
        {
            m_iNodeType = 3;
        }else if (stricmp(pTag, "audio")==0)
        {
            m_iNodeType = 4;
        }else if (stricmp(pTag, "firefly")==0)
        {
            m_iNodeType = 5;
        }else if (stricmp(pTag, "scene_object_list")==0)
        {
            m_iNodeType = 0;
        }else
        {
            m_iNodeType = -1;
           // ERR1("XML ERROR: Unknown Tag \"%s\"\n", pTag);
            CHECKEX("XML ERROR");
        }
        m_strName = "";
        m_strValue = "";
       // unguard;
    }
    virtual void OnAttribute(const char *pAttribute, const char *pValue)
    {   // pAttribute=pValue
       // guard;
        if (stricmp(pAttribute, "name")==0)
        {
            m_strName = pValue;
        }else if (stricmp(pAttribute, "img")==0)
        {
            m_iImgIdx = atoi(pValue);
        }else if (stricmp(pAttribute, "type")==0)
        {
            m_lType = atol(pValue);
        }else if (stricmp(pAttribute, "file")==0)
        {
            m_strValue = pValue;
        }else
        {
            //ERR1("XML ERROR: Unknown Attribute \"%s\"\n", pAttribute);
            CHECKEX("XML ERROR");
        }
       // unguard;
    }
    virtual void OnCloseTagStart(const char *pTag)
    {   // </pTag
        //guard;
        if (stricmp(pTag, "node")==0)
        {
            m_nodes.pop();
        }
        //unguard;
    }
    virtual void OnTagEndImme()
    {
        OnTagEnd();
    }
    virtual void OnTagEnd()
    {   // >
        //guard;
        HTREEITEM   htItem;
        switch (m_iNodeType)
        {
        case 1: // node
            htItem = m_pFrame->m_wndBrushObjectView.InsertItem(m_strName, m_iImgIdx, m_iImgIdx, m_nodes.top());
            m_nodes.push(htItem);
            break;
        case 2: // object
        case 3: // light
        case 4: // audio
        case 5: // firefly
            htItem = m_nodes.top();
            htItem = m_pFrame->m_wndBrushObjectView.InsertItem(m_strName, m_iImgIdx, m_iImgIdx, htItem);
            m_pFrame->m_wndBrushObjectView.SetItemData(htItem, m_lType);
            m_pFrame->m_mapBrushObject[m_strName] = m_strValue;
            break;
        }
        m_iNodeType = 0;
        //unguard;
    }
private:
    CMainFrame* m_pFrame;
    int         m_iNodeType; // -1 none, 0 root, 1 node, 2 object, 3 light, 4 audio, 5 firefly

    int         m_iImgIdx;
    long        m_lType;
    RtString    m_strName;
    RtString    m_strValue;
    std::stack<HTREEITEM>   m_nodes;
};

CWnd* CMainFrame::CreateBrushObjectView()
{
    //guard;
	if (m_wndBrushObjectView.GetSafeHwnd() == 0)
	{
		if (!m_wndBrushObjectView.Create( WS_VISIBLE | TVS_HASLINES | TVS_LINESATROOT | TVS_HASBUTTONS | TVS_SHOWSELALWAYS,
			CRect(0,0,0,0), this, ID_TREE_OBJ_VIEW ))
		{
			TRACE0( "Unable to create tree control.\n" );
			return 0;
		}
		m_wndBrushObjectView.ModifyStyleEx(0, WS_EX_STATICEDGE);
		// load the tree images bitmap and add it to the image list.
		if (!m_ilBrushObjectView.Create(IDB_BITMAP_BRUSH_OBJECT, 16, 1, RGB(0x00,0x80,0x80)))
			return 0;

		// Set the image list for the tree control.
		m_wndBrushObjectView.SetImageList( &m_ilBrushObjectView, TVSIL_NORMAL );

        CBrushObjectXmlFile xml(this);
        xml.ParserFile("scene/tool_scene_objectlist.xml");
	}
	return &m_wndBrushObjectView;
    //guard_end_ex;

}

CWnd* CMainFrame::CreateAreaListView()
{
	if (m_wndAreaView.GetSafeHwnd() == 0)
	{
        if (!m_wndAreaView.Create(CRect(0,0,0,0), this, ID_AREA_LIST_VIEW))
		{
			return 0;
		}

        CEditorFrame* pEditor = (CEditorFrame*)g_pAppCase;
        m_wndAreaView.SetScene((RtSceneTool*)g_pScene);
        pEditor->m_pMainFrame = this;
        pEditor->UpdateAreaList();
    }
	return &m_wndAreaView;
}

//tim.yang  创建路点列表视图
CWnd* CMainFrame::CreatePathPointListView()
{
	if (m_wndPathPointView.GetSafeHwnd() == 0)
	{
		if (!m_wndPathPointView.Create(CRect(0,0,0,0), this, ID_PATHPOINT_LIST_VIEW))
		{
			return 0;
		}
		CEditorFrame* pEditor = (CEditorFrame*)g_pAppCase;
		m_wndPathPointView.SetScene((RtSceneTool*)g_pScene);
	}
    return &m_wndPathPointView;
}
CWnd* CMainFrame::CreateBrushVertexView()
{
    if (m_wndBrushVertexView.GetSafeHwnd() == 0)
    {
        if (!m_wndBrushVertexView.Create(CBrushVertexForm::IDD, this))
        {
            return 0;
        }
    }
    return &m_wndBrushVertexView;
}

CWnd* CMainFrame::CreateBrushGrassView()
{
    if (m_wndBrushGrassView.GetSafeHwnd() == 0)
    {
        if (!m_wndBrushGrassView.Create(CBrushGrassForm::IDD, this))
        {
            return 0;
        }
    }
    return &m_wndBrushGrassView;
}

CWnd* CMainFrame::CreateBrushColorView()
{
	if (m_wndBrushColorView.GetSafeHwnd() == 0)
	{
        if (!m_wndBrushColorView.Create(CBrushColorForm::IDD, this))
		{
			return 0;
		}
    }
	return &m_wndBrushColorView;
}

BOOL s_bPostProcessEffect = FALSE;
double s_dPostProcessEffectP1 = 1.0;
double s_dPostProcessEffectP2 = 0.5;
BOOL s_bPostProcessObjectHeightLight = FALSE;
DWORD s_dwPostProcessObjectHeightLightColor = FALSE;
double s_dPostProcessObjectHeightLightScale = 1.f;

CWnd* CMainFrame::CreateBrushPropertyView()
{
    //guard;
	if (m_wndBrushPropertyGrid.GetSafeHwnd() == 0)
	{
		m_wndBrushPropertyGrid.Create( CRect(0, 0, 0, 0), this, 0 );
        m_wndBrushPropertyGrid.SetTheme(xtpGridThemeCool);
		//m_wndBrushPropertyGrid.ShowToolBar(TRUE);
		m_wndBrushPropertyGrid.SetOwner(this);

		CFont font;
		font.CreatePointFont(60, _T("Marlett"));

        LOGFONT lf;
		font.GetLogFont(&lf);

        CXTPPropertyGridItem *pRoot;
        CXTPPropertyGridItem *pItem, *pSelect=NULL;
        CXTPPropertyGridItemConstraints *pList;

        CEditorFrame* pEditor = (CEditorFrame*)g_pAppCase;
        pEditor->m_pMainFrame = this;

        // 环境信息.
        {
            pRoot = m_wndBrushPropertyGrid.AddCategory(_T("环境信息"));
            pRoot->SetDescription(_T("这里是各种环境信息"));

            pItem = pRoot->AddChildItem(new CXTPPropertyGridItemColor(ID_GRIDITEM_CLEAR_COLOR, pEditor->m_dwClearColor, &pEditor->m_dwClearColor));
            //pItem = pRoot->AddChildItem(new CXTPPropertyGridItemColor(ID_GRIDITEM_ENV_SUN_SPECULAR, pEditor->m_dwSunSpecular, &pEditor->m_dwSunSpecular));
            pItem = pRoot->AddChildItem(new CGridItemFloat(ID_GRIDITEM_ENV_SUN_INCLINATION, &pEditor->m_fSunInclination));
            pItem = pRoot->AddChildItem(new CGridItemFloat(ID_GRIDITEM_ENV_SUN_ROTATION, &pEditor->m_fSunRotation));
            //pItem = pRoot->AddChildItem(new CXTPPropertyGridItemColor(ID_GRIDITEM_ENV_TERRAIN_AMBIENT, pEditor->m_dwTerrainAmbient, &pEditor->m_dwTerrainAmbient));

            pRoot->Expand();
        }

        // 场景块信息
        {
            pRoot = m_wndBrushPropertyGrid.AddCategory("场景块信息");
            pItem = pRoot->AddChildItem(new CGridItemSizeString(ID_GRIDITEM_BLOCK_FILENAME, pEditor->m_szFileName, 20)); pItem->SetReadOnly(TRUE);
            pItem = pRoot->AddChildItem(new CGridItemSizeString(ID_GRIDITEM_BLOCK_NAME, pEditor->m_szBlockName, 20));
            pItem = pRoot->AddChildItem(new CGridItemColor(ID_GRIDITEM_ENVIRONMENT, &(pEditor->m_dwEnvironmentLighting))); pSelect = pItem;
            pItem = pRoot->AddChildItem(new CGridItemColor(ID_GRIDITEM_ENV_SUN_COLOR, &(pEditor->m_dwSunLighting)));
            pItem = pRoot->AddChildItem(new CXTPPropertyGridItemBool(ID_GRIDITEM_FOG_ENABLE, pEditor->m_bFog, &pEditor->m_bFog));
            pItem = pRoot->AddChildItem(new CGridItemColor(ID_GRIDITEM_FOG_COLOR, &pEditor->m_dwFogColor));
            pItem = pRoot->AddChildItem(new CGridItemFloat(ID_GRIDITEM_FOG_MIN, &(pEditor->m_fFogNear)));
            pItem = pRoot->AddChildItem(new CGridItemFloat(ID_GRIDITEM_FOG_MAX, &(pEditor->m_fFogFar)));
            pItem = pRoot->AddChildItem(new CGridItemSizeString(ID_GRIDITEM_BACKMUSIC, pEditor->m_szBackMusic, 20));
            pItem = pRoot->AddChildItem(new CGridItemFloat(ID_GRIDITEM_BACKMUSIC_START_TIME, &(pEditor->m_fBackMusicStartTime)));
            pItem = pRoot->AddChildItem(new CGridItemFloat(ID_GRIDITEM_BACKMUSIC_DIFF_TIME, &(pEditor->m_fBackMusicDifferenceTime)));
            pItem = pRoot->AddChildItem(new CGridItemSizeString(ID_GRIDITEM_BLOCK_FLAG_TEXTURE, pEditor->m_szBlockFlagTexture, 20));

         //   pItem = pRoot->AddChildItem(new CXTPPropertyGridItemBool(ID_GRIDITEM_OPTION_POSTPROCESS, pEditor->m_bPostProcessEffect, &(pEditor->m_bPostProcessEffect)));
          //  pItem = pRoot->AddChildItem(new CXTPPropertyGridItemBool(ID_GRIDITEM_OPTION_OBJHEIGHTLIGHT, pEditor->m_bPostProcessObjectHL, &(pEditor->m_bPostProcessObjectHL)));
          //  pItem = pRoot->AddChildItem(new CGridItemFloat(ID_GRIDITEM_OPTION_POSTPROCESS_CONTRAST, &(pEditor->m_fPostProcessEffectContrast)));
            pItem = pRoot->AddChildItem(new CGridItemFloat(ID_GRIDITEM_OPTION_POSTPROCESS_INTENSITY, &(pEditor->m_fPostProcessEffectIntensity)));
          //  pItem = pRoot->AddChildItem(new CGridItemFloat(ID_GRIDITEM_OPTION_OBJHEIGHTLIGHT_SCALE, &(pEditor->m_fPostProcessObjectHLScale)));
          //  pItem = pRoot->AddChildItem(new CGridItemColor(ID_GRIDITEM_OPTION_OBJHEIGHTLIGHT_COLOR_EMI, &(pEditor->m_dwPostProcessObjectHLColorEmi)));
          //  pItem = pRoot->AddChildItem(new CGridItemColor(ID_GRIDITEM_OPTION_OBJHEIGHTLIGHT_COLOR_DIF, &(pEditor->m_dwPostProcessObjectHLColorDif)));
          //  pItem = pRoot->AddChildItem(new CGridItemColor(ID_GRIDITEM_OPTION_OBJHEIGHTLIGHT_COLOR_SPE, &(pEditor->m_dwPostProcessObjectHLColorSpe)));
          //  pItem = pRoot->AddChildItem(new CGridItemFloat(ID_GRIDITEM_OPTION_OBJHEIGHTLIGHT_SPE_POWER, &(pEditor->m_fPostProcessObjectHLColorSpePower)));
            pRoot->Expand();
        }

		// 图形质量.
        {
		    pRoot = m_wndBrushPropertyGrid.AddCategory(_T("图形质量"));
            pRoot->SetDescription(_T("这里可以设置图像的质量选项，包括贴图质量等等."));

            pItem = pRoot->AddChildItem(new CXTPPropertyGridItem(_T("贴图质量"), _T("高")));
            pList = pItem->GetConstraints();
            pList->AddConstraint(_T("高"));
            pList->AddConstraint(_T("中"));
            pList->AddConstraint(_T("低"));
            pItem->SetFlags(xtpGridItemHasComboButton | xtpGridItemHasEdit);

            pItem = pRoot->AddChildItem(new CXTPPropertyGridItem(_T("几何质量"), _T("高")));
            pList = pItem->GetConstraints();
            pList->AddConstraint(_T("高"));
            pList->AddConstraint(_T("中"));
            pList->AddConstraint(_T("低"));
            pItem->SetFlags(xtpGridItemHasComboButton | xtpGridItemHasEdit);

            pItem = pRoot->AddChildItem(new CXTPPropertyGridItemBool(ID_GRIDITEM_OPTION_WATERCATOPTRICSKY, g_pScene->m_bOptionWaterCatoptricSky, &(g_pScene->m_bOptionWaterCatoptricSky)));
            pRoot->Expand();
        }

        // 灯光设置.
        {
            pRoot = m_wndBrushPropertyGrid.AddCategory(_T("灯光设置"));
            pRoot->SetDescription(_T("这里可以设置当前灯光的属性, 只有选中灯光的时候有效."));

            pItem = pRoot->AddChildItem(new CXTPPropertyGridItemColor(ID_GRIDITEM_LIGHTCOLOR, pEditor->m_dwCurLightColor, &pEditor->m_dwCurLightColor));
            pItem = pRoot->AddChildItem(new CXTPPropertyGridItemDouble(ID_GRIDITEM_LIGHT_RANGE, pEditor->m_fCurLightRange, _T("%0.2f"), &pEditor->m_fCurLightRange));
            pItem = pRoot->AddChildItem(new CXTPPropertyGridItemDouble(ID_GRIDITEM_LIGHT_ATT_C, pEditor->m_fCurLightConstantAttenuation, _T("%0.4f"), &pEditor->m_fCurLightConstantAttenuation));
            pItem = pRoot->AddChildItem(new CXTPPropertyGridItemDouble(ID_GRIDITEM_LIGHT_ATT_L, pEditor->m_fCurLightLinearAttenuation, _T("%0.4f"), &pEditor->m_fCurLightLinearAttenuation));
            pItem = pRoot->AddChildItem(new CXTPPropertyGridItemDouble(ID_GRIDITEM_LIGHT_ATT_Q, pEditor->m_fCurLightQuadraticAttenuation, _T("%0.4f"), &pEditor->m_fCurLightQuadraticAttenuation));
            pRoot->Expand();
        }
        // pRoot->Expand();
        if (pSelect) pSelect->Select();

        m_wndBrushPropertyGrid.UpdateData(FALSE);
	}

	return &m_wndBrushPropertyGrid;
    //guard_end_ex;
}

int CMainFrame::OnCreateCommandBar(LPCREATEBARSTRUCT lpCreatePopup)
{
    /*
	if (lpCreatePopup->bTearOffBar && lpCreatePopup->nID == 1005)
	{
		CXTPToolBar* pToolBar = DYNAMIC_DOWNCAST(CXTPToolBar, lpCreatePopup->pCommandBar);
		ASSERT(pToolBar);
		pToolBar->EnableDocking(xtpFlagFloating);
		pToolBar->EnableCustomization(FALSE);
		return TRUE;		
	}
    */
	if (lpCreatePopup->bPopup && _tcscmp(lpCreatePopup->lpcstrCaption, _T("界面样式")) == 0)
	{
		CXTPPopupBar* pPopupBar = CXTPPopupBar::CreatePopupBar(GetCommandBars());
		pPopupBar->SetTearOffPopup(_T("界面样式"), 1001);
		lpCreatePopup->pCommandBar = pPopupBar;
		return TRUE;
	}
	if (lpCreatePopup->bPopup && _tcscmp(lpCreatePopup->lpcstrCaption, _T("刷子")) == 0)
	{
		CXTPPopupBar* pPopupBar = CXTPPopupBar::CreatePopupBar(GetCommandBars());
		pPopupBar->SetTearOffPopup(_T("刷子"), 1002);
		lpCreatePopup->pCommandBar = pPopupBar;
		return TRUE;
	}

    if (lpCreatePopup->bPopup && lpCreatePopup->nID == IDR_TOOLBAR_SHOW)
	{
		CXTPPopupToolBar* pPopupBar = CXTPPopupToolBar::CreatePopupToolBar(GetCommandBars());
		pPopupBar->LoadToolBar(IDR_TOOLBAR_SHOW);
		pPopupBar->SetWidth(23 * 5);
		pPopupBar->SetTearOffPopup(_T("Borders"), IDR_TOOLBAR_SHOW, 23 * 5);
		
		lpCreatePopup->pCommandBar = pPopupBar;
		return TRUE;
	}

	return FALSE;	
}

int CMainFrame::OnCreateControl(LPCREATECONTROLSTRUCT lpCreateControl)
{
    if (lpCreateControl->bToolBar)
    {
        CXTPToolBar* pToolBar = DYNAMIC_DOWNCAST(CXTPToolBar, lpCreateControl->pCommandBar);
        if (!pToolBar) return FALSE;
        if (lpCreateControl->nID == IDR_TOOLBAR_SHOW && pToolBar->GetBarID() == IDR_MAINFRAME)
        {
            lpCreateControl->controlType = xtpControlSplitButtonPopup;
            return TRUE;
        }
    }
    return FALSE;
}

void CMainFrame::OnThemesOffice2000() 
{
    m_paneManager.SetTheme(xtpPaneThemeGrippered);
	CXTPPaintManager::SetTheme(xtpThemeOffice2000);
	GetCommandBars()->RedrawCommandBars();
}

void CMainFrame::OnUpdateThemesOffice2000(CCmdUI* pCmdUI) 
{
	pCmdUI->SetCheck(m_paneManager.GetCurrentTheme() == xtpPaneThemeGrippered);
}

void CMainFrame::OnThemesOfficexp() 
{
    m_paneManager.SetTheme(xtpPaneThemeOffice);
	CXTPPaintManager::SetTheme(xtpThemeOfficeXP);
	GetCommandBars()->RedrawCommandBars();
}

void CMainFrame::OnUpdateThemesOfficexp(CCmdUI* pCmdUI) 
{
	pCmdUI->SetCheck(m_paneManager.GetCurrentTheme() == xtpPaneThemeOffice);
}

void CMainFrame::OnThemesOfficeVisio() 
{
    m_paneManager.SetTheme(xtpPaneThemeVisio);
	CXTPPaintManager::SetTheme(xtpThemeOfficeXP);
	GetCommandBars()->RedrawCommandBars();
}

void CMainFrame::OnUpdateThemesOfficeVisio(CCmdUI* pCmdUI) 
{
	pCmdUI->SetCheck(m_paneManager.GetCurrentTheme() == xtpPaneThemeVisio);
}

void CMainFrame::OnThemesOffice2003() 
{
    m_paneManager.SetTheme(xtpPaneThemeOffice2003);
	CXTPPaintManager::SetTheme(xtpThemeOffice2003);
	GetCommandBars()->RedrawCommandBars();
}

void CMainFrame::OnUpdateThemesOffice2003(CCmdUI* pCmdUI) 
{
	pCmdUI->SetCheck(m_paneManager.GetCurrentTheme() == xtpPaneThemeOffice2003);	
}

void CMainFrame::OnThemesNativexptheme() 
{
    m_paneManager.SetTheme(xtpPaneThemeNativeWinXP);
	CXTPPaintManager::SetTheme(xtpThemeNativeWinXP);
	GetCommandBars()->RedrawCommandBars();
}

void CMainFrame::OnUpdateThemesNativexptheme(CCmdUI* pCmdUI) 
{
	pCmdUI->SetCheck(m_paneManager.GetCurrentTheme() == xtpPaneThemeNativeWinXP);
}

void CMainFrame::OnBuildSM()
{
    g_pScene->BuildShadowMap();
}

void CMainFrame::OnBuildSM2()
{
    g_pScene->BuildShadowMap_ForCurrBlock();
}

void CMainFrame::OnShowNormal()
{
    RtgGetRenderOption()->bTerrainNormal = !RtgGetRenderOption()->bTerrainNormal;    
}

void CMainFrame::OnEnablePE()
{
    RtGetRender()->SetPostProcessEffectOnoff(!RtGetRender()->IsUsePostProcessEffectOn());
}

void CMainFrame::OnSaveCurrBlock()
{
    RtSceneTool* pScene = (RtSceneTool*)g_pScene;
    pScene->ToolSave_CurrBlock();
}

void CMainFrame::OnShowMask(UINT nID)
{
#if RTS_DRAW_HELPER
    CEditorFrame* pEditor = (CEditorFrame*)g_pAppCase;
    RtSceneTool* pScene = (RtSceneTool*)g_pScene;
    switch (nID)
    {
    case ID_BTN_SHOW_LINE:      pScene->m_bDrawWireframe=!pScene->m_bDrawWireframe; break;
    case ID_BTN_SHOW_NORMAL:    pScene->m_bDrawNormal = !pScene->m_bDrawNormal;     break;
    case ID_BTN_SHOW_MASK_WALK: pScene->m_bDrawAttrWalk = !pScene->m_bDrawAttrWalk; break;
    case ID_BTN_SHOW_MASK_FLY:  pScene->m_bDrawAttrFly = !pScene->m_bDrawAttrFly;   break;
    case ID_BTN_SHOW_MASK_DIG:  pScene->m_bDrawAttrDig = !pScene->m_bDrawAttrDig;   break;
    case ID_BTN_SHOW_HELPER:    pScene->m_bDrawHelper = !pScene->m_bDrawHelper;     break;
    case ID_BTN_SHOW_MINIMAP:   pScene->m_bDrawMinimap = !pScene->m_bDrawMinimap;   break;
    case ID_BTN_SHOW_FOG:
        {
            pEditor->m_bFog = !pEditor->m_bFog;
            RtGetRender()->SetFogEnable(pEditor->m_bFog);
            CXTPPropertyGridItemBool* pItem = (CXTPPropertyGridItemBool*)m_wndBrushPropertyGrid.FindItem(ID_GRIDITEM_FOG_ENABLE);
            if (pItem)
            {
                pItem->SetBool(pEditor->m_bFog);
            }
        }
        break;
    }
#endif
}

void CMainFrame::OnUpdateShowMask(CCmdUI* pCmdUI)
{
#if RTS_DRAW_HELPER
    CEditorFrame* pEditor = (CEditorFrame*)g_pAppCase;
    RtSceneTool* pScene = (RtSceneTool*)g_pScene;
    switch (pCmdUI->m_nID)
    {
    case ID_BTN_SHOW_LINE:      pCmdUI->SetCheck(pScene->m_bDrawWireframe ? TRUE: FALSE); break;
    case ID_BTN_SHOW_NORMAL:    pCmdUI->SetCheck(pScene->m_bDrawNormal    ? TRUE: FALSE); break;
    case ID_BTN_SHOW_MASK_WALK: pCmdUI->SetCheck(pScene->m_bDrawAttrWalk  ? TRUE: FALSE); break;
    case ID_BTN_SHOW_MASK_FLY:  pCmdUI->SetCheck(pScene->m_bDrawAttrFly   ? TRUE: FALSE); break;
    case ID_BTN_SHOW_MASK_DIG:  pCmdUI->SetCheck(pScene->m_bDrawAttrDig   ? TRUE: FALSE); break;
    case ID_BTN_SHOW_HELPER:    pCmdUI->SetCheck(pScene->m_bDrawHelper    ? TRUE: FALSE); break;
    case ID_BTN_SHOW_FOG:       pCmdUI->SetCheck(pEditor->m_bFog          ? TRUE: FALSE); break;
    case ID_BTN_SHOW_MINIMAP:   pCmdUI->SetCheck(pScene->m_bDrawMinimap   ? TRUE: FALSE); break;
    }
#endif
}

LRESULT CMainFrame::OnGridNotify(WPARAM wParam, LPARAM lParam)
{
    //guard;
    RtgVertex3 v3;
    RtgMatrix16 m16;
    CEditorFrame* pEditor = (CEditorFrame*)g_pAppCase;
    if (wParam == XTP_PGN_INPLACEBUTTONDOWN)
    {
    }

    if (wParam == XTP_PGN_ITEMVALUE_CHANGED)
    {
        CEditorFrame* pEditor = (CEditorFrame*)g_pAppCase;
        CXTPPropertyGridItem* pItem = (CXTPPropertyGridItem*)lParam;
        switch (pItem->GetID())
        {
        case ID_GRIDITEM_CLEAR_COLOR:
            RtGetRender()->m_dwClearColor = COLOR_EXCHANGE(pEditor->m_dwClearColor);
            return TRUE;
        case ID_GRIDITEM_ENV_SUN_SPECULAR:
            ((RtSceneTool*)g_pScene)->m_lightDirect.vSpecular.x = GetRValue(pEditor->m_dwSunSpecular)/255.f;
            ((RtSceneTool*)g_pScene)->m_lightDirect.vSpecular.y = GetGValue(pEditor->m_dwSunSpecular)/255.f;
            ((RtSceneTool*)g_pScene)->m_lightDirect.vSpecular.z = GetBValue(pEditor->m_dwSunSpecular)/255.f;
            return TRUE;
        case ID_GRIDITEM_ENV_SUN_INCLINATION:
        case ID_GRIDITEM_ENV_SUN_ROTATION:
            {
                int _blockx = 0;
                int _blocky = 0;

                g_pScene->GetBlockMapbyPos(
                    RtgVertex2(RtGetRender()->m_pCamera->m_vEyePt.x, RtGetRender()->m_pCamera->m_vEyePt.y), _blockx, _blocky);
                RtsSceneBlockMap* _blockMap = g_pScene->GetBlockMapbyIndex(_blockx,_blocky);

                if (!_blockMap)
                    break;

                _blockMap->SetSunDir(pEditor->m_fSunInclination, pEditor->m_fSunRotation);
                g_pScene->RequestEnvUpdate(RtGetRender()->m_pCamera->GetEyePt());
            }
            return TRUE;
        case ID_GRIDITEM_ENV_TERRAIN_AMBIENT:
            ((RtSceneTool*)g_pScene)->m_lightTerrainAmbient.x = GetRValue(pEditor->m_dwTerrainAmbient)/255.f;
            ((RtSceneTool*)g_pScene)->m_lightTerrainAmbient.y = GetGValue(pEditor->m_dwTerrainAmbient)/255.f;
            ((RtSceneTool*)g_pScene)->m_lightTerrainAmbient.z = GetBValue(pEditor->m_dwTerrainAmbient)/255.f;
            return TRUE;
        case ID_GRIDITEM_FOG_ENABLE:
            RtGetRender()->SetFogEnable(pEditor->m_bFog);
            break;
        case ID_GRIDITEM_LIGHTCOLOR:
        case ID_GRIDITEM_LIGHT_RANGE:
        case ID_GRIDITEM_LIGHT_ATT_C:
        case ID_GRIDITEM_LIGHT_ATT_L:
        case ID_GRIDITEM_LIGHT_ATT_Q:
            if (pEditor->m_BrushTool.GetBrushType()==CBrushTool::BT_OBJECT)
            {
                RtsObject* pObj;
                pObj = ((CBrusherObject*)pEditor->m_BrushTool.m_pBrusher)->GetBrushObject();
                if (pObj->IsKindOf(RT_RUNTIME_CLASS(RtsLight)))
                {
                    switch (pItem->GetID())
                    {
                    case ID_GRIDITEM_LIGHTCOLOR:
                        v3.x = GetRValue(pEditor->m_dwCurLightColor)/255.f;
                        v3.y = GetGValue(pEditor->m_dwCurLightColor)/255.f;
                        v3.z = GetBValue(pEditor->m_dwCurLightColor)/255.f;
                        ((RtsLight*)pObj)->vDiffuse = v3;
                        ((RtsLight*)pObj)->vAmbient = v3;
                        ((RtsLight*)pObj)->vSpecular = v3;
                        break;
                    case ID_GRIDITEM_LIGHT_RANGE:
                        ((RtsLight*)pObj)->fRange = pEditor->m_fCurLightRange;
                        break;
                    case ID_GRIDITEM_LIGHT_ATT_C:
                        ((RtsLight*)pObj)->fConstantAttenuation = pEditor->m_fCurLightConstantAttenuation;
                        break;
                    case ID_GRIDITEM_LIGHT_ATT_L:
                        ((RtsLight*)pObj)->fLinearAttenuation = pEditor->m_fCurLightLinearAttenuation;
                        break;
                    case ID_GRIDITEM_LIGHT_ATT_Q:
                        ((RtsLight*)pObj)->fQuadraticAttenuation = pEditor->m_fCurLightQuadraticAttenuation;
                        break;
                    }
                }
            }
            return TRUE;
        case ID_GRIDITEM_OPTION_WATERCATOPTRICSKY:
            g_pScene->OptionSetWaterCatoptricSky(((RtSceneTool*)g_pScene)->m_bOptionWaterCatoptricSky);
            return TRUE;

        case ID_GRIDITEM_BLOCK_FILENAME:
        case ID_GRIDITEM_BLOCK_NAME:
        case ID_GRIDITEM_ENVIRONMENT:
        case ID_GRIDITEM_ENV_SUN_COLOR:
        case ID_GRIDITEM_FOG_COLOR:
        case ID_GRIDITEM_FOG_MIN:
        case ID_GRIDITEM_FOG_MAX:
        case ID_GRIDITEM_BACKMUSIC:
        case ID_GRIDITEM_BACKMUSIC_START_TIME:
        case ID_GRIDITEM_BACKMUSIC_DIFF_TIME:
        case ID_GRIDITEM_BLOCK_FLAG_TEXTURE:
        case ID_GRIDITEM_OPTION_POSTPROCESS:
        case ID_GRIDITEM_OPTION_OBJHEIGHTLIGHT:
        case ID_GRIDITEM_OPTION_POSTPROCESS_CONTRAST:
        case ID_GRIDITEM_OPTION_POSTPROCESS_INTENSITY:
        case ID_GRIDITEM_OPTION_OBJHEIGHTLIGHT_SCALE:
        case ID_GRIDITEM_OPTION_OBJHEIGHTLIGHT_COLOR_EMI:
        case ID_GRIDITEM_OPTION_OBJHEIGHTLIGHT_COLOR_DIF:
        case ID_GRIDITEM_OPTION_OBJHEIGHTLIGHT_COLOR_SPE:
        case ID_GRIDITEM_OPTION_OBJHEIGHTLIGHT_SPE_POWER:
            ((RtSceneTool*)g_pScene)->GridItemChanged();
            return TRUE;
        default:
            // MessageBox("没有实现");
            break;
        }
    }
    return 0;
    //unguard;
}

void CMainFrame::OnLockGameView()
{
    funguard;
    CEditorFrame* pEditor = (CEditorFrame*)g_pAppCase;
    RtGetRender()->m_pCamera->SetPosition(pEditor->m_vCharPos);
    pEditor->SetDefaultCamera();
    fununguard;
}

void CMainFrame::OnHeightmapView()
{
    //guard;
    /*
    CEditorFrame* pEditor = (CEditorFrame*)g_pAppCase;
    CDlgHeightMap dlg;
    RtgImage img;
    img.Load("scene/heightmap.bmp");
    img.Resize(((RtSceneTool*)g_pScene)->m_iTileCntX+1, ((RtSceneTool*)g_pScene)->m_iTileCntY+1);
    BYTE* pData = (BYTE*)img.GetRawData();
    RtScene::SVertexValue* pPos = ((RtSceneTool*)g_pScene)->m_pPositions;

    int i, iSize = (((RtSceneTool*)g_pScene)->m_iTileCntX+1)*(((RtSceneTool*)g_pScene)->m_iTileCntY+1);
    for (i=0; i<iSize; i++)
    {
        *(pData++) = pPos->cHeightMap;
        *(pData++) = pPos->cHeightMap;
        *(pData++) = pPos->cHeightMap;
        *(pData++) = pPos->cHeightMap;
        pPos ++;
    }

    HDC hDC = ::GetDC(dlg.m_hWnd);
    HBITMAP hBITMAP = (HBITMAP)img.CreateBitmap(hDC);
    //dlg.m_imgHeightmap.SetBitmap(hBITMAP);

    if (this->GetActiveView()->IsKindOf(RUNTIME_CLASS(CSceneEditorView)))
    {
        ((CSceneEditorView*)GetActiveView())->Pause();
        dlg.DoModal();
        ((CSceneEditorView*)GetActiveView())->Run();
    }
    */
    //unguard;
}

void CMainFrame::OnToolSnapShotView()
{
    //guard;
    static char BASED_CODE szFilter[] = "JPG Image Files (*.jpg)|*.jpg|BMP Image Files (*.bmp)|*.bmp|All Files (*.*)|*.*||";
    CFileDialog fileDlg(FALSE, "jpg", NULL, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, szFilter, this);

    BOOL bOpen = fileDlg.DoModal()==IDOK;
    RtCoreFile().SetDefaultDirectory(theApp.WorkingDirectory());
    if (bOpen)
    {
        CString pathName = fileDlg.GetPathName();
        CString fileName = fileDlg.GetFileTitle();
        RtGetRender()->SaveRenderTargetToFile(pathName);
    }
    //unguard;
}

void CMainFrame::OnToolMoveObject()
{
    //guard;
    CEditorFrame* pEditor = (CEditorFrame*)g_pAppCase;
    pEditor->m_BrushTool.SetBrushType(CBrushTool::BT_OBJECT_MOVE);
    //unguard;
}

void CMainFrame::OnToolAdjustObject()
{
    //guard;
    CEditorFrame* pEditor = (CEditorFrame*)g_pAppCase;
    pEditor->m_BrushTool.SetBrushType(CBrushTool::BT_OBJECT_ADJUST);
    //unguard;
}

void CMainFrame::OnToolOriginallyObjectSize()
{
    //guard;
    CEditorFrame* pEditor = (CEditorFrame*)g_pAppCase;
    pEditor->m_BrushTool.OriginallyObjectSize();
    //unguard;
}

void CMainFrame::OnTreeCtrlSelectChanged(NMHDR * pNotifyStruct, LRESULT * result)
{
    //guard;
	funguard;
    HTREEITEM hItem = m_wndBrushObjectView.GetSelectedItem();
    if (m_wndBrushObjectView.ItemHasChildren(hItem))
        return; // 如果不是子节点就直接退出
    RtString szItem = m_wndBrushObjectView.GetItemText(hItem);
    CEditorFrame* pEditor = (CEditorFrame*)g_pAppCase;
    const char* pFile = m_mapBrushObject[szItem];
    long lType = (long)m_wndBrushObjectView.GetItemData(hItem);
    long lResult = 0;
    if (szItem.GetLength()>0)
    {
        pEditor->m_BrushTool.SetBrushType(CBrushTool::BT_OBJECT);
        pEditor->m_BrushTool.SetBrushData((long)(pFile), lType, (long)(&lResult));
//-------------------------jiang modify------------------------------//
        HTREEITEM hParentItem  = m_wndBrushObjectView.GetParentItem(hItem);
		RtString szItem2 = m_wndBrushObjectView.GetItemText(hParentItem);
		if(szItem2 == "NPC")
		{
          pEditor->m_bAddNpc = true;
		}
		else
		{
          pEditor->m_bAddNpc = false;
		}
//----------------------------------------------------------------------//
        if (lResult==0)
        {
            MessageBox("这个物件还没有实现");
        }
    }
	fununguard;
    //unguard;
}

void CMainFrame::OnToolRecomputeObjectLight()
{
    g_pScene->RecomputeAllLight();
}

void CMainFrame::OnToolRecomputeObjectBelong()
{
    g_pScene->RecomputeObjectBelong();
}

void CMainFrame::OnToolCheckSameObject()
{
    int iCnt = g_pScene->CheckSameObject();
    sprintf(RtCoreBuffer2048(), "相同物件有: %d", iCnt);
    MessageBox(RtCoreBuffer2048(), "信息");
}

void CMainFrame::OnViewCenter()
{
	g_pScene->ViewCenter();
}

void CMainFrame::OnToolDeleteSameObject()
{
    int iCnt = g_pScene->DeleteSameObject();
    sprintf(RtCoreBuffer2048(), "删除的物件有: %d", iCnt);
    MessageBox(RtCoreBuffer2048(), "信息");
}

void CMainFrame::OnToolTgaReverse()
{
    //guard;
    CDlgHeightMap dlg;
    RtgImage img;

    /*
    static char BASED_CODE szFilter[] = "TGA Image Files (*.tga)|*.tga|All Files (*.*)|*.*||";
    CFileDialog fileDlg(TRUE, "tga", ".tga", OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, szFilter, this);
    BOOL bOpen = fileDlg.DoModal()==IDOK;
    int i, iSize;
    g_pFileManager->SetDefaultDirectory(theApp.WorkingDirectory());
    if (bOpen)
    {
        CString pathName = fileDlg.GetPathName();
        CString fileName = fileDlg.GetFileTitle();
        if (img.Load(pathName))
        {
            BYTE* pData = (BYTE*)img.GetRawData();
            iSize = img.info.iHeight*img.info.iWidth;
            for (i=0; i<iSize; i++)
            {
                if (pData[3])
                {
                    pData[0] = min(pData[0]*255/pData[3], 255);
                    pData[1] = min(pData[1]*255/pData[3], 255);
                    pData[2] = min(pData[2]*255/pData[3], 255);
                }
                pData += 4;
            }
            pathName = pathName.Left(pathName.GetLength()-3);
            pathName = pathName + "bmp";
            //img.Save(pathName);
        }
    }
    */

    static char BASED_CODE szFilter[] = "BMP Image Files (*.bmp)|*.bmp|All Files (*.*)|*.*||";
    CFileDialog fileDlg(TRUE, "bmp", ".bmp", OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, szFilter, this);

    BOOL bOpen = fileDlg.DoModal()==IDOK;
    RtCoreFile().SetDefaultDirectory(theApp.WorkingDirectory());
    if (bOpen)
    {
        CString pathName = fileDlg.GetPathName();
        CString fileName = fileDlg.GetFileTitle();
        if (img.Load(pathName))
        {
            //img.Resize(img.info.iWidth/2, img.info.iHeight/2);
            pathName = pathName.Left(pathName.GetLength()-4);
            pathName = pathName + ".jpg";
            img.SaveToJPG(pathName);
        }
    }
    //unguard;
}

void CMainFrame::OnToolLoadEnv()
{
    static char BASED_CODE szFilter[] = "Environment Information Files (*.env)|*.env|All Files (*.*)|*.*||";
    CFileDialog fileDlg(TRUE, "env", ".env", OFN_OVERWRITEPROMPT, szFilter, this);

    BOOL bOpen = fileDlg.DoModal()==IDOK;
    RtCoreFile().SetDefaultDirectory(theApp.WorkingDirectory());
    if (!bOpen) return;
    CEditorFrame* pEditor = (CEditorFrame*)g_pAppCase;
    ((RtSceneTool*)g_pScene)->LoadEnvironmentInfo(fileDlg.GetPathName());
}

void CMainFrame::OnToolSaveEnv()
{
    static char BASED_CODE szFilter[] = "Environment Information Files (*.env)|*.env|All Files (*.*)|*.*||";
    CFileDialog fileDlg(FALSE, "env", ".env", OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, szFilter, this);

    BOOL bOpen = fileDlg.DoModal()==IDOK;
    RtCoreFile().SetDefaultDirectory(theApp.WorkingDirectory());
    if (!bOpen) return;
    CEditorFrame* pEditor = (CEditorFrame*)g_pAppCase;
    ((RtSceneTool*)g_pScene)->SaveEnvironmentInfo(fileDlg.GetPathName());
}

void CMainFrame::OnToolAddAreaFromFile()
{
    CEditorFrame* pEditor = (CEditorFrame*)g_pAppCase;
    ((RtSceneTool*)g_pScene)->m_bRender = FALSE;
    static char BASED_CODE szFilter[] = "区域信息文件 (*.rsr)|*.rsr|所有文件 (*.*)|*.*||";
    CFileDialog fileDlg(TRUE, "rsr", ".rsr", OFN_OVERWRITEPROMPT, szFilter, this);

    BOOL bOpen = fileDlg.DoModal()==IDOK;
    RtCoreFile().SetDefaultDirectory(theApp.WorkingDirectory());
    if (bOpen)
    {
        g_pScene->AddAreaInfo(fileDlg.GetPathName());
    }
    ((RtSceneTool*)g_pScene)->m_bRender = TRUE;
}

void CMainFrame::OnNewBlock()
{
    //guard;
    CDlgNewBlock dlg(this);
    if (dlg.DoModal()==IDOK)
    {
        // MessageBox("创建");
    }
    //unguard;
}

//jiang modify
//输出地图上NPC信息
void CMainFrame::OnToolOutPutNpcInfo()
{
    /*
	RtArchive* pFile2 =  RtCoreFile().CreateFileReader("C:\\2.csv",RtFileManager::FW_Text);
	char k[256];
    pFile2->Serialize(k,256);
	for(int i = 0; i < 256;i++)
	{
		char a;
		a = k[i];

	}
	*/
   CEditorFrame* pEditor = (CEditorFrame*)g_pAppCase;
   char  szFilter[] = "NPC Files (*.csv)|*.csv";
   CFileDialog fileDlg(FALSE, "NPC", "", OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, szFilter, this);

   BOOL bOpen = fileDlg.DoModal()==IDOK;
   RtCoreFile().SetDefaultDirectory(theApp.WorkingDirectory());

   if (bOpen)
   {
      RtArchive* pFile = RtCoreFile().CreateFileWriter(NULL,fileDlg.GetPathName(),RtFileManager::FW_Text);
	  if (!pFile)
	  {
         MessageBox("输出NPC信息失败");
		 return;
	  }
	  CEditorFrame* pEditor = (CEditorFrame*)g_pAppCase;
	  std::list<BrushNpcInfo>::iterator iterobj;
      RtsObject* pEveryObj;
	  RtString szBody;
	  char szNum[20];
	  
	  float mx,my,mz;
      if (pEditor->m_NpcBrushList.size() > 0)
	  {
	     //写表头标题
		  szBody += "int,string,string,int,string,string,string,string,string,string\n";
		  szBody += "序号,NPC名称,NPC功能名称,模型编号,坐标,朝向,脚本,对话,备注,版本\n";
          szBody += "ID,Name,FunctionName,ModelID,Coordinate,Direction,Script,Dialogue\n";
          pFile->Serialize((char*)(szBody.c_str()), (int)strlen(szBody));
	  }
  
	  for( iterobj = pEditor->m_NpcBrushList.begin();iterobj != pEditor->m_NpcBrushList.end();iterobj++)
	  {
	     szBody.Empty();
	     pEveryObj = (*iterobj).m_NpcModel;
		 
		 //mx = ((RtsModel*)pEveryObj)->GetActor()->GetWorldMatrix()->_30;
         //my = ((RtsModel*)pEveryObj)->GetActor()->GetWorldMatrix()->_31;

	     mx = *(pEveryObj->GetMatrix().Position());
	     my = *(pEveryObj->GetMatrix().Position() + 1);
	     //mz = *(pEveryObj->GetMatrix().Position() + 2);
	     //sprintf(szBody,"%d,%.2f;%.2f;%.2f\n", pEveryObj->m_dwID,mx,my,mz);
		 //sprintf(szNum,"%d",(*iterobj)->iSerialNum);
		 szBody += ((*iterobj).iSerialNum).c_str();
         szBody += ",";
         szBody += ((*iterobj).szNpcName).c_str();
         szBody += ",";
         szBody += ((*iterobj).szNpcFuncName).c_str();
         szBody += ",";
         szBody += ((*iterobj).iModelId).c_str();
         szBody += ",";
         sprintf(szNum,"%.2f",mx);
         szBody += szNum;
         szBody += ";";
         sprintf(szNum,"%.2f",my);
         szBody += szNum;
         szBody += ",";
         //sprintf(szNum,"%.2f",mz);
        // szBody += szNum;
         //szBody += ",";
         sprintf(szNum,"%.2f",(*iterobj).m_NpcDir.x);
		 szBody += szNum;
		 szBody += ";";
         sprintf(szNum,"%.2f",(*iterobj).m_NpcDir.y);
		 szBody += szNum;
		 szBody += ",";
		 //pFile->Serialize((char*)(szBody.c_str()), (int)strlen(szBody));
		// sprintf(szNum,"%.2f",(*iterobj)->m_NpcDir.z);
		 //szBody += szNum;
		 //szBody += ",";
        // szBody.Empty();
         szBody += "\"";
         szBody += ((*iterobj).szScript).c_str();
		 szBody += "\"";
         szBody += ",";
         szBody += "\"";
         szBody += ((*iterobj).szDialog).c_str();
		 szBody += "\"";
		 szBody += ",";
		 //pFile->Serialize((char*)(szBody.c_str()), (int)strlen(szBody));
         //szBody.Empty();
		 szBody += "\"";
		 szBody += ((*iterobj).szAttention).c_str();
		 szBody += "\"";
		 szBody += ",";
		 //pFile->Serialize((char*)(szBody.c_str()), (int)strlen(szBody));
		// szBody.Empty();
		 szBody += "\"";
         szBody += ((*iterobj).szVertion).c_str();
		 szBody += "\"";
		 //szBody += "\r";
         szBody += "\n";
		 pFile->Serialize((char*)(szBody.c_str()), (int)strlen(szBody));

      }
	 
	 RtCoreFile().CloseFile(pFile);
   }
}
