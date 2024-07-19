#pragma once


// CDlgVerFix dialog

class CDlgVerFix : public CDialog
{
	DECLARE_DYNAMIC(CDlgVerFix)

public:
	CDlgVerFix(CWnd* pParent = NULL);   // standard constructor
	virtual ~CDlgVerFix();

// Dialog Data
	enum { IDD = IDD_DIALOG_VER_FIX };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedStart();
	void VerFix(const char *name);
	void PtWavConvert(const char *name);
	void SearchDirectory(string base,string sub);
	CString m_msg;

	int m_totalNum;
	int m_fixOk;
	int m_fixFailed;
};
