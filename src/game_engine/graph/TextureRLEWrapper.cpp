
#include "rtg_graph_inter.h"
#include "TextureRLEWrapper.h"
#include "TextureRLE.h"
#include "core/zlib.h"


namespace rt_graph 
{

	uchar*  CTextureRLEWrapper::ms_pcReservBuffer   = NULL;
	uchar*  CTextureRLEWrapper::ms_pcCompressBuffer = NULL;
    #define  SAVE_BUFFER( pPointer , pScrBuffer , nSize , nOffset )  { memcpy( pPointer + nOffset , pScrBuffer , nSize ) ; nOffset += nSize ; }
	

	void  CTextureRLEWrapper::InitTextureRLEWrapper( )
	{
		if( ms_pcReservBuffer == NULL )
		{
			ms_pcReservBuffer = RT_NEW uchar[ E_RESERV_LENGTH ];
			memset( ms_pcReservBuffer , NULL ,E_RESERV_LENGTH );
		}
		if( NULL == ms_pcCompressBuffer )
		{
			ms_pcCompressBuffer = RT_NEW uchar[ E_RESERV_LENGTH_EX ];
			memset( ms_pcCompressBuffer , NULL ,E_RESERV_LENGTH_EX );
		}
	}
	void  CTextureRLEWrapper::DestroyTextureRLEWrapper( )
	{
		if( ms_pcReservBuffer   ){delete[] ms_pcReservBuffer  ;}ms_pcReservBuffer = NULL;
		if( ms_pcCompressBuffer ){delete[] ms_pcCompressBuffer;}ms_pcCompressBuffer = NULL;
	}
	void  CTextureRLEWrapper::ResetReservBuffer( )
	{
		if( ms_pcReservBuffer )
		{
			memset( ms_pcReservBuffer ,   NULL ,E_RESERV_LENGTH );
			memset( ms_pcCompressBuffer , NULL ,E_RESERV_LENGTH_EX );
		}
	}


	inline void ResetSamePixel( SamePixelArray& kSamePixel )
	{
		kSamePixel.clear();
	}
	inline void ResetPixelDiff( STextPixelDiff& kPixelDiff )
	{
		kPixelDiff.nRedMin   = 255;
		kPixelDiff.nRedMax   = 0;
		kPixelDiff.nGreenMin = 255;
		kPixelDiff.nGreenMax = 0;
		kPixelDiff.nBlueMin  = 255;
		kPixelDiff.nBlueMax  = 0;
		kPixelDiff.nAlphaMin = 255;
		kPixelDiff.nAlphaMax = 0;
	}

	inline void DeterminPixel( RGBQUAD& kScr , STextPixelDiff& kDest )
	{
		if( kDest.nRedMin   > kScr.rgbRed       ) kDest.nRedMin    = kScr.rgbRed;
		if( kDest.nGreenMin > kScr.rgbGreen     ) kDest.nGreenMin  = kScr.rgbGreen;
		if( kDest.nBlueMin  > kScr.rgbBlue      ) kDest.nBlueMin   = kScr.rgbBlue;
		if( kDest.nAlphaMin > kScr.rgbReserved  ) kDest.nAlphaMin  = kScr.rgbReserved;	
		if( kDest.nRedMax   < kScr.rgbRed       ) kDest.nRedMax    = kScr.rgbRed;
		if( kDest.nGreenMax < kScr.rgbGreen     ) kDest.nGreenMax  = kScr.rgbGreen;
		if( kDest.nBlueMax  < kScr.rgbBlue      ) kDest.nBlueMax   = kScr.rgbBlue;
		if( kDest.nAlphaMax < kScr.rgbReserved  ) kDest.nAlphaMax  = kScr.rgbReserved;	
	}

	inline uint PushToSamePixel( RGBQUAD& kScr , SamePixelArray& kSamePixel , int x , int y )
	{
		for( uint i = 0 ; i < kSamePixel.size() ; i ++ )
		{
			SamePixel&  kSame = kSamePixel[ i ];
			if( abs(kSame._kPixel._uRed   - kScr.rgbRed  ) <= 1 &&
				abs(kSame._kPixel._uGreen - kScr.rgbGreen) <= 1 &&			
				abs(kSame._kPixel._uBlue  - kScr.rgbBlue ) <= 1)
			{
				SPoint kPoint;kPoint._sX = x;kPoint._sY=y;
				kSame._kPointArray.push_back( kPoint );
				return i;
			}
		}
		SamePixel  kInsert;
		kInsert._kPixel._uRed   = kScr.rgbRed;
		kInsert._kPixel._uGreen = kScr.rgbGreen;
		kInsert._kPixel._uBlue  = kScr.rgbBlue;
		SPoint kPoint;kPoint._sX = x;kPoint._sY=y;
		kInsert._kPointArray.push_back( kPoint );
		kSamePixel.push_back( kInsert );
		return (uint)(kSamePixel.size() - 1);
	}


