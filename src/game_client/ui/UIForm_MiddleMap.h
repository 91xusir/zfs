#pragma once
#include "ui/rtw_ui.h"

class CUIForm_MiddleMap
{
public:
	struct IMAGEINFO
	{
		RtwImage* pImage;
		RtwRect rect;
		IMAGEINFO(RtwImage* image,RtwRect* rec)
		{
			pImage = image;
			rect  = *rec;
		}
	};
	// 对应的设置Hint提示所使用，当鼠标进入相应的矩形范围，那么就要显示string的内容
	struct PatchPoint//寻路轨迹点信息
	{
		float x;
		float y;
		bool beFound;
	};
	struct RECTINFO
	{
		string RectInfo;
		RtwRect Rect;
	};
	CUIForm_MiddleMap(void);
	~CUIForm_MiddleMap(void);
	void Show();
	void Hide();
	void OnHide(ui::RtwWidget* sender,RtwEventDelegate* e);
	void Init();
	void Refresh();
	bool IsVisible();
	void OnHideMiddleMap(RtwWidget* sender,RtwEventDelegate* e);
	void OnMoveTo(ui::RtwWidget* sender,RtwEventDelegate* e);
	void OnMoveToXY(ui::RtwWidget* sender,RtwEventDelegate* e);
	void OnMoveToNPC(ui::RtwWidget* sender,RtwEventDelegate* e);
	void OnAutoToNPC(ui::RtwWidget* sender, RtwEventDelegate* e);
	void OnMouseRDrag(int x, int y, int increaseX, int increaseY);
	void OnEvSelectList(ui::RtwWidget* sender, RtwEventDelegate* e);
	void OnShowTaskInfo(ui::RtwWidget* sender, RtwEventDelegate* e);
	void OnDrawMiddleMap(RtwWidget* sender, RtwEventDelegate* e);
	void OnSelectComboBox(RtwWidget* sender, RtwEventDelegate* e);
	void OnCheckButtonChanged(ui::RtwWidget* sender, RtwEventDelegate* e);
	//void OnNavigationButtonDown(RtwWidget* sender, RtwEventDelegate* e);
	void OnPreDraw();
	RtwWidget* GetFormWidget(){guard;return m_frmMiddleMap;unguard;}
	RtwWidget* GetRealMapWidget(){guard;return m_pfrmMap;unguard;}
	RtwWidget* GetFocusParentWidget(RtwWidget* son);
	void SetFirstOpenMap(bool bFirst){guard;m_bOpenMapFirst = bFirst;unguard;}
	void SetMouseDownState(bool bIsDown){guard;m_bMouseRDown = bIsDown;unguard;}
	void SetOnMoveto(int x,int y);
	void SetOnMovetoNULL(){guard;m_bNeedShowPoint = false;unguard;};
	void SetReferencePoint();
	bool GetMouseDownState(){guard;return m_bMouseRDown;unguard;}
	float* GetMovetoPoint(){guard;return m_fDesPos;unguard;}
	bool GetWetherNeedShowMovetFlag(){guard;return m_bNeedShowPoint;unguard;}
	void ShowPreview();
	void GetPatchPoint();
	bool PatchPointIsFound(PatchPoint &patchpoint);
private:
	RtwWidget*					m_frmMiddleMap;		// 当前窗口
	RtwButton*                  m_btnClose;
	RtwWidget*					m_pfrmMap;
	RtwComboBox*				m_pcomComboBox;		// combobox
	RtwListBox*					m_pTaskListUI;
	/*RtwListBox*					m_pNPCListUI;*/
	RtwTree*                    m_pNPCTree;
	RtwButton*					m_pbtnTask;
	RtwButton*					m_pbtnNPC;
	RtwWidget*					m_plbTasklabel;
	RtwButton*					m_pbtnUp;
	RtwButton*					m_pbtnDown;
	RtwButton*					m_pbtnLeft;
	RtwButton*					m_pbtnRight;
	RtwButton*					m_pbtnAutoRunToNPC;
	RtwButton*					m_pbtnShowTaskInfo;
	RtwTab*						m_ptabControl;
	RtwTextBox*					m_ptbxPosX;
	RtwTextBox*					m_ptbxPosY;
	RtwButton*					m_pbtnAutoMove;
	CUiCheckButton*				m_pcheckButton;

	// 地图中用到的数据
	WORD						m_wShowFlag;		// 地图的标志位，用于控制可以去显示什么
	long						m_iAreaX;
	long						m_iAreaY;

	// 渲染位置数据，当前屏幕的以及小地图的大小
	int							m_iScreenX;
	int							m_iScreenY;
	int							m_iScreenWidth;
	int							m_iScreenHeight;
	float						m_fSceneWidth;
	float						m_fMinimapWidth;
	float						m_fMinimapHeight;

	int							m_iNpcCount;
	UIFormMiniMap::SNpcPoint   	m_ptNpcs[40];
	UIFormMiniMap::SNpcPoint   	m_lastNpcHint;

	bool						m_bMouseRDown;
	float						m_fRealMapPosX;
	float						m_fRealMapPosY;
	int							m_iRealMapWidth;
	int							m_iRealMapHeight;

	int							m_iOffsetX;
	int							m_iOffsetY;
	string						m_strNowPlayerRealMap;
	string						m_strRealMapOld;
	string						m_strNowShowMap;
	string						m_strNextShowMap;
	map<string,SSceneInfo>		m_mapMiddleMapName;
	map<string,int>				m_mapStringToInt;
	map<string,IMAGEINFO*>		m_mapMiddleMapImageinfo;
	map<string,STaskDesc*>		m_mapTaskInfo;
	vector<vector<STaskDesc*>*>	m_vectorTaskDesc;
	vector<vector<SNpc*>*>		m_vectorSNpc;
	vector<STaskDesc*>			m_vectorCanAccept;
	int							m_iSelectMapNum;
	//	记录中地图上Hint相关信息，以及对应的范围
	vector<RECTINFO>			m_vecRectInfo;
	bool						m_bOpenMapFirst;				// 用于标注当前地图是否第一次打开，如果是第一次打开，那么将其设置为真，则在渲染中地图的时候将其置为玩家实际所处地图
	bool						m_bChangeMap;
	// 中地图上相关label
	RtwLabel*					m_lbPlayer;
	RtwLabel*                   m_lbPlayerpreview;
	RtwLabel*                   m_lbPreviwRect;
	RtwLabel*					m_lbPet;
	RtwLabel*					m_lbAutoFlag;
	RtwLabel*					m_lbNowMousePos;
	RtwForm*                    m_previewLabel;
	RtwComplexImage *           m_ImageAutoFlag;
	vector<RtwLabel*>			m_vectorNPCLabel;
	vector<RtwLabel*>			m_vectorNPCNameLabel;
	vector<RtwLabel*>			m_vectorTaskLabel;
	vector<RtwLabel*>			m_vectorMonsterLabel;
	vector<RtwLabel*>			m_vectorMonsterNameLabel;
	vector<RtwLabel*>			m_vectorTeamActor;
	vector<RtwLabel*>			m_vectorPatchPoint;
	vector<PatchPoint>          m_vPatchPoint;
	// 寻路目的地
	float						m_fDesPos[2];
	RtwImage*                   m_pPreviewImage;
	RtsPathPath*                m_pathNode;
	bool                        m_isGetPatchPoint;
public:
	bool						m_bNeedShowPoint;
};
