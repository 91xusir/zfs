// XTPTaskPanelVisualStudio2015Theme.h : implementation of the CXTPTaskPanelVisualStudio2015Theme class.
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
#if !defined(__XTPTASKPANELVISUALSTUDIO2015THEME_H__)
#define __XTPTASKPANELVISUALSTUDIO2015THEME_H__
//}}AFX_CODEJOCK_PRIVATE

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Common/Base/Diagnostic/XTPDisableNoisyWarnings.h"

//===========================================================================
// Summary:  The CXTPTaskPanelVisualStudio2015Theme class is used to enable a 
//           Visutal Studio 2015 style theme.
// See Also: CXTPTaskPanel::SetTheme
//===========================================================================
class _XTP_EXT_CLASS CXTPTaskPanelVisualStudio2015Theme : public CXTPTaskPanelVisualStudio2012Theme
{
protected:

	void Triangle(CDC* pDC, POINT *pPoints, COLORREF clr, BOOL bFilled);

public:

	//-------------------------------------------------------------------------
	// Summary: Constructs a CXTPTaskPanelVisualStudio2015Theme object.
	//-------------------------------------------------------------------------
	CXTPTaskPanelVisualStudio2015Theme();
	
	virtual ~CXTPTaskPanelVisualStudio2015Theme();

	//-------------------------------------------------------------------------
	// Summary: This method is called to refresh the visual metrics of task panel.
	//-------------------------------------------------------------------------
	virtual void RefreshMetrics();
};


#include "Common/Base/Diagnostic/XTPEnableNoisyWarnings.h"
#endif // __XTPTASKPANELVISUALSTUDIO2015THEME_H__
