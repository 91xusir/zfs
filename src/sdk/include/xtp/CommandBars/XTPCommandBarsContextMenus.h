// XTPCommandBarsContextMenus.h
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
#if !defined(__XTPCOMMANDBARSCONTEXTMENUS_H__)
#define __XTPCOMMANDBARSCONTEXTMENUS_H__
//}}AFX_CODEJOCK_PRIVATE

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Common/Base/Diagnostic/XTPDisableNoisyWarnings.h"

//===========================================================================
// Summary:
//     CXTPCommandBarsContextMenus is CXTPCommandBarList derived class,
//     represents collection of context menus
//===========================================================================
class _XTP_EXT_CLASS CXTPCommandBarsContextMenus : public CXTPCommandBarList
{
public:

	//-----------------------------------------------------------------------
	// Summary:
	//     Constructs a CXTPCommandBarsContextMenus object
	// Parameters:
	//     pCommandBars - Parent CXTPCommandBars object
	//-----------------------------------------------------------------------
	CXTPCommandBarsContextMenus(CXTPCommandBars* pCommandBars);

public:

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this method to add new context menu to collection
	// Parameters:
	//     nIDBar - Menu resource identifier
	//     pCommandBar - CommandBar to be added
	//     lpszTitle - Caption of context menu
	//     pSubMenu - Context menu to convert
	//     nSubMenu - Index of submenu to add
	// Returns:
	//     Pointer to added context menu.
	//-----------------------------------------------------------------------
	CXTPCommandBar* Add(UINT nIDBar);
	CXTPCommandBar* Add(UINT nIDBar, UINT nIDMenuResource, int nSubMenu); // <combine CXTPCommandBarsContextMenus::Add@UINT>
	CXTPCommandBar* Add(CXTPCommandBar* pCommandBar); // <combine CXTPCommandBarsContextMenus::Add@UINT>
	CXTPCommandBar* Add(UINT nIDBar, LPCTSTR lpszTitle, CMenu* pSubMenu); // <combine CXTPCommandBarsContextMenus::Add@UINT>

//{{AFX_CODEJOCK_PRIVATE
#ifdef _XTP_ACTIVEX

	DECLARE_DISPATCH_MAP()
	DECLARE_OLETYPELIB_EX(CXTPCommandBarsContextMenus);

	afx_msg LPDISPATCH OleGetItem(long nIndex);
	afx_msg LPDISPATCH OleItem(long nIndex);
	afx_msg long OleGetItemCount();
	afx_msg LPDISPATCH OleFind(int nId);
	afx_msg LPDISPATCH OleAdd(int nId, LPCTSTR strCaption);
	afx_msg void OleAddPopup(LPDISPATCH lpDispatch);

	DECLARE_ENUM_VARIANT(CXTPCommandBarsContextMenus);

#endif
//}}AFX_CODEJOCK_PRIVATE
};



#include "Common/Base/Diagnostic/XTPEnableNoisyWarnings.h"
#endif // !defined(__XTPCOMMANDBARSCONTEXTMENUS_H__)
