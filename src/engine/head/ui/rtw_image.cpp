#include "ui/rtw_ui.h"
#include "gif/gif89a.h"
#include <io.h>
RT_IMPLEMENT_DYNAMIC(RtwImage,			RtwRefObject,	0,	"ui")
RT_IMPLEMENT_DYNAMIC(RtwComplexImage,	RtwImage,		0,	"ui")
RT_IMPLEMENT_DYNAMIC(CGifSequence,		RtwComplexImage,0,	"ui")

RtwImage::RtwImage():m_bBlend(false), m_Transparency(1.0), m_bHighLight(false), m_FillMode(ImageFillMode_Stretch),border(NULL)
{
	//顺便将后备透明度设置一下
	SetTransparency(1.0);
}

void RtwImage::SetTransparency(float transparen)
{
	m_Transparency = transparen;
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
		m_Color.a = 255 * transparen;
	}
	m_BackTransparency = m_Color.a;
}

void RtwImage::SetCtrlTransparent(float transparent)
{
	if (transparent > 1.0) transparent = 1.0;
	if (transparent < 0.0) transparent = 0.0;
	m_Color.a = m_BackTransparency * transparent;
}

CUiImageRegister* IUiImageFactory::mpkRegister = NULL;

RtwImage* IUiImageFactory::createImage(const std::string& filename)
{
	if (filename.empty())	//空文件
		return NULL;

	//if (_access(filename.c_str(), 0) == -1) //找不到文件
	//{
	//	return NULL;
	//}
	RtwImage* pImage = createGifSequence();

	bool bSuccess = false;

 	if (!pImage->LoadFromFile(filename))
	{
		stlforeach (std::list<std::string>, iterPath, g_workspace.getDefaultTexturePaths())
		{
			const std::string& strPath = *iterPath;
			std::string strTmp = strPath + filename;
			if (pImage->LoadFromFile(strTmp))
			{
				bSuccess = true;
				break;
			}
		}

		if (!bSuccess)
		{
			DROP_TRUI_IMAGE(pImage);
			return NULL;
		}
	}

	return pImage;
}

RtwImage* IUiImageFactory::createImage(const void* data, int length)
{
	if (length == 0)
		return NULL;

	RtwImage* pImage = createImage();

	bool bSuccess = false;

	if (pImage->LoadFromMemory(data,length))
	{
		bSuccess = true;		
	}

	if (!bSuccess)
	{
		DROP_TRUI_IMAGE(pImage);
		return NULL;
	}

	return pImage;
}

RtwImage* IUiImageFactory::createImage(RtArchive *pArc,int length,bool bflip/* = false*/)
{
	if (length == 0)
		return NULL;

	RtwImage* pImage = createImage();

	bool bSuccess = false;

	if (pImage->LoadFromRtArchive(pArc,length,bflip))
	{
		bSuccess = true;		
	}

	if (!bSuccess)
	{
		DROP_TRUI_IMAGE(pImage);
		return NULL;
	}

	return pImage;
}

RtwImage* IUiImageFactory::createImage()//得到一个图像//引用记数加一//并登记
{
	RtwImage* pImage = RealCreateImage();	//真的得到一个RtwImage
	pImage->grab();							//引用记数加一
	IUiImageFactory::UiImageFactory()->registerImage(pImage);		//所有的图片都登记在这里
	return pImage;
}

RtwComplexImage* IUiImageFactory::createImageSequence()
{
	RtwComplexImage* pImageSequence = RT_NEW RtwComplexImage();
	pImageSequence->grab();
	IUiImageFactory::UiImageFactory()->registerImage(pImageSequence);

	return pImageSequence;
}

CGifSequence* IUiImageFactory::createGifSequence()
{
	CGifSequence* pImageSequence = RT_NEW CGifSequence();
	pImageSequence->grab();
	IUiImageFactory::UiImageFactory()->registerImage(pImageSequence);

	return pImageSequence;
}

void IUiImageFactory::OnDeleteImage(RtwImage* pImage)
{
	EXT_SPACE::unordered_map<RtwImage*, DWORD>::iterator iterFinder = m_LiveImages.find(pImage);
	UI_CHECK(iterFinder!=m_LiveImages.end());
	m_LiveImages.erase(iterFinder);
}

