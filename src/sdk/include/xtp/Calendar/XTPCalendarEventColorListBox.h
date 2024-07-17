// XTPCalendarEventColorListBox.h : header file
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
#if !defined(__XTPCALENDAREVENTCOLORLISTBOX_H__)
#define __XTPCALENDAREVENTCOLORLISTBOX_H__
//}}AFX_CODEJOCK_PRIVATE

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Common/Base/Diagnostic/XTPDisableNoisyWarnings.h"

//===========================================================================
// Summary: This class is used by CXTPCalendarEventColorListBox to create an
//          inplace edit box for renaming event categories and labels.
//===========================================================================
class _XTP_EXT_CLASS CXTPCalendarEventColorEdit : public CEdit
{
public:
	// ------------------------------------------------------------------
	// Summary:    Constructs a CXTPCalendarEventColorEdit object used to
	//             edit calendar event categories and labels names.
	// Parameters: nIndex : Index of the list box item to edit.
	// ------------------------------------------------------------------
	CXTPCalendarEventColorEdit(int nIndex);

	// ------------------------------------------------------------------
	// Summary: Returns the index of the listbox item associated with this
	//          edit control.
	// ------------------------------------------------------------------
	int GetIndex() const;

protected:
	int m_nIndex; // Index of listbox item associated with this control.

//{{AFX_CODEJOCK_PRIVATE
	//{{AFX_VIRTUAL(CXTPCalendarEventColorEdit)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	//}}AFX_VIRTUAL

	//{{AFX_MSG(CXTPCalendarEventColorEdit)
	protected:
	afx_msg void OnKillFocus(CWnd* pNewWnd);
	//}}AFX_MSG
	
	DECLARE_MESSAGE_MAP()
//}}AFX_CODEJOCK_PRIVATE
};

AFX_INLINE int CXTPCalendarEventColorEdit::GetIndex() const {
	return m_nIndex;
}

//===========================================================================
// Summary: This class is display a list of Calendar Event Categories or
//          Labels that to be edited.
//===========================================================================
class _XTP_EXT_CLASS CXTPCalendarEventColorListBox : public CListBox
{
	//{{AFX_CODEJOCK_PRIVATE
	DECLARE_DYNAMIC(CXTPCalendarEventColorListBox)
	//}}AFX_CODEJOCK_PRIVATE

public:
	// ------------------------------------------------------------------
	// Summary: Constructs a CXTPCalendarEventColorListBox object used 
	//          to edit calendar event categories and labels.
	// ------------------------------------------------------------------
	CXTPCalendarEventColorListBox();

	// ------------------------------------------------------------------
	// Summary: Destructor handles cleanup and de-allocation.
	// ------------------------------------------------------------------
	virtual ~CXTPCalendarEventColorListBox();

	// ------------------------------------------------------------------
	// Summary:    This member function is called to initialize the listbox
	//             with calendar event category or list data.
	// Parameters: pData       : Points to a valid CXTPCalendarData object.
	//             pEvent      : Points to a valid CXTPCalendarEvent object.
	//             bCategories : TRUE if the list should display event 
	//                           categories, FALSE to display labels.
	// ------------------------------------------------------------------
	virtual void Init(CXTPCalendarData* pData, CXTPCalendarEvent* pEvent, BOOL bCategories);

	// ------------------------------------------------------------------
	// Summary: Called to begin editing of a listbox event category 
	//          or label item.
	// ------------------------------------------------------------------
	virtual void BeginEdit();

	// ------------------------------------------------------------------
	// Summary:    Called to end the editing of a listbox event category 
	//             or label item.
	// Parameters: bUpdate : TRUE to save edit changes, FALSE to discard.
	// ------------------------------------------------------------------
	virtual void EndEdit(BOOL bUpdate = TRUE);

	// ------------------------------------------------------------------
	// Summary:    Called to deleted a list box category or label item.
	// Parameters: nItem : Index of the item to delete.
	// ------------------------------------------------------------------
	virtual void DeleteItem(int nItem = LB_ERR);

	// ------------------------------------------------------------------
	// Summary: Called to add a new event category or label to the listbox.
	// Returns: An index of the newly added item if successful, otherwise
	//          returns LB_ERR.
	// ------------------------------------------------------------------
	virtual int AddItem();

	// ------------------------------------------------------------------
	// Summary:    Call this member function to set the color for a 
	//             calendar event category or label.
	// Parameters: crItem : RGB value representing new item color.
	//             nItem  : Index of the listbox item to set color for.
	// ------------------------------------------------------------------
	virtual void SetItemColor(COLORREF crItem, int nItem = LB_ERR);

	// ------------------------------------------------------------------
	// Summary:    Call this member function to retrieve the label object
	//             for the specified index.
	// Parameters: nIndex : Index of the item to get the label pointer for.
	// Returns:    A CXTPCalendarEventLabel object pointer if successful,
	//             otherwise returns NULL.
	// ------------------------------------------------------------------
	CXTPCalendarEventLabel* GetLabel(int nIndex);

