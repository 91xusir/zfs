// XTPChart2dPieSeriesStyle.h
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
#if !defined(__XTPCHART2DPIESERIESSTYLE_H__)
#define __XTPCHART2DPIESERIESSTYLE_H__
//}}AFX_CODEJOCK_PRIVATE

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "Common/Base/Diagnostic/XTPDisableNoisyWarnings.h"

class CXTPChartDiagramView;
class CXTPChartSeries;

//===========================================================================
// Summary:
//     This class abstracts the pie series style.This class is a kind of
//     CXTPChartPieSeriesStyleBase.
// Remarks:
//     A pie chart (or a circle graph) is a circular chart divided into sectors,
//     illustrating percents. In a pie chart, the arc length of each sector (and
//     consequently its central angle and area), is proportional to the quantity
//     it represents. Together, the sectors create a full disk.
//===========================================================================
class _XTP_EXT_CLASS CXTPChartPieSeriesStyle : public CXTPChartPieSeriesStyleBase
{
	DECLARE_SERIAL(CXTPChartPieSeriesStyle)

public:
	//-----------------------------------------------------------------------
	// Summary:
	//     Constructs a CXTPChartPieSeriesStyle object.
	// Remarks:
	//-----------------------------------------------------------------------
	CXTPChartPieSeriesStyle();

protected:
	//-------------------------------------------------------------------------
	// Summary:
	//     Call this function to create the pie diagram object.
	// Returns:
	//     A pointer to CXTPChartDiagram object which points to a newly created
	//     CXTPChart2dPieDiagramBase object.
	//-------------------------------------------------------------------------
	virtual CXTPChartDiagram* CreateDiagram();

	//-------------------------------------------------------------------------
	// Summary:
	//     Call this function to create the view of the pie series.
	// Parameters:
	//     pSeries       - A pointer to chart series  object.
	//     pDiagramView  - A pointer to the diagram view object.
	// Returns:
	//     A pointer to CXTPChartSeriesView object which points to a newly created
	//     CXTPChartPieSeriesViewBase object.
	//-------------------------------------------------------------------------
	virtual CXTPChartSeriesView* CreateView(CXTPChartSeries* pSeries, CXTPChartDiagramView* pDiagramView);

#ifdef _XTP_ACTIVEX
public:
	//{{AFX_CODEJOCK_PRIVATE
	DECLARE_INTERFACE_MAP()
	DECLARE_OLETYPELIB_EX(CXTPChartPieSeriesStyle);
	DECLARE_OLECREATE_EX(CXTPChartPieSeriesStyle);
	//}}AFX_CODEJOCK_PRIVATE
#endif
};


#include "Common/Base/Diagnostic/XTPEnableNoisyWarnings.h"
#endif //#if !defined(__XTPCHART2DPIESERIESSTYLE_H__)
