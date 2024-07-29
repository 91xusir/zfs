#if !defined(_GIF89A_)
#define _GIF89A_

#include <fstream>
#include <windows.h>
//using namespace std;

typedef struct
{	
	BOOL active;
	UINT disposalMethod;
	BOOL userInputFlag;
	BOOL trsFlag;
	WORD delayTime;
	UINT trsColorIndex;
}GCTRLEXT;

typedef struct 
{	
	WORD imageLPos;
	WORD imageTPos;
	WORD imageWidth;
	WORD imageHeight;
	BOOL lFlag;
	BOOL interlaceFlag;
	BOOL sortFlag;
	UINT lSize;
	BYTE *pColorTable;
	BYTE *dataBuf;
	GCTRLEXT ctrlExt;
}FRAME;
typedef FRAME *LPFRAME;
typedef FRAME *LPCFRAME;

typedef struct 
{	
	UINT frames;
	WORD scrWidth,scrHeight;
	BOOL gFlag;
	UINT colorRes;
	BOOL gSort;
	UINT gSize;
	UINT BKColorIdx;
	UINT pixelAspectRatio;
	BYTE *gColorTable;
}GLOBAL_INFO;
typedef GLOBAL_INFO *LPGLOBAL_INFO;
typedef const GLOBAL_INFO *LPCGLOBAL_INFO;

typedef struct
{	UINT len;
unsigned char* p;
}STRING_TABLE_ENTRY;

struct ColorRGB
{
	ColorRGB(BYTE _r = 0, BYTE _g = 0, BYTE _b = 0)
		: r(_r), g(_g), b(_b)
	{
	}

	bool operator == (const ColorRGB& c) const
	{
		return r == c.r && g == c.g && b == c.b;
	}

	BYTE r;
	BYTE g;
	BYTE b;
};

struct ColorRGBA
{
	ColorRGBA(BYTE _a = 255, BYTE _r = 0, BYTE _g = 0, BYTE _b = 0)
		: a(_a), r(_r), g(_g), b(_b)
	{
	}

	ColorRGBA& operator = (const ColorRGBA& _argb)
	{
		a = _argb.a;
		r = _argb.r;
		g = _argb.g;
		b = _argb.b;
		return *this;
	}

	ColorRGBA& operator = (const ColorRGB& _rgb)
	{
		a = 255;
		r = _rgb.r;
		g = _rgb.g;
		b = _rgb.b;
		return *this;
	}

	bool operator == (const ColorRGBA& c) const
	{
		return a == c.a && r == c.r && g == c.g && b == c.b;
	}

	bool operator == (const ColorRGB& c) const
	{
		return r == c.r && g == c.g && b == c.b;
	}

	BYTE b;
	BYTE g;
	BYTE r;
	BYTE a;
};

class CGif89a
{
private:
	//char version[4];
	CM_MEMDEF(version, 4)
	BOOL error;
	BOOL opened;
	BOOL inMem;
	BYTE gColorTable[256*3];
	BYTE lColorTable[256*3];
	//CM_MEMDEF(gColorTable, 256*3)
	//CM_MEMDEF(lColorTable, 256*3)
	streampos dataStart;
	FRAME *allFrames;
	UINT curIndex;
private:
	UINT checkFrames();
	BOOL getAllFrames();
	BOOL extractData(FRAME* f);
	BOOL initStrTable(STRING_TABLE_ENTRY* strTable,UINT rootSize);
	BOOL addStrTable(STRING_TABLE_ENTRY* strTable,UINT addIdx,UINT idx,unsigned char c);
private:
	GLOBAL_INFO gInfo;
	FRAME curFrame;
	GCTRLEXT ctrlExt;

	char*	m_pBuf;		//文件内容
	int		m_nBufSize;	//文件大小
	int		m_nCurPos;	//当前位移
public:
	CGif89a();
	CGif89a(LPCSTR fileName,BOOL inMem);
	CGif89a(const CGif89a &object);
	~CGif89a();
	BOOL operator!();
	BOOL open(LPCSTR fileName,BOOL inMem);
	void close();
	LPCFRAME getNextFrame();
	LPCGLOBAL_INFO getGlobalInfo();
};

#endif