	inline uint PushToSameAlpha( RGBQUAD& kScr , SameAlphaArray& kSameAlpha , int x , int y )
	{
		for( uint i = 0 ; i < kSameAlpha.size() ; i ++ )
		{
			SameAlpha&  kSame = kSameAlpha[ i ];
			if( kSame._uAlpha == kScr.rgbReserved )
			{
				SPoint kPoint;kPoint._sX = x;kPoint._sY=y;
				kSame._kPointArray.push_back( kPoint );
				return i;
			}
		}
		SameAlpha  kInsert;
		kInsert._uAlpha = kScr.rgbReserved;	
		SPoint kPoint;kPoint._sX = x;kPoint._sY=y;
		kInsert._kPointArray.push_back( kPoint );
		kSameAlpha.push_back( kInsert );
		return (uint)(kSameAlpha.size() - 1);
	}
	///!·ÖÎöÎÄ¼þ
	void  CTextureRLEWrapper::AnalyTexture( RtgImage*          pkScrTexture,
											SamePixelArray&    kSamePixel ,
											SameAlphaArray&    kSameAlpha , 
											STextPixelDiff&    kPixelDiff ,
											TextureIndexArray& kIndexArray )
	{
		ResetSamePixel( kSamePixel );    
		ResetPixelDiff( kPixelDiff );	
		if( NULL == pkScrTexture ) return;

		int  nWidth    = pkScrTexture->info.iWidth;
		int  nHeight   = pkScrTexture->info.iHeight;
		bool bHasAlpha = pkScrTexture->info.bAlpha;
		int  nIndex    = 0;
		RGBQUAD        kColor ;
		STextureIndex  kInsert ;
		for( int i = 0 ; i < nHeight ; i ++ )
		{
			for( int j = 0 ; j < nWidth ; j ++ )
			{
				nIndex             = i * nWidth + j;				
				kColor.rgbRed      = pkScrTexture->data.pData[ nIndex * 4 + 0 ];	
				kColor.rgbGreen    = pkScrTexture->data.pData[ nIndex * 4 + 1 ];
				kColor.rgbBlue     = pkScrTexture->data.pData[ nIndex * 4 + 2 ];
				kColor.rgbReserved = pkScrTexture->data.pData[ nIndex * 4 + 3 ];
				DeterminPixel  ( kColor , kPixelDiff );
				int nDiffuseIndex     = PushToSamePixel( kColor , kSamePixel , i , j );
				int nAlphaIndex       = PushToSameAlpha( kColor , kSameAlpha , i , j );
				kInsert.nDiffuseIndex = nDiffuseIndex;
				kInsert.nAlphaIndex   = nAlphaIndex;
				kIndexArray.push_back( kInsert );
			}
		}
	}

	///!Ö°ÄÜ¼ì²âÍ¼ÏñÎÄ¼þ²ÉÓÃÊ²Ã´·½Ê½Ñ¹Ëõ	
	int CTextureRLEWrapper::AnalyResultToCompress( bool            bHasAlpha ,
		                                           int             nOriginalSize ,
												   SamePixelArray& kSamePixel , 
												   SameAlphaArray& kSameAlpha , 
												   STextPixelDiff& kPixelDiff )
	{
		if( bHasAlpha )
		{
			uint nSamePixelNumber = (uint)kSamePixel.size();
			uint nSameAlphaNumber = (uint)kSameAlpha.size();

			if( nSamePixelNumber > nOriginalSize * 0.7f )
			{
				return ETEX_A8R8G8B8;
			}
			if( nSamePixelNumber < E_BIT_4 && nSameAlphaNumber < E_BIT_4 )
			{
				return ETEX_ALPHA_IA4C4;
			}
			if( nSamePixelNumber < E_BIT_12 && nSameAlphaNumber < E_BIT_4 )
			{
				return ETEX_ALPHA_IA4C12;
			}		
			if( nSamePixelNumber < E_BIT_8 && nSameAlphaNumber < E_BIT_8 )
			{
				return ETEX_ALPHA_IA8C8;
			}
			if( nSamePixelNumber < E_BIT_14 && nSameAlphaNumber < E_BIT_2 )
			{
				return ETEX_ALPHA_IA2C14;
			}
			return ETEX_A8R8G8B8;
		}
		else
		{
			uint nSamePixelNumber = (uint)kSamePixel.size();
			if( nSamePixelNumber > nOriginalSize * 0.7f )
			{
				return ETEX_R8G8B8;
			}
			if( nSamePixelNumber < E_BIT_8  )
			{
				return ETEX_I8;
			}
			if( nSamePixelNumber < E_BIT_10  )
			{
				return ETEX_I10;
			}
			return ETEX_R8G8B8;
		}
		return ETEX_UNKNOWN;
	}

