// XTPCalendarCaptionBarThemePart.h
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
#if !defined(__XTPCALENDARCAPTIONBARTHEMEPART_H__)
#define __XTPCALENDARCAPTIONBARTHEMEPART_H__
//}}AFX_CODEJOCK_PRIVATE

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "Common/Base/Diagnostic/XTPDisableNoisyWarnings.h"

class CXTPCalendarCaptionBarPaintManager;

//-----------------------------------------------------------------------
// Summary:
//     Defines the part object states flags.
//-----------------------------------------------------------------------
enum EStates
{
	xtpBtnStateNormal       = 0,        // defines a normal state.
	xtpBtnStateMouseOver    = 0x001,    // defines a mouse over state.
	xtpBtnStatePressed      = 0x002,    // defines a pressed state.
	xtpBtnStateChecked      = 0x004,    // defines a checked state.
	xtpBtnStateFocused      = 0x008,    // defines a ficused state.
};

//===========================================================================
// Summary:
//     This class used as a base class for graphical Parts of CalendarCaptionBar
//     paint theme.
// See Also: CXTPCalendarCaptionBarTheme
//===========================================================================
class _XTP_EXT_CLASS CXTPCalendarCaptionBarThemePart : public CXTPCalendarWMHandler
{
	//{{AFX_CODEJOCK_PRIVATE
	friend class CXTPCalendarCaptionBarTheme;

	DECLARE_DYNAMIC(CXTPCalendarCaptionBarThemePart)
	//}}AFX_CODEJOCK_PRIVATE

public:

	//-----------------------------------------------------------------------
	// Summary:
	//     Default object constructor.
	// Parameters:
	//     pTheme - Pointer to parent CXTPCalendarCaptionBarTheme object.
	//     nPartID - An object identifier (value from enum XTPEnumCalendarCaptionBarThemeObject).
	// See Also:
	//     ~CXTPCalendarCaptionBarThemePart
	//-----------------------------------------------------------------------
	CXTPCalendarCaptionBarThemePart(CXTPCalendarCaptionBarTheme* pTheme, int nPartID);

	//-----------------------------------------------------------------------
	// Summary:
	//     Default object destructor.
	// See Also:
	//     CXTPCalendarCaptionBarThemePart
	//-----------------------------------------------------------------------
	virtual ~CXTPCalendarCaptionBarThemePart();

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

	//-----------------------------------------------------------------------
	// Summary:
	//     This member function is used to draw the part content utilizing
	//     the specified device context.
	// Parameters:
	//     pDC - Pointer to a valid device context.
	//-----------------------------------------------------------------------
	virtual void Draw(CDC* pDC);

	//-------------------------------------------------------------------
	// Summary:
	//     Performs refreshing of graphical related parameters from
	//     system settings. Base implementation do nothing.
	//-------------------------------------------------------------------
	virtual void RefreshMetrics() {};

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
	//     This member function is used to determine is the specified point
	//     over visible object rectangle.
	// Parameters:
	//     pt - A CPoint object reference that contains the point to test.
	// Returns:
	//      TRUE if the point is inside the caption bar, FALSE else.
	//-----------------------------------------------------------------------
	virtual BOOL HitTest(const CPoint& pt) const;

	//-----------------------------------------------------------------------
	// Summary:
	//     This member function is used to obtain pointer to parent
	//     CXTPCalendarCaptionBarTheme object.
	// Returns:
	//     A pointer to CXTPCalendarCaptionBarTheme object.
	//-----------------------------------------------------------------------
	virtual CXTPCalendarCaptionBarTheme* GetTheme() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     This member function is used to obtain current paint theme.
	// Returns:
	//     A paint theme ID from enum XTPCalendarTheme.
	//-----------------------------------------------------------------------
	virtual XTPCalendarTheme GetPaintTheme() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     This member function is used to obtain an object identifier.
	// Returns:
	//     An object ID (value from enum XTPEnumCalendarCaptionBarThemeObject).
	//-----------------------------------------------------------------------
	virtual int GetPartID() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     This member function is used to obtain an object rectangle.
	// Returns:
	//     An object rectangle.
	//-----------------------------------------------------------------------
	virtual CRect GetRect() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     This member function is used to obtain an object text.
	// Returns:
	//     An object text as CString.
	// See Also: SetText
	//-----------------------------------------------------------------------
	virtual CString GetText() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     This member function is used to set object text.
	// Parameters:
	//     pcszText - An object text to set.
	// See Also: GetText
	//-----------------------------------------------------------------------
	virtual void SetText(LPCTSTR pcszText);

