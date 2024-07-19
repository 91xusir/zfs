// XTPCalendarCaptionBarTheme.h
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
#if !defined(__XTPCALENDARCAPTIONBARTHEME_H__)
#define __XTPCALENDARCAPTIONBARTHEME_H__
//}}AFX_CODEJOCK_PRIVATE

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "Common/Base/Diagnostic/XTPDisableNoisyWarnings.h"

class CXTPCalendarCaptionBarPaintManager;
class CXTPSwitchViewButtonPart;
class CXTPExpandButtonPart;
class CXTPDateLabelPart;
class CXTPScrollDateButtonPart;
class CXTPRadioButtonPart;
class CXTPCheckboxButtonPart;

//===========================================================================
// Summary:
//     This class CalendarCaptionBar control drawing, user interaction logic
//     and interaction logic with attached Calendar control.
//     paint theme.
// See Also: CXTPCalendarCaptionBarTheme
//===========================================================================
class _XTP_EXT_CLASS CXTPCalendarCaptionBarTheme : public CXTPCalendarWMHandler
{
	//{{AFX_CODEJOCK_PRIVATE
	DECLARE_DYNCREATE(CXTPCalendarCaptionBarTheme)

	friend class CXTPCalendarCaptionBarControl;
	friend class CXTPCalendarCaptionBarThemePart;
	//}}AFX_CODEJOCK_PRIVATE

public:
	//-----------------------------------------------------------------------
	// Summary:
	//     Default object constructor.
	// See Also:
	//     ~CXTPCalendarCaptionBarTheme
	//-----------------------------------------------------------------------
	CXTPCalendarCaptionBarTheme();

	//-----------------------------------------------------------------------
	// Summary:
	//     Default object destructor.
	// See Also:
	//     CXTPCalendarCaptionBarTheme
	//-----------------------------------------------------------------------
	virtual ~CXTPCalendarCaptionBarTheme();

	double m_dScale;        //The scale.

public:
	//-----------------------------------------------------------------------
	// Summary:
	//     This member function retrieves the minimum size required to show a
	//     CaptionBar control.
	// Parameters:
	//     pDC      - Pointer to a valid device context (may be NULL).
	//     bExactSize - BOOL flag to make Caption Bar minimum height if TRUE
	//                  and 11 pixels more if not (esthetic choice)
	// Returns:
	//     A minimum rectangle required to show CaptionBar control.
	// See Also:
	//     CXTPCalendarCaptionBarControl::CalcMinRect
	// --------------------------------------------------------------------------
	virtual CRect CalcMinRect(CDC* pDC = NULL, BOOL bExactSize = FALSE);

	//-----------------------------------------------------------------------
	// Summary:
	//     This member function is used to adjust the object's layout depending
	//     on the provided bounding rectangle.
	// Parameters:
	//     pDC      - Pointer to a valid device context (may be NULL).
	//     rcRect   - A CRect object containing the dimensions of the bounding
	//               rectangle for this part.
	//-----------------------------------------------------------------------
	virtual void AdjustLayout(CDC* pDC, const CRect& rcRect);

	//-------------------------------------------------------------------
	// Summary:
	//     Performs refreshing of graphical related parameters from
	//     system settings. Base implementation do nothing.
	//-------------------------------------------------------------------
	virtual void RefreshMetrics();

	//-----------------------------------------------------------------------
	// Summary:
	//     This member function initiates the CalendarCaptionBar control's redrawing.
	// Parameters:
	//     bUpdateNow : Set this parameter to TRUE if you'd like to force
	//                  control's redrawing.
	// Remarks:
	//     Call this member function if you want to request or force the control's
	//     redrawing.
	// See Also:
	//     CXTPCalendarCaptionBarTheme::Redraw, AdjustLayout
	//-----------------------------------------------------------------------
	virtual void Redraw(BOOL bUpdateNow = FALSE);

