#ifndef __WEAPON_SHOW_H__
#define __WEAPON_SHOW_H__

#include "core/rt2_core.h"
#include "graph/rt_graph.h"
#include "character/actor.h"

class GcBaseActor;
struct SSkill;
using namespace std;

class CWeaponShow {
   public:
    CWeaponShow(void);
    virtual ~CWeaponShow(void);

    enum WeaponState {
        STATE_NULL, // 无状态
        STATE_IDLE, // 闲置状态
        STATE_GUARD, // 防御状态
        STATE_GO, // 进攻状态
        STATE_ATTACK, // 攻击状态
        STATE_SKILL, // 技能状态
        STATE_RETURN // 返回状态
    };

    void Tick(DWORD delta); // 每帧更新
    void Render(RtgDevice* dev, RTGRenderMask vMask); // 渲染
    void RenderHighLight(RtgDevice* dev, RtgVertex3& color, RtgVertex3& scale); // 高亮渲染
    void SetWeaponState(WeaponState state); // 设置武器状态

    // 新接口
    void Init(GcBaseActor* Host, const char* Model, SWeapon* weapon, // 初始化
              SItemID& item);  // 最多支持link 3个body
    void DoAttack(DWORD TargetId, const char vpPoseName[], bool vLoop, float fSpeed); // 执行攻击
    void DoSkillAttack(bool bPosition, DWORD TargetId, const float* fPos, SSkill* skill); // 执行技能攻击
    bool PlayPose(const char vpPoseName[], bool loop = false, float fSpeed = 1.f); // 播放姿势

    void ChangeModel(const char* NewModel, SItemID& item); // 更换模型

    void RevertLinkModel(); // 恢复链接模型

    string m_modelName; // 模型名称
    string m_wayName; // 路径名称

    SSkill*            m_skill; // 技能
    SItemID            m_item; // 物品ID
    SWeapon*           m_Weapon; // 武器
    WeaponState        m_state; // 当前状态
    GcBaseActor*       m_host; // 主角
    CRT_ActorInstance* m_body; // 主体
    CRT_ActorInstance* m_way; // 路径
    CRT_ActorInstance* m_skillEffect; // 技能效果

    CRT_ActorInstance* m_FlySwordRib;  // 飞剑的刀光

    RtgMatrix12         m_worldMatrix; // 世界矩阵
    vector<RtgMatrix12> m_vWorldMatrixOther; // 其他世界矩阵

    float m_wayScale; // 路径缩放

    DWORD       m_delta; // delta时间
    DWORD       m_idleDelta; // 闲置时间
    DWORD       m_life; // 生命值
    DWORD       m_targetId; // 目标ID
    std::string m_poseName; // 姿势名称
    std::string m_szLastPoseName; // 最后一个姿势名称
    bool        m_bRender; // 是否渲染
};

#endif
