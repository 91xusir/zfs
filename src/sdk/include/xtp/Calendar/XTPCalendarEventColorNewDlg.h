// XTPCalendarEventColorNewDlg.h : header file
//
// (c)1998-2018 Codejock Software, All Rights Reserved.
//
// THIS SOURCE FILE IS THE PROPERTY OF CODEJOCK SOFTWARE AND IS NOT TO BE
// RE-DISTRIBUTED BY ANY MEANS WHATSOEVER WITHOUT THE EXPRESSED WRITTEN
// CONSENT OF CODEJOCK SOFTWARE.
//
// THIS SOURCE CODE CAN ONLY BE USED UNDER THE TERMS AND CONDITIONS OUTLINED
// IN THE XTREME TOOLKIT PRO LICENSE AGREEMENT. CODEJOCK SOFTWARE GRANTS TO
// YOU (ONE SOFTWARE DEVELOPER) THE LIMITED RIGHT TO USE THIS SOFTWARE ON A
// SINGLE COMPUTER.
//
// CONTACT INFORMATION:
// support@codejock.com
// http://www.codejock.com
//
/////////////////////////////////////////////////////////////////////////////

//{{AFX_CODEJOCK_PRIVATE
#if !defined(__XTPCALENDAREVENTCOLORNEWDLG_H__)
#define __XTPCALENDAREVENTCOLORNEWDLG_H__
//}}AFX_CODEJOCK_PRIVATE

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Common/Base/Diagnostic/XTPDisableNoisyWarnings.h"

//===========================================================================
// Summary: This class is used to add new Calendar Event Categories or Labels
//          to the existing data array.
//===========================================================================
class _XTP_EXT_CLASS CXTPCalendarEventColorNewDlg : public CDialog
{
	//{{AFX_CODEJOCK_PRIVATE
	DECLARE_DYNAMIC(CXTPCalendarEventColorNewDlg)
	//}}AFX_CODEJOCK_PRIVATE
public:
	// ------------------------------------------------------------------
	// Summary:    Constructs a CXTPCalendarEventColorNewDlg object used 
	//             to add new calendar event categories and labels.
	// Parameters: bCategories : TRUE to edit event category list, FALSE
	//                           to edit event labels.
	//                 pParent : A pointer to the dialog box's parent 
	//                           or owner window.
	// ------------------------------------------------------------------
	CXTPCalendarEventColorNewDlg(BOOL bCategories, CWnd* pParent = NULL);

protected:
	BOOL  m_bCategories; // TRUE when editing event categories.

	// ------------------------------------------------------------------
	// Summary: This member function is called to enable / disable the OK 
	//          button for the dialog.
	// ------------------------------------------------------------------
	virtual void EnableOkButton();

//{{AFX_CODEJOCK_PRIVATE
	//{{AFX_DATA(CXTPCalendarEventColorNewDlg)
	public:
	enum { IDD = XTP_IDD_CALENDAR_EVENT_COLORNEW };
	CButton	m_btnOk;
	CXTPColorPicker	m_cpEventColor;
	COLORREF m_crColor;
	CString	m_strName;
	//}}AFX_DATA

	//{{AFX_VIRTUAL(CXTPCalendarEventColorNewDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

	//{{AFX_MSG(CXTPCalendarEventColorNewDlg)
	afx_msg void OnChangeEditName();
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	afx_msg void OnSelEndOkColor();

	DECLARE_MESSAGE_MAP()
//}}AFX_CODEJOCK_PRIVATE
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.


#include "Common/Base/Diagnostic/XTPEnableNoisyWarnings.h"
#endif // !defined(__XTPCALENDAREVENTCOLORNEWDLG_H__)
