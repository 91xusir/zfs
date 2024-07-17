// XTPReportRecordItemDateTime.h: interface for the CXTPReportRecordItemDateTime class.
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
#if !defined(__XTPREPORTRECORDITEMDATETIME_H__)
#define __XTPREPORTRECORDITEMDATETIME_H__
//}}AFX_CODEJOCK_PRIVATE

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Common/Base/Diagnostic/XTPDisableNoisyWarnings.h"


//===========================================================================
// Summary:
//     Represents a time cell and inherits basic functionality
//     from CXTPReportRecordItem class.
// Remarks:
//     You create a CXTPReportRecordItemDateTime record item simply by calling a constructor with one
//     COleDateTime parameter
// See Also: CXTPReportRecordItem
//===========================================================================
class _XTP_EXT_CLASS CXTPReportRecordItemDateTime : public CXTPReportRecordItem
{
	DECLARE_SERIAL(CXTPReportRecordItemDateTime)
public:

	//-----------------------------------------------------------------------
	// Summary:
	//     Creates record date/time item.
	// Parameters:
	//     odtValue - Initial date/time item value.
	//-----------------------------------------------------------------------
	CXTPReportRecordItemDateTime(COleDateTime odtValue = (DATE)0);

	//-----------------------------------------------------------------------
	// Summary:
	//     Returns a text caption of this item.
	// Parameters:
	//     pColumn - Corresponded column of the item.
	// Remarks:
	//     Overwrites parents' member function.
	// Returns:
	//     Text caption of the item.
	//-----------------------------------------------------------------------
	virtual CString GetCaption(CXTPReportColumn* pColumn);

	//-----------------------------------------------------------------------
	// Summary:
	//     Compares this item with the provided one.
	// Parameters:
	//  pColumn - Pointer to a CXTPReportColumn object.
	//     pItem - The item provided for comparing with current.
	// Remarks:
	//     Overwrites parents' member function
	// Returns:
	//     Zero if pItem is equal with current;
	//     Less than zero if less than pItem;
	//     Greater than zero if greater than pItem.
	//-----------------------------------------------------------------------
	virtual int Compare(CXTPReportColumn* pColumn, CXTPReportRecordItem* pItem);

	//-----------------------------------------------------------------------
	// Summary:
	//     Returns group caption native value.
	// Parameters:
	//     pColumn - point to the column
	// Returns:
	//     Native group caption value.
	//-----------------------------------------------------------------------
	virtual COleVariant GetGroupCaptionValue(CXTPReportColumn* pColumn);

	//-----------------------------------------------------------------------
	// Summary:
	//     Compares group captions.
	// Parameters:
	//     pColumn   - Pointer to the column for which the captions has to be compared
	//     pGroupRow - Pointer to the group row compare caption to
	// Returns:
	//     Zero if the items' values are identical,
	//     < 0 if this item value is less than provided,
	//     or > 0 if this item value is greater than provided.
	//-----------------------------------------------------------------------
	virtual int CompareGroupCaption(CXTPReportColumn* pColumn, CXTPReportGroupRow* pGroupRow);

	//-----------------------------------------------------------------------
	// Summary:
	//     This method is called when value of in-place edit control changed
	// Parameters:
	//     pItemArgs - Pointer to structure with items arguments.
	//     szText    - New in-place edit text.
	//-----------------------------------------------------------------------
	virtual void OnEditChanged(XTP_REPORTRECORDITEM_ARGS* pItemArgs, LPCTSTR szText);

	//-----------------------------------------------------------------------
	// Summary:
	//     Returns the item value in its original format.
	// Returns:
	//     Item date-time value.
	//-----------------------------------------------------------------------
	COleDateTime GetValue();

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this method to set value of the item
	// Parameters:
	//     odtValue - New item value
	//-----------------------------------------------------------------------
	void SetValue(COleDateTime odtValue);

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member function to Store/Load a report record item
	//     using the specified data object.
	// Parameters:
	//     pPX - Source or destination CXTPPropExchange data object reference.
	//-----------------------------------------------------------------------
	virtual void DoPropExchange(CXTPPropExchange* pPX);

private:
	static int CompareValues(const COleDateTime& dt1, const COleDateTime& dt2);

protected:
	COleDateTime m_odtValue;    // Cell value.
};


AFX_INLINE COleDateTime CXTPReportRecordItemDateTime::GetValue()
{
	return m_odtValue;
}

AFX_INLINE void CXTPReportRecordItemDateTime::SetValue(COleDateTime odtValue)
{
	m_odtValue = odtValue;
}


#include "Common/Base/Diagnostic/XTPEnableNoisyWarnings.h"
#endif // !defined(__XTPREPORTRECORDITEMDATETIME_H__)
