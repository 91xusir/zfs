#include "driver_system.h"
#include "gdi_plus_render_imp.h"
#include "gdi_plus_image_factory_imp.h"
#include "render_container_imp.h"
#include "image_container_imp.h"
#include "image_factory_container_imp.h"
#include "gdi_plus_font_manager_imp.h"
#include "font_manager_container_imp.h"

#include <gdiplus.h>
#pragma comment(lib,"gdiplus.lib")
using namespace Gdiplus;

static ULONG_PTR	gGDIPlusGUID;
Graphics*			gpFrontBuffer(NULL);
Graphics*			gpBackBuffer(NULL);
Bitmap*				gpBackImage(NULL);

CDriverSystem::~CDriverSystem()
{/*LOG("");*/
	delete	mpRenderDevice;
	delete	mpImageFactory;

	if(mpFontManager != NULL)
	{
		mpFontManager->Close();
		delete	mpFontManager;
	}

	delete	mpClipboard;
}

bool CDriverSystem::Init(const string& vrString, RtgDevice* vpRtgDevice)
{/*LOG("");*/
	if(vrString == RT_DRIVER_SYSTEM)
	{//rt引擎
		mpRtgDevice = vpRtgDevice;//图形引擎
		mType = RT_DRIVER;
		
		CUiRenderDeviceImpl_Rt* p = new CUiRenderDeviceImpl_Rt;//UI渲染系统
		p->Init(mpRtgDevice);	//渲染通过图形引擎初始化

		mpRenderDevice	= p;
		mpImageFactory	= new CUiImageFactoryImpl_Rt(mpRtgDevice);	//图形工厂
		mpFontManager	= new CUiFontManagerImpl_Rt(mpRtgDevice);	//字体管理器
		mpClipboard		= new CUiClipboardImpl_Windows;				//剪贴板
		return true;
	}
	else if(vrString == GDI_PLUS_SYSTEM_AND_RT_SYSTEM )
	{//gdi+系统和Rt系统
		mpRtgDevice = vpRtgDevice;
		mType = RT_AND_GDI_PLUS;

		////GDI+初始化
		Gdiplus::GdiplusStartupInput StartupInput;
		Gdiplus::GdiplusStartup(&gGDIPlusGUID, &StartupInput, NULL);

		//子窗口初始化
		CreateSubWindow((HWND)mpRtgDevice->GetWndHandle());

		RECT Rect;
		::GetClientRect(mSubWnd,&Rect);

		gpFrontBuffer =	new Graphics(mSubWnd);
		gpBackImage = new Bitmap (Rect.right-Rect.left,Rect.bottom-Rect.top);
		gpBackBuffer =new Graphics (gpBackImage);

		//各种驱动的初始化
		CUiRenderDeviceImpl_Rt* pRtRender = new CUiRenderDeviceImpl_Rt;//RT渲染
		pRtRender->Init(mpRtgDevice);//RT渲染特有初始化
		CGdiPlusRenderImp* pGdiPlusRender = new CGdiPlusRenderImp(
																		gpBackBuffer,
																		Rect.right-Rect.left,
																		Rect.bottom-Rect.top
																  );//GDI+渲染
		mpRenderDevice = new CRtAndGdiPlusRender(pRtRender,pGdiPlusRender);//联合渲染驱动
		CUiImageFactoryImpl_Rt* pRtImageFactory = new CUiImageFactoryImpl_Rt(mpRtgDevice);//RT image factory
		CUiGdiPlusImageFactoryImp* pGdiPlusImageFactory = new CUiGdiPlusImageFactoryImp();//gdi+ image factory
		mpImageFactory = new CImageFactoryContainerImp(pRtImageFactory,pGdiPlusImageFactory);//联合渲染

		CUiFontManagerImpl_Rt* pRtFontManager = new CUiFontManagerImpl_Rt(mpRtgDevice);//rt文字管理
		CGdiPlusFontManagerImp* pGdiPlusFontManager = new CGdiPlusFontManagerImp(gpBackBuffer);//gdi+ 文字管理
		mpFontManager = new CFontManagerContainerImp(pRtFontManager,pGdiPlusFontManager);//联合文字
		
		mpClipboard = new CUiClipboardImpl_Windows;//windows剪切板

		return true;
	}
	return false;
}

void CDriverSystem::Exit()
{/*LOG("");*/
	if(mType == RT_AND_GDI_PLUS)
	{
		delete gpFrontBuffer;
		delete gpBackImage;
		delete gpBackBuffer;
		Gdiplus::GdiplusShutdown(gGDIPlusGUID);
	}
}

void CDriverSystem::RenderClear()
{/*LOG("");*/
	if(mType == RT_AND_GDI_PLUS)
	{
		gpBackBuffer->Clear(Color(48,160,48));
	}
}

