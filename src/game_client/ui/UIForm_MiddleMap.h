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
	// ��Ӧ������Hint��ʾ��ʹ�ã�����������Ӧ�ľ��η�Χ����ô��Ҫ��ʾstring������
	struct PatchPoint//Ѱ·�켣����Ϣ
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
	RtwWidget*					m_frmMiddleMap;		// ��ǰ����
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

	// ��ͼ���õ�������
	WORD						m_wShowFlag;		// ��ͼ�ı�־λ�����ڿ��ƿ���ȥ��ʾʲô
	long						m_iAreaX;
	long						m_iAreaY;

	// ��Ⱦλ�����ݣ���ǰ��Ļ���Լ�С��ͼ�Ĵ�С
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
	//	��¼�е�ͼ��Hint�����Ϣ���Լ���Ӧ�ķ�Χ
	vector<RECTINFO>			m_vecRectInfo;
	bool						m_bOpenMapFirst;				// ���ڱ�ע��ǰ��ͼ�Ƿ��һ�δ򿪣�����ǵ�һ�δ򿪣���ô��������Ϊ�棬������Ⱦ�е�ͼ��ʱ������Ϊ���ʵ��������ͼ
	bool						m_bChangeMap;
	// �е�ͼ�����label
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
	// Ѱ·Ŀ�ĵ�
	float						m_fDesPos[2];
	RtwImage*                   m_pPreviewImage;
	RtsPathPath*                m_pathNode;
	bool                        m_isGetPatchPoint;
public:
	bool						m_bNeedShowPoint;
};
