// XTPProgressPaintManager.h
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
#if !defined(__XTPPROGRESSPAINTMANAGER_H__)
#define __XTPPROGRESSPAINTMANAGER_H__
//}}AFX_CODEJOCK_PRIVATE

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Common/Base/Diagnostic/XTPDisableNoisyWarnings.h"

class CXTPProgressBase;
class CXTPPaintManager;

//===========================================================================
// Summary: CXTPProgressPaintManager object
//===========================================================================
class _XTP_EXT_CLASS CXTPProgressPaintManager
{
public:
	//-----------------------------------------------------------------------
	// Summary:
	//     Constructs a CXTPProgressPaintManager object
	// Parameters:
	//     pPaintManager - PaintManager of commandbars
	//-----------------------------------------------------------------------
	CXTPProgressPaintManager(CXTPPaintManager* pPaintManager);

	//-----------------------------------------------------------------------
	// Summary:
	//     Destroys a CXTPProgressPaintManager object, handles cleanup and deallocation
	//-----------------------------------------------------------------------
	virtual ~CXTPProgressPaintManager();

public:
	//-----------------------------------------------------------------------
	// Summary:
	//     This method is called to draw progress
	// Parameters:
	//     pDC - Pointer to device context
	//     pProgressBar - Progress to draw
	//-----------------------------------------------------------------------
	virtual void DrawProgress(CDC* pDC, CXTPProgressBase* pProgressBar);

	//-------------------------------------------------------------------------
	// Summary:
	//     Recalculates metrics
	//-------------------------------------------------------------------------
	virtual void RefreshMetrics();

	//-----------------------------------------------------------------------
	// Summary:
	//     Returns parent paintmanager object
	//-----------------------------------------------------------------------
	CXTPPaintManager* GetPaintManager() const;

protected:
	CXTPPaintManager* m_pPaintManager;          // Pointer to parent PaintManager

public:
	int m_cyProgress;           // Progress height
};

AFX_INLINE CXTPPaintManager* CXTPProgressPaintManager::GetPaintManager() const
{
	return m_pPaintManager;
}


#include "Common/Base/Diagnostic/XTPEnableNoisyWarnings.h"
#endif // !defined(__XTPPROGRESSPAINTMANAGER_H__)
