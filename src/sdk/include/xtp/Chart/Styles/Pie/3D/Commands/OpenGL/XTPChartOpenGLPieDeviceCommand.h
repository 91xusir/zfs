// XTPChartOpenGLPieDeviceCommand.h
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
#if !defined(__XTPCHARTOPENGLPIEDEVICECOMMAND_H__)
#define __XTPCHARTOPENGLPIEDEVICECOMMAND_H__

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "Common/Base/Diagnostic/XTPDisableNoisyWarnings.h"

class CXTPChart3dPieSeriesStyle;
class CXTPChartDeviceContext;
class CXTPChartSeriesStyle;

//===========================================================================
// Summary:
//     This class represents a chart pie device command,which is a kind of
//     CXTPChartDeviceCommand. It specifically handles the rendering of a pie
//     in a chart.
// Remarks:
//===========================================================================
class _XTP_EXT_CLASS CXTPChartOpenGLPieDeviceCommand : public CXTPChart3dPieDeviceCommand
{
public:
	CXTPChartOpenGLPieDeviceCommand(const CXTPPoint3d& center, double radius, double startAngle, 
		double sweepAngle, double depth, double holePercent, const CXTPChartRectF& gradientBounds, 
		const CXTPChartColor& color, const CXTPChartColor& color2);
	~CXTPChartOpenGLPieDeviceCommand();

protected:
	virtual CXTPChartElement* HitTest(CPoint point, CXTPChartElement* pParent) const;

	virtual void ExecuteOverride(CXTPChartDeviceContext* pDC);
	virtual void ApplySeriesStyle(CXTPChartSeriesStyle* pStyle);
	virtual void PrepareOverride(CXTPChartDeviceContext* pDC);

protected:
	CXTPChartRectF m_rcGradientBounds;
	CXTPChartColor m_color;
	CXTPChartColor m_color2;
	CXTPChart3dPieSeriesStyle* m_pStyle;
	GLuint m_listId;

private:
	struct BarRingInfo;
	struct TorusRingInfo;

	CXTPPoint3d AFX_CDECL GetBarPieSliceCenter(int n, ...) const;

	void DrawBarPie(double dFromAngle, double dToAngle);
	void DrawBarPie_OpenShape(const BarRingInfo& ring);
	void DrawBarPie_Shape(const BarRingInfo& ringFrom, const BarRingInfo& ringTo);
	void DrawBarPie_CloseShape(const BarRingInfo& ring);

	void DrawTorusPie(double dFromAngle, double dToAngle);
	void DrawTorusPie_OpenShape(const TorusRingInfo& ring);
	void DrawTorusPie_Shape(const TorusRingInfo& ringFrom, const TorusRingInfo& ringTo);
	void DrawTorusPie_CloseShape(const TorusRingInfo& ring);

	void ComputeConnectorPoints(double dFromAngle, double dToAngle);

private:
	BOOL m_bPrepared;
	CXTPChart3dBox m_3dBox;
	CXTPPoint3d m_ptCenterConnectorPoint;
	CXTPPoint3d m_ptOuterConnectorPoint;
};


#include "Common/Base/Diagnostic/XTPEnableNoisyWarnings.h"
#endif //#if !defined(__XTPCHARTOPENGLPIEDEVICECOMMAND_H__)
//}}AFX_CODEJOCK_PRIVATE
