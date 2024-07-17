/**
* @file rtc2_text_table.h CSV�ļ�����
* @author janhail
*/

namespace rt2_core {

/** ����ȡ���������.
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

/** ����ȡ��.
*   ���ڶ�ȡ�̶����Ϸָ���ļ�������Excel������CSV�ļ�
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
    // ����һ���ı�����ļ� szFilename
    bool LoadTableFile(const char* szFilename);

    // ���������Ϣ
    void Clear();

    // �Ƿ��Ѿ�����
    bool IsLoaded()                                 { return m_pContent!=NULL; }

    const TableLine& operator[](int nIndex) const   { return m_pLines[nIndex]; }
    const TableLine& operator[](const char* szIdx) const;

    // ȡ��ָ���к��е��ַ���
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

    int GetLineCount()  { return m_iLineCount; } // ����ܵ�����
    int GetMaxRow()     { return m_iMaxRow; }    // �����������

    void SetTitleLine(int iIdx)     { m_iTitleLine=iIdx; } // ����������(0 base)���������ַ����������
    int  GetTitleLine()             { return m_iTitleLine; }
    void SetTitleRow(int iIdx)      { m_iTitleRow=iIdx; }  // ����������(0 base)���������ַ����������
    int  GetTitleRow()              { return m_iTitleRow; }

    // ���ҹؼ���szString��һ�γ��ֵ�λ�ã��ҵ�����true���ҷ������е�iLine, iRow���򷵻�false
    bool FindPosByString(const char* szString, int& iLine, int& iRow);
    int FindLineByString(const char* szString); // ����szString��һ�γ��ֵ��У��Ҳ�������-1
    int FindRowByString(const char* szString);  // ����szString��һ�γ��ֵ��У��Ҳ�������-1

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

/** ��ȡCSV�ļ�.
*   ���ڶ�ȡExcel������CSV�ļ�
*/
class RtCsv : public RtTable
{
public:
    RtCsv() : RtTable(',') {}
    virtual ~RtCsv(){}
};


/** ����ȡģ��.
*   - OnSetTitleLine() �������ñ�ͷ��ͨ���÷�����
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
*   - OnReadStart() ��ʼ��ȡ���¼�
*   - OnReadFinish() ������ȡ���¼�
*   - OnReadLineStart() ��ʼ��ȡһ�е��¼�
*   - OnReadLineFinish() ������ȡһ�е��¼�
*   - OnCreateLineObject() ����һ������ͨ���÷�����
*      @code
*       virtual TableObject* OnCreateLineObject()
*       {
*           return new YourTableObject;
*       }
*      @endcode
*       ע�⣺ʹ�������Ķ�������TableObject�̳�
*   - OnDestroyLineObject() ����һ��������һ��Ĭ�ϵĺ�����ֻ�Ǽ򵥵�ִ��delete����
*   - OnReadLineItem() ��ȡ�����һ��Ԫ��
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
*                   return false; // ����ʽ�����˳���ȡ
*               }
*           }else
*           {
*               return false; // ��������ʶ��Item������false�ö���ʧ�ܣ����Ҫ�������������ɾ�������
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
        TRS_SKIP = -1, // ��������
        TRS_EXIT =  0, // �˳���ȡ
        TRS_OK   =  1, // ����
    };

    /** ���ж��󶼱�����������̳�.
    *   ע�⣺���е����඼�������麯��
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

    /** �������������.
    */
    RtTableLoader();
    virtual ~RtTableLoader();

    /** ����һ���ļ�.
    */
    bool Load(RtString& szLoadFilename);

    /** �ٴ��������һ��������ļ�.
    */
    bool Reload();

    /** �ٴ�����һ��ָ�����ļ�.
    */
    bool ReloadByFilename(RtString& szLoadFilename);

    /** ���������Ϣ.
    */
    void Clear();

    /** ����ID����һ������.
    */
    TableObject* Find(int iIdx);

    /** ����λ�ò���һ������.
    *   ���Ƽ�ʹ���������
    */
    TableObject* At(int iIdx);

protected:

    /** ���ñ����к�������.
    */
    virtual bool OnSetTitleLine(RtCsv& csv) { return false; }

    /** ��ʼ��ȡ.
    */
    virtual bool OnReadStart() { return false; }

    /** ��ȡ����.
    */
    virtual bool OnReadFinish() { return false; }

    /** ��ʼ��ȡһ��.
    */
    virtual ETableReadState OnReadLineStart(int iLine, const RtTable::TableLine& line) { return TRS_EXIT; }

    /** ��ȡһ�н���.
    */
    virtual ETableReadState OnReadLineFinish(int iLine, TableObject* pItem) { return TRS_EXIT; }

    /** ����һ������.
    */
    virtual TableObject* OnCreateLineObject() { return NULL; }

    /** ����һ������.
    */
    virtual void OnDestroyLineObject(TableObject* pObj) { delete pObj; }

    /** ��ȡһ��Ԫ��.
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
