#include "ui/rtw_ui.h"
#include "character/actor.h"
#define   ENTER_SYMBOL     '\n'

RT_IMPLEMENT_DYNAMIC(CUiImageImpl_Rt, RtwImage, 0, "ui")

CUiImageImpl_Rt::CUiImageImpl_Rt(RtgDevice* pRtgDevice):m_pRtgDevice(pRtgDevice)
{
	m_Image.iNumStage = 1;
	m_Image.Shaders[0].SetTexture(0);
	m_Image.Shaders[0].eColorOp = RTGTOP_MODULATE;
	m_Image.Shaders[0].eColorArg1 = RTGTA_TEXTURE;
	m_Image.Shaders[0].eColorArg2 = RTGTA_DIFFUSE;
	m_Image.Shaders[0].eAlphaOp = RTGTOP_MODULATE;
}

CUiImageImpl_Rt::~CUiImageImpl_Rt(){}

void CUiImageImpl_Rt::OnBeforeDestroy()
{
	m_pRtgDevice->DeleteShader(&m_Image);

	RtwImage::OnBeforeDestroy();
}

bool CUiImageImpl_Rt::LoadFromFile(const std::string& filename)
{
	if (m_Image.Shaders[0].GetTexture() != 0)
		m_pRtgDevice->DeleteShader(&m_Image);

	if (filename.empty())
	{
		m_Image.Shaders[0].SetTexture(0);
		return false;
	}
	else
	{
        RtgTextItem* texItem = RtGetRender()->GetTextMgr()->CreateUiTexture(filename.c_str(), D3DX_DEFAULT_NONPOW2, D3DX_DEFAULT_NONPOW2);

        if (!texItem)
            return false;

		m_Image.Shaders[0].SetTexture(texItem);
        SetSourceRect(RtwRect(0, 0, texItem->width, texItem->height));
	}

	m_FileName = filename;

	return true;
}

bool CUiImageImpl_Rt::LoadFromMemory(const void* data, int length)
{
	if (data==NULL || length==0)
	{
		return false;
	}

	if (m_Image.Shaders[0].GetTexture() != 0)
		m_pRtgDevice->DeleteShader(&m_Image);

    RtgTextItem* texItem = RtGetRender()->GetTextMgr()->CreateTextureFromFileMemery(
        (void*)data, length, D3DX_DEFAULT, D3DX_DEFAULT, D3DX_UI_MIPLEVEL);

    if (!texItem)
        return false;

	m_Image.Shaders[0].SetTexture(texItem);
    SetSourceRect(RtwRect(0, 0, texItem->width, texItem->height));

	return true;
}


bool CUiImageImpl_Rt::LoadFromMemory(int width, int height, int length, const void* data)
{
	if (data==NULL || width==0 || height==0)
	{
		return false;
	}

	if (m_Image.Shaders[0].GetTexture() != 0)
		m_pRtgDevice->DeleteShader(&m_Image);

	RtgTextItem* texItem = RtGetRender()->GetTextMgr()->CreateTextureFromARGBData(
		(void*)data, length, width, height, D3DX_UI_MIPLEVEL);

	if (!texItem)
		return false;

	m_Image.Shaders[0].SetTexture(texItem);
	SetSourceRect(RtwRect(0, 0, texItem->width, texItem->height));

	return true;
}

bool CUiImageImpl_Rt::LoadFromRtArchive(RtArchive *pArc,int length,bool bflip/* = false*/)
{
	if (pArc==NULL || length==0)
	{
		return false;
	}

	if (m_Image.Shaders[0].GetTexture() != 0)
		m_pRtgDevice->DeleteShader(&m_Image);

    RtgTextItem* texItem = RtGetRender()->GetTextMgr()->CreateTextureFromArchive(pArc);

    if (!texItem)
        return false;

	m_Image.Shaders[0].SetTexture(texItem);
    SetSourceRect(RtwRect(0, 0, texItem->width, texItem->height));

	return true;
}

void CUiImageImpl_Rt::SetTransparency(Real trans)
{
	m_Transparency = trans;
	if (m_Transparency > 1.0)
		m_Transparency = 1.0;
	else if (m_Transparency < 0.0)
		m_Transparency = 0.0;

	if (m_Transparency == 0.0)
	{
		m_Color.a = 0xFF;
	}
	else
	{
		m_Color.a = 255 * trans;
	}
}

void CUiImageImpl_Rt::SetBlend(bool bBlend)
{
	if (m_bBlend == bBlend)
		return;

	m_bBlend = bBlend;

	if (m_bBlend == true)
	{
		m_Image.bColorKey = false;
		m_Image.bEnableBlendFunc = true; // @@@@@ 如果bColorKey为true，bEnableBlendFunc就无效
		m_Image.eBlendSrcFunc = RTGBLEND_SRCALPHA;
		m_Image.eBlendDstFunc = RTGBLEND_INVSRCALPHA;
		m_Image.iNumStage = 1;
		m_Image.Shaders[0].eColorOp = RTGTOP_SELECTARG1;
		m_Image.Shaders[0].eColorArg1 = RTGTA_TEXTURE;
		m_Image.Shaders[0].eColorArg2 = RTGTA_DIFFUSE;
		m_Image.Shaders[0].eAlphaOp = RTGTOP_MODULATE;
		m_Image.Shaders[0].eAlphaArg1 = RTGTA_TEXTURE;
		m_Image.Shaders[0].eAlphaArg2 = RTGTA_DIFFUSE;
	}
	else
	{
		m_Image.bColorKey = false;
	}
}

CUiImageFactoryImpl_Rt::CUiImageFactoryImpl_Rt(RtgDevice* pRtgDevice)
{
	m_pRtgDevice = pRtgDevice;
}

CUiImageFactoryImpl_Rt::~CUiImageFactoryImpl_Rt()
{
}

RtwImage* CUiImageFactoryImpl_Rt::RealCreateImage()
{
	RtwImage* pImage = RT_NEW CUiImageImpl_Rt(m_pRtgDevice);
	return pImage;
}

RtwImage* CUiImageFactoryImpl_Rt::createImage(const std::string& filename)
{
	if (filename.empty())
		return NULL;

	RtwImage* pImage = RT_NEW CUiImageImpl_Rt(m_pRtgDevice);
	pImage->grab();

	if (pImage)
	{
		EXT_SPACE::unordered_map<RtwImage*, DWORD>::iterator iterFinder = m_LiveImages.find(pImage);
		UI_CHECK_P(iterFinder==m_LiveImages.end());
		m_LiveImages[pImage] = 0;
	}

	if (!pImage->LoadFromFile(filename))
	{
		DROP_RTUI_OBJECT(pImage);
		return NULL;
	}

	return pImage;
}

CUiRenderDeviceImpl_Rt::CUiRenderDeviceImpl_Rt()
{
	m_ZOrder = 1.0f;
	m_pRtgDevice = NULL;
	m_IB = 0;

	m_DefaultShader.bColorKey             = false;
	m_DefaultShader.bEnableBlendFunc      = true;
	m_DefaultShader.eBlendSrcFunc         = RTGBLEND_SRCALPHA;
	m_DefaultShader.eBlendDstFunc         = RTGBLEND_INVSRCALPHA;
	m_DefaultShader.iNumStage             = 1;
	m_DefaultShader.Shaders[0].eColorOp   = RTGTOP_SELECTARG1;
	m_DefaultShader.Shaders[0].eColorArg1 = RTGTA_DIFFUSE;
	m_DefaultShader.Shaders[0].eAlphaOp   = RTGTOP_MODULATE;
	m_DefaultShader.Shaders[0].eAlphaArg1 = RTGTA_TEXTURE;
	m_DefaultShader.Shaders[0].eAlphaArg2 = RTGTA_DIFFUSE;
}

CUiRenderDeviceImpl_Rt::~CUiRenderDeviceImpl_Rt(){}

bool CUiRenderDeviceImpl_Rt::Init(RtgDevice* pRtgDevice)
{
	UI_CHECK_B(pRtgDevice);
	m_pRtgDevice = pRtgDevice;
	return true;
}

void CUiRenderDeviceImpl_Rt::OnInvalidateDevice(){}
void CUiRenderDeviceImpl_Rt::OnRestoreDevice(){}

BOOL  s_bFog;
DWORD s_dwZTest;
DWORD s_dwFSAA;
DWORD s_dwLighting;
DWORD s_dwEnvironmentLighting;
DWORD s_dwZFunc;
DWORD s_dwZWrite;
DWORD s_dwEmissiveMaterialSource;
DWORD s_dwDiffuseMaterialSource;

