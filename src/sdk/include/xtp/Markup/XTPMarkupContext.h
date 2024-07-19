// XTPMarkupContext.h: interface for the CXTPMarkupBorder class.
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
#if !defined(__XTPMARKUPCONTEXT_H__)
#define __XTPMARKUPCONTEXT_H__
//}}AFX_CODEJOCK_PRIVATE

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Common/Base/Diagnostic/XTPDisableNoisyWarnings.h"

class CXTPDpi;
class CXTPMarkupObject;
class CXTPImageManager;
class CXTPMarkupUIElement;
class CXTPMarkupInputElement;
class CXTPMarkupContext;
class CXTPMarkupEventHandlerMap;
class CXTPMarkupRoutedEventArgs;
class CXTPMarkupRoutedEvent;
class CXTPMarkupDelegate;
class CXTPMarkupFont;
class CXTPMarkupType;
class CXTPMarkupKeyboardNavigation;
class CXTPToolTipContext;
class CXTPMarkupParser;
class CXTPMarkupBuilder;
class CXTPActiveScriptEngine;
class CXTPActiveScriptStdRuntime;
class CXTPMarkupDelegateMap;
class CXTPMarkupStaticExtension;
template<class TYPE>
class CXTPMarkupTypedSimpleStack;
enum XTPMarkupSmoothingMode;

#ifdef _XTP_ACTIVEX
class _XTP_EXT_CLASS CXTPMarkupContext : public CXTPCmdTarget
#else

//{{AFX_CODEJOCK_PRIVATE
class CXTPMarkupContextInternalDispatch;
//}}AFX_CODEJOCK_PRIVATE

