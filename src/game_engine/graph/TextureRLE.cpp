

#include "rtg_graph_inter.h"
#include "TextureRLE.h"
#include "TextureRLEWrapper.h"
#include "core/zlib.h"


namespace rt_graph 
{

	const char* const CTextureRLE::ms_szRLEPostFix = ".rle";
    #define  READ_BUFFER( pTotalBuffer , pDestBuffer , nSize )  { memcpy( pDestBuffer,pTotalBuffer,nSize); pTotalBuffer+=nSize; }

	CTextureRLE::CTextureRLE( RtgImage* pkImage )
		:mpkDestImage( pkImage )/*mpkTextureBuffer( NULL )*/

	{

	}
	CTextureRLE::~CTextureRLE()
	{
		/*if( mpkTextureBuffer )
		{
			delete mpkTextureBuffer;
			mpkTextureBuffer = NULL;
		}*/
		mpkDestImage = NULL;
	}

	bool  CTextureRLE::LoadRLEFromMemoryCompress  ( const uchar* pcCompressedMemoryScr , ulong nCompressedLength )
	{
		if( pcCompressedMemoryScr && nCompressedLength )
		{
			uchar*  pucRealBuffer = CTextureRLEWrapper::ms_pcReservBuffer ;
			ulong   ulRealLength  = CTextureRLEWrapper::E_RESERV_LENGTH_EX;
			int nErr              = uncompress( (Bytef*)pucRealBuffer,&ulRealLength,(Bytef*)pcCompressedMemoryScr, nCompressedLength );
			if( 0 == nErr )
			{
				return LoadRLEFromMemoryUncompress( pucRealBuffer );				 
			}			
		}    
		return false;
	}

	bool  CTextureRLE::LoadRLEFromMemoryUncompress( const uchar* pucRealBuffer )
	{
		if( pucRealBuffer && mpkDestImage )
		{
			READ_BUFFER( pucRealBuffer , &mkTextureHeader , sizeof( STextureHeader ) );
			ResetBuffer( );
            //mpkTextureBuffer = RT_NEW STexturePixel[ mkTextureHeader._sTextureWidth * mkTextureHeader._sTextureHeight ];
			

			if( mkTextureHeader._cTextureType == ETEX_ALPHA_IA4C4  ||
				mkTextureHeader._cTextureType == ETEX_ALPHA_IA4C12 ||
				mkTextureHeader._cTextureType == ETEX_ALPHA_IA2C14 ||
				mkTextureHeader._cTextureType == ETEX_ALPHA_IA8C8  )
			{
                mpkDestImage->info.bAlpha      = true;
			}else
			{
                mpkDestImage->info.bAlpha      = false;
			}			
			mpkDestImage->info.bDDS        = false;
			mpkDestImage->info.iWidth      = mkTextureHeader._sTextureWidth;
			mpkDestImage->info.iHeight     = mkTextureHeader._sTextureHeight;
			mpkDestImage->info.iSizeOfData = mkTextureHeader._sTextureWidth * mkTextureHeader._sTextureHeight * 4;
			mpkDestImage->info.iDxtcFormat = 0;
			int iDataSize = mkTextureHeader._sTextureWidth * mkTextureHeader._sTextureHeight * 4;
			mpkDestImage->data.pData = (unsigned char*)RtCoreMem().Alloc(iDataSize, __FILE__, __LINE__);			
			switch( mkTextureHeader._cTextureType )
			{
			case ETEX_ALPHA_IA4C4:
				{
					LoadTextureA2C4( pucRealBuffer );return true;			
				}
				break;
			case ETEX_ALPHA_IA4C12:
				{
					LoadTextureA4C12( pucRealBuffer );return true;					
				}
				break;	
			case ETEX_ALPHA_IA2C14:
				{
					LoadTextureA2C14( pucRealBuffer );return true;					
				}
				break;
			case ETEX_I8:
				{
					LoadTextureI8( pucRealBuffer );return true;
				}
				break;
			case ETEX_I10:
				{
					LoadTextureI10( pucRealBuffer );return true;					
				}
				break;
			case ETEX_ALPHA_IA8C8:
				{
					LoadTextureIA8C8( pucRealBuffer );return true;
				}
				break;
			case ETEX_A8R8G8B8:
				{
					LoadTextureA8R8G8B8( pucRealBuffer );return true;					
				}
				break;
			case ETEX_R8G8B8:
				{
					LoadTextureR8G8B8( pucRealBuffer );return true;
				}
				break;
			default:
				{
					return false;
				}
				break;
			}
		}
		return false;
	}

