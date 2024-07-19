#pragma once


// CDlgChannelInfo dialog

class CDlgChannelInfo : public CDialog
{
	DECLARE_DYNAMIC(CDlgChannelInfo)

public:
	CDlgChannelInfo(CWnd* pParent = NULL);   // standard constructor
	virtual ~CDlgChannelInfo();

// Dialog Data
	enum { IDD = IDD_DLG_CHANNEL_INFO };

	CXTPPropertyGrid m_grid;
	CXTPPropertyGridItem *m_pRoot;
	void Init(void *p);
	void *m_object;
	bool m_bRebuild;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
	
public:
	virtual BOOL OnInitDialog();
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg LRESULT OnGridNotify(WPARAM wParam, LPARAM lParam);
	afx_msg void OnTimer(UINT nIDEvent);
};
