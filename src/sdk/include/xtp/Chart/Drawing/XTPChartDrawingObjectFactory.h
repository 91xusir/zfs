// XTPChartDrawingObjectFactory.h
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
#if !defined(__XTPCHARTDRAWINGOBJECTFACTORY_H__)
#define __XTPCHARTDRAWINGOBJECTFACTORY_H__
//}}AFX_CODEJOCK_PRIVATE

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "Common/Base/Diagnostic/XTPDisableNoisyWarnings.h"

/////////////////////////////////////////////////////////////////////////
// CXTPChartDrawingObjectFactory

template<class T>
class CXTPMatrix;

class CXTPChartDeviceContext;
class CXTPMarkupUIElement;
class CXTPChartContainer;
class CXTPChartElement;
class CXTPChartDeviceCommand;
class CXTPChart3dRotation;

//-----------------------------------------------------------------------
// Summary:
//      Provides a set of factory methods for creating drawing device commands
//      and a base class interface for abstract drawing object factory.
// See also:
//      CXTPChart2dDrawingObjectFactory, CXTPChart3dDrawingObjectFactory
//-----------------------------------------------------------------------
class _XTP_EXT_CLASS CXTPChartDrawingObjectFactory
{
protected:
	CXTPChartDrawingObjectFactory();

public:
	virtual ~CXTPChartDrawingObjectFactory();

	static CXTPChartDeviceContext* AFX_CDECL Create2dDeviceContext(CXTPChartContainer* pContainer, HDC hDC, CXTPChartDeviceContext* p3dDC = NULL);
	static CXTPChartDeviceContext* AFX_CDECL Create3dDeviceContext(CXTPChartContainer* pContainer, HDC hDC, const RECT& rcBounds);

