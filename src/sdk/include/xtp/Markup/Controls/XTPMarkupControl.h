// XTPMarkupControl.h: interface for the CXTPMarkupControl class.
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
#if !defined(__XTPMARKUPCONTROL_H__)
#define __XTPMARKUPCONTROL_H__
//}}AFX_CODEJOCK_PRIVATE

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Common/Base/Diagnostic/XTPDisableNoisyWarnings.h"


class CXTPMarkupThickness;
class CXTPMarkupBrush;
class CXTPMarkupDrawingContext;
class CXTPMarkupDependencyProperty;

//===========================================================================
// Summary: CXTPMarkupControl is CXTPMarkupFrameworkElement derived class. It implements base class for all control elements
//===========================================================================
class _XTP_EXT_CLASS CXTPMarkupControl : public CXTPMarkupFrameworkElement
{
	DECLARE_MARKUPCLASS(CXTPMarkupControl);

public:
	// ---------------------------------------------------------------------
	// Summary:
	//     Call this method to set a value that indicates the thickness of padding space between the boundaries of the content area,
	//     and the content displayed by a control
	// Parameters
	//     nLeft - Left space
	//     nTop - Top space
	//     nRight - Right space
	//     nBottom - Bottom space
	//     nPadding - All borders
	//-----------------------------------------------------------------------
	void SetPadding(int nLeft, int nTop, int nRight, int nBottom);
	void SetPadding(int nPadding);  // <Combine CXTPMarkupControl::SetPadding@int@int@int@int>

	//-----------------------------------------------------------------------
	// Summary:
	//     Gets a value that indicates the thickness of padding space between the boundaries of the content area,
	//     and the content displayed by a control
	// Returns:
	//     CXTPMarkupThickness object contains padding space.
	// See Also: SetPadding
	//-----------------------------------------------------------------------
	CXTPMarkupThickness* GetPadding() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this method to set background of the Control object
	// Parameters: brush - New Brush to be set as background brush
	//-----------------------------------------------------------------------
	void SetBackground(CXTPMarkupBrush* brush);

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this method to get background brush of the control object
	// Returns: Pointer to CXTPMarkupBrush object contained current brush of TextBlock's object
	//-----------------------------------------------------------------------
	CXTPMarkupBrush* GetBackground() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this method to set default text brush of the control object
	// Parameters: brush - New Brush to be set as text brush
	//-----------------------------------------------------------------------
	void SetForeground(CXTPMarkupBrush* brush);

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this method to get default text brush of the control object
	// Returns: Pointer to CXTPMarkupBrush object contained current text brush of control's object
	//-----------------------------------------------------------------------
	CXTPMarkupBrush* GetForeground() const;


protected:
//{{AFX_CODEJOCK_PRIVATE
	// Implementation
	CSize ArrangeOverride(CSize szFinalSize);
	CSize MeasureOverride(CXTPMarkupDrawingContext* pDC, CSize szAvailableSize);

public:
	static CXTPMarkupDependencyProperty* m_pPaddingProperty;
	static CXTPMarkupDependencyProperty* m_pBackgroundProperty;
	static CXTPMarkupDependencyProperty* m_pForegroundProperty;
//}}AFX_CODEJOCK_PRIVATE

//{{AFX_CODEJOCK_PRIVATE
public:
	DECLARE_DISPATCH_MAP()
#ifdef _XTP_ACTIVEX
	DECLARE_INTERFACE_MAP()
	DECLARE_OLETYPELIB_EX(CXTPMarkupControl);
#endif

	afx_msg LPDISPATCH OleGetBackground();
	afx_msg void OleSetBackground(LPDISPATCH lpBackground);
	afx_msg LPDISPATCH OleGetForeground();
	afx_msg void OleSetForeground(LPDISPATCH lpBackground);
	afx_msg LPDISPATCH OleGetPadding();
	afx_msg void OleSetPadding(LPDISPATCH lpThickness);

//}}AFX_CODEJOCK_PRIVATE
};



#include "Common/Base/Diagnostic/XTPEnableNoisyWarnings.h"
#endif // !defined(__XTPMARKUPCONTROL_H__)