//===========================================================================
// Summary: CXTPMarkupContext is standalone class that used to provide common settings for Markup objects.
//===========================================================================
class _XTP_EXT_CLASS CXTPMarkupContext
#endif
{
protected:
	class CInputElementCollection;

public:
	//-----------------------------------------------------------------------
	// Summary:
	//     Constructs a CXTPMarkupContext object
	//-----------------------------------------------------------------------
	CXTPMarkupContext();

protected:
	//-----------------------------------------------------------------------
	// Summary:
	//     Destroys a CXTPMarkupContext object, handles cleanup and deallocation
	//-----------------------------------------------------------------------
	virtual ~CXTPMarkupContext();

public:
	//-----------------------------------------------------------------------
	// Summary:
	//     Call this method to get default font properties for markup objects
	// Returns: LOGFONT object contained properties for default font
	//-----------------------------------------------------------------------
	LOGFONT* GetDefaultLogFont();

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this method to get default text color of text block object
	// Returns: Default text color
	//-----------------------------------------------------------------------
	COLORREF GetDefaultForeground();

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this method to set default text color that will be used for TextBlock objects
	// Parameters: clr - Default text color
	//-----------------------------------------------------------------------
	void SetDefaultTextColor(COLORREF clr);

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this method to set default font that will be used for TextBlock objects
	// Parameters: font - Font to be set as default
	//-----------------------------------------------------------------------
	void SetDefaultFont(HFONT font);
	void SetDefaultFont(LOGFONT* font); // <combine CXTPMarkupContext::SetDefaultFont@HFONT>

	//-----------------------------------------------------------------------
	// Summary:
	//      Gets/sets default smoothing mode to be used for any drawing context
	//      that uses the context. By default smoothing mode is applied to all graphical
	//      elements except text, to enable text smoothing refer to SetDefaultTextSmoothing.
	// Parameters:
	//      smoothingMode - Smoothing mode.
	// Returns:
	//      Current or previous rendering smoothing mode.
	// See also:
	//      GetDefaultTextSmoothing, SetDefaultTextSmoothing
	//-----------------------------------------------------------------------
	XTPMarkupSmoothingMode GetDefaultSmoothingMode() const;
	XTPMarkupSmoothingMode SetDefaultSmoothingMode(XTPMarkupSmoothingMode smoothingMode); // <combine CXTPMarkupContext::GetDefaultSmoothingMode>

	//-----------------------------------------------------------------------
	// Summary:
	//      Gets/sets default smoothing mode enablement flag for text smoothing to be used for 
	//      any drawing context that uses the context. By default smoothing mode is applied to all graphical
	//      elements except text.
	// Parameters:
	//      bEnableTextSmoothing - TRUE to enable text smoothing according to overal smoothing mode.
	// Returns:
	//      Current or previous text smoothing enablement flag.
	// See also:
	//      GetDefaultSmoothingMode, SetDefaultSmoothingMode
	//-----------------------------------------------------------------------
	BOOL GetDefaultTextSmoothing() const;
	BOOL SetDefaultTextSmoothing(BOOL bEnableTextSmoothing); // <combine CXTPMarkupContext::GetDefaultTextSmoothing>

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this method to create Markup tree from its XML string.
	// Parameters: lpszBuffer - XML Markup string.
	// Returns: Returns top level Markup element.
	//-----------------------------------------------------------------------
	CXTPMarkupUIElement* Parse(LPCSTR lpszBuffer);

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this method to create Markup tree from its XML string.
	// Parameters: lpszBuffer - XML Markup string.
	// Returns: Returns top level Markup element.
	//-----------------------------------------------------------------------
	CXTPMarkupUIElement* Parse(LPCWSTR lpszBuffer);

	//-----------------------------------------------------------------------
	// Summary: Validates XML correctness.
	// Parameters: lpszBuffer - XML data to parse and validate.
	// Returns: Validation status message.
	//-----------------------------------------------------------------------
	CString ValidateXML(LPCWSTR lpszBuffer);

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this method to connect Markup event with class member or static method delegate
	// Parameters:
	//     pEvent - Event to subscribe
	//     pDelegate - method Delegate
	// Example:
	//      <code>m_wndStatic.AddHandler(CXTPMarkupHyperlink::m_pClickEvent, CreateMarkupClassDelegate(this, &CMarkupSampleDlg::OnHyperlinkClick)); </code>
	//-----------------------------------------------------------------------
	void AddHandler(CXTPMarkupRoutedEvent* pEvent, CXTPMarkupDelegate* pDelegate);

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this method to set Delegate for Markup text event
	// Parameters:
	//     lpszDelegate - Text even delegate
	//     pDelegate - method Delegate
	// Example:
	//      <code><TextBlock><Hyperlink Click='OnClick'>FTSE 100</Hyperlink></TextBlock></code>
	//      <code>.... </code>
	//      <code>pPopup->GetMarkupContext()->SetDelegate(L"OnClick", CreateMarkupFunctionDelegate(&HyperlinkClick));</code>
	//-----------------------------------------------------------------------
	void SetDelegate(LPWSTR lpszDelegate, CXTPMarkupDelegate* pDelegate);

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this method to enable/disable GDI+ usage
	// Parameters:
	//     bEnable - TRUE to enable; FALSE to disable GDI+
	//-----------------------------------------------------------------------
	void EnableGdiPlus(BOOL bEnable);

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this method to determine if GDI+ is enabled/disabled.
	// Returns: TRUE if enabled; FALSE if GDI+ is disabled.
	//-----------------------------------------------------------------------
	BOOL IsGdiPlusEnabled() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this method to get tooltip context pointer.
	//-----------------------------------------------------------------------
	CXTPToolTipContext* GetToolTipContext() const;
	
	//-----------------------------------------------------------------------
	// Summary:
	//     Call this method to determine if custom positioning for tooltips is enabled/disabled.
	// Returns: TRUE if enabled; FALSE if custom positioning for tooltips is disabled.
	// See also: SetUseCustomToolTipPosition, GetCustomToolTipPosition
	//-----------------------------------------------------------------------
	BOOL GetUseCustomToolTipPosition() const;
	
	//-----------------------------------------------------------------------
	// Summary:
	//     Call this method to enable/disable custom positioning for tooltips.
	// Parameters:
	//     bUse    - Set this parameter to TRUE if you'd like to enable custom 
	//               positioning for tooltips. FALSE else.
	//     ptPos   - A CPoint that specifies the x- and y- coordinates of the tooltip window.
	//               These coordinates are always relative to the
	//               upper-left corner of the window.
	// Remarks:
	//      This feature can be used before calling CXTPToolTipContext::FilterToolTipMessage
	//      to place tooltip window in desirable place.
	// See also: GetUseCustomToolTipPosition, GetCustomToolTipPosition
	//-----------------------------------------------------------------------
	void SetUseCustomToolTipPosition(BOOL bUse, CPoint ptPos = CPoint(0, 0));

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this method to get custom positioning for tooltips.
	// Returns: CPoint object with custom coordinates of left-top corner of tooltip window.
	// See also: GetUseCustomToolTipPosition, SetUseCustomToolTipPosition
	CPoint GetCustomToolTipPosition() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this method to set isolated image manager for elements inside Markup context.
	// Parameters:
	//     pImageManager - new ImageManager to use
	// Example:
	//     <code>m_pMarkupContext->SetImageManager(new CXTPImageManager());</code>
	//-----------------------------------------------------------------------
	void SetImageManager(CXTPImageManager* pImageManager);

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this method to retrieve image manager associated with context.
	// Returns:
	//     Pointer to CXTPImageManager that selected in context.
	// Remarks:
	//      Function return XTPImageManager(); by default, call SetImageManager to set isolated images
	//-----------------------------------------------------------------------
	CXTPImageManager* GetImageManager() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Sets context specific color key for Image element.
	// Parameters:
	//     crColorKey - New color key value or COLORREF_NULL to use
	//                  the one provided by assigned image manager
	//                  or a default RGB(0, 0xff, 0).
	//-----------------------------------------------------------------------
	void SetImageColorKey(COLORREF crColorKey);

	//-----------------------------------------------------------------------
	// Summary:
	//     Obtains Image element color key value. If context color key is not set
	//     it tries to obtain image manager color mask or returns a default
	//     value of RGB(0, 0xff, 0)
	// Returns:
	//     Image element color key.
	//-----------------------------------------------------------------------
	COLORREF GetImageColorKey() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Returns a pointer to the extension root object used to add custom markup extensions.
	// Returns:
	//     A pointer to the extension root object.
	//-----------------------------------------------------------------------
	CXTPMarkupStaticExtension* GetExtensionRoot();

	//-----------------------------------------------------------------------
	// Summary:
	//     Returns TRUE if the context is DPI aware, i.e. the graphics is scaled
	//     according to the active DPI settings.
	// Returns:
	//     TRUE is the context is DPI aware.
	//-----------------------------------------------------------------------
	BOOL IsDpiAware() const;
	
	//-----------------------------------------------------------------------
	// Summary:
	//     Enables/disable context DPI awareness. 
	// Parameters:
	//     bSet - If TRUE the graphics is scaled according to the active DPI settings.
	//     hWnd - If specified, window's device context information will be used for DPI translation.
	//     hDC - If specified, device context information will be used for DPI translation.
	//-----------------------------------------------------------------------
	void SetDpiAware(BOOL bSet = TRUE);
	void SetDpiAware(BOOL bSet, HWND hWnd); // <combine CXTPMarkupContext::SetDpiAware@BOOL>
	void SetDpiAware(BOOL bSet, HDC hDC); // <combine CXTPMarkupContext::SetDpiAware@BOOL>

	//-----------------------------------------------------------------------
	// Summary:
	//     Perform X and Y coordinates scalling according to current DPI settings
	//     is markup context is DPI aware.
	// Parameters:
	//     x, y - Coordinates to scale according to current DPI settings.
	// Returns:
	//     Scaled value if markup context is DPI aware, otherwise an unchanged value is returned.
	// See also: SetDpiAware
	//-----------------------------------------------------------------------
	int ScaleX(int x) const;
	int ScaleY(int y) const; // <combine CXTPMarkupContext::ScaleX@int>
	float ScaleX(float x) const; // <combine CXTPMarkupContext::ScaleX@int>
	float ScaleY(float y) const; // <combine CXTPMarkupContext::ScaleX@int>
	double ScaleX(double x) const; // <combine CXTPMarkupContext::ScaleX@int>
	double ScaleY(double y) const; // <combine CXTPMarkupContext::ScaleX@int>
	CSize Scale(const SIZE& size) const; // <combine CXTPMarkupContext::ScaleX@int>

	//-----------------------------------------------------------------------
	// Summary: Loads a chunk of script code in a specified language.
	// Parameters:
	//    pBuilder - Markup builder pointer from which code is loaded.
	//    lpszLanguage - Script code language.
	//    lpszCode - Script code.
	//-----------------------------------------------------------------------
	void LoadScriptCodeChunk(
		CXTPMarkupBuilder* pBuilder,
		LPCWSTR lpszLanguage,
		LPCWSTR lpszCode);

	//-----------------------------------------------------------------------
	// Summary: Runs script engine.
	// Parameters: pBuilder - markup builder pointer that runs script engine.
	//-----------------------------------------------------------------------
	void RunScriptEngine(CXTPMarkupBuilder* pBuilder);

//{{AFX_CODEJOCK_PRIVATE
	// Internal methods
	void ResetScriptEngine();
	void RegisterScriptNamedObjects(CXTPMarkupBuilder* pBuilder, CXTPMarkupObject* pObject);
	void ThrowScriptEngineException(CXTPMarkupBuilder* pBuilder, HRESULT hr);

	CXTPMarkupFont* GetFont(LOGFONT* pLogFont);

	static BOOL AFX_CDECL CompareFont(LOGFONT* pLogFont1, LOGFONT* pLogFont2);

	virtual BOOL OnWndMsg(CXTPMarkupUIElement* pUIElement, UINT message, WPARAM wParam, LPARAM lParam, LRESULT* pResult);

	void RaiseEvent(CXTPMarkupObject* pSource, CXTPMarkupRoutedEventArgs* pEventArgs);

	CXTPMarkupDelegate* LookupDelegate(LPCWSTR lpszDelegate) const;

	void CaptureMouse(CXTPMarkupInputElement* pUIElement);
	void ReleaseMouseCapture(CXTPMarkupInputElement* pUIElement);
	CXTPMarkupInputElement* GetMouseCapture() const;

	CXTPMarkupKeyboardNavigation* GetKeyboardNavigation() const;

	void AddRef();
	void Release();
	INT_PTR OnToolHitTest(CPoint point, TOOLINFO* pToolInfo);

public:
	CRect GetClientBoundingRect(CXTPMarkupInputElement* pUIElement) const;
	CRect GetUpdateRect(CXTPMarkupInputElement* pUIElement) const;
	BOOL IsVisualChild(CXTPMarkupObject* pParent, CXTPMarkupObject* pChild) const;

private:
	void HandleMouseMove(CXTPMarkupUIElement* pUIElement, CPoint point);
	void HandleMouseEnter(CXTPMarkupInputElement* pUIElement, CPoint point);
	BOOL HandleSetCursor();
	BOOL HandleMouseWheel(int nDelta);
	BOOL HandleMouseUpDown(UINT message, WPARAM wParam, LPARAM lParam);

	void BuildInputList(CXTPMarkupObject* pUIElement, CInputElementCollection* list);

	void Cleanup();

	void AddPendingUpdate(CXTPMarkupUIElement* pUIElement);

public:
	virtual void OnInvalidateVisual(CXTPMarkupUIElement* pUIElement);
	virtual void OnInvalidateArrange(CXTPMarkupUIElement* pUIElement);

public:
	CXTPMarkupObject* CreateMarkupObject(CXTPMarkupType* pRuntimeClass);
	void FinalizeMarkupObject(CXTPMarkupObject* pObject);
	static void AFX_CDECL RegisterClasses();

protected:
	virtual void GetScriptNamedItemNames(CList<LPCWSTR, LPCWSTR>& names);
	virtual HRESULT GetScriptNamedItem(LPCWSTR lpszName, IUnknown **ppiunkItem);

public:
	HWND m_hContextWnd;

protected:
	COLORREF m_clrForeground;
	COLORREF m_clrImgColorKey;
	LOGFONT m_logFont;
	XTPMarkupSmoothingMode m_DefaultSmoothingMode;
	BOOL m_bDefaultTextSmoothing;

	CXTPMarkupInputElement* m_pMouseOver;
	CXTPMarkupInputElement* m_pMouseCapture;
	CXTPMarkupUIElement* m_pActiveElement;

	CXTPMarkupTypedSimpleStack<CXTPMarkupFont>* m_pFonts;

	CXTPMarkupEventHandlerMap* m_pHandlers;
	CXTPMarkupDelegateMap* m_pDelegates;

	CXTPMarkupKeyboardNavigation* m_pKeyboardNavigation;

	CXTPToolTipContext* m_pToolTipContext;
	BOOL m_bUseCustomToolTipPos;
	CPoint m_ptCustomToolTipPos;

	friend class CXTPMarkupFont;
	class CTooltipContext;
	long m_dwRef;

	CXTPImageManager* m_pImageManager;
	CXTPMarkupStaticExtension* m_pExtensionRoot;

	LPWSTR m_lpszScriptEngineLanguage;
	CXTPActiveScriptEngine* m_pScriptEngine;
	CList<LPWSTR, LPWSTR> m_ScriptCodeChunks;
	CXTPActiveScriptStdRuntime* m_pScriptEngineStdRuntime;

	BOOL m_bEnableGdiPlus;
	CXTPDpi* m_pDpi;
	BOOL m_bUpdatesFrozen;
	BOOL m_bEntireUpdatePended;
	CList<CXTPMarkupUIElement*, CXTPMarkupUIElement*> m_PendingUpdateList;

protected:
#ifdef _XTP_ACTIVEX
	DECLARE_INTERFACE_MAP()
#endif

	BEGIN_INTERFACE_PART(ActiveScriptSite, IActiveScriptSite)
		STDMETHOD(GetLCID)(LCID *plcid);
		STDMETHOD(GetItemInfo)(
			LPCOLESTR pstrName,
			DWORD dwReturnMask,
			IUnknown **ppiunkItem,
			ITypeInfo **ppti);
		STDMETHOD(GetDocVersionString)(BSTR *pbstrVersion);
		STDMETHOD(OnScriptTerminate)(
			const VARIANT *pvarResult,
			const EXCEPINFO *pexcepinfo);
		STDMETHOD(OnStateChange)(SCRIPTSTATE ssScriptState);
		STDMETHOD(OnScriptError)(IActiveScriptError *pScriptError);
		STDMETHOD(OnEnterScript)();
		STDMETHOD(OnLeaveScript)();

#ifndef _XTP_ACTIVEX
		CXTPMarkupContext* pThis;
#endif
	END_INTERFACE_PART(ActiveScriptSite);

#ifndef _XTP_ACTIVEX
	friend class XActiveScriptSite;
#endif

#ifdef _XTP_ACTIVEX
public:
	DECLARE_DISPATCH_MAP()

	DECLARE_OLETYPELIB_EX(CXTPMarkupContext);

#else
protected:
	friend class CXTPMarkupContextInternalDispatch;
	CCmdTarget* m_pInternalDispatch;
#endif

	CCmdTarget* AccessInternalDispatch();

protected:
	void		OleSetMethod( LPDISPATCH lpHandler, LPCOLESTR lpName);
	void		OleSetHandler(LPDISPATCH lpHandler);

	LPDISPATCH	OleCreateObject(LPCOLESTR lpName);
	LPDISPATCH	OleCreateSolidBrush(COLORREF clr);
	LPDISPATCH	OleCreateThickness(long nLeft, long nTop, long nRight, long nBottom);
	LPDISPATCH	OleParse(LPCOLESTR lpText);
	BOOL		OleIsDpiAware();
	void		OleSetDpiAware(BOOL bNewValue);
	LPDISPATCH	OleGetToolTipContext();
	void		OleSetIcons(LPDISPATCH lpDisp);
	LPDISPATCH	OleGetIcons();

	BSTR		OleValidateXML(LPCOLESTR lpText);

	void		OleFreezeUpdates();
	void		OleUnfreezeUpdates();

	long		OleGetDefaultSmoothingMode();
	void		OleSetDefaultSmoothingMode(long lMode);

	BOOL		OleGetDefaultTextSmoothing();
	void		OleSetDefaultTextSmoothing(BOOL bEnableTextSmoothing);

//}}AFX_CODEJOCK_PRIVATE

	friend class CXTPMarkupInputElement;
};

