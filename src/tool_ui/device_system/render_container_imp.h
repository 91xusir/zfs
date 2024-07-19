#ifndef RENDER_CONTAINER_IMP_H 
#define RENDER_CONTAINER_IMP_H

#include "ui/rtw_ui.h"
using ui::CUiRenderDeviceImpl_Rt;
#include "gdi_plus_image_imp.h"
#include "image_container_imp.h"
#include "gdi_plus_render_imp.h"

//渲染器包装
class CRtAndGdiPlusRender:public IUiRenderDevice
{
public:
	CRtAndGdiPlusRender(CUiRenderDeviceImpl_Rt* vpRtRender,CGdiPlusRenderImp* vpGdiPlusRender)
		:mpRtRender(vpRtRender),mpGdiPlusRender(vpGdiPlusRender){/*LOG("");*/};
	
	virtual ~CRtAndGdiPlusRender()
	{/*LOG("");*/
		delete mpRtRender;
		delete mpGdiPlusRender;
	};

public:
	virtual void OnInvalidateDevice() 
	{/*LOG("");*/
		mpRtRender->OnInvalidateDevice();
		mpGdiPlusRender->OnInvalidateDevice();
	};

	virtual void OnRestoreDevice() 
	{/*LOG("");*/
		mpRtRender->OnRestoreDevice();
		mpGdiPlusRender->OnRestoreDevice();
	};

	//获取zorder
	virtual Real getZOrder() 
	{/*LOG("");*/
		return mpRtRender->getZOrder();
	};

	//获取可视的矩阵
	virtual RtwRect getViewportRect() 
	{/*LOG("");*/
		return mpRtRender->getViewportRect();
	};

	//开始渲染
	virtual void BeginRender() 
	{/*LOG("");*/
		mpRtRender->BeginRender();
		mpGdiPlusRender->BeginRender();
	};

	//结束渲染
	virtual void EndRender() 
	{/*LOG("");*/
		mpRtRender->EndRender();
		mpGdiPlusRender->EndRender();
	};

	virtual void DrawImage(RtwImage* vpImage, const RtwRect& vrRect, 
		const SUiTextAlignment vAlign = SUiTextAlignment(alignNear, alignNear), 
		const RtwRect* vpClipRect = NULL);

	virtual void DrawBorderImage(RtwImage* pImage, const RtwRect& rc, const SMargin& margin,  
		const SUiTextAlignment align,const RtwRect* pClipRect);

	virtual void DrawComplexImage(RtwComplexImage* image, const RtwRect& rect);

	virtual void DrawRectangle(const RtwRect& rect, const RtwPixel& color) 
	{/*LOG("");*/
		mpRtRender->DrawRectangle(rect,color);
		mpGdiPlusRender->DrawRectangle(rect,color);
	};

	virtual void DrawRectangleBorder(const RtwRect& rect, const RtwPixel& color,const RtwRect* pClipRect)
	{/*LOG("");*/
		mpRtRender->DrawRectangleBorder(rect,color,NULL);
		mpGdiPlusRender->DrawRectangleBorder(rect,color,NULL);
	};

	virtual void DrawLine(const SPoint& startPoint, const SPoint& endPoint, const RtwPixel& color) 
	{/*LOG("");*/
		mpRtRender->DrawLine(startPoint,endPoint,color);
		mpGdiPlusRender->DrawLine(startPoint,endPoint,color);
	};

    virtual void DrawTriangle(const SPoint& p1, const SPoint& p2, const SPoint& p3, const RtwPixel& color)
    {/*LOG("");*/
        mpRtRender->DrawTriangle(p1, p2, p3, color);
        mpGdiPlusRender->DrawTriangle(p1, p2, p3, color);
    };

private:
	CUiRenderDeviceImpl_Rt*				mpRtRender;
	CGdiPlusRenderImp*					mpGdiPlusRender;
};
#endif