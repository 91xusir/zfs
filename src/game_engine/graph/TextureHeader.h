
#ifndef  __TextureHeader_H__
#define  __TextureHeader_H__

#include <vector>
#include <string>
#include <unordered_map>
typedef  unsigned char        uchar;
typedef  unsigned short       ushort;
typedef  unsigned int         uint;
typedef  unsigned long        ulong;
typedef  std::string          String;
typedef std::vector<String>   StringVector;
#define   TEXTURE_CURRENT_VERSION   0

namespace rt_graph
{

	///!ͼƬ�ĸ�ʽ
	enum
	{
		///!----------û��Alphaͨ��---------------
		///!����ģʽ	
		ETEX_I8     = 0,
		ETEX_I10,
		///!������ģʽ������ͼƬ�������ؾ���
		ETEX_R6G5B5,
		ETEX_R5G6B5,
		ETEX_R5G5B6,
		///!������ģʽ������ͼƬ�������رȽϾ���
		ETEX_R8G4B4,
		ETEX_R4G8B4,
		ETEX_R4G4B8,	
		///!----------��Alphaͨ��---------------
		///!����ģʽ
		ETEX_ALPHA_IA4C4,
		ETEX_ALPHA_IA4C12,
		ETEX_ALPHA_IA2C14,
		ETEX_ALPHA_IA8C8,
		///!������ģʽ
		ETEX_A8R8G8B8,
		ETEX_R8G8B8,
		///!����ʶ�ĸ�ʽ
		ETEX_UNKNOWN,
	};

	enum
	{
		E_BIT_2   = 1 << 2,
		E_BIT_4   = 1 << 4,
		E_BIT_5   = 1 << 5,
		E_BIT_6   = 1 << 6,
		E_BIT_8   = 1 << 8,
		E_BIT_10  = 1 << 10,
		E_BIT_12  = 1 << 12,
		E_BIT_14  = 1 << 14,
	};
	///!��ɫ�����ɫ��ʽ
	enum
	{
		///!û��Alpha�ĸ�ʽ
		ETEXPAL_R8G8B8,
	};

#pragma pack( push, 1 )
	///!ͼƬ���ļ�ͷ
	struct STextureHeader
	{
		///!ͼƬ�ĸ�ʽ
		uchar   _cTextureType  ; 
		///!ͼƬ�İ汾
		uchar   _TextureVersion;
		///!�ļ��Ŀ�
		ushort  _sTextureWidth ;
		///!�ļ��ĸ�
		ushort  _sTextureHeight;	
	};
	///!Alpha��ɫ����ļ�ͷ
	struct STextureAlphaPal
	{
		///!��ɫ��ĳ���
		uchar  _TexturePalLength;	
		///!��ɫ�������
		uchar*  _pcTexturePal;
	};
	///!RGB��ɫ����ļ�ͷ
	struct STextureRGBPal
	{
		///!��ɫ��ĳ���
		ushort  _TexturePalLength;	
		///!��ɫ�������
		uchar*  _pcTexturePal;
	};

	///!Alpha 4 λ Color 12 λ�Ľṹ
	struct  SAlpha4Color12
	{
		///!Alpha
		ushort ucAlpha  : 4;
		///!Diffuse
		ushort usColor  : 12;
	};

	///!Alpha 4 λ Color 12 λ�Ľṹ
	struct  SAlpha2Color14
	{
		///!Alpha
		ushort ucAlpha  : 2;
		///!Diffuse
		ushort usColor  : 14;
	};
	///!12λ index �Ľṹ
	struct SIndex10
	{
		uint _a : 10;
		uint _b : 10;
		uint _c : 12;
	};
	///!4Alpha��4Color�Ľṹ
	struct SIndex44
	{
		uchar  _a : 4;
		uchar  _b : 4;
	};
	#pragma pack( pop )
}


#endif
