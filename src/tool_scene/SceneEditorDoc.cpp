// SceneEditorDoc.cpp :  CSceneEditorDoc ���ʵ��
//

#include "stdafx.h"
#include "tool_scene.h"
#include "resource.h"

#include "BrushMapView.h"
#include "BrushVertexView.h"
#include "BrushColorView.h"
#include "area_list_view.h"
#include "pathpoint_list_view.h"
#include "MainFrame.h"
#include "scene/rt_scene.h"
#include "rts_scene_tool.h"
#include "core/rt2_core.h"
#include "audio/rt_audio.h"
#include "brush_tool.h"
#include "brush_tool_object.h"
#include "direct.h"
#include "EditorFrame.h"
#include "SceneEditorDoc.h"

#ifdef RT_DEBUG_NEW
#   undef new
#endif

// CSceneEditorDoc

IMPLEMENT_DYNCREATE(CSceneEditorDoc, CDocument)

BEGIN_MESSAGE_MAP(CSceneEditorDoc, CDocument)
END_MESSAGE_MAP()


// CSceneEditorDoc ����/����

CSceneEditorDoc::CSceneEditorDoc()
{
}

CSceneEditorDoc::~CSceneEditorDoc()
{
}


// CSceneEditorDoc ���л�

//void CSceneEditorDoc::Serialize(CArchive& ar)
//{
//	if (ar.IsStoring())
//	{
//		// TODO: �ڴ���Ӵ洢����
//	}
//	else
//	{
//		// TODO: �ڴ���Ӽ��ش���
//	}
//    MessageBox(NULL, "Serialize", 0, 0);
//}


// CSceneEditorDoc ���

#ifdef _DEBUG
void CSceneEditorDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CSceneEditorDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG


// CSceneEditorDoc ����

BOOL CSceneEditorDoc::OnNewDocument()
{
//    guard;
    //if (!CDocument::OnNewDocument())
    //    return FALSE;
    // TODO: �ڴ�������³�ʼ������
    // (SDI �ĵ������ø��ĵ�)
    if (g_pScene==NULL) return TRUE;
    g_pScene->Init();
    if (!((RtSceneTool*)g_pScene)->ToolNewScene(100, 100))
    {
        return FALSE;
    }
    return TRUE;
  //  unguard;
}

BOOL CSceneEditorDoc::OnOpenDocument(LPCTSTR lpszPathName)
{
  //  guard;
    // if (!CDocument::OnOpenDocument(lpszPathName))
    //    return FALSE;
    if (g_pScene==NULL) return TRUE;

	RtCoreFile().SetDefaultDirectory(theApp.WorkingDirectory());
    g_pScene->Init();

    if (!((RtSceneTool*)g_pScene)->ToolLoadScene(lpszPathName))
    {
        //ERR1("Can't load height \"%s\"\n", lpszPathName);
        return FALSE;
    }
	
	char szDriver[8];
	char ext[8];
	char szRtsFileName[64];
	char szRtsFilePath[128];
	char szWpPathName[256];
	_TexItem *pItem;

	_splitpath(lpszPathName, szDriver, szRtsFilePath, szRtsFileName, ext);
	strcpy(szWpPathName, szDriver);
	strcat(szWpPathName, szRtsFilePath);
	strcat(szWpPathName, szRtsFileName);
	strcat(szWpPathName, ".wp");

	extern CRtgAppFrame* g_pAppCase;
	CMainFrame* thisFrame = ((CEditorFrame*)g_pAppCase)->m_pMainFrame;
	if(!thisFrame)
		return TRUE;

	thisFrame->CreatePathPointListView();
	thisFrame->m_wndPathPointView.LoadWaypointFile(szWpPathName);
	
    return TRUE;
  //  unguard;
}

BOOL CSceneEditorDoc::OnSaveDocument(LPCTSTR lpszPathName)
{
   // guard;
    RtCoreFile().SetDefaultDirectory(theApp.WorkingDirectory());
    if (!((RtSceneTool*)g_pScene)->ToolSaveScene(lpszPathName))
    {
        //ERR1("Can't save height \"%s\"\n", lpszPathName);
        return TRUE;
    }

	char szDriver[8];
	char ext[8];
	char szRtsFileName[64];
	char szRtsFilePath[128];
	char szWpPathName[256];
	_TexItem *pItem;

	_splitpath(lpszPathName, szDriver, szRtsFilePath, szRtsFileName, ext);
	strcpy(szWpPathName, szDriver);
	strcat(szWpPathName, szRtsFilePath);
	strcat(szWpPathName, szRtsFileName);
	strcat(szWpPathName, ".wp");

	extern CRtgAppFrame* g_pAppCase;
	CMainFrame* thisFrame = ((CEditorFrame*)g_pAppCase)->m_pMainFrame;
	if(!thisFrame)
		return TRUE;

	thisFrame->m_wndPathPointView.SaveWaypointFile(szWpPathName);

    return TRUE;
    // return CDocument::OnSaveDocument(lpszPathName);
   // unguard;
}
