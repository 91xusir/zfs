
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

	///!图片的格式
	enum
	{
		///!----------没有Alpha通道---------------
		///!索引模式	
		ETEX_I8     = 0,
		ETEX_I10,
		///!非索引模式，属于图片本身像素决定
		ETEX_R6G5B5,
		ETEX_R5G6B5,
		ETEX_R5G5B6,
		///!非索引模式，属于图片本身像素比较决定
		ETEX_R8G4B4,
		ETEX_R4G8B4,
		ETEX_R4G4B8,	
		///!----------有Alpha通道---------------
		///!索引模式
		ETEX_ALPHA_IA4C4,
		ETEX_ALPHA_IA4C12,
		ETEX_ALPHA_IA2C14,
		ETEX_ALPHA_IA8C8,
		///!完整的模式
		ETEX_A8R8G8B8,
		ETEX_R8G8B8,
		///!不认识的格式
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
	///!调色板的颜色格式
	enum
	{
		///!没有Alpha的格式
		ETEXPAL_R8G8B8,
	};

#pragma pack( push, 1 )
	///!图片的文件头
	struct STextureHeader
	{
		///!图片的格式
		uchar   _cTextureType  ; 
		///!图片的版本
		uchar   _TextureVersion;
		///!文件的宽
		ushort  _sTextureWidth ;
		///!文件的高
		ushort  _sTextureHeight;	
	};
	///!Alpha调色板的文件头
	struct STextureAlphaPal
	{
		///!调色板的长度
		uchar  _TexturePalLength;	
		///!调色板的数据
		uchar*  _pcTexturePal;
	};
	///!RGB调色板的文件头
	struct STextureRGBPal
	{
		///!调色板的长度
		ushort  _TexturePalLength;	
		///!调色板的数据
		uchar*  _pcTexturePal;
	};

	///!Alpha 4 位 Color 12 位的结构
	struct  SAlpha4Color12
	{
		///!Alpha
		ushort ucAlpha  : 4;
		///!Diffuse
		ushort usColor  : 12;
	};

	///!Alpha 4 位 Color 12 位的结构
	struct  SAlpha2Color14
	{
		///!Alpha
		ushort ucAlpha  : 2;
		///!Diffuse
		ushort usColor  : 14;
	};
	///!12位 index 的结构
	struct SIndex10
	{
		uint _a : 10;
		uint _b : 10;
		uint _c : 12;
	};
	///!4Alpha和4Color的结构
	struct SIndex44
	{
		uchar  _a : 4;
		uchar  _b : 4;
	};
	#pragma pack( pop )
}


#endif
