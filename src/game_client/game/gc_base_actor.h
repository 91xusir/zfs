

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
    static int s_iShadowQuality;  // 0 无阴影， 1 全部都是圈， 2 自己和兽动态其他圈， 3 全部动态
};

// 任务提示（感叹号或者问号）
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
    CRT_ActorInstance* m_pTaskGraph1;  // 感叹号
    CRT_ActorInstance* m_pTaskGraph2;  // 问号
    CRT_ActorInstance* m_pTaskGraph3;  // 问号

    CRT_ActorInstance* m_pRender;  // 应该渲染得图形指针

    DWORD m_dwLastQueryTime;
};

//添加人物头顶动态提示
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
    CRT_ActorInstance* m_pActorGraph;  // NPC头顶提示

    CRT_ActorInstance* m_pRender;  // 应该渲染得图形指针
};

class GcBaseActor : public CRT_PoseNotify {
   public:
    friend class GcActor;

    enum EStates {
        //移动状态
        NULL_STATE,    //空状态
        STAND_STATE,   //站立
        IDLE_STATE,    //空闲动作
        GUARD_STATE,   //战斗待机
        ZAZEN_STATE,   //打坐
        CHANT_STATE,   //吟唱
        WALK_STATE,    //行走
        ATTACK_STATE,  //攻击中
        HURT_STATE,    //受伤

        READY_BORN_STATE,  //准备出生
        BORN_STATE,        //正在出生
        DEAD_STATE,        //死亡状态

        //骑乘状态
        NO_RIDE_STATE,  //没有骑乘状态
        RIDE_STATE,     //骑兽状态
        BY_RIDE_STATE,  //被骑状态
    };

    enum EPoses {
        POSE_NONE = 0,   // 0无姿势，默认状态
        POSE_STAND,      // 1站立姿势
        POSE_GUARD,      // 2战斗待机动作
        POSE_IDLE,       // 3空闲状态
        POSE_TALK,       // 4对话姿势
        POSE_WALK,       // 5行走
        POSE_RUN,        // 6奔跑，追击
        POSE_ATTACK,     // 7攻击
        POSE_BASH,       // 8暴击动作
        POSE_HURT,       // 9受伤状态
        POSE_CHANT,      // 10吟唱动作
        POSE_REFINE,     // 11修炼法宝
        POSE_MAKE,       // 12制作物品
        POSE_ZAZEN,      // 13打坐
        POSE_DIG,        // 14挖掘动作
        POSE_GATHER,     // 15采药动作
        POSE_SCALP,      // 16扒皮动作
        POSE_PULL,       // 17抽出武器
        POSE_DOWN,       // 18收回武器
        POSE_DEAD,       // 19死亡
        POSE_BODY,       // 20死亡尸体
        POSE_RELIVE,     // 21复活
        POSE_RIDE,       // 22骑乘
        POSE_BORN,       // 23出生状态
        POSE_FUNACTION,  // 24特殊功能动作
        POSE_MAX,        // 25最大值，通常用于确定姿势的数量
    };

    GcBaseActor(GcActor* vpMaster);
    virtual ~GcBaseActor();

    bool CreateGraphData(SCreModel* vpModel, short HeadModelID);  //创建图形
    bool DestroyGraphData();                                      //销毁图形

    void Run(float fSecond);                                //逻辑循环
    void Render(RtgDevice& inDevice, RTGRenderMask eMask);  //渲染
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

    void FaceTo(float vX, float vY);  //面朝向
    void UpdateDirection();           //更新方向
    bool RemoveSelf();                //从地图上移除自己

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

    //动画的回调函数
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

    void Update();                                              //将位置更新到图形部分
    bool SetGraph(short HeadModelID, const char* vpActorName);  //设置图形
    void UnLoadAllSkin();

    RtsPathPath* GetPathNode() { return m_pPathNode; }  //返回路径点队列

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

    RtgMatrix16  m_Matrix;     //位置和方向
    RtsPathPath* m_pPathNode;  //路径点的队列

    RtsPathFind* m_pPathFind;            //tim.yang  寻路
    RtgMatrix16  m_drawLineStartMatrix;  //tim.yang
    long         m_isDrawPathLine;       //tim.yang  是否绘制路线

    GcActorGraph      mGraph;    //图象
    GcRenderActorBase m_Render;  //渲染的代理

    std::string m_strComplexHead;
    std::string m_strComplexBody;
    std::string m_strComplexHand;
    std::string m_strComplexFoot;
    std::string m_strComplexPants;

   private:
    bool SetGraph(SCreModel* vpModel, short HeadModelID, const char* vpActorName);  //设置图形

    void UnRide(GcActor*);  //下兽

    bool FindPath(int vX, int vY, bool vIgnoreActor = false,
                  bool vIgnoreTarget = false);  //寻找路径
    bool AddSelf(int vX, int vY);               //往地图上添加自己
    bool AddSelf(float vX, float vY);           //往地图上添加自己
    bool AddSelfFind(int vX, int vY);           //往地图上添加自己(自动查找空位)
    bool AddSelfFind(float vX, float vY);       //往地图上添加自己(自动查找空位)

    bool IsPosition(float vX, float vY);                              //是否在该点上
    bool IsPosition(int vX, int vY);                                  //是否在该格子上
    void Point2Tile(float vX, float vY, int& vrTileX, int& vrTileY);  //从点变换到格子坐标
    void Tile2Point(int vTileX, int vTileY, float& vrX, float& vrY, float& vrZ);  //格子变换到点坐标

    bool MoveInTile(float vX, float vY);  //处理格子上的移动
    bool IsLastPose(const char* posename);
    void StartMove();

    SCreModel* mpModel;  //前后端公用数据模型的指针
    float      m_Time;   //移动要的时间

    int m_LastTileX;
    int m_LastTileY;

    EStates m_CurrentState;  //当前运动状态
    EStates m_RideState;     //当前骑乘状态

    TBlockType m_blockType;

   public:
    GcActor*         m_pMaster;  //该基本角色的宿主
    CTrumpShow*      m_pTrump;   //特效展示
    CWeaponShow*     m_pWeapon;   //武器展示
    GcStateOverHead* m_pStateOverHead;  //状态头顶展示
    GcActorOverHead* m_pActorOverHead;  //NPC头顶动态提示
    float            m_fLastPoseSpeed;  //最后一个姿势的速度
    EPoses           m_vPoseID;  //姿势ID
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

    DWORD mAttackTargetID;  //当前的攻击目标ID

    // MoveToPosition 使用的变量
    float      m_fMoveSpeed;  //每秒多少
    int        m_bMoveToPositionNewMove;
    float      m_fMoveToPositionOldX, m_fMoveToPositionOldY, m_fMoveToPositionTime;
    float      m_fMoveToPositionSpeed, m_fMoveToPositionSpeedX, m_fMoveToPositionSpeedY;
    RtgVertex3 m_vMoveToPosition;

   public:
    float m_fLastMoveSpeed;
};

#endif  //GC_BASE_ACTOR_H
