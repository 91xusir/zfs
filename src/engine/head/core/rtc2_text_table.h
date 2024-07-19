/**
* @file rtc2_text_table.h CSV文件操作
* @author janhail
*/

namespace rt2_core {

/** 表格读取程序的例子.
*      @code
*       void test_azoth_table()
*       {
*           RtCsv csv;
*           csv.LoadTableFile("test.csv");
*
*           printf("Line: %d, Row: %d\n", csv.GetLineCount(), csv.GetMaxRow());
*           int i, j;
*           for (j=0; j<csv.GetLineCount(); j++)
*           {
*               for (i=0; i<csv.GetMaxRow(); i++)
*               {
*                   printf("[%c]%s\t", csv[j][i].IsValid()?'T':'F', (const char*)csv[j][i]);
*               }
*               printf("\n");
*           }
*
*           printf("1: %s\n", csv[2]["d1"]);
*           printf("2: %s\n", csv["a3"][3]);
*           printf("3: %s\n", csv["a3"]["d1"]);
*           return;
*       }
*      @endcode
------------------------------------------------------------------------*/

/** 表格读取类.
*   用于读取固定符合分割的文件，比如Excel导出的CSV文件
*/
class RtTable
{
public:
    class TableItem
    {
    public:
        TableItem(const char* szItemString) {m_szItemString = szItemString;}
        bool IsValid() const                { return m_szItemString[0]!=0; }
        operator const       char*() const  { return m_szItemString; }
        RtString             String() const { return RtString(m_szItemString); }
        const char*          Str() const    { return m_szItemString; }
        const char*          SafeStr()const { return m_szItemString?m_szItemString:""; }
        const bool           Bool() const   { return                 atoi(m_szItemString)!=0; }
        const char           Char() const   { return (char)          m_szItemString[0]; }
        const unsigned char  Byte() const   { return (unsigned char) atoi(m_szItemString); }
        const short          Short() const  { return (short)         atoi(m_szItemString); }
        const unsigned short Word() const   { return (unsigned short)atoi(m_szItemString); }
        const int            Int() const    { return                 atoi(m_szItemString); }
        const unsigned int   UInt() const   { return (unsigned int)  atoi(m_szItemString); }
        const long           Long() const   { return                 atol(m_szItemString); }
        const unsigned long  DWord() const  { return (DWORD)         atol(m_szItemString); }
        const float          Float() const  { return (float)         atof(m_szItemString); }
        const double         Double() const { return                 atof(m_szItemString); }
    private:
        const char* m_szItemString;
    };
    class TableLine
    {
    protected:
        TableLine() {}
        void SetLine(RtTable* pTable, int iLineIdx)   { m_pTable = pTable; m_iLineIdx = iLineIdx;}
    public:
        friend class RtTable;
        const TableItem operator[](int nIndex) const
        {
            return TableItem(m_pTable->GetString(m_iLineIdx, nIndex));
        }
        const TableItem operator[](const char* szIdx) const
        {
            return TableItem(m_pTable->GetString(m_iLineIdx, szIdx));
        }
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
    private:
        RtTable*  m_pTable;
        int                 m_iLineIdx;
    };

protected:
    RtTable(char cSeparator);
    virtual ~RtTable();
    bool ParseTextTable();

public:
    // 载入一个文本表格文件 szFilename
    bool LoadTableFile(const char* szFilename);

    // 清除所有信息
    void Clear();

    // 是否已经载入
    bool IsLoaded()                                 { return m_pContent!=NULL; }

    const TableLine& operator[](int nIndex) const   { return m_pLines[nIndex]; }
    const TableLine& operator[](const char* szIdx) const;

    // 取得指定行和列的字符串
    const char* GetString(int iLine, int iRow) const;
    const char* GetString(int iLine, const char* szRowIdx) const;
    const char* GetString(const char* szLineIdx, const char* szRowIdx) const;
    bool Char  (int iLine, int iRow, char          & vValue) const;
    bool Byte  (int iLine, int iRow, unsigned char & vValue) const;
    bool Short (int iLine, int iRow, short         & vValue) const;
    bool Word  (int iLine, int iRow, unsigned short& vValue) const;
    bool Int   (int iLine, int iRow, int           & vValue) const;
    bool UInt  (int iLine, int iRow, unsigned int  & vValue) const;
    bool Long  (int iLine, int iRow, long          & vValue) const;
    bool DWord (int iLine, int iRow, unsigned long & vValue) const;
    bool Float (int iLine, int iRow, float         & vValue) const;
    bool Double(int iLine, int iRow, double        & vValue) const;

    int GetLineCount()  { return m_iLineCount; } // 表格总的行数
    int GetMaxRow()     { return m_iMaxRow; }    // 表格最大的列数

    void SetTitleLine(int iIdx)     { m_iTitleLine=iIdx; } // 设置索引行(0 base)，用于用字符串索引表格
    int  GetTitleLine()             { return m_iTitleLine; }
    void SetTitleRow(int iIdx)      { m_iTitleRow=iIdx; }  // 设置索引列(0 base)，用于用字符串索引表格
    int  GetTitleRow()              { return m_iTitleRow; }

    // 查找关键字szString第一次出现的位置，找到返回true并且返回行列到iLine, iRow否则返回false
    bool FindPosByString(const char* szString, int& iLine, int& iRow);
    int FindLineByString(const char* szString); // 查找szString第一次出现的行，找不到返回-1
    int FindRowByString(const char* szString);  // 查找szString第一次出现的列，找不到返回-1

protected:
    char        m_cSeparator;
    RtString    m_szFilename;
    char*       m_pContent;
    char**      m_pItems;
    TableLine*  m_pLines;
    TableLine   m_InvalidLine;

