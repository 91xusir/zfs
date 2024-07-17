// XTPRibbonBackstagePage.h: interface for the CXTPRibbonBackstagePage class.
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
#if !defined(__XTPRIBBONBACKSTAGEPAGE_H__)
#define __XTPRIBBONBACKSTAGEPAGE_H__
//}}AFX_CODEJOCK_PRIVATE

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Common/Base/Diagnostic/XTPDisableNoisyWarnings.h"

class _XTP_EXT_CLASS CXTPRibbonBackstagePage : public CXTPResizePropertyPage
{
	DECLARE_DYNAMIC(CXTPRibbonBackstagePage)
public:
	CXTPRibbonBackstagePage(UINT nID);

	~CXTPRibbonBackstagePage();

	virtual XTPControlTheme GetTheme() const;
	virtual void SetTheme(const XTPControlTheme nTheme);

protected:
	DECLARE_MESSAGE_MAP()
	//{{AFX_MSG(CXTPRibbonBackstagePage)
	afx_msg HBRUSH OnCtlColor(CDC *pDC, CWnd *pWnd, UINT nCtlColor);
	//}}AFX_MSG


protected:
	XTPControlTheme m_nTheme;

	CXTPFont m_xtpFontCaption;
	CXTPFont m_xtpFontTitle;

	XTP_SUBSTITUTE_GDI_MEMBER_WITH_CACHED(CFont, m_fntCaption, m_xtpFontCaption, GetCaptionFontHandle);
	XTP_SUBSTITUTE_GDI_MEMBER_WITH_CACHED(CFont, m_fntTitle, m_xtpFontTitle, GetTitleFontHandle);

	CXTPBrush m_xtpBrushBack;
	XTP_SUBSTITUTE_GDI_MEMBER_WITH_CACHED(CBrush, m_brBack, m_xtpBrushBack, GetBackBrushHandle);

	COLORREF m_clrBack;
	COLORREF m_clrText;
};


#include "Common/Base/Diagnostic/XTPEnableNoisyWarnings.h"
#endif // !defined(__XTPRIBBONBACKSTAGEPAGE_H__)
