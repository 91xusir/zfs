// XTPTabManagerNavigateButtons.h
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
#if !defined(__XTPTABMANAGERNAVIGATEBUTTONS_H__)
#define __XTPTABMANAGERNAVIGATEBUTTONS_H__
//}}AFX_CODEJOCK_PRIVATE

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Common/Base/Diagnostic/XTPDisableNoisyWarnings.h"

//===========================================================================
// Summary:
//     Navigate buttons array definition
//===========================================================================
class _XTP_EXT_CLASS CXTPTabManagerNavigateButtons
{

public:
	//-------------------------------------------------------------------------
	// Summary:
	//     Constructs a CXTPTabManagerNavigateButtons object.
	//-------------------------------------------------------------------------
	CXTPTabManagerNavigateButtons();

	//-------------------------------------------------------------------------
	// Summary:
	//     Destroys a CXTPTabManagerNavigateButtons object, handles cleanup and deallocation.
	//-------------------------------------------------------------------------
	virtual ~CXTPTabManagerNavigateButtons();

public:
	//-----------------------------------------------------------------------
	// Summary:
	//     Retrieves CXTPTabManagerNavigateButton by its index.
	// Parameters:
	//     nIndex - Zero-based index of the button.
	// Returns:
	//     Pointer to CXTPTabManagerNavigateButton if successful; NULL otherwise.
	//-----------------------------------------------------------------------
	CXTPTabManagerNavigateButton* GetAt(int nIndex) const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Retrieves CXTPTabManagerNavigateButton by its index.
	// Parameters:
	//     nIndex - Zero-based index of the button.
	// Returns:
	//     Pointer to CXTPTabManagerNavigateButton if successful; NULL otherwise.
	//-----------------------------------------------------------------------
	CXTPTabManagerNavigateButton* operator[] (int nIndex) const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Adds CXTPTabManagerNavigateButton to collection
	// Parameters:
	//     pButton - New button to be added
	// Returns:
	//     Index of button in collection
	//-----------------------------------------------------------------------
	int Add(CXTPTabManagerNavigateButton* pButton);

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this method to insert CXTPTabManagerNavigateButton to collection
	// Parameters:
	//     nIndex - Index to insert button to
	//     pButton - New button to be added
	//-----------------------------------------------------------------------
	void InsertAt(int nIndex, CXTPTabManagerNavigateButton* pButton);

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this method to remove CXTPTabManagerNavigateButton from collection
	// Parameters:
	//     nIndex - Index of button to remove
	//-----------------------------------------------------------------------
	void RemoveAt(int nIndex);

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this method to clean all buttons
	//-----------------------------------------------------------------------
	void RemoveAll();

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this method to get size of collection
	// Returns:
	//     Number of elements in collection
	//-----------------------------------------------------------------------
	int GetSize() const;


protected:
	CArray<CXTPTabManagerNavigateButton*, CXTPTabManagerNavigateButton*> m_arrButtons;  /// Collection of buttons
};

AFX_INLINE CXTPTabManagerNavigateButton* CXTPTabManagerNavigateButtons::GetAt(int nIndex) const
{
	return m_arrButtons.GetAt(nIndex);
}

AFX_INLINE int CXTPTabManagerNavigateButtons::Add(CXTPTabManagerNavigateButton *pButton)
{
	return (int)m_arrButtons.Add(pButton);
}

AFX_INLINE int CXTPTabManagerNavigateButtons::GetSize() const
{
	return (int)m_arrButtons.GetSize();
}

AFX_INLINE void CXTPTabManagerNavigateButtons::InsertAt(int nIndex, CXTPTabManagerNavigateButton *pButton)
{
	m_arrButtons.InsertAt(nIndex, pButton);
}

AFX_INLINE CXTPTabManagerNavigateButton* CXTPTabManagerNavigateButtons::operator[] (int nIndex) const
{
	return m_arrButtons.GetAt(nIndex);
}


#include "Common/Base/Diagnostic/XTPEnableNoisyWarnings.h"
#endif // !defined(__XTPTABMANAGERNAVIGATEBUTTONS_H__)
