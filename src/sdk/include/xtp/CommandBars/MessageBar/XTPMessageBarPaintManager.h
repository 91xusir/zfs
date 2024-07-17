// XTPMessageBarPaintManager.h
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
#if !defined(__XTPMESSAGEBARPAINTMANAGER_H__)
#define __XTPMESSAGEBARPAINTMANAGER_H__
//}}AFX_CODEJOCK_PRIVATE

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Common/Base/Diagnostic/XTPDisableNoisyWarnings.h"

//===========================================================================
// Summary: The CXTPMessageBarPaintManager class is used by the message bar
//          to perform standard paint routines.
//===========================================================================
class _XTP_EXT_CLASS CXTPMessageBarPaintManager
{
public:

	//-----------------------------------------------------------------------
	// Input:   pPaintManager - Pointer to the owner paint manager.
	// Summary: Message bar constructor.
	//-----------------------------------------------------------------------
	CXTPMessageBarPaintManager(CXTPPaintManager* pPaintManager);

	//-----------------------------------------------------------------------
	// Summary: Call to update class colors and settings.
	//-----------------------------------------------------------------------
	virtual void RefreshMetrics();
	
	//-----------------------------------------------------------------------
	// Input:   pDC - Pointer to a valid device context.
	//          pBar - Points to a CXTPMessageBar object.
	// Summary: Draws the message bar.
	//-----------------------------------------------------------------------
	virtual void FillMessageBar(CDC* pDC, CXTPMessageBar* pBar);

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

	//-----------------------------------------------------------------------
	// Summary: Returns paint manager specific default button size value.
	//-----------------------------------------------------------------------
	virtual CSize GetDefaultButtonSize() const;

	//-----------------------------------------------------------------------
	// Summary: Returns paint manager specific bar height value.
	//-----------------------------------------------------------------------
	virtual int GetHeight() const;

	//-----------------------------------------------------------------------
	// Summary: Returns paint manager specific bar padding value.
	//-----------------------------------------------------------------------
	virtual CRect GetPadding() const;

protected:
	CXTPPaintManagerColor m_clrBack;       // Message bar background color.
	CXTPPaintManagerColor m_clrText;       // Message bar text color.
	CXTPPaintManagerColor m_clrBorder;     // Message bar border color.
	CXTPPaintManagerColor m_clrInfoBack;   // Message bar info background color.
	CXTPPaintManager*     m_pPaintManager; // Pointer to the owner paint manager.

private:

	friend class CXTPPaintManager;
};


#include "Common/Base/Diagnostic/XTPEnableNoisyWarnings.h"
#endif // !defined(__XTPMESSAGEBARPAINTMANAGER_H__)