	static CXTPChartDeviceCommand* AFX_CDECL Create2dHitTestElementCommand(CXTPChartElement* pElement);
	static CXTPChartDeviceCommand* AFX_CDECL Create2dHitTestElementCommand(CXTPChartElement* pElement, const CRect& rcBounds);
	static CXTPChartDeviceCommand* AFX_CDECL Create2dHitTestElementCommand(CXTPChartElement* pElement, const CXTPChartRectF& rcBounds);
	static CXTPChartDeviceCommand* AFX_CDECL Create2dBoundedCircleDeviceCommand(const CXTPChartPointF& center, double radius, const CXTPChartColor& color, int thickness);
	static CXTPChartDeviceCommand* AFX_CDECL Create2dSolidCircleDeviceCommand(const CXTPChartPointF& center, double radius, const CXTPChartColor& color);
	static CXTPChartDeviceCommand* AFX_CDECL Create2dHatchCircleDeviceCommand(const CXTPChartPointF& center, double radius, const CXTPChartColor& color, const CXTPChartColor& color2, XTPChartHatchStyle nStyle);
	static CXTPChartDeviceCommand* AFX_CDECL Create2dGradientCircleDeviceCommand(const CXTPChartPointF& center, double radius, const CXTPChartColor& color, const CXTPChartColor& color2, XTPChartLinearGradientMode nMode);
	static CXTPChartDeviceCommand* AFX_CDECL Create2dSolidLineDeviceCommand(const CXTPPoint2f& p1, const CXTPPoint2f& p2, const CXTPChartColor& color, int thickness);
	static CXTPChartDeviceCommand* AFX_CDECL Create3dSolidLineDeviceCommand(const CXTPPoint3d& p1, const CXTPPoint3d& p2, const CXTPChartColor& color, int thickness);
	static CXTPChartDeviceCommand* AFX_CDECL Create2dDashedLineDeviceCommand(const CXTPPoint2f& p1, const CXTPPoint2f& p2, const CXTPChartColor& color, int thickness, XTPChartDashStyle nDashStyle, const CXTPChartLineDashArray& dashArray);
	static CXTPChartDeviceCommand* AFX_CDECL Create2dSolidPolylineDeviceCommand(const CXTPChartPoints& p, const CXTPChartColor& color, int thickness);
	static CXTPChartDeviceCommand* AFX_CDECL Create2dSolidSplineDeviceCommand(const CXTPChartPoints& p, const CXTPChartColor& color, int thickness);
	static CXTPChartDeviceCommand* AFX_CDECL Create2dDashedSplineDeviceCommand(const CXTPChartPoints& p, const CXTPChartColor& color, int thickness, XTPChartDashStyle nDashStyle, const CXTPChartLineDashArray& dashArray);
	static CXTPChartDeviceCommand* AFX_CDECL Create2dDashedPolylineDeviceCommand(const CXTPChartPoints& p, const CXTPChartColor& color, int thickness, XTPChartDashStyle nDashStyle, const CXTPChartLineDashArray& dashArray);
	static CXTPChartDeviceCommand* AFX_CDECL Create2dSolidSplinePolygonDeviceCommand(const CXTPChartPoints& points, const CXTPChartColor& color, BOOL bTwoSides = FALSE);
	static CXTPChartDeviceCommand* AFX_CDECL Create2dBoundedPolygonDeviceCommand(const CXTPChartPoints& points, const CXTPChartColor& color, int thickness);
	static CXTPChartDeviceCommand* AFX_CDECL Create2dSolidPolygonDeviceCommand(const CXTPChartPoints& points, const CXTPChartColor& color);
	static CXTPChartDeviceCommand* AFX_CDECL Create2dHatchPolygonDeviceCommand(const CXTPChartPoints& points, const CXTPChartColor& color, const CXTPChartColor& color2, XTPChartHatchStyle nStyle);
	static CXTPChartDeviceCommand* AFX_CDECL Create2dGradientPolygonDeviceCommand(const CXTPChartPoints& points, const CXTPChartColor& color, const CXTPChartColor& color2, XTPChartLinearGradientMode nMode);
	static CXTPChartDeviceCommand* AFX_CDECL Create2dPolygonAntialiasingDeviceCommand(BOOL bAntiAlias = TRUE);
	static CXTPChartDeviceCommand* AFX_CDECL Create3dShadeModelDeviceCommand(BOOL bAntiAlias = TRUE);
	static CXTPChartDeviceCommand* AFX_CDECL Create2dInnerBorderDeviceCommand(const CXTPChartRectF& rect, const CXTPChartColor& color, int thickness);
	static CXTPChartDeviceCommand* AFX_CDECL Create2dBoundedRectangleDeviceCommand(const CXTPChartRectF& rect, const CXTPChartColor& color, int thickness);
	static CXTPChartDeviceCommand* AFX_CDECL Create2dSolidRectangleDeviceCommand(const CXTPChartRectF& rect, const CXTPChartColor& color);
	static CXTPChartDeviceCommand* AFX_CDECL Create2dGradientRectangleDeviceCommand(const CXTPChartRectF& rect, const CXTPChartColor& color, const CXTPChartColor& color2, XTPChartLinearGradientMode nMode);
	static CXTPChartDeviceCommand* AFX_CDECL Create2dHatchRectangleDeviceCommand(const CXTPChartRectF& rect, const CXTPChartColor& color, const CXTPChartColor& color2, XTPChartHatchStyle nStyle);
	static CXTPChartDeviceCommand* AFX_CDECL Create2dTextAntialiasingDeviceCommand(BOOL bAntialias = TRUE);
	static CXTPChartDeviceCommand* AFX_CDECL Create2dBoundedTextDeviceCommand(const CXTPChartString& strText, CXTPChartFont* pFont, const CXTPChartColor& color, const CXTPChartRectF& rectangle);
	static CXTPChartDeviceCommand* AFX_CDECL Create2dMarkupElementDeviceCommand(CXTPMarkupUIElement* pMarkupUIElement, CXTPChartFont* pFont, const CXTPChartColor& color, const CXTPChartRectF& rectangle);
	static CXTPChartDeviceCommand* AFX_CDECL Create2dSaveStateDeviceCommand();
	static CXTPChartDeviceCommand* AFX_CDECL Create3dSaveMatrixDeviceCommand();
	static CXTPChartDeviceCommand* AFX_CDECL Create3dMultiplyMatrixDeviceCommand(const CXTPMatrix<double>& matrix, BOOL bPushMatrix);
	static CXTPChartDeviceCommand* AFX_CDECL Create3dLoadIdentityDeviceCommand();
	static CXTPChartDeviceCommand* AFX_CDECL Create2dClipDeviceCommand(CRect rcClip);
	static CXTPChartDeviceCommand* AFX_CDECL Create2dClipRegionDeviceCommand(const CXTPChartPoints& points);
	static CXTPChartDeviceCommand* AFX_CDECL Create2dRotateDeviceCommand(float fAngle);
	static CXTPChartDeviceCommand* AFX_CDECL Create2dRotateDeviceCommand(float fAngle, const CXTPPoint3d& rotateVector);
	static CXTPChartDeviceCommand* AFX_CDECL Create3dRotateDeviceCommand(const CXTPChart3dRotation& rotation);
	static CXTPChartDeviceCommand* AFX_CDECL Create2dTranslateDeviceCommand(double dx, double dy);
	static CXTPChartDeviceCommand* AFX_CDECL Create3dTranslateDeviceCommand(double dx, double dy, double dz);
	static CXTPChartDeviceCommand* AFX_CDECL Create2dGradientPieDeviceCommand(const CXTPPoint3d& center, double majorSemiAxis, double minorSemiAxis, double startAngle, double sweepAngle, double depth, double holePercent, const CXTPChartRectF& gradientBounds, const CXTPChartColor& color, const CXTPChartColor& color2);
	static CXTPChartDeviceCommand* AFX_CDECL Create3dGradientPieDeviceCommand(const CXTPPoint3d& center, double radius, double startAngle, double sweepAngle, double depth, double holePercent, const CXTPChartRectF& gradientBounds, const CXTPChartColor& color, const CXTPChartColor& color2);
	static CXTPChartDeviceCommand* AFX_CDECL Create2dBoundedPieDeviceCommand(const CXTPPoint3d& center, double majorSemiAxis, double minorSemiAxis, double startAngle, double sweepAngle, double depth, int holePercent, const CXTPChartColor& color, int nThickness);
	static CXTPChartDeviceCommand* AFX_CDECL Create2dFillBackgroundDeviceCommand(const CXTPChartRectF& rect, const CXTPChartColor& color);
	static CXTPChartDeviceCommand* AFX_CDECL Create3dFillBackgroundDeviceCommand(const CXTPChartColor& color);
	static CXTPChartDeviceCommand* AFX_CDECL Create3dCameraDeviceCommand(const CXTPPoint3f& ptPosition, const CXTPPoint3f& ptFocal, const CXTPPoint3f& ptUpVector);

public:
	virtual CXTPChartDeviceContext* CreateDeviceContext(CXTPChartContainer* pContainer, HDC hDC, const RECT& rcBounds) = 0;
	virtual CXTPChartDeviceCommand* CreatePolygonAntialiasingDeviceCommand(BOOL bAntiAlias = TRUE) = 0;
	virtual CXTPChartDeviceCommand* CreateGradientPieDeviceCommand(const CXTPPoint3d& center, double majorSemiAxis, double minorSemiAxis, double startAngle, double sweepAngle, double depth, double holePercent, const CXTPChartRectF& gradientBounds, const CXTPChartColor& color, const CXTPChartColor& color2) = 0;
	virtual CXTPChartDeviceCommand* CreateFillBackgroundDeviceCommand(const CXTPChartRectF& rect, const CXTPChartColor& color) = 0;
	virtual CXTPChartDeviceCommand* CreateSolidLineDeviceCommand(const CXTPPoint3d& p1, const CXTPPoint3d& p2, const CXTPChartColor& color, int thickness) = 0;
};

