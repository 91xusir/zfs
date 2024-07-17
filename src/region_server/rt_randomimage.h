
#ifndef _RTG_RANDOM_IMAGE_H_
#define _RTG_RANDOM_IMAGE_H_

class RtRandomImage
{
public:
    RtRandomImage();
    virtual ~RtRandomImage();

    // ��ʼ���������ֱ�Ϊ��TGA��ʽ����ĸ���ַ��������ַ�λ�����飬�ַ�������飬�ַ����飬���ñ��
    bool Init(const char* szTgaAlphabet, int iCharCnt, int* iCharPos, int* iCharWidth, char* szCharList, int* iVaildFlag=NULL);

    // ����ͼ�񣬸���szStr��������ݴ���һ��ͼ�񣬸�ͼ��Ĵ�СΪ(iWidth,iHeight)
    //     ���ص��Ǹ�ͼ�����ɫ�������Ͻ�Ϊ��һ�����أ�����ɨ�裬ÿһ��������RGB����ֵ���
    //     ���szStr���в���ʶ���ַ�����������
    //     ������ɵ�ͼ���С��iWidth,iHeight�󣬳����Ĳ��ֽ�������
    unsigned char* CreateImage24b(const char* szStr, int iWidth, int iHeight, int* pDataSize=NULL);
    unsigned long* CreateImage32b(const char* szStr, int iWidth, int iHeight, int* pDataSize=NULL);

    unsigned long* CreateRandomImage32b(const char* szStr, int iWidth, int iHeight, int* pDataSize=NULL, int iTwistLev=2);

    // �������
    unsigned long* AddNoise32b(unsigned long* pData, int iWidth, int iHeight, unsigned long dwColorKey, unsigned long dwColorText, unsigned long dwColorBG);

    // ����һ��BMP��ʽ�ڴ��ͼ��
    unsigned char* CreateImageBMP(const char* szStr, int iWidth, int iHeight, int* pDataSize=NULL);

    // ����һ��BMP��ʽ��ͼ�񣬲����浽ָ���ļ���
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

    int                 m_iCharCnt;     // �ַ��ĸ���
    char*               m_szCharList;   // �ַ��б�
    int*                m_iCharPos;     // �ַ��Ŀ�ʼλ��
    int*                m_iCharWidth;   // �ַ��Ŀ��
    int                 m_pMap[256];

    unsigned char*      m_pAlphabet;    // �ַ���(ֻ��ARGB���ݵ�һ���ڴ�)
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
