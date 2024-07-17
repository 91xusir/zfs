/**
* @file rtc2_text_util.h 命令行配置读取
* @author janhail
*/

namespace rt2_core {

/** 文本配置类.
*   用于分析简单的配置字符串
*     - 配置字符串的格式为 "Item=Value[;Item=Value]"
*     - 其中Item为由大小写字母、数字和下划线组成的字符串，不区分大小写
*       Value 可以为任意字符串，如果内部包含分号，就请用双引号包起来
*     - 双引号内部的任何字符都回被保留下来，包括空格，如果字符串内部需要
*       使用双引号，就用连接的两个双引号[""]代替，双引号外的空格会被忽略
*     - 如果只有Item而没有Value，就会认为这个Item的Value为空
*     - 配置之间用分号分割
*     - 下面举几个配置的例子:
*          - IP=192.168.0.251;Port=6666;Username=Janhail;Password="eQia;E2a#"
*          - Key = 25 ; UserString = "My name is ""Janhail Luo"" ! " ;Show=1
*          - IsWin32;IsMFC;IsPython;Version=1.2
*/
class RtTextConfig
{
public:
    RtTextConfig();
    RtTextConfig(const char* szText);

    ~RtTextConfig();
    void Clear();

    /** 设置配置字符串
    *     如果这个字符串分析出格式错误，就从错误以后的位置都没有内容
    *     也就是说出现错误之前的配置还是有效
    */
    bool SetConfig(const char* szText);

    /// 根据Item名字来取得值，如果找不到这个名字，就返回false
    bool GetValue(const char* szItem, const char** ppValue=NULL);

    /// 根据索引位置(从0开始)来找Item名字和值，如果位置错误，就返回false
    bool GetItem(int iIdx, const char** ppItem, const char** ppValue=NULL);

    /// 取得总共的配置项目数
    int GetCount()      { return m_iCount; }

private:
    char*   m_pCurPos;
    bool    m_bError;
    bool    m_bNullValue;
    void GetInit();
    inline bool IsIDChar(char c)
    {
        return ((c>='a' && c<='z') || (c>='A' && c<='Z') || (c>='0' && c<='9') || (c=='_'));
    }
    inline bool IsSpace(char c)
    {
        return (c==' ' || c=='\t' || c==10 || c==13 || c=='\n');
    }
    inline void SkipSpace()
    {
        while (IsSpace(*m_pCurPos))
        {
            m_pCurPos ++;
        }
    }
    const char* GetNextItem(bool bFlag);
    const char* GetNextValue(bool bFlag);

private:
    struct SItem
    {
        const char* pItem;
        const char* pValue;
    };

    char*   m_pConfigText;
    SItem*  m_pItem;
    int     m_iCount;

};

int SaveToString(char* szStr, char  v);
int SaveToString(char* szStr, short v);
int SaveToString(char* szStr, int   v);
int SaveToString(char* szStr, long  v);
int SaveToString(char* szStr, BYTE  v);
int SaveToString(char* szStr, WORD  v);
int SaveToString(char* szStr, UINT  v);
int SaveToString(char* szStr, DWORD v);
int SaveToString(char* szStr, float v);
int SaveToString(char* szStr, const char* v);

int LoadFromString(char* szStr, char  &v);
int LoadFromString(char* szStr, short &v);
int LoadFromString(char* szStr, int   &v);
int LoadFromString(char* szStr, long  &v);
int LoadFromString(char* szStr, BYTE  &v);
int LoadFromString(char* szStr, WORD  &v);
int LoadFromString(char* szStr, UINT  &v);
int LoadFromString(char* szStr, DWORD &v);
int LoadFromString(char* szStr, float &v);
int LoadFromString(char* szStr, char* v);
std::string LoadFromString(char* szStr);

} // namespace rt2_core

/*------------------------------------------------------------------------
-   The End.
------------------------------------------------------------------------*/
