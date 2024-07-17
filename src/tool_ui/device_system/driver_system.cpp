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
	{//rt����
		mpRtgDevice = vpRtgDevice;//ͼ������
		mType = RT_DRIVER;
		
		CUiRenderDeviceImpl_Rt* p = new CUiRenderDeviceImpl_Rt;//UI��Ⱦϵͳ
		p->Init(mpRtgDevice);	//��Ⱦͨ��ͼ�������ʼ��

		mpRenderDevice	= p;
		mpImageFactory	= new CUiImageFactoryImpl_Rt(mpRtgDevice);	//ͼ�ι���
		mpFontManager	= new CUiFontManagerImpl_Rt(mpRtgDevice);	//���������
		mpClipboard		= new CUiClipboardImpl_Windows;				//������
		return true;
	}
	else if(vrString == GDI_PLUS_SYSTEM_AND_RT_SYSTEM )
	{//gdi+ϵͳ��Rtϵͳ
		mpRtgDevice = vpRtgDevice;
		mType = RT_AND_GDI_PLUS;

		////GDI+��ʼ��
		Gdiplus::GdiplusStartupInput StartupInput;
		Gdiplus::GdiplusStartup(&gGDIPlusGUID, &StartupInput, NULL);

		//�Ӵ��ڳ�ʼ��
		CreateSubWindow((HWND)mpRtgDevice->GetWndHandle());

		RECT Rect;
		::GetClientRect(mSubWnd,&Rect);

		gpFrontBuffer =	new Graphics(mSubWnd);
		gpBackImage = new Bitmap (Rect.right-Rect.left,Rect.bottom-Rect.top);
		gpBackBuffer =new Graphics (gpBackImage);

		//���������ĳ�ʼ��
		CUiRenderDeviceImpl_Rt* pRtRender = new CUiRenderDeviceImpl_Rt;//RT��Ⱦ
		pRtRender->Init(mpRtgDevice);//RT��Ⱦ���г�ʼ��
		CGdiPlusRenderImp* pGdiPlusRender = new CGdiPlusRenderImp(
																		gpBackBuffer,
																		Rect.right-Rect.left,
																		Rect.bottom-Rect.top
																  );//GDI+��Ⱦ
		mpRenderDevice = new CRtAndGdiPlusRender(pRtRender,pGdiPlusRender);//������Ⱦ����
		CUiImageFactoryImpl_Rt* pRtImageFactory = new CUiImageFactoryImpl_Rt(mpRtgDevice);//RT image factory
		CUiGdiPlusImageFactoryImp* pGdiPlusImageFactory = new CUiGdiPlusImageFactoryImp();//gdi+ image factory
		mpImageFactory = new CImageFactoryContainerImp(pRtImageFactory,pGdiPlusImageFactory);//������Ⱦ

		CUiFontManagerImpl_Rt* pRtFontManager = new CUiFontManagerImpl_Rt(mpRtgDevice);//rt���ֹ���
		CGdiPlusFontManagerImp* pGdiPlusFontManager = new CGdiPlusFontManagerImp(gpBackBuffer);//gdi+ ���ֹ���
		mpFontManager = new CFontManagerContainerImp(pRtFontManager,pGdiPlusFontManager);//��������
		
		mpClipboard = new CUiClipboardImpl_Windows;//windows���а�

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
//		���ƣ�WinProc
//		���ܣ�windows�Ļص��������Ǵ��ڵĴ������
//		��ڣ����ھ����������Ϣ����Ϣ�ĵ�һ����������Ϣ�ĵڶ�������
//----------------------------------------------------
LRESULT CALLBACK WinProc(HWND vhWnd,
						 UINT vMsg,
						 WPARAM vwParam,
						 LPARAM vlParam)
{/*LOG("");*/

	switch(vMsg)
	{
	case WM_KEYUP://��ť����
	case WM_KEYDOWN://��ť����
		break; 
	case WM_LBUTTONDOWN:	//������������
	case WM_RBUTTONDOWN:	//����Ҽ�������
	case WM_LBUTTONUP:		//����������
	case WM_RBUTTONUP:		//����Ҽ�������
	case WM_LBUTTONDBLCLK:	//������˫��
	case WM_RBUTTONDBLCLK:	//����Ҽ�˫��
	case WM_MOUSEMOVE:		//����ƶ���Ϣ
		break;
	case WM_ACTIVATE:		//���ڼ����ȡ��
		switch(LOWORD(vwParam))
		{
		case WA_CLICKACTIVE:
			break;
		case WA_ACTIVE:
			break;
		case WA_INACTIVE:	//�Ǽ���
			break;
		}
		break;
	case WM_CREATE:			//���ڴ���
		break;
	case WM_PAINT:			//��ͼ��Ϣ
		break;
	case WM_CLOSE:			//���ڹر�
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
	//��Ļ�Ŀͻ�����
	RECT RectScreen;
	::SystemParametersInfo(SPI_GETWORKAREA,0,&RectScreen,0);

	HINSTANCE hInstance = ::GetModuleHandle(NULL);

	RECT Rect;
	mMainWnd = vMainWnd;
	GetWindowRect(mMainWnd,&Rect);

	WNDCLASSEX WinClass;				//�յ�WINDOWS��
	WinClass.cbSize=sizeof(WNDCLASSEX);	//�ýṹ�Ĵ�С
	WinClass.style=CS_HREDRAW|CS_VREDRAW|CS_DBLCLKS|CS_OWNDC;
	WinClass.lpfnWndProc=WinProc;		//ָ���ص�����
	WinClass.cbClsExtra=0;				//ԭ��Ϊ����ʱ����ϢԤ����
	WinClass.cbWndExtra=0;				//ԭ��Ϊ����ʱ����ϢԤ����
	WinClass.hInstance=hInstance;		//�ô������ʵ�����
	WinClass.hIcon=LoadIcon(hInstance,"ICON_FIRST");	//���ɵ�EXE�ļ���ͼͼ��

	//Сͼ�꣨�ݹ۲촴������Ҫ��WS_SYSMENU�ſ��ļ�<��ȷ����>��
	WinClass.hIconSm=LoadIcon(hInstance,"ICON_FOX");		
	WinClass.hCursor=NULL;	//������
	WinClass.hbrBackground=(HBRUSH)GetStockObject(BLACK_BRUSH);	//������ɫ
	WinClass.lpszMenuName=NULL;			//װ�ز˵�
	WinClass.lpszClassName="WinClass1";	//�µ�����(�����ִ�Сд���ݹ۲죬��ȷ��)

	if(RegisterClassEx(&WinClass)==NULL)//��WINDOWע�����
	{
		return false;
	}

	mSubWnd =CreateWindowEx(NULL,		//�߼����ԣ��ϳ��õ������ô�����������WS_EX_TOPMOST
		"WinClass1",					//���봴���Ĵ��������ڵ�����������ղ�ע���������
		"chat",							//�ô��ڵ�����
		WS_VISIBLE|WS_THICKFRAME|WS_SYSMENU,//���ڵ���۱�ʶ
		0,//CW_USEDEFAULT,				//���ڵ�X����
		0,//CW_USEDEFAULT,				//���ڵ�Y����
		Rect.right-Rect.left,			//���ڵĿ�
		Rect.bottom-Rect.top,			//���ڵĸ�
		NULL,							//�ô��ڵĸ����ڣ����û�и��������棬Ҳ����NULL
		NULL,							//�ô��ڵĲ˵������û����NULL
		hInstance,						//Ӧ�ó����ʵ��
		NULL);							//�߼����ԣ�����ΪNULL

	if(mSubWnd==NULL)
	{
		return false;
	}
	SetCursor(LoadCursor(NULL,IDC_ARROW));
	return true;
}