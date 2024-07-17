// XTPChartGdiPolygonDeviceCommand.h
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
#if !defined(__XTPCHARTGDIPOLYGONDEVICECOMMAND_H__)
#define __XTPCHARTGDIPOLYGONDEVICECOMMAND_H__

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "Common/Base/Diagnostic/XTPDisableNoisyWarnings.h"

class _XTP_EXT_CLASS CXTPChartGdiPolygonDeviceCommand : public CXTPChartPolygonDeviceCommand
{
protected:
	CXTPChartGdiPolygonDeviceCommand(const CXTPChartPoints& points);

protected:
	virtual  CXTPChartElement* HitTest(CPoint point, CXTPChartElement* pParent) const;
};

//===========================================================================
// Summary:
//     This class represents a polygon device command,which is a kind of CXTPChartDeviceCommand.
//     It specifically handles the rendering of polygonal shapes in a chart.
// Remarks:
//===========================================================================
class _XTP_EXT_CLASS CXTPChartGdiSolidPolygonDeviceCommand : public CXTPChartGdiPolygonDeviceCommand
{
public:
	//-----------------------------------------------------------------------
	// Summary:
	//     Constructs a CXTPChartGdiSolidPolygonDeviceCommand object.
	// Parameters:
	//     points    - The collection of points which make the polygon.
	//     color     - The color of boundary.
	// Remarks:
	//-----------------------------------------------------------------------
	CXTPChartGdiSolidPolygonDeviceCommand(const CXTPChartPoints& points, const CXTPChartColor& color);
	~CXTPChartGdiSolidPolygonDeviceCommand();

protected:
	//-----------------------------------------------------------------------
	// Summary:
	//     This is a virtual function override of base class CXTPChartDeviceContext,
	//     act polymorphically to do the actual drawing of the chart element, to which
	//     this device command is associated with.
	// Parameters:
	//     pDC      - The device context of the chart.
	// Remarks:
	//-----------------------------------------------------------------------
	void ExecuteOverride(CXTPChartDeviceContext* pDC);

protected:
	CXTPChartColor m_color;      //The color of the boundary.
};

class _XTP_EXT_CLASS CXTPChartGdiSolidSplinePolygonDeviceCommand : public CXTPChartSolidSplinePolygonDeviceCommand
{
public:
	//-----------------------------------------------------------------------
	// Summary:
	//     Constructs a CXTPChartGdiSolidSplinePolygonDeviceCommand object.
	// Parameters:
	//     points    - The collection of points which make the polygon spline.
	//     color     - The fill color of the solid spline.
	// Remarks:
	//-----------------------------------------------------------------------
	CXTPChartGdiSolidSplinePolygonDeviceCommand(const CXTPChartPoints& points, const CXTPChartColor& color, BOOL bTwoSides = FALSE);
	~CXTPChartGdiSolidSplinePolygonDeviceCommand();

protected:
	//-----------------------------------------------------------------------
	// Summary:
	//     This is a virtual function override of base class CXTPChartDeviceContext,
	//     act polymorphically to do the actual drawing of the chart element, to which
	//     this device command is associated with.
	// Parameters:
	//     pDC      - The device context of the chart.
	// Remarks:
	//-----------------------------------------------------------------------
	virtual void ExecuteOverride(CXTPChartDeviceContext* pDC);

	virtual CXTPChartElement* HitTest(CPoint point, CXTPChartElement* pParent) const;

protected:
	Gdiplus::GpPath* m_pGpPath;
};

//===========================================================================
// Summary:
//     This class represents a gradient polygon device command,which is a kind
//     of CXTPChartDeviceCommand. It specifically handles the rendering of gradient
//     polygonal shapes in a chart.
//
// Remarks:
//===========================================================================
class _XTP_EXT_CLASS CXTPChartGdiGradientPolygonDeviceCommand : public CXTPChartGdiPolygonDeviceCommand
{
public:
	//-----------------------------------------------------------------------
	// Summary:
	//     Constructs a CXTPChartGdiGradientPolygonDeviceCommand object.
	// Parameters:
	//     points    - The collection of points which make the polygon spline.
	//     color     - The start color of the gradient.
	//     color2    - The end color of the gradient.
	//     nMode     - The linear gradient mode, it can be horizontal, vertical,
	//                 forward diagonal or backward diagonal.
	// Remarks:
	//-----------------------------------------------------------------------
	CXTPChartGdiGradientPolygonDeviceCommand(const CXTPChartPoints& points, const CXTPChartColor& color, const CXTPChartColor& color2, XTPChartLinearGradientMode nMode);

protected:
	//-----------------------------------------------------------------------
	// Summary:
	//     This is a virtual function override of base class CXTPChartDeviceContext,
	//     act polymorphically to do the actual drawing of the chart element, to which
	//     this device command is associated with.
	// Parameters:
	//     pDC      - The device context of the chart.
	// Remarks:
	//-----------------------------------------------------------------------
	virtual void ExecuteOverride(CXTPChartDeviceContext* pDC);

protected:
	CXTPChartColor m_color;              //The start color of the gradient.
	CXTPChartColor m_color2;             //The end color of the gradiednt.
	XTPChartLinearGradientMode m_nMode;  //The gradient mode.
};

