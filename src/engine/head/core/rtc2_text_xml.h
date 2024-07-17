/**
* @file rtc2_text_xml.h XML文件操作
* @author janhail
*/

namespace rt2_core {

/*------------------------------------------------------------------------
-   class RtScriptXML.
------------------------------------------------------------------------*/

////////////////////////////////////////////////////////////////////////////////
// 读取并分析XML格式数据
// By Janhail.
//

/** XML 读取类(通过继承函数进行读取).
*   - XML的格式为: Element: <Tag[ Attribute=value]>[Item,Element]<\Tag>
*       - Tag 为一个不包含 =\n\r\t" > 的字符串
*       - Attribute 为一个不包含 =\n\r\t" > 的字符串
*       - value 为一个不包含 =\n\r\t" > 的字符串
*             - 或者为一个双引号中的字符串
*       - Item 为一个包含任何字符的字符串
*       - 用 "\>" 代替 ">"
*       - 用 "\\" 代替 "\"
*       - 用 "\<" 代替 "<"
*/
class RtScriptXML
{
public:
    RtScriptXML();
    virtual ~RtScriptXML();

    virtual void OnError(const char* szFilename, const char* szErrMsg, long lLineNo);

    virtual void OnXmlStart(const char *pTag){}
    virtual void OnTagStart(const char *pTag){}
    virtual void OnTagEnd(){}
    virtual void OnTagEndImme(){}
    virtual void OnAttribute(const char *pAttribute, const char *pValue){}
    virtual void OnCloseTagStart(const char *pTag){}
    virtual void OnCloseTagEnd(){}
    virtual void OnItem(const char *pItem){}

public:
    const char* GetFilename();

    /** 分析文件szFilename的内容, 返回0正确，其他错误.
    *     bHtmlCompatible 是否和HTML兼容，如果兼容就会在input,meta等tag后面自动加上'\'
    */
    int ParserFile(const char* szFilename, int bHtmlCompatible=0);

    /** 分析字符串szString的内容, 返回0正确，其他错误.
    *     bHtmlCompatible 是否和HTML兼容，如果兼容就会在input,meta等tag后面自动加上'\'
    */
    int ParserString(const char* szString, int bHtmlCompatible=0);

    //int ScanningAll(TCallbackVisit pCallbackVisit=0, void *arg=0);

private:
    RtString m_strFilename;
    RtString m_strContent;

};

/** XML 读取类(生成可以返回的节点).
*       - 读取XML生成节点
*       - 支持写入XML
*/
class RtsXmlDoc : public RtScriptXML
{
public:
    enum ENodeType {XML_ROOT, XML_TAG, XML_ITEM};
    /// 节点.
    struct NodePtr
    {
        ENodeType   eType;
        NodePtr*    pParent;                            // 父节点
        NodePtr*    pChildren;                          // 子节点
        NodePtr*    pNext;                              // 下一个兄弟节点
        std::string                         strName;          ///< 节点名字
        std::map<std::string, std::string>  mapAttribute;     // 节点属性列表
        std::map<std::string, std::string>::iterator itAttr;

        /** 取得名字.
        */
        const char* GetName();
        bool IsTag()            { return eType==XML_TAG; }
        bool IsItem()           { return eType==XML_ITEM; }
        NodePtr* GetParent()    { return pParent; }
        NodePtr* GetChildren()  { return pChildren; }
        NodePtr* GetNext()      { return pNext; }

        /** 取得第一个属性.
        *   @param ppName  [out] 输出第一个属性的名字
        *   @param ppValue [out] 输出第一个属性的值
        *   @return 返回true表示有值返回, false表示没有任何属性返回
        */
        bool FirstProp(const char** ppName, const char** ppValue);

        /** 取得下一个属性.
        *   @param ppName  [out] 输出第一个属性的名字
        *   @param ppValue [out] 输出第一个属性的值
        *   @return 返回true表示有值返回, false表示没有任何属性返回
        */
        bool NextProp(const char** ppName, const char** ppValue);

