// XTPCalendarCaptionBarOffice2013Theme.h interface for the CXTPCalendarCaptionBarOffice2003Theme class.
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
#if !defined(__XTPCALENDARCAPTIONBAROFFICE2013THEME_H__)
#define __XTPCALENDARCAPTIONBAROFFICE2013THEME_H__
//}}AFX_CODEJOCK_PRIVATE

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Common/Base/Diagnostic/XTPDisableNoisyWarnings.h"

//===========================================================================
// Summary:
//     CXTPCalendarCaptionBarOffice2013Theme is a CXTPCalendarCaptionBarOffice2003Theme derived
//     class that represents the Office 2013 white calendar theme.
//===========================================================================
class _XTP_EXT_CLASS CXTPCalendarCaptionBarOffice2013Theme : public CXTPCalendarCaptionBarOffice2003Theme
{
public:
	//-----------------------------------------------------------------------
	// Summary:
	//     Default paint manager constructor.
	// Remarks:
	//     Handles initial initialization.
	// See Also: RefreshMetrics()
	//-----------------------------------------------------------------------
	CXTPCalendarCaptionBarOffice2013Theme(CXTPCalendarCaptionBarTheme* pTheme = NULL);

	//-----------------------------------------------------------------------
	// Summary:
	//     Default paint manager destructor.
	// Remarks:
	//     Handles member items deallocation.
	//-----------------------------------------------------------------------
	virtual ~CXTPCalendarCaptionBarOffice2013Theme();

	//-------------------------------------------------------------------
	// Summary:
	//     Performs refreshing of graphical related parameters from
	//     system settings. Base implementation do nothing.
	//-------------------------------------------------------------------
	virtual void RefreshMetrics();

	virtual void PartRefresh(CXTPSwitchViewButtonPart* pBarPart);
	virtual void PartRefresh(CXTPRadioButtonPart* pBarPart);
	virtual void PartRefresh(CXTPCheckboxButtonPart* pBarPart);

	virtual void PartDraw(CXTPSwitchViewButtonPart* pBarPart, CDC* pDC);
	virtual void PartDraw(CXTPScrollDateButtonPart* pBarPart, CDC* pDC);
	virtual void PartDraw(CXTPDateLabelPart* pBarPart, CDC* pDC);

	virtual void DrawBack(CDC* pDC, CRect rcRect, int nState);

protected:

	CXTPPaintManagerColor m_clrButtonHighlight;
	CXTPPaintManagerColor m_clrButtonHighlightText;
	CXTPPaintManagerColor m_clrButtonHighlightBorder;
	CXTPPaintManagerColor m_clrButtonPressed;
	CXTPPaintManagerColor m_clrButtonPressedText;
	CXTPPaintManagerColor m_clrButtonPressedBorder;
	CXTPPaintManagerColor m_clrButtonChecked;
	CXTPPaintManagerColor m_clrButtonCheckedText;
	CXTPPaintManagerColor m_clrButtonCheckedBorder;
};


#include "Common/Base/Diagnostic/XTPEnableNoisyWarnings.h"
#endif // !defined(__XTPCALENDARCAPTIONBAROFFICE2013THEME_H__)
