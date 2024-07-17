#pragma once


// CDlgKeyInfo dialog

class CDlgKeyInfo : public CDialog
{
	DECLARE_DYNAMIC(CDlgKeyInfo)

public:
	CDlgKeyInfo(CWnd* pParent = NULL);   // standard constructor
	virtual ~CDlgKeyInfo();

// Dialog Data
	enum { IDD = IDD_DLG_KEY_INFO };

	CXTPPropertyGrid m_grid;
	CXTPPropertyGridItem *m_pRoot;
	void Init(int key,void *p);
	void *m_object;
	int   m_key;
	bool  m_bRebuild;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
	virtual BOOL OnInitDialog();
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg LRESULT OnGridNotify(WPARAM wParam, LPARAM lParam);
	afx_msg void OnTimer(UINT nIDEvent);
};
