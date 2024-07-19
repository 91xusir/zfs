// XTPChart2dPie.h
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
#if !defined(__XTPCHART2DPIE_H__)
#define __XTPCHART2DPIE_H__
//}}AFX_CODEJOCK_PRIVATE

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "Common/Base/Diagnostic/XTPDisableNoisyWarnings.h"

class CXTPChartDeviceCommand;
class CXTPChartEllipse;

//===========================================================================
// Summary:
//     This class abstracts an pie chart.
// Remarks:
//===========================================================================
class _XTP_EXT_CLASS CXTPChartPie : public CXTPChartPieBase
{
public:
	//-----------------------------------------------------------------------
	// Summary:
	//     Constructs a CXTPChartPieBase object.
	// Parameters:
	//     dStartAngle      - The start angle of the pie.
	//     dFinishAngle     - The finish angle of the pie.
	//     ellipse          - The ellipse associated with the pie.
	//     nDepthPercent    - The depth percentage, in case of 3D chart.
	//     nHolePercent     - The hole percentage.
	// Remarks:
	//-----------------------------------------------------------------------
	CXTPChartPie(double dStartAngle, double dFinishAngle, const CXTPChartEllipse& ellipse, int nDepthPercent, int nHolePercent);

public:
	//-------------------------------------------------------------------------
	// Summary:
	//     This function creates a CXTPChartDeviceCommand object, this object
	//     represents the rendering of the pie.
	// Parameters:
	//     color      - The first color used for gradient pie.
	//     color2     - The second color used for gradient pie.
	// Returns:
	//     Returns CXTPChartDeviceCommand object, this object handles
	//     the rendering of an element in the chart. Here it handles
	//     the drawing of the pie.
	// Remarks:
	// See Also:
	//-------------------------------------------------------------------------
	virtual CXTPChartDeviceCommand* CreatePieDeviceCommand(const CXTPChartColor& color, const CXTPChartColor& color2, const CXTPChartPointF& basePoint);
	virtual CXTPChartDeviceCommand* CreateBoundedPieDeviceCommand(const CXTPChartColor& color, int nThickness, const CXTPChartPointF& basePoint);
};


#include "Common/Base/Diagnostic/XTPEnableNoisyWarnings.h"
#endif //#if !defined(__XTPCHART2DPIE_H__)
