
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

//Tianh 不同的技能伤害计算方式
const int  SKILL_BADAMAGE_ONE  = 0;//保持原来的不变
const int  SKILL_BADAMAGE_TWO = 1;


const int  SKILL_NOLMAL_ATTACK = 111;
const int  SKILL_SWITCH_WEAPON = 112;


const  long   School_PublicCDMaxTime = 1800; //Tianh 主动技能公共冷却时间CD 10.02.21
const  long   Wap_PublicCDMaxTime    = 2000; //法宝技能
const  long   Prop_PublicCDMaxTime   = 1000; //其他公共CD
const  long   beast_PublicCDMaxTime  = 1000; //驯兽类

#include "skill_dummy.h"

/*----------------------------------------------------------------------------
- CSkillManage.
-   技能管理类，负责技能表格的读取，技能查找，统计等功能
-   整个程序只有一个实例
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
-   人物技能，每个人都有一个实例
----------------------------------------------------------------------------*/

class CActorSkill : public CDummyActorSkill
{
public:
    struct SActorEffect
    {
        SSkillEffect*   pAttr;
        unsigned long   dwRunTime;      // 当前技能已经运行了多少时间
        int             iRunTimes;      // 效果运行次数 (不存盘，读取的时候需要设置)
        DWORD           dwParentUID;    // 添加这个效果的被动技能的唯一UID
        bool            bNeedDelete;    // 不存盘
    };
    struct SActorPassive
    {
        SSkill*         pAttr;
        bool            bActive;
        DWORD           dwUID;          // 被动技能的唯一UID，用来判断什么效果是他添加的
    };

public:
    CActorSkill();
    virtual ~CActorSkill();

    void Init(void* pParent)                { m_pParent = pParent; }
    void* GetParent()                       { return m_pParent; }

    void StartupSkill();                        // 启动技能(会使用所有的被动技能)
    void ShutdownSkill();                       // 关闭技能(会关闭所有的被动技能，并且删除所有的效果)

    const char* SaveToString();                 // 保存技能
	void LoadFromString(const char* szString,long UpdateTime);  // 载入技能

    const char* SaveEffectToString();                 // 保存效果
    void LoadEffectFromString(const char* szString);  // 载入效果

    bool IsTargetSkill(unsigned short wID);     // 是否是对目标(对人或者怪物)的技能
    const char* GetLastErrInfo();

	CSkillInstance* GetSkillByIndex(int iIndex);//获得参数指定的第几个技能(怪物使用)
    CSkillInstance* GetRandomSkill();           // 随即取一个技能出来(怪物使用)
    void SetActiveSkill(unsigned short wID);    // 激活一个技能
    unsigned short GetActiveSkill();            // 取得当前激活的技能

    /////////////////////////////////////////////////////////////////////////
    // 技能操作(主动和被动)

    bool StudySkill(unsigned short wID, bool bRefresh);     // 学习技能
    bool AddSkill(unsigned short wID, unsigned short wExp, bool bRefresh);       // 增加技能
    bool DeleteSkill(unsigned short wID, bool bRefresh);    // 删除技能
    void ClearSkill(bool bRefresh);                         // 清除技能
    void ClearFightSkill(bool bRefresh); // 洗战斗点
    void ClearLifeSkill(bool bRefresh); // 洗生活点
	//////////////////////////////
	//<add by Tianh> 通过ID找到技能wSubID
	int GetSkillwSubID(int ID);
    /////////////////////////////////////////////////////////////////////////
    // 效果操作

    SActorEffect* AddEffect(unsigned short wID, DWORD dwUID, bool bRefresh);
    bool DeleteEffect(unsigned short wID, bool bRefresh); // 删除全部ID为dwID的特效
    void DeleteUIDEffect(DWORD dwUID, bool bRefresh);
    void ClearEffect(bool bRefresh);

    /////////////////////////////////////////////////////////////////////////
    // 查找操作

    SActorEffect* FindEffect(unsigned short wID);                   // 查找效果
    SActorEffect* FindEffectByClassID(int cID);                     // 查找效果
    CSkillInstance* FindActiveSkill(unsigned short wID);            // 只查找主动技能
    SActorPassive* FindPassiveSkill(unsigned short wID);            // 只查找被动技能
	bool FindPassive(unsigned short wID); 
	short FindEffectbyClass(int cClass);                 // 通过cClass找到正在使用技能的ID
	bool FindEffectbyId(int wID);                     // 查找效果
    bool FindAllSkill(unsigned short wID);                          // 查找所有技能
    SSkill* FindSkillByName(const char* szSkillName);               // 查找所有技能
    SSkill* FindSkillBySubID(WORD wSubID);                          // 查找所有技能
    CSkillInstance* FindActiveSkillByName(const char* szSkillName); // 只查找主动技能
    CSkillInstance* FindActiveSkillBySubID(WORD wSubID);            // 只查找主动技能
    SActorPassive* FindPassiveSkillByName(const char* szSkillName); // 只查找主动技能
    SActorPassive* FindPassiveSkillBySubID(WORD wSubID);            // 只查找主动技能
    SActorPassive* FindStrengthenSkill(WORD wSkillID);              // 查找wSkillID的加强技能是否存在

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

