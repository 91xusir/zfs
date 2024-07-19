// XTPChartDiagramView.h
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
#if !defined(__XTPCHARTDIAGRAMVIEW_H__)
#define __XTPCHARTDIAGRAMVIEW_H__
//}}AFX_CODEJOCK_PRIVATE

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "Common/Base/Diagnostic/XTPDisableNoisyWarnings.h"


class CXTPChartSeriesView;
class CXTPChartElementView;
class CXTPChartDiagramView;
class CXTPChartSeries;
class CXTPChartPanel;
class CXTPChartDiagram;
class CXTPChartDeviceContext;
class CXTPChartDeviceCommand;
class CXTPChart3dDiagram;

//===========================================================================
// Summary:
//     This class represents the view of a chart diagram, which is a kind of
//     CXTPChartElementView.
// Remarks:
//===========================================================================
class _XTP_EXT_CLASS CXTPChartDiagramView : public CXTPChartElementView
{
	DECLARE_DYNAMIC(CXTPChartDiagramView);

public:
	//-----------------------------------------------------------------------
	// Summary:
	//     Constructs a CXTPChartDiagram object.
	// Parameters:
	//     pDiagram    - A pointer to chart diagram object.
	//     pParentView - A pointer to the parent view.
	// Remarks:
	//-----------------------------------------------------------------------
	CXTPChartDiagramView(CXTPChartDiagram* pDiagram, CXTPChartElementView* pParentView);

public:

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this function to get the bounds of the diagram.
	// Returns:
	//     A CRect object contains the bounds of the diagram.
	// Remarks:
	//-----------------------------------------------------------------------
	CRect GetBounds() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this function to get the diagram object associated with the
	//     view.
	// Returns:
	//     A CXTPChartDiagram pointer, the actual type could be CXTPChartDiagram2D
	//     or CXTPChartDiagram3D.
	// Remarks:
	//-----------------------------------------------------------------------
	CXTPChartDiagram* GetDiagram() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this function to get the label view object associated with
	//     this diagram view.
	// Returns:
	//     A CXTPChartElementView pointer, representing the label view.
	// Remarks:
	//-----------------------------------------------------------------------
	CXTPChartElementView* GetLabelsView() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this function to get the series view object associated with
	//     this diagram view.
	// Returns:
	//     A CXTPChartElementView pointer, representing the series view.
	// Remarks:
	//-----------------------------------------------------------------------
	CXTPChartElementView* GetSeriesView() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this function to calculate the view of the diagram.
	// Parameters:
	//     pDC      - Pointer to the chart device context object.
	//     rcBounds - The bounding rectangle.
	// Remarks:
	//     This is a virtual function, so the sub classes can give their type
	//     specific implementation for this function.
	//-----------------------------------------------------------------------
	virtual void CalculateView(CXTPChartDeviceContext* pDC, CRect rcBounds);

	//-----------------------------------------------------------------------
	// Summary:
	//      Updates chart range.
	// Parameters:
	//      pDC - Target device context.
	//-----------------------------------------------------------------------
	virtual void UpdateRange(CXTPChartDeviceContext* pDC);

	//-----------------------------------------------------------------------
	// Summary:
	//      Handles mouse wheel event.
	// Parameters:
	//      nFlags - Indicates whether various virtual keys are down.
	//      zDelta - Indicates distance rotated.
	//      pt - Specifies the x- and y-coordinate of the cursor. 
	//-----------------------------------------------------------------------
	virtual void OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);

	//-----------------------------------------------------------------------
	// Summary:
	//      Performs view initialization.
	// Parameters:
	//      pDC - Target device context.
	//-----------------------------------------------------------------------
	virtual void CreateView(CXTPChartDeviceContext* pDC);

protected:
	virtual CXTPChartDeviceCommand* CreateDeviceCommand(CXTPChartDeviceContext* pDC);

protected:
	CXTPChartDiagram* m_pDiagram;        //Pointer to the associated diagram object.
	CRect m_rcBounds;                   //The diagram bounds.
	CXTPChartElementView* m_pLabelsView; //The label view.
	CXTPChartElementView* m_pSeriesView; //The series view.
	CXTPChartElementView* m_pTitlesView; // Titles View.

public:
	HCURSOR m_hcurNormalHand;
	HCURSOR m_hcurDragHand;
};


AFX_INLINE CRect CXTPChartDiagramView::GetBounds() const {
	return m_rcBounds;
}
AFX_INLINE CXTPChartDiagram* CXTPChartDiagramView::GetDiagram() const {
	return m_pDiagram;
}
AFX_INLINE CXTPChartElementView* CXTPChartDiagramView::GetLabelsView() const {
	return m_pLabelsView;
}
AFX_INLINE CXTPChartElementView* CXTPChartDiagramView::GetSeriesView() const {
	return m_pSeriesView;
}

//===========================================================================
// Summary:
//     This class represents the view of a chart diagram, which is a kind of
//     CXTPChartElementView.
// Remarks:
//===========================================================================
class _XTP_EXT_CLASS CXTPChart3dDiagramView : public CXTPChartDiagramView
{
	DECLARE_DYNAMIC(CXTPChart3dDiagramView);

public:
	//-----------------------------------------------------------------------
	// Summary:
	//     Constructs a CXTPChart3dDiagramView object.
	// Parameters:
	//     pDiagram    - A pointer to chart diagram object.
	//     pParentView - A pointer to the parent view.
	// Remarks:
	//-----------------------------------------------------------------------
	CXTPChart3dDiagramView(CXTPChartDiagram* pDiagram, CXTPChartElementView* pParentView);

public:
	//-----------------------------------------------------------------------
	// Summary:
	//      Creates view device command.
	// Parameters:
	//      pDC - Device context pointer for which a command is to be created.
	// Returns:
	//      A new vide device command. A caller is responsible for deleting 
	//      the command object.
	//-----------------------------------------------------------------------
	virtual CXTPChartDeviceCommand* CreateDeviceCommand(CXTPChartDeviceContext* pDC);

	//{{AFX_CODEJOCK_PRIVATE
public:
	virtual void OnMouseMove(UINT nFlags, CPoint point);
	virtual void OnLButtonDown(UINT nFlags, CPoint point);
	virtual void OnLButtonUp(UINT nFlags, CPoint point);
	virtual void OnMouseWheel(UINT nFlags, short zDelta, CPoint point);
	virtual BOOL OnSetCursor(CPoint point);
	//}}AFX_CODEJOCK_PRIVATE

private:
	CXTPChart3dDiagram* m_p3dDiagram;
};


#include "Common/Base/Diagnostic/XTPEnableNoisyWarnings.h"
#endif //#if !defined(__XTPCHARTDIAGRAMVIEW_H__)
