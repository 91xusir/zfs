// XTPFrameThemeOffice2013.h : interface for the CXTPFrameThemeOffice2013 class.
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
#if !defined(__XTPFRAMETHEMEOFFICE2013_H__)
#define __XTPFRAMETHEMEOFFICE2013_H__
//}}AFX_CODEJOCK_PRIVATE

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "Common/Base/Diagnostic/XTPDisableNoisyWarnings.h"

class CXTPPaintManager;
class CXTPCommandBarsFrameHook;

class _XTP_EXT_CLASS CXTPFrameThemeOffice2013 : public CXTPFrameThemeModernUI
{
public:

	CXTPFrameThemeOffice2013(CXTPPaintManager *pPaintManager);

	virtual ~CXTPFrameThemeOffice2013();

protected:

	//-----------------------------------------------------------------------
	// Summary:
	//     This method is called to draw frame caption bar.
	// Parameters:
	//     pDC - Pointer to device context
	//     rcCaptionBar - Caption bar rectangle
	//     pSite - Frame to draw
	//     bActive - TRUE if frame is active
	//-----------------------------------------------------------------------
	virtual void DrawCaptionBar(CDC *pDC, CRect rcCaptionBar, CWnd *pSite, BOOL bActive);

	//-----------------------------------------------------------------------
	// Summary:
	//     This method is called to draw caption button
	// Parameters:
	//      pDC - Pointer to device context
	//      rc - Bounding rectangle of the button
	//      nId - Identifier of the button
	//      bSelected - TURE if button is highlighted
	//      bPressed -  TURE if button is pressed
	//      bActive - TURE if frame is active
	//-----------------------------------------------------------------------
	virtual void DrawFrameCaptionButton(CDC *pDC, CRect rc, int nId, BOOL bSelected, BOOL bPressed, BOOL bActive);

	//-----------------------------------------------------------------------
	// Summary:
	//     This method is called to refresh the visual metrics of the manager.
	//-----------------------------------------------------------------------
	virtual void RefreshMetrics();

	virtual LPCTSTR GetStyleResourceName() const;

	virtual CRect CalcFrameBorder(const CXTPCommandBarsFrameHook *pFrameHook) const;

	virtual CXTPFrameShadow* CreateShadow(CWnd* pFrame);

	CXTPPaintManagerColor m_clrFrameCaptionBorder;

	CXTPColorSetCaptionButton m_csFrameCloseButtonActive;   // Close button active background color.
	CXTPColorSetCaptionButton m_csFrameCloseButtonInactive; // Close button inactive background color.
	CXTPColorSetCaptionButton m_csFrameCloseButtonHot;      // Close button hot background color.
	CXTPColorSetCaptionButton m_csFrameCloseButtonPressed;  // Close button pressed background color.

	CXTPColorSetCaptionButton m_csFrameCloseButtonActiveBSMode;   // Close button active background color.
	CXTPColorSetCaptionButton m_csFrameCloseButtonInactiveBSMode; // Close button inactive background color.
	CXTPColorSetCaptionButton m_csFrameCaptionButtonInactiveBSMode;
	CXTPColorSetCaptionButton m_csFrameCaptionButtonActiveBSMode;
	CXTPColorSetCaptionButton m_csFrameCaptionButtonHotBSMode;
	CXTPColorSetCaptionButton m_csFrameCaptionButtonPressedBSMode;
};


#include "Common/Base/Diagnostic/XTPEnableNoisyWarnings.h"
#endif // !defined(__XTPFRAMETHEMEOFFICE2013_H__)
