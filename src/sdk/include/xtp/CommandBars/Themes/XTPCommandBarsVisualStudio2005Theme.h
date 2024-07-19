// XTPCommandBarsVisualStudio2005Theme.h
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
#if !defined(__XTPCOMMANDBARSVISUALSTUDIO2005THEME_H__)
#define __XTPCOMMANDBARSVISUALSTUDIO2005THEME_H__
//}}AFX_CODEJOCK_PRIVATE

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Common/Base/Diagnostic/XTPDisableNoisyWarnings.h"

//===========================================================================
// Summary:
//     The CXTPVisualStudio2005Theme class is used to enable a Visual Studio 2005 style theme for Command Bars
// See Also: CXTPPaintManager::SetTheme
//===========================================================================
class _XTP_EXT_CLASS CXTPVisualStudio2005Theme : public CXTPCommandBarsOffice2003Theme
{
	DECLARE_DYNAMIC(CXTPVisualStudio2005Theme)
public:

	//-----------------------------------------------------------------------
	// Summary:
	//     Constructs a CXTPVisualStudio2005Theme object.
	//-----------------------------------------------------------------------
	CXTPVisualStudio2005Theme();

	//-----------------------------------------------------------------------
	// Summary:
	//     Destroys a CXTPVisualStudio2005Theme object, handles cleanup and deallocation
	//-----------------------------------------------------------------------
	virtual ~CXTPVisualStudio2005Theme();

	//-----------------------------------------------------------------------
	// Summary:
	//     This method is called to refresh the visual metrics of the manager.
	//-----------------------------------------------------------------------
	void RefreshMetrics();

public:

	//-----------------------------------------------------------------------
	// Summary:
	//     Retrieves the base theme of the manager.
	// Returns:
	//     Returns the base theme.
	//-----------------------------------------------------------------------
	virtual XTPPaintTheme BaseTheme() { return xtpThemeVisualStudio2005; }
};

//{{AFX_CODEJOCK_PRIVATE
typedef CXTPVisualStudio2005Theme CXTPWhidbeyTheme;
//}}AFX_CODEJOCK_PRIVATE


#include "Common/Base/Diagnostic/XTPEnableNoisyWarnings.h"
#endif // !defined(__XTPCOMMANDBARSVISUALSTUDIO2005THEME_H__)
