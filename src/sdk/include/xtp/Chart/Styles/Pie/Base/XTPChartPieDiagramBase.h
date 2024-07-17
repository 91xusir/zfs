// XTPChartPieDiagramBase.h
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
#if !defined(__XTPCHARTPIEDIAGRAMBASE_H__)
#define __XTPCHARTPIEDIAGRAMBASE_H__
//}}AFX_CODEJOCK_PRIVATE

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "Common/Base/Diagnostic/XTPDisableNoisyWarnings.h"

class CXTPChartDeviceContext;
class CXTPChartDiagramView;
class CXTPChartElementView;

//{{AFX_CODEJOCK_PRIVATE

class _XTP_EXT_CLASS CXTPChartPieDiagramBaseImpl
{
protected:
	void CalculateSeriesLayout(CXTPChartDeviceContext* pDC, CXTPChartDiagramView* pView);
};

//}}AFX_CODEJOCK_PRIVATE


//===========================================================================
// Summary:
//     This class abstracts the pie diagram.This class is a kind of
//     CXTPChartDiagram.
// Remarks:
//     A pie chart (or a circle graph) is a circular chart divided into sectors,
//     illustrating percents. In a pie chart, the arc length of each sector (and
//     consequently its central angle and area), is proportional to the quantity
//     it represents. Together, the sectors create a full disk.
//===========================================================================
template<class BaseDiagram>
class CXTPChartPieDiagramBase 
	: public BaseDiagram
	, private CXTPChartPieDiagramBaseImpl
{
protected:
	//-----------------------------------------------------------------------
	// Summary:
	//     Constructs a CXTPChart2dPieDiagramBase object.
	// Remarks:
	//-----------------------------------------------------------------------
	CXTPChartPieDiagramBase();

public:
	//-------------------------------------------------------------------------
	// Summary:
	//     Call this function to create the view of the pie diagram.
	// Parameters:
	//     pDC           - A pointer to chart device context.
	//     pParent       - A pointer to the parent view.
	// Returns:
	//     A pointer to CXTPChartDiagramView object which points to a newly created
	//     CXTPChartPieDiagramViewBase object.
	//-------------------------------------------------------------------------
	virtual CXTPChartDiagramView* CreateView(CXTPChartDeviceContext* pDC, CXTPChartElementView* pParent) = 0;

	//-------------------------------------------------------------------------
	// Summary:
	//     Call this function to calculate the pie series layout.
	// Parameters:
	//     pDC           - A pointer to chart device context.
	//     pView         - A pointer to the diagram view.
	//-------------------------------------------------------------------------
	virtual void CalculateSeriesLayout(CXTPChartDeviceContext* pDC, CXTPChartDiagramView* pView);
};

template<class BaseDiagram>
AFX_INLINE CXTPChartPieDiagramBase<BaseDiagram>::CXTPChartPieDiagramBase() {
}

template<class BaseDiagram>
AFX_INLINE void CXTPChartPieDiagramBase<BaseDiagram>::CalculateSeriesLayout(CXTPChartDeviceContext* pDC, CXTPChartDiagramView* pView) {
	CXTPChartPieDiagramBaseImpl::CalculateSeriesLayout(pDC, pView);
}

typedef CXTPChartPieDiagramBase<CXTPChart2dDiagram> CXTPChart2dPieDiagramBase;
typedef CXTPChartPieDiagramBase<CXTPChart3dDiagram> CXTPChart3dPieDiagramBase;


#include "Common/Base/Diagnostic/XTPEnableNoisyWarnings.h"
#endif //#if !defined(__XTPCHARTPIEDIAGRAMBASE_H__)
