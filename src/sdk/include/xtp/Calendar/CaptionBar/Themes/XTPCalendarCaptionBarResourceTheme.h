// XTPCalendarCaptionBarResourceTheme.h interface for the CXTPCalendarCaptionBarOffice2013Theme class.
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
#if !defined(__XTPCALENDARCAPTIONBARRESOURCETHEME_H__)
#define __XTPCALENDARCAPTIONBARRESOURCETHEME_H__
//}}AFX_CODEJOCK_PRIVATE

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Common/Base/Diagnostic/XTPDisableNoisyWarnings.h"

//===========================================================================
// Summary:
//     CXTPCalendarCaptionBarResourceTheme is a CXTPCalendarCaptionBarOffice2013Theme derived
//     class that represents the Office 2000 calendar theme.
//===========================================================================
class _XTP_EXT_CLASS CXTPCalendarCaptionBarResourceTheme : public CXTPCalendarCaptionBarOffice2013Theme
{
public:
	//-----------------------------------------------------------------------
	// Summary:
	//     Default paint manager constructor.
	// Remarks:
	//     Handles initial initialization.
	// See Also: RefreshMetrics()
	//-----------------------------------------------------------------------
	CXTPCalendarCaptionBarResourceTheme(CXTPCalendarCaptionBarTheme* pTheme = NULL);

	//-----------------------------------------------------------------------
	// Summary:
	//     Default paint manager destructor.
	// Remarks:
	//     Handles member items deallocation.
	//-----------------------------------------------------------------------
	virtual ~CXTPCalendarCaptionBarResourceTheme();

	//-------------------------------------------------------------------
	// Summary:
	//     Performs refreshing of graphical related parameters from
	//     system settings. Base implementation do nothing.
	//-------------------------------------------------------------------
	virtual void RefreshMetrics();

	// ----------------------------------------------------------------------
	// Summary:
	//     This member function is used to read color by name from resources.
	// Parameters:
	//     pcszColorName :  string with Color Name
	//     clrDefault :     COLORREF of default color
	// Returns:
	//     A RGB color value.
	// ----------------------------------------------------------------------
	virtual COLORREF GetColor2(LPCTSTR pcszColorName, COLORREF clrDefault);

	virtual void PartAdjustLayout(CXTPExpandButtonPart* pBarPart, CDC* /*pDC*/, const CRect& rcRect);
	virtual void PartDraw(CXTPSwitchViewButtonPart* pBarPart, CDC* pDC);
	virtual void PartDraw(CXTPScrollDateButtonPart* pBarPart, CDC* pDC);
	virtual void PartDraw(CXTPDateLabelPart* pBarPart, CDC* pDC);
	virtual void PartDraw(CXTPExpandButtonPart* pBarPart, CDC* pDC);

protected:
	virtual CXTPResourceImage* GetResourceImage(LPCTSTR lpszImageFile);
};


#include "Common/Base/Diagnostic/XTPEnableNoisyWarnings.h"
#endif // !defined(__XTPCALENDARCAPTIONBARRESOURCETHEME_H__)
