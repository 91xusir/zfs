// XTPReportThemeDefault.h: interface for the CXTPReportPaintManager class.
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
#if !defined(__XTPREPORTTHEMEDEFAULT_H__)
#define __XTPREPORTTHEMEDEFAULT_H__
//}}AFX_CODEJOCK_PRIVATE

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Common/Base/Diagnostic/XTPDisableNoisyWarnings.h"

//===========================================================================
// Summary:
//     CXTPReportThemeDefault is a CXTPReportPaintManager derived
//     class.  This represents an Office 97 style theme.
//===========================================================================
class _XTP_EXT_CLASS CXTPReportThemeDefault : public CXTPReportPaintManager
{
public:
	//-----------------------------------------------------------------------
	// Summary:
	//     Default paint manager constructor, handles properties initialization.
	//-----------------------------------------------------------------------
	CXTPReportThemeDefault();

	//-----------------------------------------------------------------------
	// Summary:
	//     This method is called to draw background of the column using current column style
	// Parameters:
	//     pDC - Point to used Device Context
	//     pColumn - Column needs to draw
	//     rcColumn - Bounding rectangle of the column
	//     bColumnPressed - TRUE if column is pressed by user.
	//     bDraggingFromHeader - TRUE if column is begin dragged from header area.
	//     pHeader - Pointer to a header the column belongs to.
	//-----------------------------------------------------------------------
	virtual void DrawColumnBackground(CDC* pDC,
		CXTPReportColumn* pColumn,
		CRect rcColumn,
		BOOL& bColumnPressed,
		BOOL& bDraggingFromHeader,
		CXTPReportHeader* pHeader = NULL);
};


#include "Common/Base/Diagnostic/XTPEnableNoisyWarnings.h"
#endif //#if !defined(__XTPREPORTTHEMEDEFAULT_H__)