void CDriverSystem::RenderFilp()
{/*LOG("");*/
	if(mType == RT_AND_GDI_PLUS)
	{
		gpFrontBuffer->DrawImage(gpBackImage,0,0);
	}
}

//----------------------------------------------------
//		名称：WinProc
//		功能：windows的回调函数，是窗口的处理过程
//		入口：窗口句柄，传入消息，消息的第一个参数，消息的第二个参数
//----------------------------------------------------
LRESULT CALLBACK WinProc(HWND vhWnd,
						 UINT vMsg,
						 WPARAM vwParam,
						 LPARAM vlParam)
{/*LOG("");*/

	switch(vMsg)
	{
	case WM_KEYUP://按钮弹起
	case WM_KEYDOWN://按钮按下
		break; 
	case WM_LBUTTONDOWN:	//鼠标左键被按下
	case WM_RBUTTONDOWN:	//鼠标右键被按下
	case WM_LBUTTONUP:		//鼠标左键弹起
	case WM_RBUTTONUP:		//鼠标右键被弹起
	case WM_LBUTTONDBLCLK:	//鼠标左键双击
	case WM_RBUTTONDBLCLK:	//鼠标右键双击
	case WM_MOUSEMOVE:		//鼠标移动消息
		break;
	case WM_ACTIVATE:		//窗口激活或取消
		switch(LOWORD(vwParam))
		{
		case WA_CLICKACTIVE:
			break;
		case WA_ACTIVE:
			break;
		case WA_INACTIVE:	//非激活
			break;
		}
		break;
	case WM_CREATE:			//窗口创建
		break;
	case WM_PAINT:			//上图消息
		break;
	case WM_CLOSE:			//窗口关闭
		return 0;
		break;
	case WM_EXITSIZEMOVE:
		break;
	case WM_COMMAND:
		break;
	}
	return DefWindowProc(vhWnd, vMsg, vwParam, vlParam);
}

bool CDriverSystem::CreateSubWindow(HWND vMainWnd)
{/*LOG("");*/
	//屏幕的客户区域
	RECT RectScreen;
	::SystemParametersInfo(SPI_GETWORKAREA,0,&RectScreen,0);

	HINSTANCE hInstance = ::GetModuleHandle(NULL);

	RECT Rect;
	mMainWnd = vMainWnd;
	GetWindowRect(mMainWnd,&Rect);

	WNDCLASSEX WinClass;				//空的WINDOWS类
	WinClass.cbSize=sizeof(WNDCLASSEX);	//该结构的大小
	WinClass.style=CS_HREDRAW|CS_VREDRAW|CS_DBLCLKS|CS_OWNDC;
	WinClass.lpfnWndProc=WinProc;		//指定回调函数
	WinClass.cbClsExtra=0;				//原是为运行时间信息预留的
	WinClass.cbWndExtra=0;				//原是为运行时间信息预留的
	WinClass.hInstance=hInstance;		//该窗口类的实例句柄
	WinClass.hIcon=LoadIcon(hInstance,"ICON_FIRST");	//生成的EXE文件的图图标

	//小图标（据观察创建窗口要用WS_SYSMENU才看的见<不确定！>）
	WinClass.hIconSm=LoadIcon(hInstance,"ICON_FOX");		
	WinClass.hCursor=NULL;	//载入光标
	WinClass.hbrBackground=(HBRUSH)GetStockObject(BLACK_BRUSH);	//背景颜色
	WinClass.lpszMenuName=NULL;			//装载菜单
	WinClass.lpszClassName="WinClass1";	//新的类名(不区分大小写，据观察，不确定)

	if(RegisterClassEx(&WinClass)==NULL)//向WINDOW注册该类
	{
		return false;
	}

	mSubWnd =CreateWindowEx(NULL,		//高级特性，较常用的是总让窗口在最上面WS_EX_TOPMOST
		"WinClass1",					//你想创建的窗口所基于的类名，（你刚才注册的类名）
		"chat",							//该窗口的名称
		WS_VISIBLE|WS_THICKFRAME|WS_SYSMENU,//窗口的外观标识
		0,//CW_USEDEFAULT,				//窗口的X坐标
		0,//CW_USEDEFAULT,				//窗口的Y坐标
		Rect.right-Rect.left,			//窗口的宽
		Rect.bottom-Rect.top,			//窗口的高
		NULL,							//该窗口的父窗口，如果没有父就是桌面，也就是NULL
		NULL,							//该窗口的菜单句柄，没有填NULL
		hInstance,						//应用程序的实例
		NULL);							//高级特性，设置为NULL

	if(mSubWnd==NULL)
	{
		return false;
	}
	SetCursor(LoadCursor(NULL,IDC_ARROW));
	return true;
}