	///!通过一个内存创建一个RLE文件
	void  CTextureRLE::LoadRLEFromFile( const char* pcTextureName )
	{
		FILE*  pkFile = fopen( pcTextureName , "rb+" );
		if( pkFile )
		{
			int nTextureLength = 0;
			fread( &nTextureLength , 4 , 1 , pkFile );
			if( nTextureLength > 0 )
			{
				CTextureRLEWrapper::ResetReservBuffer();
				uchar* pkBuffer   = CTextureRLEWrapper::ms_pcCompressBuffer;										
				ulong nUnCompLen  = CTextureRLEWrapper::E_RESERV_LENGTH_EX;
				fread( pkBuffer , nTextureLength , 1 , pkFile );
				LoadRLEFromMemoryCompress( pkBuffer , nTextureLength );
			}
			fclose( pkFile );
		}	
		//SaveRLEToTGA( "c:\\1.tga" );
	}
	///!从A2C4读取文件
	void CTextureRLE::LoadTextureA2C4( const uchar* pkBuffer )
	{
		int nPalAlphaWidth = 0;
		int nColorWidth    = 0;
		STextureAlphaPal  kScrAlphaPal;
		READ_BUFFER( pkBuffer , &kScrAlphaPal._TexturePalLength , sizeof( kScrAlphaPal._TexturePalLength ) );
		kScrAlphaPal._pcTexturePal    = RT_NEW uchar[ kScrAlphaPal._TexturePalLength ];
		READ_BUFFER( pkBuffer , kScrAlphaPal._pcTexturePal , kScrAlphaPal._TexturePalLength );	
		STextureRGBPal  kTextureRGBPal;
		READ_BUFFER( pkBuffer , &kTextureRGBPal._TexturePalLength , sizeof( kTextureRGBPal._TexturePalLength ) );
		kTextureRGBPal._pcTexturePal = RT_NEW uchar[ kTextureRGBPal._TexturePalLength * 3 ];
		READ_BUFFER( pkBuffer , kTextureRGBPal._pcTexturePal , kTextureRGBPal._TexturePalLength * 3 );
		SIndex44  kNeedInsert;
		for( int i = 0 ; i < mkTextureHeader._sTextureHeight ; i ++ )
		{
			for( int j = 0 ; j < mkTextureHeader._sTextureWidth ; j ++ )
			{						
				READ_BUFFER( pkBuffer , &kNeedInsert ,sizeof( kNeedInsert ) );
				int nCurrentIndex = i * mkTextureHeader._sTextureWidth + j;
				mpkDestImage->data.pData[ nCurrentIndex * 4 + 0 ] = kTextureRGBPal._pcTexturePal[ kNeedInsert._a * 3 + 0 ];
				mpkDestImage->data.pData[ nCurrentIndex * 4 + 1 ] = kTextureRGBPal._pcTexturePal[ kNeedInsert._a * 3 + 1 ];
				mpkDestImage->data.pData[ nCurrentIndex * 4 + 2 ] = kTextureRGBPal._pcTexturePal[ kNeedInsert._a * 3 + 2 ];
				mpkDestImage->data.pData[ nCurrentIndex * 4 + 3 ] = kScrAlphaPal._pcTexturePal  [ kNeedInsert._b ];
			}
		}
		delete[] kScrAlphaPal._pcTexturePal;
		delete[] kTextureRGBPal._pcTexturePal;
	}
	///!从A2C14读取文件
	void CTextureRLE::LoadTextureA2C14( const uchar* pkBuffer )
	{
		int nPalAlphaWidth = 0;
		int nColorWidth    = 0;
		STextureAlphaPal  kScrAlphaPal;
		READ_BUFFER( pkBuffer , &kScrAlphaPal._TexturePalLength , sizeof( kScrAlphaPal._TexturePalLength ) );
		kScrAlphaPal._pcTexturePal    = RT_NEW uchar[ kScrAlphaPal._TexturePalLength ];
		READ_BUFFER( pkBuffer , kScrAlphaPal._pcTexturePal , kScrAlphaPal._TexturePalLength );	
		STextureRGBPal  kTextureRGBPal;
		READ_BUFFER( pkBuffer , &kTextureRGBPal._TexturePalLength , sizeof( kTextureRGBPal._TexturePalLength ) );
		kTextureRGBPal._pcTexturePal = RT_NEW uchar[ kTextureRGBPal._TexturePalLength * 3 ];
		READ_BUFFER( pkBuffer , kTextureRGBPal._pcTexturePal , kTextureRGBPal._TexturePalLength * 3 );
		SAlpha2Color14  kNeedInsert;
		for( int i = 0 ; i < mkTextureHeader._sTextureHeight ; i ++ )
		{
			for( int j = 0 ; j < mkTextureHeader._sTextureWidth ; j ++ )
			{						
				READ_BUFFER( pkBuffer , &kNeedInsert ,sizeof( kNeedInsert ) );
				int nCurrentIndex = i * mkTextureHeader._sTextureWidth + j;
				mpkDestImage->data.pData[ nCurrentIndex * 4 + 0 ] = kTextureRGBPal._pcTexturePal[ kNeedInsert.usColor * 3 + 0 ];
				mpkDestImage->data.pData[ nCurrentIndex * 4 + 1 ] = kTextureRGBPal._pcTexturePal[ kNeedInsert.usColor * 3 + 1 ];
				mpkDestImage->data.pData[ nCurrentIndex * 4 + 2 ] = kTextureRGBPal._pcTexturePal[ kNeedInsert.usColor * 3 + 2 ];
				mpkDestImage->data.pData[ nCurrentIndex * 4 + 3 ] = kScrAlphaPal._pcTexturePal  [ kNeedInsert.ucAlpha ];
			}
		}
		delete[] kScrAlphaPal._pcTexturePal;
		delete[] kTextureRGBPal._pcTexturePal;
	}
	///!从I10读取文件
	void CTextureRLE::LoadTextureI10( const uchar* pkBuffer )
	{
		int nColorWidth    = 0;		
		STextureRGBPal  kTextureRGBPal;
		READ_BUFFER( pkBuffer , &kTextureRGBPal._TexturePalLength , sizeof( kTextureRGBPal._TexturePalLength ) );
		kTextureRGBPal._pcTexturePal = RT_NEW uchar[ kTextureRGBPal._TexturePalLength * 3 ];
		READ_BUFFER( pkBuffer , kTextureRGBPal._pcTexturePal , kTextureRGBPal._TexturePalLength * 3 );
		int nTotalIndex       = mkTextureHeader._sTextureWidth * mkTextureHeader._sTextureHeight;	
		int nRealIndex        = nTotalIndex / 3 + 1;
		int nInsertSize       = sizeof( SIndex10 );
		int bCurrentTotal     = 0; 
		int nRealPixelIndex   = 0;		
		SIndex10  kNeedInsert;	
		for( int i = 0 ; i < nRealIndex ; i ++ )
		{
			READ_BUFFER( pkBuffer , &kNeedInsert , sizeof( kNeedInsert ) );		
			if( nRealPixelIndex < nTotalIndex )
			{
				mpkDestImage->data.pData[ nRealPixelIndex * 4 + 0 ] = kTextureRGBPal._pcTexturePal[ kNeedInsert._a * 3 + 0 ];
				mpkDestImage->data.pData[ nRealPixelIndex * 4 + 1 ] = kTextureRGBPal._pcTexturePal[ kNeedInsert._a * 3 + 1 ];
				mpkDestImage->data.pData[ nRealPixelIndex * 4 + 2 ] = kTextureRGBPal._pcTexturePal[ kNeedInsert._a * 3 + 2 ];
				mpkDestImage->data.pData[ nRealPixelIndex * 4 + 3 ] = 255;
			}
			nRealPixelIndex ++;
			if( nRealPixelIndex < nTotalIndex )
			{
				mpkDestImage->data.pData[ nRealPixelIndex * 4 + 0 ] = kTextureRGBPal._pcTexturePal[ kNeedInsert._b * 3 + 0 ];
				mpkDestImage->data.pData[ nRealPixelIndex * 4 + 1 ] = kTextureRGBPal._pcTexturePal[ kNeedInsert._b * 3 + 1 ];
				mpkDestImage->data.pData[ nRealPixelIndex * 4 + 2 ] = kTextureRGBPal._pcTexturePal[ kNeedInsert._b * 3 + 2 ];
				mpkDestImage->data.pData[ nRealPixelIndex * 4 + 3 ] = 255;
			}
			nRealPixelIndex ++;
			if( nRealPixelIndex < nTotalIndex )
			{
				mpkDestImage->data.pData[ nRealPixelIndex * 4 + 0 ]= kTextureRGBPal._pcTexturePal[ kNeedInsert._c * 3 + 0 ];
				mpkDestImage->data.pData[ nRealPixelIndex * 4 + 1 ]= kTextureRGBPal._pcTexturePal[ kNeedInsert._c * 3 + 1 ];
				mpkDestImage->data.pData[ nRealPixelIndex * 4 + 2 ]= kTextureRGBPal._pcTexturePal[ kNeedInsert._c * 3 + 2 ];
				mpkDestImage->data.pData[ nRealPixelIndex * 4 + 3 ]= 255;
			}
			nRealPixelIndex ++;
		}
		delete[] kTextureRGBPal._pcTexturePal;
	}