//-----------------------------------------------------------------------
// Summary:
//      Implements 2D drawing object factory singleton.
// See also:
//      CXTPChartDrawingObjectFactory, CXTPChart3dDrawingObjectFactory
//-----------------------------------------------------------------------
class CXTPChart2dDrawingObjectFactory 
	: public CXTPSingleton<CXTPChart2dDrawingObjectFactory>
	, public CXTPChartDrawingObjectFactory
{
	friend class CXTPSingleton<CXTPChart2dDrawingObjectFactory>;

	CXTPChart2dDrawingObjectFactory();

public:
	virtual CXTPChartDeviceContext* CreateDeviceContext(CXTPChartContainer* pContainer, HDC hDC, const RECT& rcBounds);
	virtual CXTPChartDeviceCommand* CreatePolygonAntialiasingDeviceCommand(BOOL bAntiAlias = TRUE);
	virtual CXTPChartDeviceCommand* CreateGradientPieDeviceCommand(const CXTPPoint3d& center, double majorSemiAxis, double minorSemiAxis, double startAngle, double sweepAngle, double depth, double holePercent, const CXTPChartRectF& gradientBounds, const CXTPChartColor& color, const CXTPChartColor& color2);
	virtual CXTPChartDeviceCommand* CreateFillBackgroundDeviceCommand(const CXTPChartRectF& rect, const CXTPChartColor& color);
	virtual CXTPChartDeviceCommand* CreateSolidLineDeviceCommand(const CXTPPoint3d& p1, const CXTPPoint3d& p2, const CXTPChartColor& color, int thickness);
};

//-----------------------------------------------------------------------
// Summary:
//      Implements 3D drawing object factory singleton.
// See also:
//      CXTPChartDrawingObjectFactory, CXTPChart2dDrawingObjectFactory
//-----------------------------------------------------------------------
class CXTPChart3dDrawingObjectFactory
	: public CXTPSingleton<CXTPChart3dDrawingObjectFactory>
	, public CXTPChartDrawingObjectFactory
{
	friend class CXTPSingleton<CXTPChart3dDrawingObjectFactory>;

	CXTPChart3dDrawingObjectFactory();

public:
	virtual CXTPChartDeviceContext* CreateDeviceContext(CXTPChartContainer* pContainer, HDC hDC, const RECT& rcBounds);
	virtual CXTPChartDeviceCommand* CreatePolygonAntialiasingDeviceCommand(BOOL bAntiAlias = TRUE);
	virtual CXTPChartDeviceCommand* CreateGradientPieDeviceCommand(const CXTPPoint3d& center, double majorSemiAxis, double minorSemiAxis, double startAngle, double sweepAngle, double depth, double holePercent, const CXTPChartRectF& gradientBounds, const CXTPChartColor& color, const CXTPChartColor& color2);
	virtual CXTPChartDeviceCommand* CreateFillBackgroundDeviceCommand(const CXTPChartRectF& rect, const CXTPChartColor& color);
	virtual CXTPChartDeviceCommand* CreateSolidLineDeviceCommand(const CXTPPoint3d& p1, const CXTPPoint3d& p2, const CXTPChartColor& color, int thickness);
};


#include "Common/Base/Diagnostic/XTPEnableNoisyWarnings.h"
#endif //#if !defined(__XTPCHARTDRAWINGOBJECTFACTORY_H__)
