// XTPChartOpenGLLineDeviceCommand.h
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
#if !defined(__XTPCHARTOPENGLLINEDEVICECOMMAND_H__)
#define __XTPCHARTOPENGLLINEDEVICECOMMAND_H__

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
class _XTP_EXT_CLASS CXTPChartOpenGLSolidLineDeviceCommand : public CXTPChart3dLineDeviceCommand
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
	CXTPChartOpenGLSolidLineDeviceCommand(const CXTPPoint3d& p1, const CXTPPoint3d& p2, const CXTPChartColor& color, int thickness);

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


#include "Common/Base/Diagnostic/XTPEnableNoisyWarnings.h"
#endif //#if !defined(__XTPCHARTOPENGLLINEDEVICECOMMAND_H__)
//}}AFX_CODEJOCK_PRIVATE
