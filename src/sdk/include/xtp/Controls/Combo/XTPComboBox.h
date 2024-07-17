// XTPComboBox.h interface for the CXTPComboBox class.
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
#if !defined(__XTPCOMBOBOX_H__)
#define __XTPCOMBOBOX_H__
//}}AFX_CODEJOCK_PRIVATE

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "Common/Base/Diagnostic/XTPDisableNoisyWarnings.h"

class CXTPComboBoxTheme;
class CXTPComboBoxEditCtrl;

//===========================================================================
// Summary: The CXTPComboBox class provides the functionality of a themed 
//          Widnows combo-box. 
//===========================================================================
class _XTP_EXT_CLASS CXTPComboBox : public CComboBox
{
	DECLARE_DYNAMIC(CXTPComboBox)

public:

	//-----------------------------------------------------------------------
	// Summary: Constructs a CXTPComboBox object
	//-----------------------------------------------------------------------
	CXTPComboBox();

	//-----------------------------------------------------------------------
	// Summary: Destroys a CXTPComboBox object, handles cleanup and deallocation
	//-----------------------------------------------------------------------
	virtual ~CXTPComboBox();

	//-----------------------------------------------------------------------
	// Summary:  Call this member to switch the visual theme of the control.
	// Input:    nTheme - New visual theme. Can be any of the values listed 
	//                    in the Remarks section.
	// Remarks:  nTheme can be one of the theme IDs specified by XTPControlTheme.
	// See Also: XTPControlTheme
	//-----------------------------------------------------------------------
	BOOL SetTheme(XTPControlTheme nTheme);

	//-----------------------------------------------------------------------
	// Summary: This member function is called to set the theme for the combo-box
	//          control using the theme specified by pTheme.
	// Input:   pTheme - Points to a valid CXTPComboBox theme object.
	//-----------------------------------------------------------------------
	virtual void SetTheme(CXTPComboBoxTheme *pTheme);

	//-----------------------------------------------------------------------
	// Summary: Call this member funciton to retrieve a pointer to the current
	//          theme used by the combo-box.
	// Returns: A CXTPComboBoxTheme pointer or NULL if no theme is set.
	//-----------------------------------------------------------------------
	CXTPComboBoxTheme* GetTheme();

	//-----------------------------------------------------------------------
	// Summary: Call this member funciton to retrieve a pointer to the current
	//          theme used by the combo-box.
	// Returns: A CXTPComboBoxTheme pointer or NULL if no theme is set.
	//-----------------------------------------------------------------------
	const CXTPComboBoxTheme* GetTheme() const;

	//-----------------------------------------------------------------------
	// Summary: Redraws the button.
	// Input:   bUpdateWindow - TRUE to notify the parent window.
	//-----------------------------------------------------------------------
	void RedrawComboBox(BOOL bUpdateWindow = FALSE);

	//-----------------------------------------------------------------------
	// Summary: Gets the flat style appearance of the combo-box.
	// Remarks: The control will appear flat until the mouse pointer moves 
	//          over it, at which point it appears three-dimensional.
	// Returns: TRUE if the flat style is used, FALSE otherwise.
	//-----------------------------------------------------------------------
	BOOL GetFlatStyle() const;

	//-----------------------------------------------------------------------
	// Summary: Sets the flat style appearance of the button control.
	// Input:   bFlatStyle - TRUE to use the flat style, FALSE otherwise.
	// Remarks: The control will appear flat until the mouse pointer moves 
	//          over it, at which point it appears three-dimensional.
	//-----------------------------------------------------------------------
	void SetFlatStyle(BOOL bFlatStyle = TRUE);

	//-----------------------------------------------------------------------
	// Summary: Determines if the control is drawn using visual styles\SkinFramework, 
	//          if supported.
	// Remarks: This method enables Windows XP visual styles for the control. 
	//          This control will draw with visual styles if the control and 
	//          the operating system supports it. Visual styles and SkinFramwork 
	//          skins are specifications for the appearance of controls.  
	//          GetUseVisualStyle determines whether to use the currently set 
	//          Windows XP visual style.  If using SkinFramwork, it determines 
	//          whether to use the currently set style of the SkinFramework.
	// Returns: TRUE if the currently set Windows XP visual style will be used 
	//          to theme the control.  If using SkinFramwork, the currently set 
	//          style of the SkinFramework will be used to skin the control.
	//          FALSE if the currently set appearance\style in the OS will be used.
	//-----------------------------------------------------------------------
	BOOL GetUseVisualStyle() const;

