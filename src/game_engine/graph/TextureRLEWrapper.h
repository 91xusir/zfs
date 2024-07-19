

#ifndef  __TextureRLEWrapper_H__
#define  __TextureRLEWrapper_H__

#include "TextureHeader.h"
class  CTextureRLE;

namespace rt_graph 
{

	///!Pixel 所在的点
	struct  SPoint
	{
		short _sX,_sY;
	};
	typedef std::vector<SPoint>  SPointArray;
	///!普通文件格式
	struct  NormalPixel
	{
		uchar  _uRed;
		uchar  _uGreen;
		uchar  _uBlue;
		uchar  _uAlpha;	
	};
	struct  SamePixel
	{
		NormalPixel   _kPixel;
		SPointArray   _kPointArray;
	};

	struct SameAlpha
	{
		uchar        _uAlpha;
		SPointArray  _kPointArray;
	};

	struct  STextPixelDiff
	{
		int     nRedMin   , nRedMax; 	
		int     nGreenMin , nGreenMax;	
		int     nBlueMin  , nBlueMax;	
		int     nAlphaMin , nAlphaMax;	
	};



	struct STextureIndex
	{
		int nDiffuseIndex;
		int nAlphaIndex;
	};

	typedef std::vector<SamePixel>            SamePixelArray;
	typedef EXT_SPACE::unordered_map<ulong,SamePixel>    SamePixelHashmap;
	typedef std::vector<SameAlpha>            SameAlphaArray;
	typedef std::vector<NormalPixel>          NormalPixelArray;
	typedef std::vector<STextureIndex>        TextureIndexArray;

	class  CTextureRLEWrapper
	{
	public:
		enum
		{
			E_RESERV_LENGTH    = 1024 * 1024 * 4 ,
			E_RESERV_LENGTH_EX = 1024 * 1024 * 5
		};
	private:
		CTextureRLEWrapper( )  {}
		~CTextureRLEWrapper( )  {}
	public:
		///!初始化
		static void  InitTextureRLEWrapper( );
		static void  DestroyTextureRLEWrapper( );
		///!输入体格cxImage,根据图片决定采用何种方式压缩
		static void  CompressTextureToRLE( RtgImage* , const char* pcTextureName );
	protected:
		///!分析文件
		static void  AnalyTexture( RtgImage* pkScr,SamePixelArray& kSamePixel ,  SameAlphaArray& kSameAlpha , STextPixelDiff& kPixelDiff , TextureIndexArray& kIndexArray);
		///!职能检测图像文件采用什么方式压缩	
		static int   AnalyResultToCompress( bool bHasAlpha ,int nOriginalSize ,SamePixelArray& kSamePixel ,  SameAlphaArray& kSameAlpha , STextPixelDiff& kPixelDiff  );

	protected:
		static void  SaveTexture_A8R8G8B8( uchar*& pkBuffer, int& nTextureLength , RtgImage* pkScr );
		static void  SaveTexture_R8G8B8  ( uchar*& pkBuffer, int& nTextureLength , RtgImage* pkScr );
		static void  SaveTexture_A4C12   ( uchar*& pkBuffer, int& nTextureLength , RtgImage* pkScr, SamePixelArray& kSamePixel ,  SameAlphaArray& kSameAlpha , TextureIndexArray& kIndexArray );
		static void  SaveTexture_A2C14   ( uchar*& pkBuffer, int& nTextureLength , RtgImage* pkScr, SamePixelArray& kSamePixel ,  SameAlphaArray& kSameAlpha , TextureIndexArray& kIndexArray );
		static void  SaveTexture_I8      ( uchar*& pkBuffer, int& nTextureLength , RtgImage* pkScr, SamePixelArray& kSamePixel ,  SameAlphaArray& kSameAlpha , TextureIndexArray& kIndexArray );
		static void  SaveTexture_I10     ( uchar*& pkBuffer, int& nTextureLength , RtgImage* pkScr, SamePixelArray& kSamePixel ,  SameAlphaArray& kSameAlpha , TextureIndexArray& kIndexArray  );
		static void  SaveTexture_IA8C8   ( uchar*& pkBuffer, int& nTextureLength , RtgImage* pkScr, SamePixelArray& kSamePixel ,  SameAlphaArray& kSameAlpha , TextureIndexArray& kIndexArray  );
		static void  SaveTexture_I4C4    ( uchar*& pkBuffer, int& nTextureLength , RtgImage* pkScr, SamePixelArray& kSamePixel ,  SameAlphaArray& kSameAlpha , TextureIndexArray& kIndexArray  );
		///!存储文件
		static void  SaveRLEBuffe( const uchar* pkBuffer, int nTextureLength , const char* pcTextureName );
	public:
		static void  ResetReservBuffer( );
	public:
		///!缓存
		static uchar*  ms_pcReservBuffer;
		static uchar*  ms_pcCompressBuffer;

	};
    
}

#endif
