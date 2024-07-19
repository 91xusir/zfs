

#ifndef _EditorFrame_H_
#define _EditorFrame_H_

class RtsObject;
class CMainFrame;
class CBrusherObject;
struct RtsAreaHelper;

#include <list>
using namespace std;



//-----jiang modify-----//
struct BrushNpcInfo
{
  RtsObject* m_NpcModel;     //ģ����Ϣ
  std::string  iSerialNum;            //���        
  std::string szNpcName;     //NPC����
  std::string szNpcFuncName; //NPC��������
  std::string szScript;      //script����
  std::string szDialog;      //�Ի�����
  std::string szAttention;   //��ע
  std::string szVertion;     //�汾
  std::string iModelId;      //ģ�ͱ��
  RtgVertex3  m_NpcDir;      //NPC����
};

//--------------------//





class CEditorFrame : public CRtgAppFrame
{
public:
	RT_DECLARE_DYNCREATE(CEditorFrame, CRtgAppFrame, NULL, "")
    CEditorFrame();
    virtual ~CEditorFrame();
    virtual bool OnFrameInit();
    virtual void OnFrameClose();
    virtual void OnRender();
    virtual void OnFrameMove(float fDifSecond);
    virtual bool OnErrorMessage(const char* szMsg, int iType); // iType 0 ����, 1 �����˳��Ĵ���

    virtual void OnMouseLDown(int iButton, int x, int y);
    virtual void OnMouseLDrag(int iButton, int x, int y, int increaseX, int increaseY);
    virtual void OnMouseLUp(int iButton, int x, int y);

    virtual void OnMouseRDown(int iButton, int x, int y);
    virtual void OnMouseRDrag(int iButton, int x, int y, int increaseX, int increaseY);
    virtual void OnMouseRUp(int iButton, int x, int y);

    virtual void OnMouseMDown(int iButton, int x, int y);
    virtual void OnMouseMDrag(int iButton, int x, int y, int increaseX, int increaseY);
    virtual void OnMouseMUp(int iButton, int x, int y);

    virtual void OnKeyDown(int iButton, int iKey);
    virtual void OnMouseWheel(int iButton, long vDelta);

    void SetDefaultCamera();

    void UpdateProperty();
    void UpdateAreaList();
    void AddArea(RtsAreaHelper* pArea);
    void RemoveArea(RtsAreaHelper* pArea);
    void SelectArea(RtsAreaHelper* pArea);

protected:
    void OnBrushBegin(int iButton, int x, int y);
    void OnBrushMove(int iButton, int x, int y);
    void LoadConfigFile(const char* szFilename);
    void SaveConfigFile(const char* szFilename);

#ifdef DO_CONSOLE
    virtual bool OnConsoleCommand(const char* szCommand);
    bool    m_bShowStatChar;
    bool    m_bShowStatScene;
    bool    m_bShowStatAudio;
#endif

public:
    void*       m_pView;    // CSceneEditorView ��ָ�룬Ҫǿ��ת��
    CMainFrame* m_pMainFrame;
    RtgShader   m_ShaderNull;

    RtgLight    m_Light;
    int         m_iCameraMode;
    RtgCameraEditor m_LastEditorCamera;
    RtgCameraEditor m_LastPerspectiveCamera;
    int         m_iFillTerrain;
    RtgVertex2  m_iCameraPos;

    bool        m_bDrawCharPos;
    RtgVertex3  m_vCharPos;
    RtgVertex3  m_vLastStart, m_vLastEnd;

    // Brush
    int         m_iLastBrushTileX;
    int         m_iLastBrushTileY;
    int         m_iLastBrushMouseBtn;
    CBrushTool  m_BrushTool;
	//-------------jiang add------------//
   
	bool m_bAddNpc;
	list<BrushNpcInfo> m_NpcBrushList;
    //----------------------------------//
    // һ������
    BOOL        m_bCheckSnapToGrid;
    DWORD       m_dwClearColor;
    DWORD       m_dwSunSpecular;
    DWORD       m_dwTerrainAmbient;
    float      m_fSunInclination;
    float      m_fSunRotation;
    BOOL        m_bFog;

    // ����������
    char        m_szFileName[20];           // ��������ļ���
    char        m_szBlockName[20];          // �����������

    DWORD       m_dwEnvironmentLighting;    // ��������Ϣ
    DWORD       m_dwSunLighting;            // ƽ�й���Ϣ

    BOOL        m_bPostProcessEffect;               // �Ƿ�򿪺��ڴ���
    BOOL        m_bPostProcessObjectHL;             // �Ƿ������߹�
    float       m_fPostProcessEffectContrast;       // ���ڴ���Աȶ�
    float       m_fPostProcessEffectIntensity;      // ���ڴ���ǿ��
    float       m_fPostProcessObjectHLScale;        // �������
    DWORD       m_dwPostProcessObjectHLColorEmi;    // �Է���
    DWORD       m_dwPostProcessObjectHLColorDif;    // ������
    DWORD       m_dwPostProcessObjectHLColorSpe;    // �߹�
    float       m_fPostProcessObjectHLColorSpePower;// �߹�ǿ��

    DWORD       m_dwFogColor;               // �����ɫ
    float       m_fFogNear;
    float       m_fFogFar;

    char        m_szBlockFlagTexture[20];   // �������־��ͼ(���ڽ��볡��ʱ��ı��)

    float       m_fBackMusicStartTime;      // �ӻ����ֵ����ŵļ��ʱ��
    float       m_fBackMusicDifferenceTime; // �������ֲ�����ɺ����೤ʱ���ٴβ���
    char        m_szBackMusic[20];          // ���������ļ�

   
    DWORD       m_dwCurLightColor;
    double      m_fCurLightRange;
    double      m_fCurLightConstantAttenuation;
    double      m_fCurLightLinearAttenuation;
    double      m_fCurLightQuadraticAttenuation;

    float       m_fCameraDefaultYaw;
    float       m_fCameraDefaultDegree;
    float       m_fCameraDefaultDistance;


    // for Test TakeGeometry
    RtgVertex3  m_pTakeGeometryVertices[3000];
    int         m_iTakeGeometryVertexNum;
    short       m_pTakeGeometryIndices[3000];
    int         iTakeGeometryIndexNum;
    // Test TakeGeometry end
};

#endif