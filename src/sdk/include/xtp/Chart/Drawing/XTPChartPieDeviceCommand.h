// XTPChartPieDeviceCommand.h
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
#if !defined(__XTPCHARTPIEDEVICECOMMAND_H__)
#define __XTPCHARTPIEDEVICECOMMAND_H__
//}}AFX_CODEJOCK_PRIVATE

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "Common/Base/Diagnostic/XTPDisableNoisyWarnings.h"

//===========================================================================
// Summary:
//     This class a common implementation for a pie chart device command.
// Remarks:
//===========================================================================
class _XTP_EXT_CLASS CXTPChartPieDeviceCommandImpl
{
	friend class CXTPChartPieDeviceCommand;
	friend class CXTPChart3dPieDeviceCommand;

private:
	//-----------------------------------------------------------------------
	// Summary:
	//     Constructs a CXTPChartPieDeviceCommandImpl object.
	// Parameters:
	//     ptCenter       - The center point of the pie.
	//     dMajorSemiAxis - The major semi axis of the ellipse.
	//     dMinorSemiAxis - The minor semi axis of the ellipse.
	//     dStartAngle    - The start angle of the pie.
	//     dSweepAngle    - The sweep angle of the pie.
	//     dDepth         - The depth of the pie or torus.
	//     nHolePercent   - Determines the size of the hole when drawing a doughnut.
	//-----------------------------------------------------------------------
	CXTPChartPieDeviceCommandImpl(const CXTPPoint3d& ptCenter, double dMajorSemiAxis, double dMinorSemiAxis, double dStartAngle, double dSweepAngle,
		double dDepth, double nHolePercent);

protected:
	//-----------------------------------------------------------------------
	// Summary:
	//      Handles object destruction.
	//-----------------------------------------------------------------------
	virtual ~CXTPChartPieDeviceCommandImpl();

public:
	CXTPPoint3d m_ptCenter; // The center point of the pie.
	double m_dMajorSemiAxis; // The major semi axis of the ellipse.
	double m_dMinorSemiAxis; // The minor semi axis of the ellipse.
	double m_dStartAngle; // The start angle of the pie.
	double m_dSweepAngle; // The sweep angle of the pie.
	double m_dDepth; // The depth of the pie or torus.
	double m_dHolePercent; // Determines the size of the hole when drawing a doughnut.

	double m_dInnerMajorSemiAxis; // The inner size of the major semi axis of the ellipse.
	double m_dInnerMinorSemiAxis; // The inner size of the minor semi axis of the ellipse.

	double m_dFacetSize; // Facet size.
	double m_dRadius; // Pie radius.
	double m_dCorrectedDepth; // Pie corrected depth.
	double m_dInnerRadius; // Pie inner radius.

	BOOL m_bDoughnut; // TRUE indicates doughnut style.
};

//-----------------------------------------------------------------------
// Summary:
//      Base class implementation for 2D pie chart device command.
//-----------------------------------------------------------------------
class _XTP_EXT_CLASS CXTPChartPieDeviceCommand
	: public CXTPChartDeviceCommand
	, protected CXTPChartPieDeviceCommandImpl
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
	CXTPChartPieDeviceCommand(const CXTPPoint3d& ptCenter, double dMajorSemiAxis, double dMinorSemiAxis, double dStartAngle, double dSweepAngle,
		double dDepth, double nHolePercent);
};

// CXTPChart2dPieDeviceCommand alias is necessary fo consistency.
typedef CXTPChartPieDeviceCommand CXTPChart2dPieDeviceCommand;

//-----------------------------------------------------------------------
// Summary:
//      Base class implementation for 3D pie chart device command.
//-----------------------------------------------------------------------
class _XTP_EXT_CLASS CXTPChart3dPieDeviceCommand
	: public CXTPChart3dDeviceCommand
	, protected CXTPChartPieDeviceCommandImpl
{
protected:
	//-----------------------------------------------------------------------
	// Summary:
	//     Constructs a CXTPChartPieDeviceCommand object.
	// Parameters:
	//     nAntialiasingPolicy - Anti-aliasing policy identifier that defines
	//                           how command behavesdepending on device context
	//                           rendering states in regards of anti-aliasing.
	//     ptCenter       - The center point of the pie.
	//     dMajorSemiAxis - The major semi axis of the ellipse.
	//     dMinorSemiAxis - The minor semi axis of the ellipse.
	//     dStartAngle    - The start angle of the pie.
	//     dSweepAngle    - The sweep angle of the pie.
	//     dDepth         - The depth of the pie or torus.
	//     nHolePercent   - Determines the size of the hole when drawing a doughnut.
	//-----------------------------------------------------------------------
	CXTPChart3dPieDeviceCommand(XTPChart3dAntialiasingPolicy nAntialiasingPolicy, const CXTPPoint3d& ptCenter, 
		double dMajorSemiAxis, double dMinorSemiAxis, double dStartAngle, double dSweepAngle,
		double dDepth, double nHolePercent);
};


#include "Common/Base/Diagnostic/XTPEnableNoisyWarnings.h"
#endif //#if !defined(__XTPCHARTPIEDEVICECOMMAND_H__)
