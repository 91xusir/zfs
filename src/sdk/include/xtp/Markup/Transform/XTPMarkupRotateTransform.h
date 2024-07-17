// XTPMarkupRotateTransform.h: interface for the CXTPMarkupRotateTransform class.
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
#if !defined(__XTPMARKUPROTATETRANSFORM_H__)
#define __XTPMARKUPROTATETRANSFORM_H__
//}}AFX_CODEJOCK_PRIVATE

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Common/Base/Diagnostic/XTPDisableNoisyWarnings.h"

class CXTPMarkupTransformationMatrix;
class CXTPMarkupDependencyProperty;

//===========================================================================
// Summary: Default RotateTransform implementation.
//===========================================================================
class _XTP_EXT_CLASS CXTPMarkupRotateTransform : public CXTPMarkupTransform
{
	DECLARE_MARKUPCLASS(CXTPMarkupRotateTransform);

public:
	//-------------------------------------------------------------------------
	// Summary: Obtains angle value.
	// Returns: Angle value.
	//-------------------------------------------------------------------------
	double GetAngle() const;

	//-------------------------------------------------------------------------
	// Summary: Sets new angle value.
	// Parameters: angle - Angle value.
	//-------------------------------------------------------------------------
	void SetAngle(double angle);

	//-------------------------------------------------------------------------
	// Summary: Obtains center X value.
	// Returns: Center X value.
	//-------------------------------------------------------------------------
	double GetCenterX() const;

	//-------------------------------------------------------------------------
	// Summary: Sets new center X value.
	// Paramteres: centerX - Center X value.
	//-------------------------------------------------------------------------
	void SetCenterX(double centerX);

	//-------------------------------------------------------------------------
	// Summary: Obtains center Y value.
	// Returns: Center Y value.
	//-------------------------------------------------------------------------
	double GetCenterY() const;

	//-------------------------------------------------------------------------
	// Summary: Sets new center Y value.
	// Returns: centerY - Center Y value.
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
	afx_msg double OleGetAngle();
	afx_msg void OleSetAngle(double angle);
	afx_msg double OleGetCenterX();
	afx_msg void OleSetCenterX(double centerX);
	afx_msg double OleGetCenterY();
	afx_msg void OleSetCenterY(double centerY);

public:
	static CXTPMarkupDependencyProperty *m_pPropertyAngle;   // Angle property
	static CXTPMarkupDependencyProperty *m_pPropertyCenterX; // CenterX property
	static CXTPMarkupDependencyProperty *m_pPropertyCenterY; // CenterY property
};


#include "Common/Base/Diagnostic/XTPEnableNoisyWarnings.h"
#endif // !defined(__XTPMARKUPROTATETRANSFORM_H__)
