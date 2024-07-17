// XTPMarkupStackPanel.h: interface for the CXTPMarkupStackPanel class.
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
#if !defined(__XTPMARKUPSTACKPANEL_H__)
#define __XTPMARKUPSTACKPANEL_H__
//}}AFX_CODEJOCK_PRIVATE

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Common/Base/Diagnostic/XTPDisableNoisyWarnings.h"

class CXTPMarkupDrawingContext;

//===========================================================================
// Summary: CXTPMarkupStackPanel is CXTPMarkupPanel derived class. It implements StackPanel XAML Tag
//===========================================================================
class _XTP_EXT_CLASS CXTPMarkupStackPanel : public CXTPMarkupPanel
{
	DECLARE_MARKUPCLASS(CXTPMarkupStackPanel)
protected:
	//-----------------------------------------------------------------------
	// Summary:
	//     Constructs a CXTPMarkupStackPanel object
	//-----------------------------------------------------------------------
	CXTPMarkupStackPanel();

	//-----------------------------------------------------------------------
	// Summary:
	//     Destroys a CXTPMarkupStackPanel object, handles cleanup and deallocation
	//-----------------------------------------------------------------------
	virtual ~CXTPMarkupStackPanel();

public:
	//-----------------------------------------------------------------------
	// Summary:
	//     Call this method to set orientation of child elements inside StackPanel
	// Parameters:
	//     orientation - xtpMarkupOrientationHorizontal to position elements horizontally; xtpMarkupOrientationVertical otherwise.
	//-----------------------------------------------------------------------
	void SetOrientation(XTPMarkupOrientation orientation);

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this method to get current orientation of child elements inside StackPanel
	// Returns:xtpMarkupOrientationHorizontal to position elements horizontally; xtpMarkupOrientationVertical otherwise.
	//-----------------------------------------------------------------------
	XTPMarkupOrientation GetOrientation() const;

protected:
//{{AFX_CODEJOCK_PRIVATE
	// Implementation
	virtual CSize MeasureOverride(CXTPMarkupDrawingContext* pDC, CSize szAvailableSize);
	virtual CSize ArrangeOverride(CSize szFinalSize);

public:
	static CXTPMarkupDependencyProperty* m_pOrientationProperty;
//}}AFX_CODEJOCK_PRIVATE

//{{AFX_CODEJOCK_PRIVATE
public:
	DECLARE_DISPATCH_MAP()
#ifdef _XTP_ACTIVEX
	DECLARE_INTERFACE_MAP()
	DECLARE_OLETYPELIB_EX(CXTPMarkupStackPanel);
#endif

	afx_msg long OleGetOrientation();
	afx_msg void OleSetOrientation(long nValue);
//}}AFX_CODEJOCK_PRIVATE
};



#include "Common/Base/Diagnostic/XTPEnableNoisyWarnings.h"
#endif // !defined(__XTPMARKUPSTACKPANEL_H__)
