
#ifndef _SKILL_MANAGE_H_
#define _SKILL_MANAGE_H_

//#include <unordered_map>
#include "skill_base.h"

class CG_CmdPacket;

const int   g_iActorMaxSkill = 80;
const int   g_iActorMaxPassive = 80;
const int   g_iActorMaxEffect = 80;

const int  SKILL_ELEMENT_WATER      = 0;
const int  SKILL_ELEMENT_FIRE	    = 1;
const int  SKILL_ELEMENT_POISON	    = 2;
const int  SKILL_MAX_ELEMENT_ATTR   = 3;

//Tianh ��ͬ�ļ����˺����㷽ʽ
const int  SKILL_BADAMAGE_ONE  = 0;//����ԭ���Ĳ���
const int  SKILL_BADAMAGE_TWO = 1;


const int  SKILL_NOLMAL_ATTACK = 111;
const int  SKILL_SWITCH_WEAPON = 112;


const  long   School_PublicCDMaxTime = 1800; //Tianh �������ܹ�����ȴʱ��CD 10.02.21
const  long   Wap_PublicCDMaxTime    = 2000; //��������
const  long   Prop_PublicCDMaxTime   = 1000; //��������CD
const  long   beast_PublicCDMaxTime  = 1000; //ѱ����

#include "skill_dummy.h"

/*----------------------------------------------------------------------------
- CSkillManage.
-   ���ܹ����࣬�����ܱ��Ķ�ȡ�����ܲ��ң�ͳ�Ƶȹ���
-   ��������ֻ��һ��ʵ��
----------------------------------------------------------------------------*/
class CSkillManage
{
public:
    CSkillManage();
    ~CSkillManage();

    bool Init(const char* szSkillFile, const char* szPassiveFile, const char* szEffectFile, const char* szMutexFile);
    bool LoadMutex(const char* szMutexFile);
    bool Clean();

    SSkill* FindSkill(unsigned short wID);
	SSkill* FindNextLevelSkill(unsigned short wSkillID);
	SSkill* FindLastLevelSkill(unsigned short wSkillID);

    const char* FindSkillClass(unsigned short wClassID);

    SSkill* FirstSkill();
    SSkill* NextSkill();

    int GetTotalSkill()                         { return m_iTotalSkill; }
    SSkill* GetSkillPool()                      { return m_pPoolSkill; }

    SSkillEffect* FindEffect(unsigned short wID);

    SSkillEffect* FirstEffect();
    SSkillEffect* NextEffect();

    int GetTotalEffect()                        { return m_iTotalEffect; }
    SSkillEffect* GetEffectPool()               { return m_pPoolEffect; }

private:
    int                             m_iTotalSkill;
    SSkill*                         m_pPoolSkill;
    EXT_SPACE::unordered_map<unsigned short, SSkill*>  m_mapSkill;
    EXT_SPACE::unordered_map<unsigned short, SSkill*>::iterator m_itSkill;

    int                             m_iTotalEffect;
    SSkillEffect*                   m_pPoolEffect;
    EXT_SPACE::unordered_map<unsigned short, SSkillEffect*>  m_mapEffect;
    EXT_SPACE::unordered_map<unsigned short, SSkillEffect*>::iterator m_itEffect;

    unsigned short*                 m_pPoolMutex;
	RtCsv m_csvSkill;
	RtCsv m_csvPassive;
	RtCsv m_csvEffect;

public:
    SSkillNodeFilter*            m_pSkillFilterNode;
    SSkillActionNode*            m_pSkillEffectNode;

};

/*----------------------------------------------------------------------------
- CActorSkill.
-   ���＼�ܣ�ÿ���˶���һ��ʵ��
----------------------------------------------------------------------------*/

class CActorSkill : public CDummyActorSkill
{
public:
    struct SActorEffect
    {
        SSkillEffect*   pAttr;
        unsigned long   dwRunTime;      // ��ǰ�����Ѿ������˶���ʱ��
        int             iRunTimes;      // Ч�����д��� (�����̣���ȡ��ʱ����Ҫ����)
        DWORD           dwParentUID;    // ������Ч���ı������ܵ�ΨһUID
        bool            bNeedDelete;    // ������
    };
    struct SActorPassive
    {
        SSkill*         pAttr;
        bool            bActive;
        DWORD           dwUID;          // �������ܵ�ΨһUID�������ж�ʲôЧ��������ӵ�
    };

public:
    CActorSkill();
    virtual ~CActorSkill();

