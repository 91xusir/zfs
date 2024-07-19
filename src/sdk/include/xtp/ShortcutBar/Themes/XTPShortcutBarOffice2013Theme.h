// XTPShortcutBarOffice2013Theme.h interface for the CXTPShortcutBarOffice2013Theme class.
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
#if !defined(__XTPSHORTCUTBAROFFICE2013THEME_H__)
#define __XTPSHORTCUTBAROFFICE2013THEME_H__
//}}AFX_CODEJOCK_PRIVATE

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Common/Base/Diagnostic/XTPDisableNoisyWarnings.h"

//===========================================================================
// Summary:
//     CXTPShortcutBarOffice2013Theme is a CXTPShortcutBarResourceTheme derived
//     class that represents the Office 2003 shortcut bar theme.
//===========================================================================
class _XTP_EXT_CLASS CXTPShortcutBarOffice2013Theme : public CXTPShortcutBarResourceTheme
{
public:

	//-----------------------------------------------------------------------
	// Summary: 
	//-----------------------------------------------------------------------
	CXTPShortcutBarOffice2013Theme();

	//-----------------------------------------------------------------------
	// Summary: 
	//-----------------------------------------------------------------------
	virtual ~CXTPShortcutBarOffice2013Theme();

	//-----------------------------------------------------------------------
	// Summary: 
	//-----------------------------------------------------------------------
	virtual void RefreshMetrics();

	//-----------------------------------------------------------------------
	// Summary:
	//     This method is called to draw the caption of the specified shortcut
	//     pane item.  This is the sub-pane of a shortcut bar pane.
	// Parameters:
	//     pDC - Points to the client device context.
	//     pItem - Shortcut item pane to be drawn
	//     bDraw - bDraw - TRUE to draw, FALSE to determine size of the shortcut
	//             pane caption.
	// Remarks:
	//     Panes of the shortcut bar are displayed in the shortcut bar's client
	//     area.  Each pane consists of a caption and one or more sub-panes.
	//     Each sub-pane has it's own caption and is drawn with the
	//     DrawPaneItemCaption method.
	//
	//     The caption of the pane item is set when the pane item is created in the
	//     CXTPShortcutBarPane::AddItem method.
	//
	//     The sub-items of a pane can be retrieved with the CXTPShortcutBarPane::GetItem
	//     method.
	// Returns:
	//     Size of the pane item caption.  This is retrieved from m_nPaneItemHeight.
	//-----------------------------------------------------------------------
	virtual int DrawPaneItemCaption(CDC* pDC, CXTPShortcutBarPaneItem* pItem, BOOL bDraw);

	//-----------------------------------------------------------------------
	// Summary:
	//     This method is called to draw the expand button.
	// Parameters:
	//     pDC - Points to the client device context.
	//     rc - Client rectangle of the expand button.
	//-----------------------------------------------------------------------
	virtual void DrawExpandButton(CDC* pDC, CRect rc);

	virtual void DrawPaneCaptionMinimizeButton(CDC* pDC, CXTPShortcutBarPane* pPane);

	//-----------------------------------------------------------------------
	// Summary:
	//     This method is called to draw gripper of the ShortcutBar control
	// Parameters:
	//     pDC - Points to the client device context.
	//     pShortcutBar - Points to the ShortcutBar control
	//     bDraw - TRUE to draw, FALSE to determine size of the gripper.
	// Returns:
	//     Height of the gripper.
	//-----------------------------------------------------------------------
	virtual int DrawGripper(CDC* pDC, CXTPShortcutBar* pShortcutBar, BOOL bDraw);

	//-----------------------------------------------------------------------
	// Summary:
	//     This method is called to draw the specified item of the ShortcutBar control.
	// Parameters:
	//     pDC - Points to the client device context.
	//     pItem - Item to be drawn.
	//-----------------------------------------------------------------------
	void FillShortcutItemEntry(CDC* pDC, CXTPShortcutBarItem* pItem);

	//-----------------------------------------------------------------------
	// Summary:
	//     This method is called to draw navigation item of minimized ShortcutBar
	// Parameters:
	//     pDC - Points to the client device context.
	//     pItem - Shortcut item to be drawn
	//-----------------------------------------------------------------------
	virtual void FillNavigationItemEntry(CDC* pDC, CXTPShortcutBarItem* pItem);

	virtual void FillPopupBack(CDC* pDC, CRect rc, CSize szGap);
	
	//-----------------------------------------------------------------------
	// Summary:
	//     This method is called to fill the client area of the ShortcutBar control.
	// Parameters:
	//     pDC - Points to the client device context.
	//     pShortcutBar - Points to the ShortcutBar control.
	//-----------------------------------------------------------------------
	virtual void FillShortcutBar(CDC* pDC, CXTPShortcutBar* pShortcutBar);

protected:

	CXTPPaintManagerColor m_clrBack;
	CXTPPaintManagerColor m_clrBackHilite;
	CXTPPaintManagerColor m_clrBackPushed;
	CXTPPaintManagerColor m_clrBackPushedDark;
	CXTPPaintManagerColor m_clrBackSelected;
	CXTPPaintManagerColor m_clrGlyph;         // Normal glyph color.
	CXTPPaintManagerColor m_clrGlyphFill;     // Expanded glyph fill color.
	CXTPPaintManagerColor m_clrGlyphExpanded; // Expanded glyph border color.
	CXTPPaintManagerColor m_clrGlyphPushed;   // Pushed glyph color.
};

#include "Common/Base/Diagnostic/XTPEnableNoisyWarnings.h"
#endif // !defined(__XTPSHORTCUTBAROFFICE2013THEME_H__)