        const char* GetProp(const char* pPropName) const;       ///< 取得属性，如果没有该属性就返回NULL
        const char* GetProp_s(const char* pPropName) const;     ///< 取得属性，如果没有该属性就返回""
        int GetPropInt(const char* pPropName) const;            ///< 取得属性，按照数值返回，内部单纯的调用atoi
        unsigned long GetPropDword(const char* pPropName) const;///< 取得属性，按照数值返回，内部单纯的调用atoi
        int GetPropHex(const char* pPropName) const;            ///< 取得属性，按照数值返回，字符串被认为是16进制，不符合就返回0
        float GetPropFloat(const char* pPropName) const;        ///< 取得属性，按照浮点数返回，内部单纯的调用atof
        bool GetPropBool(const char* pPropName) const;         ///< 取得属性

        /** 添加属性.
        */
        bool AddProp(const char* szPropName, const char* szPropValue);
        bool AddProp(const char* szPropName, unsigned long dwValue, bool bHex=false);
        bool AddProp(const char* szPropName, int iValue, bool bHex=false);
        bool AddProp(const char* szPropName, float fValue);
        bool AddProp(const char* szPropName, bool bValue);

        /** 设置属性值
        */
        bool SetProp(const char* szPropName, const char* szPropValue);
        bool SetProp(const char* szPropName, unsigned long dwValue, bool bHex=false);
        bool SetProp(const char* szPropName, int iValue, bool bHex=false);
        bool SetProp(const char* szPropName, float fValue);
        bool SetProp(const char* szPropName, bool bValue);

        /** 删除属性.
        */
        bool DelProp(const char* szPropName);

        /** 添加项.
        */
        NodePtr* AddItem(const char* szItemContent);

        /** 删除项.
        */
        bool DelItem(const char* szItemContent);

        /** 添加子标记.
        */
        NodePtr* AddTag(const char* szTagName);

        /** 删除子标记.
        *   @param szTagName 标记名字
        *   @param bAll      为true表示删除这个标记下面一层的所有指定名字的标记, false表示只删除第一个
        *   @return          返回true表示删除成功，否则就没有找到该名字的标记
        */
        bool DelTag(const char* szTagName, bool bAll=false);

        bool WriteToArchive(RtArchive& arc, bool bFormated, int iLevel);
#ifndef _DEBUG
        inline void* operator new(size_t iSize)     { return RtMemoryAllocator::Instance().Alloc(iSize, __FILE__, __LINE__); }
        inline void operator delete(void *p)        { return RtMemoryAllocator::Instance().Free(p); }
        inline void* operator new[](size_t iSize)   { return RtMemoryAllocator::Instance().Alloc(iSize, __FILE__, __LINE__); }
        inline void operator delete[](void* p)      { return RtMemoryAllocator::Instance().Free(p); }
#endif
    };

public:
    RtsXmlDoc();
    virtual ~RtsXmlDoc();

    void Cleanup();
    NodePtr*    GetRoot()       { return m_pRoot; }
    const char* GetDocDesc()    { return m_strDocDesc.c_str(); }

    /** 创建root.
    *   @param szRootname Root的名字
    *   @return 如果root已经创建，就会返回false，否则返回true
    */
    NodePtr* NewRoot(const char* szRootname);

    bool WriteToFile(const char* szFilename);
    bool WriteToArchive(RtArchive& arc, bool bFormated=false);

protected:
    virtual void OnXmlStart(const char *pTag);
    virtual void OnTagStart(const char *pTag);
    virtual void OnTagEnd();
    virtual void OnTagEndImme();
    virtual void OnAttribute(const char *pAttribute, const char *pValue);
    virtual void OnCloseTagStart(const char *pTag);
    virtual void OnCloseTagEnd();
    virtual void OnItem(const char *pItem);

protected:
    NodePtr*    m_pRoot;
    NodePtr*    m_pCurrent;
    std::string m_strDocDesc;

};

} // namespace rt2_core

/*------------------------------------------------------------------------
-   The End.
------------------------------------------------------------------------*/
