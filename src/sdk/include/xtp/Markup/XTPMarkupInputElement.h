// XTPMarkupInputElement.h: interface for the CXTPMarkupInputElement class.
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
#if !defined(__XTPMARKUPINPUTELEMENT_H__)
#define __XTPMARKUPINPUTELEMENT_H__
//}}AFX_CODEJOCK_PRIVATE

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Common/Base/Diagnostic/XTPDisableNoisyWarnings.h"

class CXTPMarkupString;
class CXTPMarkupContext;
class CXTPMarkupTriggerCollection;

class CXTPMarkupEventHandlerMap;
class CXTPMarkupDelegate;
class CXTPMarkupRoutedEventArgs;
class CXTPMarkupMouseEventArgs;
class CXTPMarkupMouseWheelEventArgs;
class CXTPMarkupQueryCursorEventArgs;
class CXTPMarkupMouseButtonEventArgs;
class CXTPMarkupTransformationMatrix;


class _XTP_EXT_CLASS CXTPMarkupInputElement : public CXTPMarkupObject
{
	DECLARE_MARKUPCLASS(CXTPMarkupInputElement);

protected:
	CXTPMarkupInputElement();
	virtual ~CXTPMarkupInputElement();

public:
	virtual void OnMouseLeave(CXTPMarkupMouseEventArgs* e);
	virtual void OnMouseEnter(CXTPMarkupMouseEventArgs* e);
	virtual void OnMouseMove(CXTPMarkupMouseEventArgs* e);
	virtual void OnQueryCursor(CXTPMarkupQueryCursorEventArgs* e);

	virtual void OnMouseLeftButtonUp(CXTPMarkupMouseButtonEventArgs* e);
	virtual void OnMouseRightButtonUp(CXTPMarkupMouseButtonEventArgs* e);
	virtual void OnMouseLeftButtonDown(CXTPMarkupMouseButtonEventArgs* e);
	virtual void OnMouseRightButtonDown(CXTPMarkupMouseButtonEventArgs* e);

	virtual void OnLostMouseCapture(CXTPMarkupMouseEventArgs* e);
	CXTPMarkupContext* GetMarkupContext() const;

	virtual void OnMouseWheel(CXTPMarkupMouseWheelEventArgs* e);

	void Apply();


public:
	virtual CXTPMarkupInputElement* InputHitTest(CPoint point) const;

	BOOL IsMouseOver() const;
	CXTPMarkupObject* GetToolTip() const;
	void SetToolTip(LPCTSTR lpszTooltip);

	void AddHandler(CXTPMarkupRoutedEvent* pEvent, CXTPMarkupDelegate* pDelegate);

	CXTPMarkupObject* FindResource(const CXTPMarkupObject* pKey) const;

	void Focus();
	BOOL IsKeyboardFocused() const;

	CXTPMarkupString* GetName() const;
	void SetName(LPCTSTR lpszName);

protected:
	void RaiseEvent(CXTPMarkupRoutedEventArgs* pEventArgs);

protected:
	void CaptureMouse();
	void ReleaseMouseCapture();

protected:
	void FireTriggers(CXTPMarkupDependencyProperty* pProperty, CXTPMarkupObject* pNewValue);
	BOOL AddStyleTriggers(CXTPMarkupStyle* pStyle, CXTPMarkupDependencyProperty* pProperty, CXTPMarkupObject* pNewValue);

protected:
	void SetPropertyObject(CXTPMarkupBuilder* pBuilder, CXTPMarkupDependencyProperty* pProperty, CXTPMarkupObject* pValue);
	virtual void OnFinalRelease();
	virtual void OnApply();
	void ApplyTree(CXTPMarkupObject* pChild);

	static void AFX_CDECL OnStylePropertyChanged(CXTPMarkupObject* d, CXTPMarkupPropertyChangedEventArgs* /*e*/);
	static void AFX_CDECL OnCursorPropertyChanged(CXTPMarkupObject* d, CXTPMarkupPropertyChangedEventArgs* e);
	virtual CXTPMarkupObject* GetValueCore(CXTPMarkupDependencyProperty* pProperty) const;


protected:
//{{AFX_CODEJOCK_PRIVATE
public:
	DECLARE_DISPATCH_MAP()
#ifdef _XTP_ACTIVEX
	DECLARE_INTERFACE_MAP()
	DECLARE_OLETYPELIB_EX(CXTPMarkupInputElement);
#endif

