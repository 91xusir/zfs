#pragma once
#include "afxwin.h"

struct RtsSceneBlockMap;

// CDlgBlockInfo dialog

class CDlgBlockInfo : public CDialog
{
	DECLARE_DYNAMIC(CDlgBlockInfo)

public:
	CDlgBlockInfo(CWnd* pParent = NULL);   // standard constructor
	virtual ~CDlgBlockInfo();

    void Init(RtsSceneBlockMap* pBlockMap);

// Dialog Data
	enum { IDD = IDD_DLG_BLOCK_INFO };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
    virtual void OnOK();

	DECLARE_MESSAGE_MAP()

private:
    CXTPPropertyGrid        m_wndGrid;
    RtsSceneBlockMap*       m_pBlockMap;

public:
    afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
    afx_msg void OnSize(UINT nType, int cx, int cy);
    afx_msg LRESULT OnGridNotify(WPARAM wParam, LPARAM lParam);

};
