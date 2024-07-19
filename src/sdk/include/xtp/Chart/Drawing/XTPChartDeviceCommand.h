// XTPChartDeviceCommand.h
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
#if !defined(__XTPCHARTDEVICECOMMAND_H__)
#define __XTPCHARTDEVICECOMMAND_H__
//}}AFX_CODEJOCK_PRIVATE

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "Common/Base/Diagnostic/XTPDisableNoisyWarnings.h"

class CXTPChartDeviceContext;
class CXTPChartFont;
class CXTPChartElement;
class CXTPChartSeriesStyle;
class CXTPChartElementView;

//-----------------------------------------------------------------------
// Summary:
//      Defines a callback interface for chart device command event sink.
// See also:
//      CXTPChartDeviceCommand
//-----------------------------------------------------------------------
class _XTP_EXT_CLASS CXTPChartDeviceCommandEvents
	: public CXTPTypeIdProvider<CXTPChartDeviceCommandEvents>
{
protected:
	//-----------------------------------------------------------------------
	// Summary:
	//      Constructs event sink.
	//-----------------------------------------------------------------------
	CXTPChartDeviceCommandEvents();

public:

	//-----------------------------------------------------------------------
	// Summary:
	//      Notifies that a command enters BeforeExecute step.
	// Parameters:
	//      pCommand - Sender command pointer.
	//      pDC - Device context pointer for which a command is being executed.
	//-----------------------------------------------------------------------
	virtual void OnBeforeExecute(CXTPChartDeviceCommand* pCommand, CXTPChartDeviceContext* pDC);

	//-----------------------------------------------------------------------
	// Summary:
	//      Notifies that a command enters AfterExecute step.
	// Parameters:
	//      pCommand - Sender command pointer.
	//      pDC - Device context pointer for which a command has been executed.
	//-----------------------------------------------------------------------
	virtual void OnAfterExecute(CXTPChartDeviceCommand* pCommand, CXTPChartDeviceContext* pDC);
	
	//-----------------------------------------------------------------------
	// Summary:
	//      Notifies about updated boundaries of the command result.
	// Parameters:
	//      pCommand - Sender command pointer.
	//      rcBounds - Updated bounray rectangle.
	//-----------------------------------------------------------------------
	virtual void OnUpdateBounds(CXTPChartDeviceCommand* pCommand, CRect rcBounds);
	
	//-----------------------------------------------------------------------
	// Summary:
	//      Invoked when event sink is to be unsubscribed from command events.
	// Parameters:
	//      pCommand - Sender command pointer.
	//-----------------------------------------------------------------------
	void OnUnsubscribe(CXTPChartDeviceCommand* pCommand);
};

