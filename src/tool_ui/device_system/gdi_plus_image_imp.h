#ifndef GDI_PLUS_IMAGE_IMP_H
#define GDI_PLUS_IMAGE_IMP_H

#include "ui/rtw_ui.h"
#include "ref_pointer.h"
#include "gdi_plus_image_factory_imp.h"
using fox::CRefPtr;

//image//gdi+й╣ож 
class CUiGdiPlusImageImp : public RtwImage
{
	typedef CRefPtr<Bitmap> CBitmapPtr;
	friend class CUiGdiPlusImageFactoryImp;
	friend class CGdiPlusRenderImp;
protected:
	CUiGdiPlusImageImp(CUiGdiPlusImageFactoryImp& vrFactory):mpBitmap(NULL),mrFactory(vrFactory){/*LOG("");*/};
	virtual ~CUiGdiPlusImageImp(){/*LOG("");*/};
public:
	virtual void OnBeforeDestroy()
	{/*LOG("");*/
		RtwImage::OnBeforeDestroy();
	};

	virtual bool LoadFromFile(const string& vrFileName)
	{/*LOG("");*/
		if (vrFileName.empty())
		{
			return false;
		}
		m_FileName = vrFileName;
		mpBitmap = mrFactory.GetBitmap(vrFileName);

		if(mpBitmap!=NULL)
		{
			SetSourceRect(RtwRect(0,0,mpBitmap->GetWidth()-1,mpBitmap->GetHeight()-1));
		}

		return true;
	};

	virtual bool LoadFromMemory(const void* data, int length)
	{/*LOG("");*/
		return false;
	};
	virtual void SetTransparency(Real vTransparency)
	{/*LOG("");*/
		m_Transparency = vTransparency;
		m_Transparency = vTransparency > 1.0 ? 1.0 : m_Transparency;
		m_Transparency = vTransparency < 0.0 ? 0.0 : m_Transparency;

		if (m_Transparency == 0.0)
		{
			m_Color.a = 0xFF;
		}
		else
		{
			m_Color.a = 255 * vTransparency;
		}

	};

	virtual void SetBlend(bool bBlend)
	{/*LOG("");*/
		m_bBlend = bBlend;
	};

	virtual void SetSourceRect(RtwRect& vrRect)
	{/*LOG("");*/
		m_Source=vrRect;
	}

private:
	CBitmapPtr 					mpBitmap;
	CUiGdiPlusImageFactoryImp&	mrFactory;
};
#endif//GDI_PLUS_IMAGE_IMP_H