// XTPChartGdiLineDeviceCommand.h
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
#if !defined(__XTPCHARTGDILINEDEVICECOMMAND_H__)
#define __XTPCHARTGDILINEDEVICECOMMAND_H__

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "Common/Base/Diagnostic/XTPDisableNoisyWarnings.h"

//===========================================================================
// Summary:
//     This class represents a solid line device command, which is a kind of
//     CXTPChartDeviceCommand It specifically handles the rendering of solid
//     lines in a chart.
//
// Remarks:
//===========================================================================
class _XTP_EXT_CLASS CXTPChartGdiSolidLineDeviceCommand : public CXTPChartLineDeviceCommand
{
public:
	//-----------------------------------------------------------------------
	// Summary:
	//     Constructs a CXTPChartSolidLineDeviceCommand object.
	// Parameters:
	//     p1        - The first point of the line.
	//     p2        - The second point of the line
	//     color     - The color of the line.
	//     thickness - The thickness of the line.
	// Remarks:
	//-----------------------------------------------------------------------
	CXTPChartGdiSolidLineDeviceCommand(const CXTPPoint2f& p1, const CXTPPoint2f& p2, const CXTPChartColor& color, int thickness);

protected:
	//-----------------------------------------------------------------------
	// Summary:
	//     This is a virtual function override of base class CXTPChartDeviceContext,
	//     act polymorphically to do the actual drawing of the chart element, to which
	//     this device command is associated with.
	// Parameters:
	//     pDC      - The GDI device context of the chart.
	// Remarks:
	//-----------------------------------------------------------------------
	void ExecuteOverride(CXTPChartDeviceContext* pDC);
};

//===========================================================================
// Summary:
//     This class represents a solid polyline device command, which is a kind of
//     CXTPChartDeviceCommand It specifically handles the rendering of solid
//     lines in a chart.
//
// Remarks:
//===========================================================================
class _XTP_EXT_CLASS CXTPChartGdiSolidPolylineDeviceCommand : public CXTPChartPolylineDeviceCommand
{
public:
	//-----------------------------------------------------------------------
	// Summary:
	//     Constructs a CXTPChartSolidPolylineDeviceCommand object.
	// Parameters:
	//     p1        - The first point of the line.
	//     p2        - The second point of the line
	//     color     - The color of the line.
	//     thickness - The thickness of the line.
	// Remarks:
	//-----------------------------------------------------------------------
	CXTPChartGdiSolidPolylineDeviceCommand(const CXTPChartPoints& p, const CXTPChartColor& color, int thickness);

protected:
	//-----------------------------------------------------------------------
	// Summary:
	//     This is a virtual function override of base class CXTPChartDeviceContext,
	//     act polymorphically to do the actual drawing of the chart element, to which
	//     this device command is associated with.
	// Parameters:
	//     pDC      - The GDI device context of the chart.
	// Remarks:
	//-----------------------------------------------------------------------
	void ExecuteOverride(CXTPChartDeviceContext* pDC);

protected:
	CXTPChartColor m_color;      //The line color.
};

class _XTP_EXT_CLASS CXTPChartGdiSplineDeviceCommand : public CXTPChartSplineDeviceCommand
{
protected:
	CXTPChartGdiSplineDeviceCommand(const CXTPChartPoints& p, int thickness);

protected:
	CXTPChartElement* HitTest(CPoint point, CXTPChartElement* pParent) const;

protected:
	CXTPChartPoints m_p;     //The points which form the spline.
	int m_nThickness;       //The thickness of the spline.
};

//===========================================================================
// Summary:
//     This class represents a solid spline device command, which is a kind of
//     CXTPChartDeviceCommand It specifically handles the rendering of solid
//     splines in a chart.
//
// Remarks:
//===========================================================================
class _XTP_EXT_CLASS CXTPChartGdiSolidSplineDeviceCommand : public CXTPChartSplineDeviceCommand
{
public:
	//-----------------------------------------------------------------------
	// Summary:
	//     Constructs a CXTPChartGdiSolidSplineDeviceCommand object.
	// Parameters:
	//     p         - The points which forms the spline.
	//     color     - The color of the spline.
	//     thickness - The thickness of the spline.
	// Remarks:
	//-----------------------------------------------------------------------
	CXTPChartGdiSolidSplineDeviceCommand(const CXTPChartPoints& p, const CXTPChartColor& color, int thickness);

protected:
	//-----------------------------------------------------------------------
	// Summary:
	//     This is a virtual function override of base class CXTPChartDeviceContext,
	//     act polymorphically to do the actual drawing of the chart element, to which
	//     this device command is associated with.
	// Parameters:
	//     pDC      - The GDI device context of the chart.
	// Remarks:
	//-----------------------------------------------------------------------
	void ExecuteOverride(CXTPChartDeviceContext* pDC);

protected:
	CXTPChartColor m_color;  //The spline color.

};

