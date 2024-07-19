#ifndef IMAGE_CONTAINER_IMP_H
#define IMAGE_CONTAINER_IMP_H

#include "ui/rtw_ui.h"
using ui::RtwImage;

//图形包装
class CRtAndGdiPlusImage:public RtwImage
{
	friend class CImageFactoryContainerImp;
	friend class CRtAndGdiPlusRender;

protected:
	CRtAndGdiPlusImage(RtwImage* vpRtImage,RtwImage* vpGdiPlusImage)
		:mpRtImage(vpRtImage),mpGdiPlusImage(vpGdiPlusImage){/*LOG("");*/};

	~CRtAndGdiPlusImage()
	{/*LOG("");*/
		DROP_RTUI_OBJECT(mpRtImage);
		DROP_RTUI_OBJECT(mpGdiPlusImage);
	};
public:
	// base class inherit
	virtual void OnBeforeDestroy() 
	{/*LOG("");*/
		RtwImage::OnBeforeDestroy(); 
	}

	//加载图形文件.
	virtual bool LoadFromFile(const std::string& vrFileName) 
	{/*LOG("");*/
		return (mpRtImage->LoadFromFile(vrFileName) 
			&& mpGdiPlusImage->LoadFromFile(vrFileName));
	};

	//从内存加载图形
	virtual bool LoadFromMemory(const void* vpData, int vLength)
	{/*LOG("");*/
		return ( mpRtImage->LoadFromMemory(vpData,vLength) 
			&& mpGdiPlusImage->LoadFromMemory(vpData,vLength));
	};

	//设置图形透明
	virtual void SetTransparency(Real vTransparency) 
	{/*LOG("");*/
		mpRtImage->SetTransparency(vTransparency);
		mpGdiPlusImage->SetTransparency(vTransparency);

		//透明度
		m_Transparency = vTransparency;
		if (m_Transparency > 1.0)
			m_Transparency = 1.0;
		else if (m_Transparency < 0.0)
			m_Transparency = 0.0;

		if (m_Transparency == 0.0)//透明度不能为0
		{
			m_Color.a = 0xFF;
		}
		else
		{
			m_Color.a = 255 * vTransparency;
		}
	};

	//是否启用效果(纹理混合, 半透明之类)
	virtual void SetBlend(bool vBlend) 
	{/*LOG("");*/
		mpRtImage->SetBlend(vBlend);
		mpGdiPlusImage->SetBlend(vBlend);
		m_bBlend = vBlend;
	};

	//设置源,将指定矩阵的图片载入图片对象
	virtual void SetSourceRect(RtwRect& vrRect)
	{/*LOG("");*/
		mpRtImage->SetSourceRect(vrRect);
		mpGdiPlusImage->SetSourceRect(vrRect);
		m_Source = vrRect;
	}
private:
	RtwImage*		mpRtImage;
	RtwImage*		mpGdiPlusImage;
};
#endif