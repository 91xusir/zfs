#ifndef GDI_PLUS_FONT_IMP_H
#define GDI_PLUS_FONT_IMP_H

#include <string>
#include <Windows.h>
#include <GdiPlus.h>
#include "ui/rtw_ui.h"
#include "gb_to_unicode_windows.h"
#include "cmylog.h"

using std::string;
using Gdiplus::Font;
using Gdiplus::FontStyleRegular;
using Gdiplus::FontStyleUnderline;
using Gdiplus::UnitPixel;
using Gdiplus::Graphics;

class CGdiPlusRenderImp;

class CGdiPlusFontImp:public RtwFont
{
public:
	CGdiPlusFontImp(Graphics* vpRender,const string& vrFontName,int vFontSize)
		:RtwFont(vrFontName,vFontSize),
		mFontSize(vFontSize),mFont(GbToUnicode(vrFontName),
		vFontSize,FontStyleRegular,UnitPixel),
		mUnderLineFont(GbToUnicode(vrFontName),vFontSize,FontStyleUnderline,UnitPixel),
		mpRender(vpRender){/*LOG("");*/};

	virtual ~CGdiPlusFontImp(){/*LOG("");*/}

	virtual void Close(){};
	virtual void OnInvalidateDevice() {/*LOG("");*/};
	virtual void OnRestoreDevice() {/*LOG("");*/};

	virtual void DrawString(const std::string& str, const RtwRect& rect, 	const RtwPixel& textColor, 
		bool bWordWrap ,SUiTextAlignment align , bool bUnderLine , const RtwRect* pClipRect, int RenderSize = 0);

	virtual void DrawString3D(const std::string& str, const SRect3D& rect, const RtwPixel& textColor,
		bool bWordWrap , SUiTextAlignment align ,bool bUnderLine , const RtwRect* pClipRect, int RenderSize = 0);

	virtual void Flush(){};
	virtual int GetStringWidth(const std::string& str, int length = -1, int RenderSize = 0);
	virtual bool PickCharacter(const SPoint& point, int& outIndex, 
									bool& outDoubleByte, bool& outNear, const std::string& str, 
									const RtwRect& rect, int RenderSize = 0);
	virtual bool PickCharacter(int width, int& outIndex, bool& outDoubleByte, 
		bool& outNear, const std::string& str, int RenderSize = 0);
	virtual void ResetTexture(bool isReset);

protected:
	void TestDrawString(const std::string& str, const RtwRect& rect, 	const RtwPixel& textColor, 
		bool bWordWrap ,SUiTextAlignment align , bool bUnderLine , const RtwRect* pClipRect );

	void RealDrawString(const std::string& str, const RtwRect& rect, 	const RtwPixel& textColor, 
		bool bWordWrap ,SUiTextAlignment align , bool bUnderLine , const RtwRect* pClipRect );

	//¸¨Öúº¯Êý//ÉÏ×Ö
	void GdiPlusDrawString(const string& vrString,const RtwRect& vrTargetRect,const RtwPixel& textColor,bool vUnderLine);
private:
	int							mFontSize;
	Font						mFont;
	Font						mUnderLineFont;
	Graphics*					mpRender;
};
#endif//GDI_PLUS_FONT_IMP_H