	//-----------------------------------------------------------------------
	// Summary:
	//     This member functions is used to get is CaptionBar object visible.
	// Returns:
	//     TRUE if corresponding object is visible, FALSE otherwise.
	// See Also:
	//     SetSwitchViewButtons, SetScrollDateButtons, SetDateLabel
	//-----------------------------------------------------------------------
	BOOL IsSwitchViewButtons() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     This member functions is used to get is Scroll Date Buttons object visible.
	// Returns:
	//     TRUE if corresponding object is visible, FALSE otherwise.
	//-----------------------------------------------------------------------
	BOOL IsScrollDateButtons() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     This member functions is used to get is Date Label object visible.
	// Returns:
	//     TRUE if corresponding object is visible, FALSE otherwise.
	//-----------------------------------------------------------------------
	BOOL IsDateLabel() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     This member functions is used to get is Expand Button object visible.
	// Returns:
	//     TRUE if corresponding object is visible, FALSE otherwise.
	//-----------------------------------------------------------------------
	BOOL IsExpandButton() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     This member function is used to set visible Switch View Buttons object's state.
	// Parameters:
	//     bVisible - A BOOL value to define visible state.
	// See Also: GetVisible
	//-----------------------------------------------------------------------
	void SetSwitchViewButtons(BOOL bVisible);

	//-----------------------------------------------------------------------
	// Summary:
	//     This member functions is used to set Scroll Date Buttons object visible or not.
	// Parameters:
	//     bVisible - A BOOL value to define visible state.
	//-----------------------------------------------------------------------
	void SetScrollDateButtons(BOOL bVisible);

	//-----------------------------------------------------------------------
	// Summary:
	//     This member functions is used to set Date Label object visible or not.
	// Parameters:
	//     bVisible - A BOOL value to define visible state.
	//-----------------------------------------------------------------------
	void SetDateLabel(BOOL bVisible);

	//-----------------------------------------------------------------------
	// Summary:
	//     This member functions is used to set Expand Button object visible or not.
	// Parameters:
	//     bVisible - A BOOL value to define visible state.
	//-----------------------------------------------------------------------
	void SetExpandButton(BOOL bVisible);

	//-----------------------------------------------------------------------
	// Summary:
	//     This member functions is used to set Expand Button Checked state.
	// Parameters:
	//     bVisible - A BOOL value to define checked state.
	//-----------------------------------------------------------------------
	void SetExpandButtonCheck(BOOL bCheck);

	//-----------------------------------------------------------------------
	// Summary:
	//     This member functions is used to get is Expand Button object checked.
	// Returns:
	//     TRUE if corresponding object is checked, FALSE otherwise.
	//-----------------------------------------------------------------------
	BOOL GetExpandButtonCheck();

	//-----------------------------------------------------------------------
	// Summary:
	//     This member function is used to obtain current paint theme.
	// Returns:
	//     A paint theme ID from enum XTPCalendarTheme.
	//-----------------------------------------------------------------------
	virtual XTPCalendarTheme GetPaintTheme() const;

	// ------------------------------------------------------------
	// Summary:
	//     This member function is used to set current paint theme.
	// Parameters:
	//     nTheme :  A paint theme ID from enum XTPCalendarTheme.
	//     pPaintManager : Pointer to a valid CXTPCalendarCaptionBarPaintManager derived class.
	// ------------------------------------------------------------
	virtual void SetPaintTheme(XTPCalendarTheme nTheme);
	virtual void SetPaintTheme(CXTPCalendarCaptionBarPaintManager* pPaintManager);

	//-----------------------------------------------------------------------
	// Summary:
	//     This member function is used to get Notify Connection object
	// Returns:
	//     pointer to CXTPNotifyConnection object;
	//-----------------------------------------------------------------------
	CXTPNotifyConnection* GetConnection();

public:
	//-----------------------------------------------------------------------
	// Summary:
	//     This member function is used to draw the object content utilizing
	//     the specified device context.
	// Parameters:
	//     pDC - Pointer to a valid device context.
	//-----------------------------------------------------------------------
	virtual void Draw(CDC* pDC);

	// ----------------------------
	// Summary:
	//     This member function is used to draw the object borders
	// Parameters:
	//     pDC - Pointer to a valid device context.
	//     rcRect - object rectangle
	//     rcBorders - border rectangle
	//
	// ----------------------------
	virtual void DrawBorders(CDC* pDC, const CRect& rcRect, const CRect& rcBorders);

	//-----------------------------------------------------------------------
	// Summary:
	//     This member function is used to obtain an object rectangle.
	// Returns:
	//     An object rectangle.
	//-----------------------------------------------------------------------
	virtual const CRect& GetRect();