	///!从R8G8B8读取文件
	void CTextureRLE::LoadTextureR8G8B8( const uchar* pkBuffer )
	{
		for( int i = 0 ; i < mkTextureHeader._sTextureHeight ; i ++ )
		{
			for( int j = 0 ; j < mkTextureHeader._sTextureWidth ; j ++ )
			{
				int nCurrentIndex = i * mkTextureHeader._sTextureWidth + j;
				mpkDestImage->data.pData[ nCurrentIndex * 4 + 0 ]= pkBuffer[ nCurrentIndex * 3 + 0 ];
				mpkDestImage->data.pData[ nCurrentIndex * 4 + 1 ]= pkBuffer[ nCurrentIndex * 3 + 1 ];
				mpkDestImage->data.pData[ nCurrentIndex * 4 + 2 ]= pkBuffer[ nCurrentIndex * 3 + 2 ];	
				mpkDestImage->data.pData[ nCurrentIndex * 4 + 3 ]= 255;
			}
		}
	}
	///!从I8读取文件
	void CTextureRLE::LoadTextureI8( const uchar* pkBuffer )
	{	
		int nColorWidth    = 0;		
		STextureRGBPal  kTextureRGBPal;
		READ_BUFFER( pkBuffer , &kTextureRGBPal._TexturePalLength , sizeof( kTextureRGBPal._TexturePalLength ) );
		kTextureRGBPal._pcTexturePal = RT_NEW uchar[ kTextureRGBPal._TexturePalLength * 3 ];
		READ_BUFFER( pkBuffer , kTextureRGBPal._pcTexturePal , kTextureRGBPal._TexturePalLength * 3 );
		for( int i = 0 ; i < mkTextureHeader._sTextureWidth ; i ++ )
		{
			for( int j = 0 ; j < mkTextureHeader._sTextureHeight ; j ++ )
			{						
				uchar  kPixelIndex = 0;
				READ_BUFFER( pkBuffer , &kPixelIndex ,sizeof( kPixelIndex ) );
				int nCurrentIndex = i * mkTextureHeader._sTextureHeight + j;
				mpkDestImage->data.pData[ nCurrentIndex * 4 + 0 ] = kTextureRGBPal._pcTexturePal[ kPixelIndex * 3 + 0 ];
				mpkDestImage->data.pData[ nCurrentIndex * 4 + 1 ] = kTextureRGBPal._pcTexturePal[ kPixelIndex * 3 + 1 ];
				mpkDestImage->data.pData[ nCurrentIndex * 4 + 2 ] = kTextureRGBPal._pcTexturePal[ kPixelIndex * 3 + 2 ];
				mpkDestImage->data.pData[ nCurrentIndex * 4 + 3 ] = 255;
			}
		}
		delete[] kTextureRGBPal._pcTexturePal;
	}