	void  CTextureRLEWrapper::CompressTextureToRLE( RtgImage* pkScrImage , const char* pcTextureName )
	{	
		if( NULL == pkScrImage ) { return ; }
		SamePixelArray     akSamePixel;
		TextureIndexArray  akIndexArray;
		SameAlphaArray     akSameAlpha;
		STextPixelDiff     kTexPixelDiff;
		bool bHasAlpha  = pkScrImage->info.bAlpha;
		AnalyTexture( pkScrImage , akSamePixel , akSameAlpha ,kTexPixelDiff ,akIndexArray);
		int nResult = AnalyResultToCompress( bHasAlpha , pkScrImage->info.iHeight * pkScrImage->info.iWidth, akSamePixel , akSameAlpha ,kTexPixelDiff );
		int nTextureLength = 0;
		switch( nResult )
		{
		case ETEX_ALPHA_IA4C4:
			{
				SaveTexture_I4C4( ms_pcReservBuffer,nTextureLength ,pkScrImage,akSamePixel,akSameAlpha , akIndexArray );
			}
			break;
		case ETEX_ALPHA_IA4C12:
			{
				SaveTexture_A4C12( ms_pcReservBuffer,nTextureLength ,pkScrImage,akSamePixel,akSameAlpha , akIndexArray );
			}
			break;	
		case ETEX_ALPHA_IA2C14:
			{
				SaveTexture_A2C14( ms_pcReservBuffer,nTextureLength ,pkScrImage,akSamePixel,akSameAlpha , akIndexArray );
			}
			break;
		case ETEX_I8:
			{
				SaveTexture_I8( ms_pcReservBuffer,nTextureLength ,pkScrImage,akSamePixel,akSameAlpha , akIndexArray );
			}
			break;
		case ETEX_I10:
			{
				SaveTexture_I10( ms_pcReservBuffer,nTextureLength ,pkScrImage,akSamePixel,akSameAlpha , akIndexArray );
			}
			break;
		case ETEX_ALPHA_IA8C8:
			{
				SaveTexture_IA8C8( ms_pcReservBuffer,nTextureLength ,pkScrImage,akSamePixel,akSameAlpha , akIndexArray );
			}
			break;
		case ETEX_A8R8G8B8:
			{
				SaveTexture_A8R8G8B8( ms_pcReservBuffer , nTextureLength , pkScrImage );
			}
			break;
		case ETEX_R8G8B8:
			{
				SaveTexture_R8G8B8( ms_pcReservBuffer , nTextureLength , pkScrImage );
			}
			break;
		}
		SaveRLEBuffe( ms_pcReservBuffer , nTextureLength ,  pcTextureName );	
		return ;
	}
	void  CTextureRLEWrapper::SaveRLEBuffe( const uchar* pkBuffer, int nTextureLength , const char* pcTextureName )
	{
		String  sTextureName = pcTextureName;	
		sTextureName        += CTextureRLE::ms_szRLEPostFix;
		FILE* pkFile         = fopen( sTextureName.data() , "wb+" );
		unsigned long nCompressedLength = nTextureLength;
		int nError = compress( (Bytef*)ms_pcCompressBuffer, &nCompressedLength, (const Bytef*)pkBuffer, nTextureLength );	
		if( 0 == nError && pkFile )
		{
			fwrite( &nCompressedLength  , 4 , 1 , pkFile );
			fwrite( ms_pcCompressBuffer , nCompressedLength , 1 , pkFile );		
			fclose( pkFile );
		}
		///!Ñ¹ËõÊ§°Ü
		if( 0 != nError && pkFile )
		{
			int nTextureCompress = 0;
			fwrite( &nTextureCompress  , 4 , 1 , pkFile );
			fwrite( pkBuffer , nTextureLength , 1 , pkFile );		
			fclose( pkFile );
		}
	}
	///!ÌîÐ´Texture
	void  CTextureRLEWrapper::SaveTexture_I4C4( uchar*&            pkBuffer,
		int&               nTextureLength , 
		RtgImage*           pkScr,
		SamePixelArray&    kSamePixel , 
		SameAlphaArray&    kSameAlpha ,
		TextureIndexArray& kIndexArray  )
	{
		nTextureLength = 0;
		if( NULL == pkBuffer ) return;
		STextureHeader  kHeader;	
		kHeader._cTextureType   = ETEX_ALPHA_IA4C4;
		kHeader._sTextureWidth  = (ushort)pkScr->info.iWidth;
		kHeader._sTextureHeight = (ushort)pkScr->info.iHeight;
		kHeader._TextureVersion = TEXTURE_CURRENT_VERSION;
		SAVE_BUFFER( pkBuffer , &kHeader ,sizeof( kHeader ) , nTextureLength );
		uint nSameAlphaNumber = (uint)kSameAlpha.size();
		uint nSamePixelNumber = (uint)kSamePixel.size();
		STextureAlphaPal  kTextureAlphaPal;
		kTextureAlphaPal._TexturePalLength = nSameAlphaNumber;
		kTextureAlphaPal._pcTexturePal     = RT_NEW uchar[ kTextureAlphaPal._TexturePalLength ];
		for( int i = 0 ; i < kTextureAlphaPal._TexturePalLength ; i ++ )
		{
			kTextureAlphaPal._pcTexturePal[ i ] = kSameAlpha[ i ]._uAlpha;
		}
		STextureRGBPal  kTextureRGBPal;
		kTextureRGBPal._TexturePalLength = nSamePixelNumber;
		kTextureRGBPal._pcTexturePal     = RT_NEW uchar[ kTextureRGBPal._TexturePalLength * 3 ];
		for( int i = 0 ; i < kTextureRGBPal._TexturePalLength ; i ++ )
		{
			kTextureRGBPal._pcTexturePal[ i * 3 + 0 ] = kSamePixel[ i ]._kPixel._uRed;
			kTextureRGBPal._pcTexturePal[ i * 3 + 1 ] = kSamePixel[ i ]._kPixel._uGreen;
			kTextureRGBPal._pcTexturePal[ i * 3 + 2 ] = kSamePixel[ i ]._kPixel._uBlue;
		}
		SAVE_BUFFER( pkBuffer , &kTextureAlphaPal._TexturePalLength ,sizeof( kTextureAlphaPal._TexturePalLength ) , nTextureLength );
		for( int i = 0 ; i < kTextureAlphaPal._TexturePalLength ; i ++ )
		{
			SAVE_BUFFER( pkBuffer , &kTextureAlphaPal._pcTexturePal[ i ] ,sizeof( kTextureAlphaPal._pcTexturePal[ i ] ) , nTextureLength );
		}
		SAVE_BUFFER( pkBuffer , &kTextureRGBPal._TexturePalLength ,sizeof( kTextureRGBPal._TexturePalLength ) , nTextureLength );
		for( int i = 0 ; i < kTextureRGBPal._TexturePalLength ; i ++ )
		{
			SAVE_BUFFER( pkBuffer , &kTextureRGBPal._pcTexturePal[ i * 3 + 0 ] ,sizeof( kTextureRGBPal._pcTexturePal[ i * 3 + 0 ] ) , nTextureLength );
			SAVE_BUFFER( pkBuffer , &kTextureRGBPal._pcTexturePal[ i * 3 + 1 ] ,sizeof( kTextureRGBPal._pcTexturePal[ i * 3 + 1 ] ) , nTextureLength );
			SAVE_BUFFER( pkBuffer , &kTextureRGBPal._pcTexturePal[ i * 3 + 2 ] ,sizeof( kTextureRGBPal._pcTexturePal[ i * 3 + 2 ] ) , nTextureLength );
		}

		for( int i = 0 ; i < (int)kIndexArray.size() ; i ++ )
		{
			SIndex44  KInsert;
			KInsert._a = kIndexArray[ i ].nDiffuseIndex;
			KInsert._b = kIndexArray[ i ].nAlphaIndex;		
			SAVE_BUFFER( pkBuffer , &KInsert ,1 , nTextureLength );		
		}	
	}

