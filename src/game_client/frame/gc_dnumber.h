// ��̬�����֣�����ͷ��������Ѫ
#include <graph/rtg_vertex.h>
#ifndef GC_DNUMBER_H
#define GC_DNUMBER_H

// ���岻ͬ��ͼƬ���ͳ���
const unsigned char PIC_NORMAL = 0;  // ��ͨ
const unsigned char PIC_WATER  = 1;  // ˮ
const unsigned char PIC_FIRE   = 2;  // ��
const unsigned char PIC_POISON = 3;  // ��
const unsigned char PIC_NOBAD  = 4;  // ����
const unsigned char PIC_CHAR   = 5;  // �ַ�ͼƬ
const unsigned char PIC_MAX    = 6;  // ͼƬ���ֵ������ѭ����

// �����ַ����������ڲ�ͬ�����ֻ�Ч��
constexpr auto CHAR_JINGYAN     = "0";  // ����;
constexpr auto CHAR_BANGGONG    = "1";  // �ﹱ;
constexpr auto CHAR_BAOJIJIAXUE = "2";  // ����
constexpr auto CHAR_XISHOU      = "3";  // ����
constexpr auto CHAR_MENGONG     = "4";  // �͹���
constexpr auto CHAR_BAOJIDU     = "5";  // ������
constexpr auto CHAR_DIKANG      = "6";  // �ֿ�
constexpr auto CHAR_SHENGWANG   = "7";  //
constexpr auto CHAR_BAOJIHUO    = "8";  // ������
constexpr auto CHAR_GEDANG      = "9";  // ����
constexpr auto CHAR_XIUWEI      = "a";  // ��Ϊ
constexpr auto CHAR_BAOJISHUI   = "b";  // ����ˮ
constexpr auto CHAR_SHANBI      = "c";  // ����
constexpr auto CHAR_LINGLI      = "d";  // ����
constexpr auto CHAR_BAOJIPUTONG = "e";  // ������ͨ

// ���岻ͬ�ķ�����
const char DIR_MIDDLE  = 0;  // �м�
const char DIR_LEFTUP  = 1;  // ����
const char DIR_RIGHTUP = 2;  // ����
const char DIR_LEFT    = 3;  // ��
const char DIR_RIGHT   = 4;  // ��

// GcDynamicNumber �࣬���ڴ���̬���֣�����ͷ���˺����֣�
class GcDynamicNumber {
   protected:
    // ���ڵ���
    enum { MAX_NUM_NODE = 100 };

    // �ṹ�壬���ڴ洢���ֽڵ���Ϣ
    struct SNumNode {
        SNumNode() {
            pBindNode = NULL;
            CM_MEMPROTECTOR(szNum, 16)  // �ڴ汣��
        }

        ~SNumNode() {
            CM_MEMUNPROTECTOR(szNum)  // �ڴ�ȡ������
        }

        float      fTime;     // ���ִ���ʱ��
        RtgVertex3 vPos;      // ����λ��
        CM_MEMDEF(szNum, 16)  // ��������
        DWORD     dwColor;    // ������ɫ
        char      cType;      // ����
        bool      spec;       // �Ƿ�����Ч��
        char      cDir;       // ����
        float     fScale;     // ����
        SNumNode* pBindNode;  // �󶨽ڵ�
    };

    // ����ַ���
    enum { MAX_CHAR = 16 };

    // �ṹ�壬���ڴ洢�ַ� UV ����
    struct SCharUV {
        RtgVertex2 uvs[6];  // UV ����
    };

   public:
    // ���캯��
    GcDynamicNumber();
    // ��������
    ~GcDynamicNumber();

    // ��ʼ��һ�Σ������Ǵ��������ã�
    void InitOnce();
    // ����һ�Σ����������ٻ����ã�
    void ClearOnce();

    // ����ַ����������б���
    void AddString(const char* szString, const char* szNumber, const RtgVertex3& vPos,
                   float fScale = 1.f, bool spec = false, char cDir = DIR_MIDDLE,
                   char type = PIC_NORMAL);
    void AddString(const char* szString, const RtgVertex3& vPos, float fScale = 1.f,
                   bool spec = false, char cDir = DIR_MIDDLE, char type = PIC_NORMAL);
    void AddString(const char* szString, const RtgVertex3& vPos, DWORD dwColor = 0xFFFFFFFF,
                   float fScale = 1.f, bool spec = false, char type = PIC_NORMAL);
    // ��Ⱦ����
    void Render(RtgCamera& inCamera, RtgDevice& inDevice, float fSecond);

   private:
    // ������������
    void SetNumberTexture(int iXCnt, int iYCnt, const char* szChars);
    // �����ַ�����
    void SetCharTexture(int iXCnt, int iYCnt, const char* szChars);

   private:
    float      m_fSpeed;              // �ƶ��ٶ�
    float      m_fTime;               // ʱ��
    int        m_iCnt;                // ���ּ���
    int        m_iCntChar;            // �ַ�����
    int        m_iFirst;              // ��һ���ڵ�����
    int        m_iLast;               // ���һ���ڵ�����
    float      m_fSizeX;              // �����
    float      m_fSizeY;              // ���߶�
    RtgVertex3 m_pPaneVertex[6];      // ��嶥��
    float      m_fSizeCharX;          // �ַ����
    float      m_fSizeCharY;          // �ַ��߶�
    RtgVertex3 m_pPaneVertexChar[6];  // �ַ���嶥��
    SNumNode   m_Node[MAX_NUM_NODE];  // ���ֽڵ�����
    RtgShader  m_Shader[PIC_MAX];     // ������ɫ��

    SCharUV    m_UVs[MAX_CHAR];       // �ַ� UV ����
    SCharUV*   m_pUVs[128];           // �ַ� UV ����ָ������

    SCharUV    m_UVsChar[MAX_CHAR];   // �ַ� UV ����
    SCharUV*   m_pUVsChar[128];       // �ַ� UV ����ָ������
};

#endif  // GC_DNUMBER_H

/*----------------------------------------------------------------------------
-   The End.
----------------------------------------------------------------------------*/
