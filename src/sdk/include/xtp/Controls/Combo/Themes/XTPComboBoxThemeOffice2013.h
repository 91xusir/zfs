// XTPComboBoxThemeOffice2013.h: interface for the CXTPComboBoxThemeOffice2013 class.
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
#if !defined(__XTPCOMBOBOXTHEMEOFFICE2013_H__)
#define __XTPCOMBOBOXTHEMEOFFICE2013_H__
//}}AFX_CODEJOCK_PRIVATE

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Common/Base/Diagnostic/XTPDisableNoisyWarnings.h"

//===========================================================================
// Summary: Used as the paint manager for combo-box to render an Office 2013
//          style appearance.
//===========================================================================
class _XTP_EXT_CLASS CXTPComboBoxThemeOffice2013 : public CXTPComboBoxThemeVisualStudio
{
public:

	//-----------------------------------------------------------------------
	// Summary: Default constructor
	//-----------------------------------------------------------------------
	CXTPComboBoxThemeOffice2013();

	//-----------------------------------------------------------------------
	// Summary: Destructor handles cleanup and deallocation.
	//-----------------------------------------------------------------------
	virtual ~CXTPComboBoxThemeOffice2013();

	//-----------------------------------------------------------------------
	// Summary: This member function is called by the theme manager to refresh
	//          colors and metrics used by the theme manager.
	// Input:   pComboBox : Points to a CXTPComboBox object.
	//-----------------------------------------------------------------------
	virtual void RefreshMetrics(CXTPComboBox* pComboBox);

	//-----------------------------------------------------------------------
	// Summary: Called by the paint manager to draw the button portion of the
	//          combo-box control.
	// Input:   pDC       : Points to the device context for the combo-box.
	//          pComboBox : Points to a CXTPComboBox object.
	//          rcBtn     : Size of the button to draw.
	//-----------------------------------------------------------------------
	virtual void DrawButton(CDC *pDC, CXTPComboBox* pComboBox, CRect rcBtn);

	//-----------------------------------------------------------------------
	// Summary: Called by the paint manager to draw the drop down arrow for
	//          the combo-box drop button.
	// Input:   pDC       : Points to the device context for the combo-box.
	//          pComboBox : Points to a CXTPComboBox object.
	//          rcBtn     : Size of the button to draw.
	//-----------------------------------------------------------------------
	virtual void DrawButtonTriangle(CDC *pDC, CXTPComboBox* pComboBox, CRect rcBtn);

protected:

	CXTPPaintManagerColor m_clrButtonDisabled;      // Drop down button disabled color.
	CXTPPaintManagerColor m_clrButtonArrowDisabled; // Drop down arrow disabled color.
};



#include "Common/Base/Diagnostic/XTPEnableNoisyWarnings.h"
#endif // __XTPCOMBOBOXTHEMEOFFICE2013_H__
