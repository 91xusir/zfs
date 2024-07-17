// XTPChartDiagram.h
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
#if !defined(__XTPCHARTDIAGRAM_H__)
#define __XTPCHARTDIAGRAM_H__
//}}AFX_CODEJOCK_PRIVATE

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "Common/Base/Diagnostic/XTPDisableNoisyWarnings.h"


class CXTPChartSeriesView;
class CXTPChartElementView;
class CXTPChartDiagramView;
class CXTPChartSeries;
class CXTPArcBall;
template<class T> class CXTPMatrix;

typedef CArray<CXTPChartSeries*, CXTPChartSeries*> CXTPChartSeriesArray;



//===========================================================================
// Summary:
//     This class represents a chart diagram, which is a kind of CXTPChartElement.
// Remarks:
//===========================================================================
class _XTP_EXT_CLASS CXTPChartDiagram : public CXTPChartPanel
{
	DECLARE_DYNAMIC(CXTPChartDiagram)

protected:
	//-----------------------------------------------------------------------
	// Summary:
	//     Constructs a CXTPChartDiagram object.
	// Remarks:
	//-----------------------------------------------------------------------
	CXTPChartDiagram();

public:
	//-----------------------------------------------------------------------
	// Summary:
	//     Call this function to create view of the diagram.
	// Parameters:
	//     pDC      - The chart device context.
	//     pParent  - A pointer to the parent view object.
	// Returns:
	//     A pointer to CXTPChartDiagramView object.
	// Remarks:
	//     This is a virtual function, so the sub classes can give their type
	//     specific implementation for this function.
	//-----------------------------------------------------------------------
	virtual CXTPChartDiagramView* CreateView(CXTPChartDeviceContext* pDC, CXTPChartElementView* pParent);

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this function to calculate series layout of the diagram.
	// Parameters:
	//     pDC      - The chart device context.
	//     pView  - A pointer to the diagram view object.
	// Remarks:
	//     This is a virtual function, so the sub classes can give their type
	//     specific implementation for this function.
	//-----------------------------------------------------------------------
	virtual void CalculateSeriesLayout(CXTPChartDeviceContext* pDC, CXTPChartDiagramView* pView);

public:

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this function to get the series collection object.
	// Returns:
	//     A reference to CXTPChartSeriesArray object.
	// Remarks:
	//-----------------------------------------------------------------------
	const CXTPChartSeriesArray& GetSeries() const;

protected:
	//-----------------------------------------------------------------------
	// Summary:
	//     This function called when a series is added to the series collection
	//     in the chart content object which has a collection of series.
	// Parameters:
	//     pSeries - A pointer to chart series object.
	// Remarks:
	//-----------------------------------------------------------------------
	virtual BOOL OnSeriesAdded(CXTPChartSeries* pSeries);

	//-----------------------------------------------------------------------
	// Summary:
	//     This function called when a series is removed from the series collection
	//     in the chart content object which has a collection of series.
	// Parameters:
	//     pSeries - A pointer to chart series object.
	// Remarks:
	//-----------------------------------------------------------------------
	void OnSeriesRemoved(CXTPChartSeries* pSeries);

protected:
	//-------------------------------------------------------------------------
	// Summary:
	//     Call this function to decrease the usage count of the object.
	//-------------------------------------------------------------------------
	void Release();

public:
	//-----------------------------------------------------------------------
	// Summary:
	//     Reads or writes this object from or to an archive.
	// Parameters:
	//     pPX - A CXTPPropExchange object to serialize to or from.
	//----------------------------------------------------------------------
	virtual void DoPropExchange(CXTPPropExchange* pPX);

public:

	friend class CXTPChartContent;
	friend class CXTPChartSeries;


#ifdef _XTP_ACTIVEX
public:
//{{AFX_CODEJOCK_PRIVATE
	DECLARE_DISPATCH_MAP()
	DECLARE_INTERFACE_MAP()
	DECLARE_OLETYPELIB_EX(CXTPChartDiagram);
	LPDISPATCH OleGetTitles();
//}}AFX_CODEJOCK_PRIVATE
#endif

protected:
	CXTPChartSeriesArray m_arrSeries;        //The series collection.
};

//-----------------------------------------------------------------------
// Summary:
//      Represents a virtual diagram rectangular domain area.
//-----------------------------------------------------------------------
class _XTP_EXT_CLASS CXTPChartDiagramDomain
{
public:
	//-----------------------------------------------------------------------
	// Summary:
	//      Constructs diagram domain.
	// Parameters:
	//      rcInnerBounds - Rectangle value that defines domain area.
	//-----------------------------------------------------------------------
	CXTPChartDiagramDomain(const CXTPChartRectF& rcInnerBounds);

	//-----------------------------------------------------------------------
	// Summary:
	//      Determines domain area boundary rectangle.
	// Returns:
	//      Domain area boundary rectangle
	//-----------------------------------------------------------------------
	const CXTPChartRectF& GetInnerBounds() const;

private:
	const CXTPChartRectF m_rcInnerBounds;
};

