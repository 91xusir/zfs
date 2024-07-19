/**
* @file rtc2_util.h ����
* @author janhail
*/

namespace rt2_core {

/*-----------------------------------------------------------------------------
-     һЩ���õĺ�
-----------------------------------------------------------------------------*/

#define     countof(array)                      (sizeof(array)/sizeof(array[0]))
#ifdef WIN32
#define     stlforeach_template(type, iter, container)   type::iterator iter; for (iter = (container).begin(); iter != (container).end(); iter++)
#define     stlforeach(type, iter, container)   type::iterator iter; for (iter = (container).begin(); iter != (container).end(); iter++)
#else
#define     stlforeach_template(type, iter, container)   typename type::iterator iter; for (iter = (container).begin(); iter != (container).end(); iter++)
#define     stlforeach(type, iter, container)  type::iterator iter; for (iter = (container).begin(); iter != (container).end(); iter++)
#endif

/*-----------------------------------------------------------------------------
-   �ַ���������.
-----------------------------------------------------------------------------*/

inline char rtToUpper( char c )    { return (c<'a' || c>'z') ? (c) : (char)(c+'A'-'a');}
inline char rtToLower( char c )    { return (c<'A' || c>'Z') ? (c) : (char)(c+'a'-'A');}
inline BOOL rtIsAlpha( char c )    { return (c>='a' && c<='z') || (c>='A' && c<='Z');}
inline BOOL rtIsDigit( char c )    { return c>='0' && c<='9';}
inline BOOL rtIsAlnum( char c )    { return (c>='a' && c<='z') || (c>='A' && c<='Z') || (c>='0' && c<='9');}

/*-----------------------------------------------------------------------------
-   �ַ�����������
-----------------------------------------------------------------------------*/

/// �����������һ���ַ���
//      ע�⣺�ַ�������������ڲ�����ģ���СΪ40��byte�������ǹ̶��ģ�
//      ���Բ�Ҫ��һ������������ʹ�������������
RT2_CORE_API char* rtFormatNumber(long lNumber);

/// ������lNumber��ǧ�ַ���ʾ������ "123,456,789"
//     iBufSize ��������Ϊ0���ͱ�ʾ�����Buffer�Ĵ�С
RT2_CORE_API char* rtEncodeIntegerNumber(char* szBuf, int iBufSize, DWORD dwNumber);

RT2_CORE_API int rtChrInStr(const char* pStr, char cChr);
RT2_CORE_API const char* rtStrRChr(const char* pStr, char cChr);
RT2_CORE_API const char* rtStrRChrs(const char* pStr, const char* pChrs);
RT2_CORE_API void rtTrimSpaces( char* pString);

/*-----------------------------------------------------------------------------
-   �ڴ溯��
-----------------------------------------------------------------------------*/

RT2_CORE_API BOOL rtMemIsZero( const void* pV, int iCount );
RT2_CORE_API void rtMemswap( void* pBuf1, void* pBuf2, DWORD iSize );


/*-----------------------------------------------------------------------------
-   ����ѹ��
-----------------------------------------------------------------------------*/

/** Lzw����ѹ��.
*/
class RT2_CORE_API RtZCompress
{
public:
    /** ѹ��.
    * @param in     ��Ҫѹ��������
    * @param in_len ���ݴ�С
    * @param out    ����Ļ�����
    * @return ѹ����Ĵ�С
    */
    static int Compress(void *in,unsigned int in_len,void *out);

    /** ��ѹ��.
    * @param in     ��Ҫ��ѹ��������
    * @param in_len ���ݴ�С
    * @param out    ����Ļ�����
    * @return ��ѹ����Ĵ�С
    */
    static int Decompression(void *in,unsigned int in_len,void *out);
};

/** ZLib����ѹ��.
*/
class RT2_CORE_API RtZLib
{
public:
    /** ѹ��.
    * @param in      ��Ҫѹ��������
    * @param in_len  ���ݴ�С
    * @param out     ����Ļ�����
    * @param out_len ������ݵĴ�С
    * @return �Ƿ�ɹ�
    */
    static bool Compress(void *in,unsigned int in_len,void *out, unsigned int& out_len);

    /** ��ѹ��.
    * @param in      ��Ҫ��ѹ��������
    * @param in_len  ���ݴ�С
    * @param out     ����Ļ�����
    * @param out_len ������ݵĴ�С
    * @return �Ƿ�ɹ�
    */
    static bool Decompression(void *in,unsigned int in_len,void *out, unsigned int& out_len);
};

/*-----------------------------------------------------------------------------
-   ����
-----------------------------------------------------------------------------*/

namespace rtc_hashing {

/** MD5����.
*/
class RT2_CORE_API RtMD5
{
public:
    struct RtMD5Context     // MD5 context
    {
        unsigned long int state[4];
        unsigned long int count[2];
        unsigned char     buffer[64];
    };

public:
    RtMD5();
    void Update(unsigned char *pData, unsigned int dwLen);
    void Final();

    unsigned char* GetMD5(unsigned char pDigest[16]);
    // ��ӡMD5ֵ���ַ���������szString�У�szString�ĳ�������Ϊ33�ֽ�
    void GetMD5String(char *szString);

public:
    bool EncodeFile(char const *filename);     // ����һ���ļ���MD5, ����falseΪ�ļ���ʧ��
    void EncodeString(char const *str);        // ����һ���ַ�����MD5
    void EncodeData(char const *data, int len); // ����һ�����ݵ�MD5

private:
    RtMD5Context    m_context;
    unsigned char   m_pDigest[16];
};

/** MD5�����unsigned long.
����һ�����ݣ�����Ϊunsigned long�����ɽ�����������(long1^long2^long3^long4)
*/
unsigned long rtMD5_DataShort( const void *data, int length );

/** ����һ������, digest��СΪchar[16].
*/
void rtMD5_Data( const char *data, int len, char *digest );

/** ����һ��string, digest��СΪchar[16].
*/
void rtMD5_String( const char *str, char *digest );


/** 
*/
bool rtMD5_File(const char *filename, char *digest );
/** CRC8 ����.
*/
unsigned char rtCRC8_Data( const void *data, int length );
void rtCRC8_Init( unsigned char &crcvalue );
void rtCRC8_Update( unsigned char &crcvalue, const void *data, int length );
void rtCRC8_Finish( unsigned char &crcvalue );

/** CRC16 ����.
*/
unsigned short rtCRC16_Data( const void *data, int length );
void rtCRC16_Init( unsigned short &crcvalue );
void rtCRC16_Update( unsigned short &crcvalue, const void *data, int length );
void rtCRC16_Finish( unsigned short &crcvalue );

/** CRC32 ����.
*/
unsigned long rtCRC32_Data( const void *data, int length );
void rtCRC32_Init( unsigned long &crcvalue );
void rtCRC32_Update( unsigned long &crcvalue, const void *data, int length );
void rtCRC32_Finish( unsigned long &crcvalue );

} // namespace rtc_hashing

using namespace rtc_hashing;

} // namespace rt2_core

/*-----------------------------------------------------------------------------
-   The End.
-----------------------------------------------------------------------------*/
