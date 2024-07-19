// XTPMarkupRowDefinition.h: interface for the CXTPMarkupRowDefinition class.
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
#if !defined(__XTPMARKUPROWDEFINITION_H__)
#define __XTPMARKUPROWDEFINITION_H__
//}}AFX_CODEJOCK_PRIVATE

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Common/Base/Diagnostic/XTPDisableNoisyWarnings.h"

class CXTPMarkupDependencyProperty;
class CXTPMarkupGridLength;

//===========================================================================
// Summary: CXTPMarkupRowDefinition is base CXTPMarkupDefinitionBase derived class.
//          It implements RowDefinition XAML Tag
//===========================================================================
class _XTP_EXT_CLASS CXTPMarkupRowDefinition : public CXTPMarkupDefinitionBase
{
	DECLARE_MARKUPCLASS(CXTPMarkupRowDefinition);
public:
	//-----------------------------------------------------------------------
	// Summary:
	//     Constructs a CXTPMarkupRowDefinition object
	//-----------------------------------------------------------------------
	CXTPMarkupRowDefinition();

public:
	//-----------------------------------------------------------------------
	// Summary:
	//     Call this method to set minimum height constraint for the row
	// Parameters:
	//     nHeight - New minimum height constraint
	//-----------------------------------------------------------------------
	void SetMinHeight(int nHeight);

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this method to set maximum height constraint for row
	// Parameters:
	//     nHeight - New maximum height constraint
	//-----------------------------------------------------------------------
	void SetMaxHeight(int nHeight);

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this method to set height for the row
	// Parameters:
	//     nHeight - New height for the row
	//-----------------------------------------------------------------------
	void SetHeight(int nHeight);

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this method to set height for the row
	// Parameters:
	//     pLength - New height for the row
	//-----------------------------------------------------------------------
	void SetHeight(CXTPMarkupGridLength* pLength);

//{{AFX_CODEJOCK_PRIVATE
	// Implementation
public:
	static CXTPMarkupDependencyProperty* m_pHeightProperty;
	static CXTPMarkupDependencyProperty* m_pMinHeightProperty;
	static CXTPMarkupDependencyProperty* m_pMaxHeightProperty;
//}}AFX_CODEJOCK_PRIVATE

//{{AFX_CODEJOCK_PRIVATE
public:
	DECLARE_DISPATCH_MAP()
#ifdef _XTP_ACTIVEX
	DECLARE_INTERFACE_MAP()
	DECLARE_OLETYPELIB_EX(CXTPMarkupRowDefinition);
#endif

	afx_msg void OleSetMinHeight(int nHeight);
	afx_msg void OleSetMaxHeight(int nHeight);
	afx_msg void OleSetHeight(int nHeight);
	afx_msg int OleGetMinHeight();
	afx_msg int OleGetMaxHeight();
	afx_msg int OleGetHeight();
	afx_msg int OleGetUnitType();
	afx_msg void OleSetUnitType(int nType);
//}}AFX_CODEJOCK_PRIVATE
};


#include "Common/Base/Diagnostic/XTPEnableNoisyWarnings.h"
#endif // !defined(__XTPMARKUPROWDEFINITION_H__)
