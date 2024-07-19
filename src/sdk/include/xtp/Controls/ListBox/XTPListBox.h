// XTPListBox.h interface for the CXTPListBox class.
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
#if !defined(__XTPLISTBOX_H__)
#define __XTPLISTBOX_H__
//}}AFX_CODEJOCK_PRIVATE

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "Common/Base/Diagnostic/XTPDisableNoisyWarnings.h"

//===========================================================================
// Summary: Constants used by the CXTPListBox control to determine the current
//          theme style that has been set.
//===========================================================================
enum XTPListBoxStyle
{
	xtpListBoxStandard,        // Default theme.
	xtpListBoxOfficeXP,        // Office XP style theme.
	xtpListBoxOffice2007,      // Office 2007 style theme.
	xtpListBoxOffice2013,      // Office 2013 style theme.
	xtpListBoxOffice2016 = xtpListBoxOffice2013, // Office 2016 style theme.
	xtpListBoxVisualStudio2015 // Visual Studio 2015 style theme.
};

class CXTPListBox;

//===========================================================================
// Summary:
//     CXTPListBoxTheme is the base class used by CXTPListBox themes to draw
//     the list-box control.
//===========================================================================
class _XTP_EXT_CLASS CXTPListBoxTheme
{
public:
	
	//-----------------------------------------------------------------------
	// Summary: This method is called to draw the CXTPListBox object.
	// Input:   pDC      - Pointer to a valid device context.
	//          pListBox - Pointer to a valid CXTPListBox object.
	//          lpDIS    - A long pointer to a DRAWITEMSTRUCT structure. The 
	//                     structure contains information about the item to be 
	//                     drawn and the type of drawing required. 
	//-----------------------------------------------------------------------
	virtual void DrawItem(CDC *pDC, CXTPListBox *pListBox, LPDRAWITEMSTRUCT lpDIS) = 0;

	//-----------------------------------------------------------------------
	// Summary: This method is called to draw text for the CXTPListBox object.
	// Input:   pDC      - Pointer to a valid device context.
	//          rcText   - Size of the area to draw the text to.
	//          pListBox - Pointer to a valid CXTPListBox object.
	//          lpDIS    - A long pointer to a DRAWITEMSTRUCT structure. The 
	//                     structure contains information about the item to be 
	//                     drawn and the type of drawing required. 
	//-----------------------------------------------------------------------
	virtual void DrawItemText(CDC *pDC, CRect rcText, CXTPListBox *pListBox, LPDRAWITEMSTRUCT lpDIS);

	//-----------------------------------------------------------------------
	// Summary: Called to update colors and metrics used to draw the 
	//          CXTPListBox object.
	//-----------------------------------------------------------------------
	virtual void RefreshMetrics() = 0;

	//-----------------------------------------------------------------------
	// Summary: Called to retreive the background color for the list-box.
	// Returns: RGB value representing the background color.
	//-----------------------------------------------------------------------
	virtual COLORREF GetBackColor();

	//-----------------------------------------------------------------------
	// Summary: Call this member function to draw the list-control borders.
	// Input:   pDC - Points to a valid device context.
	//          rc  - Size of the area ot draw.
	// Returns: TRUE if non-client border drawing is handled, otherwise FALSE.
	//-----------------------------------------------------------------------
	virtual BOOL DrawNcBorders(CDC* pDC, CRect rc);
};

//===========================================================================
// Summary:
//     CXTPListBox is a CListBox derived class. CXTPListBox extends the standard
//     list-box control to enable flicker free drawing.
//===========================================================================
class _XTP_EXT_CLASS CXTPListBox : public CListBox
{
	DECLARE_DYNAMIC(CXTPListBox)

public:
	//-----------------------------------------------------------------------
	// Summary: Constructs a CXTPListBox object
	//-----------------------------------------------------------------------
	CXTPListBox();

