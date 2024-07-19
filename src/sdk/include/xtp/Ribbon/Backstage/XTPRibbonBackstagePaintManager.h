// XTPRibbonBackstagePaintManager.h: interface for the CXTPRibbonBackstagePaintManager class.
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
#if !defined(__XTPRIBBONBACKSTAGEPAINTMANAGER_H__)
#define __XTPRIBBONBACKSTAGEPAINTMANAGER_H__
//}}AFX_CODEJOCK_PRIVATE

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Common/Base/Diagnostic/XTPDisableNoisyWarnings.h"

class CXTPRibbonBackstageView;
class CXTPControl;
class CXTPPaintManager;

class _XTP_EXT_CLASS CXTPRibbonBackstagePaintManager
{
// Construction / Destruction
public:

	CXTPRibbonBackstagePaintManager();

	virtual ~CXTPRibbonBackstagePaintManager();

// Drawing interface
public:
	virtual void DrawBackground(CDC *pDC, CXTPRibbonBackstageView *pView);

	virtual void DrawControl(CDC *pDC, CXTPRibbonBackstageView *pView, CXTPControl *pControl);
	virtual void DrawKeyboardTip(CDC *pDC, CXTPPaintManager *pPaintManager, CXTPControl *pControl);

	virtual int GetMenuItemCommandHeight() const;
	virtual int GetMenuItemPageHeight() const;

	virtual int GetMenuItemCommandGapX() const;
	virtual int GetMenuItemCommandGapY() const;
	virtual int GetMenuItemPageGapY() const;

	virtual int GetMenuItemTextGap() const;
	virtual int GetMenuImageMargin() const;
	virtual int GetMenuSeparatorHeight() const;

public:

	int m_nCommandGap;
	int m_nCommandGapY;
	int m_nMenuItemCommandHeight;
	int m_nMenuItemPageHeight;
	int m_nMenuItemPageGapY;
	int m_nTextGap;
	int m_nImageMargin;
	int m_nMenuSeparatorHeight;
};


#include "Common/Base/Diagnostic/XTPEnableNoisyWarnings.h"
#endif // !defined(__XTPRIBBONBACKSTAGEPAINTMANAGER_H__)
