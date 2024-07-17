#pragma once
#include "afxcmn.h"
#include "TimeView.h"
#include "Resource.h"
// CDlgSliderView dialog
class CDlgSliderView : public CXTResizeDialog
{
	DECLARE_DYNAMIC(CDlgSliderView)

public:
	CDlgSliderView(CWnd* pParent = NULL);				// standard constructor
	virtual ~CDlgSliderView();

// Dialog Data
	enum { IDD = IDD_DLG_SLIDER_VIEW };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	DECLARE_MESSAGE_MAP()

public:
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnBnClickedAnimPlay();
	afx_msg void OnBnClickedAnimStop();
	afx_msg void OnBnClickedAnimLoop();
	afx_msg void OnBnClickedPoseConn();

	void SetCurFrame(int frame);
	void SetAnimRange(int min,int max,int pos);
	virtual BOOL OnInitDialog();

	CScrollBar m_animSlider;
	CTimeView  m_view;
	CString m_frameInfo;
};
