// XTPCommandBarList.h
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
#if !defined(__XTPCOMMANDBARLIST_H__)
#define __XTPCOMMANDBARLIST_H__
//}}AFX_CODEJOCK_PRIVATE

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Common/Base/Diagnostic/XTPDisableNoisyWarnings.h"

class CXTPCommandBar;
class CXTPCommandBars;

//===========================================================================
// Summary:
//     CXTPCommandBarList is a CList derived class. It is used as a collection
//     of CXTPCommandBar classes.
//===========================================================================
class _XTP_EXT_CLASS CXTPCommandBarList : public CXTPCmdTarget
{
public:

	//-----------------------------------------------------------------------
	// Summary:
	//     Constructs a CXTPCommandBarList object
	// Parameters:
	//     pCommandBars - Parent CXTPCommandBars pointer
	//-----------------------------------------------------------------------
	CXTPCommandBarList(CXTPCommandBars* pCommandBars)
	{
		m_pCommandBars = pCommandBars;
	}

	//-----------------------------------------------------------------------
	// Summary:
	//     Destroys a CXTPCommandBarList object, handles cleanup and
	//     deallocation
	//-----------------------------------------------------------------------
	virtual ~CXTPCommandBarList();

public:
	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member to find the CommandBar with the specified
	//     identifier.
	// Parameters:
	//     nBarID - Identifier CommandBar to find.
	// Returns:
	//     A pointer to a CXTPCommandBar object.
	//-----------------------------------------------------------------------
	CXTPCommandBar* FindCommandBar(UINT nBarID) const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Returns Commandbars object attached to list
	// Returns:
	//     Pointer to CXTPCommandBars parent object
	// See Also:
	//     CXTPCommandBars, CXTPCommandBar
	//-----------------------------------------------------------------------
	CXTPCommandBars* GetCommandBars() const { return m_pCommandBars; }

	//-----------------------------------------------------------------------
	// Summary:
	//     Returns CommandBar in list by its index
	// Parameters:
	//     nIndex - Index of the commandbar to return
	// Returns:
	//     CommandBar in list by its index
	// See Also: GetCount
	//-----------------------------------------------------------------------
	CXTPCommandBar* GetAt(int nIndex) const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Return count of commanbars in list
	// Returns:
	//     Commanbars count
	// See Also: GetAt
	//-----------------------------------------------------------------------
	int GetCount() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this method to add new commandbar to the list
	// Parameters:
	//      pCommandBar - CommandBar to be added
	// Returns:
	//      Pointer to just added CommandBar
	// See Also: GetAt, GetCount
	//-----------------------------------------------------------------------
	virtual CXTPCommandBar* Add(CXTPCommandBar* pCommandBar);

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this method to check if CommandBar exists in the list
	// Parameters:
	//      pCommandBar - CommandBar to test
	// Returns:
	//      TRUE if CommandBar exists in the list
	//-----------------------------------------------------------------------
	BOOL Lookup(CXTPCommandBar* pCommandBar) const;

	//-------------------------------------------------------------------------
	// Summary:
	//     Call this method to clean the list
	// See Also: Remove
	//-------------------------------------------------------------------------
	void RemoveAll();

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this method to remove CommandBar from the list
	// Parameters:
	//     pCommandBar - CommandBar to be removed
	// See Also: RemoveAll
	//-----------------------------------------------------------------------
	void Remove(CXTPCommandBar* pCommandBar);

protected:
	CXTPCommandBars* m_pCommandBars;            // Parent CXTPCommandBars object
	CArray<CXTPCommandBar*, CXTPCommandBar*> m_arrBars;         // Array of CommandBars
};


#include "Common/Base/Diagnostic/XTPEnableNoisyWarnings.h"
#endif // !defined(__XTPCOMMANDBARLIST_H__)
