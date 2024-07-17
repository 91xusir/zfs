// XTPChartPieSeriesStyleBase.h
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
#if !defined(__XTPCHARTPIESERIESSTYLEBASE_H__)
#define __XTPCHARTPIESERIESSTYLEBASE_H__
//}}AFX_CODEJOCK_PRIVATE

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "Common/Base/Diagnostic/XTPDisableNoisyWarnings.h"

class CXTPChartSeriesView;
class CXTPChartBorder;


//===========================================================================
// Summary:
//     This class abstracts the basic style of a pie serie.This class is a kind of
//     CXTPChartSeriesStyle.
// Remarks:
//     A pie chart (or a circle graph) is a circular chart divided into sectors,
//     illustrating percents. In a pie chart, the arc length of each sector (and
//     consequently its central angle and area), is proportional to the quantity
//     it represents. Together, the sectors create a full disk.
//===========================================================================
class _XTP_EXT_CLASS CXTPChartPieSeriesStyleBase : public CXTPChartSeriesStyle
{
	DECLARE_DYNAMIC(CXTPChartPieSeriesStyleBase)

protected:
	//-----------------------------------------------------------------------
	// Summary:
	//     Constructs a CXTPChartPieSeriesStyleBase object.
	// Remarks:
	//-----------------------------------------------------------------------
	CXTPChartPieSeriesStyleBase();

	//-------------------------------------------------------------------------
	// Summary:
	//     Destroys a CXTPChartPieSeriesStyleBase object, handle cleanup.
	//-------------------------------------------------------------------------
	virtual ~CXTPChartPieSeriesStyleBase();

public:
	//-------------------------------------------------------------------------
	// Summary:
	//     Call this function to get the hole percent of the pie chart.
	// Returns:
	//     An integer value specifying the hole percentage.
	//-------------------------------------------------------------------------
	int GetHolePercent() const;

	//-------------------------------------------------------------------------
	// Summary:
	//     Call this function to set the hole percent of the pie chart.
	// Parameters:
	//     nHolePercent - An integer value specifying the hole percentage.
	//-------------------------------------------------------------------------
	void SetHolePercent(int nHolePercent);

	//-------------------------------------------------------------------------
	// Summary:
	//     Call this function to get the exploded distance percent of the pie
	//     chart.
	// Returns:
	//     An integer value specifying the hole percentage.
	// Remarks:
	//    A chart with one or more sectors separated from the rest of the disk
	//    is known as an exploded pie chart.
	//-------------------------------------------------------------------------
	int GetExplodedDistancePercent() const;
	//-------------------------------------------------------------------------
	// Summary:
	//     Call this function to set the exploded distance percent of the pie
	//     chart.
	// Parameters:
	//     nExplodedDistancePercent - An integer value specifying the exploded
	//                                distance percentage.
	// Remarks:
	//    A chart with one or more sectors separated from the rest of the disk
	//    is known as an exploded pie chart.
	//-------------------------------------------------------------------------
	void SetExplodedDistancePercent(int nExplodedDistancePercent);

	//-------------------------------------------------------------------------
	// Summary:
	//     Call this function to get the rotation of the sectors of the pie chart.
	// Returns:
	//     An integer value specifying the rotation angle in degree.
	//-------------------------------------------------------------------------
	int GetRotation() const;

	//-------------------------------------------------------------------------
	// Summary:
	//     Call this function to set the rotation of the sectors of the pie chart.
	// Parameters:
	//     nRotation - An integer value specifying the rotation angle in degree.
	//-------------------------------------------------------------------------
	void SetRotation(int nRotation);

	//-------------------------------------------------------------------------
	// Summary:
	//     Call this function to get the depth of 3D pie chart.
	// Returns:
	//     An integer value specifying the depth of 3D pie chart.
	//-------------------------------------------------------------------------
	virtual int GetDepth() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member function to Store/Load an event using the
	//     specified data object.
	// Parameters:
	//     pPX - Source or destination CXTPPropExchange data object reference.
	// Remarks:
	//     This member function is used to store or load event data to or
	//     from a storage.
	//-----------------------------------------------------------------------
	void DoPropExchange(CXTPPropExchange* pPX);
	