	//-----------------------------------------------------------------------
	// Summary:
	//     This member function is used to get is part object visible.
	// Returns:
	//     TRUE if object is visible, FALSE otherwise.
	// See Also: SetVisible
	//-----------------------------------------------------------------------
	virtual BOOL IsVisible() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     This member function is used to set visible object's state.
	// Parameters:
	//     bVisible - A BOOL value to define visible state.
	// See Also: GetVisible
	//-----------------------------------------------------------------------
	virtual void SetVisible(BOOL bVisible);

	//-----------------------------------------------------------------------
	// Summary:
	//     This member function is used to get checked state.
	// Returns:
	//     Non-zero if object is checked, 0 (zero) otherwise.
	// See Also: SetCheck
	//-----------------------------------------------------------------------
	virtual int GetCheck() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     This member function is used to set checked state.
	// Parameters:
	//     nCheck - Set as Non-zero if object is checked, 0 (zero) otherwise.
	// See Also: GetCheck
	//-----------------------------------------------------------------------
	virtual void SetCheck(int nCheck);

	//-----------------------------------------------------------------------
	// Summary:
	//     This member function is used to get object state (flags from
	//     CXTPCalendarCaptionBarThemePart::EStates enum).
	// Returns:
	//     Current object state flags.
	// See Also: SetState
	//-----------------------------------------------------------------------
	int GetState() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     This member function is used to set object state as set of flags
	//     from CXTPCalendarCaptionBarThemePart::EStates enum.
	// Parameters:
	//     nState - Current object state flags.
	// See Also: SetState
	//-----------------------------------------------------------------------
	virtual void SetState(int nState);

	//-----------------------------------------------------------------------
	// Summary:
	//     This member function is used to get object text color.
	// Returns:
	//     m_clrTextColor value returned if it is defined, otherwise
	//     GetTheme()->m_clrTextColor value returned.
	// See Also:
	//     CXTPCalendarCaptionBarThemePart::m_clrTextColor,
	//     CXTPCalendarCaptionBarTheme::m_clrTextColor
	//-----------------------------------------------------------------------
	virtual COLORREF GetTextColor();

	//-----------------------------------------------------------------------
	// Summary:
	//-----------------------------------------------------------------------
	CXTPCalendarCaptionBarPaintManager* GetPaintManager() const;

public:

	//-----------------------------------------------------------------------
	// Summary:
	//     This member function is used to processes mouse movement events.
	// Parameters:
	//     nFlags  - A UINT that is used to indicate whether various virtual
	//               keys are down.
	//     point   - A CPoint that specifies the x- and y- coordinates of the cursor.
	//               These coordinates are always relative to the
	//               upper-left corner of the window.
	// Remarks:
	//     This method is called by the parent object when the user
	//     moves the mouse cursor.
	//-----------------------------------------------------------------------
	virtual void OnMouseMove(UINT nFlags, CPoint point);

	//-----------------------------------------------------------------------
	// Summary:
	//     This member function is used to process left mouse button down events.
	// Parameters:
	//     nFlags  - An int that indicates whether various virtual keys are down.
	//     point   - A CPoint object that specifies the x- and y- coordinates of the cursor.
	//               These coordinates are always relative to the
	//               upper-left corner of the window.
	// Returns:
	//     TRUE if the message is handled successfully, FALSE else.
	// Remarks:
	//     This method is called by the parent object when the user
	//     presses the left mouse button.
	//-----------------------------------------------------------------------
	virtual BOOL OnLButtonDown(UINT nFlags, CPoint point);

	//-----------------------------------------------------------------------
	// Summary:
	//     This member function is used to process left mouse button up events.
	// Parameters:
	//     nFlags  - An int that indicates whether various virtual keys are down.
	//     point   - A CPoint object that specifies the x- and y- coordinates of the cursor.
	//               These coordinates are always relative to the
	//               upper-left corner of the window.
	// Returns:
	//     TRUE if the message is handled successfully, FALSE else.
	// Remarks:
	//     This method is called by the parent object when the user
	//     releases the left mouse button.
	//-----------------------------------------------------------------------
	virtual BOOL OnLButtonUp(UINT nFlags, CPoint point);

public:
	CXTPPaintManagerColor   m_clrTextColor; // Store the object's text color.
	CRect   m_rcRect;   // Store a Part rectangle.
	int     m_nState;   // Store a Part state.
	CString m_strText;  // Store a Part text.

protected:
	CXTPCalendarCaptionBarTheme* m_pTheme;  // Store pointer to a parent theme object.
	int m_nPartID;      // Store a Part ID.

