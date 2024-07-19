

#ifndef  __TextureRLE_H__
#define  __TextureRLE_H__
#include "TextureHeader.h"
namespace rt_graph 
{		
	struct STexturePixel
	{
		uchar  _ucR,_ucG,_ucB,_ucA;
	};

	bool rtgiLoadImageRLEFile(const char* szFilename, RtgImage* pImage);
	bool rtgiLoadImageRLE    (RtArchive* pArc, RtgImage* pImage,bool bflip=false);



	class  CTextureRLE
	{
	public:
		CTextureRLE( RtgImage* );
		~CTextureRLE( );
	public:	
		///!通过一个文件创建一个RLE文件
		void  LoadRLEFromFile( const char* pcTextureName );
		///!通过一个内存创建一个RLE文件
		bool  LoadRLEFromMemoryCompress  ( const uchar* pcTextureMemory , ulong nMemoryLength );
		bool  LoadRLEFromMemoryUncompress( const uchar* pcTextureMemory );	
	protected:
		///!重置
		inline void  ResetBuffer( )  { /*if( mpkTextureBuffer ) delete[] mpkTextureBuffer ; mpkTextureBuffer = NULL;*/ }
	protected:
		///!从A8R8G8B8读取文件
		void LoadTextureA8R8G8B8( const uchar* );	
		void LoadTextureIA8C8   ( const uchar* );	
		void LoadTextureA4C12   ( const uchar* );	
		void LoadTextureI8      ( const uchar* );	
		void LoadTextureR8G8B8  ( const uchar* );	
		void LoadTextureI10     ( const uchar* );	
		void LoadTextureA2C14   ( const uchar* );	
		void LoadTextureA2C4    ( const uchar* );
	protected:
		///!把一个RLE文件存储为TGA文件
		void SaveRLEToTGA( const char* );
	protected:
		///!文件头
		STextureHeader   mkTextureHeader;
		///!文件的数据
		///STexturePixel*   mpkTextureBuffer;
		RtgImage*        mpkDestImage;
	public:
		static const char* const  ms_szRLEPostFix;
	};    
}
#endif