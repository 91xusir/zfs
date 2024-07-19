#include "gdi_plus_render_imp.h"
#include "gdi_plus_image_imp.h"

CGdiPlusRenderImp::CGdiPlusRenderImp(Graphics* vpBackBuffer,const int vWidth,const int vHeight)
:mpBuffer(vpBackBuffer),mWidth(vWidth),mHeight(vHeight){/*LOG("");*/}

CGdiPlusRenderImp::~CGdiPlusRenderImp(){/*LOG("");*/}

void CGdiPlusRenderImp::OnInvalidateDevice(){/*LOG("");*/};
void CGdiPlusRenderImp::OnRestoreDevice(){/*LOG("");*/};
Real CGdiPlusRenderImp::getZOrder() {/*LOG("");*/return m_ZOrder;};

RtwRect CGdiPlusRenderImp::getViewportRect() 
{/*LOG("");*/
	RtwRect rc;
	rc.left = 0;
	rc.top = 0;
	rc.right = mWidth - 1;
	rc.bottom =mHeight - 1;
	return rc;
};

void CGdiPlusRenderImp::BeginRender(){/*LOG("");*/};
void CGdiPlusRenderImp::EndRender(){/*LOG("");*/};

void CGdiPlusRenderImp::DrawImage(RtwImage* vpImage, const RtwRect& vrTargetRect,
								  const EImageFillMode  vFillMode,const SUiTextAlignment vAlign, 
								  const RtwRect* vpClipRect)
{/*LOG("");*/
	//安全检查
	if(vpImage==NULL)
	{
		return;
	}

	if (!vrTargetRect.isValid())
	{
		return;
	}

	CUiGdiPlusImageImp* pImage = (CUiGdiPlusImageImp*) vpImage;


	//对齐
	RtwRect	TargetRect=vrTargetRect;
	RtwRect	SourceRect=pImage->m_Source;

	if(vFillMode!= ImageFillMode_Stretch)
	{//只有不需要缩放的时候才有对齐问题

		TargetRect.ReSize(SSize(SourceRect.getWidth(),SourceRect.getHeight()));			//因为不缩放//改变目标大小
		TargetRect.Align(vrTargetRect,vAlign.align,vAlign.lineAlign);					//根据参数对齐
	}

	//剪裁
	if(vpClipRect!=NULL)
	{//有剪裁
		int SourceClipWidth = (vpClipRect->getWidth() * SourceRect.getWidth() ) / TargetRect.getWidth();
		int SourceClipHeight = (vpClipRect->getHeight() * SourceRect.getHeight() ) / TargetRect.getHeight();
		int SourceClipLeft  = SourceRect.left - 
			(((TargetRect.left-vpClipRect->left)* SourceRect.getWidth()) / TargetRect.getWidth()) ;
		int SourceClipTop   = SourceRect.top - 
			(((TargetRect.top - vpClipRect->top) * SourceRect.getHeight()) / TargetRect.getHeight()); 

		RtwRect SourceClipRect(SourceClipLeft,SourceClipTop,SourceClipLeft+SourceClipWidth-1,SourceClipTop+SourceClipHeight-1);
		SourceRect.ClipBy(SourceClipRect);//源矩形剪裁
		TargetRect.ClipBy(*vpClipRect);//目标矩形剪裁
	}

	if(!TargetRect.isValid())
	{
		return;
	}

	if(!SourceRect.isValid())
	{
		return;
	}

	//绘制
	DrawImage(vpImage,TargetRect,SourceRect);
}

void CGdiPlusRenderImp::DrawImage(RtwImage* vpImage,const RtwRect& vrTargetRect,const RtwRect& vrSourceRect)
{/*LOG("");*/
	CUiGdiPlusImageImp* pImage = (CUiGdiPlusImageImp*)vpImage;
	Bitmap* pBitmap = pImage->mpBitmap.Get();
	if(pBitmap==NULL)
	{
		return ;
	}

	//gdi+要求的矩形格式
	Rect DestRect(vrTargetRect.left,vrTargetRect.top,
		vrTargetRect.getWidth(),vrTargetRect.getHeight());

	if(vpImage->getTransparency() ==1.0 || vpImage->m_bBlend)
	{//完全不透明
		//DrawImage中源Rect的宽高，要减1
		mpBuffer->DrawImage(pBitmap,DestRect,vrSourceRect.left,vrSourceRect.top,
			vrSourceRect.getWidth()-1,vrSourceRect.getHeight()-1,UnitPixel);
	}
	else
	{
		//整幅图片的透明处理
		Real Transparency = vpImage->getTransparency();

		static ColorMatrix TempColorMatrix ={
				1.0f, 0.0f, 0.0f, 0.0f, 0.0f,
				0.0f, 1.0f, 0.0f, 0.0f, 0.0f,
				0.0f, 0.0f, 1.0f, 0.0f, 0.0f,
				0.0f, 0.0f, 0.0f, 1.0f, 0.0f,
				0.0f, 0.0f, 0.0f, 0.0f, 1.0f
		};

		TempColorMatrix.m[3][3] = Transparency;//这个是整幅图像的透明度
		static ImageAttributes ImageAttr;;
		ImageAttr.SetColorMatrix(&TempColorMatrix );

		//DrawImage中源Rect的宽高，要减1
		mpBuffer->DrawImage(pBitmap,DestRect,vrSourceRect.left,vrSourceRect.top,
			vrSourceRect.getWidth()-1,vrSourceRect.getHeight()-1,UnitPixel);
	}
}

void CGdiPlusRenderImp::DrawImage(RtwImage* vpImage, const RtwRect& vrTargetRect, 
								  const SUiTextAlignment vAlign , const RtwRect* vpClipRect)
{/*LOG("");*/
	if(vpImage!=NULL)
	{
		DrawImage(vpImage,vrTargetRect,vpImage->m_FillMode,vAlign,vpClipRect);
	}
};

void CGdiPlusRenderImp::DrawBorderImage(RtwImage* pImage, const RtwRect& rc,
										const SMargin& margin,  const SUiTextAlignment align ,const RtwRect* pClipRect)
{/*LOG("");*/

}
void CGdiPlusRenderImp::DrawComplexImage(RtwComplexImage* image, const RtwRect& rect){/*LOG("");*/}
void CGdiPlusRenderImp::DrawRectangle(const RtwRect& rect, const RtwPixel& color) 
{/*LOG("");*/
	mpBuffer->FillRectangle(&SolidBrush(Color(color)),rect.left,rect.top,rect.right-rect.left,rect.bottom-rect.top);
};

void CGdiPlusRenderImp::DrawRectangleBorder(const RtwRect& rect, const RtwPixel& color,const RtwRect* pClipRect ) 
{/*LOG("");*/
	mpBuffer->DrawRectangle(&Pen(Color(color)),rect.left,rect.top,rect.right-rect.left,rect.bottom-rect.top);
};

void CGdiPlusRenderImp::DrawLine(const SPoint& startPoint, const SPoint& endPoint, const RtwPixel& color) 
{/*LOG("");*/
	mpBuffer->DrawLine(&Pen(Color(color)),startPoint.x,startPoint.y,endPoint.x,endPoint.y);
};

void CGdiPlusRenderImp::DrawTriangle(const SPoint& p1, const SPoint& p2, const SPoint& p3, const RtwPixel& color){/*LOG("");*/};
