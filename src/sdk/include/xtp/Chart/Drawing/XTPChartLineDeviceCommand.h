// XTPChartLineDeviceCommand.h
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
#if !defined(__XTPCHARTLINEDEVICECOMMAND_H__)
#define __XTPCHARTLINEDEVICECOMMAND_H__
//}}AFX_CODEJOCK_PRIVATE

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "Common/Base/Diagnostic/XTPDisableNoisyWarnings.h"

// Array type used for dashed chart lines.
typedef CArray<float, float> CXTPChartLineDashArray;

//-----------------------------------------------------------------------
// Summary:
//      Provides interface for 2D line drawing device command.
//-----------------------------------------------------------------------
class _XTP_EXT_CLASS CXTPChartLineDeviceCommand : public CXTPChartDeviceCommand
{
protected:
	//-----------------------------------------------------------------------
	// Summary:
	//      Constructs CXTPChartLineDeviceCommand object.
	// Parameters:
	//      p1 - Start point.
	//      p2 - End point.
	//      color - Line color.
	//      thickness - Line thickness in pixels.
	//-----------------------------------------------------------------------
	CXTPChartLineDeviceCommand(const CXTPPoint2f& p1, const CXTPPoint2f& p2, const CXTPChartColor& color, int thickness);

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
	virtual CXTPChartElement* HitTest(CPoint point, CXTPChartElement* pParent) const;

protected:
	CXTPPoint2f m_p1;  //The first point.
	CXTPPoint2f m_p2;  //The second point.
	CXTPChartColor m_color;     //The line color.
	int m_nThickness;           //The thickness of the line.
};

//-----------------------------------------------------------------------
// Summary:
//      Provides interface for 2D poly-line drawing device command.
//-----------------------------------------------------------------------
class _XTP_EXT_CLASS CXTPChartPolylineDeviceCommand : public CXTPChartDeviceCommand
{
protected:
	//-----------------------------------------------------------------------
	// Summary:
	//      Constructs CXTPChartPolylineDeviceCommand object.
	// Parameters:
	//      p - A collection of poly-line points.
	//      thickness - Line thickness in pixels.
	//-----------------------------------------------------------------------
	CXTPChartPolylineDeviceCommand(const CXTPChartPoints& p, int thickness);

protected:
	CXTPChartPoints m_p;     // A collection of poly-line points.
	int m_nThickness;       // Line thickness in pixels.
};

//-----------------------------------------------------------------------
// Summary:
//      Provides interface for 2D spline drawing device command.
//-----------------------------------------------------------------------
class _XTP_EXT_CLASS CXTPChartSplineDeviceCommand : public CXTPChartDeviceCommand
{
protected:
	//-----------------------------------------------------------------------
	// Summary:
	//      Constructs CXTPChartSplineDeviceCommand object.
	// Parameters:
	//      p - A collection of spline points.
	//      thickness - Line thickness in pixels.
	//-----------------------------------------------------------------------
	CXTPChartSplineDeviceCommand(const CXTPChartPoints& p, int thickness);

protected:
	CXTPChartPoints m_p;     // A collection of spline points.
	int m_nThickness;       // Line thickness in pixels.
};

//-----------------------------------------------------------------------
// Summary:
//      Provides interface for 3D line drawing device command.
//-----------------------------------------------------------------------
class _XTP_EXT_CLASS CXTPChart3dLineDeviceCommand : public CXTPChartDeviceCommand
{
protected:
	//-----------------------------------------------------------------------
	// Summary:
	//      Constructs CXTPChart3dLineDeviceCommand object.
	// Parameters:
	//      p1 - Start point.
	//      p2 - End point.
	//      color - Line color.
	//      thickness - Line thickness in pixels.
	//-----------------------------------------------------------------------
	CXTPChart3dLineDeviceCommand(const CXTPPoint3d& p1, const CXTPPoint3d& p2, const CXTPChartColor& color, int thickness);

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
	virtual CXTPChartElement* HitTest(CPoint point, CXTPChartElement* pParent) const;

	//-----------------------------------------------------------------------
	// Summary:
	//      Notifies that a command enters AfterExecute step.
	// Parameters:
	//      pCommand - Sender command pointer.
	//      pDC - Device context pointer for which a command has been executed.
	//-----------------------------------------------------------------------
	virtual void AfterExecute(CXTPChartDeviceContext* pDC);

protected:
	CXTPPoint3d m_p1;  //The first point.
	CXTPPoint3d m_p2;  //The second point.
	CXTPChartColor m_color;     //The line color.
	int m_nThickness;           //The thickness of the line.

private:
	BOOL m_bProjected;
	CPoint m_pt2d1;
	CPoint m_pt2d2;
};


#include "Common/Base/Diagnostic/XTPEnableNoisyWarnings.h"
#endif //#if !defined(__XTPCHARTLINEDEVICECOMMAND_H__)
