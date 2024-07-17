// XTPChartTransformationDeviceCommand.h
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
#if !defined(__XTPCHARTTRANSFORMATIONDEVICECOMMAND_H__)
#define __XTPCHARTTRANSFORMATIONDEVICECOMMAND_H__
//}}AFX_CODEJOCK_PRIVATE

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "Common/Base/Diagnostic/XTPDisableNoisyWarnings.h"


//===========================================================================
// Summary:
//      Base class implementation for 2D chart clipping device command.
//===========================================================================
class _XTP_EXT_CLASS CXTPChartClipDeviceCommand : public CXTPChartDeviceCommand
{
protected:
	//-----------------------------------------------------------------------
	// Summary:
	//     Constructs a CXTPChartClipDeviceCommand object.
	//-----------------------------------------------------------------------
	CXTPChartClipDeviceCommand(CRect rcClip);

protected:
	CRect m_rcClip;     //The clipping rectangle.
};

//===========================================================================
// Summary:
//      Base class implementation for 2D chart rotation device command.
//===========================================================================
class _XTP_EXT_CLASS CXTPChartRotateDeviceCommand  : public CXTPChartDeviceCommand
{
protected:
	//-----------------------------------------------------------------------
	// Summary:
	//     Constructs a CXTPChartRotateDeviceCommand object.
	// Parameters:
	//     fAngle - The angle of rotation.
	//-----------------------------------------------------------------------
	CXTPChartRotateDeviceCommand(float fAngle);
	//-----------------------------------------------------------------------
	// Summary:
	//     Constructs a CXTPChartRotateDeviceCommand object.
	// Parameters:
	//     fAngle        - The angle of rotation.
	//     rotateVector  - The vector to be rotated.
	//-----------------------------------------------------------------------
	CXTPChartRotateDeviceCommand(float fAngle, const CXTPPoint3d& rotateVector);

	//-----------------------------------------------------------------------
	// Summary:
	//      Performs recursive hit testing of the element at the specified point.
	// Parameters:
	//      point - Point at which an element to be found.
	//      pParent - Parent element pointer.
	// Returns:
	//      A pointer to the element found at the specified point or NULL
	//      if no element is found.
	//-----------------------------------------------------------------------
	CXTPChartElement* HitTest(CPoint point, CXTPChartElement* pParent) const;

protected:
	float m_fAngle;                //The angle of rotation.
	CXTPPoint3d m_ptRotateVector;  //The rotation vector.
};

//===========================================================================
// Summary:
//      Base class implementation for 3D chart rotation device command.
//===========================================================================
class _XTP_EXT_CLASS CXTPChart3dRotateDeviceCommand : public CXTPChart3dDeviceCommand
{
public:
	//-----------------------------------------------------------------------
	// Summary:
	//     Constructs a CXTPChart3dRotateDeviceCommand object.
	// Parameters:
	//     rotation - Rotation parameters.
	//-----------------------------------------------------------------------
	CXTPChart3dRotateDeviceCommand(const CXTPChart3dRotation& rotation);

	//-----------------------------------------------------------------------
	// Summary:
	//      Performs recursive hit testing of the element at the specified point.
	// Parameters:
	//      point - Point at which an element to be found.
	//      pParent - Parent element pointer.
	// Returns:
	//      A pointer to the element found at the specified point or NULL
	//      if no element is found.
	//-----------------------------------------------------------------------
	CXTPChartElement* HitTest(CPoint point, CXTPChartElement* pParent) const;

protected:
	CXTPChart3dRotation m_rotation; // Rotation parameters.
};

//===========================================================================
// Summary:
//      Base class implementation for 2D chart translation device command.
//===========================================================================
class _XTP_EXT_CLASS CXTPChartTranslateDeviceCommand : public CXTPChartDeviceCommand
{
public:
	//-----------------------------------------------------------------------
	// Summary:
	//     Constructs a CXTPChartTranslateDeviceCommand object.
	// Parameters:
	//     dx - The change in X direction
	//     dy - The change in Y direction
	//-----------------------------------------------------------------------
	CXTPChartTranslateDeviceCommand(double dx, double dy);

protected:
	double m_dx;    //The change in X direction.
	double m_dy;    //The change in Y direction.
};

//===========================================================================
// Summary:
//      Base class implementation for 3D chart translation device command.
//===========================================================================
class _XTP_EXT_CLASS CXTPChart3dTranslateDeviceCommand : public CXTPChart3dDeviceCommand
{
public:
	//-----------------------------------------------------------------------
	// Summary:
	//     Constructs a CXTPChartTranslateDeviceCommand object.
	// Parameters:
	//     dx - The change in X direction
	//     dy - The change in Y direction
	//     dz - The change in Z direction
	//-----------------------------------------------------------------------
	CXTPChart3dTranslateDeviceCommand(double dx, double dy, double dz);

protected:
	double m_dx;    //The change in X direction.
	double m_dy;    //The change in Y direction.
	double m_dz;    //The change in Z direction.
};


#include "Common/Base/Diagnostic/XTPEnableNoisyWarnings.h"
#endif //#if !defined(__XTPCHARTTRANSFORMATIONDEVICECOMMAND_H__)
