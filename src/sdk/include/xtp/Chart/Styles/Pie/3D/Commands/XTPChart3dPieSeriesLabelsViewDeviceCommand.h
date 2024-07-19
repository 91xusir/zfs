// XTPChart3dPieSeriesLabelsViewDeviceCommand.h
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
#if !defined(__XTPCHART3DPIESERIESLABELSVIEWDEVICECOMMAND_H__)
#define __XTPCHART3DPIESERIESLABELSVIEWDEVICECOMMAND_H__
//}}AFX_CODEJOCK_PRIVATE

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "Common/Base/Diagnostic/XTPDisableNoisyWarnings.h"

class CXTPChart3dPieSeriesLabelsView;
class CXTPChart3dPieSeriesLabelView;
class CXTPChart3dPieSeriesPointView;
class CXTPChart3dPieSeriesLabelDeviceCommand;
class CXTPChart3dDeviceContext;
class CXTPChart3dPieSeriesLabel;

//-----------------------------------------------------------------------
// Summary:
//      Implements 3D chart pie series label device command.
//-----------------------------------------------------------------------
class _XTP_EXT_CLASS CXTPChart3dPieSeriesLabelDeviceCommand : public CXTPChart3dDeviceCommand
{
	DECLARE_DYNAMIC(CXTPChart3dPieSeriesLabelDeviceCommand);

public:
	//-----------------------------------------------------------------------
	// Summary:
	//      Constructs CXTPChart3dPieSeriesLabelDeviceCommand object.
	// Parameters:
	//      pSeriesLabelView - Associated series label view pointer.
	//-----------------------------------------------------------------------
	CXTPChart3dPieSeriesLabelDeviceCommand(CXTPChart3dPieSeriesLabelView* pSeriesLabelView);

public:
	//{{AFX_CODEJOCK_PRIVATE
	virtual void ExecuteOverride(CXTPChartDeviceContext* pDC);
	//{{AFX_CODEJOCK_PRIVATE

	//-----------------------------------------------------------------------
	// Summary:
	//      Obtains label view object pointer.
	// Returns:
	//      Label view object pointer
	//-----------------------------------------------------------------------
	CXTPChart3dPieSeriesLabelView* GetSeriesLabelView() const;

	//-----------------------------------------------------------------------
	// Summary:
	//      Returns TRUE if label connector is bent.
	// Returns:
	//      TRUE if label connector is bent.
	//-----------------------------------------------------------------------
	BOOL IsBent() const;

	//-----------------------------------------------------------------------
	// Summary:
	//      Checks whether command object is in a failed state.
	// Returns:
	//      TRUE if command object is in a failed state.
	// See also:
	//      SetFailed
	//-----------------------------------------------------------------------
	BOOL IsFailed() const;

	//-----------------------------------------------------------------------
	// Summary:
	//      Sets command object to a failed state.
	//-----------------------------------------------------------------------
	void SetFailed();

	//-----------------------------------------------------------------------
	// Summary:
	//      Obtains label rectangle.
	// Returns:
	//      Label rectangle.
	// See also:
	//      SetLabelRect
	//-----------------------------------------------------------------------
	CRect GetLabelRect() const;

	//-----------------------------------------------------------------------
	// Summary:
	//      Sets label rectangle.
	// Parameters:
	//      rcLabel - Label rectangle.
	// See also:
	//      GetLabelRect
	//-----------------------------------------------------------------------
	void SetLabelRect(CRect rcLabel);

	//-----------------------------------------------------------------------
	// Summary:
	//      Computes connector vector.
	// Parameters:
	//      pDC - Target device context pointer.
	// Returns:
	//      TRUE if the vector is successfully computed.
	//-----------------------------------------------------------------------
	BOOL ComputeConnectorVector(CXTPChart3dDeviceContext* pDC);

	//-----------------------------------------------------------------------
	// Summary:
	//      Computes connector bend.
	// Parameters:
	//      pDC - Target device context pointer.
	// Returns:
	//      TRUE if the bend is successfully computed.
	//-----------------------------------------------------------------------
	BOOL ComputeConnectorBend(CXTPChart3dDeviceContext* pDC, CRect rcBoundaries);

	//-----------------------------------------------------------------------
	// Summary:
	//      Computes label.
	// Parameters:
	//      pDC - Target device context pointer.
	//-----------------------------------------------------------------------
	void ComputeLabel(CXTPChart3dDeviceContext* pDC);