	//-----------------------------------------------------------------------
	// Summary:
	//     This member function is used to obtain an owner (parent, host) window.
	// Returns:
	//     A pointer to CWnd object.
	//-----------------------------------------------------------------------
	CWnd* GetOwnerWnd() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     This member function is used to set owner (parent, host) window.
	// Remarks:
	//     CXTPCalendarCaptionBarControl::SetTheme call this method.
	// Parameters:
	//     pCaptionBar - A pointer to CXTPCalendarCaptionBarControl window.
	//-----------------------------------------------------------------------
	virtual void SetOwner(CXTPCalendarCaptionBarControl* pCaptionBar);

	//-----------------------------------------------------------------------
	// Summary:
	//     This member function is used to attach to CalendarControl to interact
	//     as a single control.
	// Parameters:
	//     pCalendar - A pointer to CXTPCalendarControl object or set as NULL
	//                 to detach from calendar.
	// See Also: GetCalendarCtrl
	//-----------------------------------------------------------------------
	virtual void AttachCalendar(CXTPCalendarControl* pCalendar);

	//-----------------------------------------------------------------------
	// Summary:
	//     This member function is used to obtain attached calendar control.
	// Returns:
	//     A pointer to CXTPCalendarControl object.
	//-----------------------------------------------------------------------
	virtual CXTPCalendarControl* GetCalendarCtrl() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     This member function is called to update CaptionBar controls states
	//     correspondingly to attached calendar control.
	//-----------------------------------------------------------------------
	virtual void UpdateControlsState();

	//-----------------------------------------------------------------------
	// Summary:
	//     This member function is called when user click on the caption bar
	//     element (part).
	// Parameters:
	//     nPartID - An identifier of clicked part object.
	//-----------------------------------------------------------------------
	virtual void OnPartClick(int nPartID);

	//-----------------------------------------------------------------------
	// Summary:
	//     Gives access to theme paint manager.
	// Returns:
	//     A pointer to a theme paint manager or NULL of no paint manager is set.
	//-----------------------------------------------------------------------
    CXTPCalendarCaptionBarPaintManager* GetPaintManager() const;

	BOOL m_bTimelineEnable; // Flag to show (and able to use) button to toggle TimeLine mode
	BOOL m_bMulticolumnsEnable; // Flag to show (and able to use) button to toggle Multi-Column / 2-Column Week modes
	CXTPCalendarCaptionBarControl* m_pCaptionBar; // Store pointer owner CaptionBar control.

protected:

	//{{AFX_CODEJOCK_PRIVATE
	virtual int GetChildHandlersCount() const;
	virtual CXTPCalendarWMHandler* GetChildHandlerAt(int nIndex) const;

	virtual void OnBeforeDestroy();
	virtual void OnMouseMove(UINT nFlags, CPoint point);
	virtual BOOL OnTimer(UINT_PTR uTimerID);

	virtual void OnKillFocus (CWnd* pNewWnd);
	virtual BOOL _UpdateControlsHotState(BOOL* pbMouseLeave = NULL);
	//}}AFX_CODEJOCK_PRIVATE

protected:

	CXTPCalendarCaptionBarPaintManager* m_pPaintManager;
	CXTPCalendarControl* m_pCalendar;   // Store pointer attached calendar control.

	CRect m_rcRect;                     // Store object rect.
	UINT_PTR m_nCheckHotStateTimerID;   // Store timer ID to refresh controls hot state.

protected:

	CXTPSwitchViewButtonPart*  m_pButtonViewDay;   // Store Switch to DayView button part object.
	CXTPSwitchViewButtonPart*  m_pButtonViewWeek;  // Store Switch to WeekView button part object.
	CXTPSwitchViewButtonPart*  m_pButtonViewMonth; // Store Switch to MonthView button part object.

	CXTPRadioButtonPart* m_pShowWorkWeek;  // Store Show Work Week radio button part object.
	CXTPRadioButtonPart* m_pShowFullWeek;  // Store Show Full Week radio button part object.
	CXTPCheckboxButtonPart* m_pShowMcWeek;    // Store Show Multicolumns Week check box part object.
	CXTPCheckboxButtonPart* m_pShowTimeline;  // Store Show Timeline check box part object.
	//---------------------------------
	CXTPScrollDateButtonPart*  m_pScrollPrevDate;  // Store Scroll Date Prev button part object.
	CXTPScrollDateButtonPart*  m_pScrollNextDate;  // Store Scroll Date Next button part object.
	CXTPDateLabelPart*         m_pDateLabel;       // Store Date Label part object.

	CXTPExpandButtonPart*      m_pExpandButton;       // Store Expand button part object.

