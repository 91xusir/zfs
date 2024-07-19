// XTPCommandBarsModernUITheme.h
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
#if !defined(__XTPCOMMANDBARSMODERNUITHEME_H__)
#define __XTPCOMMANDBARSMODERNUITHEME_H__
//}}AFX_CODEJOCK_PRIVATE

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "Common/Base/Diagnostic/XTPDisableNoisyWarnings.h"

//===========================================================================
// Summary:
//     The CXTPCommandBarsModernUITheme class is the base class for the
//      Modern UI themes for Command Bars.
// See Also: CXTPPaintManager::SetTheme
//===========================================================================
class _XTP_EXT_CLASS CXTPCommandBarsModernUITheme : public CXTPCommandBarsOfficeXPTheme
{
	DECLARE_DYNAMIC(CXTPCommandBarsModernUITheme)
		
public:
	//-----------------------------------------------------------------------
	// Summary:
	//     Constructs a CXTPCommandBarsModernUITheme object.
	//-----------------------------------------------------------------------
	CXTPCommandBarsModernUITheme();

	//-----------------------------------------------------------------------
	// Summary:
	//     Destroys a CXTPCommandBarsModernUITheme object, handles cleanup and deallocation
	//-----------------------------------------------------------------------
	virtual ~CXTPCommandBarsModernUITheme();

	//-----------------------------------------------------------------------
	// Summary:
	//     Returns paint manager used to draw frames.
	// Returns:
	//     Returns paint manager used to draw frames.
	//-----------------------------------------------------------------------
	virtual CXTPFramePaintManager* GetFramePaintManager();

	//-----------------------------------------------------------------------
	// Summary:
	//     Retrieves ribbon paint manager.
	// Returns:
	//     Pointer to CXTPRibbonPaintManager object.
	//-----------------------------------------------------------------------
	virtual CXTPRibbonPaintManager* GetRibbonPaintManager();

	//-----------------------------------------------------------------------
	// Summary:
	//     Retrieves markup context.
	// Returns:
	//     Pointer to CXTPMarkupContext object.
	//-----------------------------------------------------------------------
	virtual CXTPMarkupContext* GetMarkupContext();

protected:
	//-----------------------------------------------------------------------
	// Summary:
	//     This method is called to refresh the visual metrics of the manager.
	//-----------------------------------------------------------------------
	virtual void RefreshMetrics();

	CXTPMarkupContext*  m_pMarkupContext; // Points to a valid markup context object.
	CXTPResourceImages* m_pImages;        // Points to a valid resource images object.
};

/////////////////////////////////////////////////////////////////////////////

AFX_INLINE CXTPFramePaintManager* CXTPCommandBarsModernUITheme::GetFramePaintManager() {
	return m_pFramePaintManager;
}
#ifdef _XTP_INCLUDE_RIBBON
AFX_INLINE CXTPRibbonPaintManager* CXTPCommandBarsModernUITheme::GetRibbonPaintManager() {
	return m_pRibbonPaintManager;
}
#else
AFX_INLINE CXTPRibbonPaintManager* CXTPCommandBarsModernUITheme::GetRibbonPaintManager() {
	return NULL;
}
#endif
AFX_INLINE CXTPMarkupContext* CXTPCommandBarsModernUITheme::GetMarkupContext() {
	return m_pMarkupContext;
}


#include "Common/Base/Diagnostic/XTPEnableNoisyWarnings.h"
#endif // !defined(__XTPCOMMANDBARSMODERNUITHEME_H__)
