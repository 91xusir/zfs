// XTPChartOpenGLDeviceContext.h
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
#if !defined(__XTPCHARTOPENGLDEVICECONTEXT_H__)
#define __XTPCHARTOPENGLDEVICECONTEXT_H__

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "Common/Base/Diagnostic/XTPDisableNoisyWarnings.h"

template<class T>
class CXTPMatrix;
class CXTPChartContainer;
class CXTPChartDeviceCommand;

class _XTP_EXT_CLASS CXTPChartOpenGLDeviceContext : public CXTPChart3dDeviceContext
{
	DECLARE_DYNAMIC(CXTPChartOpenGLDeviceContext);

public:
	//-----------------------------------------------------------------------
	// Summary:
	//     Constructs a CXTPChartOpenGLDeviceContext object.
	// Parameters:
	//     hDC - Handle to the windows device context.
	// Remarks:
	//-----------------------------------------------------------------------
	CXTPChartOpenGLDeviceContext(CXTPChartContainer* pContainer, HDC hDC, CRect rcBounds);

	//-------------------------------------------------------------------------
	// Summary:
	//     Destroys a CXTPChartOpenGLDeviceContext object, handles cleanup
	//-------------------------------------------------------------------------
	~CXTPChartOpenGLDeviceContext();

public:
	//-------------------------------------------------------------------------
	// Summary:
	//     Call this function to trigger the drawing.
	// Parameters:
	//     pCommand - A pointer to chart device command object.
	//-------------------------------------------------------------------------
	virtual void Execute(CXTPChartDeviceCommand* pCommand);

	virtual BOOL Initialize();

	//-------------------------------------------------------------------------
	// Summary:
	//     Call this function to get the measurements of texts rendered in a
	//     device context using a specific font.
	// Parameters:
	//     pText - The string to be measured.
	//     pFont - Pointer to chart font object.
	//-------------------------------------------------------------------------
	virtual CXTPChartSizeF MeasureString(const CXTPChartString* pText, CXTPChartFont* pFont);

	virtual CXTPChartSizeF MeasureMarkupElement(CXTPMarkupUIElement* pMarkupUIElement, CXTPChartFont* pFont);
	virtual void Rotate(const CXTPChart3dRotation& rotation);
	virtual BOOL Project(const CXTPPoint3d& pt3d, CPoint& pt2d, double* pdWinZ = NULL);
	virtual BOOL Project(const CXTPChart3dBox& box, CRect& rc);
	virtual BOOL Unproject(const CPoint& pt2d, CXTPPoint3d& pt3d, double dWinZ = 0.);
	virtual void Transform(CXTPPoint3d& v, const CXTPMatrix<double>& matrix);
	virtual void Transform(CXTPChart3dBox& box, const CXTPMatrix<double>& matrix);
	virtual void Transform(CXTPPoint3d& v, 
		const CXTPPoint3d* pTranslation = NULL,
		const CXTPChart3dRotation* pRotation = NULL);
	virtual void Transform(CXTPChart3dBox& box, 
		const CXTPPoint3d* pTranslation = NULL,
		const CXTPChart3dRotation* pRotation = NULL);
	virtual BOOL DepthTest(CPoint point, double& dWinZ);

private:
	BOOL InitializePixelFormat();
	void SetOpenGLParameters();
	BOOL OnPreRender();
	void OnPostRender();
	double ComputeAntialiasingJitterFactor() const;
	void Execute3dCommandTreeAntialiased(CXTPChartDeviceCommand* pCommand);
	void Execute3dCommandTree(CXTPChartDeviceCommand* pCommand);
	void Execute2dCommandTree(CXTPChartDeviceCommand* pCommand);
	CXTPMatrix<double>* BuildTransformationMatrix(
		const CXTPPoint3d* pTranslation = NULL,
		const CXTPChart3dRotation* pRotation = NULL);

protected:
	HGLRC m_hGLContext;
	int m_nExecuteNesting;
	CList<CXTPChartDeviceCommand*, CXTPChartDeviceCommand*> m_2dCommands;
	BOOL m_bHave2dCommands;
	BOOL m_bIn2dCommandExecutionMode;
	CXTPChartDeviceContext* m_p2dDC;
};


#include "Common/Base/Diagnostic/XTPEnableNoisyWarnings.h"
#endif //#if !defined(__XTPCHARTOPENGLDEVICECONTEXT_H__)
//}}AFX_CODEJOCK_PRIVATE
