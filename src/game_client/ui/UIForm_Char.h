#pragma once
#include "ui/rtw_ui.h"
/* gao 2010.1.12
新版本的角色属性
说明：角色的基本属性是随时都有可能发生变化，
所以每次在显示的时候，我们都会跟据当前玩家的数据进行设置
所以，本类中可能不会有记录数据
*/
#define MAXTRUMPSKILLNUM 6
class CUIForm_Char
{
public:
	enum NOWSHOWTYPE
	{
		EQUIP,
		TRUMP,
		NAZARITIMS,
		POPULARITY,
		TITLE,
		NOSELECT = -1
	};
	enum TRUMPTYPE
	{
		Fire_Att,
		Fire_Def,
		Water_Att,
		Water_Def,
		Poision_Att,
		Poision_Def,
		Undef
	};
	struct TRUMPINFO 
	{
		TRUMPINFO(int lev,int mp,int nowmp,TRUMPTYPE type,int trumpadd,int levnum,int mpnum,int trumpaddnumm):
		m_iLevel(lev),
		m_iMP(mp),
		m_iNowMP(nowmp),
		m_eTrumpType(type),
		m_iTrumpAdd(trumpadd),
		m_iLevNum(levnum),
		m_iMPNum(mpnum),
		m_iTrumpAddNum(trumpaddnumm)
		{
		}
		TRUMPINFO()
		{
			m_iLevel = 0;
			m_iMP = 0;
			m_iNowMP = 0;
			m_eTrumpType = Undef;
			m_iTrumpAdd = 0;
			m_iLevNum = 0;
			m_iMPNum = 0;
			m_iTrumpAddNum = 0;
		}
		int m_iLevel;
		int m_iMP;
		int m_iNowMP;
		TRUMPTYPE m_eTrumpType;
		int m_iTrumpAdd;
		int m_iLevNum;
		int m_iMPNum;
		int m_iTrumpAddNum;
	};

	CUIForm_Char(void);
	~CUIForm_Char(void);
	void Init();				// 数据初始化;
	void Show(bool refresh= false);
	void OnShowAttribute();
	void OnShowEquip();
	void OnShowTrump();
	void OnShowXiuXing();
	void OnShowShenWang();
	void Hide();
	void Refresh(bool refresh= false);
	void RefreshChar();
	bool IsVisible();
	void OnBtnOK(RtwWidget* vpSender, RtwEventDelegate* e);
	void OnBtnCancel(RtwWidget* vpSender, RtwEventDelegate* e);
	void OnHideGuide(RtwWidget* vpSender, RtwEventDelegate* e);
	void Add_SubButton(RtwWidget* vpSender, RtwEventDelegate* e);
	RtwPixel GetColor(float rate);
	void SetCharButtonNormal();
	void SetCharButtonAnimation();
	RtwImage* LoadSequenceImage(const string &strFileName);
	void ResetTrumpUI();
	char *GetRifineName(int attach);
	void ReceiveTrumpCmd(char rifine,long succeed,long temp = 0);
	void OnTrumpSkillLevup();
	void OnFinishRifineTrump(bool succeed);
	bool					m_bIsRifine;
	NOWSHOWTYPE GetNowShowType() {return m_eNowShow;}
	void Create3DViewGraph(bool resetView = true);
	void ReCreate3DViewGraph();
	void RefreshTrump();

private:
	void OnRClick_Parts(RtwWidget* sender, void*);
	void OnDrag_Parts(RtwWidget* sender, void*);
	void OnDragEnd_Parts(RtwWidget* sender, void*);
	void OnLClick_Parts(RtwWidget *sender, void*);
	void OnGetHintText_Parts(RtwWidget* sender, void*);
	bool Is_Widget_Parts(const string& id, CItemContainerBase::EEquipParts& outPart);
	void ItemHyperLinkSend(DWORD itemTag, DWORD itemId, DWORD playerID, const std::string &strName, RtwPixel color);

	//add by yz: 匹配在道具栏拖拽的item和装备栏 2010-6-9
	bool Is_Widget_Parts(SItemID& rItemID, CItemContainerBase::EEquipParts inPart);
	//end yz

