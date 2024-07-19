

#ifndef  __RtChaCompress_H__
#define  __RtChaCompress_H__


static const RtgVertex4  gsVertex4Table[] =
{
	RtgVertex4( 0.0f,       0.0f,       0.0f,       1.0f),
	RtgVertex4( 1.0f,       1.0f,       1.0f,       1.0f),
	RtgVertex4( 0.75294125f,0.75294125f,0.75294125f,1.0f),
};


struct SShortVec3
{
	short x;
	short y;
	short z;	
	SShortVec3():x(0),y(0),z(0)                            {}
	SShortVec3( float a , float b, float c ):x(a),y(b),z(c){}
	~SShortVec3(){}
	friend RtArchive& operator<<(RtArchive &ar, SShortVec3 &s) 
	{
		ar << s.x << s.y << s.z;
		return ar;
	}
};
struct SShortBBox
{
	SShortVec3  kCenter;
	SShortVec3  kExtend;
	friend RtArchive& operator<<(RtArchive &ar, SShortBBox &s) 
	{
		ar << s.kCenter << s.kExtend ;
		return ar;
	}
};

struct SMatrixTable
{
	unsigned char uc0 : 4;
	unsigned char uc1 : 4;
	unsigned char uc2 : 4;
	unsigned char uc3 : 4;
};
struct  SShortMatrix12
{
    SShortVec3    sMatrixRotate[ 3 ];
	SShortVec3    sMatrixTraslate;
	SMatrixTable  cTableIdx;

	friend RtArchive& operator<<(RtArchive& ar, SShortMatrix12& s) 
	{
		ar.Serialize( &s.cTableIdx , sizeof( short ) );
		ar << s.sMatrixRotate[ 0 ] << s.sMatrixRotate[ 1 ] << s.sMatrixRotate[ 2 ];
		ar << s.sMatrixTraslate;
		return ar;
	}
};

struct SMtlStandardKeyTable
{
	unsigned char cAmb  : 4;
	unsigned char cDif  : 4;
	unsigned char cEmb  : 4;
	unsigned char cSpe  : 4;
	unsigned char cEnv  : 4;
	unsigned char cRot0 : 4;
	unsigned char cTra0 : 4;
	unsigned char cSca0 : 4;
	unsigned char cRot1 : 4;
	unsigned char cTra1 : 4;
	unsigned char cSca1 : 4;
	unsigned char cRot2 : 4;
	unsigned char cTra2 : 4;
	unsigned char cSca2 : 4;	
};
struct SFrameBufferOptionTable
{
	unsigned char bTwoSize  : 1;
	unsigned char bFog      : 1;
	unsigned char bZTest    : 1;
	unsigned char bZWrite   : 1;
	unsigned char bShadow   : 1;
	unsigned char nAlphaMode: 3;	
};

///!------------------------------------------------------------------------------------------
inline unsigned char GetColorTableIndex( RtgVertex4& kInsert )
{
	for( int i = 0 ; i < sizeof( gsVertex4Table ) / sizeof( gsVertex4Table[ 0 ] ) ; i ++ )
	{
		if( gsVertex4Table[ i ] == kInsert )
		{
			return i;
		}
	}
	return 0xFF;
}

inline RtgVertex4 GetTableColorByIndex( unsigned char ucIdx )
{
    if( ucIdx != 0xFF )
	{
		return gsVertex4Table[ ucIdx ];
	}
	return RtgVertex4( 0.0f,0.0f,0.0f,0.0f);
}
inline RtgVertex3 GetTableColorByIndex_3( unsigned char ucIdx )
{
	if( ucIdx != 0xFF )
	{
		return RtgVertex3( gsVertex4Table[ ucIdx ].x,gsVertex4Table[ ucIdx ].y,gsVertex4Table[ ucIdx ].z);
	}
	return RtgVertex3( 0.0f,0.0f,0.0f );
}


