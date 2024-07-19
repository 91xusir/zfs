// XTPSpinButtonCtrl.h interface for the CXTPSpinButtonCtrl class.
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
#if !defined(__XTPSPINBUTTONCTRL_H__)
#define __XTPSPINBUTTONCTRL_H__
//}}AFX_CODEJOCK_PRIVATE

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "Common/Base/Diagnostic/XTPDisableNoisyWarnings.h"

class CXTPEdit;
class CXTPWinThemeWrapper;

//===========================================================================
// Summary: The CXTPSpinButtonCtrl class provides the functionality of a 
//          themed Windows spin button control.
//===========================================================================
class _XTP_EXT_CLASS CXTPSpinButtonCtrl : public CSpinButtonCtrl
{
	DECLARE_DYNAMIC(CXTPSpinButtonCtrl)

public:

	//-----------------------------------------------------------------------
	// Summary:
	//     Constructs a CXTPSpinButtonCtrl object
	//-----------------------------------------------------------------------
	CXTPSpinButtonCtrl();

	//-----------------------------------------------------------------------
	// Summary:
	//     Destroys a CXTPSpinButtonCtrl object, handles cleanup and deallocation
	//-----------------------------------------------------------------------
	~CXTPSpinButtonCtrl();

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member to switch the visual theme of the control.
	// Parameters:
	//     nTheme - New visual theme. Can be any of the values listed in the Remarks section.
	// Remarks:
	//     nStyle can be one of the following:
	//     * <b>xtpControlThemeDefault</b> Standard appearance style.
	//     * <b>xtpControlThemeFlat</b> Flat appearance style.
	//     * <b>xtpControlThemeUltraFlat</b> Ultra flat appearance style.
	//     * <b>xtpControlThemeOffice2000</b> Office 2000 appearance style.
	//     * <b>xtpControlThemeOfficeXP</b> Office XP appearance style.
	//     * <b>xtpControlThemeOffice2003</b> Office 2003 appearance style.
	//     * <b>xtpControlThemeResource</b> Office 2007 appearance style.
	//-----------------------------------------------------------------------
	void SetTheme(XTPControlTheme nTheme);

	//-----------------------------------------------------------------------
	// Summary:
	//     Determines if the control is drawn using visual styles\SkinFramework, if supported.
	// Remarks:
	//     This method enables Windows XP visual styles for the control. This
	//     Control will draw with visual styles if the control and the operating
	//     system supports it.
	//     Visual styles and SkinFramwork skins are specifications for the
	//     appearance of controls.  GetUseVisualStyle determines whether to
	//     use the currently set Windows XP visual style.  If using SkinFramwork,
	//     it determines whether to use the currently set style of the SkinFramework.
	// Returns:
	//     TRUE if the currently set Windows XP visual style will be used to
	//     theme the control.  If using SkinFramwork, the currently set style
	//     of the SkinFramework will be used to skin the control.
	//
	//     FALSE if the currently set appearance\style in the OS will be used.
	//-----------------------------------------------------------------------
	BOOL GetUseVisualStyle() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Enables Windows XP visual styles for the control.
	// Remarks:
	//     This method enables Windows XP visual styles for the control. This
	//     Control will draw with visual styles if the control and the operating
	//     system supports it.
	//     Visual styles and SkinFramwork skins are specifications for the
	//     appearance of controls.  GetUseVisualStyle determines whether to
	//     use the currently set Windows XP visual style.  If using SkinFramwork,
	//     it determines whether to use the currently set style of the SkinFramework.
	// Parameters:
	//     bUseVisualStyle - TRUE if the currently set Windows XP visual style will be used to
	//                       theme the control.  If using SkinFramwork, the currently set style
	//                       of the SkinFramework will be used to skin the control.
	//
	//                       FALSE if the currently set appearance\style in the OS will be used.
	//-----------------------------------------------------------------------
	void SetUseVisualStyle(BOOL bUseVisualStyle = TRUE);

protected:

	//-----------------------------------------------------------------------
	// Summary: Determines which spin button, if any, is at a specified position.
	// Input:   pt - Point to be tested.
	// Returns: The id of the spin button at the position specified by pt, can
	//          be either UD_HITNOWHERE, UD_HITDOWN or UD_HITUP.
	//-----------------------------------------------------------------------
	UINT HitTest(CPoint pt);

	//-----------------------------------------------------------------------
	// Summary: Called by the framework to render the spin button control.
	// Input:   pDC - Points to a valid device context.
	//-----------------------------------------------------------------------
	void OnDraw(CDC* pDC);

	//-----------------------------------------------------------------------
	// Summary: his member is called to update color, text and other 
	//          visual elements of the control.
	//-----------------------------------------------------------------------
	void RefreshMetrics();

	//-----------------------------------------------------------------------
	// Summary: Called to determine if the border is chared with a buddy.
	// Returns: TRUE if the borders are shared.
	//-----------------------------------------------------------------------
	BOOL HasSharedBorder() const;

