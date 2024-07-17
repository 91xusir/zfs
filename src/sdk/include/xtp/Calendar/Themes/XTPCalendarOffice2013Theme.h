// XTPCalendarOffice2013Theme.h interface for the CXTPCalendarOffice2000Theme class.
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
#if !defined(__XTPCALENDAROFFICE2013THEME_H__)
#define __XTPCALENDAROFFICE2013THEME_H__
//}}AFX_CODEJOCK_PRIVATE

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Common/Base/Diagnostic/XTPDisableNoisyWarnings.h"

//===========================================================================
// Summary:
//     CXTPCalendarOffice2013Theme is a CXTPCalendarOffice2000Theme derived
//     class that represents the Office 2000 calendar theme.
//===========================================================================
class _XTP_EXT_CLASS CXTPCalendarOffice2013Theme : public CXTPCalendarOffice2000Theme
{
public:

	//-----------------------------------------------------------------------
	// Summary:
	//     Default paint manager constructor.
	// Remarks:
	//     Handles initial initialization.
	// See Also: RefreshMetrics()
	//-----------------------------------------------------------------------
	CXTPCalendarOffice2013Theme();

	//-----------------------------------------------------------------------
	// Summary:
	//     Default paint manager destructor.
	// Remarks:
	//     Handles member items deallocation.
	//-----------------------------------------------------------------------
	virtual ~CXTPCalendarOffice2013Theme();

	//-----------------------------------------------------------------------
	// Summary:
	//     This member function is used to initialize all drawing defaults.
	// Remarks:
	//     Initializes all drawings defaults (fonts, colors, etc.).
	//     most of defaults are system defaults.
	//-----------------------------------------------------------------------
	virtual void RefreshMetrics();

	//-----------------------------------------------------------------------
	// Summary:
	//     This member function is used to draw the Header.
	// Parameters:
	//     pDC             - Pointer to a valid device context.
	//     rcHeader        - A CRect that contains the bounding rectangle coordinates
	//                       used to draw the header.
	//     bIsSelected     - A BOOL. TRUE if the header is selected. FALSE otherwise.
	//     bIsCurrent      - A BOOL. TRUE if the header represents the current time.
	//                       FALSE otherwise.
	//     grclr           - A CXTPPaintManagerColorGradient object that specifies the Gradient color.
	//     clrUnderLineHdr - A COLORREF object that specifies the color of header's underline.
	//-----------------------------------------------------------------------
	virtual void DrawHeader(CDC* pDC, CRect& rcHeader, BOOL bIsSelected, BOOL bIsCurrent, const CXTPPaintManagerColorGradient& grclr, COLORREF clrUnderLineHdr);

	using CXTPCalendarOffice2000Theme::DrawHeader;

	//-----------------------------------------------------------------------
	// Summary:
	//     This member function is called to update event label default colors
	//-----------------------------------------------------------------------
	virtual void UpdateEventLabelsDefaultColors();

	//-----------------------------------------------------------------------
	// Summary:
	//     This member function is called to update event category default colors
	//-----------------------------------------------------------------------
	virtual void UpdateEventCategoriesDefaultColors();

	//-----------------------------------------------------------------------
	// Summary:
	//     Initializes Calendar images.
	// Remarks:
	//     Called by the paint manager to initialize images used by the 
	//     Calendar control.
	//-----------------------------------------------------------------------
	virtual void UpdateGlyphs();

	//-----------------------------------------------------------------------
	// Summary:    Call this member function to get the background color for
	//             the specified event.
	// Parameters: pEvent : Points to the Calendar event.
	// Returns:    An RGB value representing the event background color.
	//-----------------------------------------------------------------------
	virtual COLORREF GetEventBackColor(CXTPCalendarEvent* pEvent);

	using CXTPCalendarOffice2000Theme::GetEventBackColor;

	//-----------------------------------------------------------------------
	// Summary:    Call this member function to get the edge color for
	//             the specified event.
	// Parameters: pEvent : Points to the Calendar event.
	// Returns:    An RGB value representing the event edge color.
	//-----------------------------------------------------------------------
	virtual COLORREF GetEventEdgeColor(CXTPCalendarEvent* pEvent);

	virtual void DrawCategories(CDC* pDC, CRect& rcEvent, CXTPCalendarEvent* pEvent);