inline unsigned char GetColorTableIndex( RtgVertex3& kInsert )
{
	for( int i = 0 ; i < sizeof( gsVertex4Table ) / sizeof( gsVertex4Table[ 0 ] ) ; i ++ )
	{
		if( gsVertex4Table[ i ].x == kInsert.x &&
			gsVertex4Table[ i ].y == kInsert.y &&
			gsVertex4Table[ i ].z == kInsert.z )
		{
			return i;
		}
	}
	return 0xFF;
}
inline RtgVertex4  LongToVector4_Color( long nColor )
{
	RtgVertex4 kVector(0.0f,0.0f,0.0f,0.0f);
	unsigned char R = ( nColor & 0x00ff0000 ) >> 16;
	unsigned char G = ( nColor & 0x0000ff00 ) >> 8;
	unsigned char B = ( nColor & 0x000000ff ) >> 0;
	unsigned char A = ( nColor & 0xff000000 ) >> 24;
	kVector.x = R / 255.0f;
	kVector.y = G / 255.0f;
	kVector.z = B / 255.0f;
	kVector.w = A / 255.0f;
	return kVector;
}
inline RtgVertex3  LongToVector3_Color( long nColor )
{
	RtgVertex3 kVector(0.0f,0.0f,0.0f);
	unsigned char R = ( nColor & 0x00ff0000 ) >> 16;
	unsigned char G = ( nColor & 0x0000ff00 ) >> 8;
	unsigned char B = ( nColor & 0x000000ff ) >> 0;
	unsigned char A = ( nColor & 0xff000000 ) >> 24;
	kVector.x = R / 255.0f;
	kVector.y = G / 255.0f;
	kVector.z = B / 255.0f;	
	return kVector;
}

inline long Vector4ToLong_Color( RtgVertex4 kColor )
{
	unsigned char R = kColor.x * 255;
	unsigned char G = kColor.y * 255;
	unsigned char B = kColor.z * 255;
	unsigned char A = kColor.w * 255;
	return  ( A << 24 ) | ( R << 16 ) | ( G << 8 ) | ( B );
}
inline long Vector3ToLong_Color( RtgVertex3 kColor )
{
	unsigned char R = kColor.x * 255;
	unsigned char G = kColor.y * 255;
	unsigned char B = kColor.z * 255;
	unsigned char A = 0;
	return  ( A << 24 ) | ( R << 16 ) | ( G << 8 ) | ( B );
}

inline SShortVec3  Vector3ToShort3_Vector( RtgVertex3 kInVec )
{
	SShortVec3  kReturn;
	kReturn.x = kInVec.x ;
	kReturn.y = kInVec.y ;
	kReturn.z = kInVec.z ;	
	return kReturn;
}

inline RtgVertex3  Vector3ToShort3_Vector_Inverse( SShortVec3 kInVec )
{
	RtgVertex3  kReturn;
	kReturn.x = kInVec.x ;
	kReturn.y = kInVec.y ;
	kReturn.z = kInVec.z ;	
	return kReturn;
}

inline SShortVec3  Vector3ToShort3_Vector_10( RtgVertex3 kInVec )
{
	SShortVec3  kReturn;
	kReturn.x = kInVec.x * 10;
	kReturn.y = kInVec.y * 10;
	kReturn.z = kInVec.z * 10;

	if( abs(kInVec.x) >= 3000 || abs(kInVec.y) >= 3000 || abs(kInVec.z) >= 3000 )
	{
		if( kInVec.x >= 3000 ) 	kInVec.x = 3000;
		if( kInVec.y >= 3000 ) 	kInVec.y = 3000;
		if( kInVec.z >= 3000 ) 	kInVec.z = 3000;
		if( kInVec.x <= -3000 ) kInVec.x = -3000;
		if( kInVec.y <= -3000 ) kInVec.y = -3000;
		if( kInVec.z <= -3000 ) kInVec.z = -3000;
		kReturn.x = kInVec.x * 10;
		kReturn.y = kInVec.y * 10;
		kReturn.z = kInVec.z * 10;
	}
	return kReturn;
}
inline RtgVertex3  Vector3ToShort3_Vector_10_Inverse( SShortVec3 kInVec )
{
	RtgVertex3  kReturn;
	kReturn.x = kInVec.x * 0.1f;
	kReturn.y = kInVec.y * 0.1f;
	kReturn.z = kInVec.z * 0.1f;	
	return kReturn;
}

