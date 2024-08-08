/**
* @file rtc2_text_ini.h INI文件操作
* @author janhail
*/
#pragma once
namespace rt2_core {

/*------------------------------------------------------------------------
-   class RtIni.
------------------------------------------------------------------------*/

typedef int (*TCallbackINIVisit)(int iItemType, const char* szArg1, const char* szArg2, int iValueType, void *arg);

/** Ini文件操作.
*/
class RtIni
{
public:
    enum ItemType
    {
        IT_ENTRY   = 1,
        IT_SECTION = 2,
    };

    enum ValueType
    {
        VT_NONE   = 0,
        VT_LONG   = 1,
        VT_DOUBLE = 2,
        VT_STRING = 3,
    };
    struct SItem
    {
        RtString        szName;
        ValueType       eType;
        RtString        szValue;
        SItem*          pNext;
        SItem*          pPrev;
    private:
        long ConvertToLong() const;
        double ConvertToFloat() const;
        void SetLong(long v);
        void SetFloat(double v);
    public:
        SItem()
        {
            eType = VT_NONE;
            pNext = NULL;
            pPrev = NULL;
        };

        operator const char*() const        { return szValue.c_str(); }
        operator bool()        const        { return ConvertToLong()!=0; }
        operator char()        const        { return (char)ConvertToLong(); }
        operator BYTE()        const        { return (BYTE)ConvertToLong(); }
        operator short()       const        { return (short)ConvertToLong(); }
        operator WORD()        const        { return (WORD)ConvertToLong(); }
        operator int()         const        { return (int)ConvertToLong(); }
        operator long()        const        { return ConvertToLong(); }
        operator DWORD()       const        { return (DWORD)ConvertToLong(); }
        operator float()       const        { return (float)ConvertToFloat(); }
        operator double()      const        { return ConvertToFloat(); }

        const SItem& operator = (const char* v);
        const SItem& operator = (bool v)    { SetLong(v); return *this; }
        const SItem& operator = (char v)    { SetLong(v); return *this; }
        const SItem& operator = (BYTE v)    { SetLong(v); return *this; }
        const SItem& operator = (short v)   { SetLong(v); return *this; }
        const SItem& operator = (WORD v)    { SetLong(v); return *this; }
        const SItem& operator = (int v)     { SetLong(v); return *this; }
        const SItem& operator = (long v)    { SetLong(v); return *this; }
        const SItem& operator = (DWORD v)   { SetLong(v); return *this; }
        const SItem& operator = (float v)   { SetFloat(v);return *this; }
        const SItem& operator = (double v)  { SetFloat(v);return *this; }
#ifndef _DEBUG
        inline void* operator new(size_t iSize)
        {
            return RtMemoryAllocator::Instance().Alloc(iSize, __FILE__, __LINE__);
        }
        inline void operator delete(void *p)
        {
            return RtMemoryAllocator::Instance().Free(p);
        }
        inline void* operator new[](size_t iSize)
        {
            return RtMemoryAllocator::Instance().Alloc(iSize, __FILE__, __LINE__);
        }
        inline void operator delete[](void* p)
        {
            return RtMemoryAllocator::Instance().Free(p);
        }
#endif

    };

    struct SSection
    {
        RtString        szName;
        SItem*          pFirstItem;
        SItem*          pLastItem;
        SSection*       pNext;
        SSection*       pPrev;
        RtIni*          pIni;
    public:
        SSection(RtIni* p);
        ~SSection();

        int GetCount();
        SItem& operator[](int nIndex);
        SItem& operator[](const char* szIdx);
#ifndef _DEBUG
        inline void* operator new(size_t iSize)
        {
            return RtMemoryAllocator::Instance().Alloc(iSize, __FILE__, __LINE__);
        }
        inline void operator delete(void *p)
        {
            return RtMemoryAllocator::Instance().Free(p);
        }
        inline void* operator new[](size_t iSize)
        {
            return RtMemoryAllocator::Instance().Alloc(iSize, __FILE__, __LINE__);
        }
        inline void operator delete[](void* p)
        {
            return RtMemoryAllocator::Instance().Free(p);
        }
#endif
    };

    static SItem s_nullItem;

public:
    /** 构造函数.
    * @param bSameEntry 是否允许相同的项，如果这个值为true，那同一个Section下面允许有相同名字的项
    */
    RtIni(bool bSameEntry=false); // 是否允许相同的项

    /** 取得当前打开的文件名.
    * @return 返回文件名，如果当前没有打开任何文件，就返回NULL
    */
    virtual ~RtIni();

