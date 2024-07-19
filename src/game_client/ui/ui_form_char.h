#ifndef __UI_FORM_CHAR_H__
#define __UI_FORM_CHAR_H__

class UIFormCharProp
{
public:
	UIFormCharProp();
	virtual ~UIFormCharProp();

	bool Init();
	bool Update();
	bool Exit();

	bool IsVisible();
	void OnShow(void *,void *);
	void OnClickAddAttr(RtwWidget *sender,void *);
	RtwPixel GetColor(float rate);

	void OnSkillCriHint(RtwWidget *sender,void *);

private:
	bool m_bValid;

	RtwWidget	*m_pFormAll;

	RtwWidget   *m_form;
	RtwWidget   *m_jobName;
	RtwWidget   *m_faction;
	RtwWidget   *m_name;
	RtwWidget   *m_lev;
	RtwWidget   *m_exp;

	RtwWidget	*m_exploit;
	RtwWidget   *m_pk;
	RtwWidget   *m_credit;
	
	RtwWidget	*m_attrPoint;
	RtwWidget	*m_skillPoint;
	
	RtwWidget	*m_hp;
	RtwWidget	*m_mp;
	
	RtwWidget	*m_str;
	RtwWidget	*m_con;
	RtwWidget	*m_dex;
	RtwWidget	*m_int;
	RtwWidget	*m_hit;

	RtwWidget	*m_strBtn;
	RtwWidget	*m_conBtn;
	RtwWidget	*m_dexBtn;
	RtwWidget	*m_intBtn;
	RtwWidget	*m_hitBtn;
	
	RtwWidget	*m_strPower;
	RtwWidget	*m_conPower;
	RtwWidget	*m_dexPower;
	RtwWidget	*m_intPower;
	RtwWidget	*m_hitPower;
	
	RtwWidget	*m_attack;
	RtwWidget	*m_damage;
	RtwWidget	*m_dodge;
	RtwWidget	*m_armor;
	RtwWidget	*m_attSpeed;
	
	RtwProgressBar*	m_expBar;
	RtwProgressBar*	m_hpBar;
	RtwProgressBar*	m_mpBar;

	RtwWidget	*m_fireArmor;
	RtwWidget	*m_waterArmor;
	RtwWidget	*m_poisonArmor;

	RtwWidget	*m_waterDamage;
	RtwWidget	*m_fireDamage;
	RtwWidget	*m_poisonDamage;
};
//tim.yang   神兽系统
class UIFormPetProp
{
public:
	UIFormPetProp();
	virtual ~UIFormPetProp(){};

	bool Init();
	bool Update();
	bool Exit();
	bool Clear();
	bool IsVisible();
	void OnShow(void *,void *);
	bool SetPetSkillIcon(RtwWidget* skill,int skillid);
	//void OnClickAddAttr(RtwWidget *sender,void *);
	//RtwPixel GetColor(float rate);

	//void OnSkillCriHint(RtwWidget *sender,void *);

private:
	bool m_bValid;

	RtwWidget	*m_pFormAll;
	RtwWidget   *m_form;

	RtwWidget   *m_name1;//名称
	RtwWidget   *m_name2;//名字
	RtwWidget   *m_lev;//等级
	RtwWidget   *m_expcount;//经验值
	RtwProgressBar	*m_expbar;//经验条
	RtwWidget	*m_fiveElements;//五行
	RtwWidget	*m_growth;//成长
	RtwWidget	*m_feed;//养成

	RtwWidget	*m_attack;//伤害
	RtwWidget	*m_damage;//防御
	RtwWidget	*m_dodge;//命中
	RtwWidget	*m_armor;//闪躲
	RtwWidget	*m_attSpeed;//攻击速度

	RtwWidget	*m_skill1;//技能1
	RtwWidget	*m_skill2;//技能2
	RtwWidget	*m_skill3;//技能3
	RtwWidget	*m_skill4;//技能4
	RtwWidget	*m_skill5;//技能5
};
//end
class UIFormPetSimple
{
public:
	UIFormPetSimple();
	virtual ~UIFormPetSimple();

