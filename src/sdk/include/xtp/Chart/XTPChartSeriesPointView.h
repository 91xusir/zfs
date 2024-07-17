// XTPChartSeriesPointView.h
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
#if !defined(__XTPCHARTSERIESPOINTVIEW_H__)
#define __XTPCHARTSERIESPOINTVIEW_H__
//}}AFX_CODEJOCK_PRIVATE

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "Common/Base/Diagnostic/XTPDisableNoisyWarnings.h"

class CXTPChartSeriesView;
class CXTPChartSeriesPoint;
class CXTPChartSeriesStyle;
class CXTPChartSeries;
class CXTPChartDiagramView;
class CXTPChartDiagram;
class CXTPChartLegendView;

//-----------------------------------------------------------------------
// Summary:
//      Represents chart series point view.
//-----------------------------------------------------------------------
class _XTP_EXT_CLASS CXTPChartSeriesPointView : public CXTPChartElementView, public CXTPChartLegendItemView
{
	DECLARE_DYNAMIC(CXTPChartSeriesPointView);

public:
	//-----------------------------------------------------------------------
	// Summary:
	//      Constructs the view object.
	// Parameters:
	//      pPoint - Point model object pointer.
	//      pParentView - Parent view pointer.
	//      bAddToParent - If TRUE, the view will be added to parent's child elements.
	//-----------------------------------------------------------------------
	CXTPChartSeriesPointView(CXTPChartSeriesPoint* pPoint, CXTPChartElementView* pParentView);
	CXTPChartSeriesPointView(CXTPChartSeriesPoint* pPoint, 
		CXTPChartElementView* pParentView, BOOL bAddToParent); // <combine CXTPChartSeriesPointView::CXTPChartSeriesPointView@CXTPChartSeriesPoint*@CXTPChartElementView*>

public:
	//-----------------------------------------------------------------------
	// Summary:
	//      Obtains point model object.
	// Returns:
	//      Point model object pointer.
	//-----------------------------------------------------------------------
	CXTPChartSeriesPoint* GetPoint() const;

	//-----------------------------------------------------------------------
	// Summary:
	//      Obtains parent series view.
	// Returns:
	//      Parent series view pointer.
	//-----------------------------------------------------------------------
	CXTPChartSeriesView* GetSeriesView() const;

	//-----------------------------------------------------------------------
	// Summary:
	//      Obtain view color values.
	// Returns:
	//      View color value.
	//-----------------------------------------------------------------------
	CXTPChartColor GetColor() const;
	CXTPChartColor GetColor2() const; // <combine CXTPChartSeriesPointView::GetColor>

	//-----------------------------------------------------------------------
	// Summary:
	//      Obtains point interval value.
	// Returns:
	//      Point interval value.
	//-----------------------------------------------------------------------
	double GetInternalValue() const;

	//-------------------------------------------------------------------------
	// Summary:
	//     This function returns the legend name, it is a pure virtual
	//     function so it should be implemented in the derived classes.
	// Returns:
	//     Returns the legend name represented by the CXTPChartString class.
	// Remarks:
	// See Also:
	//-------------------------------------------------------------------------
	virtual CXTPChartString GetLegendName() const;

	//-------------------------------------------------------------------------
	// Summary:
	//     This function creates a CXTPChartDeviceCommand object, this object
	//     represents the rendering of a legend item in the chart.
	// Returns:
	//     Returns CXTPChartDeviceCommand object, this object handles
	//     the rendering of an element in the chart. Here it handles
	//     the drawing of the legend item.
	// Remarks:
	// See Also:
	//-------------------------------------------------------------------------
	virtual CXTPChartDeviceCommand* CreateLegendDeviceCommand(CXTPChartDeviceContext* pDC, CRect rcBounds);

	//-----------------------------------------------------------------------
	// Summary:
	//      Provides a default empty implementation for min/max range update request.
	// Parameters:
	//      nMinValue - The updated minimal value.
	//      nMaxValue - The updated maximal value.
	//-----------------------------------------------------------------------
	virtual void UpdateMinMaxRange(double& nMinValue, double& nMaxValue) const;
public:

	double m_dInternalValue; // Point internal value.

protected:
	CXTPChartSeriesPoint* m_pPoint; // Point model object pointer.

	friend class CXTPChartSeriesView;
};

AFX_INLINE CXTPChartSeriesPoint* CXTPChartSeriesPointView::GetPoint() const {
	return m_pPoint;
}

AFX_INLINE CXTPChartSeriesView* CXTPChartSeriesPointView::GetSeriesView() const {
	return (CXTPChartSeriesView*)(GetParentView()->GetParentView());
}

AFX_INLINE double CXTPChartSeriesPointView::GetInternalValue() const {
	return m_dInternalValue;
}

AFX_INLINE void CXTPChartSeriesPointView::UpdateMinMaxRange(double& /*nMinValue*/, double& /*nMaxValue*/) const {
}


#include "Common/Base/Diagnostic/XTPEnableNoisyWarnings.h"
#endif //#if !defined(__XTPCHARTSERIESPOINTVIEW_H__)