    /** 取得当前打开的文件名.
    * @return 返回文件名，如果当前没有打开任何文件，就返回NULL
    */
    const char* GetFilename();

    /** 打开文件.
    * @param szFilename: 为文件名
    * @param bWrite: true表示这个文件用于写入, false用于读出
    * @return 返回true正确，其他错误
    */
    bool OpenFile(const char* szFilename, bool bWrite=false);

    /** 关闭文件.
    * @param bWrite表示关闭的是否是否保存文件
    * @return 返回true正确，其他错误
    */
    bool CloseFile(bool bWrite=true);

    /** 保存文件.
    * @return 返回true正确，其他错误
    */
    bool SaveFile();

    /** 保存到指定文件.
    * @param szFilename文件名
    * @return 返回true正确，其他错误
    */
    bool SaveAs(const char* szFilename);

    /** 清除数据.
    */
    void ClearData();

    // 重载

    int GetCount();
    SSection& operator[](int nIndex);
    SSection& operator[](const char* szIdx);

    // add

    bool AddSection(const char* szSection);
    bool AddEntry(const char* szSection, const char* szEntry, const char* szValue);
    bool AddEntry(const char* szSection, const char* szEntry, long lNumber);
    bool AddEntry(const char* szSection, const char* szEntry, DWORD dwNumber);
    bool AddEntry(const char* szSection, const char* szEntry, float fNumber);
    bool AddEntry(const char* szSection, const char* szEntry, double dNumber);

    // delete

    bool DelSection(const char* szSection);
    bool DelEntry(const char* szSection, const char* szEntry, bool bAllSameEntry=false); // bAllSameEntry是否删除所有相同的项

    // set, 当允许相同的项的时候，以下函数只能设置第一个找到的项

    bool SetEntry(const char* szSection, const char* szEntry, const char* szValue);
    bool SetEntry(const char* szSection, const char* szEntry, long lNumber);
    bool SetEntry(const char* szSection, const char* szEntry, DWORD dwNumber);
    bool SetEntry(const char* szSection, const char* szEntry, float fNumber);
    bool SetEntry(const char* szSection, const char* szEntry, double dNumber);

    // get, 当允许相同的项的时候，以下函数只能取出第一个找到的项

    ValueType GetEntryType(const char* szSection, const char* szEntry);
    const char* GetEntry(const char* szSection, const char* szEntry);
    bool GetEntry(const char* szSection, const char* szEntry, long* pNumber);
    bool GetEntry(const char* szSection, const char* szEntry, float* pNumber);
    bool GetEntry(const char* szSection, const char* szEntry, double* pNumber);

    bool GetEntry(const char* szSection, const char* szEntry, char* pStr, int iMaxLen);
    bool GetEntry(const char* szSection, const char* szEntry, bool* pNumber);
    bool GetEntry(const char* szSection, const char* szEntry, char* pNumber);
    bool GetEntry(const char* szSection, const char* szEntry, BYTE* pNumber);
    bool GetEntry(const char* szSection, const char* szEntry, short* pNumber);
    bool GetEntry(const char* szSection, const char* szEntry, WORD* pNumber);
    bool GetEntry(const char* szSection, const char* szEntry, int* pNumber);
    bool GetEntry(const char* szSection, const char* szEntry, DWORD* pNumber);

    // searching

    bool FindSection(const char* szSection);
    bool FindEntry(const char* szSection, const char* szEntry);

    bool FirstSection(RtString* pName=NULL);
    bool NextSection(RtString* pName=NULL);
    bool FirstEntry(RtString* pName=NULL, RtString* pValue=NULL, ValueType* pType=NULL);
    bool NextEntry(RtString* pName=NULL, RtString* pValue=NULL, ValueType* pType=NULL);

    int ScanningAll(TCallbackINIVisit pCallbackVisit=0, void *arg=0);

    virtual void OnError(const char* szFilename, const char* szErrMsg, long lLineNo);
    virtual int  OnVisit(int iItemType, const char* szArg1, const char* szArg2, int iValueType, void *arg);

private:
    RtString        m_szFilename;
    SSection        m_sDefaultSection;
    SSection*       m_pFirstSection;
    SSection*       m_pLastSection;
    SSection*       m_pCurrentSection;
    SItem*          m_pCurrentItem;
    bool            m_bInit;
    bool            m_bCanWrite;
    bool            m_bSameEntry;
};

} // namespace rt2_core

/*------------------------------------------------------------------------
-   The End.
------------------------------------------------------------------------*/