	afx_msg long OleGetCursor();
	afx_msg void OleSetCursor(long nValue);
	afx_msg VARIANT OleGetToolTip();
	afx_msg void OleSetToolTip(const VARIANT& va);
	afx_msg BOOL OleGetIsMouseOver();
	afx_msg void OleAddHandler(LPDISPATCH lpEventDisp, LPCOLESTR Method);
	afx_msg LPDISPATCH OleFindName(LPCOLESTR lpszName);
	void OleSetName(LPCTSTR lpszName);
	BSTR OleGetName();

	afx_msg LPDISPATCH OleGetMouseLeaveEvent();
	afx_msg LPDISPATCH OleGetMouseEnterEvent();
	afx_msg LPDISPATCH OleGetMouseLeftButtonUpEvent();
	afx_msg LPDISPATCH OleGetMouseLeftButtonDownEvent();
	afx_msg LPDISPATCH OleGetMouseRightButtonUpEvent();
	afx_msg LPDISPATCH OleGetMouseRightButtonDownEvent();
	afx_msg LPDISPATCH OleGetMouseMoveEvent();
	afx_msg LPDISPATCH OleGetLostMouseCaptureEvent();

//}}AFX_CODEJOCK_PRIVATE

protected:
	CXTPMarkupEventHandlerMap* m_pHandlers;
	CXTPMarkupContext* m_pMarkupContext;
	CXTPMarkupTriggerCollection* m_pActiveTriggers;
	CXTPMarkupProperties* m_pTriggerProperties;
	CXTPMarkupStyle* m_pStyleCache;

public:
	static CXTPMarkupDependencyProperty* m_pCursorProperty;
	static CXTPMarkupDependencyProperty* m_pIsMouseOverProperty;
	static CXTPMarkupDependencyProperty* m_pStyleProperty;
	static CXTPMarkupDependencyProperty* m_pResourcesProperty;
	static CXTPMarkupDependencyProperty* m_pToolTipProperty;

	static CXTPMarkupDependencyProperty* m_pIsKeyboardFocusedProperty;
	static CXTPMarkupDependencyProperty* m_pIsFocusableProperty;

	static CXTPMarkupRoutedEvent* m_pMouseLeaveEvent;
	static CXTPMarkupRoutedEvent* m_pMouseEnterEvent;
	static CXTPMarkupRoutedEvent* m_pMouseLeftButtonUpEvent;
	static CXTPMarkupRoutedEvent* m_pMouseLeftButtonDownEvent;
	static CXTPMarkupRoutedEvent* m_pMouseRightButtonUpEvent;
	static CXTPMarkupRoutedEvent* m_pMouseRightButtonDownEvent;
	static CXTPMarkupRoutedEvent* m_pMouseMoveEvent;
	static CXTPMarkupRoutedEvent* m_pLostMouseCaptureEvent;

private:
	friend class CXTPMarkupContext;
};

AFX_INLINE CXTPMarkupContext* CXTPMarkupInputElement::GetMarkupContext() const {
	return m_pMarkupContext;
}





//===========================================================================
// Summary: Provides rendering support for XML Markup, which includes hit testing,
//          coordinate transformation, and bounding box calculations.
//===========================================================================
class _XTP_EXT_CLASS CXTPMarkupVisual : public CXTPMarkupInputElement
{
	DECLARE_MARKUPCLASS(CXTPMarkupVisual);
public:

	//-------------------------------------------------------------------------
	// Summary: Provides the base initialization for objects derived from the
	//          CXTPMarkupVisual class.
	//-------------------------------------------------------------------------
	CXTPMarkupVisual();

	virtual ~CXTPMarkupVisual();

public:
	//-------------------------------------------------------------------------
	// Summary: Call this method to draw Visual object inside drawing context.
	// Parameters: drawingContext - Drawing context to draw in.
	//-------------------------------------------------------------------------
	void Render(CXTPMarkupDrawingContext* drawingContext);

	//-----------------------------------------------------------------------
	// Input:   point - Point to test.
	// Summary: Detects if a point belongs to this object.
	// Returns: The CXTPMarkupInputElement at the specified point.
	//-----------------------------------------------------------------------
	CXTPMarkupInputElement* InputHitTest(CPoint point) const;

public:

	//-----------------------------------------------------------------------
	// Summary: Gets the number of child elements for the Visual.
	// Remarks: By default, a Visual has no children. Therefore, the default
	//          implementation always returns 0.
	// Returns: The number of child elements.
	//-----------------------------------------------------------------------
	virtual int GetVisualChildrenCount() const;

