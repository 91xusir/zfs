// XTPEditThemeOffice2000.h
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
#if !defined(__XTPEDITTHEMEOFFICE2000_H__)
#define __XTPEDITTHEMEOFFICE2000_H__
//}}AFX_CODEJOCK_PRIVATE

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Common/Base/Diagnostic/XTPDisableNoisyWarnings.h"

//===========================================================================
// Summary: CXTPEditThemeOffice2000 is a class used to perform theme
//          drawing tasks.
//===========================================================================
class _XTP_EXT_CLASS CXTPEditThemeOffice2000 : public CXTPEditThemeDefault
{
public:

	//-----------------------------------------------------------------------
	// Summary: Constructs a CXTPEditThemeOffice2000 object
	//-----------------------------------------------------------------------
	CXTPEditThemeOffice2000();

	//-----------------------------------------------------------------------
	// Summary: Destructor, handles cleanup and deallocation.
	//-----------------------------------------------------------------------
	virtual ~CXTPEditThemeOffice2000();

	//-----------------------------------------------------------------------
	// Summary: This member function is called by the theme manager to refresh
	//          the visual styles used by each components theme.
	// Input:   pEdit - Points to a CXTPEdit object.
	//-----------------------------------------------------------------------
	virtual void RefreshMetrics(CXTPEdit* pEdit);

	//-----------------------------------------------------------------------
	// Summary: This member function is called by the theme manager to draw
	//          non-client borders for the edit control.
	// Input:   pDC   : Points to a valid device context for the control.
	//          rcWnd : Size of the area to draw borders on.
	//          pEdit : Points to a CXTPEdit object.
	//-----------------------------------------------------------------------
	virtual void DrawNcBorders(CDC* pDC, CRect rcWnd, CXTPEdit* pEdit);

	//-----------------------------------------------------------------------
	// Summary: Called by the framework to draw the borders for the associated
	//          buddy window.
	// Input:   pBuddy     - Points to the buddy control associated with the edit.
	//          pDC        - Points to a valid device context.
	//          rc         - Size of the buddy border area to be drawn.
	//          nAlignment - Side of the edit box the buddy is attached to, 
	//                       either UDS_ALIGNRIGHT or UDS_ALIGNLEFT.
	//          pEdit      - Points to the edit control for the buddy window.
	//-----------------------------------------------------------------------
	virtual void DrawBuddyBorders(CWnd* pBuddy, CDC* pDC, CRect rc, int nAlignemnt, CXTPEdit* pEdit);

	//-----------------------------------------------------------------------
	// Summary: Called by the framework to redraw the focus rectangle for the
	//          edit control.
	//-----------------------------------------------------------------------
	virtual BOOL RedrawFocusedFrame() const;
};


#include "Common/Base/Diagnostic/XTPEnableNoisyWarnings.h"
#endif // __XTPEDITTHEMEOFFICE2000_H__
