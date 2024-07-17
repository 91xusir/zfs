// XTPRibbonBackstageLabel.h: interface for the CXTPRibbonBackstageLabel class.
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
#if !defined(__XTPRIBBONBACKSTAGELABEL_H__)
#define __XTPRIBBONBACKSTAGELABEL_H__
//}}AFX_CODEJOCK_PRIVATE

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Common/Base/Diagnostic/XTPDisableNoisyWarnings.h"

class _XTP_EXT_CLASS CXTPRibbonBackstageLabel : public CStatic
{
public:
	CXTPRibbonBackstageLabel();
	virtual ~CXTPRibbonBackstageLabel();

public:
	void SetTextColor(COLORREF clrText);
	virtual void SetTheme(XTPControlTheme nTheme);
	virtual void RefreshMetrics();

	//{{AFX_VIRTUAL(CXTPRibbonBackstageLabel)
	//}}AFX_VIRTUAL

protected:
	DECLARE_MESSAGE_MAP()
	//{{AFX_MSG(CXTPRibbonBackstageLabel)
	afx_msg void OnPaint();
	//}}AFX_MSG

protected:
	COLORREF m_clrText;
	COLORREF m_clrBack;
};



AFX_INLINE void CXTPRibbonBackstageLabel::SetTextColor(COLORREF clrText)
{
	m_clrText = clrText;
}


#include "Common/Base/Diagnostic/XTPEnableNoisyWarnings.h"
#endif // !defined(__XTPRIBBONBACKSTAGELABEL_H__)