    void Init(void* pParent)                { m_pParent = pParent; }
    void* GetParent()                       { return m_pParent; }

    void StartupSkill();                        // ��������(��ʹ�����еı�������)
    void ShutdownSkill();                       // �رռ���(��ر����еı������ܣ�����ɾ�����е�Ч��)

    const char* SaveToString();                 // ���漼��
	void LoadFromString(const char* szString,long UpdateTime);  // ���뼼��

    const char* SaveEffectToString();                 // ����Ч��
    void LoadEffectFromString(const char* szString);  // ����Ч��

    bool IsTargetSkill(unsigned short wID);     // �Ƿ��Ƕ�Ŀ��(���˻��߹���)�ļ���
    const char* GetLastErrInfo();

	CSkillInstance* GetSkillByIndex(int iIndex);//��ò���ָ���ĵڼ�������(����ʹ��)
    CSkillInstance* GetRandomSkill();           // �漴ȡһ�����ܳ���(����ʹ��)
    void SetActiveSkill(unsigned short wID);    // ����һ������
    unsigned short GetActiveSkill();            // ȡ�õ�ǰ����ļ���

    /////////////////////////////////////////////////////////////////////////
    // ���ܲ���(�����ͱ���)

    bool StudySkill(unsigned short wID, bool bRefresh);     // ѧϰ����
    bool AddSkill(unsigned short wID, unsigned short wExp, bool bRefresh);       // ���Ӽ���
    bool DeleteSkill(unsigned short wID, bool bRefresh);    // ɾ������
    void ClearSkill(bool bRefresh);                         // �������
    void ClearFightSkill(bool bRefresh); // ϴս����
    void ClearLifeSkill(bool bRefresh); // ϴ�����
	//////////////////////////////
	//<add by Tianh> ͨ��ID�ҵ�����wSubID
	int GetSkillwSubID(int ID);
    /////////////////////////////////////////////////////////////////////////
    // Ч������

    SActorEffect* AddEffect(unsigned short wID, DWORD dwUID, bool bRefresh);
    bool DeleteEffect(unsigned short wID, bool bRefresh); // ɾ��ȫ��IDΪdwID����Ч
    void DeleteUIDEffect(DWORD dwUID, bool bRefresh);
    void ClearEffect(bool bRefresh);

    /////////////////////////////////////////////////////////////////////////
    // ���Ҳ���

    SActorEffect* FindEffect(unsigned short wID);                   // ����Ч��
    SActorEffect* FindEffectByClassID(int cID);                     // ����Ч��
    CSkillInstance* FindActiveSkill(unsigned short wID);            // ֻ������������
    SActorPassive* FindPassiveSkill(unsigned short wID);            // ֻ���ұ�������
	bool FindPassive(unsigned short wID); 
	short FindEffectbyClass(int cClass);                 // ͨ��cClass�ҵ�����ʹ�ü��ܵ�ID
	bool FindEffectbyId(int wID);                     // ����Ч��
    bool FindAllSkill(unsigned short wID);                          // �������м���
    SSkill* FindSkillByName(const char* szSkillName);               // �������м���
    SSkill* FindSkillBySubID(WORD wSubID);                          // �������м���
    CSkillInstance* FindActiveSkillByName(const char* szSkillName); // ֻ������������
    CSkillInstance* FindActiveSkillBySubID(WORD wSubID);            // ֻ������������
    SActorPassive* FindPassiveSkillByName(const char* szSkillName); // ֻ������������
    SActorPassive* FindPassiveSkillBySubID(WORD wSubID);            // ֻ������������
    SActorPassive* FindStrengthenSkill(WORD wSkillID);              // ����wSkillID�ļ�ǿ�����Ƿ����

	short HaveCollectSkill();
	short HaveProtectSkill();

    int SkillCount();
    int EffectCount();
    SSkill* FirstSkill();
    SSkill* NextSkill();
    SActorEffect* FirstEffect();
    SActorEffect* NextEffect();
    CSkillInstance* FirstActiveSkill();
    CSkillInstance* NextActiveSkill();
    SActorPassive* FirstPassiveSkill();
    SActorPassive* NextPassiveSkill();

	int GetAllActiveSkillID(std::vector<int> &vIDList);

    /////////////////////////////////////////////////////////////////////////

    void Run(unsigned long dwMillisecond);

    bool CanUseSkill(unsigned short wID);
    bool CanUseSkill(unsigned short wID, CActorSkill* pTarget);
    bool CanUseSkill(unsigned short wID, float* pPos);

