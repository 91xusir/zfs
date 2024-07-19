// XTPChartElementView.h
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
#if !defined(__XTPCHARTELEMENTVIEW_H__)
#define __XTPCHARTELEMENTVIEW_H__
//}}AFX_CODEJOCK_PRIVATE

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "Common/Base/Diagnostic/XTPDisableNoisyWarnings.h"

class CXTPChartDeviceCommand;
class CXTPChartDeviceContext;
class CXTPChartContainer;

//===========================================================================
// Summary:
//     This class represents the view of a chart element.All views have a
//     parent view and one or more child views.
// Remarks:
//     This a base class for the views of various chart elements like axis,
//     legend, diagram, tick marks etc.
//===========================================================================
class _XTP_EXT_CLASS CXTPChartElementView : public CXTPChartObject
{
	DECLARE_DYNAMIC(CXTPChartElementView);

public:
	//-----------------------------------------------------------------------
	// Summary:
	//     Constructs a CXTPChartElementView object.
	// Parameters:
	//     pParentView - A pointer to the parent view.
	//     bAddToParent - If TRUE, the view will be added to parent's child elements.
	//     pContainer - Parent container pointer.
	//-----------------------------------------------------------------------
	CXTPChartElementView(CXTPChartElementView* pParentView);
	CXTPChartElementView(CXTPChartElementView* pParentView, BOOL bAddToParent); // <combine CXTPChartElementView::CXTPChartElementView@CXTPChartElementView*>
	CXTPChartElementView(CXTPChartContainer* pContainer);  // <combine CXTPChartElementView::CXTPChartElementView@CXTPChartElementView*>

protected:
	//-------------------------------------------------------------------------
	// Summary:
	//     Destroys a CXTPChartElementView object, handles cleanup
	//-------------------------------------------------------------------------
	virtual ~CXTPChartElementView();

public:

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this function to get the number of child views associated
	//     with this view.
	// Returns:
	//     In integer specifying the number of children.
	// Remarks:
	//-----------------------------------------------------------------------
	int GetCount() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this function to get a child view at a particular index.
	// Returns:
	//     A pointer to a chart element view object.
	// Remarks:
	//-----------------------------------------------------------------------
	CXTPChartElementView* GetAt(int nIndex) const;

	//-------------------------------------------------------------------------
	// Summary:
	//     This function creates a CXTPChartDeviceCommand object, this object
	//     represents the rendering of the chart element.
	// Parameters:
	//     pDC     - Pointer to a CXTPChartDeviceContext object.
	// Returns:
	//     Returns CXTPChartDeviceCommand object, this object handles
	//     the rendering of an element in the chart.The derived class objects
	//     will return more specific device command objects according to their
	//     type.
	// Remarks:
	//     This is a virtual function, hence the sub-classes can override this
	//     function and can give specific implementation according to their type.
	//-------------------------------------------------------------------------
	virtual CXTPChartDeviceCommand* CreateDeviceCommand(CXTPChartDeviceContext* pDC);

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this function to get the parent view.
	// Returns:
	//     A pointer to a chart element view object.
	// Remarks:
	//-----------------------------------------------------------------------
	CXTPChartElementView* GetParentView() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this function to add a child view to the current view.
	// Parameters:
	//     pChildView - A pointer to a chart element view object.
	// Remarks:
	//-----------------------------------------------------------------------
	CXTPChartElementView* AddChildView(CXTPChartElementView* pChildView);

	//-----------------------------------------------------------------------
	// Summary:
	//      Provide access to the collection of the children elements
	//      owned by the view.
	// Returns:
	//      A pointer to the first element in the collection. The total number
	//      of the elements can be determined by use GetCount method.
	// See also:
	//      GetCount
	//-----------------------------------------------------------------------
	CXTPChartElementView** GetChildren();

	//-----------------------------------------------------------------------
	// Summary:
	//      Obtains viewed object's bound rectangle.
	// Parameters:
	//      bIncludingChildren - If TRUE, the bounding rectangle will include all 
	//         children objects' rectangles.
	// Returns:
	//      Viewed object's bound rectangle.
	//-----------------------------------------------------------------------
	CRect GetBounds() const;
	CRect GetBounds(BOOL bIncludingChildren) const; // <combine CXTPChartElementView::GetBounds>

public:
	//{{AFX_CODEJOCK_PRIVATE
	virtual void OnMouseMove(UINT nFlags, CPoint point);
	virtual void OnLButtonDown(UINT nFlags, CPoint point);
	virtual void OnLButtonUp(UINT nFlags, CPoint point);
	virtual BOOL OnSetCursor(CPoint point);
	//}}AFX_CODEJOCK_PRIVATE

public:
	void Release();

private:
	void Initialize(CXTPChartElementView* pParentView, BOOL bAddToParent);

protected:
	CArray<CXTPChartElementView*, CXTPChartElementView*> m_arrChildren;   //The child view collection.
	CXTPChartElementView* m_pParentView;  //The parent view.
	CXTPChartContainer* m_pContainer;  // The parent container.
	CRect m_rcBounds;  // Bounding rectangle.
};


AFX_INLINE int CXTPChartElementView::GetCount() const {
	return (int)m_arrChildren.GetSize();
}
AFX_INLINE CXTPChartElementView* CXTPChartElementView::GetAt(int nIndex) const {
	return nIndex >= 0 && nIndex < m_arrChildren.GetSize() ? m_arrChildren.GetAt(nIndex) : NULL;
}
AFX_INLINE CXTPChartElementView* CXTPChartElementView::GetParentView() const {
	return m_pParentView;
}
AFX_INLINE CXTPChartElementView** CXTPChartElementView::GetChildren() {
	return m_arrChildren.GetData();
}
AFX_INLINE CRect CXTPChartElementView::GetBounds() const {
	return m_rcBounds;
}


#include "Common/Base/Diagnostic/XTPEnableNoisyWarnings.h"
#endif //#if !defined(__XTPCHARTELEMENTVIEW_H__)