inline SShortVec3  Vector3ToShort3_Vector_50( RtgVertex3 kInVec )
{
	SShortVec3  kReturn;
	kReturn.x = kInVec.x * 50;
	kReturn.y = kInVec.y * 50;
	kReturn.z = kInVec.z * 50;

	if( abs(kInVec.x) >= 650 || abs(kInVec.y) >= 650 || abs(kInVec.z) >= 650 )
	{
		if( kInVec.x >= 650 ) 	kInVec.x = 650;
		if( kInVec.y >= 650 ) 	kInVec.y = 650;
		if( kInVec.z >= 650 ) 	kInVec.z = 650;
		if( kInVec.x <= -650 ) kInVec.x = -650;
		if( kInVec.y <= -650 ) kInVec.y = -650;
		if( kInVec.z <= -650 ) kInVec.z = -650;
		kReturn.x = kInVec.x * 50;
		kReturn.y = kInVec.y * 50;
		kReturn.z = kInVec.z * 50;
	}
	return kReturn;
}

inline RtgVertex3  Vector3ToShort3_Vector_50_Inverse( SShortVec3 kInVec )
{
	RtgVertex3  kReturn;
	kReturn.x = kInVec.x * 0.02f;
	kReturn.y = kInVec.y * 0.02f;
	kReturn.z = kInVec.z * 0.02f;
	return kReturn;
}

inline SShortVec3  Vector3ToShort3_Vector_100( RtgVertex3 kInVec )
{
	SShortVec3  kReturnVect;
	kReturnVect.x = kInVec.x * 100;
	kReturnVect.y = kInVec.y * 100;
	kReturnVect.z = kInVec.z * 100;	
	if( abs(kInVec.x) > 320 || abs(kInVec.y) > 320 || abs(kInVec.z) > 320 )
	{
		if( kInVec.x >= 320 ) 	kInVec.x = 320;
		if( kInVec.y >= 320 ) 	kInVec.y = 320;
		if( kInVec.z >= 320 ) 	kInVec.z = 320;
		if( kInVec.x <= -320 )  kInVec.x = -320;
		if( kInVec.y <= -320 )  kInVec.y = -320;
		if( kInVec.z <= -320 )  kInVec.z = -320;
		kReturnVect.x = kInVec.x * 100;
		kReturnVect.y = kInVec.y * 100;
		kReturnVect.z = kInVec.z * 100;
	}
	return SShortVec3(kReturnVect.x,kReturnVect.y,kReturnVect.z);
}

inline RtgVertex3  Vector3ToShort3_Vector_100_Inverse( SShortVec3 kInVec )
{
	RtgVertex3  kReturn;
	kReturn.x = kInVec.x * 0.01f;
	kReturn.y = kInVec.y * 0.01f;
	kReturn.z = kInVec.z * 0.01f;
	return kReturn;
}
inline SShortVec3  Vector3ToShort3_Vector_1000( RtgVertex3 kInVec )
{
	SShortVec3  kReturn;
	kReturn.x = kInVec.x * 1000;
	kReturn.y = kInVec.y * 1000;
	kReturn.z = kInVec.z * 1000;

	if( abs(kInVec.x) >= 32 || abs(kInVec.y) >= 32 || abs(kInVec.z) >= 32 )
	{
		if( kInVec.x >= 32 ) 	kInVec.x = 32;
		if( kInVec.y >= 32 ) 	kInVec.y = 32;
		if( kInVec.z >= 32 ) 	kInVec.z = 32;
		if( kInVec.x <= -32 ) kInVec.x = -32;
		if( kInVec.y <= -32 ) kInVec.y = -32;
		if( kInVec.z <= -32 ) kInVec.z = -32;
		kReturn.x = kInVec.x * 1000;
		kReturn.y = kInVec.y * 1000;
		kReturn.z = kInVec.z * 1000;
	}
	return kReturn;
}

inline RtgVertex3  Vector3ToShort3_Vector_1000_Inverse( SShortVec3 kInVec )
{
	RtgVertex3  kReturn;
	kReturn.x = kInVec.x * 0.001f;
	kReturn.y = kInVec.y * 0.001f;
	kReturn.z = kInVec.z * 0.001f;	
	return kReturn;
}

