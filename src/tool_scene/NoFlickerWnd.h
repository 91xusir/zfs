
#pragma once

template<class BASE_CLASS>
class CNoFlickerWnd : public BASE_CLASS
{
public:

	// This member function is called to set the background fill 
	// color for the flicker free control.
	//
	inline void SetBackColor(
		// An RGB value.
		COLORREF crBack) {m_crBack = crBack;}

	// This member function is called to retrieve the background fill color
	// for the flicker free control. Returns an RGB value.
	//
	inline COLORREF GetBackColor() {return m_crBack;}

protected:

	// An RGB value.
	COLORREF m_crBack;

	// This method provides a CE procedure (WindowProc) for a CWnd object.
	// It dispatches messages through the window message map. The return value
	// depends on the message.
	//
	virtual LRESULT WindowProc(
		// Specifies the Windows message to be processed.
		UINT message,
		// Provides additional information used in processing the message. The
		// parameter value depends on the message.
		WPARAM wParam,
		// Provides additional information used in processing the message. The
		// parameter value depends on the message.
		LPARAM lParam)
	{
		switch (message)
		{
		case WM_PAINT:
			{
				CPaintDC dc(this);
				
				// Get the client rect, and paint to a memory device context.  This
				// will help reduce screen flicker.  Pass the memory device context
				// to the default window procedure to do default painting.
				
				CRect r;
				GetClientRect(&r);
				CXTPBufferDC memDC(dc, r);
				memDC.FillSolidRect(r, GetSysColor(COLOR_WINDOW));
				
				return BASE_CLASS::DefWindowProc( WM_PAINT, (WPARAM)memDC.m_hDC, 0 );
			}
			
		case WM_ERASEBKGND:
			{
				return TRUE;
			}
		}
		
		return BASE_CLASS::WindowProc(message, wParam, lParam);
	}
};