void CUiRenderDeviceImpl_Rt::BeginRender()
{
	m_bDrawing = true;

	s_bFog = m_pRtgDevice->GetFogEnable();
	s_dwZTest = m_pRtgDevice->GetRenderState( RTGRS_Z_TEST );
	s_dwFSAA = m_pRtgDevice->GetRenderState( RTGRS_FULLSCRENE_ANTIALIASING );
	s_dwLighting = m_pRtgDevice->GetRenderState( RTGRS_LIGHTING );
	s_dwEnvironmentLighting = m_pRtgDevice->GetRenderState( RTGRS_ENVIRONMENT_LIGHTING );
	s_dwZFunc = m_pRtgDevice->GetRenderState( RTGRS_Z_FUNC );
	s_dwZWrite = m_pRtgDevice->GetRenderState( RTGRS_Z_WRITE );
	s_dwEmissiveMaterialSource = m_pRtgDevice->GetRenderState(RTGRS_EMISSIVEMATERIALSOURCE);
	s_dwDiffuseMaterialSource  = m_pRtgDevice->GetRenderState(RTGRS_DIFFUSEMATERIALSOURCE);

	m_pRtgDevice->SetTextureFilterMin(RTGTEXF_POINT);
	m_pRtgDevice->SetTextureFilterMag(RTGTEXF_POINT);
	m_pRtgDevice->SetTextureFilterMip(RTGTEXF_NONE);
	m_pRtgDevice->SetDefaultMaterial();
	m_pRtgDevice->SetFogEnable(FALSE);
	m_pRtgDevice->SetRenderState( RTGRS_Z_TEST,                     FALSE );
	m_pRtgDevice->SetRenderState( RTGRS_FULLSCRENE_ANTIALIASING,    FALSE );
	m_pRtgDevice->SetRenderState( RTGRS_LIGHTING,                   FALSE );
	m_pRtgDevice->SetRenderState( RTGRS_ENVIRONMENT_LIGHTING,       0xffffffff );
	m_pRtgDevice->SetRenderState( RTGRS_Z_FUNC,					    RTGCMP_EQUAL );
	m_pRtgDevice->SetRenderState(RTGRS_Z_WRITE, FALSE);
	m_pRtgDevice->SetRenderState(RTGRS_EMISSIVEMATERIALSOURCE,      RTGMCS_MATERIAL);
	m_pRtgDevice->SetRenderState(RTGRS_DIFFUSEMATERIALSOURCE,       RTGMCS_COLOR1);
	m_pRtgDevice->SetTextureAddress(RTGTADD_CLAMP);
	m_pRtgDevice->ClearRenderTarget(false, false, true);

	m_ZOrder = 1.0f;
}

void CUiRenderDeviceImpl_Rt::EndRender()
{
	m_pRtgDevice->SetFogEnable(s_bFog);
	m_pRtgDevice->SetRenderState( RTGRS_Z_TEST,                     s_dwZTest );
	m_pRtgDevice->SetRenderState( RTGRS_FULLSCRENE_ANTIALIASING,    s_dwFSAA );
	m_pRtgDevice->SetRenderState( RTGRS_LIGHTING,                   s_dwLighting );
	m_pRtgDevice->SetRenderState( RTGRS_ENVIRONMENT_LIGHTING,       s_dwEnvironmentLighting);
	m_pRtgDevice->SetRenderState( RTGRS_Z_FUNC,					    s_dwZFunc );
	m_pRtgDevice->SetRenderState(RTGRS_Z_WRITE,                     s_dwZWrite);
	m_pRtgDevice->SetTextureFilterMin(RTGTEXF_LINEAR);
	m_pRtgDevice->SetTextureFilterMag(RTGTEXF_LINEAR);
	m_pRtgDevice->SetTextureFilterMip(RTGTEXF_LINEAR);
	m_pRtgDevice->SetRenderState(RTGRS_EMISSIVEMATERIALSOURCE,      s_dwEmissiveMaterialSource);
	m_pRtgDevice->SetRenderState(RTGRS_DIFFUSEMATERIALSOURCE,       s_dwDiffuseMaterialSource);
	m_pRtgDevice->SetTextureAddress(RTGTADD_WRAP);

	m_bDrawing = false;
}

void CUiRenderDeviceImpl_Rt::DrawImage(RtwImage* pImage, const RtwRect& rc, const SUiTextAlignment align, const RtwRect* pClipRect)
{
	if (!pImage)
		return;
	if (!rc.isValid())
		return;

	RtwRect rcAfterClip = rc;
	if (pClipRect)
	{
		rcAfterClip.ClipBy(*pClipRect);
	}
	if (!rcAfterClip.isValid())
		return;

	CUiImageImpl_Rt* pRtImage = (CUiImageImpl_Rt*)pImage;

    RtgTextItem* texItem = pRtImage->m_Image.Shaders[0].GetTexture();

    if (!texItem)
        return;

	float tl = 0.0f;
	float tr = 0.0f;
	float tt = 0.0f;
	float tb = 0.0f;
	if (pImage->m_FillMode == ImageFillMode_Stretch)
	{
		tl = (Real)pImage->getSourceRect().left / (texItem->width - 1);
		tr = (Real)pImage->getSourceRect().right / (texItem->width - 1);
		tt = (Real)pImage->getSourceRect().top / (texItem->height - 1);// - 0.000001;
		tb = (Real)pImage->getSourceRect().bottom / (texItem->height - 1);// - 0.000001;

		if (pClipRect)
		{
			if (pClipRect->left > rc.left)
				tl += (float)(pClipRect->left - rc.left) / rc.getWidth() * (tr - tl);
			if (pClipRect->right < rc.right)
				tr -= (float)(rc.right - pClipRect->right) / rc.getWidth() * (tr - tl);
			if (pClipRect->top > rc.top)
				tt += (float)(pClipRect->top - rc.top) / rc.getHeight() * (tb - tt);
			if (pClipRect->bottom < rc.bottom)
				tb -= (float)(rc.bottom - pClipRect->bottom) / rc.getHeight() * (tb - tt);
		}
	}
	else 
	{
		int _Width = rc.getWidth();
		if (_Width > pImage->getSize().width)
			_Width = pImage->getSize().width;

		int _Height = rc.getHeight();
		if (_Height > pImage->getSize().height)
			_Height = pImage->getSize().height;

		switch (align.align)
		{
		case alignNear:
			{
				tl = (Real)pImage->getSourceRect().left / (texItem->width - 1);
				tr = (Real)(pImage->getSourceRect().left + _Width - 1) / (texItem->width - 1);
			}
			break;
		case alignCenter:
			{
				tl = (Real)pImage->getSourceRect().left / (texItem->width - 1);
				tr = (Real)(pImage->getSourceRect().left + _Width - 1) / (texItem->width - 1);
			}
			break;
		case alignFar:
			{
				tl = (Real)(pImage->getSourceRect().right - _Width + 1) / (texItem->width - 1);
				tr = (Real)pImage->getSourceRect().right / (texItem->width - 1);
			}
			break;
		default:
			break;
		}
		switch (align.lineAlign)
		{
		case alignNear:
			{
				tt = (Real)pImage->getSourceRect().top / (texItem->height - 1);
				tb = (Real)(pImage->getSourceRect().top + _Height - 1) / (texItem->height - 1);
			}
			break;
		case alignCenter:
			{
				tt = (Real)pImage->getSourceRect().bottom / (texItem->height - 1);
				tb = (Real)(pImage->getSourceRect().bottom + _Height - 1) / (texItem->height - 1);
			}
			break;
		case alignFar:
			{
				tt = (Real)(pImage->getSourceRect().bottom - _Height + 1) / (texItem->height - 1);
				tb = (Real)pImage->getSourceRect().bottom / (texItem->height - 1);
			}
			break;
		default:
			break;
		}
	}

	float left = rcAfterClip.left;
	float right = rcAfterClip.right + 1;
	float top = rcAfterClip.top;
	float bottom = rcAfterClip.bottom + 1 ;

	DWORD color= pRtImage->m_Color;

	SImageVertex* pVerts = NULL;
	DWORD resId = 0;
    RtgShareBufferItem _shItem;
	pVerts = (SImageVertex*)RtGetRender()->GetBuffMgr()->Lock_Shared_vb(sizeof(SImageVertex)*4, &_shItem);
        
	*pVerts++ = SImageVertex(left,	top,	m_ZOrder, color, tl, tt);
	*pVerts++ = SImageVertex(left,	bottom,	m_ZOrder, color, tl, tb);
	*pVerts++ = SImageVertex(right,	bottom,	m_ZOrder, color, tr, tb);
	*pVerts++ = SImageVertex(right,	top,	m_ZOrder, color, tr, tt);

	RtGetRender()->GetBuffMgr()->Unlock(&_shItem);

	if (pImage->m_bBlend)
		m_pRtgDevice->SetRenderState(RTGRS_ALPHA_BLEND, 1);
	else 
		m_pRtgDevice->SetRenderState(RTGRS_ALPHA_BLEND, 0);

	if( pRtImage->m_bHighLight )
	{
		pRtImage->m_Image.Shaders[ 0 ].eColorOp = RTGTOP_MODULATE4X;
	}else
	{
		pRtImage->m_Image.Shaders[ 0 ].eColorOp = RTGTOP_SELECTARG1;
	}

	if (pRtImage)
	{
		m_pRtgDevice->SetShader(&(pRtImage->m_Image));
        m_pRtgDevice->SetVertexFormat(RTG_TRANSFORMED_VCT);
        m_pRtgDevice->DrawPrimitive(D3DPT_TRIANGLEFAN, &_shItem, sizeof(SImageVertex), 2);
	}
}

