// XTPChartPieSeriesLabelBase.h
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
#if !defined(__XTPCHARTPIESERIESLABELBASE_H__)
#define __XTPCHARTPIESERIESLABELBASE_H__
//}}AFX_CODEJOCK_PRIVATE

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "Common/Base/Diagnostic/XTPDisableNoisyWarnings.h"

class CXTPChartSeriesView;
class CXTPChartPieSeriesPointViewBase;
class CXTPChartPieSeriesStyleBase;
class CXTPChartPieSeriesLabelViewBase;

//===========================================================================
// Summary:
//     This enumeration tell the various positions available for the pie label.
// Remarks:
//===========================================================================
enum XTPChartPieLabelPosition
{
	xtpChartPieLabelOutside,    // Outside the pie.
	xtpChartPieLabelInside,     // Inside the pie.
	xtpChartPieLabelRadial,     // Radial label.
	xtpChartPieLabelTwoColumns  // 2 Columns
};

//===========================================================================
// Summary:
//     CXTPChartPieSeriesLabelBase is a kind of CXTPChartSeriesLabel, this class
//     abstracts the label of a pie series.
// Remarks:
//
//===========================================================================
class _XTP_EXT_CLASS CXTPChartPieSeriesLabelBase : public CXTPChartSeriesLabel
{
	DECLARE_DYNAMIC(CXTPChartPieSeriesLabelBase);

public:
	//-----------------------------------------------------------------------
	// Summary:
	//     Constructs a CXTPChartPieSeriesLabelBase object.
	// Remarks:
	//-----------------------------------------------------------------------
	CXTPChartPieSeriesLabelBase();

	//-------------------------------------------------------------------------
	// Summary:
	//     Destroys a CXTPChartPieSeriesLabelBase object, handles cleanup
	//-------------------------------------------------------------------------
	virtual ~CXTPChartPieSeriesLabelBase();

public:
	//-------------------------------------------------------------------------
	// Summary:
	//     Call this function to get the label position.
	// Returns:
	//     A pie label position object describing the position of pie label.
	//-------------------------------------------------------------------------
	XTPChartPieLabelPosition GetPosition() const;

	//-------------------------------------------------------------------------
	// Summary:
	//     Call this function to set the label position.
	// Parameters:
	//     nPosition - A pie label position object describing the position of
	//                 pie label.
	//-------------------------------------------------------------------------
	void SetPosition(XTPChartPieLabelPosition nPosition);

	//-------------------------------------------------------------------------
	// Summary:
	//     Call this function to get the basic style of the pie series.
	// Returns:
	//     A pointer to CXTPChartPieSeriesStyleBase object.
	//-------------------------------------------------------------------------
	CXTPChartPieSeriesStyleBase* GetStyle() const;

public:
	void DoPropExchange(CXTPPropExchange* pPX);
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
		CXTPChartSeriesPointView* pPointView, CXTPChartElementView* pParentView) = 0;

public:
	//-------------------------------------------------------------------------
	// Summary:
	//     Call this function to check whether the label is inside the pie.
	// Returns:
	//     Returns TRUE if the label is inside the pie and FALSE if not.
	// Remarks:
	// See Also:
	//-------------------------------------------------------------------------
	BOOL IsInside() const;

protected:
	XTPChartPieLabelPosition m_nPosition;    //The pie label position.

#ifdef _XTP_ACTIVEX
public:
	//{{AFX_CODEJOCK_PRIVATE
	DECLARE_DISPATCH_MAP()
	DECLARE_INTERFACE_MAP()
	DECLARE_OLETYPELIB_EX(CXTPChartPieSeriesLabelBase);
	//}}AFX_CODEJOCK_PRIVATE
#endif
};

//-----------------------------------------------------------------------
// Summary:
//      Defines a base class interface for series label view.
//-----------------------------------------------------------------------
class CXTPChartPieSeriesLabelViewBase : public CXTPChartSeriesLabelView
{
	DECLARE_DYNAMIC(CXTPChartPieSeriesLabelViewBase);

protected:
	//-----------------------------------------------------------------------
	// Summary:
	//      Constructs CXTPChartPieSeriesLabelViewBase object.
	// Parameters:
	//      pLabel - Associated label model pointer.
	//      pPointView - Point view object pointer associated with the label.
	//      pParentView - Parent view pointer.
	//-----------------------------------------------------------------------
	CXTPChartPieSeriesLabelViewBase(CXTPChartSeriesLabel* pLabel, CXTPChartSeriesPointView* pPointView, CXTPChartElementView* pParentView);

public:
	//-----------------------------------------------------------------------
	// Summary:
	//      Obtains label position.
	// Returns:
	//      Label position.
	//-----------------------------------------------------------------------
	XTPChartPieLabelPosition GetPosition() const;

	//-----------------------------------------------------------------------
	// Summary:
	//      Creates view drawing device command for the device context specified.
	// Parameters:
	//      pDC - Device context pointer for which the drawing command is to be created.
	// Returns:
	//      A new instance of view drawing device command or NULL if no command
	//      is provided.
	//-----------------------------------------------------------------------
	virtual CXTPChartDeviceCommand* CreateDeviceCommand(CXTPChartDeviceContext* pDC) = 0;

	//-----------------------------------------------------------------------
	// Summary:
	//      Creates connector drawing device command for the device context specified.
	// Parameters:
	//      pDC - Device context pointer for which the drawing command is to be created.
	// Returns:
	//      A new instance of connector drawing device command or NULL if no command
	//      is provided.
	//-----------------------------------------------------------------------
	virtual CXTPChartDeviceCommand* CreateConnectorDeviceCommand(CXTPChartDeviceContext* pDC) = 0;
};

AFX_INLINE XTPChartPieLabelPosition CXTPChartPieSeriesLabelBase::GetPosition() const {
	return m_nPosition;
}
AFX_INLINE void CXTPChartPieSeriesLabelBase::SetPosition(XTPChartPieLabelPosition nPosition) {
	m_nPosition = nPosition;
	OnChartChanged();
}

//-----------------------------------------------------------------------
// Summary:
//      Defines a base class interface for series label collection view.
//-----------------------------------------------------------------------
class _XTP_EXT_CLASS CXTPChartPieSeriesLabelsViewBase : public CXTPChartElementView
{
	DECLARE_DYNAMIC(CXTPChartPieSeriesLabelsViewBase);

protected:
	//-----------------------------------------------------------------------
	// Summary:
	//      Constructs CXTPChartPieSeriesLabelsViewBase object.
	// Parameters:
	//      pParentView - Parent view pointer.
	//-----------------------------------------------------------------------
	CXTPChartPieSeriesLabelsViewBase(CXTPChartElementView* pParentView);

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
	virtual CXTPChartDeviceCommand* CreateDeviceCommand(CXTPChartDeviceContext* pDC) = 0;

	//-----------------------------------------------------------------------
	// Summary:
	//      Resolves label overlapping in implementation specific way.
	//-----------------------------------------------------------------------
	virtual void ResolveOverlapping() = 0;
};


#include "Common/Base/Diagnostic/XTPEnableNoisyWarnings.h"
#endif //#if !defined(__XTPCHARTPIESERIESLABELBASE_H__)
