// XTPChartPolygonDeviceCommand.h
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
#if !defined(__XTPCHARTPOLYGONDEVICECOMMAND_H__)
#define __XTPCHARTPOLYGONDEVICECOMMAND_H__
//}}AFX_CODEJOCK_PRIVATE

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "Common/Base/Diagnostic/XTPDisableNoisyWarnings.h"

//-----------------------------------------------------------------------
// Summary:
//      Base class implementation for 2D chart polygon device command.
//-----------------------------------------------------------------------
class _XTP_EXT_CLASS CXTPChartPolygonDeviceCommand : public CXTPChartDeviceCommand
{
protected:
	//-----------------------------------------------------------------------
	// Summary:
	//      Constructs CXTPChartPolygonDeviceCommand object.
	// Parameters:
	//      points - Point collection used for drawing polygon.
	//-----------------------------------------------------------------------
	CXTPChartPolygonDeviceCommand(const CXTPChartPoints& points);

protected:
	CXTPChartPoints m_points;   //The points which form the polygon.
	CXTPChartRectF m_bounds;    //The rectanglur broundary of the polygon.
};

//===========================================================================
// Summary:
//      Base class implementation for 2D chart solid spline polygon device command.
//===========================================================================
class _XTP_EXT_CLASS CXTPChartSolidSplinePolygonDeviceCommand : public CXTPChartDeviceCommand
{
public:
	//-----------------------------------------------------------------------
	// Summary:
	//     Constructs a CXTPChartSolidSplinePolygonDeviceCommand object.
	// Parameters:
	//     color     - The fill color of the solid spline.
	//     bTwoSides - Unused. Reserved.
	//-----------------------------------------------------------------------
	CXTPChartSolidSplinePolygonDeviceCommand(const CXTPChartColor& color, BOOL bTwoSides = FALSE);

protected:
	CXTPChartColor m_color;      //The fill color of the spline polygon.
	BOOL m_bTwoSides; // Unused. Reserved.
};

//===========================================================================
// Summary:
//      Base class implementation for 2D chart bounded polygon device command.
//===========================================================================
class _XTP_EXT_CLASS CXTPChartBoundedPolygonDeviceCommand : public CXTPChartDeviceCommand
{
public:
	//-----------------------------------------------------------------------
	// Summary:
	//     Constructs a CXTPChartBoundedPolygonDeviceCommand object.
	// Parameters:
	//     rect      - The rectangular area to be rendered by the device command.
	//     color     - The color of the rectangle border.
	//     thickness - The thickness of the rectangular border.
	//-----------------------------------------------------------------------
	CXTPChartBoundedPolygonDeviceCommand(const CXTPChartPoints& points, const CXTPChartColor& color, int thickness);

protected:
	CXTPChartPoints m_points;    //The points which form the spline polygon.
	CXTPChartColor m_color;      //The color of the rectangle border.
	int m_thickness;            //The thickness of the rectangle border.
};

//===========================================================================
// Summary:
//      Base class implementation for 2D chart antialiased polygon device command.
//===========================================================================
class _XTP_EXT_CLASS CXTPChartPolygonAntialiasingDeviceCommand : public CXTPChartDeviceCommand
{
public:
	//-----------------------------------------------------------------------
	// Summary:
	//     Constructs a CXTPChartPolygonAntialiasingDeviceCommand object.
	// Parameters:
	//      bAntiAlias - TRUE if the antialiasing is enabled and FALSE if the
	//      antialiasing is disabled.
	// Remarks:
	//-----------------------------------------------------------------------
	CXTPChartPolygonAntialiasingDeviceCommand(BOOL bAntiAlias = TRUE);

	//-----------------------------------------------------------------------
	// Summary:
	//      This override do some ground works if any, before the native mode
	//      antialiased drawing of polygons.
	// Parameters:
	//     pDC - A pointer to the chart native device context.
	// Remarks:
	//-----------------------------------------------------------------------
	virtual void BeforeExecute(CXTPChartDeviceContext* pDC) = 0;

	//-----------------------------------------------------------------------
	// Summary:
	//     This override do some final cut if any, after the native antialiased
	//     drawing of polygons.
	// Parameters:
	//     pDC - A pointer to the chart native device context.
	// Remarks:
	//-----------------------------------------------------------------------
	virtual void AfterExecute(CXTPChartDeviceContext* pDC) = 0;

protected:
	BOOL m_bAntiAlias;              //TRUE if antialiasing enabled, FALSE if not.
};


#include "Common/Base/Diagnostic/XTPEnableNoisyWarnings.h"
#endif //#if !defined(__XTPCHARTPOLYGONDEVICECOMMAND_H__)
