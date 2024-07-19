// XTPCalendarEventColorDlg.h : header file
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
#if !defined(__XTPCALENDAREVENTCOLORDLG_H__)
#define __XTPCALENDAREVENTCOLORDLG_H__
//}}AFX_CODEJOCK_PRIVATE

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Common/Base/Diagnostic/XTPDisableNoisyWarnings.h"

//===========================================================================
// Summary: This class is used to edit Calendar Event Category and Label 
//          information.
//===========================================================================
class _XTP_EXT_CLASS CXTPCalendarEventColorDlg : public CDialog
{
	//{{AFX_CODEJOCK_PRIVATE
	DECLARE_DYNAMIC(CXTPCalendarEventColorDlg)
	//}}AFX_CODEJOCK_PRIVATE
public:
	// ------------------------------------------------------------------
	// Summary:    Constructs a CXTPCalendarEventColorDlg object used to
	//             edit calendar event categories and labels.
	// Parameters: bCategories   : TRUE to edit event category list, FALSE
	//                             to edit event labels.
	//             pCalendarCtrl : Points to a valid CXTPCalendarControl
	//                             object.
	//             pParent       : A pointer to the dialog box's parent 
	//                             or owner window.
	// ------------------------------------------------------------------
	CXTPCalendarEventColorDlg(BOOL bCategories, CXTPCalendarControl* pCalendarCtrl, CWnd* pParent = NULL);

protected:
	// ------------------------------------------------------------------
	// Summary:    This member function is called to update the diplayed
	//             color for the color picker whenever a listbox 
	//             selection changes.
	// Parameters: nIndex : Index of the listbox item to retrieve color
	//                      information from.  If LB_ERR, the currently
	//                      selected item is used.
	// ------------------------------------------------------------------
	void UpdateSelColor(int nIndex = LB_ERR);

	BOOL                 m_bCategories;   // TRUE when editing event categories.
	CXTPCalendarControl* m_pCalendarCtrl; // Pointer to the calendar object.
	CXTPCalendarEventPtr m_pEvent;        // Pointer to the calendar event object.

//{{AFX_CODEJOCK_PRIVATE
	//{{AFX_DATA(CXTPCalendarEventColorDlg)
	enum { IDD = XTP_IDD_CALENDAR_EVENT_COLOR };
	CStatic	m_wndEventInfo;
	CXTPColorPicker	m_cpEvent;
	CXTPCalendarEventColorListBox	m_lboxColors;
	//}}AFX_DATA

	//{{AFX_VIRTUAL(CXTPCalendarEventColorDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

	//{{AFX_MSG(CXTPCalendarEventColorDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnBtnRename();
	afx_msg void OnBtnDelete();
	afx_msg void OnBtnNew();
	afx_msg void OnSelChangeListCategories();
	afx_msg void OnBtnOK();
	afx_msg void OnEventReset();
	//}}AFX_MSG
	afx_msg void OnSelEndOkColor();

	DECLARE_MESSAGE_MAP()
//}}AFX_CODEJOCK_PRIVATE
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.


#include "Common/Base/Diagnostic/XTPEnableNoisyWarnings.h"
#endif // !defined(__XTPCALENDAREVENTCOLORDLG_H__)
