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
        STATE_NULL, // ��״̬
        STATE_IDLE, // ����״̬
        STATE_GUARD, // ����״̬
        STATE_GO, // ����״̬
        STATE_ATTACK, // ����״̬
        STATE_SKILL, // ����״̬
        STATE_RETURN // ����״̬
    };

    void Tick(DWORD delta); // ÿ֡����
    void Render(RtgDevice* dev, RTGRenderMask vMask); // ��Ⱦ
    void RenderHighLight(RtgDevice* dev, RtgVertex3& color, RtgVertex3& scale); // ������Ⱦ
    void SetWeaponState(WeaponState state); // ��������״̬

    // �½ӿ�
    void Init(GcBaseActor* Host, const char* Model, SWeapon* weapon, // ��ʼ��
              SItemID& item);  // ���֧��link 3��body
    void DoAttack(DWORD TargetId, const char vpPoseName[], bool vLoop, float fSpeed); // ִ�й���
    void DoSkillAttack(bool bPosition, DWORD TargetId, const float* fPos, SSkill* skill); // ִ�м��ܹ���
    bool PlayPose(const char vpPoseName[], bool loop = false, float fSpeed = 1.f); // ��������

    void ChangeModel(const char* NewModel, SItemID& item); // ����ģ��

    void RevertLinkModel(); // �ָ�����ģ��

    string m_modelName; // ģ������
    string m_wayName; // ·������

    SSkill*            m_skill; // ����
    SItemID            m_item; // ��ƷID
    SWeapon*           m_Weapon; // ����
    WeaponState        m_state; // ��ǰ״̬
    GcBaseActor*       m_host; // ����
    CRT_ActorInstance* m_body; // ����
    CRT_ActorInstance* m_way; // ·��
    CRT_ActorInstance* m_skillEffect; // ����Ч��

    CRT_ActorInstance* m_FlySwordRib;  // �ɽ��ĵ���

    RtgMatrix12         m_worldMatrix; // �������
    vector<RtgMatrix12> m_vWorldMatrixOther; // �����������

    float m_wayScale; // ·������

    DWORD       m_delta; // deltaʱ��
    DWORD       m_idleDelta; // ����ʱ��
    DWORD       m_life; // ����ֵ
    DWORD       m_targetId; // Ŀ��ID
    std::string m_poseName; // ��������
    std::string m_szLastPoseName; // ���һ����������
    bool        m_bRender; // �Ƿ���Ⱦ
};

#endif