//===========================================================================
// Summary:
//     This class represents a hatch polygon device command,which is a kind
//     of CXTPChartDeviceCommand. It specifically handles the rendering of hatch
//     polygonal shapes in a chart.
//
// Remarks:
//===========================================================================
class _XTP_EXT_CLASS CXTPChartGdiHatchPolygonDeviceCommand : public CXTPChartGdiPolygonDeviceCommand
{
public:
	//-----------------------------------------------------------------------
	// Summary:
	//     Constructs a CXTPChartGdiHatchPolygonDeviceCommand object.
	// Parameters:
	//     points    - The collection of points which make the polygon spline.
	//     color     - The first color of the pattern.
	//     color2    - The second color of the pattern.
	//     nStyle    - The hatch style.
	// Remarks:
	//-----------------------------------------------------------------------
	CXTPChartGdiHatchPolygonDeviceCommand(const CXTPChartPoints& points, const CXTPChartColor& color, const CXTPChartColor& color2, XTPChartHatchStyle nStyle);

protected:
	//-----------------------------------------------------------------------
	// Summary:
	//     This is a virtual function override of base class CXTPChartDeviceContext,
	//     act polymorphically to do the actual drawing of the chart element, to which
	//     this device command is associated with.
	// Parameters:
	//     pDC      - The device context of the chart.
	// Remarks:
	//-----------------------------------------------------------------------
	virtual void ExecuteOverride(CXTPChartDeviceContext* pDC);

protected:
	CXTPChartColor m_color;          //The first color of the pattern.
	CXTPChartColor m_color2;         //The second color of the pattern.
	XTPChartHatchStyle m_nStyle;     //The hatch style.
};

class _XTP_EXT_CLASS CXTPChartGdiBoundedPolygonDeviceCommand : public CXTPChartBoundedPolygonDeviceCommand
{
public:
	//-----------------------------------------------------------------------
	// Summary:
	//     Constructs a CXTPChartGdiBoundedPolygonDeviceCommand object.
	// Parameters:
	//     rect      - The rectangular area to be rendered by the device command.
	//     color     - The color of the rectangle border.
	//     thickness - The thickness of the rectangular border.
	// Remarks:
	//-----------------------------------------------------------------------
	CXTPChartGdiBoundedPolygonDeviceCommand(const CXTPChartPoints& points, const CXTPChartColor& color, int thickness);

protected:
	//-----------------------------------------------------------------------
	// Summary:
	//     This is a virtual function override of base class CXTPChartDeviceContext,
	//     act polymorphically to do the actual drawing of the chart element, to which
	//     this device command is associated with.
	// Parameters:
	//     pDC      - The device context of the chart.
	// Remarks:
	//-----------------------------------------------------------------------
	virtual void ExecuteOverride(CXTPChartDeviceContext* pDC);
};

class _XTP_EXT_CLASS CXTPChartGdiPolygonAntialiasingDeviceCommand : public CXTPChartPolygonAntialiasingDeviceCommand
{
public:
	//-----------------------------------------------------------------------
	// Summary:
	//     Constructs a CXTPChartGdiPolygonAntialiasingDeviceCommand object.
	// Parameters:
	//      bAntiAlias - TRUE if the antialiasing is enabled and FALSE if the
	//      antialiasing is disabled.
	// Remarks:
	//-----------------------------------------------------------------------
	CXTPChartGdiPolygonAntialiasingDeviceCommand(BOOL bAntiAlias = TRUE);
	~CXTPChartGdiPolygonAntialiasingDeviceCommand();

protected:

	//-----------------------------------------------------------------------
	// Summary:
	//      This override do some ground works if any, before the native mode
	//      antialiased drawing of polygons.
	// Parameters:
	//     pDC - A pointer to the chart native device context.
	// Remarks:
	//-----------------------------------------------------------------------
	virtual void BeforeExecute(CXTPChartDeviceContext* pDC);

	//-----------------------------------------------------------------------
	// Summary:
	//     This override do some final cut if any, after the native antialiased
	//     drawing of polygons.
	// Parameters:
	//     pDC - A pointer to the chart native device context.
	// Remarks:
	//-----------------------------------------------------------------------
	virtual void AfterExecute(CXTPChartDeviceContext* pDC);

protected:
	long m_bOldAntiAlias;           //The previous value of anti aliasing.
};


#include "Common/Base/Diagnostic/XTPEnableNoisyWarnings.h"
#endif //#if !defined(__XTPCHARTGDIPOLYGONDEVICECOMMAND_H__)
//}}AFX_CODEJOCK_PRIVATE
