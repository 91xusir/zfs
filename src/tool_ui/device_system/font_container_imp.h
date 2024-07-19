#ifndef FONT_CONTAINER_IMP_H
#define FONT_CONTAINER_IMP_H

#include "ui/rtw_ui.h"
#include "gdi_plus_font_imp.h"

class CFontContainerImp:public RtwFont
{
public:
	CFontContainerImp(
						const std::string& vrFontName,	//������
						const int vFontSize,			//�����С
						CUiFontImpl_Rt* vpRtFont,		//UI����ָ��
						CGdiPlusFontImp* vpGdiPlusFont	//GDI����ָ��
					):RtwFont(vrFontName,vFontSize),
					mpRtFont(vpRtFont),
					mpGdiPlusFont(vpGdiPlusFont){/*LOG("");*/};
	
	virtual ~CFontContainerImp()
	{/*LOG("");*/
		delete mpRtFont;
		delete mpGdiPlusFont;
	};
public:
	virtual void Close()//�ر�����
	{/*LOG("");*/
		mpRtFont->Close();
		mpGdiPlusFont->Close();
	};

	virtual void OnInvalidateDevice() //ˢ���豸
	{/*LOG("");*/
		mpRtFont->OnInvalidateDevice();
		mpGdiPlusFont->OnInvalidateDevice();
	};
	
	virtual void OnRestoreDevice()//�����豸?
	{/*LOG("");*/
		mpRtFont->OnRestoreDevice();
		mpGdiPlusFont->OnRestoreDevice();
	};

	virtual void DrawString(const std::string& str, const RtwRect& rect, 
		const RtwPixel& textColor, bool bWordWrap ,SUiTextAlignment align, 
		bool bUnderLine , const RtwRect* pClipRect, int RenderSize = 0) //���ֻ���
	{/*LOG("");*/
		mpRtFont->DrawString(str,rect,textColor,bWordWrap,align,bUnderLine,pClipRect,RenderSize);
		mpGdiPlusFont->DrawString(str,rect,textColor,bWordWrap,align,bUnderLine,pClipRect,RenderSize);
	};

	virtual void DrawString3D(const std::string& str, const SRect3D& rect,
		const RtwPixel& textColor, bool bWordWrap , SUiTextAlignment align,
		bool bUnderLine ,const RtwRect* pClipRect, int RenderSize = 0 )
	{/*LOG("");*/
	}

	virtual void Flush()//�ַ�ˢ��
	{/*LOG("");*/
		mpRtFont->Flush();
		mpGdiPlusFont->Flush();
	}

	virtual int GetStringWidth(const std::string& str, int length = -1, int RenderSize = 0)//��ȡ�ַ����
	{/*LOG("");*/
		return mpRtFont->GetStringWidth(str,length,RenderSize);
	};

	virtual bool PickCharacter(
		const SPoint& point, int& outIndex, bool& outDoubleByte, bool& outNear, 
		const std::string& str, const RtwRect& rect, int RenderSize = 0)
	{/*LOG("");*/
		return (mpRtFont->PickCharacter(point,outIndex,outDoubleByte,outNear,str,rect,RenderSize) && 
			mpGdiPlusFont->PickCharacter(point,outIndex,outDoubleByte,outNear,str,rect,RenderSize));
	};

	virtual bool PickCharacter(int width, int& outIndex, bool& outDoubleByte, bool& outNear, 
		const std::string& str, int RenderSize = 0) 
	{/*LOG("");*/
		return ( mpRtFont->PickCharacter(width,outIndex,outDoubleByte,outNear,str,RenderSize) && 
			mpGdiPlusFont->PickCharacter(width,outIndex,outDoubleByte,outNear,str,RenderSize));
	}
	virtual void ResetTexture(bool isReset){}
private:
	CUiFontImpl_Rt*				mpRtFont;
	CGdiPlusFontImp*			mpGdiPlusFont;
};
#endif