	//-----------------------------------------------------------------------
	// Summary:
	//      Obtains border object pointer.
	// Returns:
	//      Associated border object pointer.
	//-----------------------------------------------------------------------
	CXTPChartBorder* GetBorder() const;

	//-----------------------------------------------------------------------
	// Summary:
	//      Obtains border color.
	// Returns:
	//      Border color value.
	//-----------------------------------------------------------------------
	CXTPChartColor GetBorderActualColor() const;

protected:
	//-------------------------------------------------------------------------
	// Summary:
	//     Call this function to create the pie diagram object.
	// Returns:
	//     A pointer to CXTPChartDiagram object which points to a newly created
	//     CXTPChart2dPieDiagramBase/CXTPChart3dPieDiagramBase object.
	//-------------------------------------------------------------------------
	virtual CXTPChartDiagram* CreateDiagram() = 0;

	//-------------------------------------------------------------------------
	// Summary:
	//     Call this function to create the view of the pie series.
	// Parameters:
	//     pSeries       - A pointer to chart series  object.
	//     pDiagramView  - A pointer to the diagram view object.
	// Returns:
	//     A pointer to CXTPChartSeriesView object which points to a newly created
	//     CXTPChartPieSeriesViewBase object.
	//-------------------------------------------------------------------------
	virtual CXTPChartSeriesView* CreateView(CXTPChartSeries* pSeries, CXTPChartDiagramView* pDiagramView) = 0;

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this function to check whether chart diagram object passed
	//     is a kind of CXTPChart2dPieDiagramBase/CXTPChart3dPieDiagramBase.
	// Parameters:
	//     pDiagram - A pointer to a chart diagram, whose type is to be identified.
	// Returns:
	//     TRUE if the chart diagram is a pie diagram and FALSE if not.
	// Remarks:
	//-----------------------------------------------------------------------
	virtual BOOL IsStyleDiagram(CXTPChartDiagram* pDiagram) const;

protected:
#ifdef _XTP_ACTIVEX
public:
	//{{AFX_CODEJOCK_PRIVATE
	DECLARE_DISPATCH_MAP()
	DECLARE_INTERFACE_MAP()

	afx_msg LPDISPATCH OleGetBorder();
//}}AFX_CODEJOCK_PRIVATE
#endif

protected:
	int m_nHolePercent;             //The hole percentage.
	int m_nExplodedDistancePercent; //The exploded distance percentage.
	int m_nRotation;                //The sector rotation angle.
	CXTPChartBorder* m_pBorder;     // Associated border information.
};

AFX_INLINE int CXTPChartPieSeriesStyleBase::GetHolePercent() const {
	return m_nHolePercent;
}
AFX_INLINE void CXTPChartPieSeriesStyleBase::SetHolePercent(int nHolePercent) {
	m_nHolePercent = nHolePercent;
	OnChartChanged();
}
AFX_INLINE int CXTPChartPieSeriesStyleBase::GetExplodedDistancePercent() const {
	return m_nExplodedDistancePercent;
}
AFX_INLINE void CXTPChartPieSeriesStyleBase::SetExplodedDistancePercent(int nExplodedDistancePercent) {
	m_nExplodedDistancePercent = nExplodedDistancePercent;
	OnChartChanged();
}
AFX_INLINE int CXTPChartPieSeriesStyleBase::GetRotation() const {
	return m_nRotation;
}
AFX_INLINE void CXTPChartPieSeriesStyleBase::SetRotation(int nRotation) {
	m_nRotation = nRotation;
	OnChartChanged();
}
AFX_INLINE CXTPChartBorder* CXTPChartPieSeriesStyleBase::GetBorder() const {
	return m_pBorder;
}


#include "Common/Base/Diagnostic/XTPEnableNoisyWarnings.h"
#endif //#if !defined(__XTPCHARTPIESERIESSTYLEBASE_H__)
