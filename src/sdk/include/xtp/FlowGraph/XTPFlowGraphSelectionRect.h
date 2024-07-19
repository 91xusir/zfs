// XTPFlowGraphSelectionRect.h: interface for the CXTPFlowGraphSelectionRect class.
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
#if !defined(__XTPFLOWGRAPHVISIOSELECTIONRECT_H__)
#define __XTPFLOWGRAPHVISIOSELECTIONRECT_H__
//}}AFX_CODEJOCK_PRIVATE

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Common/Base/Diagnostic/XTPDisableNoisyWarnings.h"

class CXTPFlowGraphDrawContext;

// constants are in the same order as rectangles appear after GetSizingRects method
enum SelectionRectHitTest
{
	SelectionRectHitTestTopLeft = 0,
	SelectionRectHitTestTopCenter,
	SelectionRectHitTestTopRight,
	SelectionRectHitTestMiddleRight,
	SelectionRectHitTestBottomRight,
	SelectionRectHitTestBottomCenter,
	SelectionRectHitTestBottomLeft,
	SelectionRectHitTestMiddleLeft,
	SelectionRectHitTestNone
};

class _XTP_EXT_CLASS CXTPFlowGraphSelectionRect
{
protected:

	static int m_iSizeSquareSize;

	static void AFX_CDECL GetSizingRects(const CRect &rc, CRect *rects);

public:

	CXTPFlowGraphSelectionRect();
	virtual ~CXTPFlowGraphSelectionRect();

	void Draw(CXTPFlowGraphDrawContext* pDC, const CRect &rc);

	static SelectionRectHitTest AFX_CDECL HitTest(const CRect &rc, const CPoint &point);

	static void AFX_CDECL ResizeRect(CRect &rcRect, CSize szMinSize, CPoint ptOffset, SelectionRectHitTest hitPoint);

	static UINT AFX_CDECL GetCursorBySelectionHitTest(SelectionRectHitTest hitTest);
};


#include "Common/Base/Diagnostic/XTPEnableNoisyWarnings.h"
#endif //#if !defined(__XTPFLOWGRAPHVISIOSELECTIONRECT_H__)