AFX_INLINE CXTPChartDiagramDomain::CXTPChartDiagramDomain(const CXTPChartRectF& rcInnerBounds)
	: m_rcInnerBounds(rcInnerBounds) { }

AFX_INLINE const CXTPChartRectF& CXTPChartDiagramDomain::GetInnerBounds() const {
	return m_rcInnerBounds;
}

AFX_INLINE const CXTPChartSeriesArray& CXTPChartDiagram::GetSeries() const {
	return m_arrSeries;
}

//===========================================================================
// Summary:
//     The base class for all 2D diagrams.
// Remarks:
//===========================================================================
class _XTP_EXT_CLASS CXTPChart2dDiagram : public CXTPChartDiagram
{
	DECLARE_DYNAMIC(CXTPChart2dDiagram)

protected:
	//-----------------------------------------------------------------------
	// Summary:
	//     Constructs a CXTPChartDiagram2D object.
	// Remarks:
	//-----------------------------------------------------------------------
	CXTPChart2dDiagram();
};

//===========================================================================
// Summary:
//     The base class for all 3D diagrams.
// Remarks:
//===========================================================================
class _XTP_EXT_CLASS CXTPChart3dDiagram : public CXTPChartDiagram
{
	DECLARE_DYNAMIC(CXTPChart3dDiagram)

protected:
	//-----------------------------------------------------------------------
	// Summary:
	//     Constructs a CXTPChart3dDiagram object.
	// Remarks:
	//-----------------------------------------------------------------------
	CXTPChart3dDiagram();

	//-----------------------------------------------------------------------
	// Summary:
	//      Handles CXTPChart3dDiagram object destruction.
	//-----------------------------------------------------------------------
	~CXTPChart3dDiagram();

public:
	// The side of the box in 3D space whithin which a chart is drawn.
	static const float m_fChartBoxSize;
	// The size of a primitive, the smallest part of a chart, e.g. triangle side size.
	static const int m_nPrimitiveSize;
	// The closest possible distance to the center of a model.
	static const float m_fMinModelDistance;
	// The farthest possible distance to the center of a model.
	static const float m_fMaxModelDistance;

	//-----------------------------------------------------------------------
	// Summary:
	//      Obtains camera coordinates in 3D space. The camera is always directed
	//      to the center of the model.
	// Returns:
	//      Camera coordinates in 3D space.
	//-----------------------------------------------------------------------
	const CXTPPoint3f& GetCameraPosition() const;

	//-----------------------------------------------------------------------
	// Summary:
	//      Obtains view rotation matrix. The matrix is used for rotating view
	//      and not the model itself.
	// Returns:
	//      View rotation matrix.
	//-----------------------------------------------------------------------
	const CXTPMatrix<double>& GetViewRotationMatrix() const;

	//-----------------------------------------------------------------------
	// Summary:
	//      Sets boundaries for arc ball used for rotating the view.
	// Parameters:
	//      dWidth - Available width value.
	//      dHeight - Available height value.
	//-----------------------------------------------------------------------
	void SetArcBallBounds(double dWidth, double dHeight);

	//-----------------------------------------------------------------------
	// Summary:
	//      Sets start arc ball rotation point.
	// Parameters:
	//      point - Specifies the relative x- and y- coordinates of 
	//              the start arc ball rotation point.
	// Returns:
	//      TRUE is the start rotation point is set, otherwise the return value FALSE.
	//-----------------------------------------------------------------------
	BOOL SetStartArcBallRotationPoint(POINT point);

	//-----------------------------------------------------------------------
	// Summary:
	//      Rotates arc ball to the specified point.
	// Parameters:
	//      point - Specifies the relative x- and y- coordinates of 
	//              the end rotation point.
	// Returns:
	//      TRUE is the rotation is completed, otherwise the return value FALSE.
	//-----------------------------------------------------------------------
	BOOL RotateArcBallToPoint(POINT point);

	//-----------------------------------------------------------------------
	// Summary:
	//      Zooms arc ball in or out.
	// Parameters:
	//      fDelta - The zooming distance. If positive arc ball is zoomed in.
	//               Nevative value zooms arc ball out.
	//-----------------------------------------------------------------------
	void ZoomArcBall(float fDelta);

private:
	CXTPPoint3f m_ptCameraPosition;
	CXTPArcBall* m_pArcBall;
	CXTPMatrix<double>* m_pMatrixBeforeRotation;
	CXTPMatrix<double>* m_pMatrixAfterRotation;
};

AFX_INLINE const CXTPPoint3f& CXTPChart3dDiagram::GetCameraPosition() const {
	return m_ptCameraPosition;
}

AFX_INLINE const CXTPMatrix<double>& CXTPChart3dDiagram::GetViewRotationMatrix() const {
	return *m_pMatrixAfterRotation;
}


#include "Common/Base/Diagnostic/XTPEnableNoisyWarnings.h"
#endif //#if !defined(__XTPCHARTDIAGRAM_H__)