	//-----------------------------------------------------------------------
	// Summary:
	//      Determines connector length factor value.
	// Returns:
	//      Connector length factor value.
	// See also:
	//      SetConnectorLengthFactor
	//-----------------------------------------------------------------------
	double GetConnectorLengthFactor() const;

	//-----------------------------------------------------------------------
	// Summary:
	//      Sets connector length factor value.
	// Parameters:
	//      dFactor - New connector length factor value.
	// See also:
	//      GetConnectorLengthFactor
	//-----------------------------------------------------------------------
	void SetConnectorLengthFactor(double dFactor);

	//-----------------------------------------------------------------------
	// Summary:
	//      Determines connector endpoint vertical offset value.
	// Returns:
	//      Connector endpoint vertical offset value.
	// See also:
	//      SetConnectorEndpointVerticalOffset
	//-----------------------------------------------------------------------
	double GetConnectorEndpointVerticalOffset() const;

	//-----------------------------------------------------------------------
	// Summary:
	//      Sets connector endpoint vertical offset value.
	// Parameters:
	//      yOffset - New connector endpoint vertical offset value.
	// See also:
	//      GetConnectorEndpointVerticalOffset
	//-----------------------------------------------------------------------
	void SetConnectorEndpointVerticalOffset(double yOffset);

	//-----------------------------------------------------------------------
	// Summary:
	//      Sets column anchor vertical position value.
	// Parameters:
	//      pDC - Target device context.
	//      y - New column anchor vertical position value.
	// Returns:
	//      TRUE if the new value is successfully computed and applied.
	//-----------------------------------------------------------------------
	BOOL SetColumnAnchorVerticalPosition(CXTPChart3dDeviceContext* pDC, int y);

private:
	BOOL ComputeConnectorStartPoint(CXTPChart3dDeviceContext* pDC, 
		CXTPPoint3d& ptStart, CPoint& ptProjected) const;
	CXTPPoint3d ComputeConnectorBendPoint() const;
	CXTPPoint3d ComputeConnectorEndPoint() const;
	CXTPPoint3d ComputeConnectorVectorEndPoint(const CXTPPoint3d& point) const;
	void ComputeLabel_TwoColumn(const CSize& szLabel, const CPoint& ptAnchor);
	void ComputeLabel_Outside(const CSize& szLabel,
		const CPoint& ptAnchorOrigin, const CPoint& ptAnchor);
	void ComputeLabel_InsideAndRadial(const CSize& szLabel, const CPoint& ptAnchor);

protected:
	CXTPChart3dPieSeriesLabelView* m_pSeriesLabelView;
	CXTPChart3dPieSeriesPointView* m_pPointView;
	CRect m_rcLabel;
	BOOL m_bFailed;
	double m_dConnectorLengthFactor;
	double m_dConnectorEndpointVerticalOffset;
};

AFX_INLINE BOOL CXTPChart3dPieSeriesLabelDeviceCommand::IsFailed() const {
	return m_bFailed;
}
AFX_INLINE void CXTPChart3dPieSeriesLabelDeviceCommand::SetFailed() {
	m_bFailed = TRUE;
}
AFX_INLINE CXTPChart3dPieSeriesLabelView* CXTPChart3dPieSeriesLabelDeviceCommand::GetSeriesLabelView() const {
	return m_pSeriesLabelView;
}
AFX_INLINE CRect CXTPChart3dPieSeriesLabelDeviceCommand::GetLabelRect() const {
	return m_rcLabel;
}
AFX_INLINE void CXTPChart3dPieSeriesLabelDeviceCommand::SetLabelRect(CRect rcLabel) {
	m_rcLabel = rcLabel;
}
AFX_INLINE double CXTPChart3dPieSeriesLabelDeviceCommand::GetConnectorLengthFactor() const {
	return m_dConnectorLengthFactor;
}
AFX_INLINE void CXTPChart3dPieSeriesLabelDeviceCommand::SetConnectorLengthFactor(double dFactor) {
	m_dConnectorLengthFactor = dFactor;
}
AFX_INLINE double CXTPChart3dPieSeriesLabelDeviceCommand::GetConnectorEndpointVerticalOffset() const {
	return m_dConnectorEndpointVerticalOffset;
}
AFX_INLINE void CXTPChart3dPieSeriesLabelDeviceCommand::SetConnectorEndpointVerticalOffset(double yOffset) {
	m_dConnectorEndpointVerticalOffset = yOffset;
}