    int         m_iLineCount;
    int         m_iMaxRow;

    int         m_iTitleLine;
    int         m_iTitleRow;

};

/** 读取CSV文件.
*   用于读取Excel导出的CSV文件
*/
class RtCsv : public RtTable
{
public:
    RtCsv() : RtTable(',') {}
    virtual ~RtCsv(){}
};


/** 表格读取模版.
*   - OnSetTitleLine() 用来设置表头，通常用法如下
*      @code
*       virtual bool OnSetTitleLine(RtCsv& csv)
*       {
*           int iLine = csv.FindLineByString("ID");
*           if(iLine<0) return false;
*           csv.SetTitleLine(iLine);
*           csv.SetTitleRow(0);
*           return true;
*       }
*      @endcode
*   - OnReadStart() 开始读取的事件
*   - OnReadFinish() 结束读取的事件
*   - OnReadLineStart() 开始读取一行的事件
*   - OnReadLineFinish() 结束读取一行的事件
*   - OnCreateLineObject() 创建一个对象，通常用法如下
*      @code
*       virtual TableObject* OnCreateLineObject()
*       {
*           return new YourTableObject;
*       }
*      @endcode
*       注意：使用这个类的对象必须从TableObject继承
*   - OnDestroyLineObject() 销毁一个对象，有一个默认的函数，只是简单的执行delete操作
*   - OnReadLineItem() 读取行里的一个元素
*      @code
*       virtual ETableReadState OnReadLineItem(int iLine, const char* szItemTitle, const RtTable::TableItem& itemLine, TableObject* pTObj)
*       {
*           YourTableObject* pObj = (YourTableObject*)pTObj;
*           if (strcmp(szItemTitle, "ID")==0)
*           {
*               pObj->dwID = itemLine.DWord();
*           }else if (strcmp(szItemTitle, "Name")==0)
*           {
*               pObj->strName = itemLine.Str();
*           }else if (strcmp(szItemTitle, "Sex")==0)
*           {
*               pObj->bSex = itemLine.Bool();
*           }else if (strcmp(szItemTitle, "Items")==0)
*           {
*               if (sscanf(itemLine.Str(),"%d,%d,%d", &(pObj->Item[0]), &(pObj->Item[1]), &(pObj->Item[2])) != 3)
*               {
*                   return false; // 表格格式错误，退出读取
*               }
*           }else
*           {
*               return false; // 遇到不认识的Item，返回false让读表失败，如果要忽略这个错误，请删除本语句
*           }
*           return true;
*       }
*      @endcode
*/
class RtTableLoader
{
public:
    enum ETableReadState
    {
        TRS_SKIP = -1, // 跳过本行
        TRS_EXIT =  0, // 退出读取
        TRS_OK   =  1, // 继续
    };

    /** 所有对象都必须从这个基类继承.
    *   注意：所有的子类都不能有虚函数
    */
    struct TableObject
    {
#ifndef _DEBUG
    inline void* operator new(size_t iSize)     { return RtMemoryAllocator::Instance().Alloc(iSize, __FILE__, __LINE__); }
    inline void operator delete(void *p)        { return RtMemoryAllocator::Instance().Free(p); }
    inline void* operator new[](size_t iSize)   { return RtMemoryAllocator::Instance().Alloc(iSize, __FILE__, __LINE__); }
    inline void operator delete[](void* p)      { return RtMemoryAllocator::Instance().Free(p); }
#endif
    };

    /** 构造和析构函数.
    */
    RtTableLoader();
    virtual ~RtTableLoader();

    /** 载入一个文件.
    */
    bool Load(RtString& szLoadFilename);

    /** 再次载入最后一次载入的文件.
    */
    bool Reload();

    /** 再次载入一个指定的文件.
    */
    bool ReloadByFilename(RtString& szLoadFilename);

    /** 清除所有信息.
    */
    void Clear();

    /** 根据ID查找一个对象.
    */
    TableObject* Find(int iIdx);

    /** 根据位置查找一个对象.
    *   不推荐使用这个函数
    */
    TableObject* At(int iIdx);

protected:

    /** 设置标题行和索引列.
    */
    virtual bool OnSetTitleLine(RtCsv& csv) { return false; }

    /** 开始读取.
    */
    virtual bool OnReadStart() { return false; }

    /** 读取结束.
    */
    virtual bool OnReadFinish() { return false; }

    /** 开始读取一行.
    */
    virtual ETableReadState OnReadLineStart(int iLine, const RtTable::TableLine& line) { return TRS_EXIT; }

    /** 读取一行结束.
    */
    virtual ETableReadState OnReadLineFinish(int iLine, TableObject* pItem) { return TRS_EXIT; }

    /** 创建一个对象.
    */
    virtual TableObject* OnCreateLineObject() { return NULL; }

    /** 销毁一个对象.
    */
    virtual void OnDestroyLineObject(TableObject* pObj) { delete pObj; }

    /** 读取一个元素.
    */
    virtual ETableReadState OnReadLineItem(int iLine, const char* szItemTitle, const RtTable::TableItem& itemLine, TableObject* pTObj) { return TRS_EXIT; }

protected:
    RtString                    m_szTableFilename;
	std::vector<TableObject*>   m_table;
    std::map<int, TableObject*> m_map;

};

} // namespace rt2_core

/*------------------------------------------------------------------------
-   End.
------------------------------------------------------------------------*/