AFX_INLINE LOGFONT* CXTPMarkupContext::GetDefaultLogFont()
{
	return &m_logFont;
}

AFX_INLINE COLORREF CXTPMarkupContext::GetDefaultForeground()
{
	return m_clrForeground;
}

AFX_INLINE void CXTPMarkupContext::SetDefaultTextColor(COLORREF clr)
{
	m_clrForeground = clr;
}

AFX_INLINE void CXTPMarkupContext::SetDefaultFont(HFONT font)
{
	LOGFONT lf;
	GetObject(font, sizeof(LOGFONT), &lf);
	m_logFont = lf;
}

AFX_INLINE void CXTPMarkupContext::SetDefaultFont(LOGFONT *pLogFont)
{
	m_logFont = *pLogFont;
}

AFX_INLINE XTPMarkupSmoothingMode CXTPMarkupContext::GetDefaultSmoothingMode() const
{
	return m_DefaultSmoothingMode;
}

AFX_INLINE XTPMarkupSmoothingMode CXTPMarkupContext::SetDefaultSmoothingMode(XTPMarkupSmoothingMode smoothingMode)
{
	XTPMarkupSmoothingMode oldMode = m_DefaultSmoothingMode;
	m_DefaultSmoothingMode = smoothingMode;
	return oldMode;
}