void CUiRenderDeviceImpl_Rt::DrawBorderImage(RtwImage* pImage, const RtwRect& rc, const SMargin& margin, const SUiTextAlignment align /* = SUiTextAlignment(alignNear ,alignNear)*/,const RtwRect* pClipRect/* = NULL */)
{
	funguard;
	if (!pImage)
		return;
	if (!rc.isValid())
		return;
	RtwRect rcAfterClip = rc;
	rcAfterClip.left=rc.left;
	rcAfterClip.top=rc.top;
	if (pClipRect)
	{
		rcAfterClip.ClipBy(*pClipRect);
	}
	if (!rcAfterClip.isValid())
		return;

	CUiImageImpl_Rt* pRtImage = NULL;
	if (0) //(!pImage->IsKindOf(CUiImageImpl_Rt::GetRuntimeClass()))
	{
		CHECK(0);
		return;
	}
	else 
	{
		pRtImage = (CUiImageImpl_Rt*)(pImage->getRenderImage());
	}

    RtgTextItem* texItem = pRtImage->m_Image.Shaders[0].GetTexture();

    if (!texItem)
        return;

	DWORD color = pImage->m_Color;

	float l = rcAfterClip.left ;
	float r = rcAfterClip.right;
	float t = rcAfterClip.top ;
	float b = rcAfterClip.bottom;	

	float tw = texItem->width;//pImage->getSize().width;
	float th = texItem->height;//pImage->getSize().height;

	float tl = pImage->getSourceRect().left / tw;
	float tr = pImage->getSourceRect().right / tw;
	float tt = pImage->getSourceRect().top / th;
	float tb = pImage->getSourceRect().bottom / th;

	float tbs[4];

	const SMargin& bs = margin;

	tbs[0] = bs.left > 0 ? bs.left / tw : 0;
	tbs[1] = bs.top > 0 ? bs.top / th : 0;
	tbs[2] = bs.right > 0 ? bs.right / tw : 0;
	tbs[3] = bs.bottom > 0 ? bs.bottom / th : 0;

	float xinl = l + bs.left; 
	float xinr = r - bs.right;
	float yint = t + bs.top;
	float yinb = b - bs.bottom;

	float txinl = tl + tbs[0];
	float txinr = tr - tbs[2];
	float tyint = (tt + tbs[1]);	
	float tyinb = (tb - tbs[3]);

    RtgShareBufferItem _ibItem;
	WORD* inds = (WORD*)RtGetRender()->GetBuffMgr()->Lock_Shared_ib( sizeof(WORD)*48, &_ibItem);
        

	// TOP
	*inds++ = 0; *inds++ = 4; *inds++ = 5; *inds++ = 0; *inds++ = 5; *inds++ = 1;
	*inds++ = 1; *inds++ = 5; *inds++ = 6; *inds++ = 1; *inds++ = 6; *inds++ = 2;
	*inds++ = 2; *inds++ = 6; *inds++ = 7; *inds++ = 2; *inds++ = 7; *inds++ = 3;

	// MIDDLE
	*inds++ = 4; *inds++ = 8; *inds++ = 9; *inds++ = 4; *inds++ = 9; *inds++ = 5;
	*inds++ = 6; *inds++ = 10; *inds++ = 11; *inds++ = 6; *inds++ = 11; *inds++ = 7;

	// BOTTOM
	*inds++ = 8; *inds++ = 12; *inds++ = 13; *inds++ = 8; *inds++ = 13; *inds++ = 9;
	*inds++ = 9; *inds++ = 13; *inds++ = 14; *inds++ = 9; *inds++ = 14; *inds++ = 10;
	*inds++ = 10; *inds++ = 14; *inds++ = 15; *inds++ = 10; *inds++ = 15; *inds++ = 11;

	RtGetRender()->GetBuffMgr()->Unlock(&_ibItem);


    RtgShareBufferItem _vbItem;
	SImageVertex* pVerts = (SImageVertex*)RtGetRender()->GetBuffMgr()->Lock_Shared_vb(sizeof(SImageVertex)*16, &_vbItem);
        


	*pVerts++ = SImageVertex(l,		t,		m_ZOrder, color, tl,		tt);
	*pVerts++ = SImageVertex(xinl,	t,		m_ZOrder, color, txinl,		tt);
	*pVerts++ = SImageVertex(xinr,	t,		m_ZOrder, color, txinr,		tt);
	*pVerts++ = SImageVertex(r,		t,		m_ZOrder, color, tr,		tt);

	*pVerts++ = SImageVertex(l,	   yint,	m_ZOrder, color, tl,	    tyint);
	*pVerts++ = SImageVertex(xinl,	yint,	m_ZOrder, color, txinl,		tyint);
	*pVerts++ = SImageVertex(xinr,	yint,	m_ZOrder, color, txinr,		tyint);
	*pVerts++ = SImageVertex(r,		yint,	m_ZOrder, color, tr,		tyint);

	*pVerts++ = SImageVertex(l,		yinb,	m_ZOrder, color, tl,		tyinb);
	*pVerts++ = SImageVertex(xinl,	yinb,	m_ZOrder, color, txinl,		tyinb);
	*pVerts++ = SImageVertex(xinr,	yinb,	m_ZOrder, color, txinr,		tyinb);
	*pVerts++ = SImageVertex(r,		yinb,	m_ZOrder, color, tr,		tyinb);

	*pVerts++ = SImageVertex(l,		b,		m_ZOrder, color, tl,		tb);
	*pVerts++ = SImageVertex(xinl,	b,		m_ZOrder, color, txinl,		tb);
	*pVerts++ = SImageVertex(xinr,	b,		m_ZOrder, color, txinr,		tb);
	*pVerts++ = SImageVertex(r,		b,		m_ZOrder, color, tr,		tb);

	RtGetRender()->GetBuffMgr()->Unlock(&_vbItem);

	if (pImage->m_bBlend)
		m_pRtgDevice->SetRenderState(RTGRS_ALPHA_BLEND, 1);
	else 
		m_pRtgDevice->SetRenderState(RTGRS_ALPHA_BLEND, 0);

	m_pRtgDevice->SetShader(&(pRtImage->m_Image));
    m_pRtgDevice->SetVertexFormat(RTG_TRANSFORMED_VCT);

    m_pRtgDevice->DrawIndexedPrimitive(
        D3DPT_TRIANGLELIST,
        &_vbItem,
        sizeof(SImageVertex),
        &_ibItem,
        16
        );
    /*
    RtGetRender()->SetStreamSource(0, RtGetRender()->GetBuffMgr()->GetShared_vb(), sizeof(SImageVertex));
    RtGetRender()->SetIndices(RtGetRender()->GetBuffMgr()->GetShared_ib());
    RtGetRender()->DrawIndexedPrimitive(
        D3DPT_TRIANGLELIST,
        0,
        0,
        16,
        0,
        16)
    */

	fununguard;
}
void CUiRenderDeviceImpl_Rt::DrawComplexImage(RtwComplexImage* image, const RtwRect& rect)
{
	funguard;

	UI_CHECK(image);

	RtwRect innerRect;

	if (image->border)
	{	
		DrawBorderImage(image->border,rect,image->borderSize);
	}

	if (image->getRenderImage())
	{
		RtwRect rc = rect;
		rc.Deflate(image->borderSize);

		DrawImage(image->getRenderImage(), rc);
	}

	fununguard;
}

void CUiRenderDeviceImpl_Rt::SetTextureRenderTarget( RtgTextItem* texItem , bool bClearUp , long nBackColor )
{
    if (!texItem || m_pRtgDevice)
        return;

    m_pRtgDevice->SetRenderTarget(texItem);

    if (bClearUp)
        m_pRtgDevice->ClearRenderTarget( false, true , false , nBackColor );
}
void CUiRenderDeviceImpl_Rt::RestorRenderTarget( )
{
	if( m_pRtgDevice )
	{
		m_pRtgDevice->RestoreRenderTarget();
	}
}
void CUiRenderDeviceImpl_Rt::DrawRectangle(const RtwRect& rect, const RtwPixel& color)
{
	if (rect.getWidth() <= 0 || rect.getHeight() <= 0)
		return;

	if (color.a == 0xFF)
		m_pRtgDevice->SetRenderState(RTGRS_ALPHA_BLEND, 0);
	else 
		m_pRtgDevice->SetRenderState(RTGRS_ALPHA_BLEND, 1);

    RtgShareBufferItem _vbItem;
	SShapeVertex* pVerts = (SShapeVertex*)RtGetRender()->GetBuffMgr()->Lock_Shared_vb(sizeof(SShapeVertex)*4, &_vbItem);
        

	float left = rect.left;
	float right = rect.right + 1;
	float top = rect.top;
	float bottom = rect.bottom + 1;

	*pVerts++ = SShapeVertex(left,   top,     m_ZOrder, color);
	*pVerts++ = SShapeVertex(left,   bottom,  m_ZOrder, color);
	*pVerts++ = SShapeVertex(right,  bottom,  m_ZOrder, color);
	*pVerts++ = SShapeVertex(right,  top,     m_ZOrder, color);

	RtGetRender()->GetBuffMgr()->Unlock(&_vbItem);
	
    m_pRtgDevice->SetVertexFormat(RTG_TRANSFORMED_VC);
    m_pRtgDevice->UseTexture(0, NULL);
    m_pRtgDevice->SetShader(&m_DefaultShader);
    m_pRtgDevice->DrawPrimitive(D3DPT_TRIANGLEFAN, &_vbItem, sizeof(SShapeVertex), 2);

    /*
	RtGetRender()->SetStreamSource(0, RtGetRender()->GetBuffMgr()->GetShared_vb(), sizeof(SShapeVertex));
    RtGetRender()->DrawPrimitive(D3DPT_TRIANGLEFAN, 0, 2);
    */
}

