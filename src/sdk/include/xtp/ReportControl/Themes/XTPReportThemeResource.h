// XTPReportThemeResource.h: interface for the CXTPReportPaintManager class.
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
#if !defined(__XTPReportThemeResource_H__)
#define __XTPReportThemeResource_H__
//}}AFX_CODEJOCK_PRIVATE

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Common/Base/Diagnostic/XTPDisableNoisyWarnings.h"

//===========================================================================
// Summary:
//     CXTPReportThemeResource is a CXTPReportPaintManager derived
//     class.  This represents an Visual Studio 2010 style theme.
//===========================================================================
class _XTP_EXT_CLASS CXTPReportThemeResource : public CXTPReportPaintManager
{
public:
	//-----------------------------------------------------------------------
	// Summary:
	//     Default paint manager constructor, handles properties initialization.
	//-----------------------------------------------------------------------
	CXTPReportThemeResource();

	//-------------------------------------------------------------------------
	// Summary:
	//     This method is called to refresh the visual metrics of the
	//     report control.
	//-------------------------------------------------------------------------
	virtual void RefreshMetrics();

	//-----------------------------------------------------------------------
	// Summary:
	//     Draws collapsed bitmap.
	// Parameters:
	//     pDC      - Pointer to the used Device Context.
	//     pRow     - Pointer to the Row.
	//     rcBitmap - Bitmap area rectangle coordinates.
	// Returns:
	//     Size of drawn bitmap.
	//-----------------------------------------------------------------------
	CSize DrawCollapsedBitmap(CDC* pDC, const CXTPReportRow* pRow, CRect& rcBitmap);

	//-----------------------------------------------------------------------
	// Summary:
	//     This method used to draw "No items" text message in Group By area
	//     when there are no items inside it. Override it for custom drawing.
	// Parameters:
	//     pDC     - Pointer to the used Device Context.
	//     rcItem  - Reference to text area rectangle coordinates.
	// See Also:
	//     CXTPReportPaintManager::m_strNoGroupBy
	//-----------------------------------------------------------------------
	virtual void DrawNoGroupByText(CDC* pDC, CRect rcItem);

	//-----------------------------------------------------------------------
	// Summary:
	//     Fills column footer control area by its background.
	// Parameters:
	//     pDC      -  Pointer to the used Device Context.
	//     rcFooter -  Footer area rectangle coordinates.
	//-----------------------------------------------------------------------
	virtual void FillFooter(CDC* pDC, CRect rcFooter);

	//-----------------------------------------------------------------------
	// Summary:
	//     Draws triangle that shows the sort order of column.
	// Parameters:
	//     pDC        - Pointer to the used Device Context.
	//     rcTriangle - Triangle area rectangle coordinates.
	//     bToDown    - If TRUE the top of triangle turn downward, otherwise upward.
	// Remarks:
	//     You use this function to draw a triangle image on the header of the column
	//     that is sorted. Triangle represents the direction of sorting: ascending or descending.
	//     Sizes of the triangle depend on the rectangle area that is provided to draw
	// Example:
	// <code>
	// // draw ascendant triangle
	// CRect rcTriangle;
	// rcTriangle.CopyRect(rcHeader);
	// rcTriangle.DeflateRect(40, 5, 5, 5);
	// DrawTriangle(pDC, rcTriangle, TRUE);
	//
	// // draw descendant triangle
	// CRect rcTriangle;
	// rcTriangle.CopyRect(rcHeader);
	// rcTriangle.DeflateRect(40, 5, 5, 5);
	// DrawTriangle(pDC, rcTriangle, FALSE);
	// </code>
	//-----------------------------------------------------------------------
	virtual void DrawTriangle(CDC* pDC, CRect rcTriangle, BOOL bToDown, int nShadowWidth, BOOL bColumnPressed);
	
protected:
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

	virtual void DrawPlainColumnBackground(CDC* /*pDC*/, CRect /*rcColumn*/);
	
public:
	//-----------------------------------------------------------------------
	// Summary:
	//     Draws column footer divider.
	// Parameters:
	//     pDC           - Pointer to the used Device Context.
	//     pColumn       - Column header area rectangle coordinates.
	//     rcColumn      - Column area rectangle coordinates.
	//-----------------------------------------------------------------------
	virtual void DrawColumnFooterDivider(CDC* pDC, CXTPReportColumn* pColumn, CRect rcColumn);

	//-----------------------------------------------------------------------
	// Summary:
	//     Draws text for column header.
	// Parameters:
	//     pDC              - Pointer to the used Device Context.
	//     rcText           - Size and position of the text to render.
	//     strCaption       - NULL terminated string representing caption text.
	//     nHeaderAlignment - DT_ text alignment flags.
	//     bIsHeader        - TRUE if column is to be rendered for header.
	//-----------------------------------------------------------------------
	virtual void DrawColumnText(CDC* pDC, CRect rcText, CString strCaption, int nHeaderAlignment, BOOL bIsHeader, BOOL bColumnPressed);
	
	//-----------------------------------------------------------------------
	// Summary:
	//     Draws the divider between report sections (e.g. dividing header record rows and body rows).
	// Parameters:
	//     pDC   - Pointer to the used Device Context.
	//     rc - Rectangle coordinates.
	//     dividerPosition - position of section divider.
	//     dividerStyle - style of section divider.
	//     dividerColor - color of section divider.
	//-----------------------------------------------------------------------
	void DrawSectionDivider(CDC* pDC,
		const CRect& rc,
		XTPReportSectionDividerPosition dividerPosition,
		XTPReportSectionDividerStyle    dividerStyle,
		CXTPPaintManagerColor           dividerColor);
};


#include "Common/Base/Diagnostic/XTPEnableNoisyWarnings.h"
#endif //#if !defined(__XTPReportThemeResource_H__)
