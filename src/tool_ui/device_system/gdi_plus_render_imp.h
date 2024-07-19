#ifndef GDI_PLUS_RENDER_IMP_H
#define GDI_PLUS_RENDER_IMP_H

#include <Windows.h>
#include <gdiplus.h>
#include "ui/rtw_ui.h"
#include "cmylog.h"
using namespace Gdiplus;

class CGdiPlusRenderImp:public IUiRenderDevice
{
public:
	CGdiPlusRenderImp(Graphics* vpBackBuffer,const int vWidth,const int vHeight);
	virtual ~CGdiPlusRenderImp();
public:
	virtual void OnInvalidateDevice();
	virtual void OnRestoreDevice();
	virtual Real getZOrder();
	virtual RtwRect getViewportRect(); 
public:
	virtual void BeginRender();
	virtual void EndRender();
	
	//IUiRenderDevice接口要求的形式
	virtual void DrawImage(RtwImage* vpImage, const RtwRect& vrTargetRect, 
		const SUiTextAlignment vAlign = SUiTextAlignment(alignNear, alignNear), 
		const RtwRect* vpClipRect = NULL); 

	virtual void DrawBorderImage(RtwImage* pImage, const RtwRect& rc, const SMargin& margin,
		const SUiTextAlignment align  = SUiTextAlignment(alignNear, alignNear),
		const RtwRect* pClipRect = NULL);

	virtual void DrawComplexImage(RtwComplexImage* image, const RtwRect& rect);
	virtual void DrawRectangle(const RtwRect& rect, const RtwPixel& color);
	virtual void DrawRectangleBorder(const RtwRect& rect, const RtwPixel& color,const RtwRect* pClipRect) ;
	virtual void DrawLine(const SPoint& startPoint, const SPoint& endPoint, const RtwPixel& color);
    virtual void DrawTriangle(const SPoint& p1, const SPoint& p2, const SPoint& p3, const RtwPixel& color);

protected:
	//辅助函数//处理剪裁
	void DrawImage(RtwImage* vpImage, const RtwRect& vrTargetRect, 
		const EImageFillMode  vFillMode,const SUiTextAlignment vAlign = SUiTextAlignment(alignNear, alignNear),
		const RtwRect* vpClipRect = NULL);

	//剪裁结束后的上图
	void DrawImage(RtwImage* vpImage,const RtwRect& vrTargetRect,const RtwRect& vrSourceRect);
private:
	Real            m_ZOrder;
	Graphics*       mpBuffer;
	int				mWidth;
	int				mHeight;
};
#endif