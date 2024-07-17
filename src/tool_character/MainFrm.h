// MainFrm.h : CMainFrame 类的接口
//
#pragma once
#include "DlgSliderView.h"
#include <vector>
using namespace std;
// #define CXTPFrameWnd CXTPFrameWnd
//#include "actor.h"
#include "TimeView.h"

// global function
bool GetActorFile(string &name,string &path);
bool GetMultiActorFile(vector<CString> &path);
extern bool g_bRenderShadow;
extern DWORD shadowFloat;
extern string g_openFile;
extern bool g_bRelatedMode;
extern bool g_bsave;

class CToolActor : public CRT_PoseNotify
{
public:
	CToolActor();
	virtual ~CToolActor();

	// from CRT_PoseNotify
	virtual void OnPoseBegin(SRT_Pose *pose);
	virtual void OnPoseEnd(SRT_Pose *pose);
	virtual void OnPoseEvent(SRT_Pose *pose,SRT_PoseEvent *event);

	void StopPose()
	{
		m_inst->StopPose();
	}
	string GetSelectedPose(void *p)
	{
		// R [12/18/2008 Louis.Huang]
		//return m_data->GetSelectedPose(p);
		return m_inst->GetSelectedPose(p);
	}
	void PlayPose(string pose,bool loop=false)
	{
		m_inst->PlayPose(pose.c_str(),loop);
	}
	void AttachPoseGrid(void *p)
	{
		m_inst->AttachPoseGrid(p);
	}
	bool OnPoseGridNotify(WPARAM wParam, LPARAM lParam)
	{
		return m_inst->OnPoseGridNotify(wParam,lParam);
	}
	void AttachLinkBoxGrid(void *p)
	{
		m_inst->AttachLinkBoxGrid(p);
	}
	bool OnLinkBoxGridNotify(WPARAM wParam, LPARAM lParam)
	{
		return m_inst->OnLinkBoxGridNotify(wParam,lParam);
	}
	void AttachGrid(void *p)
	{
		m_inst->AttachGrid(p);
	}
	bool OnGridNotify(WPARAM wParam, LPARAM lParam)
	{
		return m_inst->OnGridNotify(wParam,lParam);
	}

	// effect 
	CRT_Effect *GetSelectedEffect(void *p)
	{
		return m_inst->GetSelectedEffect(p);
	}
	void AttachEffectGrid(void *p)
	{
		m_inst->AttachEffectGrid(p);
	}
	bool OnEffectGridNotify(WPARAM wParam, LPARAM lParam)
	{
		return m_inst->OnEffectGridNotify(wParam,lParam);
	}
	
	void Tick(unsigned long delta)
	{
		m_inst->Tick(delta);
	}
	void UseFrame(unsigned long frame)
	{
		m_inst->UseFrame(frame);
	}
	unsigned long GetFrameNum()
	{
		return m_inst->GetFrameNum();
	}
    bool Save(RtArchive *ar)
	{
		if(m_inst)
			return m_inst->Save(ar);
		return false;
	}

	void Render(RtgDevice *dev, RTGRenderMask mask);
	void RenderShadow(RtgDevice *dev);
	int  GetId()		{ return m_id; }
	void SetId(int id)	{ m_id = id; }
	int  m_id;

	void ModifyWeapon();
	void DelWeapon();

	CRT_ActorInstance *m_inst;
	CRT_Skin*		   m_pSelectedSkin;
	//CRT_ActorInstance *m_weapon;
    vector<CRT_ActorInstance*> m_vSubActor;

	CRT_Actor *m_data;
	string m_pose1;
	string m_pose2;
	
	list<CRT_ActorInstance*> m_eftList;
	RtgMatrix16 m_matrix;
};

extern vector<CToolActor*> g_actorList;
extern CToolActor *g_activeActor;

void SetActiveActor(CToolActor *ob);
void SetActiveActor(int id);
void DelActiveActor();
CToolActor *CreateToolActor();

// #define CXTPFrameWnd CXTPFrameWnd
class CMainFrame : public CXTPFrameWnd	
{
protected: // 仅从序列化创建
	CMainFrame();
	DECLARE_DYNCREATE(CMainFrame)

// 属性
public:
    CXTPDockingPaneManager  m_paneManager;
    CImageList              m_imageListTree;
   
	// new value
	CTimeView				m_viewKey;
	CXTPPropertyGrid        m_gridEnv;
	CXTPPropertyGrid        m_gridPose;
	CXTPPropertyGrid        m_gridActor;
	CXTPPropertyGrid        m_gridMtlLib;
	CXTPPropertyGrid        m_gridLinkBox;

	CXTPPropertyGrid        m_gridProp;
    // CDlgSliderView          m_viewSlider;

// 操作
public:
	CWnd* CreateViewKey();

	CWnd* CreateGridEnv();
	CWnd* CreateGridPose(bool force);
	CWnd* CreateGridActor(bool force);
	CWnd* CreateGridMtlLib(bool force);
	CWnd* CreateGridProp(bool force);
	CWnd* CreateGridLinkBox(bool force);

	CWnd* CreateViewSlider();
	void  CreateAllGrid();
	afx_msg int OnCreateControl(LPCREATECONTROLSTRUCT lpCreateControl);

public:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
    virtual BOOL DestroyWindow();

public:
	virtual ~CMainFrame();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:  
	CStatusBar  m_wndStatusBar;

protected:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
    afx_msg LRESULT OnDockingPaneNotify(WPARAM wParam, LPARAM lParam);
    afx_msg LRESULT OnGridNotify(WPARAM wParam, LPARAM lParam);
    afx_msg void OnTreeCtrlSelectChanged(NMHDR * pNotifyStruct, LRESULT * result);
	afx_msg void OnPaint();
	afx_msg void OnClose();
	/*
	afx_msg void OnViewTreeView();
	afx_msg void OnViewPropertyView();
	afx_msg void OnViewSliderView();
	*/
	DECLARE_MESSAGE_MAP()

public:
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnDestroy();
    
	CRT_Effect *GetSelectedEffect();
	CRT_Skin *GetSelectedSkin();
	CRT_SkinInstance *GetSelectedSkinInstance();
	CRT_MaterialLib *GetSelectedMtlLib();
	CRT_Material *GetSelectedMtl();
	string GetGridSelectedPose();
	string GetListSelectedPose();
	string m_oldSelectedPose;

	void RebuildPoseList();
	CXTPControlComboBox *GetPoseList();
	CXTPControlComboBox *m_pPoseList;
	CXTPToolBar *m_toolBar;
	CRT_MaterialLib *m_selectedMtlLib;

	bool m_bRebuildGridActor;
	bool m_bRebuildGridMtlLib;
	bool m_bRebuildGridPose;
	bool m_bRebuildGridLinkBox;
	bool m_bRebuildGridProp;
	DWORD m_selectedChannel;
    	
	void SaveAsActor();
	void SaveAsSkin();
	/*void AutoSave();*/
	
};
