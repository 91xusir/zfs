// XTPListBoxThemeOffice2013.h
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
#if !defined(__XTPLISTBOXTHEMEOFFICE2013_H__)
#define __XTPLISTBOXTHEMEOFFICE2013_H__
//}}AFX_CODEJOCK_PRIVATE

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Common/Base/Diagnostic/XTPDisableNoisyWarnings.h"

//===========================================================================
// Summary:
//     CXTPListBoxThemeOffice2013 is used by CXTPListBox to draw an Office
//     2013 style list box.
//===========================================================================
class _XTP_EXT_CLASS CXTPListBoxThemeOffice2013 : public CXTPListBoxTheme
{
public:

	//-----------------------------------------------------------------------
	// Summary: Default constructor.
	//-----------------------------------------------------------------------
	CXTPListBoxThemeOffice2013();

	//-----------------------------------------------------------------------
	// Summary: This method is called to draw the CXTPListBox object.
	// Input:   pDC      - Pointer to a valid device context.
	//          pListBox - Pointer to a valid CXTPListBox object.
	//          lpDIS    - A long pointer to a DRAWITEMSTRUCT structure. The 
	//                     structure contains information about the item to be 
	//                     drawn and the type of drawing required. 
	//-----------------------------------------------------------------------
	virtual void DrawItem(CDC *pDC, CXTPListBox *pListBox, LPDRAWITEMSTRUCT lpDIS);

	//-----------------------------------------------------------------------
	// Summary: Called to update colors and metrics used to draw the 
	//          CXTPListBox object.
	//-----------------------------------------------------------------------
	virtual void RefreshMetrics();

	//-----------------------------------------------------------------------
	// Summary: Called to retreive the background color for the list-box.
	// Returns: RGB value representing the background color.
	//-----------------------------------------------------------------------
	virtual COLORREF GetBackColor();

	//-----------------------------------------------------------------------
	// Summary: Call this member function to draw the list-control borders.
	// Input:   pDC - Points to a valid device context.
	//          rc  - Size of the area ot draw.
	// Returns: TRUE if non-client border drawing is handled, otherwise FALSE.
	//-----------------------------------------------------------------------
	virtual BOOL DrawNcBorders(CDC* pDC, CRect rc);

public:

	COLORREF m_clrBack;                // List-box background color.
	COLORREF m_clrBorder;              // List-box border color.
	COLORREF m_clrItemText;            // List-box item normal text color.
	COLORREF m_clrItemTextHighlight;   // List-box item highlighted text color.
	COLORREF m_clrItemTextSelected;    // List-box item selected text color.
	COLORREF m_clrItemBack;            // List-box item normal background color.
	COLORREF m_clrItemBackHighlight;   // List-box item highlighted background color.
	COLORREF m_clrItemBackSelected;    // List-box item selected background color.
	COLORREF m_clrItemBorder;          // List-box item normal border color.
	COLORREF m_clrItemBorderHighlight; // List-box item highlighted border color.
	COLORREF m_clrItemBorderSelected;  // List-box item selected border color.
};


#include "Common/Base/Diagnostic/XTPEnableNoisyWarnings.h"
#endif // !defined(__XTPLISTBOXTHEMEOFFICE2013_H__)