void CUiRenderDeviceImpl_Rt::DrawRectangleBorder(const RtwRect& rect, const RtwPixel& color,const RtwRect* pClipRect)
{

	if (!rect.isValid())
		return;

	RtwRect rcAfterClip = rect;

	if (pClipRect)
	{
		rcAfterClip.ClipBy(*pClipRect);
	}
	if (!rcAfterClip.isValid())
		return;

	if (rect.getWidth() <= 0 || rect.getHeight() <= 0)
		return;

	if (color.a == 0xFF)
		m_pRtgDevice->SetRenderState(RTGRS_ALPHA_BLEND, 0);
	else 
		m_pRtgDevice->SetRenderState(RTGRS_ALPHA_BLEND, 1);

    RtgShareBufferItem _vbItem;
	SShapeVertex* pVerts = (SShapeVertex*)RtGetRender()->GetBuffMgr()->Lock_Shared_vb(sizeof(SShapeVertex)*5, &_vbItem);
        
	float left = rcAfterClip.left;
	float right = rcAfterClip.right;
	float top = rcAfterClip.top;
	float bottom = rcAfterClip.bottom;

	SShapeVertex* pTempVerts = pVerts;
	*pTempVerts++ = SShapeVertex(left,   top,     m_ZOrder, color);
	*pTempVerts++ = SShapeVertex(left,   bottom,  m_ZOrder, color);
	*pTempVerts++ = SShapeVertex(right,  bottom,  m_ZOrder, color);
	*pTempVerts++ = SShapeVertex(right,  top,     m_ZOrder, color);
	*pTempVerts++ = SShapeVertex(left,   top,     m_ZOrder, color);

	RtGetRender()->GetBuffMgr()->Unlock(&_vbItem);

	m_pRtgDevice->SetVertexFormat(RTG_TRANSFORMED_VC);
    m_pRtgDevice->UseTexture(0, 0);
    m_pRtgDevice->SetShader(&m_DefaultShader);
    m_pRtgDevice->DrawPrimitive(D3DPT_LINESTRIP, &_vbItem, sizeof(SShapeVertex), 4);

    /*
	RtGetRender()->SetStreamSource(0, RtGetRender()->GetBuffMgr()->GetShared_vb(), sizeof(SShapeVertex));
    RtGetRender()->DrawPrimitive(D3DPT_LINESTRIP, 0, 4);
    */
}

void CUiRenderDeviceImpl_Rt::DrawLine(const SPoint& startPoint, const SPoint& endPoint, const RtwPixel& color)
{
	if (color.a == 0xFF)
		m_pRtgDevice->SetRenderState(RTGRS_ALPHA_BLEND, 0);
	else 
		m_pRtgDevice->SetRenderState(RTGRS_ALPHA_BLEND, 1);

    RtgShareBufferItem _vbItem;
	SShapeVertex* pVerts = (SShapeVertex*)RtGetRender()->GetBuffMgr()->Lock_Shared_vb(sizeof(SShapeVertex)*2, &_vbItem);
        

	SShapeVertex* pTempVerts = pVerts;
	*pTempVerts++ = SShapeVertex(startPoint.x,          startPoint.y,       m_ZOrder, color);
	*pTempVerts++ = SShapeVertex(endPoint.x,            endPoint.y,         m_ZOrder, color);

	RtGetRender()->GetBuffMgr()->Unlock(&_vbItem);

	m_pRtgDevice->SetVertexFormat(RTG_TRANSFORMED_VC);
	m_pRtgDevice->UseTexture(0, 0);
    m_pRtgDevice->DrawPrimitive(D3DPT_LINELIST, &_vbItem, sizeof(SShapeVertex), 1);

    /*
	RtGetRender()->SetStreamSource(0, RtGetRender()->GetBuffMgr()->GetShared_vb(), sizeof(SShapeVertex));
    RtGetRender()->DrawPrimitive(D3DPT_LINELIST, 0, 1);
    */
}

void CUiRenderDeviceImpl_Rt::DrawTriangle(const SPoint& p1, const SPoint& p2, const SPoint& p3, const RtwPixel& color)
{
	if (color.a == 0xFF)
		m_pRtgDevice->SetRenderState(RTGRS_ALPHA_BLEND, 0);
	else 
		m_pRtgDevice->SetRenderState(RTGRS_ALPHA_BLEND, 1);

    RtgShareBufferItem _vbItem;
	SShapeVertex* pVerts = (SShapeVertex*)RtGetRender()->GetBuffMgr()->Lock_Shared_vb(sizeof(SShapeVertex)*3, &_vbItem);
        

	SShapeVertex* pTempVerts = pVerts;
	*pTempVerts++ = SShapeVertex(p1.x,          p1.y,       m_ZOrder, color);
	*pTempVerts++ = SShapeVertex(p2.x,          p2.y,       m_ZOrder, color);
	*pTempVerts++ = SShapeVertex(p3.x,          p3.y,       m_ZOrder, color);

	RtGetRender()->GetBuffMgr()->Unlock(&_vbItem);

	m_pRtgDevice->SetVertexFormat(RTG_TRANSFORMED_VC);
	m_pRtgDevice->UseTexture(0, 0);
    m_pRtgDevice->DrawPrimitive(D3DPT_TRIANGLELIST, &_vbItem, sizeof(SShapeVertex), 1);
    /*
	RtGetRender()->SetStreamSource(0, RtGetRender()->GetBuffMgr()->GetShared_vb(), sizeof(SShapeVertex));
    RtGetRender()->DrawPrimitive(D3DPT_TRIANGLELIST, 0, 1);
    */
}

RtwRect CUiRenderDeviceImpl_Rt::getViewportRect()
{
	RtwRect rc;
	rc.left = 0;
	rc.top = 0;
	rc.right = m_pRtgDevice->m_iWndWidth - 1;
	rc.bottom = m_pRtgDevice->m_iWndHeight - 1;
	return rc;
}

void CUiRenderDeviceImpl_Rt::BeginRenderViewPort(RtgCameraD3DView *pCam, DWORD dwX,DWORD dwY,DWORD dwWeight,DWORD dwHeight,float fMin,float fMax, bool bRefreshCam /*= true*/)
{
	EndRender();
	((RtgDeviceD3D9*)m_pRtgDevice)->BeginRenderViewPort(dwX,dwY,dwWeight,dwHeight,fMin,fMax);
	m_pLastCam = m_pRtgDevice->m_pCamera;
	m_pRtgDevice->SetCamera(pCam);
	if (bRefreshCam)
		m_pRtgDevice->SetMatrixFromCamera();
}

void CUiRenderDeviceImpl_Rt::EndRenderViewPort(bool bRefreshCam/* = true*/)
{
	BeginRender();
	((RtgDeviceD3D9*)m_pRtgDevice)->EndRenderViewPort();
	m_pRtgDevice->SetCamera(m_pLastCam);
	if (bRefreshCam)
		m_pRtgDevice->SetMatrixFromCamera();
}

void CUiRenderDeviceImpl_Rt::SetMatrixFromCamera()
{
	m_pRtgDevice->SetMatrixFromCamera();
}

void CUiRenderDeviceImpl_Rt::RenderActorInstance(CRT_ActorInstance *pActor,RTGRenderMask eMask)
{
	if (pActor)
		pActor->Render(m_pRtgDevice,eMask);
}

CUiFontImpl_Rt::CUiFontImpl_Rt(const std::string& FontName, int FontSize )//add by fox
:RtwFont(FontName,FontSize), m_CurrBufferIndex(0), m_MaxCharBufferCount(0), 
m_BitmapBuffer(NULL), m_hDC(0), m_hBitmap(NULL), m_hFont(NULL), m_pRtgDevice(NULL){}

CUiFontImpl_Rt::~CUiFontImpl_Rt(){}

bool CUiFontImpl_Rt::Create(RtgDevice* pRtgDevice)
{
	Reset();

	UI_CHECK_B(pRtgDevice);
	m_pRtgDevice = pRtgDevice;

	m_FontWidth = (m_FontSize>>1);

	if (m_FontSize <= 0)
		return false;

	m_MaxCharBufferCount = (TEXTURE_WIDTH / m_FontSize) * (TEXTURE_HEIGHT / m_FontSize) * TEXTURE_COUNT;

	// Create bitmap
	UI_CHECK_B(m_hBitmap == NULL); 
	UI_CHECK_B(m_hDC == NULL);

	m_hDC = CreateCompatibleDC(NULL);

	BITMAPINFO BitmapInfo;
	ZeroMemory(&BitmapInfo.bmiHeader,sizeof(BITMAPINFOHEADER));
	BitmapInfo.bmiHeader.biSize		= sizeof(BITMAPINFOHEADER);
	BitmapInfo.bmiHeader.biWidth		= m_FontSize;
	BitmapInfo.bmiHeader.biHeight		= m_FontSize;
	BitmapInfo.bmiHeader.biPlanes		= 1;
	BitmapInfo.bmiHeader.biBitCount		= 32;
	BitmapInfo.bmiHeader.biCompression	= BI_RGB;

	m_hBitmap = CreateDIBSection(m_hDC, &BitmapInfo, DIB_RGB_COLORS,(void **) &m_BitmapBuffer, NULL, 0 );

	if (m_hBitmap == NULL)
	{
		RtCoreLog().Error("Create font bitmap fail!\n");
		return false;
	}

	SelectObject(m_hDC, m_hBitmap);

	//接着设置背景色和文字色：
	SetTextColor(m_hDC, RGB(255,255,255));
	SetBkColor(m_hDC, 0);

	//设置文字为上对齐：
	SetTextAlign(m_hDC, TA_TOP);

	// Create font
	LOGFONT LogFont;
	ZeroMemory( &LogFont, sizeof(LogFont) );
	LogFont.lfHeight			= -m_FontSize;
	LogFont.lfWidth				= 0;
	LogFont.lfEscapement		= 0;
	LogFont.lfOrientation		= 0;

	if( m_bIsBolder )
	{
		LogFont.lfWeight = FW_SEMIBOLD;  
	}else
	{
		LogFont.lfWeight = FW_NORMAL;
	}
	if( m_bIsItalic )
	{
		LogFont.lfItalic = TRUE;
	}else
	{
		LogFont.lfItalic = FALSE;
	}   

// 	LogFont.lfWeight			= FW_NORMAL;//字体重量//正常
// 	LogFont.lfItalic			= FALSE;
	LogFont.lfUnderline			= FALSE;
	LogFont.lfStrikeOut			= FALSE;
	LogFont.lfCharSet			= DEFAULT_CHARSET;
	LogFont.lfOutPrecision		= OUT_DEFAULT_PRECIS; 
	LogFont.lfClipPrecision		= CLIP_DEFAULT_PRECIS; 
	LogFont.lfQuality			= ANTIALIASED_QUALITY;
	LogFont.lfPitchAndFamily	= DEFAULT_PITCH;
	lstrcpyn( LogFont.lfFaceName, m_FontName.c_str(), 32);

	m_hFont = CreateFontIndirect(&LogFont);

	if (m_hFont == NULL)
		return false;

	SelectObject(m_hDC, m_hFont);

	// 初始化Texture
	for (int i=0; i<TEXTURE_COUNT; ++i)
	{
		m_textures[i].dirty = false;
		m_textures[i].buffer = RT_NEW BYTE[TEXTURE_WIDTH*TEXTURE_HEIGHT*4];
		memset( m_textures[i].buffer , 0 , TEXTURE_WIDTH*TEXTURE_HEIGHT*4 );
		m_textures[i].pending.clear();

		m_textures[i].shader = RT_NEW RtgShader;
		m_textures[i].shader->iNumStage = 1;
		m_textures[i].shader->bTwoSide = true;
#ifdef _DEBUG
		//m_pRtgDevice->m_Ability.bDynamicTextures = false;
#endif
		m_textures[i].shader->Shaders[0].SetTexture(
            RtGetRender()->GetTextMgr()->CreateDynamicTexture(
                TEXTURE_WIDTH, 
                TEXTURE_HEIGHT, 
                D3DFMT_A8R8G8B8
                ));
		m_textures[i].shader->Shaders[0].eColorOp = RTGTOP_SELECTARG1;
		m_textures[i].shader->Shaders[0].eColorArg1 = RTGTA_DIFFUSE;
		m_textures[i].shader->Shaders[0].eColorArg2 = RTGTA_TEXTURE;
		m_textures[i].shader->Shaders[0].eAlphaOp   = RTGTOP_SELECTARG1;
		m_textures[i].shader->Shaders[0].eAlphaArg1 = RTGTA_TEXTURE;
		m_textures[i].shader->Shaders[0].eAlphaArg2 = RTGTA_DIFFUSE;
		m_textures[i].shader->bEnableBlendFunc = true;
		m_textures[i].shader->eBlendSrcFunc    = RTGBLEND_SRCALPHA;
		m_textures[i].shader->eBlendDstFunc    = RTGBLEND_INVSRCALPHA;
		m_textures[i].shader->bColorKey = true;
	}

	return true;
}