	///!ÌîÐ´Texture
	void  CTextureRLEWrapper::SaveTexture_IA8C8( uchar*&            pkBuffer,
		int&               nTextureLength , 
		RtgImage*           pkScr,
		SamePixelArray&    kSamePixel , 
		SameAlphaArray&    kSameAlpha ,
		TextureIndexArray& kIndexArray  )
	{
		nTextureLength = 0;
		if( NULL == pkBuffer ) return;
		STextureHeader  kHeader;		
		kHeader._cTextureType   = ETEX_ALPHA_IA8C8;
		kHeader._sTextureWidth  = pkScr->info.iWidth;
		kHeader._sTextureHeight = pkScr->info.iHeight;
		kHeader._TextureVersion = TEXTURE_CURRENT_VERSION;
		SAVE_BUFFER( pkBuffer , &kHeader ,sizeof( kHeader ) , nTextureLength );
		int nSameAlphaNumber = (uint)kSameAlpha.size();
		int nSamePixelNumber = (uint)kSamePixel.size();
		STextureAlphaPal  kTextureAlphaPal;
		kTextureAlphaPal._TexturePalLength = nSameAlphaNumber;
		kTextureAlphaPal._pcTexturePal     = RT_NEW uchar[ kTextureAlphaPal._TexturePalLength ];
		for( int i = 0 ; i < kTextureAlphaPal._TexturePalLength ; i ++ )
		{
			kTextureAlphaPal._pcTexturePal[ i ] = kSameAlpha[ i ]._uAlpha;
		}
		STextureRGBPal  kTextureRGBPal;
		kTextureRGBPal._TexturePalLength = nSamePixelNumber;
		kTextureRGBPal._pcTexturePal     = RT_NEW uchar[ kTextureRGBPal._TexturePalLength * 3 ];
		for( int i = 0 ; i < kTextureRGBPal._TexturePalLength ; i ++ )
		{
			kTextureRGBPal._pcTexturePal[ i * 3 + 0 ] = kSamePixel[ i ]._kPixel._uRed;
			kTextureRGBPal._pcTexturePal[ i * 3 + 1 ] = kSamePixel[ i ]._kPixel._uGreen;
			kTextureRGBPal._pcTexturePal[ i * 3 + 2 ] = kSamePixel[ i ]._kPixel._uBlue;
		}
		SAVE_BUFFER( pkBuffer , &kTextureAlphaPal._TexturePalLength ,sizeof( kTextureAlphaPal._TexturePalLength ) , nTextureLength );
		for( int i = 0 ; i < kTextureAlphaPal._TexturePalLength ; i ++ )
		{
			SAVE_BUFFER( pkBuffer , &kTextureAlphaPal._pcTexturePal[ i ] ,sizeof( kTextureAlphaPal._pcTexturePal[ i ] ) , nTextureLength );
		}
		SAVE_BUFFER( pkBuffer , &kTextureRGBPal._TexturePalLength ,sizeof( kTextureRGBPal._TexturePalLength ) , nTextureLength );
		for( int i = 0 ; i < kTextureRGBPal._TexturePalLength ; i ++ )
		{
			SAVE_BUFFER( pkBuffer , &kTextureRGBPal._pcTexturePal[ i * 3 + 0 ] ,sizeof( kTextureRGBPal._pcTexturePal[ i * 3 + 0 ] ) , nTextureLength );
			SAVE_BUFFER( pkBuffer , &kTextureRGBPal._pcTexturePal[ i * 3 + 1 ] ,sizeof( kTextureRGBPal._pcTexturePal[ i * 3 + 1 ] ) , nTextureLength );
			SAVE_BUFFER( pkBuffer , &kTextureRGBPal._pcTexturePal[ i * 3 + 2 ] ,sizeof( kTextureRGBPal._pcTexturePal[ i * 3 + 2 ] ) , nTextureLength );
		}

		for( int i = 0 ; i < (int)kIndexArray.size() ; i ++ )
		{
			uchar  cColorIndex    = kIndexArray[ i ].nDiffuseIndex;
			uchar  cAlphaIndex    = kIndexArray[ i ].nAlphaIndex;		
			SAVE_BUFFER( pkBuffer , &cColorIndex ,1 , nTextureLength );
			SAVE_BUFFER( pkBuffer , &cAlphaIndex ,1 , nTextureLength );
		}	
	}