	///!从A4C12读取文件
	void CTextureRLE::LoadTextureA4C12( const uchar* pkBuffer )
	{
		int nPalAlphaWidth = 0;
		int nColorWidth    = 0;
		STextureAlphaPal  kScrAlphaPal;
		READ_BUFFER( pkBuffer , &kScrAlphaPal._TexturePalLength , sizeof( kScrAlphaPal._TexturePalLength ) );
		kScrAlphaPal._pcTexturePal    = RT_NEW uchar[ kScrAlphaPal._TexturePalLength ];
		READ_BUFFER( pkBuffer , kScrAlphaPal._pcTexturePal , kScrAlphaPal._TexturePalLength );	
		STextureRGBPal  kTextureRGBPal;
		READ_BUFFER( pkBuffer , &kTextureRGBPal._TexturePalLength , sizeof( kTextureRGBPal._TexturePalLength ) );
		kTextureRGBPal._pcTexturePal = RT_NEW uchar[ kTextureRGBPal._TexturePalLength * 3 ];
		READ_BUFFER( pkBuffer , kTextureRGBPal._pcTexturePal , kTextureRGBPal._TexturePalLength * 3 );
		SAlpha4Color12  kNeedInsert;
		for( int i = 0 ; i < mkTextureHeader._sTextureWidth ; i ++ )
		{
			for( int j = 0 ; j < mkTextureHeader._sTextureHeight ; j ++ )
			{						
				READ_BUFFER( pkBuffer , &kNeedInsert ,sizeof( kNeedInsert ) );
				int nCurrentIndex = i * mkTextureHeader._sTextureHeight + j;
				mpkDestImage->data.pData[ nCurrentIndex * 4 + 0 ] = kTextureRGBPal._pcTexturePal[ kNeedInsert.usColor * 3 + 0 ];
				mpkDestImage->data.pData[ nCurrentIndex * 4 + 1 ] = kTextureRGBPal._pcTexturePal[ kNeedInsert.usColor * 3 + 1 ];
				mpkDestImage->data.pData[ nCurrentIndex * 4 + 2 ] = kTextureRGBPal._pcTexturePal[ kNeedInsert.usColor * 3 + 2 ];
				mpkDestImage->data.pData[ nCurrentIndex * 4 + 3 ] = kScrAlphaPal._pcTexturePal  [ kNeedInsert.ucAlpha ];
			}
		}
		delete[] kScrAlphaPal._pcTexturePal;
		delete[] kTextureRGBPal._pcTexturePal;
	}

