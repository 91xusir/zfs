// XTPStatusBarPaneSlider.h
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
#if !defined(__XTPSTATUSBARPANESLIDER_H__)
#define __XTPSTATUSBARPANESLIDER_H__
//}}AFX_CODEJOCK_PRIVATE

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Common/Base/Diagnostic/XTPDisableNoisyWarnings.h"

//===========================================================================
// Summary:
//     CXTPStatusBarSliderPane is a CXTPStatusBarPane derived class. CXTPStatusBarSliderPane works with
//     CXTStatusBar and allows you to create themed Slider in your status
//     bar area.
//===========================================================================
class _XTP_EXT_CLASS CXTPStatusBarSliderPane : public CXTPStatusBarScrollBarPane
{
	DECLARE_DYNAMIC(CXTPStatusBarSliderPane)

public:

	//-----------------------------------------------------------------------
	// Summary:
	//     Constructs a CXTPStatusBarSliderPane object.
	//-----------------------------------------------------------------------
	CXTPStatusBarSliderPane();

	//-----------------------------------------------------------------------
	// Summary:
	//     Destroys a CXTPStatusBarSliderPane object, handles cleanup and
	//     deallocation
	//-----------------------------------------------------------------------
	~CXTPStatusBarSliderPane();

protected:

	//----------------------------------------------------------------------
	// Summary:
	//     This method is called to retrieve pointer to CXTPScrollBarPaintManager.
	//----------------------------------------------------------------------
	virtual CXTPScrollBarPaintManager* GetScrollBarPaintManager() const;

public:

	//-----------------------------------------------------------------------
	// Summary: Call this method to set ticks for slider
	// Input:   pTicks - Pointer to double array of ticks
	//          nCount - Count of elements in array.
	//-----------------------------------------------------------------------
	void SetTicks(double* pTicks, int nCount);

#ifdef _XTP_ACTIVEX
//{{AFX_CODEJOCK_PRIVATE
	DECLARE_DISPATCH_MAP()
	DECLARE_INTERFACE_MAP()
	DECLARE_OLETYPELIB_EX(CXTPStatusBarSliderPane);
	void OleSetTooltipPart(int nPart, LPCTSTR lpszTooltip);
	void OleSetTicks(const VARIANT& Ticks);
//}}AFX_CODEJOCK_PRIVATE
#endif
};


#include "Common/Base/Diagnostic/XTPEnableNoisyWarnings.h"
#endif // !defined(__XTPSTATUSBARPANESLIDER_H__)