	///!ÌîÐ´Texture
	void  CTextureRLEWrapper::SaveTexture_I10( uchar*&            pkBuffer,
		int&               nTextureLength , 
		RtgImage*           pkScr,
		SamePixelArray&    kSamePixel , 
		SameAlphaArray&    kSameAlpha ,
		TextureIndexArray& kIndexArray  )
	{
		nTextureLength = 0;
		if( NULL == pkBuffer ) return;
		STextureHeader  kHeader;		
		kHeader._cTextureType   = ETEX_I10;
		kHeader._sTextureWidth  = pkScr->info.iWidth;
		kHeader._sTextureHeight = pkScr->info.iHeight;
		kHeader._TextureVersion = TEXTURE_CURRENT_VERSION;
		SAVE_BUFFER( pkBuffer , &kHeader ,sizeof( kHeader ) , nTextureLength );
		int nSamePixelNumber = (uint)kSamePixel.size();	
		STextureRGBPal  kTextureRGBPal;
		kTextureRGBPal._TexturePalLength = nSamePixelNumber;
		kTextureRGBPal._pcTexturePal     = RT_NEW uchar[ kTextureRGBPal._TexturePalLength * 3 ];
		for( int i = 0 ; i < kTextureRGBPal._TexturePalLength ; i ++ )
		{
			kTextureRGBPal._pcTexturePal[ i * 3 + 0 ] = kSamePixel[ i ]._kPixel._uRed;
			kTextureRGBPal._pcTexturePal[ i * 3 + 1 ] = kSamePixel[ i ]._kPixel._uGreen;
			kTextureRGBPal._pcTexturePal[ i * 3 + 2 ] = kSamePixel[ i ]._kPixel._uBlue;
		}
		SAVE_BUFFER( pkBuffer , &kTextureRGBPal._TexturePalLength ,sizeof( kTextureRGBPal._TexturePalLength ) , nTextureLength );
		for( int i = 0 ; i < kTextureRGBPal._TexturePalLength ; i ++ )
		{
			SAVE_BUFFER( pkBuffer , &kTextureRGBPal._pcTexturePal[ i * 3 + 0 ] ,sizeof( kTextureRGBPal._pcTexturePal[ i * 3 + 0 ] ) , nTextureLength );
			SAVE_BUFFER( pkBuffer , &kTextureRGBPal._pcTexturePal[ i * 3 + 1 ] ,sizeof( kTextureRGBPal._pcTexturePal[ i * 3 + 1 ] ) , nTextureLength );
			SAVE_BUFFER( pkBuffer , &kTextureRGBPal._pcTexturePal[ i * 3 + 2 ] ,sizeof( kTextureRGBPal._pcTexturePal[ i * 3 + 2 ] ) , nTextureLength );
		}
		int nTotalIndex  = (uint)kIndexArray.size();	
		int nRealIndex   = nTotalIndex / 3 + 1;
		int nInsertSize  = sizeof( SIndex10 );
		int bCurrentTotal= 0;
		for( int i = 0 ; i < nRealIndex ; i ++ )
		{
			SIndex10  kNeedInsert;
			if( bCurrentTotal < nTotalIndex )
			{
				kNeedInsert._a = kIndexArray[ bCurrentTotal ].nDiffuseIndex;
			}
			bCurrentTotal ++;		
			if( bCurrentTotal < nTotalIndex )
			{
				kNeedInsert._b = kIndexArray[ bCurrentTotal ].nDiffuseIndex;
			}
			bCurrentTotal ++;
			if( bCurrentTotal < nTotalIndex )
			{
				kNeedInsert._c = kIndexArray[ bCurrentTotal ].nDiffuseIndex;
			}
			bCurrentTotal ++;
			SAVE_BUFFER( pkBuffer , &kNeedInsert ,nInsertSize , nTextureLength );
		}
	}

