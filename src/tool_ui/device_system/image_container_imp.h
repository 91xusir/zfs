#ifndef IMAGE_CONTAINER_IMP_H
#define IMAGE_CONTAINER_IMP_H

#include "ui/rtw_ui.h"
using ui::RtwImage;

//ͼ�ΰ�װ
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

	//����ͼ���ļ�.
	virtual bool LoadFromFile(const std::string& vrFileName) 
	{/*LOG("");*/
		return (mpRtImage->LoadFromFile(vrFileName) 
			&& mpGdiPlusImage->LoadFromFile(vrFileName));
	};

	//���ڴ����ͼ��
	virtual bool LoadFromMemory(const void* vpData, int vLength)
	{/*LOG("");*/
		return ( mpRtImage->LoadFromMemory(vpData,vLength) 
			&& mpGdiPlusImage->LoadFromMemory(vpData,vLength));
	};

	//����ͼ��͸��
	virtual void SetTransparency(Real vTransparency) 
	{/*LOG("");*/
		mpRtImage->SetTransparency(vTransparency);
		mpGdiPlusImage->SetTransparency(vTransparency);

		//͸����
		m_Transparency = vTransparency;
		if (m_Transparency > 1.0)
			m_Transparency = 1.0;
		else if (m_Transparency < 0.0)
			m_Transparency = 0.0;

		if (m_Transparency == 0.0)//͸���Ȳ���Ϊ0
		{
			m_Color.a = 0xFF;
		}
		else
		{
			m_Color.a = 255 * vTransparency;
		}
	};

	//�Ƿ�����Ч��(������, ��͸��֮��)
	virtual void SetBlend(bool vBlend) 
	{/*LOG("");*/
		mpRtImage->SetBlend(vBlend);
		mpGdiPlusImage->SetBlend(vBlend);
		m_bBlend = vBlend;
	};

	//����Դ,��ָ�������ͼƬ����ͼƬ����
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