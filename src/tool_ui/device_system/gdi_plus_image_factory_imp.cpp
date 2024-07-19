#include "gdi_plus_image_factory_imp.h"
#include "gdi_plus_image_imp.h"

//image工厂
RtwImage* CUiGdiPlusImageFactoryImp::RealCreateImage()
{/*LOG("");*/
	return NULL;//pImage;
}

CRefPtr<Bitmap> CUiGdiPlusImageFactoryImp::GetBitmap(const string& vrFileName)
{/*LOG("");*/
	map<string,CBitmapPtr>::iterator It;
	It = mBitmapList.find(vrFileName);

	if(It==mBitmapList.end())
	{//不存在
		CBitmapPtr p(LoadBitmapEx(vrFileName));
		if(p!=NULL)
		{
			mBitmapList[vrFileName] = p;
		}
		return p;
	}
	else
	{//已经存在
		return It->second;
	}
}

Bitmap* CUiGdiPlusImageFactoryImp::LoadBitmapFrom(BitmapData& vrUserData)
{/*LOG("");*/
	Bitmap* pBitmap = new Bitmap(vrUserData.Width,vrUserData.Height,vrUserData.PixelFormat);
	Rect UserRect(0,0,vrUserData.Width,vrUserData.Height);
	Status Ret= pBitmap->LockBits(&UserRect,ImageLockModeWrite|ImageLockModeUserInputBuf,vrUserData.PixelFormat,&vrUserData);
	if(Ret==Ok)
	{
		pBitmap->UnlockBits(&vrUserData);
		return pBitmap;
	}
	else
	{
		return NULL;
	}
}

bool ExistFile(const string& vrFileName)
{/*LOG("");*/
	FILE* p=fopen(vrFileName.c_str(),"rb");
	if(p==NULL)
	{
		return false;
	}
	else
	{
		fclose(p);
		return true;
	}
}

string ChangeFileName(const string& vrFileName)
{/*LOG("");*/
	string Temp = vrFileName.substr(0,vrFileName.find('.'));

	string FullName;
	FullName=Temp+".bmp";

	if(ExistFile(FullName))
	{
		return FullName;
	}

	FullName=Temp+".tga";

	if(ExistFile(FullName))
	{
		return FullName;
	}

	FullName=Temp+".jpg";

	if(ExistFile(FullName))
	{
		return FullName;
	}

	FullName=Temp+".dds";
	if(ExistFile(FullName))
	{
		return FullName;
	}

	FullName.clear();
	return FullName;
}

Bitmap* CUiGdiPlusImageFactoryImp::LoadBitmapExTemp(const string& vrFileName)
{/*LOG("");*/
	//用户数据//内容
	const int ColorBits=4;
	const int Width=100;
	const int Height=50;

	BYTE* pUser=new BYTE[Height*Width*ColorBits];

	for(int i=0;i!=Width*Height*ColorBits;)
	{
		pUser[i++]=0;//b
		pUser[i++]=255;//g
		pUser[i++]=255;//r
		pUser[i++]=128;//a
	}

	//数据属性
	BitmapData UserData;
	UserData.Height=Height;
	UserData.Width=Width;
	UserData.PixelFormat =PixelFormat32bppARGB;
	UserData.Reserved=NULL;
	UserData.Scan0=pUser;
	UserData.Stride=UserData.Width*ColorBits;

	Bitmap* pBitmap = LoadBitmapFrom(UserData);

	//释放用户数据
	delete[] pUser;

	return pBitmap;
}

Bitmap* CUiGdiPlusImageFactoryImp::LoadBitmapEx(const string& vrFileName)
{/*LOG("");*/
	//将文件名转换得到一个名字
	string FileName = ChangeFileName(vrFileName);
	if(FileName.empty())
	{
		return NULL;
	}

	//用户自己将数据载入到内存
	RtgImage TempImage;

	TempImage.Load(FileName.c_str());
	const int ColorBits=4;

	//用户数据//内容//temp
	BYTE* pImageData =(BYTE*) TempImage.GetRawData();
	int Width=TempImage.info.iWidth;
	int Height=TempImage.info.iHeight;
	int ImageSize = Width*Height*ColorBits;

	BYTE* pUser=new BYTE[ImageSize];

	int i=0;
	int j=0;
	for(int HeightStep=0;HeightStep!=Height;++HeightStep)
	{
		for(int WidthStep=0;WidthStep!=Width;++WidthStep,j+=4)
		{
			pUser[i++] = pImageData[j+2];//b
			pUser[i++] = pImageData[j+1];//g
			pUser[i++] = pImageData[j];//r
			pUser[i++] = pImageData[j+3];//a
		}
	}

	//数据属性
	BitmapData UserData;
	UserData.Height=Height;//TempImage.info.iHeight;
	UserData.Width=Width;//TempImage.info.iWidth;
	UserData.PixelFormat =PixelFormat32bppARGB;
	UserData.Reserved=NULL;
	UserData.Scan0=pUser;
	UserData.Stride=Width* ColorBits;//;UserData.Width*ColorBits;//!!!!!!!!!!!!!!!!!

	Bitmap* pBitmap = LoadBitmapFrom(UserData);

	//释放用户数据
	delete[] pUser;

	return pBitmap;
}