	///!ÌîÐ´Texture
	void  CTextureRLEWrapper::SaveTexture_R8G8B8( uchar*& pkBuffer, int& nTextureLength , RtgImage* pkScrTexture )
	{
		nTextureLength = 0;
		if( NULL == pkBuffer ) return;
		STextureHeader  kHeader;		
		kHeader._cTextureType   = ETEX_R8G8B8;
		kHeader._sTextureWidth  = pkScrTexture->info.iWidth;
		kHeader._sTextureHeight = pkScrTexture->info.iHeight;
		kHeader._TextureVersion = TEXTURE_CURRENT_VERSION;
		SAVE_BUFFER( pkBuffer , &kHeader ,sizeof( kHeader ) , nTextureLength );
		int      nIndex = 0;
		RGBQUAD  kColor;
		for( int i = 0 ; i < kHeader._sTextureHeight ; i ++ )
		{
			for( int j = 0 ; j < kHeader._sTextureWidth ; j ++ )
			{
				nIndex             = i * kHeader._sTextureWidth + j;				
				kColor.rgbRed      = pkScrTexture->data.pData[ nIndex * 4 + 0 ];	
				kColor.rgbGreen    = pkScrTexture->data.pData[ nIndex * 4 + 1 ];
				kColor.rgbBlue     = pkScrTexture->data.pData[ nIndex * 4 + 2 ];
				kColor.rgbReserved = pkScrTexture->data.pData[ nIndex * 4 + 3 ];

				SAVE_BUFFER( pkBuffer , &kColor.rgbRed ,     1 , nTextureLength );
				SAVE_BUFFER( pkBuffer , &kColor.rgbGreen ,   1 , nTextureLength );
				SAVE_BUFFER( pkBuffer , &kColor.rgbBlue ,    1 , nTextureLength );			
			}
		}	
	}

	///!ÌîÐ´Texture
	void  CTextureRLEWrapper::SaveTexture_A8R8G8B8( uchar*& pkBuffer, int& nTextureLength , RtgImage* pkScrTexture )
	{
		nTextureLength = 0;
		if( NULL == pkBuffer ) return;
		STextureHeader  kHeader;		
		kHeader._cTextureType   = ETEX_A8R8G8B8;
		kHeader._sTextureWidth  = pkScrTexture->info.iWidth;
		kHeader._sTextureHeight = pkScrTexture->info.iHeight;
		kHeader._TextureVersion = TEXTURE_CURRENT_VERSION;
		SAVE_BUFFER( pkBuffer , &kHeader ,sizeof( kHeader ) , nTextureLength );

		int      nIndex = 0;
		RGBQUAD  kColor;

		for( int i = 0 ; i < kHeader._sTextureHeight ; i ++ )
		{
			for( int j = 0 ; j < kHeader._sTextureWidth ; j ++ )
			{
				
				nIndex             = i * kHeader._sTextureWidth + j;				
				kColor.rgbRed      = pkScrTexture->data.pData[ nIndex * 4 + 0 ];	
				kColor.rgbGreen    = pkScrTexture->data.pData[ nIndex * 4 + 1 ];
				kColor.rgbBlue     = pkScrTexture->data.pData[ nIndex * 4 + 2 ];
				kColor.rgbReserved = pkScrTexture->data.pData[ nIndex * 4 + 3 ];

				SAVE_BUFFER( pkBuffer , &kColor.rgbRed ,     1 , nTextureLength );
				SAVE_BUFFER( pkBuffer , &kColor.rgbGreen ,   1 , nTextureLength );
				SAVE_BUFFER( pkBuffer , &kColor.rgbBlue ,    1 , nTextureLength );
				SAVE_BUFFER( pkBuffer , &kColor.rgbReserved ,1 , nTextureLength );
			}
		}
	}
	///!ÌîÐ´Texture
	void  CTextureRLEWrapper::SaveTexture_A4C12( uchar*& pkBuffer, int& nTextureLength , RtgImage* pkScr, SamePixelArray& kSamePixel ,  SameAlphaArray& kSameAlpha , TextureIndexArray& kIndexArray )
	{
		nTextureLength = 0;
		if( NULL == pkBuffer ) return;
		STextureHeader  kHeader;		
		kHeader._cTextureType   = ETEX_ALPHA_IA4C12;
		kHeader._sTextureWidth  = pkScr->info.iWidth;
		kHeader._sTextureHeight = pkScr->info.iHeight;
		kHeader._TextureVersion = TEXTURE_CURRENT_VERSION;
		SAVE_BUFFER( pkBuffer , &kHeader ,sizeof( kHeader ) , nTextureLength );
		int nSameAlphaNumber = (uint)kSameAlpha.size();
		int nSamePixelNumber = (uint)kSamePixel.size();
		STextureAlphaPal  kTextureAlphaPal;
		kTextureAlphaPal._TexturePalLength = nSameAlphaNumber;
		kTextureAlphaPal._pcTexturePal     = RT_NEW uchar[ kTextureAlphaPal._TexturePalLength ];
		for( int i = 0 ; i < kTextureAlphaPal._TexturePalLength ; i ++ )
		{
			kTextureAlphaPal._pcTexturePal[ i ] = kSameAlpha[ i ]._uAlpha;
		}
		STextureRGBPal  kTextureRGBPal;
		kTextureRGBPal._TexturePalLength = nSamePixelNumber;
		kTextureRGBPal._pcTexturePal     = RT_NEW uchar[ kTextureRGBPal._TexturePalLength * 3 ];
		for( int i = 0 ; i < kTextureRGBPal._TexturePalLength ; i ++ )
		{
			kTextureRGBPal._pcTexturePal[ i * 3 + 0 ] = kSamePixel[ i ]._kPixel._uRed;
			kTextureRGBPal._pcTexturePal[ i * 3 + 1 ] = kSamePixel[ i ]._kPixel._uGreen;
			kTextureRGBPal._pcTexturePal[ i * 3 + 2 ] = kSamePixel[ i ]._kPixel._uBlue;
		}
		SAVE_BUFFER( pkBuffer , &kTextureAlphaPal._TexturePalLength ,sizeof( kTextureAlphaPal._TexturePalLength ) , nTextureLength );
		for( int i = 0 ; i < kTextureAlphaPal._TexturePalLength ; i ++ )
		{
			SAVE_BUFFER( pkBuffer , &kTextureAlphaPal._pcTexturePal[ i ] ,sizeof( kTextureAlphaPal._pcTexturePal[ i ] ) , nTextureLength );
		}
		SAVE_BUFFER( pkBuffer , &kTextureRGBPal._TexturePalLength ,sizeof( kTextureRGBPal._TexturePalLength ) , nTextureLength );
		for( int i = 0 ; i < kTextureRGBPal._TexturePalLength ; i ++ )
		{
			SAVE_BUFFER( pkBuffer , &kTextureRGBPal._pcTexturePal[ i * 3 + 0 ] ,sizeof( kTextureRGBPal._pcTexturePal[ i * 3 + 0 ] ) , nTextureLength );
			SAVE_BUFFER( pkBuffer , &kTextureRGBPal._pcTexturePal[ i * 3 + 1 ] ,sizeof( kTextureRGBPal._pcTexturePal[ i * 3 + 1 ] ) , nTextureLength );
			SAVE_BUFFER( pkBuffer , &kTextureRGBPal._pcTexturePal[ i * 3 + 2 ] ,sizeof( kTextureRGBPal._pcTexturePal[ i * 3 + 2 ] ) , nTextureLength );
		}
		for( int i = 0 ; i < (int)kIndexArray.size() ; i ++ )
		{
			SAlpha4Color12  kNeedInsert;
			kNeedInsert.ucAlpha  = kIndexArray[ i ].nAlphaIndex;
			kNeedInsert.usColor  = kIndexArray[ i ].nDiffuseIndex;
			int  nInsertSize     = sizeof( kNeedInsert );
			SAVE_BUFFER( pkBuffer , &kNeedInsert ,nInsertSize , nTextureLength );
		}	
	}

