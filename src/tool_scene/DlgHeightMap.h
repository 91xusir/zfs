#pragma once
#include "afxwin.h"


// CDlgHeightMap dialog

class CDlgHeightMap : public CDialog
{
	DECLARE_DYNAMIC(CDlgHeightMap)

public:
	CDlgHeightMap(CWnd* pParent = NULL);   // standard constructor
	virtual ~CDlgHeightMap();

// Dialog Data
	enum { IDD = IDD_DIALOG_HEIGHTMAP };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
    // œ‘ æ∏ﬂ∂»Õº
    CStatic m_imgHeightmap;
    void* m_pImage;
    virtual BOOL OnInitDialog();
    CStatic m_imgRectWnd;
};
