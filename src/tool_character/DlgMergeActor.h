#pragma once


// CDlgMergeActor dialog
#include <vector>
using namespace std;
class CRT_Actor;
class CDlgMergeActor : public CDialog
{
	DECLARE_DYNAMIC(CDlgMergeActor)

public:
	CDlgMergeActor(CWnd* pParent = NULL);   // standard constructor
	virtual ~CDlgMergeActor();

// Dialog Data
	enum { IDD = IDD_MERGE_ACTOR };
	bool MergeFile(CRT_Actor *a1, CRT_Actor *a2, bool eventOnly);

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedGetActor1();
	CString m_actor1Path;
	CString m_actor2Path;
	vector<CString> m_mulitActor2Path;

	afx_msg void OnBnClickedGetActor2();
	afx_msg void OnBnClickedOk();
	BOOL m_bReplace;
};
