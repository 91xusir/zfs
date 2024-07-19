

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
  RtsObject* m_NpcModel;     //模型信息
  std::string  iSerialNum;            //序号        
  std::string szNpcName;     //NPC名字
  std::string szNpcFuncName; //NPC功能名称
  std::string szScript;      //script内容
  std::string szDialog;      //对话内容
  std::string szAttention;   //备注
  std::string szVertion;     //版本
  std::string iModelId;      //模型编号
  RtgVertex3  m_NpcDir;      //NPC朝向
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
    virtual bool OnErrorMessage(const char* szMsg, int iType); // iType 0 错误, 1 必须退出的错误

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
    void*       m_pView;    // CSceneEditorView 的指针，要强制转换
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
    // 一般属性
    BOOL        m_bCheckSnapToGrid;
    DWORD       m_dwClearColor;
    DWORD       m_dwSunSpecular;
    DWORD       m_dwTerrainAmbient;
    float      m_fSunInclination;
    float      m_fSunRotation;
    BOOL        m_bFog;

    // 场景块属性
    char        m_szFileName[20];           // 场景块的文件名
    char        m_szBlockName[20];          // 场景块的名字

    DWORD       m_dwEnvironmentLighting;    // 环境光信息
    DWORD       m_dwSunLighting;            // 平行光信息

    BOOL        m_bPostProcessEffect;               // 是否打开后期处理
    BOOL        m_bPostProcessObjectHL;             // 是否打开物件高光
    float       m_fPostProcessEffectContrast;       // 后期处理对比度
    float       m_fPostProcessEffectIntensity;      // 后期处理强度
    float       m_fPostProcessObjectHLScale;        // 物件缩放
    DWORD       m_dwPostProcessObjectHLColorEmi;    // 自发光
    DWORD       m_dwPostProcessObjectHLColorDif;    // 漫反射
    DWORD       m_dwPostProcessObjectHLColorSpe;    // 高光
    float       m_fPostProcessObjectHLColorSpePower;// 高光强度

    DWORD       m_dwFogColor;               // 雾的颜色
    float       m_fFogNear;
    float       m_fFogFar;

    char        m_szBlockFlagTexture[20];   // 场景块标志贴图(用于进入场景时候的标记)

    float       m_fBackMusicStartTime;      // 从换音乐到播放的间隔时间
    float       m_fBackMusicDifferenceTime; // 背景音乐播放完成后间隔多长时间再次播放
    char        m_szBackMusic[20];          // 背景音乐文件

   
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