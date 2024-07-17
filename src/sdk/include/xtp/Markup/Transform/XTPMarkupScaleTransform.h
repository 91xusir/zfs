// XTPMarkupScaleTransform.h: interface for the CXTPMarkupScaleTransform class.
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
#if !defined(__XTPMARKUPSCALETRANSFORM_H__)
#define __XTPMARKUPSCALETRANSFORM_H__
//}}AFX_CODEJOCK_PRIVATE

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Common/Base/Diagnostic/XTPDisableNoisyWarnings.h"

class CXTPMarkupTransformationMatrix;
class CXTPMarkupDependencyProperty;

//===========================================================================
// Summary: Default ScaleTransform implementation.
//===========================================================================
class _XTP_EXT_CLASS CXTPMarkupScaleTransform : public CXTPMarkupTransform
{
	DECLARE_MARKUPCLASS(CXTPMarkupScaleTransform);

public:
	//-------------------------------------------------------------------------
	// Summary: Obtains scale X value.
	// Returns: Scale X value.
	//-------------------------------------------------------------------------
	double GetScaleX() const;

	//-------------------------------------------------------------------------
	// Summary: Sets new scale X value.
	// Paramters: scaleX - Scale X value.
	//-------------------------------------------------------------------------
	void SetScaleX(double scaleX);

	//-------------------------------------------------------------------------
	// Summary: Obtains scale Y value.
	// Returns: Scale Y value.
	//-------------------------------------------------------------------------
	double GetScaleY() const;

	//-------------------------------------------------------------------------
	// Summary: Sets new scale Y value.
	// Paramters: scaleY - Scale Y value.
	//-------------------------------------------------------------------------
	void SetScaleY(double scaleY);

	//-------------------------------------------------------------------------
	// Summary: Obtains center X value.
	// Returns: Center X value.
	//-------------------------------------------------------------------------
	double GetCenterX() const;

	//-------------------------------------------------------------------------
	// Summary: Sets new center X value.
	// Paramters: centerX - Center X value.
	//-------------------------------------------------------------------------
	void SetCenterX(double centerX);

	//-------------------------------------------------------------------------
	// Summary: Obtains center Y value.
	// Returns: Center Y value.
	//-------------------------------------------------------------------------
	double GetCenterY() const;

	//-------------------------------------------------------------------------
	// Summary: Sets new center Y value.
	// Paramters: centerY - Center Y value.
	//-------------------------------------------------------------------------
	void SetCenterY(double centerY);

	//-------------------------------------------------------------------------
	// Summary: Do nothing in default implementation.
	// Parameters: pMatrix - A pointer to markup transformation matrix which
	//                       is to be transformed.
	// Returns: NULL in default implementation.
	//-------------------------------------------------------------------------
	virtual CXTPMarkupTransformationMatrix* ApplyTransform(const CXTPMarkupTransformationMatrix* pMatrix);

private:
	DECLARE_DISPATCH_MAP();
	afx_msg double OleGetScaleX();
	afx_msg void OleSetScaleX(double scaleX);
	afx_msg double OleGetScaleY();
	afx_msg void OleSetScaleY(double scaleY);
	afx_msg double OleGetCenterX();
	afx_msg void OleSetCenterX(double centerX);
	afx_msg double OleGetCenterY();
	afx_msg void OleSetCenterY(double centerY);

public:
	static CXTPMarkupDependencyProperty *m_pPropertyScaleX;  // ScaleX property
	static CXTPMarkupDependencyProperty *m_pPropertyScaleY;  // ScaleY property
	static CXTPMarkupDependencyProperty *m_pPropertyCenterX; // CenterX property
	static CXTPMarkupDependencyProperty *m_pPropertyCenterY; // CenterY property
};


#include "Common/Base/Diagnostic/XTPEnableNoisyWarnings.h"
#endif // !defined(__XTPMARKUPSCALETRANSFORM_H__)
