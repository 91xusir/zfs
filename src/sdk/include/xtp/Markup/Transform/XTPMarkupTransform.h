// XTPMarkupTransform.h: interface for the CXTPMarkupTransform class.
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
#if !defined(__XTPMARKUPTRANSFORM_H__)
#define __XTPMARKUPTRANSFORM_H__
//}}AFX_CODEJOCK_PRIVATE

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Common/Base/Diagnostic/XTPDisableNoisyWarnings.h"

class CXTPMarkupTransformationMatrix;
class CXTPMarkupRenderTransform;
class CXTPMarkupDependencyProperty;
class CXTPMarkupObject;

//===========================================================================
// Summary: Base class for all markup transformation objects.
//===========================================================================
class _XTP_EXT_CLASS CXTPMarkupTransform : public CXTPMarkupObject
{
	DECLARE_MARKUPCLASS(CXTPMarkupTransform);

protected:
	//-------------------------------------------------------------------------
	// Summary: Prevents a class from being instantiated.
	//-------------------------------------------------------------------------
	CXTPMarkupTransform();

public:
	//-------------------------------------------------------------------------
	// Summary: Applies abstract transformation on a matrix passed.
	// Parameters: pMatrix - A pointer to markup transformation matrix which
	//                       is to be transformed.
	// Returns: A pointer to a transformed markup matrix or NULL if failed
	//          or not supported. It is a callee's responsibility to delete
	//          an object returned.
	//-------------------------------------------------------------------------
	virtual CXTPMarkupTransformationMatrix* ApplyTransform(const CXTPMarkupTransformationMatrix* pMatrix);

protected:
	virtual void OnPropertyChanged(
		CXTPMarkupDependencyProperty* pProperty,
		CXTPMarkupObject* pOldValue,
		CXTPMarkupObject* pNewValue);
	virtual void OnSetAsProperty(
		CXTPMarkupDependencyProperty* pProperty,
		CXTPMarkupObject* pTargetObject);

private:
	CXTPMarkupRenderTransform* m_pRenderTransform;
};


#include "Common/Base/Diagnostic/XTPEnableNoisyWarnings.h"
#endif // !defined(__XTPMARKUPTRANSFORM_H__)
