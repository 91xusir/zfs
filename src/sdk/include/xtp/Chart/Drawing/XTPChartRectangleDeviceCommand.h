// XTPChartRectangleDeviceCommand.h
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
#if !defined(__XTPCHARTRECTANGLEDEVICECOMMAND_H__)
#define __XTPCHARTRECTANGLEDEVICECOMMAND_H__
//}}AFX_CODEJOCK_PRIVATE

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "Common/Base/Diagnostic/XTPDisableNoisyWarnings.h"

//===========================================================================
// Summary:
//      Base class implementation for 2D chart rectangle device command.
//===========================================================================
class _XTP_EXT_CLASS CXTPChartRectangleDeviceCommand : public CXTPChartDeviceCommand
{
protected:
	//-----------------------------------------------------------------------
	// Summary:
	//     Constructs a CXTPChartRectangleDeviceCommand object.
	// Parameters:
	//     rect      - The rectangular area to be rendered by the device command.
	//-----------------------------------------------------------------------
	CXTPChartRectangleDeviceCommand(const CXTPChartRectF& rect);

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
	CXTPChartRectF m_rect;       //The bounds of the rectangle.
};


#include "Common/Base/Diagnostic/XTPEnableNoisyWarnings.h"
#endif //#if !defined(__XTPCHARTRECTANGLEDEVICECOMMAND_H__)
