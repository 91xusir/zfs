// XTPChart3dPieSeriesStyle.h
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
#if !defined(__XTPCHART3DPIESERIESSTYLE_H__)
#define __XTPCHART3DPIESERIESSTYLE_H__
//}}AFX_CODEJOCK_PRIVATE

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "Common/Base/Diagnostic/XTPDisableNoisyWarnings.h"

class CXTPChartDiagramView;
class CXTPChartSeries;

//===========================================================================
// Summary:
//     This class abstracts the pie series style.This class is a kind of
//     CXTPChartPieSeriesStyleBase.
// Remarks:
//     A pie chart (or a circle graph) is a circular chart divided into sectors,
//     illustrating percents. In a pie chart, the arc length of each sector (and
//     consequently its central angle and area), is proportional to the quantity
//     it represents. Together, the sectors create a full disk.
//===========================================================================
class _XTP_EXT_CLASS CXTPChart3dPieSeriesStyle : public CXTPChartPieSeriesStyleBase
{
	DECLARE_SERIAL(CXTPChart3dPieSeriesStyle)

public:
	//-----------------------------------------------------------------------
	// Summary:
	//     Constructs a CXTPChart3dPieSeriesStyle object.
	// Remarks:
	//-----------------------------------------------------------------------
	CXTPChart3dPieSeriesStyle();

public:
	//-------------------------------------------------------------------------
	// Summary:
	//     Call this function to create the pie diagram object.
	// Returns:
	//     A pointer to CXTPChartDiagram object which points to a newly created
	//     CXTPChart3dPieDiagramBase object.
	//-------------------------------------------------------------------------
	virtual CXTPChartDiagram* CreateDiagram();

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
	virtual CXTPChartSeriesView* CreateView(CXTPChartSeries* pSeries, CXTPChartDiagramView* pDiagramView);

	//-----------------------------------------------------------------------
	// Summary:
	//      Obtains depth value.
	// Returns:
	//      Depth value
	// See also:
	//      SetDepth
	//-----------------------------------------------------------------------
	virtual int GetDepth() const;

	//-----------------------------------------------------------------------
	// Summary:
	//      Sets depth value.
	// Parameters:
	//      dDepth - New depth value.
	// See also:
	//      GetDepth
	//-----------------------------------------------------------------------
	void SetDepth(int dDepth);
	
	//-----------------------------------------------------------------------
	// Summary:
	//      Obtains rotation parameters for 3D chart.
	// Returns:
	//      Rotation parameters for 3D chart.
	// See also:
	//      Set3dRotation
	//-----------------------------------------------------------------------
	CXTPChart3dRotation Get3dRotation() const;

	//-----------------------------------------------------------------------
	// Summary:
	//      Sets rotation parameters for 3D chart.
	// Parameters:
	//      rotation - Rotation parameters in degrees.
	//      dYaw - Yaw angle value in degrees.
	//      dPitch - Pitch angle value in degrees.
	//      dRoll - Roll angle value in degrees.
	// See also:
	//      Get3dRotation
	//-----------------------------------------------------------------------
	void Set3dRotation(const CXTPChart3dRotation& rotation);
	void Set3dRotation(double dYaw, double dPitch, double dRoll); // <combine CXTPChart3dPieSeriesStyle::Set3dRotation@const CXTPChart3dRotation&>

	//-----------------------------------------------------------------------
	// Summary:
	//      Checks if the chart has to be drawn as a torus instead of a pie.
	// Returns:
	//      TRUE if the chart is to be drawn as a torus.
	// See also:
	//      SetTorus
	//-----------------------------------------------------------------------
	BOOL IsTorus() const;

	//-----------------------------------------------------------------------
	// Summary:
	//      Sets torus appearance of the chart.
	// Parameters:
	//      bTorus - If TRUE then the chart will be drawn as a torus.
	// See also:
	//      IsTorus
	//-----------------------------------------------------------------------
	void SetTorus(BOOL bTorus);

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

#ifdef _XTP_ACTIVEX
public:
	//{{AFX_CODEJOCK_PRIVATE
	DECLARE_DISPATCH_MAP()
	DECLARE_INTERFACE_MAP()
	DECLARE_OLETYPELIB_EX(CXTPChart3dPieSeriesStyle);
	DECLARE_OLECREATE_EX(CXTPChart3dPieSeriesStyle);
	//}}AFX_CODEJOCK_PRIVATE
#endif

private:
	int OleGetDepth();
	void OleSetDepth(int dDepth);
	LPDISPATCH OleGet3dRotation();
	void OleSet3dRotation(LPDISPATCH pRotationDisp);
	BOOL OleGetIsTorus();
	void OleSetTorus(BOOL bTorus);

private:
	CXTPChart3dRotation m_3dRotation;
	int m_nDepth;
	BOOL m_bTorus;
};

AFX_INLINE int CXTPChart3dPieSeriesStyle::GetDepth() const {
	return m_nDepth;
}
AFX_INLINE void CXTPChart3dPieSeriesStyle::SetDepth(int nDepth) {
	m_nDepth = nDepth;
	OnChartChanged();
}
AFX_INLINE void CXTPChart3dPieSeriesStyle::Set3dRotation(const CXTPChart3dRotation& rotation) {
	m_3dRotation = rotation;
	OnChartChanged();
}
AFX_INLINE void CXTPChart3dPieSeriesStyle::Set3dRotation(double dYaw, double dPitch, double dRoll) {
	Set3dRotation(CXTPChart3dRotation(dYaw, dPitch, dRoll));
}
AFX_INLINE BOOL CXTPChart3dPieSeriesStyle::IsTorus() const {
	return m_bTorus;
}
AFX_INLINE void CXTPChart3dPieSeriesStyle::SetTorus(BOOL bTorus) {
	m_bTorus = bTorus;
	OnChartChanged();
}


#include "Common/Base/Diagnostic/XTPEnableNoisyWarnings.h"
#endif //#if !defined(__XTPCHART3DPIESERIESSTYLE_H__)
