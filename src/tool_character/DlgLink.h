#pragma once
#include "afxwin.h"
#include <string>
using namespace std;

class CToolObject;
// CDlgLink dialog
class CDlgLink : public CDialog
{
	DECLARE_DYNAMIC(CDlgLink)

public:
	CDlgLink(CWnd* pParent = NULL);   // standard constructor
	virtual ~CDlgLink();

// Dialog Data
	enum { IDD = IDD_LINK };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()

public:
	CListBox m_list;
	afx_msg void OnBnClickedOk();
	virtual BOOL OnInitDialog();

	CListBox m_weaList;
	string m_weaSlot;
	string m_poseName;
	afx_msg void OnBnClickedDelWeapon();
	afx_msg void OnBnClickedLoadWeapon();

	CListBox m_pose;

	BOOL m_bRotate;
    CListBox m_LisetImported;
    afx_msg void OnLbnSelchangeWeaponSlot();
    afx_msg void OnLbnSelchangePose();
    afx_msg void OnLbnSelchangeList();
};
