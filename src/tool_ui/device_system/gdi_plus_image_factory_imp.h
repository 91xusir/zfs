#ifndef GDI_PLUS_IMAGE_FACTORY_IMP_H
#define GDI_PLUS_IMAGE_FACTORY_IMP_H

#include <string>
#include <Windows.h>
#include <gdiplus.h>
#include "ui/rtw_ui.h"
#include "ref_pointer.h"
#include "cmylog.h"

using std::string;
using fox::CRefPtr;
using namespace Gdiplus;

//image¹¤³§
class CUiGdiPlusImageFactoryImp: public IUiImageFactory
{
public:
	typedef CRefPtr<Bitmap> CBitmapPtr;

	CUiGdiPlusImageFactoryImp(){};
	virtual ~CUiGdiPlusImageFactoryImp(){};
	CBitmapPtr GetBitmap(const string& vrFileName);
	Bitmap* LoadBitmapEx(const string& vrFileName);

protected:
	virtual RtwImage* RealCreateImage();
private:
	Bitmap* LoadBitmapFrom(BitmapData& vrUserData);
	Bitmap* LoadBitmapExTemp(const string& vrFileName);
private:
	map<string,CBitmapPtr> mBitmapList;
};
#endif