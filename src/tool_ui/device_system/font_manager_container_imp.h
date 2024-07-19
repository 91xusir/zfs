#ifndef FONT_MANAGER_CONTAINER_IMP_H
#define FONT_MANAGER_CONTAINER_IMP_H

#include "ui/rtw_ui.h"
#include "gdi_plus_font_manager_imp.h"
#include "font_container_imp.h"

//�����������
class CFontManagerContainerImp:public IUiFontManager
{
public:
	CFontManagerContainerImp(CUiFontManagerImpl_Rt*	vpRtManager,CGdiPlusFontManagerImp* vpGdiPlusManager)
		:mpRtManager(vpRtManager),mpGdiPlusManager(vpGdiPlusManager)
	{/*LOG("");*/
	};

	virtual ~CFontManagerContainerImp()
	{/*LOG("");*/
		delete mpRtManager;
		delete mpGdiPlusManager;
	};


	virtual void Render() //������Ⱦ
	{/*LOG("");*/
		mpRtManager->Render();
		mpGdiPlusManager->Render();
	};

protected:
	virtual RtwFont* OnCreateFont(const std::string& vrFontName,const int vFontSize , int nFontStyle ) //����һ��ϵ������
	{/*LOG("");*/
		RtwFont* pRtFont		= mpRtManager->CreateFont(vrFontName, vFontSize);
		RtwFont* pGdiPlusFont	= mpGdiPlusManager->CreateFont(vrFontName, vFontSize);

		if(pRtFont==NULL || pGdiPlusFont==NULL)
		{
			delete pRtFont;
			delete pGdiPlusFont;
			return NULL;
		}

		return new CFontContainerImp(vrFontName,vFontSize,(CUiFontImpl_Rt*)pRtFont,(CGdiPlusFontImp*)pGdiPlusFont);
	};
private:
	CUiFontManagerImpl_Rt*			mpRtManager;
	CGdiPlusFontManagerImp*			mpGdiPlusManager;
};
#endif