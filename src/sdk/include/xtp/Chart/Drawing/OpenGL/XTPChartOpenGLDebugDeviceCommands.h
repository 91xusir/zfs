// XTPChartOpenGLDebugDeviceCommands.h
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
#ifdef _DEBUG

#if !defined(__XTPCHARTOPENGLDEBUGDEVICECOMMANDS_H__)
#define __XTPCHARTOPENGLDEBUGDEVICECOMMANDS_H__

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "Common/Base/Diagnostic/XTPDisableNoisyWarnings.h"

class _XTP_EXT_CLASS CXTPChartOpenGLDebugAxesDeviceCommand : public CXTPChart3dDeviceCommand
{
public:
	// In absolute coordinates
	CXTPChartOpenGLDebugAxesDeviceCommand(const CXTPPoint3d& pt, double dSize);

protected:
	virtual void ExecuteOverride(CXTPChartDeviceContext* pDC);

private:
	CXTPPoint3d m_pt;
	double m_dSize;
};

class _XTP_EXT_CLASS CXTPChartOpenGLDebugBoxDeviceCommand : public CXTPChart3dDeviceCommand
{
public:
	// In absolute coordinates
	CXTPChartOpenGLDebugBoxDeviceCommand(const CXTPChart3dBox& box, BOOL bSolid = FALSE);

protected:
	virtual void ExecuteOverride(CXTPChartDeviceContext* pDC);

private:
	CXTPChart3dBox m_box;
	BOOL m_bSolid;
};

#endif //#if !defined(__XTPCHARTOPENGLDEBUGDEVICECOMMANDS_H__)


#include "Common/Base/Diagnostic/XTPEnableNoisyWarnings.h"
#endif /*_DEBUG*/
//}}AFX_CODEJOCK_PRIVATE