AFX_INLINE BOOL CXTPMarkupContext::GetDefaultTextSmoothing() const
{
	return m_bDefaultTextSmoothing;
}

AFX_INLINE BOOL CXTPMarkupContext::SetDefaultTextSmoothing(BOOL bEnableTextSmoothing)
{
	BOOL bOldState = m_bDefaultTextSmoothing;
	m_bDefaultTextSmoothing = bEnableTextSmoothing;
	return bOldState;
}

AFX_INLINE void CXTPMarkupContext::SetImageColorKey(COLORREF crColorKey)
{
	m_clrImgColorKey = crColorKey;
}

AFX_INLINE CXTPMarkupKeyboardNavigation* CXTPMarkupContext::GetKeyboardNavigation() const
{
	return m_pKeyboardNavigation;
}

AFX_INLINE CXTPMarkupInputElement* CXTPMarkupContext::GetMouseCapture() const
{
	return m_pMouseCapture;
}

AFX_INLINE BOOL CXTPMarkupContext::IsGdiPlusEnabled() const
{
	return m_bEnableGdiPlus;
}

AFX_INLINE void CXTPMarkupContext::EnableGdiPlus(BOOL bEnable)
{
	m_bEnableGdiPlus = bEnable;
}

AFX_INLINE CXTPToolTipContext* CXTPMarkupContext::GetToolTipContext() const
{
	return m_pToolTipContext;
}

AFX_INLINE BOOL CXTPMarkupContext::GetUseCustomToolTipPosition() const
{
	return m_bUseCustomToolTipPos;
}
	
AFX_INLINE void CXTPMarkupContext::SetUseCustomToolTipPosition(BOOL bUse, CPoint ptPos /*= CPoint(0, 0)*/)
{
	m_bUseCustomToolTipPos = bUse;
	m_ptCustomToolTipPos = ptPos;
}

AFX_INLINE CPoint CXTPMarkupContext::GetCustomToolTipPosition() const
{
	return m_ptCustomToolTipPos;
}

AFX_INLINE BOOL CXTPMarkupContext::IsDpiAware() const
{
	return NULL != m_pDpi;
}

AFX_INLINE void CXTPMarkupContext::SetDpiAware(BOOL bSet /*= TRUE*/)
{
	SetDpiAware(bSet, HWND_DESKTOP);
}

#define MARKUP_CREATE(object_class, context) ((object_class*)context->CreateMarkupObject(MARKUP_TYPE(object_class)))

#include "Common/Base/Diagnostic/XTPEnableNoisyWarnings.h"
#endif // !defined(__XTPMARKUPCONTEXT_H__)
