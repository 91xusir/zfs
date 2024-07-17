// XTPChartGdiCircleDeviceCommand.h
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
#if !defined(__XTPCHARTGDICIRCLEDEVICECOMMAND_H__)
#define __XTPCHARTGDICIRCLEDEVICECOMMAND_H__

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "Common/Base/Diagnostic/XTPDisableNoisyWarnings.h"


//===========================================================================
// Summary:
//     This class represents a gradient circle device command,which is a kind of CXTPChartDeviceCommand.
//     It specifically handles the rendering of gradient circular shapes in a chart.
// Remarks:
//===========================================================================
class _XTP_EXT_CLASS CXTPChartGdiSolidCircleDeviceCommand : public CXTPChartCircleDeviceCommand
{
public:
	//-----------------------------------------------------------------------
	// Summary:
	//     Constructs a CXTPChartGdiSolidCircleDeviceCommand object.
	// Parameters:
	//     center    - The center point of the circle.
	//     radius    - The radius of the circle.
	//     color     - The color of the circle.
	// Remarks:
	//-----------------------------------------------------------------------
	CXTPChartGdiSolidCircleDeviceCommand(const CXTPChartPointF& center, double radius, const CXTPChartColor& color);

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
	CXTPChartColor m_color;              //The start color.
};

//===========================================================================
// Summary:
//     This class represents a gradient circle device command,which is a kind of CXTPChartDeviceCommand.
//     It specifically handles the rendering of gradient circular shapes in a chart.
// Remarks:
//===========================================================================
class _XTP_EXT_CLASS CXTPChartGdiGradientCircleDeviceCommand : public CXTPChartCircleDeviceCommand
{
public:
	//-----------------------------------------------------------------------
	// Summary:
	//     Constructs a CXTPChartGdiGradientCircleDeviceCommand object.
	// Parameters:
	//     center    - The center point of the circle.
	//     radius    - The radius of the circle.
	//     color     - The start color of the gradient.
	//     color2    - The end color of the gradient.
	//     nMode     - The linear gradient mode, it can be horizontal, vertical,
	//                 forward diagonal or backward diagonal.
	// Remarks:
	//-----------------------------------------------------------------------
	CXTPChartGdiGradientCircleDeviceCommand(const CXTPChartPointF& center, double radius, const CXTPChartColor& color, const CXTPChartColor& color2, XTPChartLinearGradientMode nMode);

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
	CXTPChartColor m_color;              //The start color.
	CXTPChartColor m_color2;             //The end color.
	XTPChartLinearGradientMode m_nMode;  //The gradient mode used.
};

//===========================================================================
// Summary:
//     This class represents a gradient hatch device command,which is a kind of CXTPChartCircleDeviceCommand.
//     It specifically handles the rendering of hatch circular shapes in a chart.
// Remarks:
//===========================================================================
class _XTP_EXT_CLASS CXTPChartGdiHatchCircleDeviceCommand : public CXTPChartCircleDeviceCommand
{
public:
	//-----------------------------------------------------------------------
	// Summary:
	//     Constructs a CXTPChartGdiHatchCircleDeviceCommand object.
	// Parameters:
	//     center    - The center point of the circle.
	//     radius    - The radius of the circle.
	//     color     - The start color of the gradient.
	//     color2    - The end color of the gradient.
	//     nStyle    - Chart hatch style.
	// Remarks:
	//-----------------------------------------------------------------------
	CXTPChartGdiHatchCircleDeviceCommand(const CXTPChartPointF& center, double radius, const CXTPChartColor& color, const CXTPChartColor& color2, XTPChartHatchStyle nStyle);

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
	CXTPChartColor m_color;              //The start color.
	CXTPChartColor m_color2;             //The end color.
	XTPChartHatchStyle m_nStyle;         //The hatch mode used.
};

class _XTP_EXT_CLASS CXTPChartGdiBoundedCircleDeviceCommand : public CXTPChartBoundedCircleDeviceCommand
{
public:
	//-----------------------------------------------------------------------
	// Summary:
	//     Constructs a CXTPChartGdiBoundedCircleDeviceCommand object.
	// Parameters:
	//     center    - The center point of the circle.
	//     radius    - The radius of the circle.
	//     color     - The color of boundary.
	//     thickness - The thickness of the boundary.
	// Remarks:
	//-----------------------------------------------------------------------
	CXTPChartGdiBoundedCircleDeviceCommand(const CXTPChartPointF& center, double radius, const CXTPChartColor& color, int thickness);

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
};


#include "Common/Base/Diagnostic/XTPEnableNoisyWarnings.h"
#endif //#if !defined(__XTPCHARTGDICIRCLEDEVICECOMMAND_H__)
//}}AFX_CODEJOCK_PRIVATE
