

#ifndef GC_BASE_ACTOR_H
#define GC_BASE_ACTOR_H
//#include "game_states_machine.h"
#include "gc_actor_graph.h"
#include "scene/rts_path_find.h"
//#include "gc_player_items.h"

#include <vector>
#include <unordered_map>
#include <string>
#include <list>

using EXT_SPACE::unordered_map;
using std::list;
using std::string;
using std::vector;

//#include "character/actor.h"

class GcActor;
struct SActor;
struct SItem;

class GcBaseActor;
class CTrumpShow;
class CWeaponShow;

//
//	class GcRenderActorGraph
//
class GcRenderActorBase : public RtsRenderAgentShadow {
    RT_DECLARE_DYNAMIC(GcRenderActorBase, RtsRenderAgentShadow, 0, "gc")
   public:
    GcRenderActorBase() {}

    virtual ~GcRenderActorBase();

    void SetGraph(GcBaseActor* pGraph) { m_pGraph = pGraph; }

    virtual void Render(RtgCamera& inCamera, RtgDevice& inDevice, RTGRenderMask eMask);
    virtual void RenderHighLight(RtgCamera& inCamera, RtgDevice& inDevice, RtgVertex3& color,
                                 RtgVertex3& scale);
    virtual void AgentRenderShader();
    virtual CRT_ActorInstance* GetActor();

   private:
    GcBaseActor* m_pGraph;

   public:
    static int s_iShadowQuality;  // 0 ����Ӱ�� 1 ȫ������Ȧ�� 2 �Լ����޶�̬����Ȧ�� 3 ȫ����̬
};

// ������ʾ����̾�Ż����ʺţ�
#define TASK_OVERHEAD_QUERY_PERIOD 2000

class GcStateOverHead : public RtsRenderAgent {
    RT_DECLARE_DYNAMIC(GcStateOverHead, RtsRenderAgent, 0, "gc")
   public:
    GcStateOverHead();
    virtual ~GcStateOverHead();
    virtual void Render(RtgCamera& inCamera, RtgDevice& inDevice, RTGRenderMask eMask);
    virtual void RenderHighLight(RtgCamera& inCamera, RtgDevice& inDevice, RtgVertex3& color,
                                 RtgVertex3& scale);
    void         OnAfterScenePreRender(RtgCamera& inCamera, RtScene* pScene);
    void         Run(float fSecond);

    void RefreshGraph(ETaskOverHead state);
    void Clear();

    static void NetSend_c2r_query_task_overhead(DWORD npcOID);

   public:
    GcActor* m_pActor;

   private:
    CRT_ActorInstance* m_pTaskGraph1;  // ��̾��
    CRT_ActorInstance* m_pTaskGraph2;  // �ʺ�
    CRT_ActorInstance* m_pTaskGraph3;  // �ʺ�

    CRT_ActorInstance* m_pRender;  // Ӧ����Ⱦ��ͼ��ָ��

    DWORD m_dwLastQueryTime;
};

//�������ͷ����̬��ʾ
class GcActorOverHead : public RtsRenderAgent {
    RT_DECLARE_DYNAMIC(GcActorOverHead, RtsRenderAgent, 0, "gc")
   public:
    GcActorOverHead();
    virtual ~GcActorOverHead();
    virtual void Render(RtgCamera& inCamera, RtgDevice& inDevice, RTGRenderMask eMask);
    virtual void RenderHighLight(RtgCamera& inCamera, RtgDevice& inDevice, RtgVertex3& color,
                                 RtgVertex3& scale);
    void         OnAfterScenePreRender(RtgCamera& inCamera, RtScene* pScene);
    void         Run(float fSecond);

    void NewActorOverHead(std::string strHint);
    void Clear();

   public:
    GcActor* m_pActor;

   private:
    CRT_ActorInstance* m_pActorGraph;  // NPCͷ����ʾ

    CRT_ActorInstance* m_pRender;  // Ӧ����Ⱦ��ͼ��ָ��
};

class GcBaseActor : public CRT_PoseNotify {
   public:
    friend class GcActor;

    enum EStates {
        //�ƶ�״̬
        NULL_STATE,    //��״̬
        STAND_STATE,   //վ��
        IDLE_STATE,    //���ж���
        GUARD_STATE,   //ս������
        ZAZEN_STATE,   //����
        CHANT_STATE,   //����
        WALK_STATE,    //����
        ATTACK_STATE,  //������
        HURT_STATE,    //����

        READY_BORN_STATE,  //׼������
        BORN_STATE,        //���ڳ���
        DEAD_STATE,        //����״̬

        //���״̬
        NO_RIDE_STATE,  //û�����״̬
        RIDE_STATE,     //����״̬
        BY_RIDE_STATE,  //����״̬
    };

