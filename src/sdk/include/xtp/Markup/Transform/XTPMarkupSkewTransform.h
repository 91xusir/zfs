// XTPMarkupSkewTransform.h: interface for the CXTPMarkupSkewTransform class.
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
#if !defined(__XTPMARKUPSKEWTRANSFORM_H__)
#define __XTPMARKUPSKEWTRANSFORM_H__
//}}AFX_CODEJOCK_PRIVATE

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Common/Base/Diagnostic/XTPDisableNoisyWarnings.h"

class CXTPMarkupTransformationMatrix;
class CXTPMarkupDependencyProperty;

//===========================================================================
// Summary: Default SkewTransform implementation.
//===========================================================================
class _XTP_EXT_CLASS CXTPMarkupSkewTransform : public CXTPMarkupTransform
{
	DECLARE_MARKUPCLASS(CXTPMarkupSkewTransform);

public:
	//-------------------------------------------------------------------------
	// Summary: Obtains angle X value.
	// Returns: Angle X value.
	//-------------------------------------------------------------------------
	double GetAngleX() const;

	//-------------------------------------------------------------------------
	// Summary: Sets new angle X value.
	// Parameters: angleX - Angle X value.
	//-------------------------------------------------------------------------
	void SetAngleX(double angleX);

	//-------------------------------------------------------------------------
	// Summary: Obtains angle Y value.
	// Returns: Angle Y value.
	//-------------------------------------------------------------------------
	double GetAngleY() const;

	//-------------------------------------------------------------------------
	// Summary: Sets new angle Y value.
	// Parameters: angleY - Angle Y value.
	//-------------------------------------------------------------------------
	void SetAngleY(double angleY);

	//-------------------------------------------------------------------------
	// Summary: Obtains center X value.
	// Returns: Center X value.
	//-------------------------------------------------------------------------
	double GetCenterX() const;

	//-------------------------------------------------------------------------
	// Summary: Sets new center X value.
	// Parameters: centerX - Center X value.
	//-------------------------------------------------------------------------
	void SetCenterX(double centerX);

	//-------------------------------------------------------------------------
	// Summary: Obtains center Y value.
	// Returns: Center Y value.
	//-------------------------------------------------------------------------
	double GetCenterY() const;

	//-------------------------------------------------------------------------
	// Summary: Sets new center Y value.
	// Parameters: centerY - Center Y value.
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
	afx_msg double OleGetAngleX();
	afx_msg void OleSetAngleX(double angleX);
	afx_msg double OleGetAngleY();
	afx_msg void OleSetAngleY(double angleY);
	afx_msg double OleGetCenterX();
	afx_msg void OleSetCenterX(double centerX);
	afx_msg double OleGetCenterY();
	afx_msg void OleSetCenterY(double centerY);

public:
	static CXTPMarkupDependencyProperty *m_pPropertyAngleX;  // AngleX property
	static CXTPMarkupDependencyProperty *m_pPropertyAngleY;  // AngleY property
	static CXTPMarkupDependencyProperty *m_pPropertyCenterX; // CenterX property
	static CXTPMarkupDependencyProperty *m_pPropertyCenterY; // CenterY property
};


#include "Common/Base/Diagnostic/XTPEnableNoisyWarnings.h"
#endif // !defined(__XTPMARKUPSKEWTRANSFORM_H__)