	void OnLClick_BtnAttribute(void*,void*);
	void OnLClick_BtnTrumpSkillUp(RtwWidget *sender, void*);
	void OnGetHintTrumpSkill(RtwWidget* sender, void*);
	void OnGetHintTrumpItem(RtwWidget* sender, void*);
	void OnChangeTab(RtwWidget* sender, void*);

	void OnSelectTrumpAttach(RtwComboBox* sender, void*);
	void OnLClick_TrumpRifine(RtwButton *sender, void*);
	void OnLDClick_LevupTrumpSkill(RtwButton *sender, void*);
	void OnDragEnd_TrumpSkill(RtwButton *sender, void*);
	void OnDrag_TrumpSkill(RtwButton *sender, void*);

	void OnLClick_RotatePlayer(RtwButton *sender, void*);

	// 按属性块，form，标签页区分

	void OnHide(void*,void*);
	void OnCharHide(void*,void*)
	{
		int iSelect = m_ptabThis->GetSelectedItem();
		if(iSelect == 0 && m_pFromThis->IsVisible())
			m_bShowChar = false;
	}
	// 按属性块，form，标签页区分
	// 人物属性主要form
	RtwWidget*				m_pFromThis;
	RtwTab*					m_ptabThis;
	RtwImage::Ptr			m_ptrHoverImage;
	NOWSHOWTYPE				m_eNowShow;
	GcPlayer*				m_pPlayer;
	// 标签1
	RtwWidget*				m_plbMetier;
	RtwWidget*				m_plbName;
	RtwWidget*				m_plbAnimal;
	RtwWidget*				m_plbPKlev;
	RtwWidget*				m_plbLevel;			
	RtwButton*				m_btnParts[CItemContainerBase::MAX_EQUIP_PARTS];
	CUiCheckButton*			m_checkShowModel;
	RtwButton*				m_btnLeftRotate;
	RtwButton*				m_btnRightRotate;
	RtwButton*				m_btnAttribute;
	public:
	Rtw3DView*				m_p3DPlayerView;

	// 标签1 附加属性页
	RtwWidget* m_pfrmChar;
	bool					m_bShowChar;
	bool					m_bFinishCreateGraph;

	//RtwWidget*	m_plbSite;			
	//RtwWidget*	m_plbSex;				//
	//RtwWidget*	m_plbEmployment;		//
	RtwProgressBar*			m_expBar;
	RtwProgressBar*			m_hpBar;
	RtwProgressBar*			m_mpBar;
	RtwProgressBar*			m_powerBar;

	RtwWidget				*m_plbexpcount;
	RtwWidget				*m_plbhpcount;
	RtwWidget				*m_plbmpcount;
	RtwWidget				*m_plbpowercount;
	RtwWidget				*m_plbUnion;

	RtwWidget				*m_plbstr;
	RtwWidget				*m_plbcon;
	RtwWidget				*m_plbdex;
	RtwWidget				*m_plbint;
	RtwWidget				*m_plbhit;
	RtwWidget				*m_plbApppoint;

	RtwButton				*m_pbtnStrAdd;
	RtwButton				*m_pbtnConAdd;
	RtwButton				*m_pbtnDexAdd;
	RtwButton				*m_pbtnIntAdd;
	RtwButton				*m_pbtnHitAdd;

	RtwButton				*m_pbtnStrSub;
	RtwButton				*m_pbtnConSub;
	RtwButton				*m_pbtnDexSub;
	RtwButton				*m_pbtnIntSub;
	RtwButton				*m_pbtnHitSub;

	RtwWidget				*m_plbStrPower;
	RtwWidget				*m_plbConPower;
	RtwWidget				*m_plbDexPower;
	RtwWidget				*m_plbIntPower;
	RtwWidget				*m_plbHitPower;

	RtwTab					*m_ptabAttribute;

	RtwWidget				*m_plbAttack;
	RtwWidget				*m_plbArmor;
	RtwWidget				*m_plbDamage;
	RtwWidget				*m_plbDodge;
	RtwWidget				*m_plbAttackSpeed;
	RtwWidget				*m_plbAttackCritical;
	RtwWidget				*m_plbReAttackCritical;
	RtwWidget				*m_plbSkillCritical;
	RtwWidget				*m_plbReSkillCritical;

