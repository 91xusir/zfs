// XTPCommandBarScrollBarCtrl.h
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
#if !defined(__XTPCOMMANDBARSCROLLBARCTRL_H__)
#define __XTPCOMMANDBARSCROLLBARCTRL_H__
//}}AFX_CODEJOCK_PRIVATE

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Common/Base/Diagnostic/XTPDisableNoisyWarnings.h"

//===========================================================================
// Summary:
//     CXTPCommandBarScrollBarCtrl is a CScrollBar derived class.
//===========================================================================
class _XTP_EXT_CLASS CXTPCommandBarScrollBarCtrl : public CXTPScrollBarCtrl
{
public:
	//-----------------------------------------------------------------------
	// Summary:
	//     Constructs a CXTPCommandBarScrollBarCtrl object
	//-----------------------------------------------------------------------
	CXTPCommandBarScrollBarCtrl();

	//-----------------------------------------------------------------------
	// Summary:
	//     Destroys a CXTPCommandBarScrollBarCtrl object, handles cleanup and deallocation
	//-----------------------------------------------------------------------
	virtual ~CXTPCommandBarScrollBarCtrl();

public:
	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member to set the command bar's object.
	// Parameters:
	//     pCommandBars - CXTPCommandBars to be set.
	//-----------------------------------------------------------------------
	void SetCommandBars(CXTPCommandBars* pCommandBars);

protected:

	virtual CXTPScrollBarPaintManager* GetScrollBarPaintManager() const;

protected:

	DECLARE_MESSAGE_MAP()
	//{{AFX_MSG(CXTPCommandBarScrollBarCtrl)
	//}}AFX_MSG

protected:
	CXTPCommandBars *m_pCommandBars;            // Parent CommandBars object
};


AFX_INLINE void CXTPCommandBarScrollBarCtrl::SetCommandBars(CXTPCommandBars *pCommandBars)
{
	m_pCommandBars = pCommandBars;
}


#include "Common/Base/Diagnostic/XTPEnableNoisyWarnings.h"
#endif // !defined(__XTPCOMMANDBARSCROLLBARCTRL_H__)
