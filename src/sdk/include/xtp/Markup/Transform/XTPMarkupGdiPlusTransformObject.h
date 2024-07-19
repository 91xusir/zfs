// XTPMarkupGdiPlusTransformObject.h: CXTPMarkupGdiPlusTransformObject template class implementation.
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

#if !defined(__XTPMARKUPGDIPLUSTRANSFORMOBJECT_H__)
#define __XTPMARKUPGDIPLUSTRANSFORMOBJECT_H__

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Common/Base/Diagnostic/XTPDisableNoisyWarnings.h"

class CXTPMarkupTransformationMatrix;
class CXTPGdiPlus;

//===========================================================================
// Summary: Implements GDI+ markup matrix access and delegates the actual
//          transformation to a derived class.
//===========================================================================
class _XTP_EXT_CLASS CXTPMarkupGdiPlusTransformObjectImpl
{
protected:
	//-------------------------------------------------------------------------
	// Summary: Prevents a class from being instantiated.
	//-------------------------------------------------------------------------
	CXTPMarkupGdiPlusTransformObjectImpl() { }

	//-------------------------------------------------------------------------
	// Summary: An abstract method that performs a native GDI+ matrix transformation.
	// Parameters:
	//    pGdiPlus - GDI+ API instance pointer.
	//    pMatrix - Native GDI+ matrix to transform instance pointer.
	// Returns: TRUE if succeeded.
	//-------------------------------------------------------------------------
	virtual BOOL TransformMatrix(const CXTPGdiPlus* pGdiPlus, GpMatrix* pMatrix) = 0;

	//-------------------------------------------------------------------------
	// Summary: Applies abstract transformation on a matrix passed.
	// Parameters: pMatrix - A pointer to markup transformation matrix which
	//                       is to be transformed.
	// Returns: A pointer to a transformed markup matrix or NULL if failed
	//          or not supported. It is a callee's responsibility to delete
	//          an object returned.
	//-------------------------------------------------------------------------
	CXTPMarkupTransformationMatrix* ApplyTransform(const CXTPMarkupTransformationMatrix* pMatrix);
};

//===========================================================================
// Summary: Base class for all GDI+ markup transformation objects.
// Parameters: BaseTransform - A target transformation class.
//===========================================================================
template<class BaseTransform>
class CXTPMarkupGdiPlusTransformObject
	: public BaseTransform
	, protected CXTPMarkupGdiPlusTransformObjectImpl
{
protected:
	//-------------------------------------------------------------------------
	// Summary: Prevents a class from being instantiated.
	//-------------------------------------------------------------------------
	CXTPMarkupGdiPlusTransformObject() { }

private:
	//-------------------------------------------------------------------------
	// Summary: Applies abstract transformation on a matrix passed.
	// Parameters: pMatrix - A pointer to markup transformation matrix which
	//                       is to be transformed.
	// Returns: A pointer to a transformed markup matrix or NULL if failed
	//          or not supported. It is a callee's responsibility to delete
	//          an object returned.
	//-------------------------------------------------------------------------
	virtual CXTPMarkupTransformationMatrix* ApplyTransform(const CXTPMarkupTransformationMatrix* pMatrix);
};

template<class BaseTransform>
AFX_INLINE CXTPMarkupTransformationMatrix* CXTPMarkupGdiPlusTransformObject<BaseTransform>
	::ApplyTransform(const CXTPMarkupTransformationMatrix* pMatrix)
{
	return CXTPMarkupGdiPlusTransformObjectImpl::ApplyTransform(pMatrix);
}


#include "Common/Base/Diagnostic/XTPEnableNoisyWarnings.h"
#endif // !defined(__XTPMARKUPGDIPLUSTRANSFORMOBJECT_H__)