void CUiFontImpl_Rt::Close()
{
	for (int i=0; i<TEXTURE_COUNT; ++i)
	{
		delete[] m_textures[i].buffer;
		DEL_ONE(m_textures[i].shader);
	}
}

void CUiFontImpl_Rt::DrawString3D(const std::string& str, const SRect3D& rect, const RtwPixel& textColor, 
								  bool bWordWrap, SUiTextAlignment align, bool bUnderLine, const RtwRect* pClipRect, int RenderSize)
{

	if (RenderSize == 0)
		RenderSize = m_FontSize;

	// 根据对齐方式和自动换行来确定位置
	int x, y;
	int StringWidth = GetStringWidth(str, (int)str.size(), RenderSize);
	int StringHeight = RenderSize;//getFontSize();

	RtwRect rcClip;
	if (pClipRect)
	{
		rcClip = *pClipRect;
		if (rcClip.left < 0)
			rcClip.left = 0;
		if (rcClip.top < 0)
			rcClip.top = 0;
	}
	else 
	{
		rcClip = g_workspace.getViewportRect();
	}

	switch (align.align)
	{
	case alignNear:
		{
			x = rect.s.left;
		}
		break;
	case alignCenter:
		{
			x = rect.s.getCenter().x - (StringWidth>>1);
		}
		break;
	case alignFar:
		{
			x = rect.s.right - StringWidth + 1;
		}
		break;
	default:
		break;
	}

	switch (align.lineAlign)
	{
	case alignNear:
		{
			y = rect.s.top;     
		}
		break;
	case alignCenter:
		{
			y = rect.s.getCenter().y - (RenderSize>>1);
		}
		break;
	case alignFar:
		{                    
			y = rect.s.bottom - RenderSize + 1;
		}
		break;
	default:
		break;
	}

	RtwRect rcRender = rect.s;
	rcRender.ClipBy(rcClip);
	DrawStringToBuffer3D(str, align, rcRender, x, y, rect.z, textColor, bWordWrap, RenderSize);

	if (bUnderLine)
	{
		SPoint StartPoint(x, y + StringHeight);
		SPoint EndPoint(x + StringWidth, y + StringHeight);
		if (StartPoint.y >= rcRender.top && StartPoint.y <= rcRender.bottom +1)	
		{
			if ( !( (StartPoint.x > rcRender.right && EndPoint.x > rcRender.right) 
				|| (StartPoint.x < rcRender.left && EndPoint.x < rcRender.left) ) )
			{
				if (StartPoint.x < rcRender.left)
					StartPoint.x = rcRender.left;
				if (EndPoint.x > rcRender.right)
					EndPoint.x = rcRender.right;
				g_workspace.getRenderDevice()->DrawLine(StartPoint, EndPoint, textColor);
			}
		}
	}
}
void CUiFontImpl_Rt::DrawString(const std::string& str, const RtwRect& rect, const RtwPixel& textColor,
								bool bWordWrap, SUiTextAlignment align, bool bUnderLine, 
								const RtwRect* pClipRect, int RenderSize)
{
	if (RenderSize == 0)
		RenderSize = m_FontSize;

	// 根据对齐方式和自动换行来确定位置
	int x, y;

	int StringWidth = GetStringWidth(str, (int)str.size(), RenderSize);
	int StringHeight = RenderSize;//getFontSize();

	RtwRect rcClip;
	if (pClipRect)
	{
		rcClip = *pClipRect;
		if (rcClip.left < 0)
			rcClip.left = 0;
		if (rcClip.top < 0)
			rcClip.top = 0;
	}
	else 
	{
		rcClip = g_workspace.getViewportRect();
	}

	switch (align.align)
	{
	case alignNear:
		{
			x = rect.left;
		}
		break;
	case alignCenter:
		{
			x = rect.getCenter().x - (StringWidth>>1);
		}
		break;
	case alignFar:
		{
			x = rect.right - StringWidth + 1;
		}
		break;
	default:
		break;
	}

	switch (align.lineAlign)
	{
	case alignNear:
		{
			y = rect.top;     
		}
		break;
	case alignCenter:
		{
			y = rect.getCenter().y - (RenderSize>>1);
		}
		break;
	case alignFar:
		{                    
			y = rect.bottom - RenderSize + 1;
		}
		break;
	default:
		break;
	}

	RtwRect rcRender = rect;
	rcRender.ClipBy(rcClip);
	DrawStringToBuffer(str, align, rcRender, x, y, textColor, bWordWrap, RenderSize);

	if (bUnderLine)
	{
		SPoint StartPoint(x, y + StringHeight);
		SPoint EndPoint(x + StringWidth, y + StringHeight);
		if (StartPoint.y >= rcRender.top && StartPoint.y <= rcRender.bottom +1)	// [chib] [2006-11-29] [modify the bug not show underline font]
		{
			if ( !( (StartPoint.x > rcRender.right && EndPoint.x > rcRender.right) || (StartPoint.x < rcRender.left && EndPoint.x < rcRender.left) ) )
			{
				if (StartPoint.x < rcRender.left)
					StartPoint.x = rcRender.left;
				if (EndPoint.x > rcRender.right)
					EndPoint.x = rcRender.right;
				g_workspace.getRenderDevice()->DrawLine(StartPoint, EndPoint, textColor);
			}
		}
	}
}

int CUiFontImpl_Rt::GetStringWidth(const std::string& str, int length/* = -1*/, const int RenderSize/* = 0*/)
{
	Real rate = RenderSize == 0 ? 1 : RenderSize / m_FontSize;
	if (length >= 0)
	{
		return m_FontWidth * length * rate;
	}
	else
	{
		return m_FontWidth * (int)str.length() * rate;
	}
}

bool CUiFontImpl_Rt::PickCharacter(const SPoint& point, int& outIndex, bool& outDoubleByte, bool& outNear, const std::string& str, const RtwRect& rect, int RenderSize/* = 0*/)
{
	if (!rect.IsContain(point))
		return false;

	int FontHeight = m_FontSize;
	int FontWidth = m_FontWidth;
	if (RenderSize)
	{
		FontHeight = FontWidth = RenderSize;
// 		FontHeight *= RenderSize / m_FontSize;
// 		FontWidth *= RenderSize / m_FontSize;
	}

	outIndex = 0;
	outDoubleByte = false;
	outNear = true;

	const char* p = str.c_str();
	int CurrPosX = rect.left;
	int CharWidth = 0;
	bool bDoubleByte = false;
	while (p)
	{
		if (*p & 0x80)
			bDoubleByte = true;
		else 
			bDoubleByte = false;
		bool  bIsEnterSymble = false;
		if( *p == ENTER_SYMBOL )
		{
			bIsEnterSymble = true;
		}
		CurrPosX += bDoubleByte ? FontWidth * 2 : FontWidth;
		if (point.x <= CurrPosX)
		{
			int delta = CurrPosX - point.x;
			if (bDoubleByte)
			{
				if (delta >= FontWidth)
					outNear = true;
				else 
					outNear = false;
			}
			else 
			{
				if (delta >= (FontWidth>>1))
					outNear = true;
				else 
					outNear = false;
			}
			outDoubleByte = bDoubleByte;
			return true;
		}

		outIndex += bDoubleByte ? 2 : 1;
		p += bDoubleByte ? 2 : 1;
		if( bIsEnterSymble )
		{
			return true;
		}
	}

	return false;
}