	virtual CXTPCalendarViewEventSubjectEditor* StartEditSubject(CXTPCalendarViewEvent* pViewEvent, CRect& rcEditor);
	virtual void SetEditFont(CXTPCalendarViewEvent* pViewEvent, CXTPCalendarViewEventSubjectEditor* pEditor);
	virtual void GetDateFormat(CString& strShort, CString& strLong);
	virtual void CreateBoldFont(CFont* pFont, CFont* pFontBold);
	virtual void CreateHeaderFont(CDC* pDC, CFont* pFont, CFont* pFontNew, BOOL bBoldFont);
	virtual CSize GetTextSize(CDC* pDC, CString strText, CFont* pFont);
	virtual void FormatLabel(CString& strLabel);

	virtual void DrawMultiDayArrowL(CDC* pDC, int nL, CRect& rcText, COleDateTime& dtDate, CFont& font);
	virtual void DrawMultiDayArrowR(CDC* pDC, int nR, CRect& rcText, COleDateTime& dtDate, CFont& font);
	virtual void DrawMultiDayEvent(CDayViewEventPart* pViewPart, CDC* pDC, CXTPCalendarDayViewEvent* pViewEvent, CRect& rcText);
	virtual void DrawSingleDayEvent(CDayViewEventPart* pViewPart, CDC* pDC, CXTPCalendarDayViewEvent* pViewEvent, CRect& rcText);
	virtual void DrawExpandSign(CDC* pDC, BOOL bExpandUp, const CRect& rcTSHours);

	virtual void PartRefresh(CControlPart* pViewPart);
	virtual void PartRefresh(CDayViewEventPart* pViewPart);
	virtual void PartRefresh(CDayViewTimeScaleCellPart* pViewPart);
	virtual void PartRefresh(CDayViewTimeScaleHeaderPart* pViewPart);
	virtual void PartRefresh(CDayViewAllDayEventsPart* pViewPart);
	virtual void PartRefresh(CDayViewHeaderPart* pViewPart);
	virtual void PartRefresh(CWeekViewPart* pViewPart);
	virtual void PartRefresh(CMonthViewHeaderPart* pViewPart);
	virtual void PartRefresh(CMonthViewEventPart* pViewPart);
	virtual void PartRefresh(CDayViewWorkCellPart* pViewPart);
	virtual void PartRefresh(CDayViewNonworkCellPart* pViewPart);
	virtual void PartRefresh(CMonthViewGridPart* pViewPart);
	virtual void PartRefresh(CTimeLineViewPart* pViewPart);

	using CXTPCalendarOffice2000Theme::PartRefresh;

	virtual BOOL ShowMultiDayTime(CXTPCalendarViewEvent* pViewEvent, CXTPCalendarViewPart* pViewPart);

	virtual void PartDrawEventSelectedBorder(CDC* pDC, CRect rcBorder, int l, int t, int r, int b);
	virtual void PartDrawEventBusy(CXTPCalendarViewEvent* pViewEvent, CXTPCalendarViewPart* pViewPart, CDC* pDC, CRect& rcEvent);
	virtual void PartDrawEventBack(CXTPCalendarViewEvent* pViewEvent, CXTPCalendarViewPart* pViewPart, CDC* pDC, CRect& rcEvent);
	virtual void PartDrawEventTime(CXTPCalendarViewEvent* pViewEvent, CXTPCalendarViewPart* pViewPart, CDC* pDC, CRect& rcEvent, CRect& rcText, BOOL bTimeAsClock);
	virtual void PartDrawEventText(CXTPCalendarViewEvent* pViewEvent, CXTPCalendarViewPart* pViewPart, CDC* pDC, CRect& rcEvent, CRect& rcText, int nViewDayIndex);

