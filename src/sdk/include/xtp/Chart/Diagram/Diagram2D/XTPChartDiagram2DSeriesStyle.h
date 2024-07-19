// XTPChartDiagram2DSeriesStyle.h
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
#if !defined(__XTPCHARTDIAGRAM2DSERIESSTYLE_H__)
#define __XTPCHARTDIAGRAM2DSERIESSTYLE_H__
//}}AFX_CODEJOCK_PRIVATE

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "Common/Base/Diagnostic/XTPDisableNoisyWarnings.h"

class CXTPChartSeriesView;
class CXTPChartMarker;

//===========================================================================
// Summary:
//     This class represents a line series style, which is a kind of
//     CXTPChartSeriesStyle.
// Remarks:
//     A point chart is a type of graph, which displays information as a
//     series of data points.It is a basic type of chart common in many
//     fields.
//===========================================================================
class _XTP_EXT_CLASS CXTPChartDiagram2DSeriesStyle : public CXTPChartSeriesStyle
{
	DECLARE_DYNAMIC(CXTPChartDiagram2DSeriesStyle)

public:
	//-----------------------------------------------------------------------
	// Summary:
	//     Constructs a CXTPChartDiagram2DSeriesStyle object.
	// Remarks:
	//-----------------------------------------------------------------------
	CXTPChartDiagram2DSeriesStyle();

	//-------------------------------------------------------------------------
	// Summary:
	//     Destroys a CXTPChartDiagram2DSeriesStyle object, handles cleanup
	//-------------------------------------------------------------------------
	virtual ~CXTPChartDiagram2DSeriesStyle();

public:
	void SetSecondaryAxisX(BOOL bSecondaryAxis);
	BOOL IsSecondaryAxisX() const;

	void SetSecondaryAxisY(BOOL bSecondaryAxis);
	BOOL IsSecondaryAxisY() const;

	void SetAxisIndexX(int nAxisIndexX);
	int GetAxisIndexX() const;

	void SetAxisIndexY(int nAxisIndexY);
	int GetAxisIndexY() const;

public:
	void DoPropExchange(CXTPPropExchange* pPX);

public:
	//-----------------------------------------------------------------------
	// Summary:
	//     Call this function to create the diagram for this series.
	// Returns:
	//     A pointer to CXTPChartDiagram2D object, which is a kind of CXTPChartDiagram.
	// Remarks:
	//-----------------------------------------------------------------------
	virtual CXTPChartDiagram* CreateDiagram();

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this function to check whether chart diagram object passed
	//     is a kind of 2D diagram(CXTPChartDiagram2D object).
	// Parameters:
	//     pDiagram - A pointer to a chart diagram, whose type is to be identified.
	// Returns:
	//     TRUE if the chart diagram is a 2D diagram and FALSE if not.
	// Remarks:
	//-----------------------------------------------------------------------
	virtual BOOL IsStyleDiagram(CXTPChartDiagram* pDiagram) const;


#ifdef _XTP_ACTIVEX
public:
//{{AFX_CODEJOCK_PRIVATE
	DECLARE_DISPATCH_MAP()
	DECLARE_INTERFACE_MAP()
	DECLARE_OLETYPELIB_EX(CXTPChartDiagram2DSeriesStyle);

//}}AFX_CODEJOCK_PRIVATE
#endif

protected:
	BOOL m_bSecondaryAxisX;
	BOOL m_bSecondaryAxisY;
	int m_nAxisIndexX;			// Holds index of secondary axisX, used when m_bSecondaryAxisX is TRUE.
	int m_nAxisIndexY;			// Holds index of secondary axisY, used when m_bSecondaryAxisY is TRUE.
};

AFX_INLINE void CXTPChartDiagram2DSeriesStyle::SetSecondaryAxisX(BOOL bSecondaryAxis) {
	if (m_bSecondaryAxisX != bSecondaryAxis)
	{
		m_bSecondaryAxisX = bSecondaryAxis;
		OnChartChanged();
	}
}
AFX_INLINE BOOL CXTPChartDiagram2DSeriesStyle::IsSecondaryAxisX() const {
	return m_bSecondaryAxisX;
}

AFX_INLINE void CXTPChartDiagram2DSeriesStyle::SetSecondaryAxisY(BOOL bSecondaryAxis) {
	if (m_bSecondaryAxisY != bSecondaryAxis)
	{
		m_bSecondaryAxisY = bSecondaryAxis;
		OnChartChanged();
	}
}
AFX_INLINE BOOL CXTPChartDiagram2DSeriesStyle::IsSecondaryAxisY() const {
	return m_bSecondaryAxisY;
}

AFX_INLINE void CXTPChartDiagram2DSeriesStyle::SetAxisIndexX(int nAxisIndexX) {
	if (nAxisIndexX > 0 && nAxisIndexX != m_nAxisIndexX)
	{
		m_nAxisIndexX = nAxisIndexX;
		OnChartChanged();
	}
}
AFX_INLINE int CXTPChartDiagram2DSeriesStyle::GetAxisIndexX() const {
	if (m_bSecondaryAxisX == FALSE)
		return 0;
	
	return m_nAxisIndexX;
}

AFX_INLINE void CXTPChartDiagram2DSeriesStyle::SetAxisIndexY(int nAxisIndexY) {
	if (nAxisIndexY > 0 && nAxisIndexY != m_nAxisIndexY)
	{
		m_nAxisIndexY = nAxisIndexY;
		OnChartChanged();
	}
}
AFX_INLINE int CXTPChartDiagram2DSeriesStyle::GetAxisIndexY() const {
	if (m_bSecondaryAxisY == FALSE)
		return 0;

	return m_nAxisIndexY;
}





#include "Common/Base/Diagnostic/XTPEnableNoisyWarnings.h"
#endif //#if !defined(__XTPCHARTDIAGRAM2DSERIESSTYLE_H__)
