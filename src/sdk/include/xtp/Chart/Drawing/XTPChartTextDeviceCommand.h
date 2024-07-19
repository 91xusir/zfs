// XTPChartTextDeviceCommand.h
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
#if !defined(__XTPCHARTTEXTDEVICECOMMAND_H__)
#define __XTPCHARTTEXTDEVICECOMMAND_H__
//}}AFX_CODEJOCK_PRIVATE

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "Common/Base/Diagnostic/XTPDisableNoisyWarnings.h"


class CXTPMarkupUIElement;

//===========================================================================
// Summary:
//     This class represents a text device command,which is a kind of CXTPChartDeviceCommand.
//     It specifically handles the rendering of texts in a chart.
// Remarks:
//===========================================================================
class _XTP_EXT_CLASS CXTPChartTextDeviceCommand : public CXTPChartDeviceCommand
{
protected:
	//-----------------------------------------------------------------------
	// Summary:
	//     Constructs a CXTPChartTextDeviceCommand object.
	// Parameters:
	//     strText   - The text to be rendered.
	//     pFont     - The font used to render.
	//     color     - The color of the text.
	// Remarks:
	//-----------------------------------------------------------------------
	CXTPChartTextDeviceCommand(const CXTPChartString& strText, CXTPChartFont* pFont, const CXTPChartColor& color);

	//-----------------------------------------------------------------------
	// Summary:
	//     This is a virtual function override of base class CXTPChartDeviceContext,
	//     act polymorphically to do the actual drawing of the chart element, to which
	//     this device command is associated with.
	// Parameters:
	//     pDC      - The device context of the chart.
	// Remarks:
	//-----------------------------------------------------------------------
	virtual void ExecuteOverride(CXTPChartDeviceContext* pDC);

	//-----------------------------------------------------------------------
	// Summary:
	//     This function do the actual drawing of the chart element, to which
	//     this device command is associated with, here it renders the text.
	// Parameters:
	//     pDC      - The device context of the chart.
	// Remarks:
	//-----------------------------------------------------------------------
	virtual void ExecuteInternal(CXTPChartDeviceContext* pDC);

protected:
	CXTPChartString m_strText;   //The string to be rendered.
	CXTPChartFont* m_pFont;      //The font used to render the text.
	CXTPChartColor m_color;      //The color of the text.

};


//===========================================================================
// Summary:
//     This class represents a text device command,which is a kind of CXTPChartDeviceCommand.
//     This class does handles the smoothing of texts in chart and by using antialising.
// Remarks:
//===========================================================================
class _XTP_EXT_CLASS CXTPChartTextAntialiasingDeviceCommand : public CXTPChartDeviceCommand
{
public:
	//-----------------------------------------------------------------------
	// Summary:
	//     Constructs a CXTPChartTextAntialiasingDeviceCommand object.
	// Parameters:
	//     bAntialias   - TRUE if the antialiasing to be enabled, FALSE to not use
	//                    the antialiasing.The default value is TRUE.
	// Remarks:
	//-----------------------------------------------------------------------
	CXTPChartTextAntialiasingDeviceCommand(BOOL bAntialias = TRUE);

protected:
	BOOL m_bAntiAlias;      //TRUE if antialiasing is enabled, FALSE if antialiasing is not used.
	int m_nOldTextRenderingHint;
};

//===========================================================================
// Summary:
//     This class represents a bounded text device command,which is a kind of
//     CXTPChartDeviceCommand. It handles the rendering of bounded texts in a chart.
// Remarks:
//===========================================================================
class _XTP_EXT_CLASS CXTPChartBoundedTextDeviceCommand : public CXTPChartTextDeviceCommand
{
public:
	//-----------------------------------------------------------------------
	// Summary:
	//     Constructs a CXTPChartBoundedTextDeviceCommand object.
	// Parameters:
	//     strText   - The text to be rendered.
	//     pFont     - The font used to render.
	//     color     - The color of the text.
	//     rectangle - The text bounds.
	// Remarks:
	//-----------------------------------------------------------------------
	CXTPChartBoundedTextDeviceCommand(const CXTPChartString& strText, CXTPChartFont* pFont, const CXTPChartColor& color, const CXTPChartRectF& rectangle);

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
	virtual CXTPChartElement* HitTest(CPoint point, CXTPChartElement* pParent) const;

protected:
	CXTPChartRectF m_rect;   //The bounding rectangle of the text.
};

//-----------------------------------------------------------------------
// Summary:
//      Implements 2D markup element drawing device command.
//-----------------------------------------------------------------------
class _XTP_EXT_CLASS CXTPChartMarkupElementDeviceCommand : public CXTPChartDeviceCommand
{
public:
	//-----------------------------------------------------------------------
	// Summary:
	//     Constructs a CXTPChartMarkupElementDeviceCommand object.
	// Parameters:
	//     pMarkupUIElement - Markup element pointer to draw.
	//     strText   - The text to be rendered.
	//     pFont     - The default font used to render.
	//     color     - The default color of the text.
	//     rectangle - The text bounds.
	//-----------------------------------------------------------------------
	CXTPChartMarkupElementDeviceCommand(CXTPMarkupUIElement* pMarkupUIElement, CXTPChartFont* pFont, const CXTPChartColor& color, const CXTPChartRectF& rectangle);

	//-----------------------------------------------------------------------
	// Summary:
	//      HAndles object deallocation.
	//-----------------------------------------------------------------------
	~CXTPChartMarkupElementDeviceCommand();

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
	CXTPChartRectF m_rect;   //The bounding rectangle of the text.
	CXTPMarkupUIElement* m_pMarkupUIElement; // Markup element to draw.
	CXTPChartColor m_color; // The default color of the text.
	CXTPChartFont* m_pFont;  // The default font of the text.
};


#include "Common/Base/Diagnostic/XTPEnableNoisyWarnings.h"
#endif //#if !defined(__XTPCHARTTEXTDEVICECOMMAND_H__)
