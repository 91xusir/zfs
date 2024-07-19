#ifndef GDI_PLUS_FONT_MANAGER_IMP_H
#define GDI_PLUS_FONT_MANAGER_IMP_H

#include <Windows.h>
#include <gdiplus.h>
#include "ui/rtw_ui.h"
#include "gdi_plus_font_imp.h"

using Gdiplus::Graphics;

class CGdiPlusFontManagerImp:public IUiFontManager
{
public:
	CGdiPlusFontManagerImp(Graphics* vpRender):mpRender(vpRender){};
	virtual ~CGdiPlusFontManagerImp(){};

	virtual void Render() {};
protected:
	virtual RtwFont* OnCreateFont(const std::string& FontName, const int FontSize , int nFontStyle )
	{/*LOG("");*/
		return new CGdiPlusFontImp(mpRender, FontName, FontSize);
	}
private:
	Graphics* mpRender;
};


#endif//GDI_PLUS_FONT_MANAGER_IMP_H