    enum EPoses {
        POSE_NONE = 0,   // 0�����ƣ�Ĭ��״̬
        POSE_STAND,      // 1վ������
        POSE_GUARD,      // 2ս����������
        POSE_IDLE,       // 3����״̬
        POSE_TALK,       // 4�Ի�����
        POSE_WALK,       // 5����
        POSE_RUN,        // 6���ܣ�׷��
        POSE_ATTACK,     // 7����
        POSE_BASH,       // 8��������
        POSE_HURT,       // 9����״̬
        POSE_CHANT,      // 10��������
        POSE_REFINE,     // 11��������
        POSE_MAKE,       // 12������Ʒ
        POSE_ZAZEN,      // 13����
        POSE_DIG,        // 14�ھ���
        POSE_GATHER,     // 15��ҩ����
        POSE_SCALP,      // 16��Ƥ����
        POSE_PULL,       // 17�������
        POSE_DOWN,       // 18�ջ�����
        POSE_DEAD,       // 19����
        POSE_BODY,       // 20����ʬ��
        POSE_RELIVE,     // 21����
        POSE_RIDE,       // 22���
        POSE_BORN,       // 23����״̬
        POSE_FUNACTION,  // 24���⹦�ܶ���
        POSE_MAX,        // 25���ֵ��ͨ������ȷ�����Ƶ�����
    };

    GcBaseActor(GcActor* vpMaster);
    virtual ~GcBaseActor();

    bool CreateGraphData(SCreModel* vpModel, short HeadModelID);  //����ͼ��
    bool DestroyGraphData();                                      //����ͼ��

    void Run(float fSecond);                                //�߼�ѭ��
    void Render(RtgDevice& inDevice, RTGRenderMask eMask);  //��Ⱦ
    void RenderHighLight(RtgCamera& inCamera, RtgDevice& inDevice, RtgVertex3& color,
                         RtgVertex3& scale);

    const char* GetLastPoseName() { return m_szLastPoseName; }

    bool        PlayPose(char vpPoseName[], bool vLoop = false);
    const char* PlayPose(EPoses vPose, bool vLoop = false, SSkill* pSkill = NULL,
                         float fSpeed = 1.f);
    const char* OldPlayPose(EPoses vPoseID, bool vLoop, SSkill* pSkill, float fSpeed);
    const char* GetPoseByWeapon(EPoses Pose, SItemID& item1, SItemID& item2);
    char*       GetPoseByWeapon(EPoses Pose, SItemID& item);
    char*       GetPoseByNPC(EPoses Pose);

    void FaceTo(float vX, float vY);  //�泯��
    void UpdateDirection();           //���·���
    bool RemoveSelf();                //�ӵ�ͼ���Ƴ��Լ�

    void SetMoveSpeed(float fSpeed);
    bool MoveToTerrainNow(int iTileX, int iTileY, bool bFindSpace = false);
    bool MoveToTerrainNow(float fX, float fY, bool bFindSpace = false);
    bool MoveToPosition(float fX, float fY, float fDistance, float fNeedTime, float fDiffTime,
                        bool bTarget);

    bool IsMoveTo() { return m_bMoveToPositionNewMove != 0; }

    const RtgMatrix16* GetMatrix() { return &m_Matrix; }

    GcActorGraph* GetGraph() { return &mGraph; }

    EStates GetState() { return m_CurrentState; }

    EStates GetRideState() { return m_RideState; }

    bool HitTest(RtgVertex3& vOrg, RtgVertex3& vDir, float& fOutMinDist, bool bHitTestTouch);

    //�����Ļص�����
    virtual void OnPoseBegin(SRT_Pose* pose);
    virtual void OnPoseEnd(SRT_Pose* pose);
    virtual void OnPoseEvent(SRT_Pose* pose, SRT_PoseEvent* event);
    virtual void OnPoseBreak(SRT_Pose* oldPose, SRT_Pose* newPose);
    bool         IsPlayer();

    void PlayEffect(const char* file, bool link, const char* slot, RtgVertex3 offset,
                    bool loop = false, float speed = 1.0f, const char* szPosename = NULL);
    void PlayDropItemEffect(const char* file = "skill_hit1", const char* link = "root");
    void StopDropItemEffect();
    void StopEffect(const char* file);
    bool FindEffect(const char* file);
    void ClearEffect();

    GcRenderActorBase* GetRender() { return &m_Render; }

    void Update();                                              //��λ�ø��µ�ͼ�β���
    bool SetGraph(short HeadModelID, const char* vpActorName);  //����ͼ��
    void UnLoadAllSkin();

    RtsPathPath* GetPathNode() { return m_pPathNode; }  //����·�������

    bool IsDrawPathLine() { return m_isDrawPathLine ? true : false; }

    void SetMatrix(const RtgMatrix16& _m16) {
        m_Matrix = _m16;
        if (mGraph.p())
            mGraph.p()->SetMatrix(_m16);
    }

