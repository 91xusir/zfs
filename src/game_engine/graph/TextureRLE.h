

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
		///!ͨ��һ���ļ�����һ��RLE�ļ�
		void  LoadRLEFromFile( const char* pcTextureName );
		///!ͨ��һ���ڴ洴��һ��RLE�ļ�
		bool  LoadRLEFromMemoryCompress  ( const uchar* pcTextureMemory , ulong nMemoryLength );
		bool  LoadRLEFromMemoryUncompress( const uchar* pcTextureMemory );	
	protected:
		///!����
		inline void  ResetBuffer( )  { /*if( mpkTextureBuffer ) delete[] mpkTextureBuffer ; mpkTextureBuffer = NULL;*/ }
	protected:
		///!��A8R8G8B8��ȡ�ļ�
		void LoadTextureA8R8G8B8( const uchar* );	
		void LoadTextureIA8C8   ( const uchar* );	
		void LoadTextureA4C12   ( const uchar* );	
		void LoadTextureI8      ( const uchar* );	
		void LoadTextureR8G8B8  ( const uchar* );	
		void LoadTextureI10     ( const uchar* );	
		void LoadTextureA2C14   ( const uchar* );	
		void LoadTextureA2C4    ( const uchar* );
	protected:
		///!��һ��RLE�ļ��洢ΪTGA�ļ�
		void SaveRLEToTGA( const char* );
	protected:
		///!�ļ�ͷ
		STextureHeader   mkTextureHeader;
		///!�ļ�������
		///STexturePixel*   mpkTextureBuffer;
		RtgImage*        mpkDestImage;
	public:
		static const char* const  ms_szRLEPostFix;
	};    
}
#endif