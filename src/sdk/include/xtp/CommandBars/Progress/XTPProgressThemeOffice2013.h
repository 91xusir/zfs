// XTPProgressThemeOffice2013.h
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
#if !defined(__XTPPROGRESSTHEMEOFFICE2013_H__)
#define __XTPPROGRESSTHEMEOFFICE2013_H__
//}}AFX_CODEJOCK_PRIVATE

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Common/Base/Diagnostic/XTPDisableNoisyWarnings.h"

class CXTPProgressBase;
class CXTPPaintManager;

//===========================================================================
// Summary: CXTPProgressThemeOffice2013 object
//===========================================================================
class _XTP_EXT_CLASS CXTPProgressThemeOffice2013 : public CXTPProgressPaintManager
{
public:

	//-----------------------------------------------------------------------
	// Summary:
	//     Constructs a CXTPProgressThemeOffice2013 object
	// Parameters:
	//     pPaintManager - PaintManager of commandbars
	//-----------------------------------------------------------------------
	CXTPProgressThemeOffice2013(CXTPPaintManager* pPaintManager);

	//-----------------------------------------------------------------------
	// Summary:
	//     Destroys a CXTPProgressThemeOffice2013 object, handles cleanup and deallocation
	//-----------------------------------------------------------------------
	virtual ~CXTPProgressThemeOffice2013();

	//-----------------------------------------------------------------------
	// Summary:
	//     This method is called to draw progress
	// Parameters:
	//     pDC - Pointer to device context
	//     pProgressBar - Progress to draw
	//-----------------------------------------------------------------------
	virtual void DrawProgress(CDC* pDC, CXTPProgressBase* pProgressBar);

	//-------------------------------------------------------------------------
	// Summary:
	//     Recalculates metrics
	//-------------------------------------------------------------------------
	virtual void RefreshMetrics();

protected:

	CXTPPaintManagerColor m_clrBorder;    // RGB value representing border color.
	CXTPPaintManagerColor m_clrBar;       // RGB value representing bar color.
	CXTPPaintManagerColor m_clrBack;      // RGB value representing back color.
};


#include "Common/Base/Diagnostic/XTPEnableNoisyWarnings.h"
#endif // !defined(__XTPPROGRESSTHEMEOFFICE2013_H__)
