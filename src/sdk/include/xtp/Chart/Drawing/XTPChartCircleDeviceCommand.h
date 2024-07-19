// XTPChartCircleDeviceCommand.h
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
#if !defined(__XTPCHARTCIRCLEDEVICECOMMAND_H__)
#define __XTPCHARTCIRCLEDEVICECOMMAND_H__
//}}AFX_CODEJOCK_PRIVATE

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "Common/Base/Diagnostic/XTPDisableNoisyWarnings.h"


//===========================================================================
// Summary:
//     This class is the base class for a all circle device commands.
//===========================================================================
class _XTP_EXT_CLASS CXTPChartCircleDeviceCommand : public CXTPChartDeviceCommand
{
protected:
	//-----------------------------------------------------------------------
	// Summary:
	//      Constructs CXTPChartCircleDeviceCommand object.
	// Parameters:
	//      center - Circle center coordinates.
	//      radius - Circle radius value.
	//-----------------------------------------------------------------------
	CXTPChartCircleDeviceCommand(const CXTPChartPointF& center, double radius);

	//-----------------------------------------------------------------------
	// Summary:
	//      Performs recursive hit testing of the element at the specified point.
	// Parameters:
	//      point - Point at which an element to be found.
	//      pParent - Parent element pointer.
	// Returns:
	//      A pointer to the element found at the specified point or NULL
	//      if no element is found.
	//-----------------------------------------------------------------------
	CXTPChartElement* HitTest(CPoint point, CXTPChartElement* pParent) const;

protected:
	CXTPChartPointF m_center;            //The center of the circle.
	double m_radius;                    //The radius of the circle.
};

//===========================================================================
// Summary:
//     This class represents a bounded circle device command,which is a kind of CXTPChartDeviceCommand.
//     It specifically handles the rendering of circular shapes in a chart.
// Remarks:
//===========================================================================
class _XTP_EXT_CLASS CXTPChartBoundedCircleDeviceCommand : public CXTPChartDeviceCommand
{
protected:
	//-----------------------------------------------------------------------
	// Summary:
	//     Constructs a CXTPChartBoundedCircleDeviceCommand object.
	// Parameters:
	//     center    - The center point of the circle.
	//     radius    - The radius of the circle.
	//     color     - The color of boundary.
	//     thickness - The thickness of the boundary.
	// Remarks:
	//-----------------------------------------------------------------------
	CXTPChartBoundedCircleDeviceCommand(const CXTPChartPointF& center, double radius, const CXTPChartColor& color, int thickness);

protected:
	CXTPChartColor m_color;          //The color of the cirular boundary.
	int m_thickness;                //The thickness of the circular boundary.
	CXTPChartPointF m_center;        //The center point of the circle.
	double m_radius;                //The radius of the circle.
};


#include "Common/Base/Diagnostic/XTPEnableNoisyWarnings.h"
#endif //#if !defined(__XTPCHARTCIRCLEDEVICECOMMAND_H__)
