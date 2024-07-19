/**
* @file rtc2_text_xml.h XML�ļ�����
* @author janhail
*/

namespace rt2_core {

/*------------------------------------------------------------------------
-   class RtScriptXML.
------------------------------------------------------------------------*/

////////////////////////////////////////////////////////////////////////////////
// ��ȡ������XML��ʽ����
// By Janhail.
//

/** XML ��ȡ��(ͨ���̳к������ж�ȡ).
*   - XML�ĸ�ʽΪ: Element: <Tag[ Attribute=value]>[Item,Element]<\Tag>
*       - Tag Ϊһ�������� =\n\r\t" > ���ַ���
*       - Attribute Ϊһ�������� =\n\r\t" > ���ַ���
*       - value Ϊһ�������� =\n\r\t" > ���ַ���
*             - ����Ϊһ��˫�����е��ַ���
*       - Item Ϊһ�������κ��ַ����ַ���
*       - �� "\>" ���� ">"
*       - �� "\\" ���� "\"
*       - �� "\<" ���� "<"
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

    /** �����ļ�szFilename������, ����0��ȷ����������.
    *     bHtmlCompatible �Ƿ��HTML���ݣ�������ݾͻ���input,meta��tag�����Զ�����'\'
    */
    int ParserFile(const char* szFilename, int bHtmlCompatible=0);

    /** �����ַ���szString������, ����0��ȷ����������.
    *     bHtmlCompatible �Ƿ��HTML���ݣ�������ݾͻ���input,meta��tag�����Զ�����'\'
    */
    int ParserString(const char* szString, int bHtmlCompatible=0);

    //int ScanningAll(TCallbackVisit pCallbackVisit=0, void *arg=0);

private:
    RtString m_strFilename;
    RtString m_strContent;

};

/** XML ��ȡ��(���ɿ��Է��صĽڵ�).
*       - ��ȡXML���ɽڵ�
*       - ֧��д��XML
*/
class RtsXmlDoc : public RtScriptXML
{
public:
    enum ENodeType {XML_ROOT, XML_TAG, XML_ITEM};
    /// �ڵ�.
    struct NodePtr
    {
        ENodeType   eType;
        NodePtr*    pParent;                            // ���ڵ�
        NodePtr*    pChildren;                          // �ӽڵ�
        NodePtr*    pNext;                              // ��һ���ֵܽڵ�
        std::string                         strName;          ///< �ڵ�����
        std::map<std::string, std::string>  mapAttribute;     // �ڵ������б�
        std::map<std::string, std::string>::iterator itAttr;

        /** ȡ������.
        */
        const char* GetName();
        bool IsTag()            { return eType==XML_TAG; }
        bool IsItem()           { return eType==XML_ITEM; }
        NodePtr* GetParent()    { return pParent; }
        NodePtr* GetChildren()  { return pChildren; }
        NodePtr* GetNext()      { return pNext; }

        /** ȡ�õ�һ������.
        *   @param ppName  [out] �����һ�����Ե�����
        *   @param ppValue [out] �����һ�����Ե�ֵ
        *   @return ����true��ʾ��ֵ����, false��ʾû���κ����Է���
        */
        bool FirstProp(const char** ppName, const char** ppValue);

        /** ȡ����һ������.
        *   @param ppName  [out] �����һ�����Ե�����
        *   @param ppValue [out] �����һ�����Ե�ֵ
        *   @return ����true��ʾ��ֵ����, false��ʾû���κ����Է���
        */
        bool NextProp(const char** ppName, const char** ppValue);

        const char* GetProp(const char* pPropName) const;       ///< ȡ�����ԣ����û�и����Ծͷ���NULL
        const char* GetProp_s(const char* pPropName) const;     ///< ȡ�����ԣ����û�и����Ծͷ���""
        int GetPropInt(const char* pPropName) const;            ///< ȡ�����ԣ�������ֵ���أ��ڲ������ĵ���atoi
        unsigned long GetPropDword(const char* pPropName) const;///< ȡ�����ԣ�������ֵ���أ��ڲ������ĵ���atoi
        int GetPropHex(const char* pPropName) const;            ///< ȡ�����ԣ�������ֵ���أ��ַ�������Ϊ��16���ƣ������Ͼͷ���0
        float GetPropFloat(const char* pPropName) const;        ///< ȡ�����ԣ����ո��������أ��ڲ������ĵ���atof
        bool GetPropBool(const char* pPropName) const;         ///< ȡ������

        /** �������.
        */
        bool AddProp(const char* szPropName, const char* szPropValue);
        bool AddProp(const char* szPropName, unsigned long dwValue, bool bHex=false);
        bool AddProp(const char* szPropName, int iValue, bool bHex=false);
        bool AddProp(const char* szPropName, float fValue);
        bool AddProp(const char* szPropName, bool bValue);

        /** ��������ֵ
        */
        bool SetProp(const char* szPropName, const char* szPropValue);
        bool SetProp(const char* szPropName, unsigned long dwValue, bool bHex=false);
        bool SetProp(const char* szPropName, int iValue, bool bHex=false);
        bool SetProp(const char* szPropName, float fValue);
        bool SetProp(const char* szPropName, bool bValue);

        /** ɾ������.
        */
        bool DelProp(const char* szPropName);

        /** �����.
        */
        NodePtr* AddItem(const char* szItemContent);

        /** ɾ����.
        */
        bool DelItem(const char* szItemContent);

        /** ����ӱ��.
        */
        NodePtr* AddTag(const char* szTagName);

        /** ɾ���ӱ��.
        *   @param szTagName �������
        *   @param bAll      Ϊtrue��ʾɾ������������һ�������ָ�����ֵı��, false��ʾֻɾ����һ��
        *   @return          ����true��ʾɾ���ɹ��������û���ҵ������ֵı��
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

    /** ����root.
    *   @param szRootname Root������
    *   @return ���root�Ѿ��������ͻ᷵��false�����򷵻�true
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
