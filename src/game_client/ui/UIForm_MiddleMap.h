#pragma once
#include "ui/rtw_ui.h"

class CUIForm_MiddleMap {
   public:
    struct IMAGEINFO {
        RtwImage* pImage;
        RtwRect   rect;

        IMAGEINFO(RtwImage* image, RtwRect* rec) {
            pImage = image;
            rect   = *rec;
        }
    };

    // 对应的设置Hint提示所使用，当鼠标进入相应的矩形范围，那么就要显示string的内容
    struct PatchPoint  //寻路轨迹点信息
    {
        float x;
        float y;
        bool  beFound;
    };

    struct RECTINFO {
        string  RectInfo;
        RtwRect Rect;
    };

    CUIForm_MiddleMap(void);
    ~CUIForm_MiddleMap(void);
    void Show();
    void Hide();
    void OnHide(ui::RtwWidget* sender, RtwEventDelegate* e);
    void Init();
    void Refresh();
    bool IsVisible();
    void OnHideMiddleMap(RtwWidget* sender, RtwEventDelegate* e);
    void OnMoveTo(ui::RtwWidget* sender, RtwEventDelegate* e);
    void OnMoveToXY(ui::RtwWidget* sender, RtwEventDelegate* e);
    void OnMoveToNPC(ui::RtwWidget* sender, RtwEventDelegate* e);
    void OnAutoToNPC(ui::RtwWidget* sender, RtwEventDelegate* e);
    void OnMouseRDrag(int x, int y, int increaseX, int increaseY);
    void OnEvSelectList(ui::RtwWidget* sender, RtwEventDelegate* e);
    void OnShowTaskInfo(ui::RtwWidget* sender, RtwEventDelegate* e);
    void OnDrawMiddleMap(RtwWidget* sender, RtwEventDelegate* e);
    void OnSelectComboBox(RtwWidget* sender, RtwEventDelegate* e);
    void OnCheckButtonChanged(ui::RtwWidget* sender, RtwEventDelegate* e);
    //void OnNavigationButtonDown(RtwWidget* sender, RtwEventDelegate* e);
    void OnPreDraw();

    RtwWidget* GetFormWidget() {
        guard;
        return m_frmMiddleMap;
        unguard;
    }

    RtwWidget* GetRealMapWidget() {
        guard;
        return m_pfrmMap;
        unguard;
    }

    RtwWidget* GetFocusParentWidget(RtwWidget* son);

    void SetFirstOpenMap(bool bFirst) {
        guard;
        m_bOpenMapFirst = bFirst;
        unguard;
    }

    void SetMouseDownState(bool bIsDown) {
        guard;
        m_bMouseRDown = bIsDown;
        unguard;
    }

    void SetOnMoveto(int x, int y);

    void SetOnMovetoNULL() {
        guard;
        m_bNeedShowPoint = false;
        unguard;
    };

    void SetReferencePoint();

    bool GetMouseDownState() {
        guard;
        return m_bMouseRDown;
        unguard;
    }

    float* GetMovetoPoint() {
        guard;
        return m_fDesPos;
        unguard;
    }

    bool GetWetherNeedShowMovetFlag() {
        guard;
        return m_bNeedShowPoint;
        unguard;
    }

    void ShowPreview();
    void GetPatchPoint();
    bool PatchPointIsFound(PatchPoint& patchpoint);

   private:
    RtwWidget*   m_frmMiddleMap;  // 当前窗口
    RtwButton*   m_btnClose;
    RtwWidget*   m_pfrmMap;
    RtwComboBox* m_pcomComboBox;  // combobox
    RtwListBox*  m_pTaskListUI;
    /*RtwListBox*					m_pNPCListUI;*/
    RtwTree*        m_pNPCTree;
    RtwButton*      m_pbtnTask;
    RtwButton*      m_pbtnNPC;
    RtwWidget*      m_plbTasklabel;
    RtwButton*      m_pbtnUp;
    RtwButton*      m_pbtnDown;
    RtwButton*      m_pbtnLeft;
    RtwButton*      m_pbtnRight;
    RtwButton*      m_pbtnAutoRunToNPC;
    RtwButton*      m_pbtnShowTaskInfo;
    RtwTab*         m_ptabControl;
    RtwTextBox*     m_ptbxPosX;
    RtwTextBox*     m_ptbxPosY;
    RtwButton*      m_pbtnAutoMove;
    CUiCheckButton* m_pcheckButton;

