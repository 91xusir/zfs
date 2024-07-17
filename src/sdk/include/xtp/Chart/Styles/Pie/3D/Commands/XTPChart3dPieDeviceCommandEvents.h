// XTPChart3dPieDeviceCommandEvents.h
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
#if !defined(__XTPCHART3DPIEDEVICECOMMANDEVENTS_H__)
#define __XTPCHART3DPIEDEVICECOMMANDEVENTS_H__
//}}AFX_CODEJOCK_PRIVATE

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "Common/Base/Diagnostic/XTPDisableNoisyWarnings.h"

//-----------------------------------------------------------------------
// Summary:
//      Defines interface for 3D chart pie device command event sink.
//-----------------------------------------------------------------------
class _XTP_EXT_CLASS CXTPChart3dPieDeviceCommandEvents 
	: public CXTPChartDeviceCommandEvents
	, public CXTPTypeIdProvider<CXTPChart3dPieDeviceCommandEvents, CXTPChartDeviceCommandEvents>
{
protected:
	//-----------------------------------------------------------------------
	// Summary:
	//      Constructs CXTPChart3dPieDeviceCommandEvents object.
	//-----------------------------------------------------------------------
	CXTPChart3dPieDeviceCommandEvents();

public:
	//-----------------------------------------------------------------------
	// Summary:
	//      Gets called when pie connector points are computed.
	// Parameters:
	//      pCommand - The sender command.
	//      ptShapeCenter - The coordinates of the physical shape center.
	//      ptOuterCenter - The coordinates of the most outer horizontal shape center point.
	//-----------------------------------------------------------------------
	virtual void OnSetConnectorPoints(
		CXTPChartDeviceCommand* pCommand,
		CXTPPoint3d ptShapeCenter,
		CXTPPoint3d ptOuterCenter);
};


#include "Common/Base/Diagnostic/XTPEnableNoisyWarnings.h"
#endif //#if !defined(__XTPCHART3DPIEDEVICECOMMANDEVENTS_H__)