	//-----------------------------------------------------------------------
	// Summary: Destroys a CXTPListBox object, handles cleanup and deallocation
	//-----------------------------------------------------------------------
	virtual ~CXTPListBox();

public:
	//-----------------------------------------------------------------------
	// Summary: Call this member function to initialize the list-box.  This method
	//          should be called directly after creating or sub-classing the control.
	// Input:   bAutoFont - True to enable automatic font initialization.
	//-----------------------------------------------------------------------
	virtual void Initialize(bool bAutoFont = true);

	//-----------------------------------------------------------------------
	// Summary: This method is called to get background color of listbox
	//-----------------------------------------------------------------------
	virtual COLORREF GetBackColor();

	//-----------------------------------------------------------------------
	// Summary:  Call this method to set list style
	// Input:    style - A XTPListBoxStyle constant used to specify which theme
	//                   to be used by the control.  This mehthod is here only
	//                   for backward compatibilty with older versions and may
	//                   be removed with a future release.  Plese use the 
	//                   SetTheme() method instead.
	// See Also: XTPListBoxStyle, GetListStyle, SetTheme
	//-----------------------------------------------------------------------
	void SetListStyle(XTPListBoxStyle style);

	//-----------------------------------------------------------------------
	// Summary: Call this method to get style of the ListBox
	// Returns: XTPListBoxStyle value contains style of listbox
	// See Also: XTPListBoxStyle, SetListStyle
	//-----------------------------------------------------------------------
	XTPListBoxStyle GetListStyle() const;

	//-----------------------------------------------------------------------
	// Summary: This member function sets the drawing theme for the control.
	// Input:   nTheme - A XTPControlTheme constant used to specify which theme
	//                   to be used by the control.
	/// See Also: XTPControlTheme
	//-----------------------------------------------------------------------
	void SetTheme(XTPControlTheme nTheme);

	//-----------------------------------------------------------------------
	// Summary: Call this member function to determine if the list-box control
	//          has input focus.
	// Returns: TRUE if the list-box has focus, otherwise FALSE.
	//-----------------------------------------------------------------------
	BOOL HasFocus() const;

protected:

	//-----------------------------------------------------------------------
	// Summary: This method determines the list-box item nearest the point 
	//          specified in pt when there is at least one item in the list-box.
	// Input:   pt - Specifies the point for which to find the nearest item, 
	//               specified relative to the upper-left corner of the client 
	//               area of the list box.
	//          bOutside - Specifies the reference to a BOOL variable which will 
	//                     be set to TRUE if pt is outside the client area of 
	//                     the list box, FALSE if pt is inside the client area 
	//                     of the list box.
	//         nIndex - Recieves the index of the item that was nearest the point
	//                  specified in pt if successful, otherwise -1.
	//-----------------------------------------------------------------------
	BOOL TryItemFromPoint(CPoint pt, BOOL& bOutside, int& nIndex) const;

//{{AFX_CODEJOCK_PRIVATE
	DECLARE_MESSAGE_MAP()

	BOOL m_bPreSubclassInit;

	//{{AFX_VIRTUAL(CXTPListBox)
	virtual void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);
	virtual void MeasureItem(LPMEASUREITEMSTRUCT lpMeasureItemStruct);
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	virtual void PreSubclassWindow();
	virtual void Init();
	//}}AFX_VIRTUAL

	//{{AFX_MSG(CXTPListBox)
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnPaint();
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg LRESULT OnPrintClient(WPARAM wParam, LPARAM lParam);
	afx_msg void OnMouseLeave();
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnSetFocus(CWnd* pOldWnd);
	afx_msg void OnKillFocus(CWnd* pNewWnd);
	afx_msg LRESULT OnSetTheme(WPARAM wParam, LPARAM lParam);
	afx_msg void OnNcPaint();
	//}}AFX_MSG
//}}AFX_CODEJOCK_PRIVATE

public:
	CXTPListBoxTheme* m_pTheme;        // Pointer to the currently active theme.
	XTPControlTheme   m_theme;         // List box theme
	int               m_nItemHeight;   // Height of the items.
	int               m_nTextPadding;  // Text padding.
	int               m_nHotItem;      // Hot item index.
};



#include "Common/Base/Diagnostic/XTPEnableNoisyWarnings.h"
#endif // #if !defined(__XTPLISTBOX_H__)