	BOOL    m_bVisible; // Store a Part visible state.
};

//-----------------------------------------------------------------------
// Summary:
//     This class implements SwitchView button part of the SwitchViewBar.
// See Also:
//     CXTPCalendarCaptionBarThemePart
//-----------------------------------------------------------------------
class _XTP_EXT_CLASS CXTPSwitchViewButtonPart: public CXTPCalendarCaptionBarThemePart
{
public:
	//{{AFX_CODEJOCK_PRIVATE
	CXTPSwitchViewButtonPart(CXTPCalendarCaptionBarTheme* pTheme, int nPartID) :
	  CXTPCalendarCaptionBarThemePart (pTheme, nPartID) {};
	  
	  virtual void RefreshMetrics();
	  virtual void AdjustLayout(CDC* pDC, const CRect& rcRect);
	  virtual void Draw(CDC* pDC);
	  //}}AFX_CODEJOCK_PRIVATE
};

//-----------------------------------------------------------------------
// Summary:
//     This class implements Radio button part of the SwitchViewBar.
// See Also:
//     CXTPCalendarCaptionBarThemePart
//-----------------------------------------------------------------------
class _XTP_EXT_CLASS CXTPRadioButtonPart : public CXTPCalendarCaptionBarThemePart
{
public:
	//{{AFX_CODEJOCK_PRIVATE
	CXTPRadioButtonPart(CXTPCalendarCaptionBarTheme* pTheme, int nPartID) :
	  CXTPCalendarCaptionBarThemePart (pTheme, nPartID) {};
	  
	  virtual void AdjustLayout(CDC* pDC, const CRect& rcRect);
	  virtual void RefreshMetrics();
	  virtual void Draw(CDC* pDC);
	  //}}AFX_CODEJOCK_PRIVATE
};

//-----------------------------------------------------------------------
// Summary:
//     This class implements Checkbox button part of the SwitchViewBar.
// See Also:
//     CXTPCalendarCaptionBarThemePart
//-----------------------------------------------------------------------
class _XTP_EXT_CLASS CXTPCheckboxButtonPart : public CXTPCalendarCaptionBarThemePart
{
public:
	//{{AFX_CODEJOCK_PRIVATE
	CXTPCheckboxButtonPart(CXTPCalendarCaptionBarTheme* pTheme, int nPartID) :
	  CXTPCalendarCaptionBarThemePart (pTheme, nPartID) {};
	  
	  virtual void AdjustLayout(CDC* pDC, const CRect& rcRect);
	  virtual void RefreshMetrics();
	  virtual void Draw(CDC* pDC);
	  //}}AFX_CODEJOCK_PRIVATE
};
//-----------------------------------------------------------------------
// Summary:
//     This class implements Scroll Date button part of the ScrollDateBar.
// See Also:
//     CXTPCalendarCaptionBarThemePart
//-----------------------------------------------------------------------
class _XTP_EXT_CLASS CXTPScrollDateButtonPart : public CXTPCalendarCaptionBarThemePart
{
public:
	//{{AFX_CODEJOCK_PRIVATE
	CXTPScrollDateButtonPart(CXTPCalendarCaptionBarTheme* pTheme, int nPartID) :
	  CXTPCalendarCaptionBarThemePart (pTheme, nPartID) {};
	  
	  virtual void Draw(CDC* pDC);
	  virtual void AdjustLayout(CDC* pDC, const CRect& rcRect);
	  
	  virtual BOOL OnLButtonDown(UINT nFlags, CPoint point);
	  virtual void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	  virtual void OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags);
      virtual LPCTSTR _GetImageName();
	  //}}AFX_CODEJOCK_PRIVATE
protected:
	//{{AFX_CODEJOCK_PRIVATE
	
	virtual void DrawScrollTriangle(CDC* pDC, CRect rcRect, BOOL bLeftDirection, COLORREF clrColor);
	//}}AFX_CODEJOCK_PRIVATE
};

