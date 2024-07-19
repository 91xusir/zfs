// XTPTreeTheme.h: interface for the CXTPTreeTheme class.
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
#if !defined(__XTPTREETHEME_H__)
#define __XTPTREETHEME_H__
//}}AFX_CODEJOCK_PRIVATE

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Common/Base/Diagnostic/XTPDisableNoisyWarnings.h"

class CXTPTreeBase;

//===========================================================================
// Summary:
//     CXTPTreeTheme is a class used to perform Tree control Theme
//     drawing tasks.  This is the base class for all tree themes.
//===========================================================================
class _XTP_EXT_CLASS CXTPTreeTheme : public CXTPControlTheme
{
public:
	// -------------------------------------------------------------------
	// Summary: Constructs a CXTPTreeTheme object
	// -------------------------------------------------------------------
	CXTPTreeTheme();

	// -------------------------------------------------------------------
	// Summary: Destroys a CXTPTreeTheme object, handles cleanup and deallocation
	// -------------------------------------------------------------------
	virtual ~CXTPTreeTheme();

	// -------------------------------------------------------------------
	// Summary: This member function performs initialization for the tree theme.
	// Parameters:
	//          pTreeBase - Points to a valid CXTPTreeBase object.
	// Remarks: Initializes all drawing defaults such as colors.
	// -------------------------------------------------------------------
	virtual void RefreshMetrics(CXTPTreeBase* pTreeBase);

	// -------------------------------------------------------------------
	// Summary: This member function is used to retrieve the tree background color.
	// Returns: An RGB value representing the tree background color.
	// -------------------------------------------------------------------
	virtual COLORREF GetTreeBackColor() const;

	// -------------------------------------------------------------------
	// Summary: This member function is used to retrieve the tree text color.
	// Returns: An RGB value representing the tree text color.
	// -------------------------------------------------------------------
	virtual COLORREF GetTreeTextColor() const;

	// -------------------------------------------------------------------
	// Summary: This member function is used to retrieve the tree text color depending on the tree item state.
	// Parameters:
	//          uState        - TVIS state flags.
	//          bTreeHasFocus - TRUE if the tree has input focus.
	//          dwStyle       - TVS style flags.
	//          crBackDef     - Default background color to be returned.
	//          pTreeBase     - Points to a valid CXTPTreeBase object.
	// Returns: An RGB value representing the tree item state text color.
	// -------------------------------------------------------------------
	virtual COLORREF GetItemTextColor(UINT uState, bool bTreeHasFocus, DWORD dwStyle, COLORREF crBackDef, CTreeCtrl* pTreeBase) const;

	// -------------------------------------------------------------------
	// Summary: This member function is used to retrieve the tree back color depending on the tree item state.
	// Parameters:
	//          uState        - TVIS state flags.
	//          bTreeHasFocus - TRUE if the tree has input focus.
	//          dwStyle       - TVS style flags.
	//          crTextDef     - Default text color to be returned.
	//          pTreeBase     - Points to a valid CXTPTreeBase object.
	// Returns: An RGB value representing the tree item state back color.
	// -------------------------------------------------------------------
	virtual COLORREF GetItemBackColor(UINT uState, bool bTreeHasFocus, DWORD dwStyle, COLORREF crTextDef, CTreeCtrl* pTreeBase) const;

	// -------------------------------------------------------------------
	// Summary: This member function is used to determine if a focus retangle should be drawn around tree items that have input focus.
	// Returns: TRUE  if a focust rectangle should be drawn, otherwise FALSE.
	// -------------------------------------------------------------------
	BOOL IsDrawFocusRect() const { return m_bDrawFocusRect; }

	// -------------------------------------------------------------------
	// Summary: This member function is used to determine if a dark theme is used.
	// Returns: TRUE when using a darker background theme, FALSE when using a lighter background theme.
	// -------------------------------------------------------------------
	virtual BOOL IsThemeDark() const { return m_bDarkTheme; }

protected:
    COLORREF m_crText;                 // Tree item text color.
	COLORREF m_crTextDisabled;         // Tree item disabled text color.
    COLORREF m_crTextSelected;         // Tree item selected text color.
    COLORREF m_crTextSelectedInactive; // Tree item selected inactive text color when TVS_SHOWSELALWAYS is used.
    COLORREF m_crTextSelectedDisabled; // Tree item selected disabled text color when TVS_SHOWSELALWAYS is used.
    COLORREF m_crBack;                 // Tree item background color.
	COLORREF m_crBackDisabled;         // Tree item disabled background color.
    COLORREF m_crBackSelected;         // Tree item selected background color.
    COLORREF m_crBackSelectedInactive; // Tree item selected inactive background color when TVS_SHOWSELALWAYS is used.
    COLORREF m_crBackSelectedDisabled; // Tree item selected disabled background color when TVS_SHOWSELALWAYS is used.
	BOOL     m_bDrawFocusRect;         // TRUE to draw a focus rectangle when a tree item has input focus.
	BOOL     m_bDarkTheme;             // TRUE when using a darker background theme, FALSE when using a lighter background theme.
};

//===========================================================================
// Summary:
//     CXTPTreeVisualStudio2012Theme is a CXTPTreeTheme derived class used to 
//     render a Visual Studio 2012 style tree.
//===========================================================================
class _XTP_EXT_CLASS CXTPTreeVisualStudio2012Theme : public CXTPTreeTheme
{
public:
	// -------------------------------------------------------------------
	// Summary: Constructs a CXTPTreeVisualStudio2012Theme object
	// Parameters: 
	//          bLight - TRUE to use Light theme, FALSE to use Dark theme.
	// -------------------------------------------------------------------
	CXTPTreeVisualStudio2012Theme(BOOL bLight = TRUE);

	// -------------------------------------------------------------------
	// Summary: This member function performs initialization for the tree theme.
	// Parameters:
	//          pTreeBase - Points to a valid CXTPTreeBase object.
	// Remarks: Initializes all drawing defaults such as colors.
	// -------------------------------------------------------------------
	virtual void RefreshMetrics(CXTPTreeBase* pTreeBase);
};

//===========================================================================
// Summary:
//     CXTPTreeOffice2013Theme is a CXTPTreeTheme derived class used to 
//     render a Office 2013 style tree.
//===========================================================================
class _XTP_EXT_CLASS CXTPTreeOffice2013Theme : public CXTPTreeTheme
{
public:
	// -------------------------------------------------------------------
	// Summary: Constructs a CXTPTreeOffice2013Theme object
	// -------------------------------------------------------------------
	CXTPTreeOffice2013Theme();

	// -------------------------------------------------------------------
	// Summary: This member function performs initialization for the tree theme.
	// Parameters:
	//          pTreeBase - Points to a valid CXTPTreeBase object.
	// Remarks: Initializes all drawing defaults such as colors.
	// -------------------------------------------------------------------
	virtual void RefreshMetrics(CXTPTreeBase* pTreeBase);
};

//===========================================================================
// Summary:
//     CXTPTreeVisualStudio2015Theme is a CXTPTreeTheme derived class used to 
//     render a Visual Studio 2015 style tree.
//===========================================================================
class _XTP_EXT_CLASS CXTPTreeVisualStudio2015Theme : public CXTPTreeOffice2013Theme
{
public:
	// -------------------------------------------------------------------
	// Summary: Constructs a CXTPTreeVisualStudio2015Theme object
	// -------------------------------------------------------------------
	CXTPTreeVisualStudio2015Theme();
};


#include "Common/Base/Diagnostic/XTPEnableNoisyWarnings.h"
#endif // !defined(__XTPTREETHEME_H__)