	bool Init();
	bool Update();
	bool Exit();
	void SetVisible(bool visible);
	void OnChangeExpSetting(void *,void *);
	void OnSelectYes(void *,void *);
	void OnSelectNo(void *,void *);
	void OnClickAI(void *,void *);
	void OnLClickUI(void *,void *);
	void NotifyServer();

private:
	bool m_bValid;
	bool m_formSetb;
	RtwWidget   *m_form;
	RtwWidget   *m_formHp;
	RtwWidget	*m_pPetFace;
	RtwWidget   *m_name;
	RtwWidget   *m_lev;
	RtwWidget   *m_expRate;
	RtwWidget	*m_hp;
	RtwWidget	*m_mp;
	RtwWidget	*m_ai;

	RtwProgressBar*	m_expBar;
	RtwProgressBar*	m_hpBar;
	RtwProgressBar*	m_mpBar;
};

class UIFormCharSimple
{
public:
	UIFormCharSimple();
	virtual ~UIFormCharSimple();

	bool Init();
	bool Update();
	bool UpdateTaskTrackList();
	bool Exit();
	void SetVisible(bool visible);
	bool IsTrackVisible();
	void SetTrackVisible(bool bVisible);
	void OnClicked_HideTrack(void*, void*);
	void OnClicked_ShowTrack(void*, void*);
	void SetCheckTrack(bool bCheck);
	void OnSetTargetSelf(void*,void*);

	void TrackTaskItem(DWORD id);
	void BuildTaskTrackList();
	void OnScroll_TaskTrack(void*, void*);
	void OnScroll_MouseMove(void*, void*);
	void OnTimerOK(void*, void*);
private:
	bool m_bValid;
	RtwWidget   *m_form;
	RtwWidget   *m_name;
	RtwWidget   *m_lev;
	RtwWidget   *m_PlayerImage;
	RtwWidget   *m_expRate;
	RtwWidget	*m_hp;
	RtwWidget	*m_mp;
	RtwWidget	*m_ep;

	RtwProgressBar*	m_expBar;
	RtwProgressBar*	m_hpBar;
	RtwProgressBar*	m_mpBar;
	RtwProgressBar*	m_epBar;

public:
	RtwHtmlView* m_dynTaskTrack;
	RtwVScrollBar* m_taskTrackScroll;
	bool bCheckTrack;
	std::string strLastTrack;
	bool bShowScroll;
	bool bUpdateImage;

	//ldr123 能量条new
public:
	void SetPowerValue(float fVal);//设置能量条进度
private:
	RtwWidget	*m_fmPower;		//能量条窗体
	RtwLabel	*m_lblPowerSequence;//能量动画
	RtwLabel	*m_lblPowerCucurbit;//葫芦动画
	RtwComplexImage *m_ImageSequence;//怒气满格后显示
	RtwComplexImage *m_ImageCucurbit;//葫芦的序列动画
	bool			m_bShowPower;	//正在播放怒气动画
	//end
};

class UIFormMagicWeaponAddMp
{
public:
	UIFormMagicWeaponAddMp();
	virtual ~UIFormMagicWeaponAddMp();

	bool Init();
	bool Update();
	bool Exit();
	void SetVisible(bool visible);
	bool GetVisible();

	void OnHide(void *,void *);
	// void OnShow(void *,void *);
	// void OnClickAddAttr(RtwWidget *sender,void *);

private:
	bool m_bValid;
	RtwWidget   *m_form;

	RtwWidget   *m_name;
	RtwButton   *m_icon;
	RtwImage    *m_image;//imageimage
	string       m_iconName;

	RtwWidget   *m_lev;
	RtwWidget   *m_mp;
	RtwWidget	*m_hostMpBarMsg;
	RtwWidget	*m_mpBarMsg;

	RtwProgressBar*	m_mpBar;
	RtwProgressBar*	m_hostMpBar;
};


class UIFormMagicWeaponAddEle
{
public:
	UIFormMagicWeaponAddEle();
	virtual ~UIFormMagicWeaponAddEle();

	bool Init();
	bool Update(int type,float rate);
	bool Update();
	bool Exit();
	void SetVisible(bool visible);
	bool GetVisible();

