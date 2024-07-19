// XTPMarkupStatic.h : header file
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
#if !defined(__MARKUPSTATIC_H__)
#define __MARKUPSTATIC_H__
//}}AFX_CODEJOCK_PRIVATE

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Common/Base/Diagnostic/XTPDisableNoisyWarnings.h"

#ifdef _XTP_INCLUDE_MARKUP

class CXTPMarkupUIElement;
class CXTPMarkupContext;

// ----------------------------------------------------------------------
// Summary: Represents a STATIC control with ability to render
//          markup content.
// ----------------------------------------------------------------------
class _XTP_EXT_CLASS CXTPMarkupStatic : public CStatic
{
public:
	// ----------------------------------------------------------------------
	// Summary: Constructs object instance.
	// ----------------------------------------------------------------------
	CXTPMarkupStatic();

	// ----------------------------------------------------------------------
	// Summary: Performs object destruction
	// ----------------------------------------------------------------------
	virtual ~CXTPMarkupStatic();

public:
	// ----------------------------------------------------------------------
	// Summary: Sets makrup content to draw.
	// Parameters: lpszMarkup - Markup content to draw.
	// ----------------------------------------------------------------------
	void SetMarkupText(LPCSTR lpszMarkup);
	void SetMarkupText(LPCWSTR lpszMarkup); // <combine CXTPMarkupStatic::SetMarkupText@LPCSTR>

	// ----------------------------------------------------------------------
	// Parameters:
	//     lpszMarkup - Markup text.
	//     nCodePage - Optional code page. Default value is CP_UTF8. Ignored
	//                 is markup charset is same as building charset.
	// Summary: Special version used by the Ribbon Cusomize dialog and will
	//          handle % to load a string from a resouce
	// ----------------------------------------------------------------------
	void SetMarkupTextEx(LPCSTR lpszMarkup, UINT nCodePage = CP_UTF8);
	void SetMarkupTextEx(LPCWSTR lpszMarkup, UINT nCodePage = CP_UTF8); // <combine CXTPMarkupStatic::SetMarkupTextEx@LPCSTR@UINT>

	// ----------------------------------------------------------------------
	// Summary: Obtain control's markup context.
	// Returns: Control's markup context pointer.
	// ----------------------------------------------------------------------
	CXTPMarkupContext* GetMarkupContext() const;

protected:
	DECLARE_MESSAGE_MAP()

	//{{AFX_VIRTUAL(CXTPMarkupStatic)
	BOOL OnWndMsg(UINT message, WPARAM wParam, LPARAM lParam, LRESULT* pResult);
	//}}AFX_VIRTUAL

	//{{AFX_MSG(CXTPMarkupStatic)
	afx_msg void OnPaint();
	//}}AFX_MSG

	CXTPMarkupUIElement* m_pUIElement;
	CXTPMarkupContext* m_pMarkupContext;
};

/////////////////////////////////////////////////////////////////////////////

AFX_INLINE CXTPMarkupContext* CXTPMarkupStatic::GetMarkupContext() const {
	return m_pMarkupContext;
}

#endif

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.


#include "Common/Base/Diagnostic/XTPEnableNoisyWarnings.h"
#endif // !defined(__MARKUPSTATIC_H__)
