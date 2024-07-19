/**
* @file rtc2_string.h 字符串操作
* @author janhail
*/

namespace rt2_core {

// 安全的取得字符串的长度(有检查空字符串)
RT2_CORE_API int SafeStrlen(const char* lpsz);
// 检查 iIndex位置上的字符是否是中文字符
//   0 不是, 1 是前面一个字符, 2 是后面一个字符
RT2_CORE_API int IsChineseChar(const char* lpsz, int iIndex);

/** 字符串类.
*       - 高性能的，如果字符串没有修改，即使被复制多次在内存中也只存在一份
*       - 线程安全的，牵涉到共享数据的读写都有使用资源锁
*       - 多功能的，定义了包含目录连接操作(*)等等的多种功能
*       - 透明的，安全重载了所有的比较操作符
*/
class RT2_CORE_API RtString
{
public:
    struct SRegularMem
    {
	    long    nRefs;              // 引用次数, -1为lock
	    int     nDataLength;        // 数据的长度
	    int     nAllocLength;       // 分配的数据长度
        //char    pData[1];           // 数据指针
	    //char*   data()  {return pData+1; }
	    char*   data()  {return (char*)(this+1); }
    };
public:
// Constructors

	// constructs empty RtString
	RtString();
	// copy constructor
	RtString(const RtString& stringSrc);
	// from a single character
	RtString(char ch, int nRepeat = 1);
	// from an ANSI string
	RtString(char* lpsz);
	// from an ANSI string
	RtString(const char* lpsz);
	// from unsigned characters
	RtString(const unsigned char* psz);
	// subset of characters from an ANSI string
	RtString(char* lpch, int nLength);

// Attributes & Operations

	// get data length
	int GetLength() const;
	// TRUE if zero length
	bool IsEmpty() const;
	// clear contents to empty
	void Empty();

	// return single character at zero-based index
	char GetAt(int nIndex) const;
	// return single character at zero-based index
	char operator[](int nIndex) const;
	// set a single character at zero-based index
	void SetAt(int nIndex, char ch);
    const char* c_str() const    { return (const char*)m_pchData; }
	// return pointer to const string
    operator char*() const       { return m_pchData; }

	operator const char*() const { return (const char*)m_pchData; }

	// overloaded assignment

	// ref-counted copy from another RtString
	const RtString& operator=(const RtString& stringSrc);
	// set string content to single character
	const RtString& operator=(char ch);
	// copy string content from ANSI string
	const RtString& operator=(char* lpsz);
	// copy string content from ANSI string
	const RtString& operator=(const char* lpsz);
	// copy string content from unsigned chars
	const RtString& operator=(const unsigned char* psz);

	// string concatenation

	// concatenate from ANSI string
    const RtString& operator+=(const char* lpsz);

	// concatenate from another RtString
	const RtString& operator+=(const RtString& str);

	// concatenate a single character
	const RtString& operator+=(char ch);

    /** 连接后面的字符串.
    *   如果最后一个字符串不是目录分割符，就添加一个目录分割符
    */
	const RtString& operator*=( const char* pStr );

    /** 连接后面的字符串.
    *   如果最后一个字符串不是目录分割符，就添加一个目录分割符
    */
	const RtString& operator*=( const RtString& str );

    /** 连接后面的字符串.
    *   如果最后一个字符串不是目录分割符，就添加一个目录分割符
    */
	RtString operator*( const char* pStr ) const;

	friend RtString operator+(const RtString& string1, const RtString& string2);
	friend RtString operator+(const RtString& str, char ch);
	friend RtString operator+(char ch, const RtString& str);
	friend RtString operator+(const RtString& str, const char* lpsz);
	friend RtString operator+(const char* lpsz, const RtString& str);

	// string comparison

	// straight character comparison
	int Compare(const char* lpsz) const;
	// compare ignoring case
	int CompareNoCase(const char* lpsz) const;

	// simple sub-string extraction

	// return nCount characters starting at zero-based nFirst
	RtString Mid(int nFirst, int nCount) const;
	// return all characters starting at zero-based nFirst
	RtString Mid(int nFirst) const;
	// return first nCount characters in string
	RtString Left(int nCount) const;
	// return nCount characters from end of string
	RtString Right(int nCount) const;

	//  characters from beginning that are also in passed string
	RtString SpanIncluding(const char* lpszCharSet) const;
	// characters from beginning that are not also in passed string
	RtString SpanExcluding(const char* lpszCharSet) const;

	// upper/lower/reverse conversion

	// NLS aware conversion to uppercase
	void MakeUpper();
	// NLS aware conversion to lowercase
	void MakeLower();
	// reverse string right-to-left
	void MakeReverse();

	// trimming anything (either side)

	// remove continuous occurrences of chTarget starting from right
	void TrimRight(char chTarget=' ');
	// remove continuous occcurrences of characters in passed string,
	// starting from right
	void TrimRight(const char* lpszTargets);
	// remove continuous occurrences of chTarget starting from left
	void TrimLeft(char chTarget=' ');
	// remove continuous occcurrences of characters in
	// passed string, starting from left
	void TrimLeft(const char* lpszTargets);