    // 地图中用到的数据
    WORD m_wShowFlag;  // 地图的标志位，用于控制可以去显示什么
    long m_iAreaX;
    long m_iAreaY;

    // 渲染位置数据，当前屏幕的以及小地图的大小
    int                      m_iScreenX;        // 屏幕 X 坐标
    int                      m_iScreenY;        // 屏幕 Y 坐标
    int                      m_iScreenWidth;    // 屏幕宽度
    int                      m_iScreenHeight;   // 屏幕高度
    float                    m_fSceneWidth;     // 场景宽度
    float                    m_fMinimapWidth;   // 小地图宽度
    float                    m_fMinimapHeight;  // 小地图高度
    int                      m_iNpcCount;       // NPC 数量
    UIFormMiniMap::SNpcPoint m_ptNpcs[40];      // NPC 坐标数组
    UIFormMiniMap::SNpcPoint m_lastNpcHint;     // 上一个 NPC 提示

    bool  m_bMouseRDown;     // 鼠标右键是否按下
    float m_fRealMapPosX;    // 实际地图 X 坐标
    float m_fRealMapPosY;    // 实际地图 Y 坐标
    int   m_iRealMapWidth;   // 实际地图宽度
    int   m_iRealMapHeight;  // 实际地图高度

    int    m_iOffsetX;             // 地图 X 方向偏移量
    int    m_iOffsetY;             // 地图 Y 方向偏移量
    string m_strNowPlayerRealMap;  // 当前玩家所在的实际地图名称
    string m_strRealMapOld;        // 上一张实际地图名称
    string m_strNowShowMap;        // 当前显示的地图名称
    string m_strNextShowMap;       // 下一张显示的地图名称

    map<string, SSceneInfo>     m_mapMiddleMapName;       // 地图名称到场景信息的映射
    map<string, int>            m_mapStringToInt;         // 字符串到整数的映射
    map<string, IMAGEINFO*>     m_mapMiddleMapImageinfo;  // 地图名称到图像信息的映射
    map<string, STaskDesc*>     m_mapTaskInfo;            // 任务名称到任务描述的映射
    vector<vector<STaskDesc*>*> m_vectorTaskDesc;         // 任务描述的二维向量
    vector<vector<SNpc*>*>      m_vectorSNpc;             // NPC 的二维向量
    vector<STaskDesc*>          m_vectorCanAccept;        // 可接受任务的向量
    int                         m_iSelectMapNum;          // 选中的地图编号
    //	记录中地图上Hint相关信息，以及对应的范围
    vector<RECTINFO> m_vecRectInfo;
    bool
        m_bOpenMapFirst;  // 用于标注当前地图是否第一次打开，如果是第一次打开，那么将其设置为真，则在渲染中地图的时候将其置为玩家实际所处地图
    bool m_bChangeMap;
    // 中地图上相关label
    RtwLabel*          m_lbPlayer;
    RtwLabel*          m_lbPlayerpreview;
    RtwLabel*          m_lbPreviwRect;
    RtwLabel*          m_lbPet;
    RtwLabel*          m_lbAutoFlag;
    RtwLabel*          m_lbNowMousePos;
    RtwForm*           m_previewLabel;
    RtwComplexImage*   m_ImageAutoFlag;
    vector<RtwLabel*>  m_vectorNPCLabel;
    vector<RtwLabel*>  m_vectorNPCNameLabel;
    vector<RtwLabel*>  m_vectorTaskLabel;
    vector<RtwLabel*>  m_vectorMonsterLabel;
    vector<RtwLabel*>  m_vectorMonsterNameLabel;
    vector<RtwLabel*>  m_vectorTeamActor;
    vector<RtwLabel*>  m_vectorPatchPoint;
    vector<PatchPoint> m_vPatchPoint;
    // 寻路目的地
    float        m_fDesPos[2];       // 寻路目的地的坐标，通常为 X 和 Y 两个浮点数
    RtwImage*    m_pPreviewImage;    // 用于预览的图像对象
    RtsPathPath* m_pathNode;         // 路径节点，用于存储寻路的路径信息
    bool         m_isGetPatchPoint;  // 标记是否已经获取到寻路点

   public:
    bool m_bNeedShowPoint;
};