//-----------------------------------------------------------------------
// Summary:
//     This class implements Date Label part of the ScrollDateBar.
// See Also:
//     CXTPCalendarCaptionBarThemePart
//-----------------------------------------------------------------------
class _XTP_EXT_CLASS CXTPDateLabelPart : public CXTPCalendarCaptionBarThemePart
{
public:
	//{{AFX_CODEJOCK_PRIVATE
	CXTPDateLabelPart(CXTPCalendarCaptionBarTheme* pTheme, int nPartID) :
	  CXTPCalendarCaptionBarThemePart (pTheme, nPartID) {};
	  
	  virtual void RefreshMetrics();
	  virtual void Draw(CDC* pDC);
	  
	  virtual CString _FormatDate(COleDateTime dtDay1, COleDateTime dtDay2, BOOL bShowMonthDay);
	  void UserAction_FormatCaptionBarLabel(CString& strCaption, COleDateTime dtDay1);
	  //}}AFX_CODEJOCK_PRIVATE
public:
	CXTPCalendarThemeFontValue  m_fntTextFont; // Store the text font.
};

//-----------------------------------------------------------------------
// Summary:
//     This class implements Expand button part of the ScrollDateBar.
// See Also:
//     CXTPCalendarCaptionBarThemePart
//-----------------------------------------------------------------------
class _XTP_EXT_CLASS CXTPExpandButtonPart : public CXTPCalendarCaptionBarThemePart
{
public:
	//{{AFX_CODEJOCK_PRIVATE
	CXTPExpandButtonPart(CXTPCalendarCaptionBarTheme* pTheme, int nPartID) :
	  CXTPCalendarCaptionBarThemePart (pTheme, nPartID) {};
	  
	  virtual void Draw(CDC* pDC);
	  virtual void AdjustLayout(CDC* pDC, const CRect& rcRect);
	  //}}AFX_CODEJOCK_PRIVATE
protected:
	//{{AFX_CODEJOCK_PRIVATE
	//virtual LPCTSTR _GetImageName();
	
	//virtual void _DrawTriangle(CDC* pDC, CPoint pt0, CPoint pt1, CPoint pt2, COLORREF clrColor);
	virtual void DrawExpandTriangle(CDC* pDC, CRect rcRect, BOOL bDownDirection, COLORREF clrColor);
	//}}AFX_CODEJOCK_PRIVATE
};

//////////////////////////////////////////////////////////////////////////

AFX_INLINE CXTPCalendarCaptionBarTheme* CXTPCalendarCaptionBarThemePart::GetTheme() const {
	return m_pTheme;
}
AFX_INLINE CRect CXTPCalendarCaptionBarThemePart::GetRect() const {
	return m_rcRect;
}
AFX_INLINE CString CXTPCalendarCaptionBarThemePart::GetText() const {
	return m_strText;
}
AFX_INLINE void CXTPCalendarCaptionBarThemePart::SetText(LPCTSTR pcszText) {
	m_strText = pcszText;
}
AFX_INLINE int CXTPCalendarCaptionBarThemePart::GetPartID() const {
	return m_nPartID;
}
AFX_INLINE BOOL CXTPCalendarCaptionBarThemePart::IsVisible() const {
	return m_bVisible;
}
AFX_INLINE void CXTPCalendarCaptionBarThemePart::SetVisible(BOOL bVisible) {
	m_bVisible = bVisible;
}
AFX_INLINE int CXTPCalendarCaptionBarThemePart::GetCheck() const {
	return !!(m_nState & xtpBtnStateChecked);
}
AFX_INLINE void CXTPCalendarCaptionBarThemePart::SetCheck(int nCheck) {
	if (nCheck)
		m_nState = m_nState | xtpBtnStateChecked;
	else
		m_nState = m_nState & (~xtpBtnStateChecked);
}
AFX_INLINE int CXTPCalendarCaptionBarThemePart::GetState() const {
	return m_nState;
}
AFX_INLINE void CXTPCalendarCaptionBarThemePart::SetState(int nState)
{
	m_nState = nState;
}


#include "Common/Base/Diagnostic/XTPEnableNoisyWarnings.h"
#endif // !defined(__XTPCALENDARCAPTIONBARTHEMEPART_H__)
