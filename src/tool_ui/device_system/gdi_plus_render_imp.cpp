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
	//��ȫ���
	if(vpImage==NULL)
	{
		return;
	}

	if (!vrTargetRect.isValid())
	{
		return;
	}

	CUiGdiPlusImageImp* pImage = (CUiGdiPlusImageImp*) vpImage;


	//����
	RtwRect	TargetRect=vrTargetRect;
	RtwRect	SourceRect=pImage->m_Source;

	if(vFillMode!= ImageFillMode_Stretch)
	{//ֻ�в���Ҫ���ŵ�ʱ����ж�������

		TargetRect.ReSize(SSize(SourceRect.getWidth(),SourceRect.getHeight()));			//��Ϊ������//�ı�Ŀ���С
		TargetRect.Align(vrTargetRect,vAlign.align,vAlign.lineAlign);					//���ݲ�������
	}

	//����
	if(vpClipRect!=NULL)
	{//�м���
		int SourceClipWidth = (vpClipRect->getWidth() * SourceRect.getWidth() ) / TargetRect.getWidth();
		int SourceClipHeight = (vpClipRect->getHeight() * SourceRect.getHeight() ) / TargetRect.getHeight();
		int SourceClipLeft  = SourceRect.left - 
			(((TargetRect.left-vpClipRect->left)* SourceRect.getWidth()) / TargetRect.getWidth()) ;
		int SourceClipTop   = SourceRect.top - 
			(((TargetRect.top - vpClipRect->top) * SourceRect.getHeight()) / TargetRect.getHeight()); 

		RtwRect SourceClipRect(SourceClipLeft,SourceClipTop,SourceClipLeft+SourceClipWidth-1,SourceClipTop+SourceClipHeight-1);
		SourceRect.ClipBy(SourceClipRect);//Դ���μ���
		TargetRect.ClipBy(*vpClipRect);//Ŀ����μ���
	}

	if(!TargetRect.isValid())
	{
		return;
	}

	if(!SourceRect.isValid())
	{
		return;
	}

	//����
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

	//gdi+Ҫ��ľ��θ�ʽ
	Rect DestRect(vrTargetRect.left,vrTargetRect.top,
		vrTargetRect.getWidth(),vrTargetRect.getHeight());

	if(vpImage->getTransparency() ==1.0 || vpImage->m_bBlend)
	{//��ȫ��͸��
		//DrawImage��ԴRect�Ŀ�ߣ�Ҫ��1
		mpBuffer->DrawImage(pBitmap,DestRect,vrSourceRect.left,vrSourceRect.top,
			vrSourceRect.getWidth()-1,vrSourceRect.getHeight()-1,UnitPixel);
	}
	else
	{
		//����ͼƬ��͸������
		Real Transparency = vpImage->getTransparency();

		static ColorMatrix TempColorMatrix ={
				1.0f, 0.0f, 0.0f, 0.0f, 0.0f,
				0.0f, 1.0f, 0.0f, 0.0f, 0.0f,
				0.0f, 0.0f, 1.0f, 0.0f, 0.0f,
				0.0f, 0.0f, 0.0f, 1.0f, 0.0f,
				0.0f, 0.0f, 0.0f, 0.0f, 1.0f
		};

		TempColorMatrix.m[3][3] = Transparency;//���������ͼ���͸����
		static ImageAttributes ImageAttr;;
		ImageAttr.SetColorMatrix(&TempColorMatrix );

		//DrawImage��ԴRect�Ŀ�ߣ�Ҫ��1
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
