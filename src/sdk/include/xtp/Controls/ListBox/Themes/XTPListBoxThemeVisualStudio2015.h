// XTPListBoxThemeVisualStudio2015.h
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
#if !defined(__XTPLISTBOXTHEMEVISUALSTUDIO2015_H__)
#define __XTPLISTBOXTHEMEVISUALSTUDIO2015_H__
//}}AFX_CODEJOCK_PRIVATE

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Common/Base/Diagnostic/XTPDisableNoisyWarnings.h"

//===========================================================================
// Summary:
//     CXTPListBoxThemeVisualStudio2015 is used by CXTPListBox to draw an Office
//     2013 style list box.
//===========================================================================
class _XTP_EXT_CLASS CXTPListBoxThemeVisualStudio2015 : public CXTPListBoxThemeOffice2013
{
public:

	//-----------------------------------------------------------------------
	// Summary: Default constructor.
	//-----------------------------------------------------------------------
	CXTPListBoxThemeVisualStudio2015();
};


#include "Common/Base/Diagnostic/XTPEnableNoisyWarnings.h"
#endif // !defined(__XTPLISTBOXTHEMEVISUALSTUDIO2015_H__)