	///!ÌîÐ´Texture
	void  CTextureRLEWrapper::SaveTexture_A2C14( uchar*& pkBuffer, int& nTextureLength , RtgImage* pkScr, SamePixelArray& kSamePixel ,  SameAlphaArray& kSameAlpha , TextureIndexArray& kIndexArray )
	{
		nTextureLength = 0;
		if( NULL == pkBuffer ) return;
		STextureHeader  kHeader;		
		kHeader._cTextureType   = ETEX_ALPHA_IA2C14;
		kHeader._sTextureWidth  = pkScr->info.iWidth;
		kHeader._sTextureHeight = pkScr->info.iHeight;
		kHeader._TextureVersion = TEXTURE_CURRENT_VERSION;
		SAVE_BUFFER( pkBuffer , &kHeader ,sizeof( kHeader ) , nTextureLength );
		int nSameAlphaNumber = (uint)kSameAlpha.size();
		int nSamePixelNumber = (uint)kSamePixel.size();
		STextureAlphaPal  kTextureAlphaPal;
		kTextureAlphaPal._TexturePalLength = nSameAlphaNumber;
		kTextureAlphaPal._pcTexturePal     = RT_NEW uchar[ kTextureAlphaPal._TexturePalLength ];
		for( int i = 0 ; i < kTextureAlphaPal._TexturePalLength ; i ++ )
		{
			kTextureAlphaPal._pcTexturePal[ i ] = kSameAlpha[ i ]._uAlpha;
		}
		STextureRGBPal  kTextureRGBPal;
		kTextureRGBPal._TexturePalLength = nSamePixelNumber;
		kTextureRGBPal._pcTexturePal     = RT_NEW uchar[ kTextureRGBPal._TexturePalLength * 3 ];
		for( int i = 0 ; i < kTextureRGBPal._TexturePalLength ; i ++ )
		{
			kTextureRGBPal._pcTexturePal[ i * 3 + 0 ] = kSamePixel[ i ]._kPixel._uRed;
			kTextureRGBPal._pcTexturePal[ i * 3 + 1 ] = kSamePixel[ i ]._kPixel._uGreen;
			kTextureRGBPal._pcTexturePal[ i * 3 + 2 ] = kSamePixel[ i ]._kPixel._uBlue;
		}
		SAVE_BUFFER( pkBuffer , &kTextureAlphaPal._TexturePalLength ,sizeof( kTextureAlphaPal._TexturePalLength ) , nTextureLength );
		for( int i = 0 ; i < kTextureAlphaPal._TexturePalLength ; i ++ )
		{
			SAVE_BUFFER( pkBuffer , &kTextureAlphaPal._pcTexturePal[ i ] ,sizeof( kTextureAlphaPal._pcTexturePal[ i ] ) , nTextureLength );
		}
		SAVE_BUFFER( pkBuffer , &kTextureRGBPal._TexturePalLength ,sizeof( kTextureRGBPal._TexturePalLength ) , nTextureLength );
		for( int i = 0 ; i < kTextureRGBPal._TexturePalLength ; i ++ )
		{
			SAVE_BUFFER( pkBuffer , &kTextureRGBPal._pcTexturePal[ i * 3 + 0 ] ,sizeof( kTextureRGBPal._pcTexturePal[ i * 3 + 0 ] ) , nTextureLength );
			SAVE_BUFFER( pkBuffer , &kTextureRGBPal._pcTexturePal[ i * 3 + 1 ] ,sizeof( kTextureRGBPal._pcTexturePal[ i * 3 + 1 ] ) , nTextureLength );
			SAVE_BUFFER( pkBuffer , &kTextureRGBPal._pcTexturePal[ i * 3 + 2 ] ,sizeof( kTextureRGBPal._pcTexturePal[ i * 3 + 2 ] ) , nTextureLength );
		}
		for( int i = 0 ; i < (int)kIndexArray.size() ; i ++ )
		{
			SAlpha2Color14  kNeedInsert;
			kNeedInsert.ucAlpha  = kIndexArray[ i ].nAlphaIndex;
			kNeedInsert.usColor  = kIndexArray[ i ].nDiffuseIndex;
			int  nInsertSize     = sizeof( kNeedInsert );
			SAVE_BUFFER( pkBuffer , &kNeedInsert ,nInsertSize , nTextureLength );
		}	
	}