bool CUiFontImpl_Rt::PickCharacter(int width, int& outIndex, bool& outDoubleByte, bool& outNear, const std::string& str, int RenderSize/* = 0*/)
{
	if (width > GetStringWidth(str))
		return false;

	int FontHeight = m_FontSize;
	int FontWidth = m_FontWidth;
	if (RenderSize)
	{
		FontHeight *= RenderSize / m_FontSize;
		FontWidth *= RenderSize / m_FontSize;
	}

	outIndex = 0;
	outDoubleByte = false;
	outNear = true;

	const char* p = str.c_str();
	int CurrPosX = 0;
	int CharWidth = 0;
	bool bDoubleByte = false;
	while (p)
	{
		if (*p & 0x80)
			bDoubleByte = true;
		else 
			bDoubleByte = false;
		bool  bIsEnterSymble = false;
		if( *p == ENTER_SYMBOL )
		{
			bIsEnterSymble = true;
		}
		CurrPosX += bDoubleByte ? FontWidth*2 : FontWidth;

		if (width <= CurrPosX)
		{
			int delta = CurrPosX - width;
			if (bDoubleByte)
			{
				if (delta >= FontWidth)
					outNear = true;
				else 
					outNear = false;
			}
			else 
			{
				if (delta >= (FontWidth>>1))
					outNear = true;
				else 
					outNear = false;
			}
			outDoubleByte = bDoubleByte;
			return true;
		}

		outIndex += bDoubleByte ? 2 : 1;
		p += bDoubleByte ? 2 : 1;

		if( bIsEnterSymble )
		{
			return true;
		}
	}

	return false;

}

void CUiFontImpl_Rt::Reset()
{
	_ResetTextureBuffer();
}

void CUiFontImpl_Rt::_ResetTextureBuffer()
{
	memset(m_indices, 0x00, sizeof(CharIndex) * 256 * 256);
	m_CurrBufferIndex = -1;
}

inline  void  AddColor( unsigned char& nDest , unsigned char nScr0 , unsigned char nScr1  )
{
	int nDestInt = nScr0 + nScr1;
	if( nDestInt > 0xFF )
	{
		nDest = 0xFF;
	}else
	{
		nDest = nScr0 + nScr1;
	}
}

void CUiFontImpl_Rt::FillTexture(WORD character)
{
	if (m_indices[character].bSet)
		return;

	// 判断贴图缓冲区满的情况
	m_CurrBufferIndex ++;
	if (m_CurrBufferIndex >= m_MaxCharBufferCount)
	{
		Flush();
		_ResetTextureBuffer();
	}

	// 分配位置
	int CharPerLine = TEXTURE_WIDTH / m_FontSize;
	int CharPerCol = TEXTURE_HEIGHT / m_FontSize;
	int CharPerTexture = CharPerLine * CharPerCol;

	int texIdx = m_CurrBufferIndex / CharPerTexture;
	int row = (m_CurrBufferIndex % CharPerTexture) / CharPerLine;
	int col = (m_CurrBufferIndex % CharPerTexture) % CharPerCol;
	m_indices[character].bSet = 1;
	m_indices[character].texIdx = texIdx;
	m_indices[character].row = row;
	m_indices[character].col = col;

	// 把字体贴到自己的贴图缓冲区
	RECT rc = { 0, 0, m_FontSize, m_FontSize };
	FillRect(m_hDC, &rc, (HBRUSH)GetStockObject(BLACK_BRUSH));
	LPCSTR  kTextContent = (LPCSTR)&character    ;
	int     nTextWidth   = character&0x80 ? 2 : 1;

	SetTextColor(m_hDC, RGB(255,255,255));
	SetBkColor(m_hDC, 0);

    TextOut(m_hDC, 0, 0, kTextContent ,nTextWidth);

	int dstStride = TEXTURE_WIDTH * 4;
	int dstAdd = dstStride - m_FontSize * 4;

	unsigned char* dst = m_textures[texIdx].buffer + row * m_FontSize * dstStride + col * m_FontSize * 4 + 3;
	unsigned char* dststop = dst + dstStride * m_FontSize;
	unsigned char* src = m_BitmapBuffer;
	unsigned char* dstTail = m_textures[texIdx].buffer + TEXTURE_WIDTH*TEXTURE_HEIGHT*4;
	unsigned char* dstHead = m_textures[texIdx].buffer ;

	while(dst < dststop)
	{
		for (int n = 0; n < m_FontSize; ++n)
		{
			if( false == m_b3DShadow )
			{
				dst[0] = *src;
				dst[1] = 0xFF;
				dst[2] = 0xFF;
				dst[3] = 0xFF;
			}else
			{				

				AddColor( dst[ 0 ] , dst[ 0 ] , *src );
				AddColor( dst[ 1 ] , dst[ 1 ] , 0xFF );
				AddColor( dst[ 2 ] , dst[ 2 ] , 0xFF );
				AddColor( dst[ 3 ] , dst[ 3 ] , 0xFF );
				unsigned char* Scr2   =  dst + 4;				
				if( Scr2 < dstTail )
				{
					AddColor( Scr2[ 0 ] , Scr2[ 0 ] , ((*src)>>2));
					AddColor( Scr2[ 1 ] , Scr2[ 1 ] , (0xFF>>2));
					AddColor( Scr2[ 2 ] , Scr2[ 2 ] , (0xFF>>2));
					AddColor( Scr2[ 3 ] , Scr2[ 3 ] , (0xFF>>2));
				}			
			}			
			dst += 4;
			src += 4;
		}
		dst += dstAdd;
	}

	m_textures[texIdx].dirty = true;
}

void CUiFontImpl_Rt::DrawCharacterToBuffer3D(WORD character, const RtwPixel& textColor, int xTarget, int yTarget, float zTarget, int width, int height, int xImage/* = 0*/, int yImage/* = 0*/, Real scale/* = 1.0*/)
{
	if (character == 0)
		return;

	//int CharWidth = (character & 0x80) ? m_FontSize : (m_FontSize>>1);

	const CharIndex& index = m_indices[character];
	FillTexture(character);

	CharQueuer queuer;
	queuer.x = xTarget;
	queuer.y = yTarget;
	queuer.z = zTarget;//g_workspace.getRenderDevice()->getZOrder();
	queuer.width = width; //CharWidth;
	queuer.height = height; //m_FontSize;
	queuer.color = textColor;
	queuer.tx1 = (Real)(index.col * m_FontSize + xImage) / TEXTURE_WIDTH;    // 左上角x
	queuer.ty2 = (Real)(index.row * m_FontSize + yImage) / TEXTURE_HEIGHT;   // 左上角y
	queuer.tx2 = queuer.tx1 + (Real)width / TEXTURE_WIDTH;      // 右下角x
	queuer.ty1 = queuer.ty2 + (Real)height / TEXTURE_HEIGHT;    // 右下角y
	queuer.scale = scale;

	m_textures[index.texIdx].pending.push_back(queuer);
}

void CUiFontImpl_Rt::DrawCharacterToBuffer(WORD character, const RtwPixel& textColor, int xTarget, int yTarget, int width, int height, int xImage/* = 0*/, int yImage/* = 0*/, Real scale/* = 1.0*/)
{
	if (character == 0)
		return;

	//int CharWidth = (character & 0x80) ? m_FontSize : (m_FontSize>>1);

	const CharIndex& index = m_indices[character];
	FillTexture(character);

	CharQueuer queuer;
	queuer.x = xTarget;
	queuer.y = yTarget;
	queuer.z = g_workspace.getRenderDevice()->getZOrder();
	queuer.width = width; //CharWidth;
	queuer.height = height; //m_FontSize;
	queuer.color = textColor;
	queuer.tx1 = (Real)(index.col * m_FontSize + xImage) / TEXTURE_WIDTH;     // 左上角x
	queuer.ty2 = (Real)(index.row * m_FontSize + yImage) / TEXTURE_HEIGHT;   // 左上角y
	queuer.tx2 = queuer.tx1 + (Real)width / TEXTURE_WIDTH;      // 右下角x
	queuer.ty1 = queuer.ty2 + (Real)height / TEXTURE_HEIGHT;    // 右下角y
	queuer.scale = scale;

	m_textures[index.texIdx].pending.push_back(queuer);
}

void CUiFontImpl_Rt::DrawStringToBuffer(const std::string& str, SUiTextAlignment align, const RtwRect& clipRect, int PosX, int PosY, const RtwPixel& textColor, bool bWordWrap/* = true*/, int RenderSize/* = 0*/)
{
	std::vector<WORD> arrChar;
	std::vector<short> arrPosX;
	std::vector<short> arrPosY;
	std::vector<short> arrImageX;
	std::vector<short> arrImageY;
	std::vector<short> arrWidth;
	std::vector<short> arrHeight;
	CalcStringPosition(str, align, clipRect, PosX, PosY, bWordWrap, arrChar, arrPosX, arrPosY, arrWidth, arrHeight, arrImageX, arrImageY, RenderSize);

	// 画到缓冲区
	for (int i=0; i<(int)arrChar.size(); ++i)
	{
		DrawCharacterToBuffer(arrChar[i], textColor, arrPosX[i], arrPosY[i], arrWidth[i], arrHeight[i], arrImageX[i], arrImageY[i], RenderSize/m_FontSize);
	}
}

