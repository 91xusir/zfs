// XTPCalendarCaptionBarOffice2000Theme.h interface for the CXTPCalendarCaptionBarTheme class.
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
#if !defined(__XTPCALENDARCAPTIONBAROFFICE2000THEME_H__)
#define __XTPCALENDARCAPTIONBAROFFICE2000THEME_H__
//}}AFX_CODEJOCK_PRIVATE

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Common/Base/Diagnostic/XTPDisableNoisyWarnings.h"

//===========================================================================
// Summary:
//     CXTPCalendarCaptionBarOffice2000Theme is a CXTPCalendarCaptionBarPaintManager derived
//     class thatrepresents the Office 2000 calendar theme.
//===========================================================================
class _XTP_EXT_CLASS CXTPCalendarCaptionBarOffice2000Theme : public CXTPCalendarCaptionBarPaintManager
{
public:
	//-----------------------------------------------------------------------
	// Summary:
	//     Default paint manager constructor.
	// Remarks:
	//     Handles initial initialization.
	// See Also: RefreshMetrics()
	//-----------------------------------------------------------------------
	CXTPCalendarCaptionBarOffice2000Theme(CXTPCalendarCaptionBarTheme* pTheme = NULL);

	//-----------------------------------------------------------------------
	// Summary:
	//     Default paint manager destructor.
	// Remarks:
	//     Handles member items deallocation.
	//-----------------------------------------------------------------------
	virtual ~CXTPCalendarCaptionBarOffice2000Theme();

	//-------------------------------------------------------------------
	// Summary:
	//     Performs refreshing of graphical related parameters from
	//     system settings. Base implementation do nothing.
	//-------------------------------------------------------------------
	virtual void RefreshMetrics();

	virtual void PartRefresh(CXTPSwitchViewButtonPart* pBarPart);
	virtual void PartRefresh(CXTPRadioButtonPart* pBarPart);
	virtual void PartRefresh(CXTPCheckboxButtonPart* pBarPart);
	virtual void PartRefresh(CXTPDateLabelPart* pBarPart);

	virtual void PartAdjustLayout(CXTPSwitchViewButtonPart* pBarPart, CDC* pDC, const CRect& rcRect);
	virtual void PartAdjustLayout(CXTPScrollDateButtonPart* pBarPart, CDC* /*pDC*/, const CRect& rcRect);
	virtual void PartAdjustLayout(CXTPRadioButtonPart* pBarPart, CDC* pDC, const CRect& rcRect);
	virtual void PartAdjustLayout(CXTPCheckboxButtonPart* pBarPart, CDC* pDC, const CRect& rcRect);
	virtual void PartAdjustLayout(CXTPExpandButtonPart* pBarPart, CDC* /*pDC*/, const CRect& rcRect);

	virtual void PartDraw(CXTPSwitchViewButtonPart* pBarPart, CDC* pDC);
	virtual void PartDraw(CXTPScrollDateButtonPart* pBarPart, CDC* pDC);
	virtual void PartDrawScrollTriangle(CXTPScrollDateButtonPart* pBarPart, CDC* pDC, CRect rcRect, BOOL bLeftDirection, COLORREF clrColor);
	virtual void PartDraw(CXTPRadioButtonPart* pBarPart, CDC* pDC);
	virtual void PartDraw(CXTPCheckboxButtonPart* pBarPart, CDC* pDC);
	virtual void PartDraw(CXTPDateLabelPart* pBarPart, CDC* pDC);
	virtual void PartDraw(CXTPExpandButtonPart* pBarPart, CDC* pDC);
	virtual void PartDrawExpandTriangle(CXTPExpandButtonPart* pBarPart, CDC* pDC, CRect rcRect, BOOL bDownDirection, COLORREF clrColor);
};


#include "Common/Base/Diagnostic/XTPEnableNoisyWarnings.h"
#endif // !defined(__XTPCALENDARCAPTIONBAROFFICE2000THEME_H__)