RtwComplexImage::RtwComplexImage()
{	
	borderSize = SMargin(0, 0, 0, 0);
	border = NULL;
	Reset();
}

RtwComplexImage::~RtwComplexImage(){}

void RtwComplexImage::OnBeforeDestroy()
{
	Reset();
	
	RtwImage::OnBeforeDestroy();
}

void RtwComplexImage::Reset()
{
	m_Play_CurrentImage = 0;
	m_nDirtyTime = 0;
	m_nCount = 0;
	m_bPause = false;

	m_CountEvent.clear();

	DropImageList();
 	DROP_TRUI_IMAGE(border);

	m_Images.clear();
}

void RtwComplexImage::Tick(DWORD dwMilliseconds)
{
	if (m_Images.empty())
	{
		return;
	}

	if (m_Images.size() == 1)
	{
		m_Play_CurrentImage = 0;
		return;
	}

	if (m_bPause)
	{
		return;
	}

	if (m_nDirtyTime < 0)
	{
		m_nDirtyTime = 0;
	}

	m_nDirtyTime += dwMilliseconds;

	if (m_Play_CurrentImage < 0)
	{
		m_Play_CurrentImage = 0;
	}

	if (m_nDirtyTime >= m_Images[m_Play_CurrentImage].m_RenderTime)
	{
		m_nDirtyTime =0;
		m_Play_CurrentImage++;
	}

	if (m_Play_CurrentImage >= (int)m_Images.size())
	{
		//图片播放次数
		if (m_CountEvent.size() > 0)
		{
			m_nCount++;

			m_iterCountEvent = m_CountEvent.find(m_nCount);
			if (m_iterCountEvent != m_CountEvent.end())
			{
				m_CountEvent[m_nCount](NULL, etLast);
			}
		}

		m_Play_CurrentImage = 0;
	}
}

bool RtwComplexImage::AddImage(RtwImage* pImage, int RenderTime/* = 0*/)
{
	UI_ENSURE_B(pImage);

	SSequenceImage _Image;
	_Image.SetImage(pImage, RenderTime);

	m_Images.push_back(_Image);
	return true;
}

RtwImage* RtwComplexImage::getCurrentImage()
{
	return getImage(m_Play_CurrentImage);
}

RtwImage* RtwComplexImage::getImage(int index)
{
	UI_ENSURE_P(index >= 0 && index < (int)m_Images.size());

	return m_Images[index].getImage();
}

void RtwComplexImage::SetSourceRect(RtwRect& vrRect) 
{
	for (int i=0; i<(int)getImageCount(); ++i)
		getImage(i)->SetSourceRect(vrRect);
	m_Source = RtwRect(vrRect.left, vrRect.top, vrRect.right, vrRect.bottom);
	m_Source = vrRect;
}


void RtwComplexImage::SetTransparency(Real trans)
{
	for (int i=0; i<(int)getImageCount(); ++i)
	{
		getImage(i)->SetTransparency(trans);
	}
	m_Transparency = trans;
}
void RtwComplexImage::SetCtrlTransparent(Real trans)
{
	for (int i=0; i<(int)getImageCount(); ++i)
	{
		getImage(i)->SetCtrlTransparent(trans);
	}

	m_BackTransparency = trans;
}

void RtwComplexImage::SetBlend(bool bBlend)
{
	for (int i=0; i<(int)getImageCount(); ++i) 
	{
		getImage(i)->SetBlend(bBlend);
	}
	m_bBlend = bBlend;
}

bool CGifSequence::LoadFromFile(const std::string& filename) 
{
	if (filename.rfind(".gif") == string::npos)
	{
		RtwImage *pImage = g_workspace.getImageFactory()->createImage();
		if(pImage->LoadFromFile(filename))
		{
			DropImageList();
			AddImage(pImage);
		}
		else
			return false;
	}
	else
	{
		if(!LoadFromGif(filename))
			return false;
	}
	m_FileName = filename;
	return true;
}

CGifSequence::CGifSequence()
{	
	Reset();
}

