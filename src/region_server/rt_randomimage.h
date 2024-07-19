
#ifndef _RTG_RANDOM_IMAGE_H_
#define _RTG_RANDOM_IMAGE_H_

class RtRandomImage
{
public:
    RtRandomImage();
    virtual ~RtRandomImage();

    // 初始化，参数分别为：TGA格式的字母表，字符个数，字符位置数组，字符宽度数组，字符数组，可用标记
    bool Init(const char* szTgaAlphabet, int iCharCnt, int* iCharPos, int* iCharWidth, char* szCharList, int* iVaildFlag=NULL);

    // 创建图像，根据szStr里面的内容创建一个图像，该图像的大小为(iWidth,iHeight)
    //     返回的是该图像的颜色留，左上角为第一个像素，按行扫描，每一个像素有RGB三个值组成
    //     如果szStr中有不认识的字符，将被跳过
    //     如果生成的图像大小比iWidth,iHeight大，超过的部分将被忽略
    unsigned char* CreateImage24b(const char* szStr, int iWidth, int iHeight, int* pDataSize=NULL);
    unsigned long* CreateImage32b(const char* szStr, int iWidth, int iHeight, int* pDataSize=NULL);

    unsigned long* CreateRandomImage32b(const char* szStr, int iWidth, int iHeight, int* pDataSize=NULL, int iTwistLev=2);

    // 添加杂质
    unsigned long* AddNoise32b(unsigned long* pData, int iWidth, int iHeight, unsigned long dwColorKey, unsigned long dwColorText, unsigned long dwColorBG);

    // 创建一个BMP格式内存的图像
    unsigned char* CreateImageBMP(const char* szStr, int iWidth, int iHeight, int* pDataSize=NULL);

    // 创建一个BMP格式的图像，并保存到指定文件中
    bool CreateImageBMPToFile(const char* szFilename, const char* szStr, int iWidth, int iHeight);

    int GetTotalWidth()         { return m_iAlphabetImgWidth; }
    int GetCharHeight()         { return m_iAlphabetImgHeight; }
    const char* GetCharTable()  { return m_szCharList; }

public:
    bool RandomString(char* szStrBuf, int iStrLen);

    static int RLE32bData(unsigned long* pRawData, int iRawDataSize, unsigned long dwColorKey, unsigned char* pRLEData, int iBufSize);
    static bool DeRLE32bData(unsigned char* pRLEData, int iRLEDataSize, unsigned long dwBGColor, unsigned long dwTextColor, unsigned long* pRawData, int iRawDataSize);

private:
    void ClearBackground(unsigned long dwColor);
    void DrawTextToImg(int x, int y, char c, unsigned long dwColor);
    unsigned long* _CreateImage(const char* szStr, int iWidth, int iHeight, int* pDataSize, bool b32b);

private:
    unsigned int        m_dwBackgroundColor;
    unsigned int        m_dwTextColor;

    int                 m_iCharCnt;     // 字符的个数
    char*               m_szCharList;   // 字符列表
    int*                m_iCharPos;     // 字符的开始位置
    int*                m_iCharWidth;   // 字符的宽度
    int                 m_pMap[256];

    unsigned char*      m_pAlphabet;    // 字符表(只有ARGB数据的一段内存)
    int                 m_iAlphabetImgWidth;
    int                 m_iAlphabetImgHeight;

    bool                m_b32bit;
    unsigned char*      m_pTempBmp;
    unsigned char*      m_pTempData;
    int                 m_iTempWidth;
    int                 m_iTempHeight;

    int                 m_iCreateWidth;
    int                 m_iCreateHeight;
};


#endif // _RTG_RANDOM_IMAGE_H_
/*----------------------------------------------------------------------------
-   The End.
----------------------------------------------------------------------------*/