	// advanced manipulation

	// replace occurrences of chOld with chNew
	int Replace(char chOld, char chNew);
	// replace occurrences of substring lpszOld with lpszNew;
	// empty lpszNew removes instances of lpszOld
	int Replace(const char* lpszOld, const char* lpszNew);
	// remove occurrences of chRemove
	int Remove(char chRemove);
	// insert character at zero-based index; concatenates
	// if index is past end of string
	int Insert(int nIndex, char ch);
	// insert substring at zero-based index; concatenates
	// if index is past end of string
	int Insert(int nIndex, const char* pstr);
	// delete nCount characters starting at zero-based index
	int Delete(int nIndex, int nCount = 1);

	// searching

	// find character starting at left, -1 if not found
	int Find(char ch) const;
	// find character starting at right
	int ReverseFind(char ch) const;
	// find character starting at zero-based index and going right
	int Find(char ch, int nStart) const;
	// find first instance of any character in passed string
	int FindOneOf(const char* lpszCharSet) const;
	// find first instance of substring
	int Find(const char* lpszSub) const;
	// find first instance of substring starting at zero-based index
	int Find(const char* lpszSub, int nStart) const;

	// simple formatting

	// printf-like formatting using passed string
	void Format(const char* lpszFormat, ...);

	// formatting for localization (uses FormatMessage API)

	// Access to string implementation buffer as "C" character array

	// get pointer to modifiable buffer at least as long as nMinBufLength
	char* GetBuffer(int nMinBufLength);
	// release buffer, setting length to nNewLength (or to first nul if -1)
	void ReleaseBuffer(int nNewLength = -1);
	// get pointer to modifiable buffer exactly as long as nNewLength
	char* GetBufferSetLength(int nNewLength);
	// release memory allocated to but unused by string
	void FreeExtra();

    // Use LockBuffer/UnlockBuffer to turn refcounting off

	// turn refcounting back on
	char* LockBuffer();
	// turn refcounting off
	void UnlockBuffer();

    // Conversion

    // convert to integer
    int         ToInt();
    // convert to long
    long        ToLong();
    // convert to float
    float       ToFloat();
    // convert to double
    double      ToDouble();
    // convert to integer
    void        ToNumber(int &data);
    // convert to long
    void        ToNumber(long &data);
    // convert to float
    void        ToNumber(float &data);
    // convert to double
    void        ToNumber(double &data);
#ifndef _DEBUG
    void* operator new(size_t iSize);
    void operator delete(void *p);
    void* operator new[](size_t iSize);
    void operator delete[](void* p);
#endif

// Implementation
public:
	~RtString();
	int GetAllocLength() const;

protected:
	char* m_pchData;   // pointer to ref counted string data

	// implementation helpers
	SRegularMem* GetData() const;
	void Init();
	void AllocCopy(RtString& dest, int nCopyLen, int nCopyIndex, int nExtraLen) const;
	void AllocBuffer(int nLen);
	void AssignCopy(int nSrcLen, const char* lpszSrcData);
	void ConcatCopy(int nSrc1Len, const char* lpszSrc1Data, int nSrc2Len, const char* lpszSrc2Data);
	void ConcatInPlace(int nSrcLen, const char* lpszSrcData);
	void CopyBeforeWrite();
	void AllocBeforeWrite(int nLen);
	void Release();
	static void Release(SRegularMem* pData);
	static void FreeData(SRegularMem* pData);
};

// Compare helpers
RT2_CORE_API bool operator==(const RtString& s1, const RtString& s2);
RT2_CORE_API bool operator==(const RtString& s1, const char* s2);
RT2_CORE_API bool operator==(const char* s1, const RtString& s2);
RT2_CORE_API bool operator!=(const RtString& s1, const RtString& s2);
RT2_CORE_API bool operator!=(const RtString& s1, const char* s2);
RT2_CORE_API bool operator!=(const char* s1, const RtString& s2);
RT2_CORE_API bool operator<(const RtString& s1, const RtString& s2);
RT2_CORE_API bool operator<(const RtString& s1, const char* s2);
RT2_CORE_API bool operator<(const char* s1, const RtString& s2);
RT2_CORE_API bool operator>(const RtString& s1, const RtString& s2);
RT2_CORE_API bool operator>(const RtString& s1, const char* s2);
RT2_CORE_API bool operator>(const char* s1, const RtString& s2);
RT2_CORE_API bool operator<=(const RtString& s1, const RtString& s2);
RT2_CORE_API bool operator<=(const RtString& s1, const char* s2);
RT2_CORE_API bool operator<=(const char* s1, const RtString& s2);
RT2_CORE_API bool operator>=(const RtString& s1, const RtString& s2);
RT2_CORE_API bool operator>=(const RtString& s1, const char* s2);
RT2_CORE_API bool operator>=(const char* s1, const RtString& s2);

} // namespace rt2_core

/*-----------------------------------------------------------------------------
-   The End.
-----------------------------------------------------------------------------*/