	//-----------------------------------------------------------------------
	// Summary: Called to retrieve a pointer to the CXTPEdit buddy window.
	// Returns: A pointer to a CXTPEdit object if a buddy is available,
	//          otherwise NULL.
	//-----------------------------------------------------------------------
	CXTPEdit* GetEditBuddy() const;

	//-----------------------------------------------------------------------
	// Summary: Called by the framework to render Windows themes for the spin
	//          button control.
	// Input:   pDC - Points to a valid device context.
	//-----------------------------------------------------------------------
	void DrawVisualStyle(CDC* pDC);

	//-----------------------------------------------------------------------
	// Summary: Called by the framework to render resource driven themes.
	// Input:   pDC - Points to a valid device context.
	//-----------------------------------------------------------------------
	void DrawOffice2007(CDC* pDC);

	//-----------------------------------------------------------------------
	// Summary: Called by the framework to render flat style spin button themes.
	// Input:   pDC - Points to a valid device context.
	//-----------------------------------------------------------------------
	void DrawFlat(CDC* pDC);

	//-----------------------------------------------------------------------
	// Summary: Called by the framework to render the button portion of the 
	//          spin button control for flat style themes.
	// Input:   pDC - Points to a valid device context.
	//-----------------------------------------------------------------------
	void DrawFlatButton(CDC* pDC, UINT nButton);

	//-----------------------------------------------------------------------
	// Summary: Called by the framework to render the up and down arrows for
	//          the spin button control.
	// Input:   pDC - Points to a valid device context.
	//-----------------------------------------------------------------------
	void DrawArrowGlyph(CDC* pDC, CRect rcArrow, BOOL bHorz, BOOL bUpArrow, COLORREF clrArrow);

	//-----------------------------------------------------------------------
	// Summary:
	// Input:   pDC - Points to a valid device context.
	//-----------------------------------------------------------------------
	void DrawStandard(CDC* pDC);

	//-----------------------------------------------------------------------
	// Summary: Called by the framework to determine if the spin button control
	//          is vertically orientated.
	// Returns: TRUE if the spin button control orientation is vertical.
	//-----------------------------------------------------------------------
	BOOL IsVertical() const;

	//-----------------------------------------------------------------------
	// Summary: Called by the spin button control to determin the size of the
	//          spin button specified by nButton.
	// Input:   nButton - Id of the button to retrieve the size information for,
	//                    can be either UD_HITUP or UD_HITDOWN.
	// Returns:  A CRect value representing the size for the specified button.
	//-----------------------------------------------------------------------
	CRect GetButtonRect(int nButton);

	//-----------------------------------------------------------------------
	// Summary:
	//     This member function is called from OnInitDialog or OnInitialUpdate
	//     to initialize the control.
	//-----------------------------------------------------------------------
	void Init();

	//{{AFX_CODEJOCK_PRIVATE
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	virtual void PreSubclassWindow();
	virtual int OnCreate(LPCREATESTRUCT lpCreateStruct);

	//{{AFX_MSG(CXTPSpinButtonCtrl)
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnPaint();
	afx_msg void OnMouseLeave();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg LRESULT OnSetTheme(WPARAM wParam, LPARAM lParam);
	DECLARE_MESSAGE_MAP()

	//}}AFX_MSG
//}}AFX_CODEJOCK_PRIVATE

	UINT                 m_nHotButton;       // Id for the currently highlighted button, can be either UD_HITNOWHERE, UD_HITDOWN or UD_HITUP.
	UINT                 m_nPressedButton;   // Id for the currently pressed button, can be either UD_HITNOWHERE, UD_HITDOWN or UD_HITUP.
	BOOL                 m_bUseVisualStyle;  // TRUE to use Windows themes.
	BOOL                 m_bPreSubclassInit; // TRUE if the control has been subclassed.
	COLORREF             m_crBack;           // Spin button background color.
	COLORREF             m_crBackHilite;     // Spin button background highlight color.
	COLORREF             m_crBackPushed;     // Spin button background pushed color.
	COLORREF             m_crBackDisabled;   // Spin button disabled color.
	COLORREF             m_crBorder;         // Border color.
	COLORREF             m_crBorderHilite;   // Border highlight color.
	COLORREF             m_crBorderPushed;   // Border pushed color.
	COLORREF             m_crBorderDisabled; // Border disabled color.
	COLORREF             m_crArrowEnabled;   // Spin button arrow color.
	COLORREF             m_crArrowHilite;    // Spin button arrow highlight color.
	COLORREF             m_crArrowPushed;    // Spin button arrow pushed color.
	COLORREF             m_crArrowDisabled;  // Spin button arrow disabled color.
	XTPControlTheme      m_nTheme;           // Currently active theme ID.
	CXTPWinThemeWrapper* m_themeSpin;        // Windows theme API wrapper pointer.
};

/////////////////////////////////////////////////////////////////////////////

AFX_INLINE BOOL CXTPSpinButtonCtrl::GetUseVisualStyle() const {
	return m_bUseVisualStyle;
}


#include "Common/Base/Diagnostic/XTPEnableNoisyWarnings.h"
#endif // #if !defined(__XTPSPINBUTTONCTRL_H__)