    void SetMatrix_Row(int _row, const RtgVertex3& _v3) {
        if (_row > 3)
            return;
        m_Matrix.SetRow(_row, _v3);
        if (mGraph.p())
            mGraph.p()->SetMatrix_Row(_row, _v3);
    }

    void SetMatrix_Col(int _col, const RtgVertex3& _v3) {
        if (_col > 3)
            return;
        m_Matrix.SetCol(_col, _v3);
        if (mGraph.p())
            mGraph.p()->SetMatrix_Col(_col, _v3);
    }

    void OnSetWeaponShow(bool bNeed);

#if DO_CONSOLE
    void DebugInfo(int vX, int vY);
#endif

   protected:
    bool SetPosition(int vX, int vY);
    bool SetPosition(float vX, float vY);

    RtgMatrix16  m_Matrix;     //λ�úͷ���
    RtsPathPath* m_pPathNode;  //·����Ķ���

    RtsPathFind* m_pPathFind;            //tim.yang  Ѱ·
    RtgMatrix16  m_drawLineStartMatrix;  //tim.yang
    long         m_isDrawPathLine;       //tim.yang  �Ƿ����·��

    GcActorGraph      mGraph;    //ͼ��
    GcRenderActorBase m_Render;  //��Ⱦ�Ĵ���

    std::string m_strComplexHead;
    std::string m_strComplexBody;
    std::string m_strComplexHand;
    std::string m_strComplexFoot;
    std::string m_strComplexPants;

   private:
    bool SetGraph(SCreModel* vpModel, short HeadModelID, const char* vpActorName);  //����ͼ��

    void UnRide(GcActor*);  //����

    bool FindPath(int vX, int vY, bool vIgnoreActor = false,
                  bool vIgnoreTarget = false);  //Ѱ��·��
    bool AddSelf(int vX, int vY);               //����ͼ������Լ�
    bool AddSelf(float vX, float vY);           //����ͼ������Լ�
    bool AddSelfFind(int vX, int vY);           //����ͼ������Լ�(�Զ����ҿ�λ)
    bool AddSelfFind(float vX, float vY);       //����ͼ������Լ�(�Զ����ҿ�λ)

    bool IsPosition(float vX, float vY);                              //�Ƿ��ڸõ���
    bool IsPosition(int vX, int vY);                                  //�Ƿ��ڸø�����
    void Point2Tile(float vX, float vY, int& vrTileX, int& vrTileY);  //�ӵ�任����������
    void Tile2Point(int vTileX, int vTileY, float& vrX, float& vrY, float& vrZ);  //���ӱ任��������

    bool MoveInTile(float vX, float vY);  //��������ϵ��ƶ�
    bool IsLastPose(const char* posename);
    void StartMove();

    SCreModel* mpModel;  //ǰ��˹�������ģ�͵�ָ��
    float      m_Time;   //�ƶ�Ҫ��ʱ��

    int m_LastTileX;
    int m_LastTileY;

    EStates m_CurrentState;  //��ǰ�˶�״̬
    EStates m_RideState;     //��ǰ���״̬

    TBlockType m_blockType;

   public:
    GcActor*         m_pMaster;  //�û�����ɫ������
    CTrumpShow*      m_pTrump;   //��Чչʾ
    CWeaponShow*     m_pWeapon;   //����չʾ
    GcStateOverHead* m_pStateOverHead;  //״̬ͷ��չʾ
    GcActorOverHead* m_pActorOverHead;  //NPCͷ����̬��ʾ
    float            m_fLastPoseSpeed;  //���һ�����Ƶ��ٶ�
    EPoses           m_vPoseID;  //����ID
    CM_MEMDEF(m_poseName, 50)
    CM_MEMDEF(m_szLastPoseName, 40)

    CRT_ActorInstance* m_pItemEffect;
    bool               m_bRender;

#ifndef PUBLISH
    static bool m_bDrawBoundBox;
#endif

   private:
    bool                     m_bLastLoop;
    BYTE                     m_bPutOnPathFinder;
    list<CRT_ActorInstance*> m_eftList;
    float                    m_fLastDistance;

    DWORD mAttackTargetID;  //��ǰ�Ĺ���Ŀ��ID

    // MoveToPosition ʹ�õı���
    float      m_fMoveSpeed;  //ÿ�����
    int        m_bMoveToPositionNewMove;
    float      m_fMoveToPositionOldX, m_fMoveToPositionOldY, m_fMoveToPositionTime;
    float      m_fMoveToPositionSpeed, m_fMoveToPositionSpeedX, m_fMoveToPositionSpeedY;
    RtgVertex3 m_vMoveToPosition;

   public:
    float m_fLastMoveSpeed;
};

#endif  //GC_BASE_ACTOR_H