	RtwWidget*				m_plbFireAtt;
	RtwWidget*				m_plbFireDef;
	RtwWidget*				m_plbWaterAtt;
	RtwWidget*				m_plbWaterDef;
	RtwWidget*				m_plbPoisonAtt;
	RtwWidget*				m_plbPoisonDef;

	RtwButton*				m_pbtnOK;
	RtwButton*				m_pbtnCancel;

	// 标签2 法宝
	RtwButton*				m_pbtnTrump;
	RtwWidget*				m_plbTrumpLev;
	RtwWidget*				m_plbTrumpFireAtt;
	RtwWidget*				m_plbTrumpFireDef;
	RtwWidget*				m_plbTrumpWaterAtt;
	RtwWidget*				m_plbTrumpWaterDef;
	RtwWidget*				m_plbTrumpPoisonAtt;
	RtwWidget*				m_plbTrumpPoisonDef;

	RtwProgressBar*			m_pproActorSoul;
	RtwWidget*				m_plbActorSoulNum;

	RtwProgressBar*			m_pproTrumpSoul;
	RtwWidget*				m_plbTrumpSoulNum;
	RtwButton*				m_pbtnToTrumpStart;

	RtwComboBox*			m_pcomTrumpType;
	RtwProgressBar*			m_pproTypeSoul;
	RtwWidget*				m_plbTypeSoulNum;
	RtwButton*				m_pbtnToTypeStart;
	RtwWidget*				m_plbTypeRate;

	RtwButton*				m_pbtnTrumpSkill[MAXTRUMPSKILLNUM];
	RtwButton*				m_pbtnTrumpSkillup[MAXTRUMPSKILLNUM];

	// 标签 3
	RtwWidget*				m_plbXiuWeiLev;
	RtwWidget*				m_plbJieDuan;
	RtwProgressBar*			m_pproXiuWei;
	RtwWidget*				m_plbXiuWeiPoint;
	RtwProgressBar*			m_pproActorSoul_XiuWei;
	RtwWidget*				m_plbActorSoulNum_xiuWei;

	RtwWidget*				m_plbSuming;
	RtwProgressBar*			m_ppgbSuming;
	RtwWidget*				m_plbMenGong;
	RtwProgressBar*			m_ppgbMenGong;
	RtwWidget*				m_plbBangGong;
	RtwProgressBar*			m_ppgbBangGong;
	RtwWidget*				m_plbJunGong;
	RtwProgressBar*			m_ppgbJunGong;
	RtwWidget*				m_plbJiFen;
	RtwProgressBar*			m_ppgbJiFen;
	RtwWidget*				m_plbMorality ;
	RtwProgressBar*			m_ppgbMorality;

	// 标签 4
	RtwWidget*				m_plbXianYu;
	RtwProgressBar*			m_ppgbXianYu;
	RtwButton*				m_pbtnSwap;

	RtwWidget*				m_plbGuanfu;
	RtwWidget*				m_plbGuanfuCount;
	RtwProgressBar*			m_ppgbGuanfu;
	RtwWidget*				m_plbManzu;
	RtwWidget*				m_plbManzuCount;
	RtwProgressBar*			m_ppgbManzu;
	RtwWidget*				m_plbYishi;
	RtwWidget*				m_plbYishiCount;
	RtwProgressBar*			m_ppgbYishi;
	RtwWidget*				m_plbYijun;
	RtwWidget*				m_plbYijunCount;
	RtwProgressBar*			m_ppgbYijun;
	RtwWidget*				m_plbLianMeng;
	RtwWidget*				m_plbLianMengCount;
	RtwProgressBar*			m_ppgbLianMeng;

	//RtwWidget*				m_plbPk;


	// 数据
	RtwRect					m_imgAnimalRect[12];
	RtwRect					m_imgMetierRect[4];
	string					m_strShengWang[6];
	RtwImage*				m_imgAnimal;
	RtwImage*				m_imgMetier;

	int						m_iAllpoint;
	int						m_istrtmp;
	int						m_icontmp;
	int						m_idextmp;
	int						m_iinttmp;
	int						m_ihittmp;

	// 图标闪烁数据
	RtwImage*				m_imgCharNormal;
	RtwImage*				m_imgCharAnimation;
	bool					m_bShowAnimation;
};
/* end */