void CUiFontImpl_Rt::DrawStringToBuffer3D(const std::string& str, SUiTextAlignment align, const RtwRect& clipRect, int PosX, int PosY,float PosZ, const RtwPixel& textColor, bool bWordWrap , int RenderSize )
{
	std::vector<WORD> arrChar;
	std::vector<short> arrPosX;
	std::vector<short> arrPosY;
	std::vector<float> arrPosZ;
	std::vector<short> arrImageX;
	std::vector<short> arrImageY;
	std::vector<short> arrWidth;
	std::vector<short> arrHeight;
	CalcStringPosition3D(str, align, clipRect, PosX, PosY, PosZ, bWordWrap, arrChar, arrPosX, arrPosY, arrPosZ,arrWidth, arrHeight, arrImageX, arrImageY, RenderSize);

	// 画到缓冲区
	for (int i=0; i<(int)arrChar.size(); ++i)
	{
		DrawCharacterToBuffer3D(arrChar[i], textColor, arrPosX[i], arrPosY[i],arrPosZ[i], arrWidth[i], arrHeight[i], arrImageX[i], arrImageY[i],(Real)RenderSize/m_FontSize);
	}

}

void CUiFontImpl_Rt::CalcCharacterPosition3D(const WORD character, SUiTextAlignment align, const RtwRect& clipRect, int PosX, int PosY, float PosZ, short& outPosX, short& outPosY, float& outPosZ, short& outWidth, short& outHeight, short& outImageX, short& outImageY, int RenderSize /* = 0 */)
{
	if (character == 0)
		return;

	int CharWidth = (character & 0x80) ? m_FontSize : (m_FontSize>>1);
	int CharHeight = m_FontSize;

	/*if (PosX < clipRect.left)
	{
	outImageX = clipRect.left - PosX;
	outPosX = clipRect.left;
	}
	else */
	{
		outImageX = 0;
		outPosX = PosX;
	}

	//if (PosX + CharWidth - 1 > clipRect.right)
	//{
	//	outWidth = clipRect.right - outPosX;
	//}
	//else 
	{
		outWidth = PosX + CharWidth - outPosX;
	}

	/*if (PosY < clipRect.top)
	{
	outImageY = clipRect.top - PosY;
	outPosY = clipRect.top;
	}
	else */
	{
		outImageY = 0;
		outPosY = PosY;
	}

	/*if (PosY + CharHeight - 1 > clipRect.bottom)
	{
	outHeight = clipRect.bottom - outPosY + 1;
	}
	else */
	{
		outHeight = PosY + CharHeight - outPosY;
	}

	// 字的截取上下颠倒，修正
	if (outWidth > 0 && outHeight > 0)
	{
		if (outHeight != CharHeight)
		{
			outImageY = m_FontSize - outImageY - outHeight;
			//if (align.lineAlign == alignNear)
			//    outImageY += CharHeight - outHeight;
			//else if (align.lineAlign == alignFar)
			//    outImageY -= CharHeight - outHeight;
		}
	}

	if (RenderSize)
	{
		outWidth = outWidth*RenderSize / m_FontSize;
		outHeight = outHeight*RenderSize / m_FontSize;
	}
	outPosZ = PosZ;

}

void CUiFontImpl_Rt::CalcCharacterPosition(const WORD character, SUiTextAlignment align, const RtwRect& clipRect, int PosX, int PosY, short& outPosX, short& outPosY, short& outWidth, short& outHeight, short& outImageX, short& outImageY, int RenderSize/* = 0*/)
{
	if (character == 0)
		return;

	int CharWidth = (character & 0x80) ? m_FontSize : (m_FontSize>>1);
	int CharHeight = m_FontSize;

	if (PosX < clipRect.left)
	{
		outImageX = clipRect.left - PosX;
		outPosX = clipRect.left;
	}
	else 
	{
		outImageX = 0;
		outPosX = PosX;
	}

	if (PosX + CharWidth - 1 > clipRect.right)
	{
		outWidth = clipRect.right - outPosX;
	}
	else 
	{
		outWidth = PosX + CharWidth - outPosX;
	}

	if (PosY < clipRect.top)
	{
		outImageY = clipRect.top - PosY;
		outPosY = clipRect.top;
	}
	else 
	{
		outImageY = 0;
		outPosY = PosY;
	}

	if (PosY + CharHeight - 1 > clipRect.bottom)
	{
		outHeight = clipRect.bottom - outPosY + 1;
	}
	else 
	{
		outHeight = PosY + CharHeight - outPosY;
	}

	// 字的截取上下颠倒，修正
	if (outWidth > 0 && outHeight > 0)
	{
		if (outHeight != CharHeight)
		{
			outImageY = m_FontSize - outImageY - outHeight;
			//if (align.lineAlign == alignNear)
			//    outImageY += CharHeight - outHeight;
			//else if (align.lineAlign == alignFar)
			//    outImageY -= CharHeight - outHeight;
		}
	}

	if (RenderSize)
	{
		outWidth *= RenderSize / m_FontSize;
		outHeight *= RenderSize / m_FontSize;
	}
}

void CUiFontImpl_Rt::CalcStringPosition(const std::string& str, SUiTextAlignment align, const RtwRect& clipRect, int PosX, int PosY, bool bWordWrap, std::vector<WORD>& outArrCharacters, std::vector<short>& outArrPosX, std::vector<short>& outArrPosY, std::vector<short>& outArrWidth, std::vector<short>& outArrHeight, std::vector<short>& outArrImageX, std::vector<short>& outArrImageY, int RenderSize/* = 0*/)
{
	outArrCharacters.clear();
	outArrPosX.clear();
	outArrPosY.clear();
	outArrWidth.clear();
	outArrHeight.clear();
	outArrImageX.clear();
	outArrImageY.clear();

	if (RenderSize == 0)
		RenderSize = m_FontSize;

	int lineHeight = RenderSize + 2;    // 行高

	int x = PosX;
	int y = PosY;
	//int width = rect.getWidth();
	//int height = rect.getHeight();

	bool bFull = false; // 区域内是否显示得下指定字符串
	WORD character = 0;
	const char* p = str.c_str();
	while (*p)
	{
		int chWidth;

		// 判断特殊字符
// 		if (*p == '\n')
// 		{
// 		    p += 1;
// 		    y += lineHeight;
// 		    x = clipRect.left;
// 		    continue;
// 		}

		// 判断是否为双字节字符
		if (*p & 0x80)
		{
			character = *(WORD*)p;
			//p += 2;
			chWidth = RenderSize;
		}
		else 
		{
			character = *(char*)p;
			//p += 1;
			chWidth = (RenderSize>>1);
		}

		short ImageX, ImageY;
		short _X,_Y;
		short Width, Height;
		CalcCharacterPosition(character, align, clipRect, x, y, _X, _Y, Width, Height, ImageX, ImageY, RenderSize);
		if (Width > 0 && Height > 0)
		{
			if (!bWordWrap || (bWordWrap && Width == chWidth) )
			{
				outArrCharacters.push_back(character);
				outArrPosX.push_back(_X);
				outArrPosY.push_back(_Y);
				outArrWidth.push_back(Width);
				outArrHeight.push_back(Height);
				outArrImageX.push_back(ImageX);
				outArrImageY.push_back(ImageY);
			}
		}

		if ( (Width < chWidth) && (x + chWidth >= clipRect.right) )
		{
			if (bWordWrap)
			{
				y += lineHeight;
				x = PosX;

				if (y > clipRect.bottom)
				{
					bFull = true;
					break;
				}
				else 
					continue;
			}
			else 
			{
				bFull = true;
				break;
			}
		}
		else if (Width == chWidth)
		{
		}
		x += chWidth;

		if (*p & 0x80)
		{
			p += 2;
		}
		else 
		{
			p += 1;
		}

		// 超出y界限的处理
		if (y >= clipRect.bottom)
		{
			bFull = true;
			break;
		}
	}


	//// 是否需要用"..."代替显示不下的字符串
	//if (!bWordWrap && bFull)
	//{
	//    int nCharCount = (int)outArrCharacters.size();
	//    int i,j;
	//    // 替换字符"..."
	//    for (i=nCharCount-1; i>0; i--)
	//    {
	//        if ( (rect.right-outArrPosX[i]) >= (2 * ((m_FontSize>>1))) )
	//        {
	//            break;
	//        }
	//    }
	//    for (j=i; j<nCharCount; ++j)
	//    {
	//        outArrCharacters[j] = '.';
	//        if (j!=i)
	//            outArrPosX[j] = outArrPosX[j-1] + (m_FontSize>>1);
	//    }
	//    //int _Tmp = _arrX[nCharCount-1] - rect.right - (m_FontSize>>1);
	//    x = outArrPosX[nCharCount-1] + (m_FontSize>>1);
	//    while (x + (m_FontSize>>1) <= rect.right)
	//    {
	//        outArrCharacters.push_back('.');
	//        outArrPosX.push_back((short)x);
	//        outArrPosY.push_back((short)y);
	//        x += (m_FontSize>>1);
	//    }
	//}
}

