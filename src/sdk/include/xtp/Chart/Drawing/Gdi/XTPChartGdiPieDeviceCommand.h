// XTPChartGdiPieDeviceCommand.h
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
#if !defined(__XTPCHARTGDIPIEDEVICECOMMAND_H__)
#define __XTPCHARTGDIPIEDEVICECOMMAND_H__

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "Common/Base/Diagnostic/XTPDisableNoisyWarnings.h"

namespace Gdiplus
{
	class GpPath;
	class GpBrush;
}

//===========================================================================
// Summary:
//     This class represents a chart pie device command,which is a kind of
//     CXTPChartDeviceCommand. It specifically handles the rendering of a pie
//     in a chart.
// Remarks:
//===========================================================================
class _XTP_EXT_CLASS CXTPChartGdiPieDeviceCommand : public CXTPChartPieDeviceCommand
{
protected:
	//-----------------------------------------------------------------------
	// Summary:
	//     Constructs a CXTPChartPieDeviceCommand object.
	// Parameters:
	//     ptCenter       - The center point of the pie.
	//     dMajorSemiAxis - The major semi axis of the ellipse.
	//     dMinorSemiAxis - The minor semi axis of the ellipse.
	//     dStartAngle    - The start angle of the pie.
	//     dSweepAngle    - The sweep angle of the pie.
	//     dDepth         - The depth of the pie or torus.
	//     nHolePercent   - Determines the size of the hole when drawing a doughnut.
	//-----------------------------------------------------------------------
	CXTPChartGdiPieDeviceCommand(const CXTPPoint3d& ptCenter, double dMajorSemiAxis, double dMinorSemiAxis, double dStartAngle, double dSweepAngle,
		double dDepth, double nHolePercent);

protected:
	Gdiplus::GpPath* CreatePieGraphicsPath(const CXTPPoint3d& center, double majorSemiaxis, double minorSemiaxis, double holePercent, double startAngle, double sweepAngle) const;
	void CalculateStartFinishPoints(const CXTPPoint3d& center, double majorSemiaxis, double minorSemiaxis, double startAngle, double sweepAngle, CXTPChartPointF& startPoint, CXTPChartPointF& finishPoint) const;

	virtual CXTPChartElement* HitTest(CPoint point, CXTPChartElement* pParent) const;
};

class _XTP_EXT_CLASS CXTPChartGdiGradientPieDeviceCommand : public CXTPChartGdiPieDeviceCommand
{
public:
	CXTPChartGdiGradientPieDeviceCommand(const CXTPPoint3d& center, double majorSemiAxis, double minorSemiAxis, double startAngle, double sweepAngle, double depth, double holePercent, const CXTPChartRectF& gradientBounds, const CXTPChartColor& color, const CXTPChartColor& color2);

protected:
	void ExecuteOverride(CXTPChartDeviceContext* pDC);

	Gdiplus::GpBrush* CreateBrush();

protected:
	CXTPChartRectF m_rcGradientBounds;
	CXTPChartColor m_color;
	CXTPChartColor m_color2;
};


class _XTP_EXT_CLASS CXTPChartGdiBoundedPieDeviceCommand : public CXTPChartGdiPieDeviceCommand
{
public:
	CXTPChartGdiBoundedPieDeviceCommand(const CXTPPoint3d& center, double majorSemiAxis, double minorSemiAxis, double startAngle, double sweepAngle, double depth, int holePercent, const CXTPChartColor& color, int nThickness);

protected:
	void ExecuteOverride(CXTPChartDeviceContext* pDC);

protected:
	CXTPChartColor m_color;
	int m_nThickness;
};


#include "Common/Base/Diagnostic/XTPEnableNoisyWarnings.h"
#endif //#if !defined(__XTPCHARTGDIPIEDEVICECOMMAND_H__)
//}}AFX_CODEJOCK_PRIVATE