	void OnMultiAddEle(void *ele,void *);
	void OnShow(void *,void *);
	void OnHide(void *,void *);
	void OnClickAddEle(RtwWidget *sender,void *);

private:
	bool m_bValid;
	RtwWidget   *m_form;
	RtwButton   *m_icon;
	//RtwImage::Ptr    m_image;//imageimage
	string       m_iconName;

	RtwWidget   *m_name;
	RtwWidget   *m_lev;
	RtwWidget	*m_mp;

	RtwWidget		*m_cost[MAX_ELEMENT_ATTR];
	RtwWidget		*m_rate[MAX_ELEMENT_ATTR];
	RtwWidget		*m_value[MAX_ELEMENT_ATTR];
	RtwProgressBar	*m_bar[MAX_ELEMENT_ATTR];
	int m_ele;
};

class UIFormRecoverPet
{
public:
	UIFormRecoverPet();
	virtual ~UIFormRecoverPet();

	bool Init();
	bool Update();
	bool Exit();
	void SetVisible(bool visible);
	bool GetVisible();

	void OnYes(void *,void *);
	void OnNo(void *,void *);
	void OnShow(void *,void *);
	void OnDragIn(RtwWidget* sender, void*);
	bool OnBagToRecoverPet(RtwWidget* sender, RtwEventDelegate* e);
	void OnDragEndRecoverPet(RtwWidget* sender, void*);
	void OnMouseRDown(RtwWidget* sender, void*);
	void OnGetHintItemText(RtwWidget* sender, void*);

private:
	bool m_bValid;
	RtwWidget   *m_form;
	RtwWidget   *m_cost;
	
	RtwButton   *m_yes;
	RtwButton   *m_no;
	RtwButton   *m_icon;
	RtwImage::Ptr		m_ptrHoverImage;
	int				m_iISendItem;
	int				m_iJSendItem;
	int				m_iPage;

	long         m_costGold;
	SItemID      m_petItem;
};

class UIFormClearSkill
{
public:
	UIFormClearSkill();
	virtual ~UIFormClearSkill();

	bool Init();
	bool Update();
	bool Exit();
	void SetVisible(bool visible);
	bool GetVisible();

	void OnYes(void *,void *);
	void OnNo(void *,void *);
	void OnShow(void *,void *);

	long         m_costGold;
	long         m_spClearLeft;
	long         m_spClearUsed;

private:
	bool m_bValid;
	RtwWidget   *m_form;
	RtwWidget   *m_cost;

	RtwButton   *m_yes;
	RtwButton   *m_no;

	RtwWidget   *m_usedCnt;
	RtwWidget   *m_leftCnt;
};

class UIFormRebirth
{
public:
	void Show(bool bModal = true);
	void Hide(bool bGray = true);
	bool IsVisible();
	void SetRebirthTime(int second);
	void SendRebirthType(char type);
	void SendRebirthTypeMsg(void*,void*);
	void OnLClick_MsgClose(void*,void*);
	void OnRebirthFromOther(long objectID,long HPrate,long MPrate);

//     void SetRebirthColor(DWORD dwColor){}//{m_gScreenEffect->SetBackgroundColor(dwColor);}
// 
private:
	RtwWidget::Ptr  m_pFrmThis;		// 整个窗口
	RtwWidget*      m_pCaption;
	RtwTextBox*     m_pTextInput;

	RtwButton*      m_pBtnPerfect;
	RtwButton*      m_pBtnOrigin;
	RtwButton*      m_pBtnBackCast;
	RtwLabel*		m_pLabelNotice;
	RtwLabel*		m_pLabelTimer;

// 	RtwWidget*		m_gScreenEffect;

	UIFormMsg*		m_pMsgBox;

	char			m_cType;

	long			m_lObjectID;
	long			m_lHPrate;
	long			m_lMPrate;
	void OnLClick_Perfect(RtwWidget* sender, void*);
	void OnLClick_Origin(RtwWidget* sender, void*);
	void OnLClick_BackCast(RtwWidget* sender, void*);
	void OnClose(void*, void*);
	void OnEnsureRebirthFromOther(void* vpSender,void* vpEvent);
	void OnCancelRebirthFromOther(void* vpSender,void* vpEvent);

public:
	UIFormRebirth();
	~UIFormRebirth();
};

#endif
