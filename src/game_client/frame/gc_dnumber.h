// 动态的数字，用于头上跳动的血
#include <graph/rtg_vertex.h>
#ifndef GC_DNUMBER_H
#define GC_DNUMBER_H

// 定义不同的图片类型常量
const unsigned char PIC_NORMAL = 0;  // 普通
const unsigned char PIC_WATER  = 1;  // 水
const unsigned char PIC_FIRE   = 2;  // 火
const unsigned char PIC_POISON = 3;  // 毒
const unsigned char PIC_NOBAD  = 4;  // 不良
const unsigned char PIC_CHAR   = 5;  // 字符图片
const unsigned char PIC_MAX    = 6;  // 图片最大值（用于循环）

// 定义字符常量，用于不同的数字或效果
constexpr auto CHAR_JINGYAN     = "0";  // 经验;
constexpr auto CHAR_BANGGONG    = "1";  // 帮贡;
constexpr auto CHAR_BAOJIJIAXUE = "2";  // 爆击
constexpr auto CHAR_XISHOU      = "3";  // 吸收
constexpr auto CHAR_MENGONG     = "4";  // 猛攻？
constexpr auto CHAR_BAOJIDU     = "5";  // 爆击毒
constexpr auto CHAR_DIKANG      = "6";  // 抵抗
constexpr auto CHAR_SHENGWANG   = "7";  //
constexpr auto CHAR_BAOJIHUO    = "8";  // 爆击火
constexpr auto CHAR_GEDANG      = "9";  // 隔挡
constexpr auto CHAR_XIUWEI      = "a";  // 修为
constexpr auto CHAR_BAOJISHUI   = "b";  // 爆击水
constexpr auto CHAR_SHANBI      = "c";  // 闪避
constexpr auto CHAR_LINGLI      = "d";  // 灵力
constexpr auto CHAR_BAOJIPUTONG = "e";  // 爆击普通

// 定义不同的方向常量
const char DIR_MIDDLE  = 0;  // 中间
const char DIR_LEFTUP  = 1;  // 左上
const char DIR_RIGHTUP = 2;  // 右上
const char DIR_LEFT    = 3;  // 左
const char DIR_RIGHT   = 4;  // 右

// GcDynamicNumber 类，用于处理动态数字（例如头顶伤害数字）
class GcDynamicNumber {
   protected:
    // 最大节点数
    enum { MAX_NUM_NODE = 100 };

    // 结构体，用于存储数字节点信息
    struct SNumNode {
        SNumNode() {
            pBindNode = NULL;
            CM_MEMPROTECTOR(szNum, 16)  // 内存保护
        }

        ~SNumNode() {
            CM_MEMUNPROTECTOR(szNum)  // 内存取消保护
        }

        float      fTime;     // 数字存在时间
        RtgVertex3 vPos;      // 数字位置
        CM_MEMDEF(szNum, 16)  // 数字内容
        DWORD     dwColor;    // 数字颜色
        char      cType;      // 类型
        bool      spec;       // 是否特殊效果
        char      cDir;       // 方向
        float     fScale;     // 缩放
        SNumNode* pBindNode;  // 绑定节点
    };

    // 最大字符数
    enum { MAX_CHAR = 16 };

    // 结构体，用于存储字符 UV 坐标
    struct SCharUV {
        RtgVertex2 uvs[6];  // UV 坐标
    };

   public:
    // 构造函数
    GcDynamicNumber();
    // 析构函数
    ~GcDynamicNumber();

    // 初始化一次（可能是创建或重置）
    void InitOnce();
    // 清理一次（可能是销毁或重置）
    void ClearOnce();

    // 添加字符串到数字列表中
    void AddString(const char* szString, const char* szNumber, const RtgVertex3& vPos,
                   float fScale = 1.f, bool spec = false, char cDir = DIR_MIDDLE,
                   char type = PIC_NORMAL);
    void AddString(const char* szString, const RtgVertex3& vPos, float fScale = 1.f,
                   bool spec = false, char cDir = DIR_MIDDLE, char type = PIC_NORMAL);
    void AddString(const char* szString, const RtgVertex3& vPos, DWORD dwColor = 0xFFFFFFFF,
                   float fScale = 1.f, bool spec = false, char type = PIC_NORMAL);
    // 渲染数字
    void Render(RtgCamera& inCamera, RtgDevice& inDevice, float fSecond);

   private:
    // 设置数字纹理
    void SetNumberTexture(int iXCnt, int iYCnt, const char* szChars);
    // 设置字符纹理
    void SetCharTexture(int iXCnt, int iYCnt, const char* szChars);

   private:
    float      m_fSpeed;              // 移动速度
    float      m_fTime;               // 时间
    int        m_iCnt;                // 数字计数
    int        m_iCntChar;            // 字符计数
    int        m_iFirst;              // 第一个节点索引
    int        m_iLast;               // 最后一个节点索引
    float      m_fSizeX;              // 面板宽度
    float      m_fSizeY;              // 面板高度
    RtgVertex3 m_pPaneVertex[6];      // 面板顶点
    float      m_fSizeCharX;          // 字符宽度
    float      m_fSizeCharY;          // 字符高度
    RtgVertex3 m_pPaneVertexChar[6];  // 字符面板顶点
    SNumNode   m_Node[MAX_NUM_NODE];  // 数字节点数组
    RtgShader  m_Shader[PIC_MAX];     // 纹理着色器

    SCharUV    m_UVs[MAX_CHAR];       // 字符 UV 坐标
    SCharUV*   m_pUVs[128];           // 字符 UV 坐标指针数组

    SCharUV    m_UVsChar[MAX_CHAR];   // 字符 UV 坐标
    SCharUV*   m_pUVsChar[128];       // 字符 UV 坐标指针数组
};

#endif  // GC_DNUMBER_H

/*----------------------------------------------------------------------------
-   The End.
----------------------------------------------------------------------------*/