	///!从IA8C8读取文件
	void CTextureRLE::LoadTextureIA8C8( const uchar* pkBuffer )
	{
		int nPalAlphaWidth = 0;
		int nColorWidth    = 0;
		STextureAlphaPal  kScrAlphaPal;
		READ_BUFFER( pkBuffer , &kScrAlphaPal._TexturePalLength , sizeof( kScrAlphaPal._TexturePalLength ) );
		kScrAlphaPal._pcTexturePal    = RT_NEW uchar[ kScrAlphaPal._TexturePalLength ];
		READ_BUFFER( pkBuffer , kScrAlphaPal._pcTexturePal , kScrAlphaPal._TexturePalLength );	
		STextureRGBPal  kTextureRGBPal;
		READ_BUFFER( pkBuffer , &kTextureRGBPal._TexturePalLength , sizeof( kTextureRGBPal._TexturePalLength ) );
		kTextureRGBPal._pcTexturePal = RT_NEW uchar[ kTextureRGBPal._TexturePalLength * 3 ];
		READ_BUFFER( pkBuffer , kTextureRGBPal._pcTexturePal , kTextureRGBPal._TexturePalLength * 3 );
		for( int i = 0 ; i < mkTextureHeader._sTextureWidth ; i ++ )
		{
			for( int j = 0 ; j < mkTextureHeader._sTextureHeight ; j ++ )
			{
				uchar  nColorIndex = 0;
				uchar  nAlphaIndex = 0;
				READ_BUFFER( pkBuffer , &nColorIndex , 1 );
				READ_BUFFER( pkBuffer , &nAlphaIndex , 1 );
				int nCurrentIndex = i * mkTextureHeader._sTextureHeight + j;
				mpkDestImage->data.pData[ nCurrentIndex * 4 + 0 ] = kTextureRGBPal._pcTexturePal[ nColorIndex * 3 + 0 ];
				mpkDestImage->data.pData[ nCurrentIndex * 4 + 1 ] = kTextureRGBPal._pcTexturePal[ nColorIndex * 3 + 1 ];
				mpkDestImage->data.pData[ nCurrentIndex * 4 + 2 ] = kTextureRGBPal._pcTexturePal[ nColorIndex * 3 + 2 ];
				mpkDestImage->data.pData[ nCurrentIndex * 4 + 3 ] = kScrAlphaPal._pcTexturePal  [ nAlphaIndex ];
			}
		}
		delete[] kScrAlphaPal._pcTexturePal;
		delete[] kTextureRGBPal._pcTexturePal;
	}
	///!从A8R8G8B8读取文件
	void CTextureRLE::LoadTextureA8R8G8B8( const uchar* pkBuffer )
	{
		for( int i = 0 ; i < mkTextureHeader._sTextureWidth ; i ++ )
		{
			for( int j = 0 ; j < mkTextureHeader._sTextureHeight ; j ++ )
			{
				int nCurrentIndex = i * mkTextureHeader._sTextureHeight + j;
				mpkDestImage->data.pData[ nCurrentIndex * 4 + 0 ]= pkBuffer[ nCurrentIndex * 4 + 0 ];
				mpkDestImage->data.pData[ nCurrentIndex * 4 + 1 ]= pkBuffer[ nCurrentIndex * 4 + 1 ];
				mpkDestImage->data.pData[ nCurrentIndex * 4 + 2 ]= pkBuffer[ nCurrentIndex * 4 + 2 ];
				mpkDestImage->data.pData[ nCurrentIndex * 4 + 3 ]= pkBuffer[ nCurrentIndex * 4 + 3 ];
			}
		}
	}

