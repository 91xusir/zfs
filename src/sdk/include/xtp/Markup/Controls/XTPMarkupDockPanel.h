// XTPMarkupDockPanel.h: interface for the CXTPMarkupDockPanel class.
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
#if !defined(__XTPMARKUPDOCKPANEL_H__)
#define __XTPMARKUPDOCKPANEL_H__
//}}AFX_CODEJOCK_PRIVATE

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Common/Base/Diagnostic/XTPDisableNoisyWarnings.h"

class CXTPMarkupUIElement;
class CXTPMarkupDrawingContext;
class CXTPMarkupObject;
class CXTPMarkupPropertyChangedEventArgs;
class CXTPMarkupBuilder;
class CXTPMarkupDependencyProperty;

enum XTPMarkupDock
{
	xtpMarkupDockLeft,
	xtpMarkupDockTop,
	xtpMarkupDockRight,
	xtpMarkupDockBottom
};

//===========================================================================
// Summary: CXTPMarkupDockPanel is CXTPMarkupPanel derived class. It implements DockPanel XAML Tag
//===========================================================================
class _XTP_EXT_CLASS CXTPMarkupDockPanel : public CXTPMarkupPanel
{
	DECLARE_MARKUPCLASS(CXTPMarkupDockPanel)
protected:
	//-----------------------------------------------------------------------
	// Summary:
	//     Constructs a CXTPMarkupDockPanel object
	//-----------------------------------------------------------------------
	CXTPMarkupDockPanel();

	//-----------------------------------------------------------------------
	// Summary:
	//     Destroys a CXTPMarkupDockPanel object, handles cleanup and deallocation
	//-----------------------------------------------------------------------
	virtual ~CXTPMarkupDockPanel();

public:
	//-----------------------------------------------------------------------
	// Summary:
	//     Call this method to set dock position of the child elements
	// Parameters:
	//     pElement - Child element to set dock position to
	//     dock - Dock Position of element
	//-----------------------------------------------------------------------
	static void AFX_CDECL SetDock(CXTPMarkupUIElement* pElement, XTPMarkupDock dock);


	//-----------------------------------------------------------------------
	// Summary:
	//     Determines dock position of the child elements
	// Parameters:
	//     pElement - Child element to get dock position.
	//-----------------------------------------------------------------------
	static XTPMarkupDock AFX_CDECL GetDock(CXTPMarkupUIElement* pElement);

public:
	//-----------------------------------------------------------------------
	// Summary:
	//     Call this method to fill last child element inside whole client area of DockPanel.
	// Parameters:
	//     bLastChildFill - TRUE to fill last child element inside client area.
	//-----------------------------------------------------------------------
	void SetLastChildFill(BOOL bLastChildFill);

	//-----------------------------------------------------------------------
	// Summary:
	//     Determines if last child element is filled inside whole client area of DockPanel.
	// Returns:
	//     TRUE to fill last child element inside client area; FALSE otherwise
	//-----------------------------------------------------------------------
	BOOL GetLastChildFill() const;


protected:
//{{AFX_CODEJOCK_PRIVATE
	// Implementation
	virtual CSize MeasureOverride(CXTPMarkupDrawingContext* pDC, CSize szAvailableSize);
	virtual CSize ArrangeOverride(CSize szFinalSize);

private:
	static void AFX_CDECL OnDockChanged(CXTPMarkupObject* d, CXTPMarkupPropertyChangedEventArgs* e);
	static CXTPMarkupObject* AFX_CDECL ConvertDock(
		CXTPMarkupBuilder* pBuilder,
		CXTPMarkupObject* pObject,
		CXTPMarkupDependencyProperty* pAssociatedProperty = NULL);

public:
	static CXTPMarkupDependencyProperty* m_pLastChildFillProperty;
	static CXTPMarkupDependencyProperty* m_pDockProperty;
//}}AFX_CODEJOCK_PRIVATE


//{{AFX_CODEJOCK_PRIVATE
public:
	DECLARE_DISPATCH_MAP()
#ifdef _XTP_ACTIVEX
	DECLARE_INTERFACE_MAP()
	DECLARE_OLETYPELIB_EX(CXTPMarkupDockPanel);
#endif

	afx_msg BOOL OleGetLastChildFill();
	afx_msg void OleSetLastChildFill(BOOL nValue);
	afx_msg int OleGetDock(LPDISPATCH lpElementDisp);
	afx_msg void OleSetDock(LPDISPATCH lpElementDisp, int nValue);
//}}AFX_CODEJOCK_PRIVATE
};


#include "Common/Base/Diagnostic/XTPEnableNoisyWarnings.h"
#endif // !defined(__XTPMARKUPDOCKPANEL_H__)
