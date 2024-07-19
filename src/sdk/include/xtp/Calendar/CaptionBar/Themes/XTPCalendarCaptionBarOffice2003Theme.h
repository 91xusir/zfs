// XTPCalendarCaptionBarOffice2003Theme.h interface for the CXTPCalendarCaptionBarOfficeXPTheme class.
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
#if !defined(__XTPCALENDARCAPTIONBAROFFICE2003THEME_H__)
#define __XTPCALENDARCAPTIONBAROFFICE2003THEME_H__
//}}AFX_CODEJOCK_PRIVATE

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Common/Base/Diagnostic/XTPDisableNoisyWarnings.h"

//===========================================================================
// Summary:
//     CXTPCalendarCaptionBarOffice2003Theme is a CXTPCalendarCaptionBarOfficeXPTheme derived
//     class that represents the Office 2000 calendar theme.
//===========================================================================
class _XTP_EXT_CLASS CXTPCalendarCaptionBarOffice2003Theme : public CXTPCalendarCaptionBarOfficeXPTheme
{
public:
	//-----------------------------------------------------------------------
	// Summary:
	//     Default paint manager constructor.
	// Remarks:
	//     Handles initial initialization.
	// See Also: RefreshMetrics()
	//-----------------------------------------------------------------------
	CXTPCalendarCaptionBarOffice2003Theme(CXTPCalendarCaptionBarTheme* pTheme = NULL);

	//-----------------------------------------------------------------------
	// Summary:
	//     Default paint manager destructor.
	// Remarks:
	//     Handles member items deallocation.
	//-----------------------------------------------------------------------
	virtual ~CXTPCalendarCaptionBarOffice2003Theme();

	virtual void PartDraw(CXTPSwitchViewButtonPart* pBarPart, CDC* pDC);
	virtual void PartDraw(CXTPScrollDateButtonPart* pBarPart, CDC* pDC);
	virtual void PartDraw(CXTPRadioButtonPart* pBarPart, CDC* pDC);
	virtual void PartDraw(CXTPCheckboxButtonPart* pBarPart, CDC* pDC);
	virtual void PartDraw(CXTPExpandButtonPart* pBarPart, CDC* pDC);
};


#include "Common/Base/Diagnostic/XTPEnableNoisyWarnings.h"
#endif // !defined(__XTPCALENDARCAPTIONBAROFFICE2003THEME_H__)
