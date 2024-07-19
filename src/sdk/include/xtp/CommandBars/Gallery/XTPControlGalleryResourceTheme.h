// XTPControlGalleryResourceTheme
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
#if !defined(__XTPCONTROLGALLERYRESOURCETHEME_H__)
#define __XTPCONTROLGALLERYRESOURCETHEME_H__
//}}AFX_CODEJOCK_PRIVATE

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Common/Base/Diagnostic/XTPDisableNoisyWarnings.h"

//===========================================================================
// Summary:
//     The CXTPControlGalleryResourceTheme class is used to enable a Gallery Office 2007 style theme for Command Bars.
// See Also: CXTPPaintManager::SetTheme
//===========================================================================
class _XTP_EXT_CLASS CXTPControlGalleryResourceTheme : public CXTPControlGalleryPaintManager
{
public:
	//-----------------------------------------------------------------------
	// Summary: Constructs a CXTPControlGalleryResourceTheme object.
	// Input:   pPaintManager - Reference to a CXTPPaintManager object.
	//-----------------------------------------------------------------------
	CXTPControlGalleryResourceTheme(CXTPPaintManager* pPaintManager);

public:
	//-----------------------------------------------------------------------
	// Summary:
	//     This method is called to draw background of the gallery
	// Parameters:
	//      pDC - Pointer to device context
	//      pGallery - CXTPControlGallery object to draw
	//      rc - Bounding rectangle of gallery object
	//-----------------------------------------------------------------------
	virtual void FillControl(CDC* pDC, CXTPControlGallery* pGallery, CRect rc);

	//-----------------------------------------------------------------------
	// Summary:
	//     This method called to draw scrollbar of inplace gallery
	// Parameters:
	//     pDC - Pointer to device context
	//     pGallery - CXTPControlGallery object to draw
	// See Also: DrawScrollBar
	//-----------------------------------------------------------------------
	virtual void DrawPopupScrollBar(CDC* pDC, CXTPControlGallery* pGallery);

	//-----------------------------------------------------------------------
	// Summary:
	//     This method is called to draw label control
	// Parameters:
	//     pDC - Pointer to device context
	//     pLabel - Label item
	//     rc - Bounding rectangle of the label
	//-----------------------------------------------------------------------
	virtual void DrawLabel(CDC* pDC, CXTPControlGalleryItem* pLabel, CRect rc);

	//-----------------------------------------------------------------------
	// Summary:
	//     This method is called to refresh the visual metrics of the manager.
	//-----------------------------------------------------------------------
	virtual void RefreshMetrics();

protected:
	COLORREF m_clrControlGallerySelected;   // Selected item color.
	COLORREF m_clrControlGalleryNormal;     // Normal item color.
	COLORREF m_clrControlGalleryBorder;     // Border color.

};


#include "Common/Base/Diagnostic/XTPEnableNoisyWarnings.h"
#endif // !defined(__XTPCONTROLGALLERYRESOURCETHEME_H__)
