
// 动态的数字，用于头上跳动的血
#ifndef GC_DNUMBER_H
#define GC_DNUMBER_H

const unsigned char PIC_NORMAL		= 0;
const unsigned char PIC_WATER		= 1;
const unsigned char PIC_FIRE		= 2;
const unsigned char PIC_POISON		= 3;
const unsigned char PIC_NOBAD		= 4;
const unsigned char PIC_CHAR		= 5;
const unsigned char PIC_MAX			= 6;

#define CHAR_JINGYAN		"0"
#define CHAR_BANGGONG		"1"
#define CHAR_BAOJIJIAXUE	"2"
#define CHAR_XISHOU			"3"
#define CHAR_MENGONG		"4"
#define CHAR_BAOJIDU		"5"
#define CHAR_DIKANG			"6"
#define CHAR_SHENGWANG		"7"
#define CHAR_BAOJIHUO		"8"
#define CHAR_GEDANG			"9"
#define CHAR_XIUWEI			"a"
#define CHAR_BAOJISHUI		"b"
#define CHAR_SHANBI			"c"
#define CHAR_LINGLI			"d"
#define CHAR_BAOJIPUTONG	"e"

const char DIR_MIDDLE	= 0;
const char DIR_LEFTUP	= 1;
const char DIR_RIGHTUP	= 2;
const char DIR_LEFT		= 3;
const char DIR_RIGHT	= 4;

class GcDynamicNumber
{
protected:
    enum {MAX_NUM_NODE = 100};
    struct SNumNode
    {
		SNumNode()
		{
			pBindNode = NULL; 
			CM_MEMPROTECTOR(szNum, 16)
		}
		~SNumNode()
		{
			CM_MEMUNPROTECTOR(szNum)
		}
        float       fTime;
        RtgVertex3  vPos;
        CM_MEMDEF(szNum, 16)
		DWORD       dwColor;
		char		cType;
		bool        spec;
		char		cDir;
		float		fScale;
		SNumNode	*pBindNode;
    };
    enum {MAX_CHAR = 16};
    struct SCharUV
    {
        RtgVertex2  uvs[6];
    };

public:
    GcDynamicNumber();
    ~GcDynamicNumber();

    void InitOnce();
    void ClearOnce();

	void AddString(const char* szString, const char* szNumber, const RtgVertex3& vPos,float fScale=1.f, bool spec=false, char cDir = DIR_MIDDLE, char type = PIC_NORMAL );
	void AddString(const char* szString, const RtgVertex3& vPos,float fScale=1.f, bool spec=false, char cDir = DIR_MIDDLE, char type = PIC_NORMAL );
    void AddString(const char* szString, const RtgVertex3& vPos, DWORD dwColor=0xFFFFFFFF, float fScale=1.f, bool spec=false, char type = PIC_NORMAL );
    void Render(RtgCamera& inCamera, RtgDevice& inDevice, float fSecond);

private:
	void SetNumberTexture(int iXCnt, int iYCnt, const char* szChars);
	void SetCharTexture(int iXCnt, int iYCnt, const char* szChars);

private:
    float       m_fSpeed;
    float       m_fTime;
    int         m_iCnt;
    int         m_iCntChar;
    int         m_iFirst;
    int         m_iLast;
    float       m_fSizeX;
    float       m_fSizeY;
	RtgVertex3  m_pPaneVertex[6];
	float       m_fSizeCharX;
	float       m_fSizeCharY;
	RtgVertex3  m_pPaneVertexChar[6];
    SNumNode    m_Node[MAX_NUM_NODE];
	RtgShader   m_Shader[PIC_MAX];
    SCharUV     m_UVs[MAX_CHAR];
	SCharUV*    m_pUVs[128];
	SCharUV     m_UVsChar[MAX_CHAR];
	SCharUV*    m_pUVsChar[128];

};
#endif//GC_DNUMBER_H
/*----------------------------------------------------------------------------
-   The End.
----------------------------------------------------------------------------*/
