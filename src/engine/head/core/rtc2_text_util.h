/**
* @file rtc2_text_util.h ���������ö�ȡ
* @author janhail
*/

namespace rt2_core {

/** �ı�������.
*   ���ڷ����򵥵������ַ���
*     - �����ַ����ĸ�ʽΪ "Item=Value[;Item=Value]"
*     - ����ItemΪ�ɴ�Сд��ĸ�����ֺ��»�����ɵ��ַ����������ִ�Сд
*       Value ����Ϊ�����ַ���������ڲ������ֺţ�������˫���Ű�����
*     - ˫�����ڲ����κ��ַ����ر����������������ո�����ַ����ڲ���Ҫ
*       ʹ��˫���ţ��������ӵ�����˫����[""]���棬˫������Ŀո�ᱻ����
*     - ���ֻ��Item��û��Value���ͻ���Ϊ���Item��ValueΪ��
*     - ����֮���÷ֺŷָ�
*     - ����ټ������õ�����:
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

    /** ���������ַ���
    *     �������ַ�����������ʽ���󣬾ʹӴ����Ժ��λ�ö�û������
    *     Ҳ����˵���ִ���֮ǰ�����û�����Ч
    */
    bool SetConfig(const char* szText);

    /// ����Item������ȡ��ֵ������Ҳ���������֣��ͷ���false
    bool GetValue(const char* szItem, const char** ppValue=NULL);

    /// ��������λ��(��0��ʼ)����Item���ֺ�ֵ�����λ�ô��󣬾ͷ���false
    bool GetItem(int iIdx, const char** ppItem, const char** ppValue=NULL);

    /// ȡ���ܹ���������Ŀ��
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
