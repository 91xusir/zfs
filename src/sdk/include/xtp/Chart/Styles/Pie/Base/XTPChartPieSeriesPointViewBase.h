// XTPChartPieSeriesPointViewBase.h
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
#if !defined(__XTPCHARTPIESERIESPOINTVIEWBASE_H__)
#define __XTPCHARTPIESERIESPOINTVIEWBASE_H__
//}}AFX_CODEJOCK_PRIVATE

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "Common/Base/Diagnostic/XTPDisableNoisyWarnings.h"

class CXTPChartPieBase;
class CXTPChartSeriesPoint;
class CXTPChartElementView;
class CXTPChartDeviceCommand;

//===========================================================================
// Summary:
//     This class abstracts the view of a pie series point.This class is a kind of
//     CXTPChartSeriesPointView.
// Remarks:
//     A pie chart (or a circle graph) is a circular chart divided into sectors,
//     illustrating percents. In a pie chart, the arc length of each sector (and
//     consequently its central angle and area), is proportional to the quantity
//     it represents. Together, the sectors create a full disk.
//===========================================================================
class _XTP_EXT_CLASS CXTPChartPieSeriesPointViewBase : public CXTPChartSeriesPointView
{
	DECLARE_DYNAMIC(CXTPChartPieSeriesPointViewBase);

protected:
	//-----------------------------------------------------------------------
	// Summary:
	//     Constructs a CXTPChartPieSeriesPointViewBase object.
	// Parameters:
	//     pPoint        - Pointer to chart series point.
	//     bAddToParent  - If TRUE the view will be be added to parent's child view list.
	// Remarks:
	//-----------------------------------------------------------------------
	CXTPChartPieSeriesPointViewBase(CXTPChartSeriesPoint* pPoint, CXTPChartElementView* pParentView);
	CXTPChartPieSeriesPointViewBase(CXTPChartSeriesPoint* pPoint, CXTPChartElementView* pParentView, BOOL bAddToParent);

public:
	//-------------------------------------------------------------------------
	// Summary:
	//     Destroys a CXTPChartPieSeriesPointViewBase object, handle cleanup.
	//-------------------------------------------------------------------------
	virtual ~CXTPChartPieSeriesPointViewBase();

public:
	//-------------------------------------------------------------------------
	// Summary:
	//     Use this function to calculate the layout of the pie series point.
	// Parameters:
	//     dAngle - The swipe angle of the sector(pie).
	//-------------------------------------------------------------------------
	virtual void CalculateLayout(double& dAngle) = 0;

	//-------------------------------------------------------------------------
	// Summary:
	//     Use this function to get the pie for the point.
	// Returns:
	//     A pointer CXTPChartPieBase object which represents a pie.
	//-------------------------------------------------------------------------
	CXTPChartPieBase* GetPie() const;

	//-------------------------------------------------------------------------
	// Summary:
	//     Use this function to get the pie for the point.
	// Returns:
	//     A pointer CXTPChartPieBase object which represents a pie.
	//-------------------------------------------------------------------------
	CXTPChartPointF GetBasePoint() const;

	//-------------------------------------------------------------------------
	// Summary:
	//     This function creates a CXTPChartDeviceCommand object, this object
	//     represents the rendering of the pie series point.
	// Parameters:
	//     pDC      - Pointer to a CXTPChartDeviceContext object.
	// Returns:
	//     Returns CXTPChartDeviceCommand object, this object handles
	//     the rendering of an element in the chart. Here it handles
	//     the drawing of the pie series point.
	// Remarks:
	// See Also:
	//-------------------------------------------------------------------------
	virtual CXTPChartDeviceCommand* CreateDeviceCommand(CXTPChartDeviceContext* pDC) = 0;

	//-------------------------------------------------------------------------
	// Summary:
	//     This function creates a label view drawing command object.
	// Parameters:
	//     pDC      - Pointer to a CXTPChartDeviceContext object.
	//     rcBounds - The rectangular boundary of the legend.
	// Returns:
	//     Returns a new label view drawing command object.
	//-------------------------------------------------------------------------
	virtual CXTPChartDeviceCommand* CreateLegendDeviceCommand(CXTPChartDeviceContext* pDC, CRect rcBounds) = 0;

public:
	double m_dValue;            //The value of the point.

protected:
	CXTPChartPieBase* m_pPie;        //The pie for this point.
	CXTPChartPointF m_basePoint; //The base point.
};

AFX_INLINE CXTPChartPieBase* CXTPChartPieSeriesPointViewBase::GetPie() const
{
	return m_pPie;
}

AFX_INLINE CXTPChartPointF CXTPChartPieSeriesPointViewBase::GetBasePoint() const
{
	return m_basePoint;
}


#include "Common/Base/Diagnostic/XTPEnableNoisyWarnings.h"
#endif //#if !defined(__XTPCHARTPIESERIESPOINTVIEWBASE_H__)
