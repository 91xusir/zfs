/**
* @file rtc2_util.h 其他
* @author janhail
*/

namespace rt2_core {

/*-----------------------------------------------------------------------------
-     一些有用的宏
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
-   字符操作函数.
-----------------------------------------------------------------------------*/

inline char rtToUpper( char c )    { return (c<'a' || c>'z') ? (c) : (char)(c+'A'-'a');}
inline char rtToLower( char c )    { return (c<'A' || c>'Z') ? (c) : (char)(c+'a'-'A');}
inline BOOL rtIsAlpha( char c )    { return (c>='a' && c<='z') || (c>='A' && c<='Z');}
inline BOOL rtIsDigit( char c )    { return c>='0' && c<='9';}
inline BOOL rtIsAlnum( char c )    { return (c>='a' && c<='z') || (c>='A' && c<='Z') || (c>='0' && c<='9');}

/*-----------------------------------------------------------------------------
-   字符串操作函数
-----------------------------------------------------------------------------*/

/// 把数字输出成一个字符串
//      注意：字符串是这个函数内部定义的，大小为40个byte，并且是固定的，
//      所以不要在一个函数调用中使用这个函数两次
RT2_CORE_API char* rtFormatNumber(long lNumber);

/// 把数字lNumber用千分符表示，比如 "123,456,789"
//     iBufSize 可以设置为0，就表示不检查Buffer的大小
RT2_CORE_API char* rtEncodeIntegerNumber(char* szBuf, int iBufSize, DWORD dwNumber);

RT2_CORE_API int rtChrInStr(const char* pStr, char cChr);
RT2_CORE_API const char* rtStrRChr(const char* pStr, char cChr);
RT2_CORE_API const char* rtStrRChrs(const char* pStr, const char* pChrs);
RT2_CORE_API void rtTrimSpaces( char* pString);

/*-----------------------------------------------------------------------------
-   内存函数
-----------------------------------------------------------------------------*/

RT2_CORE_API BOOL rtMemIsZero( const void* pV, int iCount );
RT2_CORE_API void rtMemswap( void* pBuf1, void* pBuf2, DWORD iSize );


/*-----------------------------------------------------------------------------
-   数据压缩
-----------------------------------------------------------------------------*/

/** Lzw数据压缩.
*/
class RT2_CORE_API RtZCompress
{
public:
    /** 压缩.
    * @param in     需要压缩的数据
    * @param in_len 数据大小
    * @param out    输出的缓冲区
    * @return 压缩后的大小
    */
    static int Compress(void *in,unsigned int in_len,void *out);

    /** 解压缩.
    * @param in     需要解压缩的数据
    * @param in_len 数据大小
    * @param out    输出的缓冲区
    * @return 解压缩后的大小
    */
    static int Decompression(void *in,unsigned int in_len,void *out);
};

/** ZLib数据压缩.
*/
class RT2_CORE_API RtZLib
{
public:
    /** 压缩.
    * @param in      需要压缩的数据
    * @param in_len  数据大小
    * @param out     输出的缓冲区
    * @param out_len 输出数据的大小
    * @return 是否成功
    */
    static bool Compress(void *in,unsigned int in_len,void *out, unsigned int& out_len);

    /** 解压缩.
    * @param in      需要解压缩的数据
    * @param in_len  数据大小
    * @param out     输出的缓冲区
    * @param out_len 输出数据的大小
    * @return 是否成功
    */
    static bool Decompression(void *in,unsigned int in_len,void *out, unsigned int& out_len);
};

/*-----------------------------------------------------------------------------
-   编码
-----------------------------------------------------------------------------*/

namespace rtc_hashing {

/** MD5编码.
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
    // 打印MD5值成字符串保存在szString中，szString的长度最少为33字节
    void GetMD5String(char *szString);

public:
    bool EncodeFile(char const *filename);     // 生成一个文件的MD5, 返回false为文件打开失败
    void EncodeString(char const *str);        // 生成一个字符串的MD5
    void EncodeData(char const *data, int len); // 生成一段数据的MD5

private:
    RtMD5Context    m_context;
    unsigned char   m_pDigest[16];
};

/** MD5编码成unsigned long.
编码一段数据，返回为unsigned long，是由结果计算出来的(long1^long2^long3^long4)
*/
unsigned long rtMD5_DataShort( const void *data, int length );

/** 编码一段数据, digest最小为char[16].
*/
void rtMD5_Data( const char *data, int len, char *digest );

/** 编码一个string, digest最小为char[16].
*/
void rtMD5_String( const char *str, char *digest );


/** 
*/
bool rtMD5_File(const char *filename, char *digest );
/** CRC8 编码.
*/
unsigned char rtCRC8_Data( const void *data, int length );
void rtCRC8_Init( unsigned char &crcvalue );
void rtCRC8_Update( unsigned char &crcvalue, const void *data, int length );
void rtCRC8_Finish( unsigned char &crcvalue );

/** CRC16 编码.
*/
unsigned short rtCRC16_Data( const void *data, int length );
void rtCRC16_Init( unsigned short &crcvalue );
void rtCRC16_Update( unsigned short &crcvalue, const void *data, int length );
void rtCRC16_Finish( unsigned short &crcvalue );

/** CRC32 编码.
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
