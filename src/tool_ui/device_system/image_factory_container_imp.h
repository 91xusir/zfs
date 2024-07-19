#ifndef IMAGE_FACTORY_CONTAINER_IMP_H
#define IMAGE_FACTORY_CONTAINER_IMP_H

#include "ui/rtw_ui.h"
#include "image_container_imp.h"

class CImageFactoryContainerImp:public IUiImageFactory
{
public:
	CImageFactoryContainerImp(CUiImageFactoryImpl_Rt* vpRtFactory,
		CUiGdiPlusImageFactoryImp* vpGdiPlusFactory)
		:mpRtFactory(vpRtFactory),mpGdiPlusFactory(vpGdiPlusFactory)
	{/*LOG("");*/};
	virtual ~CImageFactoryContainerImp()
	{/*LOG("");*/
		delete mpRtFactory;
		delete mpGdiPlusFactory;
	};

	virtual RtwImage* RealCreateImage() 
	{/*LOG("");*/
		return NULL;
	};
private:
	CUiImageFactoryImpl_Rt*			mpRtFactory;
	CUiGdiPlusImageFactoryImp*		mpGdiPlusFactory;
};
#endif