    // ʹ�ü��ܣ��ڲ����ж��Ƿ���ʹ�ã������ⲿ�������CanUseSkill�ж�
    bool UseSkill(unsigned short wID);
    bool UseSkill(unsigned short wID, DWORD dwTargetID);
    bool UseSkill(unsigned short wID, float* pPos);

    void TidyUp(); // �����ܰ����ѳ���ļ��������

    short GetClassSP(unsigned short wClassID);
	
	void SetbRun(bool sV){m_bRun = sV;}

    // ����

    virtual void OnStudySkill(SSkill* pSkillAttr, bool bRefresh){}
    virtual void OnClearSkill(bool bFigthSkill, int iRemoveSP){}

    // Net

    void NetWriteSkill(CG_CmdPacket* pPacket);  // ֻ�������µ��ͻ���
    void NetWriteEffect(CG_CmdPacket* pPacket); // ����Ч��

    virtual void OnSkillModify(){}
    virtual void OnEffectModify(){}
	//virtual void OnResumeTimeModify(WORD wID,  WORD wResumeTime){}

public:

    // ��������
    virtual void OnSkillUse(SSkill* pSkill, CActorSkill* pReleaser){}
	virtual void OnSkillUseCalculating(SSkill* pSkill, CActorSkill* pReleaser,int iDmgPhy,int iDmgElement[3],long bCriDamage,bool bResult){}

	virtual void OnSkillAddHP(SSkill* pSkill, CActorSkill* pReleaser){}

    // ��������
    virtual void OnPassiveOpen(SActorPassive* pPassive){}
    virtual void OnPassiveClose(SActorPassive* pPassive){}

    // Ч��
    virtual void OnEffectOpen(SActorEffect* pEffect){}
    virtual void OnEffectClose(SActorEffect* pEffect){}
    virtual bool OnEffectCycle(SActorEffect* pEffect){return false;}

    virtual void OnUseRangeSkill(SSkill* pSkillAttr,const float* pPos){}
	virtual void OnUseRangeSkillAddHp(SSkill* pSkillAttr, const float* pPos,CActorSkill* pReleaser){}



    // �����Է����ı��ʱ�����������������ã���������װж�����������ȵȾ͵����������
    //     ����������ϵı��������Ƿ���Ч�������Ч�͹ر�
    //     ���������Ϲر�״̬�ļ����Ƿ��ܴ򿪣�������Ծʹ�
	virtual void OnAttributeChanged(bool bRefresh=true);

	std::list<SSkillContinueMask>	m_listMask;				// ������CD

	std::list<SActorEffect>         m_listEffect;           // Ч��

	//Tianh ���ظ����͹���CD�ٷֱ�  ���磺1��ʾ��ȫû�ָ���0�Ѿ��ָ���0.5�ָ���һ��
	float GetPublicCDRate(SSkill* pAttr);

	unsigned long   m_SchoolPublicCDTime;
	unsigned long   m_WapPublicCDTime;
	unsigned long   m_PropPublicCDTime;
	unsigned long   m_beastPublicCDTime;

	bool          ImmunoUseful;  //�Ƿ���������BUF
	bool          ImmunoHarmful; //�Ƿ������к�BUF
	bool          ImmunoAll;    //�Ƿ����߶���BUF

protected:

    std::list<CSkillInstance*>      m_listSkill;            // ��������
    std::list<SActorPassive>        m_listPassive;          // ��������
	

    unsigned short                  m_sActiveSkillID;
    const char*                     m_pLastErrorInfo;
    void*                           m_pParent;
    DWORD                           m_dwPassiveUID;         // �������ܵ�ΨһID
    bool                            m_bRun;

    bool                                    m_bFindSkill;
    std::list<CSkillInstance*>::iterator    m_itListSkill;
    std::list<SActorPassive>::iterator      m_itListPassive;
    std::list<SActorEffect>::iterator       m_itListEffect;
};

/*----------------------------------------------------------------------------
- ȫ�ֺ���.
----------------------------------------------------------------------------*/
bool SkillInit(const char* szSkillFile, const char* szPassiveFile, const char* szEffectFile, const char* szMutexFile);
bool SkillClean();
CSkillManage* Skill();
CActorSkill* FindActorSkill(void* pActor, DWORD dwCreatureOID);

#endif
/*----------------------------------------------------------------------------
-   End.
----------------------------------------------------------------------------*/