	///!把一个RLE文件存储为TGA文件
	void CTextureRLE::SaveRLEToTGA( const char* sTextureName )
	{
#ifdef  CONVERT_IMAGE_TO_RLE
		CxImage* pkImage = RT_NEW CxImage( mkTextureHeader._sTextureWidth ,mkTextureHeader._sTextureHeight , 32 , CXIMAGE_FORMAT_TGA );
		pkImage->AlphaCreate( );
		for( int i = 0 ; i < mkTextureHeader._sTextureWidth && mpkTextureBuffer ; i ++ )
		{
			for( int j = 0 ; j < mkTextureHeader._sTextureHeight && mpkTextureBuffer; j ++ )
			{
				RGBQUAD  kScrPixel;
				int nCurrentIndex     = i * mkTextureHeader._sTextureHeight + j;
				kScrPixel.rgbRed      = mpkTextureBuffer[ nCurrentIndex ]._ucR;
				kScrPixel.rgbGreen    = mpkTextureBuffer[ nCurrentIndex ]._ucG;
				kScrPixel.rgbBlue     = mpkTextureBuffer[ nCurrentIndex ]._ucB;
				kScrPixel.rgbReserved = mpkTextureBuffer[ nCurrentIndex ]._ucA;
				pkImage->SetPixelColor( i , j , kScrPixel ,true );			
			}
		}
		pkImage->Save( sTextureName , CXIMAGE_FORMAT_TGA );
#endif
	}


	bool rtgiLoadImageRLEFile(const char* szFilename, RtgImage* pImage)
	{
		RtArchive* pArc = RtCoreFile().CreateFileReader(szFilename, 0);
		if (pArc==NULL)
		{
			RtgImage::m_eLastError = RtgImage::ERI_FILENOTFOUND;
			return false;
		}
		bool bResult = rtgiLoadImageRLE(pArc, pImage);
		RtCoreFile().CloseFile(pArc);
		return bResult;
	}
	bool rtgiLoadImageRLE ( RtArchive* pArc, RtgImage* pImage,bool bflip/*=false*/ )
	{		
		int nTextureLength = 0;
		pArc->Serialize( &nTextureLength ,sizeof( int ) );
		CTextureRLEWrapper::ResetReservBuffer();
		uchar* pkBuffer   = CTextureRLEWrapper::ms_pcCompressBuffer;										
		ulong nUnCompLen  = CTextureRLEWrapper::E_RESERV_LENGTH_EX;
		pArc->Serialize( pkBuffer ,nTextureLength );

		CTextureRLE  kTempRLE( pImage );
		bool bResult = false;
		if( nTextureLength > 0 )
		{
            bResult = kTempRLE.LoadRLEFromMemoryCompress( pkBuffer , nTextureLength ); 
		}else if( nTextureLength == 0 )
		{
            bResult = kTempRLE.LoadRLEFromMemoryUncompress( pkBuffer );
		}
		
		
		return bResult;
	}
}