	virtual void PartDraw(CDayViewEventPart* pViewPart, CDC* pDC, CXTPCalendarDayViewEvent* pViewEvent);
	virtual void PartDrawHourCell(CDayViewTimeScaleCellPart* pViewPart, CDC* pDC, CRect rc, CString strText, BOOL bFillBackground, BOOL bCurrentTime);
	virtual void PartDrawBigHourCell(CDayViewTimeScaleCellPart* pViewPart, CDC* pDC, CRect rc, CString strHour, CString strMin, int nRowPerHour, BOOL bFillBackground, BOOL bCurrentTime);
	virtual void PartDraw(CDayViewTimeScaleHeaderPart* pViewPart, CDC* pDC, CXTPCalendarDayView* /*pView*/, CRect rc, CString strText);
	virtual void PartDrawNowLine(CDayViewTimeScaleHeaderPart* pViewPart, CDC* pDC, CXTPCalendarDayView* pView, CRect rc, int y, BOOL bDrawBk);
	virtual void PartDraw(CDayViewCellPart* pViewPart, CDC* pDC, CXTPCalendarDayViewGroup* pViewGroup, CRect rc, const XTP_CALENDAR_DAYVIEWCELL_PARAMS& cellParams);
	virtual void PartDraw(CDayViewAllDayEventsPart* pViewPart, CDC* pDC, CXTPCalendarDayViewGroup* pViewGroup, CRect rc, BOOL bSelected);
	virtual void PartDraw(CDayViewHeaderPart* pViewPart, CDC* pDC, CXTPCalendarDayViewDay* pViewDay, CRect rc, CString strText);
	virtual void PartDraw(CWeekViewPart* pViewPart, CDC* pDC, CXTPCalendarWeekView* pWeekView);
	virtual void PartDrawDayHeader(CWeekViewPart* pViewPart, CDC* pDC, CRect rcDay, int nHeaderHeight, CString strHeader, BOOL bIsCurrent, BOOL bIsSelected);
	virtual void PartDraw(CWeekViewEventPart* pViewPart, CDC* pDC, CXTPCalendarWeekViewEvent* pWeekViewEvent);
	virtual void PartDrawGrid(CMonthViewGridPart* pViewPart, CDC* pDC, CXTPCalendarMonthView* pMonthView);
	virtual void PartDraw(CMonthViewHeaderPart* pViewPart, CDC* pDC, CXTPCalendarMonthView* pMonthView, CRect rc, int nCollIndex, CString strText);
	virtual void PartDrawEvent(CMonthViewEventPart* pViewPart, CDC* pDC, CXTPCalendarMonthViewEvent* pViewEvent);
	virtual void PartDrawDayDate(CMonthViewEventPart* pViewPart, CDC* pDC, CRect rc, BOOL bToday, BOOL bSelected, BOOL bFirstMonthDay, CString strText);
	virtual void PartAdjustTimeFont(CDayViewTimeScaleCellPart* pViewPart, CDC* pDC, CRect rcCell);

	using CXTPCalendarOffice2000Theme::PartDraw;
	using CXTPCalendarOffice2000Theme::PartDrawEvent;

protected:
	int m_nBusyWidth;
	int m_nPadding;
	CString m_strThemeName;

	CXTPFont m_xtpFontBold;
	CXTPFont m_xtpFontSel;
	
	XTP_SUBSTITUTE_GDI_MEMBER_WITH_CACHED(CFont, m_fontBold, m_xtpFontBold, GetBoldFontHandle);
	XTP_SUBSTITUTE_GDI_MEMBER_WITH_CACHED(CFont, m_fontSel, m_xtpFontSel, GetSelFontHandle);

	CXTPPaintManagerColor m_clrHeaderText;
	CXTPPaintManagerColor m_clrHeaderBack;
	CXTPPaintManagerColor m_clrViewBack;
	CXTPPaintManagerColor m_clrHeaderTextSelected;
	CXTPPaintManagerColor m_clrHeaderBackSelected;
	CXTPPaintManagerColor m_clrEventSelBorder;
	CXTPPaintManagerColor m_clrEventLabelNone;
	CXTPPaintManagerColor m_clrEventLabelImportant;
	CXTPPaintManagerColor m_clrEventLabelBusiness;
	CXTPPaintManagerColor m_clrEventLabelPersonal;
	CXTPPaintManagerColor m_clrEventLabelVacation;
	CXTPPaintManagerColor m_clrEventLabelMustattend;
	CXTPPaintManagerColor m_clrEventLabelTravelrequired;
	CXTPPaintManagerColor m_clrEventLabelNeedspreparation;
	CXTPPaintManagerColor m_clrEventLabelBirthday;
	CXTPPaintManagerColor m_clrEventLabelAnniversary;
	CXTPPaintManagerColor m_clrEventLabelPhonecall;
	CXTPPaintManagerColor m_clrBusyStatusOutOfOfficeBack;
	CXTPPaintManagerColor m_clrMultiDayArrow;
	CXTPPaintManagerColor m_clrHourBack;
	CXTPPaintManagerColor m_clrHourText;
	CXTPPaintManagerColor m_clrHourNowText;
	CXTPPaintManagerColor m_clrHourNowBack;
	CXTPPaintManagerColor m_clrNowLine;
	CXTPPaintManagerColor m_clrNowLineShadow;
	CXTPPaintManagerColor m_clrCellBack;
	CXTPPaintManagerColor m_clrCellText;
	CXTPPaintManagerColor m_clrNonWorkCellBack;
	CXTPPaintManagerColor m_clrGridLine;
	CXTPPaintManagerColor m_clrMonthGridLine;
	CXTPPaintManagerColor m_clrMultiDayEventFrameColor;
};


#include "Common/Base/Diagnostic/XTPEnableNoisyWarnings.h"
#endif // !defined(__XTPCALENDAROFFICE2013THEME_H__)
