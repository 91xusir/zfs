#pragma once

#include "GrassTextWnd.h"
// GrassTextWnd

class GrassTextWnd : public CWnd
{
	DECLARE_DYNAMIC(GrassTextWnd)

public:

    void SetText(const char* fileName, int sx, int sy);

	GrassTextWnd();
	virtual ~GrassTextWnd();

    int GetSubIx() const
    {
        return m_curY * m_subX + m_curX;
    }

    float GetAspectW2H();

protected:
	DECLARE_MESSAGE_MAP()
    afx_msg void OnPaint();
    afx_msg void OnLButtonDown(UINT nFlags, CPoint point);

public :
    HBITMAP m_hBitmap;
    int m_subX;
    int m_subY;
    int m_curX;
    int m_curY;

};