	///!ÌîÐ´Texture
	void  CTextureRLEWrapper::SaveTexture_I8( uchar*&        pkBuffer, 
		int&            nTextureLength , 
		RtgImage*        pkScr, 
		SamePixelArray& kSamePixel , 
		SameAlphaArray& kSameAlpha ,
		TextureIndexArray& kIndexArray )
	{
		nTextureLength = 0;
		if( NULL == pkBuffer ) return;
		STextureHeader  kHeader;		
		kHeader._cTextureType   = ETEX_I8;
		kHeader._sTextureWidth  = pkScr->info.iWidth;
		kHeader._sTextureHeight = pkScr->info.iHeight;
		kHeader._TextureVersion = TEXTURE_CURRENT_VERSION;
		SAVE_BUFFER( pkBuffer , &kHeader ,sizeof( kHeader ) , nTextureLength );	
		int nSamePixelNumber = (uint)kSamePixel.size();	
		STextureRGBPal  kTextureRGBPal;
		kTextureRGBPal._TexturePalLength = nSamePixelNumber;
		kTextureRGBPal._pcTexturePal     = RT_NEW uchar[ kTextureRGBPal._TexturePalLength * 3 ];
		for( int i = 0 ; i < kTextureRGBPal._TexturePalLength ; i ++ )
		{
			kTextureRGBPal._pcTexturePal[ i * 3 + 0 ] = kSamePixel[ i ]._kPixel._uRed;
			kTextureRGBPal._pcTexturePal[ i * 3 + 1 ] = kSamePixel[ i ]._kPixel._uGreen;
			kTextureRGBPal._pcTexturePal[ i * 3 + 2 ] = kSamePixel[ i ]._kPixel._uBlue;
		}	
		SAVE_BUFFER( pkBuffer , &kTextureRGBPal._TexturePalLength ,sizeof( kTextureRGBPal._TexturePalLength ) , nTextureLength );
		for( int i = 0 ; i < kTextureRGBPal._TexturePalLength ; i ++ )
		{
			SAVE_BUFFER( pkBuffer , &kTextureRGBPal._pcTexturePal[ i * 3 + 0 ] ,sizeof( kTextureRGBPal._pcTexturePal[ i * 3 + 0 ] ) , nTextureLength );
			SAVE_BUFFER( pkBuffer , &kTextureRGBPal._pcTexturePal[ i * 3 + 1 ] ,sizeof( kTextureRGBPal._pcTexturePal[ i * 3 + 1 ] ) , nTextureLength );
			SAVE_BUFFER( pkBuffer , &kTextureRGBPal._pcTexturePal[ i * 3 + 2 ] ,sizeof( kTextureRGBPal._pcTexturePal[ i * 3 + 2 ] ) , nTextureLength );
		}
		for( int i = 0 ; i < (int)kIndexArray.size() ; i ++ )
		{
			uchar cPixIndex      = kIndexArray[ i ].nDiffuseIndex;
			int  nInsertSize     = sizeof( cPixIndex );
			SAVE_BUFFER( pkBuffer , &cPixIndex ,nInsertSize , nTextureLength );
		}	
	}
	
}