	//-----------------------------------------------------------------------
	// Input:   nIndex - The index of the visual object in the VisualCollection.
	// Summary: Returns the specified Visual in the parent VisualCollection.
	// Returns: The child in the VisualCollection at the specified index value.
	//-----------------------------------------------------------------------
	virtual CXTPMarkupVisual* GetVisualChild(int nIndex) const;


	//-----------------------------------------------------------------------
	// Summary: Number of child elements of object
	// Returns: The number of logical child elements of this object.
	//-----------------------------------------------------------------------
	virtual int GetLogicalChildrenCount() const;

	//-----------------------------------------------------------------------
	// Summary: Returns logical child elements of this object.
	// Parameters: nIndex - The index of the object in the logical objects tree.
	// Returns: Returns the specified logical child object in the parent collection
	// Remarks: LogicalChildren allows you to iterate over child elements.
	//          This is useful for elements that may not have a defined, dedicated collection but still contain more than one child element
	//-----------------------------------------------------------------------
	virtual CXTPMarkupObject* GetLogicalChild(int nIndex) const;

	//-----------------------------------------------------------------------
	// Summary: Returns bounding rectangle of the element
	// Returns: Bounding rectangle of the element
	//-----------------------------------------------------------------------
	CRect GetBoundRect() const;

	//-----------------------------------------------------------------------
	// Summary: Computes a rectangle that has to be updated.
	// Returns: Rectangle that requires update.
	//-----------------------------------------------------------------------
	CRect GetUpdateRect() const;

	//-----------------------------------------------------------------------
	// Summary: Gets the final render size of this element.
	// Returns: The rendered size for this element.
	//-----------------------------------------------------------------------
	CSize GetRenderSize() const;

	//-----------------------------------------------------------------------
	// Summary: Gets or sets the offset value of the visual object.
	// Returns: The offset value of the visual object.
	//-----------------------------------------------------------------------
	CPoint GetVisualOffset() const;

	//-----------------------------------------------------------------------
	// Input:   rc - The available size provided by the element.
	// Summary: Returns an alternative clipping geometry.
	// Returns: The potential clipping geometry.
	//-----------------------------------------------------------------------
	virtual BOOL GetLayoutClip(CRect& rc) const;

	//-----------------------------------------------------------------------
	// Summary: Gets the visual tree parent of the visual object.
	// Returns: The Visual parent.
	//-----------------------------------------------------------------------
	CXTPMarkupVisual* GetVisualParent() const;

	//-----------------------------------------------------------------------
	// Summary: A structure that optionally describes the previous and the
	//          new transformation matrices.
	//-----------------------------------------------------------------------
	struct TransformationMatrices
	{
		//-----------------------------------------------------------------------
		// Summary: Constructs an empty object.
		//-----------------------------------------------------------------------
		TransformationMatrices();

		//-----------------------------------------------------------------------
		// Summary: Constructs an object with old and new tranformation matrices
		//          specified.
		// Parameters:
		//    pOldMatrix - A pointer to an old tranformation matrix.
		//    pNewMatrix - A pointer to a new tranformation matrix.
		//-----------------------------------------------------------------------
		TransformationMatrices(
			CXTPMarkupTransformationMatrix* pOldMatrix,
			CXTPMarkupTransformationMatrix* pNewMatrix);

		CXTPMarkupTransformationMatrix* pOldMatrix; // A pointer to an old tranformation matrix.
		CXTPMarkupTransformationMatrix* pNewMatrix; // A pointer to a new tranformation matrix.
	};

protected:
	//-----------------------------------------------------------------------
	// Summary: Applies render transformation if necessary.
	// Parameters: pContext - Drawing context to apply render transformation for.
	// Returns: A structure that optionally describes the previous and the new
	//          transformation matrices.
	//-----------------------------------------------------------------------
	virtual TransformationMatrices ApplyRenderTransform(CXTPMarkupDrawingContext* pContext);

	//-----------------------------------------------------------------------
	// Summary: Restores previously applies render transformation.
	// Parameters:
	//    pContext - Drawing context to restore render transformation for.
	//    matrices - Transformation matrices previously returned by a call
	//               to ApplyRenderTransform.
	//-----------------------------------------------------------------------
	virtual void RestoreRenderTransform(CXTPMarkupDrawingContext* pContext, TransformationMatrices& matrices);

	//-----------------------------------------------------------------------
	// Summary: Checks whether a point is inside rectangle specified taking
	//          into account transformation if applied.
	// Parameters:
	//    rect - rectangle relative to object position.
	//    point - point to check.
	// Returns: TRUE if point is inside rectangle specified.
	//-----------------------------------------------------------------------
	BOOL PtInRectTransform(const RECT& rect, const POINT& point) const;