inline SShortVec3  Vector3ToShort3_Vector_500( RtgVertex3 kInVec )
{
	SShortVec3  kReturn;
	kReturn.x = kInVec.x * 500;
	kReturn.y = kInVec.y * 500;
	kReturn.z = kInVec.z * 500;

	if( abs(kInVec.x) >= 65 || abs(kInVec.y) >= 65 || abs(kInVec.z) >= 65 )
	{
		if( kInVec.x >= 65 )  kInVec.x = 65;
		if( kInVec.y >= 65 )  kInVec.y = 65;
		if( kInVec.z >= 65 )  kInVec.z = 65;
		if( kInVec.x <= -65 ) kInVec.x = -65;
		if( kInVec.y <= -65 ) kInVec.y = -65;
		if( kInVec.z <= -65 ) kInVec.z = -65;
		kReturn.x = kInVec.x * 500;
		kReturn.y = kInVec.y * 500;
		kReturn.z = kInVec.z * 500;
	}
	return kReturn;
}

inline RtgVertex3  Vector3ToShort3_Vector_500_Inverse( SShortVec3 kInVec )
{
	RtgVertex3  kReturn;
	kReturn.x = kInVec.x * 0.002f;
	kReturn.y = kInVec.y * 0.002f;
	kReturn.z = kInVec.z * 0.002f;
	return kReturn;
}

const static char*  gsAlphaModel[] =
{
	"None",
	"Add",
	"Sub",
	"Modulate",
	"Blend"
};

inline unsigned char GetAlphaModeTableIndex( const char* pcAlphaModel )
{
	for( int i =0 ; i < sizeof( gsAlphaModel ) / sizeof( gsAlphaModel[ 0 ] ) ; i ++ )
	{
		if( strcmp( gsAlphaModel[ i ] , pcAlphaModel ) )
		{
			return i;
		}
	}
	return 0xFF;
}
const static char* gsEffectUVTable[] =
{
	"Base",
	"XY",
	"YZ",
	"ZX",
	"Camera Pos",
	"Camera Nor",
	"Camera Rel"
};
inline unsigned char GetEffectUVTableIndex( const char* pcEffectModel )
{
	for( int i =0 ; i < sizeof( gsEffectUVTable ) / sizeof( gsEffectUVTable[ 0 ] ) ; i ++ )
	{
		if( strcmp( gsEffectUVTable[ i ] , pcEffectModel ) )
		{
			return i;
		}
	}
	return 0xFF;
}

inline void ConvertRtgAABBToShortAABB( RtgAABB& kInScr , SShortBBox& kOutDest )
{
    kOutDest.kCenter = Vector3ToShort3_Vector_10( kInScr.vPos );
	kOutDest.kExtend = Vector3ToShort3_Vector_10( kInScr.vExt );
}

inline void ConvertShortAABBToRtgAABB( SShortBBox& kInScr , RtgAABB& kOutDest )
{
	kOutDest.vPos = Vector3ToShort3_Vector_10_Inverse( kInScr.kCenter );
	kOutDest.vExt = Vector3ToShort3_Vector_10_Inverse( kInScr.kExtend );
}

inline unsigned char SwitchMatrixConvert( RtgVertex3& kInsert , SShortVec3& kOutDest )
{
	unsigned char ucTaye = 0;
    if( abs(kInsert.x) > 3000 || abs(kInsert.y) > 3000 || abs(kInsert.z) > 3000 )
	{
        kOutDest = Vector3ToShort3_Vector( kInsert );
		ucTaye   = 0;
		return ucTaye;
	}
	if( abs(kInsert.x) > 650 || abs(kInsert.y) > 650 || abs(kInsert.z) > 650 )
	{
		kOutDest = Vector3ToShort3_Vector_10( kInsert );
		ucTaye   = 1;
		return ucTaye;
	}
	if( abs(kInsert.x) > 320 || abs(kInsert.y) > 320 || abs(kInsert.z) > 320 )
	{
		kOutDest = Vector3ToShort3_Vector_50( kInsert );
		ucTaye   = 2;
		return ucTaye;
	}
	if( abs(kInsert.x) > 65 || abs(kInsert.y) > 65 || abs(kInsert.z) > 65 )
	{
		kOutDest = Vector3ToShort3_Vector_100( kInsert );
		ucTaye   = 3;
		return ucTaye;
	}
	if( abs(kInsert.x) > 32 || abs(kInsert.y) > 32 || abs(kInsert.z) > 32 )
	{
		kOutDest = Vector3ToShort3_Vector_500( kInsert );
		ucTaye   = 4;
		return ucTaye;
	}	
	kOutDest = Vector3ToShort3_Vector_1000( kInsert );
	ucTaye   = 5;
	return ucTaye;
}

