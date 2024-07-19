#ifndef DRIVER_SYSTEM_H
#define DRIVER_SYSTEM_H

#include <string>
#include "core/rt2_core.h"
#include "graph/rt_graph.h"
#include "ui/rtw_ui.h"
#include "cmylog.h"

const string GDI_PLUS_SYSTEM_AND_RT_SYSTEM("GdiPlus");//GDI+系统
const string RT_DRIVER_SYSTEM("RtDriver");//RT系统

using std::string;
using rt_graph::RtgDevice;
using ui::IUiRenderDevice;	
using ui::IUiImageFactory;	
using ui::IUiFontManager;	
using ui::CClipboard;		

class CDriverSystem
{
public:
	enum ESystemType
	{
		UNKNOWN_TYPE = 0,
		RT_AND_GDI_PLUS,		//GDI+ 和 RT系统
		RT_DRIVER,				//RT
	};

	CDriverSystem()
		:mpRenderDevice(NULL),
		mpImageFactory(NULL),
		mpFontManager(NULL),
		mpClipboard(NULL),
		mpRtgDevice(NULL),
		mType(UNKNOWN_TYPE){};

	~CDriverSystem();

	bool Init(const string& vrString,RtgDevice* vpRtgDevice);
	void Exit();

	void RenderClear();
	void RenderFilp();

	IUiRenderDevice*		GetRenderDriver() {return mpRenderDevice;};
	IUiImageFactory*		GetImageFactory() {return mpImageFactory;};
	IUiFontManager*			GetFontManager()  {return mpFontManager;};
	CClipboard*			GetClipboard()    {return mpClipboard;};
private:
	bool CreateSubWindow(HWND vMainWnd);

	IUiRenderDevice*		mpRenderDevice;
	IUiImageFactory*		mpImageFactory;
	IUiFontManager*			mpFontManager;
	CClipboard*				mpClipboard;
	RtgDevice*				mpRtgDevice;
	ESystemType				mType;

	//Windows信息
	HWND			mMainWnd;
	HWND			mSubWnd;
};
#endif