	//-----------------------------------------------------------------------
	// Summary: Obtains absolute element visual offset.
	// Return: Absolute element visual offset.
	//-----------------------------------------------------------------------
	POINT GetAbsoluteVisualOffset() const;

	//-----------------------------------------------------------------------
	// Summary: When overridden in a derived class, participates in rendering operations that are directed by the layout system
	// Parameters: drawingContext - Drawing context to draw in.
	//-----------------------------------------------------------------------
	virtual void OnRender(CXTPMarkupDrawingContext* drawingContext);

	//-----------------------------------------------------------------------
	// Summary: This method is called to render focus rectangle of focused object
	// Parameters: drawingContext - Drawing context to draw in.
	//-----------------------------------------------------------------------
	virtual void OnRenderFocusVisual(CXTPMarkupDrawingContext* drawingContext);

	//-----------------------------------------------------------------------
	// Summary: Override this method in derived class to provide visual hit testing of child object
	// Parameters: point - POisiotn of cursor to test.
	//-----------------------------------------------------------------------
	virtual CXTPMarkupInputElement* InputHitTestOverride(CPoint point) const;

	virtual void RenderClipped(CXTPMarkupDrawingContext* drawingContext);

//{{AFX_CODEJOCK_PRIVATE
	// Internal methods
protected:
	void UpdateBoundRect();
	CXTPMarkupTransformationMatrix* GetAbsoluteTransformationMatrix() const;
	void SetCurrentTransformationMatrix(CXTPMarkupTransformationMatrix* pMatrix);
//}}AFX_CODEJOCK_PRIVATE

//{{AFX_CODEJOCK_PRIVATE
public:
	DECLARE_DISPATCH_MAP()
#ifdef _XTP_ACTIVEX
	DECLARE_INTERFACE_MAP()
	DECLARE_OLETYPELIB_EX(CXTPMarkupVisual);
#endif

	afx_msg LPDISPATCH OleGetVisualParent();
	afx_msg void OleGetBoundingRect(int* left, int* top, int* right, int* bottom);
	afx_msg int OleGetActualWidth();
	afx_msg int OleGetActualHeight();
//}}AFX_CODEJOCK_PRIVATE

protected:
	CPoint m_ptVisualOffset;        // The offset value of the visual object.
	CSize m_szRenderSize;           // The rendered size for this element.
	CRect m_rcBoundRect;            // Bounding rectangle of the element
	CRect m_rcUpdateRect;           // Update rectangle.
	CRect m_rcPrevUpdateRect;       // Previous update rectangle.
	CXTPMarkupTransformationMatrix* m_pCurrentTransformationMatrix;
};


AFX_INLINE CXTPMarkupVisual::TransformationMatrices::TransformationMatrices()
	: pOldMatrix(NULL)
	, pNewMatrix(NULL)
{
}

AFX_INLINE CXTPMarkupVisual::TransformationMatrices::TransformationMatrices(
	CXTPMarkupTransformationMatrix* pOldMatrix,
	CXTPMarkupTransformationMatrix* pNewMatrix)
	: pOldMatrix(pOldMatrix)
	, pNewMatrix(pNewMatrix)
{
}

AFX_INLINE void CXTPMarkupVisual::OnRender(CXTPMarkupDrawingContext* /*drawingContext*/)
{
}

AFX_INLINE void CXTPMarkupVisual::OnRenderFocusVisual(CXTPMarkupDrawingContext* /*drawingContext*/)
{
}

AFX_INLINE CXTPMarkupVisual::TransformationMatrices CXTPMarkupVisual::ApplyRenderTransform(CXTPMarkupDrawingContext* pContext)
{
	UNREFERENCED_PARAMETER(pContext);
	return TransformationMatrices();
}

AFX_INLINE void CXTPMarkupVisual::RestoreRenderTransform(CXTPMarkupDrawingContext* pContext, TransformationMatrices& matrices)
{
	UNREFERENCED_PARAMETER(pContext);
	UNREFERENCED_PARAMETER(matrices);
}

AFX_INLINE CSize CXTPMarkupVisual::GetRenderSize() const
{
	return m_szRenderSize;
}

AFX_INLINE CRect CXTPMarkupVisual::GetBoundRect() const
{
	return m_rcBoundRect;
}

AFX_INLINE CPoint CXTPMarkupVisual::GetVisualOffset() const
{
	return m_ptVisualOffset;
}

#include "Common/Base/Diagnostic/XTPEnableNoisyWarnings.h"
#endif // !defined(__XTPMARKUPINPUTELEMENT_H__)