void CUiFontImpl_Rt::CalcStringPosition3D(const std::string& str, SUiTextAlignment align, const RtwRect& clipRect, int PosX, int PosY, float PosZ, bool bWordWrap, std::vector<WORD>& outArrCharacters, std::vector<short>& outArrPosX, std::vector<short>& outArrPosY,std::vector<float>& outArrPosZ, std::vector<short>& outArrWidth, std::vector<short>& outArrHeight, std::vector<short>& outArrImageX, std::vector<short>& outArrImageY, int RenderSize/* = 0*/)
{

	outArrCharacters.clear();
	outArrPosX.clear();
	outArrPosY.clear();
	outArrPosZ.clear();
	outArrWidth.clear();
	outArrHeight.clear();
	outArrImageX.clear();
	outArrImageY.clear();

	if (RenderSize == 0)
		RenderSize = m_FontSize;

	int lineHeight = RenderSize + 2;    // 行高

	int x = PosX;
	int y = PosY;
	float z = PosZ;
	//int width = rect.getWidth();
	//int height = rect.getHeight();

	bool bFull = false; // 区域内是否显示得下指定字符串
	WORD character = 0;
	const char* p = str.c_str();
	while (*p)
	{
		int chWidth;

		//// 判断特殊字符
		//if (*p == '\n')
		//{
		//    p += 1;
		//    y += lineHeight;
		//    x = rect.left;
		//    continue;
		//}

		// 判断是否为双字节字符
		if (*p & 0x80)
		{
			character = *(WORD*)p;
			//p += 2;
			chWidth = RenderSize;
		}
		else 
		{
			character = *(char*)p;
			//p += 1;
			chWidth = (RenderSize>>1);
		}

		short ImageX, ImageY;
		short _X,_Y;
		short Width, Height;
		CalcCharacterPosition(character, align, clipRect, x, y, _X, _Y, Width, Height, ImageX, ImageY, RenderSize);
		if (Width > 0 && Height > 0)
		{
			if (!bWordWrap || (bWordWrap && Width == chWidth) )
			{
				outArrCharacters.push_back(character);
				outArrPosX.push_back(_X);
				outArrPosY.push_back(_Y);
				outArrPosZ.push_back(z);
				outArrWidth.push_back(Width);
				outArrHeight.push_back(Height);
				outArrImageX.push_back(ImageX);
				outArrImageY.push_back(ImageY);
			}
		}

		if ( (Width < chWidth) && (x + chWidth >= clipRect.right) )
		{
			if (bWordWrap)
			{
				y += lineHeight;
				x = PosX;

				if (y > clipRect.bottom)
				{
					bFull = true;
					break;
				}
				else 
					continue;
			}
			else 
			{
				bFull = true;
				break;
			}
		}
		else if (Width == chWidth)
		{
		}
		x += chWidth;

		if (*p & 0x80)
		{
			p += 2;
		}
		else 
		{
			p += 1;
		}

		// 超出y界限的处理
		if (y >= clipRect.bottom)
		{
			bFull = true;
			break;
		}
	}
}

void CUiFontImpl_Rt::DrawBufferToDevice()
{
	m_pRtgDevice->SetVertexFormat(RTG_TRANSFORMED_VCT);	

	for (int i=0; i<TEXTURE_COUNT; ++i)
	{
		StringTexture* texture = &(m_textures[i]);
		if (texture->pending.empty())
			continue;

		int numVerts = (int)texture->pending.size() * 3 * 2;

		RtgShareBufferItem _vbItem;
		FontVertex* verts = (FontVertex*)RtGetRender()->GetBuffMgr()->Lock_Shared_vb(numVerts * sizeof(FontVertex), &_vbItem);  
           

		for (std::vector<CharQueuer>::iterator queuer = texture->pending.begin(); 
			queuer != texture->pending.end();
			++queuer)
		{
			float sx = queuer->x - 0.5f;
			float sy = queuer->y - 0.5f;
			float rsx = sx + queuer->width;
			float rsy = sy + queuer->height;
			float tx1 = queuer->tx1;
			float ty1 = queuer->ty1;
			float tx2 = queuer->tx2;
			float ty2 = queuer->ty2;
			if (queuer->scale != 1.0)
			{
				tx2 = tx1 + (tx2 - tx1) * queuer->scale;
				ty2 = ty1 - (ty1 - ty2) * queuer->scale;
			}

			*verts++ = FontVertex(sx,	rsy,	queuer->z, queuer->color, tx1, ty2);
			*verts++ = FontVertex(rsx,	rsy,	queuer->z, queuer->color, tx2, ty2);
			*verts++ = FontVertex(sx,	sy,		queuer->z, queuer->color, tx1, ty1);
			*verts++ = FontVertex(rsx,	sy,		queuer->z, queuer->color, tx2, ty1);
			*verts++ = FontVertex(sx,	sy,		queuer->z, queuer->color, tx1, ty1);
			*verts++ = FontVertex(rsx,	rsy,	queuer->z, queuer->color, tx2, ty2);
		}

		RtGetRender()->GetBuffMgr()->Unlock(&_vbItem);

		texture->pending.resize(0);

		if (texture->dirty)
		{
            RtGetRender()->GetTextMgr()->UpdateTexture(texture->shader->Shaders[0].GetTexture(), texture->buffer);
			//m_pRtgDevice->m_Texture.UpdateTextureDynamic(texture->shader->Shaders[0].GetTexture(), texture->buffer);

			texture->dirty = false;
		}

		m_pRtgDevice->SetShader(texture->shader);	

        m_pRtgDevice->DrawPrimitive(D3DPT_TRIANGLELIST, &_vbItem, sizeof(FontVertex), numVerts / 3);
        /*
		RtGetRender()->SetStreamSource(0, RtGetRender()->GetBuffMgr()->GetShared_vb(), sizeof(FontVertex));
        RtGetRender()->DrawPrimitive(D3DPT_TRIANGLELIST, 0, numVerts / 3);
        */

	}
}

void CUiFontImpl_Rt::DrawBufferToDevice3D()
{
	m_pRtgDevice->SetVertexFormat(RTG_TRANSFORMED_VCT);	

	for (int i=0; i<TEXTURE_COUNT; ++i)
	{
		StringTexture* texture = &(m_textures[i]);
		if (texture->pending.empty())
			continue;

		int numVerts = (int)texture->pending.size() * 3 * 2;

		RtgShareBufferItem _vbItem;
		FontVertex* verts = (FontVertex*)RtGetRender()->GetBuffMgr()->Lock_Shared_vb(numVerts * sizeof(FontVertex), &_vbItem);
            	

		for (std::vector<CharQueuer>::iterator queuer = texture->pending.begin(); 
			queuer != texture->pending.end();
			++queuer)
		{
			float sx = queuer->x - 0.5f;
			float sy = queuer->y - 0.5f;
			float rsx = sx + queuer->width;
			float rsy = sy + queuer->height;
			float tx1 = queuer->tx1;
			float ty1 = queuer->ty1;
			float tx2 = queuer->tx2;
			float ty2 = queuer->ty2;

			if (queuer->scale != 1.0)
			{
				tx2 = tx1 + (tx2 - tx1) * queuer->scale;
				ty2 = ty1 - (ty1 - ty2) * queuer->scale;
			}

			*verts++ = FontVertex(sx,	rsy,	queuer->z, queuer->color, tx1, ty2);
			*verts++ = FontVertex(rsx,	rsy,	0.94,      queuer->color, tx2, ty2);
			*verts++ = FontVertex(sx,	sy,		0.94,      queuer->color, tx1, ty1);
			*verts++ = FontVertex(rsx,	sy,		0.94,      queuer->color, tx2, ty1);
			*verts++ = FontVertex(sx,	sy,		0.94,      queuer->color, tx1, ty1);
			*verts++ = FontVertex(rsx,	rsy,	0.94,      queuer->color, tx2, ty2);
		}

		RtGetRender()->GetBuffMgr()->Unlock(&_vbItem);

		texture->pending.resize(0);

		if (texture->dirty)
		{
            RtGetRender()->GetTextMgr()->UpdateTexture(texture->shader->Shaders[0].GetTexture(), texture->buffer);
			//m_pRtgDevice->m_Texture.UpdateTextureDynamic(texture->shader->Shaders[0].GetTexture(), texture->buffer);

			texture->dirty = false;
		}

		m_pRtgDevice->SetShader(texture->shader);	
        m_pRtgDevice->DrawPrimitive(D3DPT_TRIANGLELIST, &_vbItem, sizeof(FontVertex), numVerts / 3);

        /*
		RtGetRender()->SetStreamSource(0, RtGetRender()->GetBuffMgr()->GetShared_vb(), sizeof(FontVertex));
        RtGetRender()->DrawPrimitive(D3DPT_TRIANGLELIST, 0, numVerts / 3);
        */
	}
}

void CUiFontImpl_Rt::OnInvalidateDevice()
{
	Reset();
}

void CUiFontImpl_Rt::OnRestoreDevice()
{
	for (int i=0; i<TEXTURE_COUNT; ++i)
	{
		StringTexture* texture = &(m_textures[i]);
		RtGetRender()->GetTextMgr()->UpdateTexture(texture->shader->Shaders[0].GetTexture(), texture->buffer);
        //m_pRtgDevice->m_Texture.UpdateTextureDynamic(texture->shader->Shaders[0].GetTexture(), texture->buffer);
	}
}


CUiFontManagerImpl_Rt::CUiFontManagerImpl_Rt(RtgDevice* pRtgDevice)
{
	m_pRtgDevice = pRtgDevice;
}

CUiFontManagerImpl_Rt::~CUiFontManagerImpl_Rt()
{
}

RtwFont* CUiFontManagerImpl_Rt::OnCreateFont(const std::string& FontName,const int FontSize ,int nFontStyle )
{
	CUiFontImpl_Rt* pFont = RT_NEW CUiFontImpl_Rt(FontName,FontSize);
	pFont->SetFontStyle( nFontStyle );
	if (!pFont->Create(m_pRtgDevice))
	{
		DEL_ONE(pFont);
		return NULL;
	}

	return pFont;
}

void CUiFontManagerImpl_Rt::Close()
{
	foreach(std::vector<RtwFont*>, iter, m_Fonts)
	{
		RtwFont* pFont = *iter;
		pFont->Close();
		DEL_ONE(pFont);
	}
	m_Fonts.clear();
}

void CUiFontManagerImpl_Rt::Render()//渲染字体
{
	foreach(std::vector<RtwFont*>, iter, m_Fonts)
	{
		RtwFont* pFont = *iter;
		pFont->Flush();
	}
}