	//---------------------------------
	BOOL m_bIsSwitchViewButtons;    // Store option is SwitchViewBar visible.
	BOOL m_bIsScrollDateButtons;    // Store option is ScrollDate buttons visible.
	BOOL m_bIsDateLabel;            // Store option is date Label visible.
	BOOL m_bIsExpandButton;         // Store option is Expand button visible.

	BOOL m_bIsOneLine; // flag to present 1-line (if TRUE) or 2-lines object layout

public:

	int m_nScrollDateButtonsMargin;

	//-----------------------------------------------------------------------
	// Summary:
	//     This member functions is used to get is CaptionBar layout use 1-line shape.
	// Returns:
	//     TRUE if corresponding object use 1-line shape, FALSE if 2-lines shape.
	//-----------------------------------------------------------------------
	BOOL IsOneLine() { return m_bIsOneLine; }

	//-----------------------------------------------------------------------
	// Summary:
	//     This member functions is used to set CaptionBar layout to use 1-line shape.
	// Parameters:
	//     bSet - BOOL (TRUE to use 1-line shape, FALSE to use 2-lines shape)
	//-----------------------------------------------------------------------
	void SetOneLine(BOOL bSet) { m_bIsOneLine = bSet; }

public:
	//{{AFX_CODEJOCK_PRIVATE
	typedef CXTPCalendarPtrCollectionT<CXTPCalendarCaptionBarThemePart> CThemePartsArray;
	CThemePartsArray        m_arMembers;
	//}}AFX_CODEJOCK_PRIVATE

protected:
	//{{AFX_CODEJOCK_PRIVATE
	CXTPNotifyConnection* m_pConnection;

	CXTPNotifySink* m_pSink;

	virtual void OnEvent_Calendar(XTP_NOTIFY_CODE Event, WPARAM wParam, LPARAM lParam);
	virtual void OnEvent_ResourceImagesChanged(XTP_NOTIFY_CODE Event, WPARAM wParam, LPARAM lParam);

	friend class CCalendarCaptionBarCtrl;
	friend class CXTPCalendarCaptionBarPaintManager;
	//}}AFX_CODEJOCK_PRIVATE
};

//////////////////////////////////////////////////////////////////////////

AFX_INLINE CXTPCalendarControl* CXTPCalendarCaptionBarTheme::GetCalendarCtrl() const {
	return m_pCalendar;
}
AFX_INLINE const CRect& CXTPCalendarCaptionBarTheme::GetRect() {
	return m_rcRect;
}
AFX_INLINE BOOL CXTPCalendarCaptionBarTheme::IsSwitchViewButtons() const{
	return m_bIsSwitchViewButtons;
}
AFX_INLINE BOOL CXTPCalendarCaptionBarTheme::IsScrollDateButtons() const {
	return m_bIsScrollDateButtons;
}
AFX_INLINE BOOL CXTPCalendarCaptionBarTheme::IsDateLabel() const {
	return m_bIsDateLabel;
}
AFX_INLINE BOOL CXTPCalendarCaptionBarTheme::IsExpandButton() const {
	return m_bIsExpandButton;
}
AFX_INLINE void CXTPCalendarCaptionBarTheme::SetSwitchViewButtons(BOOL bVisible) {
	m_bIsSwitchViewButtons = bVisible;
	AdjustLayout(NULL, GetRect());
	Redraw();
}
AFX_INLINE void CXTPCalendarCaptionBarTheme::SetScrollDateButtons(BOOL bVisible) {
	m_bIsScrollDateButtons = bVisible;
	AdjustLayout(NULL, GetRect());
	Redraw();
}
AFX_INLINE void CXTPCalendarCaptionBarTheme::SetExpandButton(BOOL bVisible) {
	m_bIsExpandButton = bVisible;
	AdjustLayout(NULL, GetRect());
	Redraw();
}
AFX_INLINE void CXTPCalendarCaptionBarTheme::SetDateLabel(BOOL bVisible) {
	m_bIsDateLabel = bVisible;
	AdjustLayout(NULL, GetRect());
	Redraw();
}
AFX_INLINE CXTPCalendarCaptionBarPaintManager* CXTPCalendarCaptionBarTheme::GetPaintManager() const {
	return m_pPaintManager;
}


#include "Common/Base/Diagnostic/XTPEnableNoisyWarnings.h"
#endif // !defined(__XTPCALENDARCAPTIONBARTHEME_H__)
