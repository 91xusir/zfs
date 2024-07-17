// XTPChart2dPieSeriesLabel.h
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
#if !defined(__XTPCHART2DPIESERIESLABEL_H__)
#define __XTPCHART2DPIESERIESLABEL_H__
//}}AFX_CODEJOCK_PRIVATE

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "Common/Base/Diagnostic/XTPDisableNoisyWarnings.h"

class CXTPChartSeriesView;
class CXTPChartPieSeriesPointViewBase;
class CXTPChartPieSeriesStyleBase;

//===========================================================================
// Summary:
//     CXTPChartPieSeriesLabelBase is a kind of CXTPChartPieSeriesLabel, this class
//     abstracts the label of a pie series.
// Remarks:
//
//===========================================================================
class _XTP_EXT_CLASS CXTPChartPieSeriesLabel : public CXTPChartPieSeriesLabelBase
{
	DECLARE_SERIAL(CXTPChartPieSeriesLabel);

public:
	//-----------------------------------------------------------------------
	// Summary:
	//     Constructs a CXTPChartPieSeriesLabel object.
	// Remarks:
	//-----------------------------------------------------------------------
	CXTPChartPieSeriesLabel();

protected:
	//-------------------------------------------------------------------------
	// Summary:
	//     This function creates the view of the pie series.
	// Parameters:
	//     pDC - The chart device context object pointer.
	//     pPointView - The series point view.
	// Returns:
	//     Returns an pointer to CXTPChartSeriesLabelView which is a kind of
	//     CXTPChartElementView object newly created.
	// Remarks:
	// See Also:
	//-------------------------------------------------------------------------
	virtual CXTPChartElementView* CreateView(CXTPChartDeviceContext* pDC, 
		CXTPChartSeriesPointView* pPointView, CXTPChartElementView* pParentView);

public:
	//-------------------------------------------------------------------------
	// Summary:
	//     Call this function to calculate the anchor points and angles of the
	//     pie label stem.
	// Parameters:
	//     pPointView       - The pie series point view.
	//     borderThickness  - The border thickness of the label.
	//     lineAngle        - The line(stem) angle, out param.
	// Returns:
	//     Returns CXTPChartPointF object denoting the anchor point.
	// Remarks:
	// See Also:
	//-------------------------------------------------------------------------
	CXTPChartPointF CalculateAnchorPointAndAngles(CXTPChartPieSeriesPointViewBase* pPointView,
		int borderThickness, double& lineAngle);
};

//-----------------------------------------------------------------------
// Summary:
//      Defines 2D series label view.
//-----------------------------------------------------------------------
class _XTP_EXT_CLASS CXTPChartPieSeriesLabelView : public CXTPChartPieSeriesLabelViewBase
{
public:
	//-----------------------------------------------------------------------
	// Summary:
	//      Constructs CXTPChartPieSeriesLabelView object.
	// Parameters:
	//      pLabel - Associated label model pointer.
	//      pPointView - Point view object pointer associated with the label.
	//      pParentView - Parent view pointer.
	//-----------------------------------------------------------------------
	CXTPChartPieSeriesLabelView(CXTPChartSeriesLabel* pLabel, CXTPChartSeriesPointView* pPointView, CXTPChartElementView* pParentView);

public:
	//-----------------------------------------------------------------------
	// Summary:
	//      Calculates label and connector layout for the device context specified.
	// Parameters:
	//      pDC - Device context pointer
	//-----------------------------------------------------------------------
	void CalculateLayout(CXTPChartDeviceContext* pDC);

	//-----------------------------------------------------------------------
	// Summary:
	//      Creates connector drawing device command for the device context specified.
	// Parameters:
	//      pDC - Device context pointer for which the drawing command is to be created.
	// Returns:
	//      A new instance of connector drawing device command or NULL if no command
	//      is provided.
	//-----------------------------------------------------------------------
	virtual CXTPChartDeviceCommand* CreateConnectorDeviceCommand(CXTPChartDeviceContext* pDC);
	
	//-----------------------------------------------------------------------
	// Summary:
	//      Creates view drawing device command for the device context specified.
	// Parameters:
	//      pDC - Device context pointer for which the drawing command is to be created.
	// Returns:
	//      A new instance of view drawing device command or NULL if no command
	//      is provided.
	//-----------------------------------------------------------------------
	virtual CXTPChartDeviceCommand* CreateDeviceCommand(CXTPChartDeviceContext* pDC);

protected:
	CXTPChartRectF m_rcBounds; // The determined boundary rectange.
	CXTPChartPointF m_ptStartPoint; // The determined connector start point.
	double m_dLineAngle; // The determined connector line angle.

	friend class CXTPChartPieSeriesLabelsView;
};


//-----------------------------------------------------------------------
// Summary:
//      Defines 2D series label collection view.
//-----------------------------------------------------------------------
class _XTP_EXT_CLASS CXTPChartPieSeriesLabelsView : public CXTPChartPieSeriesLabelsViewBase
{
public:
	//-----------------------------------------------------------------------
	// Summary:
	//      Constructs CXTPChartPieSeriesLabelsView object.
	// Parameters:
	//      pParentView - Parent view pointer.
	//-----------------------------------------------------------------------
	CXTPChartPieSeriesLabelsView(CXTPChartElementView* pParentView);

public:
	//-----------------------------------------------------------------------
	// Summary:
	//      Creates view drawing device command for the device context specified.
	// Parameters:
	//      pDC - Device context pointer for which the drawing command is to be created.
	// Returns:
	//      A new instance of view drawing device command or NULL if no command
	//      is provided.
	//-----------------------------------------------------------------------
	virtual CXTPChartDeviceCommand* CreateDeviceCommand(CXTPChartDeviceContext* pDC);

	//-----------------------------------------------------------------------
	// Summary:
	//      Resolves label overlapping in implementation specific way.
	//-----------------------------------------------------------------------
	virtual void ResolveOverlapping();

protected:
	//-----------------------------------------------------------------------
	// Summary:
	//      Resolves label overlapping for column mode.
	// Parameters:
	//      arrColumn - Label collumn array reference.
	//-----------------------------------------------------------------------
	void ResolveOverlappingColumn(CArray<CXTPChartPieSeriesLabelView*, CXTPChartPieSeriesLabelView*>& arrColumn);
};


#include "Common/Base/Diagnostic/XTPEnableNoisyWarnings.h"
#endif //#if !defined(__XTPCHART2DPIESERIESLABEL_H__)
