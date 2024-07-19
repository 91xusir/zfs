#pragma once


// CMapListCtrl

class CMapListCtrl : public CListCtrl
{
	DECLARE_DYNAMIC(CMapListCtrl)

public:
	CMapListCtrl();
	virtual ~CMapListCtrl();

public:
    inline void SetBackColor(COLORREF crBack) {m_crBack = crBack;}
    inline COLORREF GetBackColor() {return m_crBack;}
    virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);

public:
    virtual void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct );

protected:
	DECLARE_MESSAGE_MAP()

protected:
    COLORREF m_crBack; // An RGB value.
};
