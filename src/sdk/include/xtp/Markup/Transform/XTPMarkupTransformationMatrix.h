// XTPMarkupTransformationMatrix.h: declaration of CXTPMarkupTransformationMatrix class.
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
#if !defined(__XTPMARKUPTRANSFORMATIONMATRIX_H__)
#define __XTPMARKUPTRANSFORMATIONMATRIX_H__
//}}AFX_CODEJOCK_PRIVATE

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Common/Base/Diagnostic/XTPDisableNoisyWarnings.h"

class CXTPMarkupTransformationMatrix;

//===========================================================================
// Summary: An abstract class for device context transformation matrix.
//===========================================================================
class _XTP_EXT_CLASS CXTPMarkupTransformationMatrix : public CObject
{
	DECLARE_DYNAMIC(CXTPMarkupTransformationMatrix);

protected:
	CXTPMarkupTransformationMatrix();
public:
	virtual ~CXTPMarkupTransformationMatrix();

	//-----------------------------------------------------------------------
	// Summary: Clones matrix object.
	// Returns: A point to a cloned matrix. A caller is responsible for
	//          deleting object himself.
	//-----------------------------------------------------------------------
	virtual CXTPMarkupTransformationMatrix* Clone() const = 0;

	//-----------------------------------------------------------------------
	// Summary: Adds offset to the current matrix.
	// Parameters: point - An offset to add.
	//-----------------------------------------------------------------------
	virtual void Offset(const POINT& offset);

	//-----------------------------------------------------------------------
	// Summary: Checks if a point is inside a rectangle with transformation applied.
	// Parameters:
	//     rect - A rectangle in relative coordinates to test in.
	//     absoluteOffset - Absolute offset of the rectangle.
	//     point - an absolute point to test.
	// Returns: TRUE if the point his the rectangle specified.
	//-----------------------------------------------------------------------
	virtual BOOL PtInRect(
		const RECT& rect,
		const POINT& absoluteOffset,
		const POINT& point) const;

	//-----------------------------------------------------------------------
	// Summary: Computes bounding rectangle after transformation.
	// Parameters:
	//     rect - A rectangle in relative coordinates to transform.
	//     absoluteOffset - Absolute offset of the rectangle.
	// Returns: Result bounding rectangle
	//-----------------------------------------------------------------------
	virtual CRect GetBoundRect(
		const RECT& rect,
		const POINT& absoluteOffset) const;
};

AFX_INLINE CXTPMarkupTransformationMatrix::CXTPMarkupTransformationMatrix()
{
}

AFX_INLINE CXTPMarkupTransformationMatrix::~CXTPMarkupTransformationMatrix()
{
}


#include "Common/Base/Diagnostic/XTPEnableNoisyWarnings.h"
#endif // !defined(__XTPMARKUPTRANSFORMATIONMATRIX_H__)
