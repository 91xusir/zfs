#pragma once

#include "GrassTextWnd.h"
#include "afxwin.h"
#include "afxcmn.h"


class CBrushGrassForm : public CDialog
{
	DECLARE_DYNAMIC(CBrushGrassForm)

public:
	CBrushGrassForm(CWnd* pParent = NULL);   // standard constructor
	virtual ~CBrushGrassForm();

// Dialog Data
	enum { IDD = IDD_DLG_GRASS };

    WORD GetTextId()
    {
        int sel = m_ComboxText.GetCurSel();

        if (sel == CB_ERR)
            return 0;

        return ((m_ComboxText.GetItemData(sel) & 0xff) << 8) | (m_TextWnd.GetSubIx() & 0xff);
    }

    int GetDensity()
    {
        return m_grassDensity;
    }

    void GetGrassSizeInfo(RtgVertex2* v, float* aspectW)
    {
        v->x = m_grassHeightMin;
        v->y = m_grassHeightMax;
        *aspectW = m_TextWnd.GetAspectW2H(); 
    }


public :
    GrassTextWnd m_TextWnd;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
    void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);

    void OnOK()
    {
    }

    void OnCancel()
    {
    }
	DECLARE_MESSAGE_MAP()
public:
    afx_msg void OnBnClickedCheck1();
    CComboBox m_ComboxText;
    virtual BOOL OnInitDialog();
    afx_msg void OnCbnSelchangeCombo1();
    int m_grassDensity;
    float m_grassHeightMin;
    float m_grassHeightMax;
    float m_grassWidthMin;
    float m_grassWidthMax;
    CSliderCtrl m_BrushSize;
    afx_msg void OnEnKillfocusEdit();
    afx_msg void OnEnChangeEdit2();
    afx_msg void OnEnChangeEdit4();
};
