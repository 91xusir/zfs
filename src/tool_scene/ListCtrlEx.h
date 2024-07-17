#pragma once


// CListCtrlEx

class CListCtrlEx : public CListCtrl
{
	DECLARE_DYNAMIC(CListCtrlEx)

public:
	CListCtrlEx();
	virtual ~CListCtrlEx();

    int InsertItem(int nItem, LPCTSTR lpszItem, HBITMAP hbitmap)
    {
        m_vBitmap.push_back(hbitmap);
        return CListCtrl::InsertItem(nItem, lpszItem, 0);
    }

    vector<HBITMAP> m_vBitmap;
    CFont m_Font;

protected:
	DECLARE_MESSAGE_MAP()
public:
//    afx_msg void OnNMCustomdraw(NMHDR *pNMHDR, LRESULT *pResult);
    afx_msg void OnPaint();
    afx_msg BOOL OnEraseBkgnd(CDC* pDC);
      //afx_msg void OnNMClick(NMHDR *pNMHDR, LRESULT *pResult);
   // afx_msg void OnHdnItemclick(NMHDR *pNMHDR, LRESULT *pResult);
//    afx_msg void OnLvnKeydown(NMHDR *pNMHDR, LRESULT *pResult);
//    afx_msg void OnLvnItemActivate(NMHDR *pNMHDR, LRESULT *pResult);
//    afx_msg void OnLvnHotTrack(NMHDR *pNMHDR, LRESULT *pResult);
      afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
};


