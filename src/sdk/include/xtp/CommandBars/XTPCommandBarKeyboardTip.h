// XTPCommandBarKeyboardTip.h
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
#if !defined(__XTPCOMMANDBARKEYBOARDTIP_H__)
#define __XTPCOMMANDBARKEYBOARDTIP_H__
//}}AFX_CODEJOCK_PRIVATE

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Common/Base/Diagnostic/XTPDisableNoisyWarnings.h"

class CXTPCommandBars;

//===========================================================================
// Summary:
//     CXTPCommandBarKeyboardTip is CWnd derived class, represents Keyboard tip single window
//===========================================================================
class _XTP_EXT_CLASS CXTPCommandBarKeyboardTip : public CWnd
{
public:

	//-----------------------------------------------------------------------
	// Summary:
	//     Constructs a CXTPCommandBarKeyboardTip object
	// Parameters:
	//     pCommandBars - CXTPCommandBars parent object
	//     pOwner - Owner object
	//     lpszCaption - Caption of control
	//     pt - Client coordinates
	//     dwAlign - Align of tip
	//     bEnabled - TRUE if keyboard tip is enabled
	//-----------------------------------------------------------------------
	CXTPCommandBarKeyboardTip(CXTPCommandBars* pCommandBars, CCmdTarget* pOwner, LPCTSTR lpszCaption, CPoint pt, DWORD dwAlign, BOOL bEnabled);

protected:
//{{AFX_CODEJOCK_PRIVATE
	DECLARE_MESSAGE_MAP()

	afx_msg void OnPaint();
	afx_msg LRESULT OnNcHitTest(CPoint point);
//}}AFX_CODEJOCK_PRIVATE

public:
	CString m_strTip;           // Tooltip
	CString m_strCaption;       // Caption
	CString m_strPrefix;       // Caption
	CCmdTarget* m_pOwner;       // Owner object
	CPoint m_pt;                // Client coordinates
	BOOL m_bEnabled;            // Enabled flag
	BOOL m_bVisible;            // TRUE if visible
	CXTPCommandBars* m_pCommandBars;    // Parent CXTPCommandBars object
	DWORD m_dwAlign;            // Tip align
	BOOL m_bExplicit;           // TRUE when user specify manually KeyTip; FALSE if calculated automatically
};


#include "Common/Base/Diagnostic/XTPEnableNoisyWarnings.h"
#endif // !defined(__XTPCOMMANDBARKEYBOARDTIP_H__)
