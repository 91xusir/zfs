// XTPEditThemeResource.h
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
#if !defined(__XTPEDITTHEMERESOURCE_H__)
#define __XTPEDITTHEMERESOURCE_H__
//}}AFX_CODEJOCK_PRIVATE

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Common/Base/Diagnostic/XTPDisableNoisyWarnings.h"

//===========================================================================
// Summary: CXTPEditThemeResource is a class used to perform theme
//          drawing tasks.
//===========================================================================
class _XTP_EXT_CLASS CXTPEditThemeResource : public CXTPEditThemeOfficeXP
{
public:

	//-----------------------------------------------------------------------
	// Summary: Constructs a CXTPEditThemeResource object
	//-----------------------------------------------------------------------
	CXTPEditThemeResource();

	//-----------------------------------------------------------------------
	// Summary: Destructor, handles cleanup and deallocation.
	//-----------------------------------------------------------------------
	virtual ~CXTPEditThemeResource();

	//-----------------------------------------------------------------------
	// Summary: This member function is called by the theme manager to refresh
	//          the visual styles used by each components theme.
	// Input:   pEdit - Points to a CXTPEdit object.
	//-----------------------------------------------------------------------
	virtual void RefreshMetrics(CXTPEdit* pEdit);

	//-----------------------------------------------------------------------
	// Summary: Called by the framework to adjust the size of the area to draw
	//          the buddy control borders on.
	// Input:   pBuddy     - Points to the buddy control associated with the edit.
	//          rcUpDown   - Size of the buddy border area to be drawn.
	//          nAlignment - Side of the edit box the buddy is attached to, 
	//                       either UDS_ALIGNRIGHT or UDS_ALIGNLEFT.
	// Returns: TRUE if successful, otherwise FALSE.
	//-----------------------------------------------------------------------
	virtual BOOL AdjustBuddyRect(CWnd* pBuddy, CRect& rcUpDown, int nAlignemnt);

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
};


#include "Common/Base/Diagnostic/XTPEnableNoisyWarnings.h"
#endif // __XTPEDITTHEMERESOURCE_H__