	//-----------------------------------------------------------------------
	// Summary: Enables Windows XP visual styles for the control.
	// Remarks: This method enables Windows XP visual styles for the control. 
	//          This control will draw with visual styles if the control and 
	//          the operating system supports it.  Visual styles and SkinFramwork 
	//          skins are specifications for the appearance of controls.  
	//          GetUseVisualStyle determines whether to use the currently set 
	//          Windows XP visual style.  If using SkinFramwork, it determines 
	//          whether to use the currently set style of the SkinFramework.
	// Input:   bUseVisualStyle - TRUE if the currently set Windows XP visual 
	//                            style will be used to theme the control.  
	//                            If using SkinFramwork, the currently set style
	//                            of the SkinFramework will be used to skin the 
	//                            control. FALSE if the currently set 
	//                            appearance\style in the OS will be used.
	//-----------------------------------------------------------------------
	void SetUseVisualStyle(BOOL bUseVisualStyle = TRUE);

	//-----------------------------------------------------------------------
	// Summary: This member function enables or disables auto completion.
	// Input:   bEnable - TRUE to enable auto completion, otherwise FALSE.
	//-----------------------------------------------------------------------
	void EnableAutoCompletion(BOOL bEnable = TRUE);

	//-----------------------------------------------------------------------
	// Summary      : Call this member function to set the background color
	//                for the CXTPComboBox control.
	// Parameters   : crBack - RGB value representing background color.
	// See Also     : SetTextColor, GetBackColor, GetTextColor
	//-----------------------------------------------------------------------
	virtual void SetBackColor(COLORREF crBack);

	//-----------------------------------------------------------------------
	// Summary      : Call this member function to retrieve the background color
	//                for the CXTPComboBox.
	// Returns      : An RGB color value representing the background color for
	//                the CXTPComboBox.
	// See Also     : SetTextColor, SetBackColor, GetTextColor
	//-----------------------------------------------------------------------
	COLORREF GetBackColor() const;

	//-----------------------------------------------------------------------
	// Summary      : Call this member function to set the text color
	//                for the CXTPComboBox control.
	// Parameters   : crText - RGB value representing text color.
	// See Also     : SetBackColor, GetBackColor, GetTextColor
	//-----------------------------------------------------------------------
	virtual void SetTextColor(COLORREF crText);

	//-----------------------------------------------------------------------
	// Summary      : Call this member function to retrieve the text color
	//                for the CXTPComboBox.
	// Returns      : An RGB color value representing the text color for
	//                the CXTPComboBox.
	// See Also     : SetTextColor, SetBackColor, GetBackColor
	//-----------------------------------------------------------------------
	COLORREF GetTextColor() const;

	//-----------------------------------------------------------------------
	// Summary: This member is called to update color, text and other visual 
	//          elements of the control.
	//-----------------------------------------------------------------------
	void RefreshMetrics();

	//-----------------------------------------------------------------------
	// Summary: Call this member function to determine if the combo-box has
	//          input focus.
	// Returns: TRUE if the combo-box has input focus, otherwise FALSE.
	//-----------------------------------------------------------------------
	BOOL IsFocused() const;

	//-----------------------------------------------------------------------
	// Summary: Call this member function to determine if the combo-box state
	//          is highilghted, meaning the mouse cursor is currently hovering
	//          the control.
	// Returns: TRUE if the combo-box state is highlighted, otherwise FALSE.
	//-----------------------------------------------------------------------
	BOOL IsHighlighted() const;

	//-----------------------------------------------------------------------
	// Summary: Call this member funciton to retreive a handle to the client 
	//          background brush.
	// Input:   pDC - Points to a valid device context.
	// Returns: A HBRUSH handle representing the client background brush.
	//-----------------------------------------------------------------------
	HBRUSH GetClientBrush(CDC* pDC);

