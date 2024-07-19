// NavigateButtonActiveFiles.h : header file
//
// This file is a part of the XTREME TOOLKIT PRO MFC class library.
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
#if !defined(__NAVIGATEBUTTONACTIVEFILES_H__)
#define __NAVIGATEBUTTONACTIVEFILES_H__
//}}AFX_CODEJOCK_PRIVATE

#include "Common/Base/Diagnostic/XTPDisableNoisyWarnings.h"

class _XTP_EXT_CLASS CXTPTabClientWnd::CNavigateButtonActiveFiles : public CXTPTabManagerNavigateButton
{
public:
	CNavigateButtonActiveFiles(CXTPTabManager* pManager, CXTPTabClientWnd* pTabClientWnd);

	void DrawEntry(CDC* pDC, CRect rc, COLORREF clr, BOOL bEnabled);

	void Reposition(CRect& rcNavigateButtons);

	void PerformClick(HWND hWnd, CPoint pt);

protected:
	BOOL m_bHiddenTabs;
	CXTPTabClientWnd* m_pTabClientWnd;
};

#include "Common/Base/Diagnostic/XTPEnableNoisyWarnings.h"

//{{AFX_CODEJOCK_PRIVATE
#endif // #if !defined(__NAVIGATEBUTTONACTIVEFILES_H__)
//}}AFX_CODEJOCK_PRIVATE