//===========================================================================
// Summary:
//     This class represents a dash line device command, which is a kind of
//     CXTPChartDeviceCommand It specifically handles the rendering of solid
//     lines in a chart.
//
// Remarks:
//===========================================================================
class _XTP_EXT_CLASS CXTPChartGdiDashedLineDeviceCommand : public CXTPChartLineDeviceCommand
{
public:
	//-----------------------------------------------------------------------
	// Summary:
	//     Constructs a CXTPChartGdiDashedLineDeviceCommand object.
	// Parameters:
	//     p1         - The first point of the line.
	//     p2         - The second point of the line
	//     color      - The color of the line.
	//     thickness  - The thickness of the line.
	//     nDashStyle - The dash style.
	// Remarks:
	//     The dash line style are the following.
	//     Empty style, invisible.
	//     Solid line style.
	//     Dash line style.
	//     Dot line style.
	//     Alternate dash and dot line style.
	//     Dash dot dot line style.
	//-----------------------------------------------------------------------
	CXTPChartGdiDashedLineDeviceCommand(const CXTPPoint2f& p1, const CXTPPoint2f& p2, const CXTPChartColor& color, int thickness, XTPChartDashStyle nDashStyle, const CXTPChartLineDashArray& dashArray);

protected:
	//-----------------------------------------------------------------------
	// Summary:
	//     This is a virtual function override of base class CXTPChartDeviceContext,
	//     act polymorphically to do the actual drawing of the chart element, to which
	//     this device command is associated with.
	// Parameters:
	//     pDC      - The GDI device context of the chart.
	// Remarks:
	//-----------------------------------------------------------------------
	void ExecuteOverride(CXTPChartDeviceContext* pDC);

protected:
	XTPChartDashStyle m_nDashStyle;  //The dash style.
	CXTPChartLineDashArray m_arrDashArray;
};

//===========================================================================
// Summary:
//     This class represents a dashed spline device command, which is a kind of
//     CXTPChartDeviceCommand It specifically handles the rendering of dashed
//     splines in a chart.
//
// Remarks:
//===========================================================================
class _XTP_EXT_CLASS CXTPChartGdiDashedSplineDeviceCommand : public CXTPChartSplineDeviceCommand
{
public:
	//-----------------------------------------------------------------------
	// Summary:
	//     Constructs a CXTPChartGdiDashedSplineDeviceCommand object.
	// Parameters:
	//     p         - The points which forms the spline.
	//     color     - The color of the spline.
	//     thickness - The thickness of the spline.
	//     nDashStyle - The dash style.
	// Remarks:
	//     The dash line style are the following.
	//     Empty style, invisible.
	//     Solid line style.
	//     Dash line style.
	//     Dot line style.
	//     Alternate dash and dot line style.
	//     Dash dot dot line style.
	//-----------------------------------------------------------------------
	CXTPChartGdiDashedSplineDeviceCommand(const CXTPChartPoints& p, const CXTPChartColor& color, int thickness, XTPChartDashStyle nDashStyle, const CXTPChartLineDashArray& dashArray);

protected:
	//-----------------------------------------------------------------------
	// Summary:
	//     This is a virtual function override of base class CXTPChartDeviceContext,
	//     act polymorphically to do the actual drawing of the chart element, to which
	//     this device command is associated with.
	// Parameters:
	//     pDC      - The GDI device context of the chart.
	// Remarks:
	//-----------------------------------------------------------------------
	void ExecuteOverride(CXTPChartDeviceContext* pDC);

protected:
	CXTPChartColor m_color;          //The spline color.
	XTPChartDashStyle m_nDashStyle;  //The dash style.
	CXTPChartLineDashArray m_arrDashArray;
};

//===========================================================================
// Summary:
//     This class represents a dashed polyline device command, which is a kind of
//     CXTPChartDeviceCommand It specifically handles the rendering of dashed
//     lines in a chart.
//
// Remarks:
//===========================================================================
class _XTP_EXT_CLASS CXTPChartGdiDashedPolylineDeviceCommand : public CXTPChartPolylineDeviceCommand
{
public:
	//-----------------------------------------------------------------------
	// Summary:
	//     Constructs a CXTPChartGdiDashedPolylineDeviceCommand object.
	// Parameters:
	//     p1        - The first point of the line.
	//     p2        - The second point of the line
	//     color     - The color of the line.
	//     thickness - The thickness of the line.
	//     nDashStyle - The dash style.
	// Remarks:
	//     The dash line style are the following.
	//     Empty style, invisible.
	//     Solid line style.
	//     Dash line style.
	//     Dot line style.
	//     Alternate dash and dot line style.
	//     Dash dot dot line style.
	//-----------------------------------------------------------------------
	CXTPChartGdiDashedPolylineDeviceCommand(const CXTPChartPoints& p, const CXTPChartColor& color, int thickness, XTPChartDashStyle nDashStyle, const CXTPChartLineDashArray& dashArray);

protected:
	//-----------------------------------------------------------------------
	// Summary:
	//     This is a virtual function override of base class CXTPChartDeviceContext,
	//     act polymorphically to do the actual drawing of the chart element, to which
	//     this device command is associated with.
	// Parameters:
	//     pDC      - The GDI device context of the chart.
	// Remarks:
	//-----------------------------------------------------------------------
	void ExecuteOverride(CXTPChartDeviceContext* pDC);

protected:
	CXTPChartColor m_color;          //The spline color.
	XTPChartDashStyle m_nDashStyle;  //The dash style.
	CXTPChartLineDashArray m_arrDashArray;
};


#include "Common/Base/Diagnostic/XTPEnableNoisyWarnings.h"
#endif //#if !defined(__XTPCHARTGDILINEDEVICECOMMAND_H__)
//}}AFX_CODEJOCK_PRIVATE