    // 使用技能，内部会判断是否能使用，所以外部无需调用CanUseSkill判断
    bool UseSkill(unsigned short wID);
    bool UseSkill(unsigned short wID, DWORD dwTargetID);
    bool UseSkill(unsigned short wID, float* pPos);

    void TidyUp(); // 整理技能包，把出错的技能清除掉

    short GetClassSP(unsigned short wClassID);
	
	void SetbRun(bool sV){m_bRun = sV;}

    // 重载

    virtual void OnStudySkill(SSkill* pSkillAttr, bool bRefresh){}
    virtual void OnClearSkill(bool bFigthSkill, int iRemoveSP){}

    // Net

    void NetWriteSkill(CG_CmdPacket* pPacket);  // 只用来更新到客户端
    void NetWriteEffect(CG_CmdPacket* pPacket); // 更新效果

    virtual void OnSkillModify(){}
    virtual void OnEffectModify(){}
	//virtual void OnResumeTimeModify(WORD wID,  WORD wResumeTime){}

public:

    // 主动技能
    virtual void OnSkillUse(SSkill* pSkill, CActorSkill* pReleaser){}
	virtual void OnSkillUseCalculating(SSkill* pSkill, CActorSkill* pReleaser,int iDmgPhy,int iDmgElement[3],long bCriDamage,bool bResult){}

	virtual void OnSkillAddHP(SSkill* pSkill, CActorSkill* pReleaser){}

    // 被动技能
    virtual void OnPassiveOpen(SActorPassive* pPassive){}
    virtual void OnPassiveClose(SActorPassive* pPassive){}

    // 效果
    virtual void OnEffectOpen(SActorEffect* pEffect){}
    virtual void OnEffectClose(SActorEffect* pEffect){}
    virtual bool OnEffectCycle(SActorEffect* pEffect){return false;}

    virtual void OnUseRangeSkill(SSkill* pSkillAttr,const float* pPos){}
	virtual void OnUseRangeSkillAddHp(SSkill* pSkillAttr, const float* pPos,CActorSkill* pReleaser){}



    // 当属性发生改变的时候，这个函数被人物调用，比如人物装卸武器，升级等等就调用这个技能
    //     用来检查身上的被动技能是否还有效，如果无效就关闭
    //     另外检查身上关闭状态的技能是否能打开，如果可以就打开
	virtual void OnAttributeChanged(bool bRefresh=true);

	std::list<SSkillContinueMask>	m_listMask;				// 保留的CD

	std::list<SActorEffect>         m_listEffect;           // 效果

	//Tianh 返回各类型公共CD百分比  比如：1表示完全没恢复，0已经恢复，0.5恢复到一半
	float GetPublicCDRate(SSkill* pAttr);

	unsigned long   m_SchoolPublicCDTime;
	unsigned long   m_WapPublicCDTime;
	unsigned long   m_PropPublicCDTime;
	unsigned long   m_beastPublicCDTime;

	bool          ImmunoUseful;  //是否免疫有益BUF
	bool          ImmunoHarmful; //是否免疫有害BUF
	bool          ImmunoAll;    //是否免疫多有BUF

protected:

    std::list<CSkillInstance*>      m_listSkill;            // 主动技能
    std::list<SActorPassive>        m_listPassive;          // 被动技能
	

    unsigned short                  m_sActiveSkillID;
    const char*                     m_pLastErrorInfo;
    void*                           m_pParent;
    DWORD                           m_dwPassiveUID;         // 被动技能的唯一ID
    bool                            m_bRun;

    bool                                    m_bFindSkill;
    std::list<CSkillInstance*>::iterator    m_itListSkill;
    std::list<SActorPassive>::iterator      m_itListPassive;
    std::list<SActorEffect>::iterator       m_itListEffect;
};

/*----------------------------------------------------------------------------
- 全局函数.
----------------------------------------------------------------------------*/
bool SkillInit(const char* szSkillFile, const char* szPassiveFile, const char* szEffectFile, const char* szMutexFile);
bool SkillClean();
CSkillManage* Skill();
CActorSkill* FindActorSkill(void* pActor, DWORD dwCreatureOID);

#endif
/*----------------------------------------------------------------------------
-   End.
----------------------------------------------------------------------------*/