//===========================================================================
// Summary:
//     Defines a common implementation interface for chart drawing commands.
//===========================================================================
class _XTP_EXT_CLASS CXTPChartDeviceCommand 
	: public CXTPChartObject
	, public CXTPObservable<CXTPChartDeviceCommand, CXTPChartDeviceCommandEvents>
{
	DECLARE_DYNAMIC(CXTPChartDeviceCommand);

public:

	//-----------------------------------------------------------------------
	// Summary:
	//     Constructs a CXTPChartDeviceCommand object.
	//-----------------------------------------------------------------------
	CXTPChartDeviceCommand();

	//-------------------------------------------------------------------------
	// Summary:
	//     Destroys a CXTPChartDeviceCommand object, handles cleanup
	//-------------------------------------------------------------------------
	virtual ~CXTPChartDeviceCommand();

public:
	//-----------------------------------------------------------------------
	// Summary:
	//      Determines if the execution of the command has been committed.
	// Returns:
	//      TRUE if the execution of the command has been committed.
	// See also:
	//      Commit
	//-----------------------------------------------------------------------
	BOOL IsCommitted() const;

	//-----------------------------------------------------------------------
	// Summary:
	//      Commits the execution of the command. A command cannot be modified
	//      after being committed.
	// See also:
	//      IsCommitted
	//-----------------------------------------------------------------------
	void Commit();

	//-----------------------------------------------------------------------
	// Summary:
	//      Obtains the number of immediate children commands.
	// Returns:
	//      The number of immediate children commands.
	// See also:
	//      GetChildCommand
	//-----------------------------------------------------------------------
	int GetChildCommandCount() const;

	//-----------------------------------------------------------------------
	// Summary:
	//      Obtains immediate child command point by index.
	// Parameters:
	//      nIndex - Child command index. Must not exceed value returned by
	//               GetChildCommandCount call.
	// Returns:
	//      Child command pointer or NULL if no child command is defined
	//      at the specified location.
	// See also:
	//      GetChildCommandCount
	//-----------------------------------------------------------------------
	CXTPChartDeviceCommand* GetChildCommand(int nIndex) const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this function to add a child device command.
	// Parameters:
	//     pCommand - A pointer to the child device command.
	// Remarks:
	//     A device command object keeps an array of child device commands, an
	//     instruction to execute the drawing of the parent object trigger the
	//     drawing of children as well.
	//-----------------------------------------------------------------------
	CXTPChartDeviceCommand* AddChildCommand(CXTPChartDeviceCommand* pCommand);

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this function to execute the drawing.
	// Parameters:
	//     pDC - A pointer to the chart device context.
	// Remarks:
	//     An instruction to execute the drawing of the parent object trigger the
	//     drawing of children as well.
	//-----------------------------------------------------------------------
	virtual void Execute(CXTPChartDeviceContext* pDC);

	//-----------------------------------------------------------------------
	// Summary:
	//     Override this function to do some ground works if any, before the drawing.
	// Parameters:
	//     pDC - A pointer to the chart device context.
	//-----------------------------------------------------------------------
	virtual void BeforeExecute(CXTPChartDeviceContext* pDC);

	//-----------------------------------------------------------------------
	// Summary:
	//     Override this function to do some final cut if any, after the drawing.
	// Parameters:
	//     pDC - A pointer to the chart device context.
	//-----------------------------------------------------------------------
	virtual void AfterExecute(CXTPChartDeviceContext* pDC);

	//-----------------------------------------------------------------------
	// Summary:
	//     Override this function to write the drawing code for specific objects.
	// Parameters:
	//     pDC - A pointer to the chart device context.
	//-----------------------------------------------------------------------
	virtual void ExecuteOverride(CXTPChartDeviceContext* pDC);

	//-----------------------------------------------------------------------
	// Summary:
	//      Get the number of executions made by the command.
	// Returns:
	//      The number of executions made by the command.
	//-----------------------------------------------------------------------
	UINT GetPassNumber() const;

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
	//      Applies series style to the current element.
	// Parameters:
	//      pStyle - Series style to apply.
	//-----------------------------------------------------------------------
	virtual void ApplySeriesStyle(CXTPChartSeriesStyle* pStyle);

	//-----------------------------------------------------------------------
	// Summary:
	//      Performs inital preparation steps necessary for successful command execution.
	// Parameters:
	//      pDC - Device context pointer for which the command is to be prepared.
	// See also:
	//      PrepareOverride
	//-----------------------------------------------------------------------
	virtual void Prepare(CXTPChartDeviceContext* pDC);

	//-----------------------------------------------------------------------
	// Summary:
	//      Derived implementation must override this method for implementing
	//      implementation specific preparation behavior.
	// Parameters:
	//      pDC - Device context pointer for which the command is to be prepared.
	// See also:
	//      Prepare
	//-----------------------------------------------------------------------
	virtual void PrepareOverride(CXTPChartDeviceContext* pDC);

	//-----------------------------------------------------------------------
	// Summary:
	//      Determines if the command is currently enabled.
	// Returns:
	//      TRUE if the command is currently enabled.
	// See also:
	//      SetEnabled
	//-----------------------------------------------------------------------
	virtual BOOL IsEnabled() const;

	//-----------------------------------------------------------------------
	// Summary:
	//      Enables/disable the command.
	// Parameters:
	//      bEnabled - TRUE to enable the command.
	//-----------------------------------------------------------------------
	void SetEnabled(BOOL bEnabled = TRUE);

	//-----------------------------------------------------------------------
	// Summary:
	//      Obtains device context pointer for which the command is currenly
	//      being prepared or executed.
	// Returns:
	//      Device context pointer for which the command is currenly
	//      being prepared or executed or NULL if no preparation or execution
	//      is in progress.
	//-----------------------------------------------------------------------
	CXTPChartDeviceContext* GetCurrentDC() const;

	//-----------------------------------------------------------------------
	// Summary:
	//      Binds a view to the command object. The view reference is held
	//      until another view is bound.
	// Parameters:
	//      pView - A view pointer. If another view is bound its reference
	//              is released. NULL unbinds the current view.
	//-----------------------------------------------------------------------
	void BindView(CXTPChartElementView* pView);

	//-----------------------------------------------------------------------
	// Summary:
	//      Obtains a bound view pointer or NULL if no view is bound.
	// Returns:
	//      A bound view pointer or NULL if no view is bound.
	//-----------------------------------------------------------------------
	CXTPChartElementView* GetBoundView() const;

protected:
	//The array of child device commands.
	CArray<CXTPChartDeviceCommand*, CXTPChartDeviceCommand*> m_arrChildren;

private:
	BOOL m_bCommitted;
	UINT m_nPassNumber;
	BOOL m_bEnabled;
	CXTPChartDeviceContext* m_pCurrentDC;
	CXTPChartElementView* m_pBoundView;
};

