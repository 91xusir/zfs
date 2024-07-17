// XTPComboBoxEditCtrl.h : interface of the CXTPComboBoxEditCtrl class.
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
#if !defined(__XTPCOMBOBOXEDITCTRL_H__)
#define __XTPCOMBOBOXEDITCTRL_H__
//}}AFX_CODEJOCK_PRIVATE

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Common/Base/Diagnostic/XTPDisableNoisyWarnings.h"

//===========================================================================
// Summary: Used by the CXTPComboBox object to subclass the edit-box portion
//          of the control.
//===========================================================================
class _XTP_EXT_CLASS CXTPComboBoxEditCtrl : public CEdit
{
protected:

	//{{AFX_CODEJOCK_PRIVATE

	//{{AFX_VIRTUAL(CXTPComboBoxEditCtrl)
	virtual void OnMouseLeave();
	virtual void OnMouseMove(UINT nFlags, CPoint point);
	//}}AFX_VIRTUAL

	//{{AFX_MSG(CXTPComboBoxEditCtrl)
	afx_msg void OnSetFocus(CWnd* pOldWnd);
	afx_msg void OnKillFocus(CWnd* pNewWnd);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()

	//}}AFX_CODEJOCK_PRIVATE
};

#include "Common/Base/Diagnostic/XTPEnableNoisyWarnings.h"
//{{AFX_CODEJOCK_PRIVATE
#endif // __XTPCOMBOBOXEDITCTRL_H__
//}}AFX_CODEJOCK_PRIVATE
