// XTPMarkupGridLength.h: interface for the CXTPMarkupGridLength class.
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
#if !defined(__XTPMARKUPGRIDLENGTH_H__)
#define __XTPMARKUPGRIDLENGTH_H__
//}}AFX_CODEJOCK_PRIVATE

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Common/Base/Diagnostic/XTPDisableNoisyWarnings.h"

class CXTPMarkupObject;
class CXTPMarkupBuilder;

//===========================================================================
// Summary: CXTPMarkupGridLength is class defined length of the grid rows and columns
//===========================================================================
class _XTP_EXT_CLASS CXTPMarkupGridLength : public CXTPMarkupObject
{
	DECLARE_MARKUPCLASS(CXTPMarkupGridLength);
public:
	//===========================================================================
	// Summary: Unit type of the length
	//===========================================================================
	enum GridUnitType
	{
		unitTypeAuto,  // Automatically calculated length
		unitTypePixel, // Length in pixels
		unitTypeStar   // Length in percents
	};
public:
	//-----------------------------------------------------------------------
	// Summary:
	//     Constructs a CXTPMarkupGridLength object
	// Parameters:
	//     nValue - Length of the grid elements
	//      type - Type of the Length units
	//-----------------------------------------------------------------------
	CXTPMarkupGridLength(double nValue = 0, GridUnitType type = unitTypePixel);

	//-----------------------------------------------------------------------
	// Summary:
	//     Retrieves type of the length units
	// Returns:
	//     unitTypePixel if length calculated in pixels and unitTypeStar if its calculated in percents
	//-----------------------------------------------------------------------
	GridUnitType GetUnitType() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Retrieves value of the length
	// Returns:
	//     Length in pixels or percents.
	//-----------------------------------------------------------------------
	double GetValue() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Determines if length uses percent units
	// Returns:
	//     TRUE if length uses percent units
	//-----------------------------------------------------------------------
	BOOL IsStar() const {
		return m_type == unitTypeStar;
	}

	//-----------------------------------------------------------------------
	// Summary:
	//     Determines if length is Auto type
	// Returns:
	//     TRUE if type of the length is unitTypeAuto
	//-----------------------------------------------------------------------
	BOOL IsAuto() const {
		return m_type == unitTypeAuto;
	}

public:
	//-----------------------------------------------------------------------
	// Summary:
	//     Helper method used to convert String to Length
	// Parameters:
	//     pBuilder - Pointer to builder object
	//     pObject - String representation of the Length
	// Returns
	//     New CXTPMarkupGridLength object
	//-----------------------------------------------------------------------
	CXTPMarkupObject* ConvertFrom(CXTPMarkupBuilder* pBuilder, CXTPMarkupObject* pObject) const;

public:
	double m_nValue;     // Value
	GridUnitType m_type; // Unit type
};

AFX_INLINE CXTPMarkupGridLength::GridUnitType CXTPMarkupGridLength::GetUnitType() const
{
	return m_type;
};

AFX_INLINE double CXTPMarkupGridLength::GetValue() const
{
	return m_nValue;
}


#include "Common/Base/Diagnostic/XTPEnableNoisyWarnings.h"
#endif // !defined(__XTPMARKUPGRIDLENGTH_H__)