	//-----------------------------------------------------------------------
	// Summary: Call this member function to retreve the size of the combo-box
	//          in client coordinates.
	// Returns: A CRect object representing the client size for the combo-box.
	//-----------------------------------------------------------------------
	CRect GetComboBoxRect() const;

	//-----------------------------------------------------------------------
	// Summary: This member fuction is called ot set the highlighted state for
	//          the combo-box.
	// Input:   bHot - TRUE to set the combo-box state to highlighted.
	//-----------------------------------------------------------------------
	void SetHighlighted(BOOL bHot);

protected:

	//-----------------------------------------------------------------------
	// Summary: Called by the framework to redraw the focused frame for the
	//          combo-box.
	//-----------------------------------------------------------------------
	void RedrawFocusedFrame();

	//-----------------------------------------------------------------------
	// Summary: Called by the framework to determine if the combo-box needs to
	//          be redrawn.
	// Returns: TRUE if the combo-box needs to be redrawn.
	//-----------------------------------------------------------------------
	BOOL NeedRedraw();

//{{AFX_CODEJOCK_PRIVATE

	//{{AFX_VIRTUAL(CXTPComboBox)
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	virtual void PreSubclassWindow();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	//}}AFX_VIRTUAL

	virtual void Init();

	//{{AFX_MSG(CXTPComboBox)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnPaint();
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnKillFocus(CWnd* pNewWnd);
	afx_msg void OnSetFocus(CWnd* pOldWnd);
	afx_msg void OnSysColorChange();
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg HBRUSH CtlColor(CDC* pDC, UINT nCtlColor);//reflect message
	afx_msg BOOL OnEditUpdate();
	afx_msg BOOL OnCloseUp();
	afx_msg BOOL OnDropDown();
	afx_msg void OnMouseLeave();
	//}}AFX_MSG
	afx_msg LRESULT OnSetTheme(WPARAM wParam, LPARAM lParam);

	DECLARE_MESSAGE_MAP()

//}}AFX_CODEJOCK_PRIVATE

protected:

	BOOL m_bPreSubclassInit; // TRUE if the control has been subclassed.
	BOOL m_bUseVisualStyle;  // TRUE to use Windows themes.
	BOOL m_bFlatStyle;       // TRUE to draw the control using flat styles, this is mostly deprecated, use SetTheme() instead.
	BOOL m_bHighlighted;     // TRUE when the combo-box is highlighted.
	BOOL m_bFocused;         // TRUE when the combo-box has input focus.
	BOOL m_bAutoComp;        // TRUE when the combo-box is using the auto-complete feature.
	BOOL m_bDisableAC;       // TRUE to disable auto-complete temporarily.

	CXTPComboBoxTheme*    m_pTheme;   // Pointer to the current theme object.
	CXTPComboBoxEditCtrl* m_pWndEdit; // Pointer to the edit-control for the combo box.

private:
	
	friend class CXTPComboBoxEditCtrl;
};

/////////////////////////////////////////////////////////////////////////////

AFX_INLINE BOOL CXTPComboBox::GetUseVisualStyle() const {
	return m_bUseVisualStyle;
}
AFX_INLINE BOOL CXTPComboBox::IsFocused() const {
	return m_bFocused;
}
AFX_INLINE BOOL CXTPComboBox::IsHighlighted() const {
	return m_bHighlighted;
}
AFX_INLINE BOOL CXTPComboBox::GetFlatStyle() const{
	return m_bFlatStyle;
}
AFX_INLINE void CXTPComboBox::SetFlatStyle(BOOL bFlatStyle/* = TRUE*/) {
	m_bFlatStyle = bFlatStyle; if (m_hWnd) Invalidate(FALSE);
}
AFX_INLINE void CXTPComboBox::EnableAutoCompletion(BOOL bEnable/*=TRUE*/) {
	m_bAutoComp = bEnable;
}


#include "Common/Base/Diagnostic/XTPEnableNoisyWarnings.h"
#endif // #if !defined(__XTPCOMBOBOX_H__)
