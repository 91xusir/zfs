// XTPRibbonRenameDialog.h: interface for the CXTPRibbonRenameDialog class.
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
#if !defined(__XTPRIBBONRENAMEDIALOG_H__)
#define __XTPRIBBONRENAMEDIALOG_H__
//}}AFX_CODEJOCK_PRIVATE

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Common/Base/Diagnostic/XTPDisableNoisyWarnings.h"

class _XTP_EXT_CLASS CXTPRibbonRenameDialog : public CDialog
{
	//{{AFX_CODEJOCK_PRIVATE
	DECLARE_DYNAMIC(CXTPRibbonRenameDialog)
	//}}AFX_CODEJOCK_PRIVATE
	
public:
	CXTPRibbonRenameDialog();
	
	enum { IDD = XTP_IDD_RIBBONCUSTOMIZE_RENAME };
	
	const CString& GetName() const;
	void SetName(LPCTSTR lpszNewName);

protected:
	virtual void DoDataExchange(CDataExchange* pDX);

private:
	CString m_strName;
};

AFX_INLINE const CString& CXTPRibbonRenameDialog::GetName() const {
	return m_strName;
}

AFX_INLINE void CXTPRibbonRenameDialog::SetName(LPCTSTR lpszNewName) {
	ASSERT(NULL != lpszNewName);
	m_strName = lpszNewName;
}

//{{AFX_CODEJOCK_PRIVATE

#include "Common/Base/Diagnostic/XTPEnableNoisyWarnings.h"
#endif  // __XTPRIBBONRENAMEDIALOG_H__
//}}AFX_CODEJOCK_PRIVATE
