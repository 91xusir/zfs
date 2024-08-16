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

    // ��Ӧ������Hint��ʾ��ʹ�ã�����������Ӧ�ľ��η�Χ����ô��Ҫ��ʾstring������
    struct PatchPoint  //Ѱ·�켣����Ϣ
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
    RtwWidget*   m_frmMiddleMap;  // ��ǰ����
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

    // ��ͼ���õ�������
    WORD m_wShowFlag;  // ��ͼ�ı�־λ�����ڿ��ƿ���ȥ��ʾʲô
    long m_iAreaX;
    long m_iAreaY;

    // ��Ⱦλ�����ݣ���ǰ��Ļ���Լ�С��ͼ�Ĵ�С
    int                      m_iScreenX;        // ��Ļ X ����
    int                      m_iScreenY;        // ��Ļ Y ����
    int                      m_iScreenWidth;    // ��Ļ���
    int                      m_iScreenHeight;   // ��Ļ�߶�
    float                    m_fSceneWidth;     // �������
    float                    m_fMinimapWidth;   // С��ͼ���
    float                    m_fMinimapHeight;  // С��ͼ�߶�
    int                      m_iNpcCount;       // NPC ����
    UIFormMiniMap::SNpcPoint m_ptNpcs[40];      // NPC ��������
    UIFormMiniMap::SNpcPoint m_lastNpcHint;     // ��һ�� NPC ��ʾ

    bool  m_bMouseRDown;     // ����Ҽ��Ƿ���
    float m_fRealMapPosX;    // ʵ�ʵ�ͼ X ����
    float m_fRealMapPosY;    // ʵ�ʵ�ͼ Y ����
    int   m_iRealMapWidth;   // ʵ�ʵ�ͼ���
    int   m_iRealMapHeight;  // ʵ�ʵ�ͼ�߶�

    int    m_iOffsetX;             // ��ͼ X ����ƫ����
    int    m_iOffsetY;             // ��ͼ Y ����ƫ����
    string m_strNowPlayerRealMap;  // ��ǰ������ڵ�ʵ�ʵ�ͼ����
    string m_strRealMapOld;        // ��һ��ʵ�ʵ�ͼ����
    string m_strNowShowMap;        // ��ǰ��ʾ�ĵ�ͼ����
    string m_strNextShowMap;       // ��һ����ʾ�ĵ�ͼ����

    map<string, SSceneInfo>     m_mapMiddleMapName;       // ��ͼ���Ƶ�������Ϣ��ӳ��
    map<string, int>            m_mapStringToInt;         // �ַ�����������ӳ��
    map<string, IMAGEINFO*>     m_mapMiddleMapImageinfo;  // ��ͼ���Ƶ�ͼ����Ϣ��ӳ��
    map<string, STaskDesc*>     m_mapTaskInfo;            // �������Ƶ�����������ӳ��
    vector<vector<STaskDesc*>*> m_vectorTaskDesc;         // ���������Ķ�ά����
    vector<vector<SNpc*>*>      m_vectorSNpc;             // NPC �Ķ�ά����
    vector<STaskDesc*>          m_vectorCanAccept;        // �ɽ������������
    int                         m_iSelectMapNum;          // ѡ�еĵ�ͼ���
    //	��¼�е�ͼ��Hint�����Ϣ���Լ���Ӧ�ķ�Χ
    vector<RECTINFO> m_vecRectInfo;
    bool
        m_bOpenMapFirst;  // ���ڱ�ע��ǰ��ͼ�Ƿ��һ�δ򿪣�����ǵ�һ�δ򿪣���ô��������Ϊ�棬������Ⱦ�е�ͼ��ʱ������Ϊ���ʵ��������ͼ
    bool m_bChangeMap;
    // �е�ͼ�����label
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
    // Ѱ·Ŀ�ĵ�
    float        m_fDesPos[2];       // Ѱ·Ŀ�ĵص����꣬ͨ��Ϊ X �� Y ����������
    RtwImage*    m_pPreviewImage;    // ����Ԥ����ͼ�����
    RtsPathPath* m_pathNode;         // ·���ڵ㣬���ڴ洢Ѱ·��·����Ϣ
    bool         m_isGetPatchPoint;  // ����Ƿ��Ѿ���ȡ��Ѱ·��

   public:
    bool m_bNeedShowPoint;
};
