// XTPCalendarOffice2003Theme.h interface for the CXTPCalendarOffice2000Theme class.
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
#if !defined(__XTPCALENDAROFFICE2003THEME_H__)
#define __XTPCALENDAROFFICE2003THEME_H__
//}}AFX_CODEJOCK_PRIVATE

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Common/Base/Diagnostic/XTPDisableNoisyWarnings.h"

//===========================================================================
// Summary:
//     CXTPCalendarOffice2003Theme is a CXTPCalendarOffice2000Theme derived
//     class that represents the Office 2000 calendar theme.
//===========================================================================
class _XTP_EXT_CLASS CXTPCalendarOffice2003Theme : public CXTPCalendarOffice2000Theme
{
public:

	//-----------------------------------------------------------------------
	// Summary:
	//     Default paint manager constructor.
	// Remarks:
	//     Handles initial initialization.
	// See Also: RefreshMetrics()
	//-----------------------------------------------------------------------
	CXTPCalendarOffice2003Theme();

	//-----------------------------------------------------------------------
	// Summary:
	//     Default paint manager destructor.
	// Remarks:
	//     Handles member items deallocation.
	//-----------------------------------------------------------------------
	virtual ~CXTPCalendarOffice2003Theme();

	//-----------------------------------------------------------------------
	// Summary:
	//     This member function is used to initialize all drawing defaults.
	// Remarks:
	//     Initializes all drawings defaults (fonts, colors, etc.).
	//     most of defaults are system defaults.
	//-----------------------------------------------------------------------
	virtual void RefreshMetrics();

	virtual void PartRefresh(CWeekViewPart* pViewPart);
	virtual void PartRefresh(CDayViewWorkCellPart* pViewPart);
	virtual void PartRefresh(CDayViewNonworkCellPart* pViewPart);
	virtual void PartRefresh(CMonthViewGridPart* pViewPart);

	virtual void PartDraw(CDayViewCellPart* pViewPart, CDC* pDC, CXTPCalendarDayViewGroup* pViewGroup, CRect rc, const XTP_CALENDAR_DAYVIEWCELL_PARAMS& cellParams);
	virtual void PartDrawDayHeader(CWeekViewPart* pViewPart, CDC* pDC, CRect rcDay, int nHeaderHeight, CString strHeader, BOOL bIsCurrent, BOOL bIsSelected);
	virtual void PartDrawDayDate(CMonthViewEventPart* pViewPart, CDC* pDC, CRect rc, BOOL bToday, BOOL bSelected, BOOL bFirstMonthDay, CString strText);

	using CXTPCalendarOffice2000Theme::PartRefresh;
	using CXTPCalendarOffice2000Theme::PartDraw;
};


#include "Common/Base/Diagnostic/XTPEnableNoisyWarnings.h"
#endif // !defined(__XTPCALENDAROFFICE2003THEME_H__)
