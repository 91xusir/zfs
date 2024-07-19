// XTPStatusBarPaneProgress.h
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
#if !defined(__XTPSTATUSBARPANEPROGRESS_H__)
#define __XTPSTATUSBARPANEPROGRESS_H__
//}}AFX_CODEJOCK_PRIVATE

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Common/Base/Diagnostic/XTPDisableNoisyWarnings.h"

//===========================================================================
// Summary:
//     CXTPStatusBarProgressPane is a CXTPStatusBarPane derived class. CXTPStatusBarProgressPane works with
//     CXTStatusBar and allows you to create progress bar in your status bar area.
//===========================================================================
class _XTP_EXT_CLASS CXTPStatusBarProgressPane : public CXTPStatusBarPane, public CXTPProgressBase
{
	DECLARE_DYNAMIC(CXTPStatusBarProgressPane)
public:
	//-----------------------------------------------------------------------
	// Summary:
	//     Constructs a CXTPStatusBarProgressPane object
	//-----------------------------------------------------------------------
	CXTPStatusBarProgressPane();

	//-----------------------------------------------------------------------
	// Summary:
	//     Destroys a CXTPStatusBarProgressPane object, handles cleanup and deallocation
	//-----------------------------------------------------------------------
	~CXTPStatusBarProgressPane();

protected:
	//----------------------------------------------------------------------
	// Summary:
	//     This method is called to redraw progress bar.
	//----------------------------------------------------------------------
	virtual void RedrawProgress();

	//----------------------------------------------------------------------
	// Summary:
	//     This method is called to retrieve pointer to CXTPProgressPaintManager.
	//----------------------------------------------------------------------
	virtual CXTPProgressPaintManager* GetProgressPaintManager() const;

	//----------------------------------------------------------------------
	// Summary:
	//     This method is called to get bounding rectangle of progressbar
	//----------------------------------------------------------------------
	virtual CRect GetProgressRect();

	//-----------------------------------------------------------------------
	// Summary:
	//     This method is called to draw pane item. Override it to draw custom pane.
	// Parameters:
	//     pDC    - Pointer to a valid device context
	//     rcItem - Item rectangle
	//-----------------------------------------------------------------------
	virtual void OnDraw(CDC* pDC, CRect rcItem);

public:
#ifdef _XTP_ACTIVEX
//{{AFX_CODEJOCK_PRIVATE
	DECLARE_DISPATCH_MAP()
	DECLARE_INTERFACE_MAP()
	DECLARE_OLETYPELIB_EX(CXTPStatusBarProgressPane);

	int OleGetPos();
	void OleSetPos(int nPos);
	void OleRedrawProgress();
//}}AFX_CODEJOCK_PRIVATE
#endif
};


#include "Common/Base/Diagnostic/XTPEnableNoisyWarnings.h"
#endif // !defined(__XTPSTATUSBARPANEPROGRESS_H__)