inline void SwitchMatrixConvert_Inverse( SShortVec3& kInsert , RtgVertex3& kOutDest , unsigned char ucTaye )
{
	if( ucTaye == 0 )
	{
		kOutDest = Vector3ToShort3_Vector_Inverse( kInsert );		
		return;
	}
	if( ucTaye == 1 )
	{
		kOutDest = Vector3ToShort3_Vector_10_Inverse( kInsert );				
		return;
	}
	if( ucTaye == 2 )
	{
		kOutDest = Vector3ToShort3_Vector_50_Inverse( kInsert );				
		return;
	}
	if( ucTaye == 3 )
	{
		kOutDest = Vector3ToShort3_Vector_100_Inverse( kInsert );		
		return;
	}
	if( ucTaye == 4 )
	{
		kOutDest = Vector3ToShort3_Vector_500_Inverse( kInsert );				
		return;
	}	
	kOutDest = Vector3ToShort3_Vector_1000_Inverse( kInsert );	
}

inline void ConvertRtgMatrixToShortMatrix( RtgMatrix12& kInMatrix12 , SShortMatrix12& kOutDest )
{
	kOutDest.cTableIdx.uc0 = SwitchMatrixConvert( RtgVertex3( kInMatrix12.m_m[0] , kInMatrix12.m_m[1] ,  kInMatrix12.m_m[2]  ) , kOutDest.sMatrixRotate[ 0 ] );
	kOutDest.cTableIdx.uc1 = SwitchMatrixConvert( RtgVertex3( kInMatrix12.m_m[3] , kInMatrix12.m_m[4] ,  kInMatrix12.m_m[5]  ) , kOutDest.sMatrixRotate[ 1 ] );
	kOutDest.cTableIdx.uc2 = SwitchMatrixConvert( RtgVertex3( kInMatrix12.m_m[6] , kInMatrix12.m_m[7] ,  kInMatrix12.m_m[8]  ) , kOutDest.sMatrixRotate[ 2 ] );
	kOutDest.cTableIdx.uc3 = SwitchMatrixConvert( RtgVertex3( kInMatrix12.m_m[9] , kInMatrix12.m_m[10] , kInMatrix12.m_m[11] ) , kOutDest.sMatrixTraslate    );
}

inline void ConvertShortMatrixToRtgMatrix( SShortMatrix12& kInScr , RtgMatrix12& kOutDest )
{
	RtgVertex3  kMatrixVector[ 4 ];
	SwitchMatrixConvert_Inverse( kInScr.sMatrixRotate[0] , kMatrixVector[ 0 ] , kInScr.cTableIdx.uc0 );
	SwitchMatrixConvert_Inverse( kInScr.sMatrixRotate[1] , kMatrixVector[ 1 ] , kInScr.cTableIdx.uc1 );
	SwitchMatrixConvert_Inverse( kInScr.sMatrixRotate[2] , kMatrixVector[ 2 ] , kInScr.cTableIdx.uc2 );
	SwitchMatrixConvert_Inverse( kInScr.sMatrixTraslate  , kMatrixVector[ 3 ] , kInScr.cTableIdx.uc3 );

	kOutDest.m_m[0]=kMatrixVector[0].x;
	kOutDest.m_m[1]=kMatrixVector[0].y;
	kOutDest.m_m[2]=kMatrixVector[0].z;

	kOutDest.m_m[3]=kMatrixVector[1].x;
	kOutDest.m_m[4]=kMatrixVector[1].y;
	kOutDest.m_m[5]=kMatrixVector[1].z;

	kOutDest.m_m[6]=kMatrixVector[2].x;
	kOutDest.m_m[7]=kMatrixVector[2].y;
	kOutDest.m_m[8]=kMatrixVector[2].z;

	kOutDest.m_m[9] =kMatrixVector[3].x;
	kOutDest.m_m[10]=kMatrixVector[3].y;
	kOutDest.m_m[11]=kMatrixVector[3].z;
}






#endif