#include "render_container_imp.h"

void CRtAndGdiPlusRender::DrawImage(RtwImage* vpImage, const RtwRect& vrRect, 
									const SUiTextAlignment vAlign  , const RtwRect* vpClipRect)
{/*LOG("");*/
	CRtAndGdiPlusImage* pImage =(CRtAndGdiPlusImage*)vpImage;
	mpRtRender->DrawImage(pImage->mpRtImage,vrRect,vAlign,vpClipRect);
	mpGdiPlusRender->DrawImage(pImage->mpGdiPlusImage,vrRect,vAlign,vpClipRect);

	return;
};
void CRtAndGdiPlusRender::DrawBorderImage(RtwImage* pImage, const RtwRect& rc,const SMargin& margin, 
							const SUiTextAlignment align ,const RtwRect* pClipRect){/*LOG("");*/}
void CRtAndGdiPlusRender::DrawComplexImage(RtwComplexImage* image, const RtwRect& rect){/*LOG("");*/}
