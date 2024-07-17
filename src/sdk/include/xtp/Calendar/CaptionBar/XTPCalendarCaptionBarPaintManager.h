// XTPCalendarCaptionBarPaintManager.h: interface for the CXTPCalendarCaptionBarPaintManager class.
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
#if !defined(_XTPCALENDARCONTROLPAINTMANAGER_H__)
#define _XTPCALENDARCONTROLPAINTMANAGER_H__
//}}AFX_CODEJOCK_PRIVATE

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Common/Base/Diagnostic/XTPDisableNoisyWarnings.h"

//===========================================================================
// Summary:
//     Utility class, handles most of the drawing activities.
// Remarks:
//     It stores all settings, needed by control to perform drawing operations :
//     fonts, colors, styles for all others classes of control. It also implements
//     all functions for drawing typical graphical primitives, functions that
//     directly work with device context.
//     Can be overridden to provide another look and feel for the control.
//     Thus, you have an easy way to change the "skin" of your control. Just provide
//     your own implementation of CXTPCalendarCaptionBarPaintManager and don't
//     touch the functionality of any of the other control classes.
//
//          Create a CXTPCalendarCaptionBarPaintManager by calling its constructor.
//          Furthermore, you can call get and set functions to change
//          settings as needed.
//===========================================================================
class _XTP_EXT_CLASS CXTPCalendarCaptionBarPaintManager : public CXTPCmdTarget
{
public:
	//-----------------------------------------------------------------------
	// Summary:
	//     Default paint manager constructor.
	// Remarks:
	//     Handles initial initialization.
	// See Also: RefreshMetrics()
	//-----------------------------------------------------------------------
	CXTPCalendarCaptionBarPaintManager(CXTPCalendarCaptionBarTheme* pTheme = NULL);

	//-----------------------------------------------------------------------
	// Summary:
	//     Default paint manager destructor.
	// Remarks:
	//     Handles member items deallocation.
	//-----------------------------------------------------------------------
	virtual ~CXTPCalendarCaptionBarPaintManager();

	//-----------------------------------------------------------------------
	// Summary:
	//     This member function is used to obtain current paint theme.
	// Returns:
	//     A paint theme ID from enum XTPCalendarTheme.
	//-----------------------------------------------------------------------
	virtual XTPCalendarTheme GetPaintTheme() const;

	//-------------------------------------------------------------------
	// Summary:
	//     Performs refreshing of graphical related parameters from
	//     system settings. Base implementation do nothing.
	//-------------------------------------------------------------------
	virtual void RefreshMetrics();

	virtual void Draw(CDC* pDC);
	virtual void DrawBorders(CDC* pDC, const CRect& rcRect, const CRect& rcBorders);
	virtual void DrawTriangle(CDC* pDC, CPoint pt0, CPoint pt1, CPoint pt2, COLORREF clrColor);

	virtual void PartRefresh(CXTPSwitchViewButtonPart* pBarPart) = 0;
	virtual void PartRefresh(CXTPRadioButtonPart* pBarPart) = 0;
	virtual void PartRefresh(CXTPCheckboxButtonPart* pBarPart) = 0;
	virtual void PartRefresh(CXTPDateLabelPart* pBarPart) = 0;

	virtual void PartAdjustLayout(CXTPSwitchViewButtonPart* pBarPart, CDC* pDC, const CRect& rcRect) = 0;
	virtual void PartAdjustLayout(CXTPScrollDateButtonPart* pBarPart, CDC* /*pDC*/, const CRect& rcRect) = 0;
	virtual void PartAdjustLayout(CXTPRadioButtonPart* pBarPart, CDC* pDC, const CRect& rcRect) = 0;
	virtual void PartAdjustLayout(CXTPCheckboxButtonPart* pBarPart, CDC* pDC, const CRect& rcRect) = 0;
	virtual void PartAdjustLayout(CXTPExpandButtonPart* pBarPart, CDC* /*pDC*/, const CRect& rcRect) = 0;

	virtual void PartDraw(CXTPSwitchViewButtonPart* pBarPart, CDC* pDC) = 0;
	virtual void PartDraw(CXTPScrollDateButtonPart* pBarPart, CDC* pDC) = 0;
	virtual void PartDrawScrollTriangle(CXTPScrollDateButtonPart* pBarPart, CDC* pDC, CRect rcRect, BOOL bLeftDirection, COLORREF clrColor) = 0;
	virtual void PartDraw(CXTPRadioButtonPart* pBarPart, CDC* pDC) = 0;
	virtual void PartDraw(CXTPCheckboxButtonPart* pBarPart, CDC* pDC) = 0;
	virtual void PartDraw(CXTPDateLabelPart* pBarPart, CDC* pDC) = 0;
	virtual void PartDraw(CXTPExpandButtonPart* pBarPart, CDC* pDC) = 0;
	virtual void PartDrawExpandTriangle(CXTPExpandButtonPart* pBarPart, CDC* pDC, CRect rcRect, BOOL bDownDirection, COLORREF clrColor) = 0;

