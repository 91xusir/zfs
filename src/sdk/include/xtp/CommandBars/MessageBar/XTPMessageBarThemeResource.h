// XTPMessageBarThemeResource.h
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
#if !defined(__XTPMESSAGEBARTHEMERESOURCE_H__)
#define __XTPMESSAGEBARTHEMERESOURCE_H__
//}}AFX_CODEJOCK_PRIVATE

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Common/Base/Diagnostic/XTPDisableNoisyWarnings.h"

class CXTPResourceTheme;
class CXTPMessageBar;
class CXTPMessageBarButton;

class _XTP_EXT_CLASS CXTPMessageBarThemeResource : public CXTPMessageBarPaintManager
{
public:
	CXTPMessageBarThemeResource(CXTPResourceTheme *pTheme);

	//-----------------------------------------------------------------------
	// Summary: Draws the message bar.
	// Input:   pDC - Pointer to a valid device context.
	//          pBar - Pointer to the message bar to draw.
	//-----------------------------------------------------------------------
	virtual void FillMessageBar(CDC* pDC, CXTPMessageBar* pBar);

	//-----------------------------------------------------------------------
	// Summary: Call this member to draw a button in the message bar.
	// Input:   pDC - Pointer to a valid device context.
	//          pButton - Button to draw.
	//-----------------------------------------------------------------------
	virtual void DrawMessageBarButton(CDC* pDC, CXTPMessageBarButton* pButton);

	//-----------------------------------------------------------------------
	// Summary: Returns paint manager specific default button size value.
	//-----------------------------------------------------------------------
	virtual CSize GetDefaultButtonSize() const;

	//-----------------------------------------------------------------------
	// Summary: Returns paint manager specific bar height value.
	//-----------------------------------------------------------------------
	virtual int GetHeight() const;

protected:
	CXTPResourceTheme *m_pTheme;
};


#include "Common/Base/Diagnostic/XTPEnableNoisyWarnings.h"
#endif // !defined(__XTPMESSAGEBARTHEMERESOURCE_H__)