bool CGifSequence::LoadFromGif(const std::string &filename)
{
	CGif89a _Gif(filename.c_str(), TRUE);

	if (!_Gif)
		return false;

	LPCGLOBAL_INFO info;
	LPCFRAME frame;
	info = _Gif.getGlobalInfo();

	UINT uPitch = (UINT)ceil(32 * info->scrWidth  / 32.f) * 4;
	BYTE* _argb = RT_NEW BYTE[uPitch * info->scrHeight];
	UINT dataSize = uPitch*info->scrHeight;

	for(UINT i = 0; i < info->frames; ++i)
	{	
		frame = _Gif.getNextFrame();

		ColorRGB* _tab = (ColorRGB*)info->gColorTable;

		if (frame->lFlag)
			_tab = (ColorRGB*)frame->pColorTable;

		for (int y = 0; y < frame->imageHeight; ++y)
		{
			ColorRGBA* dst = (ColorRGBA*)(_argb + uPitch * (y + frame->imageTPos));
			BYTE* src = frame->dataBuf + frame->imageWidth * y;

			for (int x = 0; x < frame->imageWidth; ++x)
			{   
				if (frame->ctrlExt.trsFlag && src[x] == frame->ctrlExt.trsColorIndex)
				{
 					if (i == 0)
						dst[x + frame->imageLPos].a = 0;// = _tab[info->BKColorIdx];
				}
				else
					dst[x + frame->imageLPos] = _tab[src[x]];
			}
		}

// 		_PiexlAccess<DWORD> pd = _PiexlAccess<DWORD>(_argb, info->scrWidth, info->scrHeight, uPitch);
// 		pd.cubeBlurAlpha(1);

		RtwImage *pImage = g_workspace.getImageFactory()->createImage();
		if (pImage->LoadFromMemory(info->scrWidth, info->scrHeight, dataSize, _argb))
		{
			pImage->SetSourceRect(RtwRect(0, 0, info->scrWidth, info->scrHeight));
			pImage->SetBlend(true);
			pImage->m_FileName = filename;

			AddImage(pImage, frame->ctrlExt.delayTime*10);
		}
	}

	DEL_ARRAY(_argb);
	return true;
}

CGifSequence::~CGifSequence(){}

void CGifSequence::OnBeforeDestroy()
{
	Reset();
	RtwComplexImage::OnBeforeDestroy();
}

CUiImageRegister::~CUiImageRegister()
{
#ifdef _DEBUG
	// Check if widgets are all destroyed (但是对于static则无能为力)
	if (!m_LiveImages.empty())
	{
		//static char strOut2048[2048];
		S_MEMDEF(strOut2048, 2048)
		S_MEMPROTECTOR(strOut2048, 2048, bMP)
		int LiveCount = (int)m_LiveImages.size();
		rt2_sprintf(strOut2048, "[UI] 共有%d个RtwImage对象没有被销毁，以下列出%d个： \n", LiveCount, LiveCount>=5 ? 5 : LiveCount);
		int counter = 0;
		EXT_SPACE::unordered_map<RtwImage*, DWORD>::iterator iterLive = m_LiveImages.begin();
		for(; iterLive!=m_LiveImages.end(); ++iterLive)
		{
			if (++counter > 5)
				break;

			RtwImage* pkLiveImage = iterLive->first;
			rt2_strcat(strOut2048, pkLiveImage->m_FileName.c_str());
			rt2_strcat(strOut2048, ",");
		}
		rt2_strcat(strOut2048, "\n");
		CHECKEX(strOut2048);
	}
#endif
};

void CUiImageRegister::registerImage(RtwImage* vpImage)
{
	if (vpImage)
	{
		EXT_SPACE::unordered_map<RtwImage*, DWORD>::iterator iterFinder = m_LiveImages.find(vpImage);
		UI_CHECK(iterFinder==m_LiveImages.end());
		m_LiveImages[vpImage] = 0;
	}
}

void CUiImageRegister::unregisterImage(RtwImage* vpImage)
{
	EXT_SPACE::unordered_map<RtwImage*, DWORD>::iterator iterFinder = m_LiveImages.find(vpImage);
	UI_CHECK(iterFinder!=m_LiveImages.end());
	m_LiveImages.erase(iterFinder);
}
