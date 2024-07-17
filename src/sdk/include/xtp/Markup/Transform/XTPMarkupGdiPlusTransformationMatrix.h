// XTPMarkupGdiPlusTransformationMatrix.h: declaration of CXTPMarkupGdiPlusTransformationMatrix class.
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
#if !defined(__XTPMARKUPGDIPLUSTRANSFORMATIONMATRIX_H__)
#define __XTPMARKUPGDIPLUSTRANSFORMATIONMATRIX_H__
//}}AFX_CODEJOCK_PRIVATE

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Common/Base/Diagnostic/XTPDisableNoisyWarnings.h"

class CXTPGdiPlus;

//===========================================================================
// Summary: Encapsulates GDI+ transformation matrix access.
//===========================================================================
class _XTP_EXT_CLASS CXTPMarkupGdiPlusTransformationMatrix : public CXTPMarkupTransformationMatrix
{
	DECLARE_DYNAMIC(CXTPMarkupGdiPlusTransformationMatrix);

public:

	//-------------------------------------------------------------------------
	// Summary: Constructs GDI+ markup matrix.
	// Parameters:
	//    pGdiPlus - A pointer to GDI+ API instance.
	//    pMatrix - A pointer to native GDI+ matrix. The wrapper takes an
	//              exclusive ownership of the native GDI+ matrix passed.
	//-------------------------------------------------------------------------
	CXTPMarkupGdiPlusTransformationMatrix(
		CXTPGdiPlus* pGdiPlus,
		GpMatrix* pMatrix);

	//-------------------------------------------------------------------------
	// Summary: Cleans up resources held.
	//-------------------------------------------------------------------------
	~CXTPMarkupGdiPlusTransformationMatrix();

	//-------------------------------------------------------------------------
	// Summary: Gives access to GDI+ API instance.
	// Returns: GDI+ API instance pointer.
	//-------------------------------------------------------------------------
	CXTPGdiPlus* GetGdiPlus();

	//-------------------------------------------------------------------------
	// Summary: Gives access to GDI+ API instance.
	// Returns: GDI+ API instance pointer.
	//-------------------------------------------------------------------------
	const CXTPGdiPlus* GetGdiPlus() const;

	//-------------------------------------------------------------------------
	// Summary: Gives access to the native GDI+ matrix instance.
	// Returns: Native GDI+ matrix instance pointer.
	//-------------------------------------------------------------------------
	GpMatrix* GetMatrix();

	//-------------------------------------------------------------------------
	// Summary: Gives access to the native GDI+ matrix instance.
	// Returns: Native GDI+ matrix instance pointer.
	//-------------------------------------------------------------------------
	const GpMatrix* GetMatrix() const;

public:
	//-----------------------------------------------------------------------
	// Summary: Clones matrix object.
	// Returns: A point to a cloned matrix. A caller is responsible for
	//          deleting object himself.
	//-----------------------------------------------------------------------
	virtual CXTPMarkupTransformationMatrix* Clone() const;

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

private:
	BOOL TransformRect(
		const RECT& rect,
		const POINT& absoluteOffset,
		PointF (&transformedRectPoints)[4]) const;

private:
	CXTPGdiPlus* m_pGdiPlus; // GDI+ API instance pointer.
	GpMatrix* m_pMatrix;     // Native GDI+ matrix instance pointer.
};

AFX_INLINE CXTPGdiPlus* CXTPMarkupGdiPlusTransformationMatrix::GetGdiPlus() {
	return m_pGdiPlus;
}

AFX_INLINE const CXTPGdiPlus* CXTPMarkupGdiPlusTransformationMatrix::GetGdiPlus() const {
	return m_pGdiPlus;
}

AFX_INLINE GpMatrix* CXTPMarkupGdiPlusTransformationMatrix::GetMatrix() {
	return m_pMatrix;
}

AFX_INLINE const GpMatrix* CXTPMarkupGdiPlusTransformationMatrix::GetMatrix() const {
	return m_pMatrix;
}


#include "Common/Base/Diagnostic/XTPEnableNoisyWarnings.h"
#endif // !defined(__XTPMARKUPGDIPLUSTRANSFORMATIONMATRIX_H__)