	// ------------------------------------------------------------------
	// Summary:    Call this member function to retrieve the category object
	//             for the specified index.
	// Parameters: nIndex : Index of the item to get the category pointer for.
	// Returns:    A CXTPCalendarEventLabel object pointer if successful,
	//             otherwise returns NULL.
	// ------------------------------------------------------------------
	CXTPCalendarEventCategory* GetCategory(int nIndex);

	// ------------------------------------------------------------------
	// Summary: This member function is called to get a pointer to the
	//          edit control used for renaming calendar event categories
	//          and labels.
	// Returns: A pointer to a valid CXTPCalendarEventColorEdit if active,
	//          otherwise returns NULL.
	// ------------------------------------------------------------------
	CXTPCalendarEventColorEdit* GetEdit();

	// ------------------------------------------------------------------
	// Summary:    This member function is called to update the calendar
	//             event categories based on the current listbox state.
	// Parameters: pData : Points to a valid CXTPCalendarData object.
	// ------------------------------------------------------------------
	void UpdateCategories(CXTPCalendarData* pData);

	// ------------------------------------------------------------------
	// Summary:    This member function is called to update the calendar
	//             event labels based on the current listbox state.
	// Parameters: pData : Points to a valid CXTPCalendarData object.
	// ------------------------------------------------------------------
	void UpdateLabels(CXTPCalendarData* pData);

	// ------------------------------------------------------------------
	// Summary:    This member function is called to toggle the checked 
	//             state for the specified calendar event category.
	// Parameters: nItem : Index of the item to toggle check state for.
	// ------------------------------------------------------------------
	void ToggleCheck(int nItem);

	// ------------------------------------------------------------------
	// Summary:    This member function is called to set the modified 
	//             state for the listbox control.
	// Parameters: bModified : TRUE if the state of the list box has changed
	//                         and calendar data needs to be updated.
	// ------------------------------------------------------------------
	void SetModified(BOOL bModified);

	// ------------------------------------------------------------------
	// Summary: This member function is called to check the modified
	//          state of the listbox control.
	// Returns: TRUE if the listox state has been changed, otherwise 
	//          returns FALSE.
	// ------------------------------------------------------------------
	BOOL IsModified();

protected:
	BOOL                         m_bModified;   // TRUE if the listbox state has been changed.
	BOOL                         m_bCategories; // TRUE if the listbox should display calendar event categories.
	CSize                        m_sizeColor;   // Size in pixels of the color selection box.
	CXTPCalendarData*            m_pData;       // Points to a valid CXTPCalendarData object.
	CXTPCalendarEvent*           m_pEvent;      // Points to a valid CXTPCalendarEvent object.
	CXTPCalendarEventColorEdit*  m_pWndEdit;    // Points to a valid CXTPCalendarEventColorEdit object, only valid when a list box item is being edited.
	CXTPCalendarEventCategoryIDs m_arChecked;   // Array which store checked item IDs.

	// ------------------------------------------------------------------
	// Summary:    This member function is called to determine the size
	//             and location of a color rectangle for the specified
	//             item.
	// Parameters: rItem : Size and location of the listbox item used to
	//                     calculate the color rectangle.
	// Returns:    A CRect object representing the size and location of
	//             the color rectangle for the specified item.
	// ------------------------------------------------------------------
	CRect CalcColorRect(CRect rItem);

//{{AFX_CODEJOCK_PRIVATE
	//{{AFX_VIRTUAL(CXTPCalendarEventColorListBox)
	public:
	virtual void MeasureItem(LPMEASUREITEMSTRUCT lpMIS);
	virtual void DrawItem(LPDRAWITEMSTRUCT lpDIS);
	virtual void DeleteItem(LPDELETEITEMSTRUCT lpDIS);
	//}}AFX_VIRTUAL

	//{{AFX_MSG(CXTPCalendarEventColorListBox)
	protected:
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg HBRUSH CtlColor(CDC* pDC, UINT nCtlColor);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
//}}AFX_CODEJOCK_PRIVATE
};

/////////////////////////////////////////////////////////////////////////////

AFX_INLINE CXTPCalendarEventColorEdit* CXTPCalendarEventColorListBox::GetEdit() {
	return m_pWndEdit;
}
AFX_INLINE void CXTPCalendarEventColorListBox::SetModified(BOOL bModified) {
	m_bModified = bModified;
}
AFX_INLINE BOOL CXTPCalendarEventColorListBox::IsModified() {
	return m_bModified;
}

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.


#include "Common/Base/Diagnostic/XTPEnableNoisyWarnings.h"
#endif // !defined(__XTPCALENDAREVENTCOLORLISTBOX_H__)
