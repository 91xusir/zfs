// XTPMessageBarThemeOffice2013.h
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
#if !defined(__XTPMESSAGEBARTHEMEOFFICE2013_H__)
#define __XTPMESSAGEBARTHEMEOFFICE2013_H__
//}}AFX_CODEJOCK_PRIVATE

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Common/Base/Diagnostic/XTPDisableNoisyWarnings.h"

class CXTPPaintManager;
class CXTPMessageBar;
class CXTPMessageBarButton;

//===========================================================================
// Summary: The CXTPMessageBarThemeOffice2013 class is used by the message bar
//          to draw an Office 2103 style message bar.
//===========================================================================
class _XTP_EXT_CLASS CXTPMessageBarThemeOffice2013 : public CXTPMessageBarPaintManager
{
public:

	//-----------------------------------------------------------------------
	// Input:   pPaintManager - Pointer to the owner paint manager.
	// Summary: Message bar constructor.
	//-----------------------------------------------------------------------
	CXTPMessageBarThemeOffice2013(CXTPPaintManager *pPaintManager);

	//-----------------------------------------------------------------------
	// Summary: Call to update class colors and settings.
	//-----------------------------------------------------------------------
	virtual void RefreshMetrics();
	
	//-----------------------------------------------------------------------
	// Input:   pDC - Pointer to a valid device context.
	//          pBar - Points to a CXTPMessageBar object.
	// Summary: Draws the message bar.
	//-----------------------------------------------------------------------
	virtual void FillMessageBar(CDC *pDC, CXTPMessageBar *pBar);

	//-----------------------------------------------------------------------
	// Input:   pDC - Pointer to a valid device context.
	//          pButton - Button to draw.
	// Summary: Call this member to draw a button in the message bar.
	//-----------------------------------------------------------------------
	virtual void DrawMessageBarButton(CDC* pDC, CXTPMessageBarButton* pButton);

	//-----------------------------------------------------------------------
	// Input:   x - Specifies the left position.
	//          y - Specifies the right position.
	//          cy - Specifies the height of the client area.
	// Summary: Call this member to retrieve the close button coordinates.
	// Returns: A CRect object containing the location of the close button.
	//-----------------------------------------------------------------------
	virtual CRect GetCloseButtonRect(int x, int y, int cy);

protected:
	
	CRect m_rcBorders;
	CXTPPaintManagerColor m_clrTextHilite;         // Message bar text hilite color.
	CXTPPaintManagerColor m_clrButtonBack;         // Message bar button background color.
	CXTPPaintManagerColor m_clrButtonBackHilite;   // Message bar button background hilite color.
	CXTPPaintManagerColor m_clrButtonBackPushed;   // Message bar button background pushed color.
	CXTPPaintManagerColor m_clrButtonBorder;       // Message bar button border color.
	CXTPPaintManagerColor m_clrButtonBorderHilite; // Message bar button border hilite color.
	CXTPPaintManagerColor m_clrButtonBorderPushed; // Message bar button border pushed color.

private:

	friend class CXTPPaintManager;
};


#include "Common/Base/Diagnostic/XTPEnableNoisyWarnings.h"
#endif // !defined(__XTPMESSAGEBARTHEMEOFFICE2013_H__)