	CXTPPaintManagerColor GetSwitchViewBarBkColor() const;
	void SetSwitchViewBarBkColor(const CXTPPaintManagerColor& color);
	
	CXTPPaintManagerColor GetScrollDateBarBkColor() const;
	void SetScrollDateBarBkColor(const CXTPPaintManagerColor& color);

	CXTPPaintManagerColor GetTextColor() const;
	void SetTextColor(const CXTPPaintManagerColor& color);

	void GetBorderColors(
		CXTPPaintManagerColor& clrLeft,
		CXTPPaintManagerColor& clrTop,
		CXTPPaintManagerColor& clrRight,
		CXTPPaintManagerColor& clrBottom) const;
	void SetBorderColors(
		const CXTPPaintManagerColor& clrLeft,
		const CXTPPaintManagerColor& clrTop,
		const CXTPPaintManagerColor& clrRight,
		const CXTPPaintManagerColor& clrBottom);

	CXTPPaintManagerColor GetScrollTriangleColor() const;
	void SetScrollTriangleColor(const CXTPPaintManagerColor& color);

protected:
	
	XTPCalendarTheme	         m_nPaintTheme;            // 
	CXTPCalendarCaptionBarTheme* m_pTheme;                 // Store pointer to a parent theme object.
	CXTPPaintManagerColor        m_clrSwitchViewBarBk;     // Store SwitchViewBar background color.
	CXTPPaintManagerColor        m_clrScrollDateBarBk;     // Store ScrollDateBar background color.
	CXTPPaintManagerColor        m_clrTextColor;           // Store the text color.
	CXTPPaintManagerColor        m_clrBorders[4];          // Store border line color, left = 0, top = 1, right = 2, bottom = 3.
	CXTPPaintManagerColor        m_clrScrollTriangleColor; // Defines date scroll arrow color.
	CXTPCalendarThemeFontValue   m_fntTextFont;            // Store the text font.
	CXTPWinThemeWrapper*         m_pThemeButton;           // 

	friend class CXTPCalendarCaptionBarThemePart;
};

/////////////////////////////////////////////////////////////////////////////

AFX_INLINE XTPCalendarTheme CXTPCalendarCaptionBarPaintManager::GetPaintTheme() const {
	return m_nPaintTheme;
}

AFX_INLINE CXTPPaintManagerColor CXTPCalendarCaptionBarPaintManager::GetSwitchViewBarBkColor() const {
	return m_clrSwitchViewBarBk;
}

AFX_INLINE void CXTPCalendarCaptionBarPaintManager::SetSwitchViewBarBkColor(const CXTPPaintManagerColor& color) {
	m_clrSwitchViewBarBk = color;
}

AFX_INLINE CXTPPaintManagerColor CXTPCalendarCaptionBarPaintManager::GetScrollDateBarBkColor() const {
	return m_clrScrollDateBarBk;
}

AFX_INLINE void CXTPCalendarCaptionBarPaintManager::SetScrollDateBarBkColor(const CXTPPaintManagerColor& color) {
	m_clrScrollDateBarBk = color;
}

AFX_INLINE CXTPPaintManagerColor CXTPCalendarCaptionBarPaintManager::GetTextColor() const {
	return m_clrTextColor;
}

AFX_INLINE void CXTPCalendarCaptionBarPaintManager::SetTextColor(const CXTPPaintManagerColor& color) {
	m_clrTextColor = color;
}

AFX_INLINE void CXTPCalendarCaptionBarPaintManager::GetBorderColors(
	CXTPPaintManagerColor& clrLeft,
	CXTPPaintManagerColor& clrTop,
	CXTPPaintManagerColor& clrRight,
	CXTPPaintManagerColor& clrBottom) const {

	clrLeft = m_clrBorders[0];
	clrTop = m_clrBorders[1];
	clrRight = m_clrBorders[2];
	clrBottom = m_clrBorders[3];
}

AFX_INLINE void CXTPCalendarCaptionBarPaintManager::SetBorderColors(
	const CXTPPaintManagerColor& clrLeft,
	const CXTPPaintManagerColor& clrTop,
	const CXTPPaintManagerColor& clrRight,
	const CXTPPaintManagerColor& clrBottom) {

	m_clrBorders[0] = clrLeft;
	m_clrBorders[1] = clrTop;
	m_clrBorders[2] = clrRight;
	m_clrBorders[3] = clrBottom;
}

AFX_INLINE CXTPPaintManagerColor CXTPCalendarCaptionBarPaintManager::GetScrollTriangleColor() const {
	return m_clrScrollTriangleColor;
}

AFX_INLINE void CXTPCalendarCaptionBarPaintManager::SetScrollTriangleColor(const CXTPPaintManagerColor& color) {
	m_clrScrollTriangleColor = color;
}



#include "Common/Base/Diagnostic/XTPEnableNoisyWarnings.h"
#endif // !defined(_XTPCALENDARCONTROLPAINTMANAGER_H__)