AFX_INLINE BOOL CXTPChartDeviceCommand::IsCommitted() const {
	return m_bCommitted;
}
AFX_INLINE void CXTPChartDeviceCommand::Commit() {
	ASSERT("Must not be committed twice" && !m_bCommitted);
	m_bCommitted = TRUE;
}

AFX_INLINE UINT CXTPChartDeviceCommand::GetPassNumber() const {
	return m_nPassNumber;
}

AFX_INLINE void CXTPChartDeviceCommand::SetEnabled(BOOL bEnabled /*= TRUE*/) {
	m_bEnabled = bEnabled;
}

AFX_INLINE CXTPChartDeviceContext* CXTPChartDeviceCommand::GetCurrentDC() const {
	return m_pCurrentDC;
}

AFX_INLINE CXTPChartElementView* CXTPChartDeviceCommand::GetBoundView() const {
	return m_pBoundView;
}

enum XTPChart3dAntialiasingPolicy
{
	xtpChart3dAntialiasingPolicyDefault,
	xtpChart3dAntialiasingPolicyEnabled,
	xtpChart3dAntialiasingPolicyDisabled,
};

//===========================================================================
// Summary:
//     Defines a common implementation interface for chart 3D drawing commands.
//===========================================================================
class _XTP_EXT_CLASS CXTPChart3dDeviceCommand : public CXTPChartDeviceCommand
{
	DECLARE_DYNAMIC(CXTPChart3dDeviceCommand);

protected:
	//-----------------------------------------------------------------------
	// Summary:
	//     Constructs a CXTPChart3dDeviceCommand object.
	//-----------------------------------------------------------------------
	CXTPChart3dDeviceCommand(XTPChart3dAntialiasingPolicy nAntialiasingPolicy);

public:
	//-----------------------------------------------------------------------
	// Summary:
	//      Determines if the command is currently enabled.
	// Returns:
	//      TRUE if the command is currently enabled.
	// Remarks:
	//      3D commands consider anti-aliasing policy and current device context
	//      rendering state in order to determine current enabled status.
	//      If device context is skipping anti-aliasing phase when calling
	//      this method then the policy is disregarded and the command is
	//      considered enabled unles it's been explicitly disabled by the user.
	//      If device context is in anti-alising od refault rendering state when 
	//      this method is called the policy determines the command state
	//      unless it's been explicitly disabled by the user.
	// See also:
	//      SetEnabled, XTPChart3dAntialiasingPolicy
	//-----------------------------------------------------------------------
	virtual BOOL IsEnabled() const;

private:
	XTPChart3dAntialiasingPolicy m_nAntialiasingPolicy;
};

//-----------------------------------------------------------------------
// Summary:
//      Defines a base class interface for 2D hit test command.
//-----------------------------------------------------------------------
class _XTP_EXT_CLASS CXTPChartHitTestElementCommand : public CXTPChartDeviceCommand
{
public:
	//-----------------------------------------------------------------------
	// Summary:
	//      Constructs CXTPChartHitTestElementCommand object instance.
	// Parameters:
	//      pElement - Chart element pointer for which hit testing is to be performed
	//                 including all its children elements recursively.
	//      rcBounds - Hit test boundary rectangle.
	// Returns:
	//      
	// See also:
	//      
	//-----------------------------------------------------------------------
	CXTPChartHitTestElementCommand(CXTPChartElement* pElement);
	CXTPChartHitTestElementCommand(CXTPChartElement* pElement, const CRect& rcBounds); // <combine CXTPChartHitTestElementCommand::CXTPChartHitTestElementCommand@CXTPChartElement*>
	CXTPChartHitTestElementCommand(CXTPChartElement* pElement, const CXTPChartRectF& rcBounds); // <combine CXTPChartHitTestElementCommand::CXTPChartHitTestElementCommand@CXTPChartElement*>

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
	CXTPChartElement* m_pElement; // Chart element pointer for which hit testing is to be performed.
	CRect m_rcBounds; // Hit test boundary rectangle.
};


#include "Common/Base/Diagnostic/XTPEnableNoisyWarnings.h"
#endif //#if !defined(__XTPCHARTDEVICECOMMAND_H__)
