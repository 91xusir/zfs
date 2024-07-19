// XTPChartOpenGLTransformationDeviceCommand.h
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
#if !defined(__XTPCHARTOPENGLTRANSFORMATIONDEVICECOMMAND_H__)
#define __XTPCHARTOPENGLTRANSFORMATIONDEVICECOMMAND_H__

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "Common/Base/Diagnostic/XTPDisableNoisyWarnings.h"

template<class T>
class CXTPMatrix;

class CXTPChart3dRotation;

class _XTP_EXT_CLASS CXTPChartOpenGLSaveMatrixDeviceCommand : public CXTPChart3dDeviceCommand
{
public:
	CXTPChartOpenGLSaveMatrixDeviceCommand();

public:
	virtual void BeforeExecute(CXTPChartDeviceContext* pDC);
	virtual void AfterExecute(CXTPChartDeviceContext* pDC);
};

class _XTP_EXT_CLASS CXTPChartOpenGLLoadIdentityDeviceCommand : public CXTPChart3dDeviceCommand
{
public:
	CXTPChartOpenGLLoadIdentityDeviceCommand();

public:
	virtual void ExecuteOverride(CXTPChartDeviceContext* pDC);
};

class _XTP_EXT_CLASS CXTPChartOpenGLMultiplyMatrixDeviceCommand : public CXTPChart3dDeviceCommand
{
public:
	CXTPChartOpenGLMultiplyMatrixDeviceCommand(const CXTPMatrix<double>& matrix, BOOL bPushMatrix);
	~CXTPChartOpenGLMultiplyMatrixDeviceCommand();

public:
	virtual void BeforeExecute(CXTPChartDeviceContext* pDC);
	virtual void ExecuteOverride(CXTPChartDeviceContext* pDC);
	virtual void AfterExecute(CXTPChartDeviceContext* pDC);

private:
	CXTPMatrix<double>* m_pMatrix;
	BOOL m_bPushMatrix;
};

//===========================================================================
// Summary:
//     This class abstracts a rotation device command, which does rotation
//     transformations. This is a kind of CXTPChartDeviceCommand.
// Remarks:
//===========================================================================
class _XTP_EXT_CLASS CXTPChartOpenGLRotateDeviceCommand : public CXTPChart3dRotateDeviceCommand
{
public:
	//-----------------------------------------------------------------------
	// Summary:
	//     Constructs a CXTPChartOpenGLRotateDeviceCommand object.
	// Parameters:
	//     fAngle - The angle of rotation.
	//-----------------------------------------------------------------------
	CXTPChartOpenGLRotateDeviceCommand(const CXTPChart3dRotation& rotation);

public:
	//-----------------------------------------------------------------------
	// Summary:
	//     Call this function to do the actual drawing.
	// Parameters:
	//     pDC - A pointer to the chart device context.
	// Remarks:
	//-----------------------------------------------------------------------
	virtual void ExecuteOverride(CXTPChartDeviceContext* pDC);
};

//===========================================================================
// Summary:
//     This class abstracts a rotation device command, which does translation
//     on vertices.This is a kind of CXTPChartDeviceCommand.
// Remarks:
//===========================================================================
class _XTP_EXT_CLASS CXTPChartOpenGLTranslateDeviceCommand : public CXTPChart3dTranslateDeviceCommand
{
public:
	//-----------------------------------------------------------------------
	// Summary:
	//     Constructs a CXTPChartOpenGLTranslateDeviceCommand object.
	// Parameters:
	//     dx - The change in X direction
	//     dy - The change in Y direction
	//-----------------------------------------------------------------------
	CXTPChartOpenGLTranslateDeviceCommand(double dx, double dy, double dz);

public:
	//-----------------------------------------------------------------------
	// Summary:
	//     Call this function to do the actual drawing.
	// Parameters:
	//     pDC - A pointer to the chart device context.
	// Remarks:
	//-----------------------------------------------------------------------
	virtual void ExecuteOverride(CXTPChartDeviceContext* pDC);
};


#include "Common/Base/Diagnostic/XTPEnableNoisyWarnings.h"
#endif //#if !defined(__XTPCHARTOPENGLTRANSFORMATIONDEVICECOMMAND_H__)
//}}AFX_CODEJOCK_PRIVATE