//-----------------------------------------------------------------------
// Summary:
//      Represents device command for all pie series labels view.
//-----------------------------------------------------------------------
class _XTP_EXT_CLASS CXTPChart3dPieSeriesLabelsViewDeviceCommand 
	: public CXTPChart3dDeviceCommand
	, private CXTPChartDeviceCommandEvents
{
public:
	//-----------------------------------------------------------------------
	// Summary:
	//      Constructs command object instance.
	// Parameters:
	//      pSeriesLabelsView - View pointer.
	//-----------------------------------------------------------------------
	CXTPChart3dPieSeriesLabelsViewDeviceCommand(CXTPChart3dPieSeriesLabelsView* pSeriesLabelsView);

public:
	//-----------------------------------------------------------------------
	// Summary:
	//     Override this function to write the drawing code for specific objects.
	// Parameters:
	//     pDC - A pointer to the chart device context.
	//-----------------------------------------------------------------------
	virtual void ExecuteOverride(CXTPChartDeviceContext* pDC);

private:
	CXTPChart3dPieSeriesLabel* GetLabel() const;
	void ComputeLabelConnectorVectors(CXTPChart3dDeviceContext* pDC, BOOL& bNeedComputeBends);
	BOOL ComputeBendBoundaries(CXTPChart3dDeviceContext* pDC, CRect& rcBounaries);
	void ComputeLabelConnectorBends(CXTPChart3dDeviceContext* pDC, CRect rcBoundaries);
	void ComputeLabels(CXTPChart3dDeviceContext* pDC);
	BOOL ResolveLabelOverlaps(CXTPChart3dDeviceContext* pDC, const CRect& rcBoundaries);
	BOOL ResolveLabelOverlaps_ChangePositionInColumn(CXTPChart3dDeviceContext* pDC,
		BOOL bLeftDirected, const CRect& rcBoundaries);
	BOOL ResolveLabelOverlaps_ChangeConnectorLength(CXTPChart3dDeviceContext* pDC);

	CXTPChart3dPieSeriesLabelDeviceCommand* FindLabelDeviceCommand(CXTPChartDeviceCommand* pCommand) const;

private:
	virtual void OnAfterExecute(CXTPChartDeviceCommand* pCommand, CXTPChartDeviceContext* pDC);

	struct CommandCallbackData
	{
		enum State { None, ComputeVector, ComputeBend, 
			ComputeBendConnector, ColumnAnchorVericalPosition };

		CommandCallbackData(State state = None);
		void SwitchTo(State state);
		State GetState() const;

		BOOL bSuccess; // for all
		CXTPChart3dDeviceContext* pDC; // for ComputeVector, ComputeBend, ComputeBendConnector, ColumnAnchorVericalPosition
		CXTPChart3dPieSeriesLabelDeviceCommand* pLabelCommand; // for ComputeVector, ComputeBendConnector
		const CXTPPoint3d* ptBend; // for ComputeBend
		LPRECT prcBoundaries; // for ComputeBend, ComputeBendConnector
		int nNewYPos; // for ColumnAnchorVericalPosition

	private:
		State state;
	};

	CommandCallbackData m_CommandCallbackData;

protected:
	CXTPChart3dPieSeriesLabelsView* m_pSeriesLabelsView; // View pointer
};

AFX_INLINE void CXTPChart3dPieSeriesLabelsViewDeviceCommand::CommandCallbackData::SwitchTo(State newState) {
	*this = CommandCallbackData(newState);
}

AFX_INLINE CXTPChart3dPieSeriesLabelsViewDeviceCommand::CommandCallbackData::State
CXTPChart3dPieSeriesLabelsViewDeviceCommand::CommandCallbackData::GetState() const {
	return state;
}

AFX_INLINE CXTPChart3dPieSeriesLabelsViewDeviceCommand::CommandCallbackData::CommandCallbackData(State state)
	: state(state)
	, bSuccess(FALSE)
	, ptBend(NULL)
	, prcBoundaries(NULL)
	, pLabelCommand(NULL)
	, pDC(NULL)
{
}


#include "Common/Base/Diagnostic/XTPEnableNoisyWarnings.h"
#endif //#if !defined(__XTPCHART3DPIESERIESLABELSVIEWDEVICECOMMAND_H__)
