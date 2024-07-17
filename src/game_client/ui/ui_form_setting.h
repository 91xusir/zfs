#ifndef _INC_UI_FORM_SETTING_H_
#define _INC_UI_FORM_SETTING_H_

class UIFormSetting : GcTimerListener
{
    struct SDepth
    {
        int depth;
        std::vector<int> rate;
    };

	struct Resolution
	{
		int width;
		int height;

		std::vector<SDepth> depth;
	};
public:
	UIFormSetting();
	virtual ~UIFormSetting();
	static short    sUIInputProject;
	static short    sUIShowProject;
	void Change();
	void Show();
	void Hide();
	bool IsVisible(){guard;return m_frmThis->IsVisible();unguard;}
	void SetGemeFullScreenOrNo();
private:
	void OnClick_OK(void*, void*);
	void OnClick_Cancel(void*, void*);
	void OnExpertClick_OK(void*,void*);
	void OnExpertClick_Cancel(void*,void*);
	void OnClick_Apply(void*, void*);
	void OnScroll_MusicVolume(void*, void*);
	void OnScroll_SoundVolume(void*, void*);
	void OnScroll_VideoVolume(void*, void*);
    void OnSelect_Resolution(void*, void*);
	void OnScroll_UsersInScreen(void* , void*);
	//void OnSelect_InputMode(ui::RtwWidget* vpSender, RtwEventDelegate* );
	void OnClick_Expert(ui::RtwWidget* vpSender, RtwEventDelegate* );
	void On_HudChange(ui::RtwWidget* vpSender, RtwEventDelegate* );
	void OnClick_DefaultCamera(void*, void*);
	void OnClick_ConfirmResolution_Yes(void*, void*);
	void OnClick_ConfirmResolution_No(void*, void*);
	void OnClick_Used3D(void*, void*);
    virtual void OnTimerEvent(DWORD dwID);
	void InitResolution();
	void RefreshVideoSettings();
	void RefreshAudioSettings();
	void RefreshGameSettings();
	void RefreshShowSettings();
	void RefreshRefuseSettings();
	void OnCheckButtonFullScreenOrNo(ui::RtwWidget* vpSender, RtwEventDelegate*);
	void OnCheckButtonShowSkillShotcut(ui::RtwWidget* vpSender, RtwEventDelegate*);
	void OnCheckButtonSilence(ui::RtwWidget* vpSender, RtwEventDelegate*);
	void OnCheckButtonTexture(ui::RtwWidget* vpSender, RtwEventDelegate*);
	void OnCheckButtonEffect(ui::RtwWidget* vpSender, RtwEventDelegate*);
	void OnCheckButtonShadow(ui::RtwWidget* vpSender, RtwEventDelegate*);
	void OnSaveVideoSettings();
	void OnSaveShowSettings();
	void OnSaveRefuseSettings();
	void OnCheckButtonCamera(ui::RtwWidget* vpSender, RtwEventDelegate*);
private:
    GcSetting       m_oldSetting;
    // Graph
	//RtwComboBox*	m_resolution;		//分辨率
	//CUiCheckButton*		m_fullscreen;		//是否全屏幕
	//RtwComboBox*	m_graphQuality;		//图形显示质量

	//RtwComboBox*	m_textureQuality;	//贴图质量
	//RtwComboBox*	m_effectQuality;	//特效质量
	//RtwComboBox*	m_shadowQuality;	//阴影质量
	//CUiCheckButton*		m_sawtooth;			//全屏抗锯齿
	//CUiCheckButton*		m_reflect;			//环境反射
	//CUiCheckButton*		m_reflectpeople;	//人物反射
	//CUiCheckButton*		m_effectPost;       //后期处理
 //   CUiCheckButton*		m_effectObject;     //后期物件处理


    // Game
 //   RtwHScrollBar*   m_usersInScreen;	//同屏人数
	//CUiCheckButton*		m_displayhud;		//是否显示HUD
	//CUiCheckButton*		m_hudSay;			//是否显示说话内容
	//CUiCheckButton*		m_hudName;			//是否显示名字
	//CUiCheckButton*		m_hudGuild;			//是否显示诸侯国
	//CUiCheckButton*		m_hudNick;			//是否显示称号
	//CUiCheckButton*		m_hudCountry;		//是否显示国家

	//CUiCheckButton*		m_UIRefuse;			//拒绝所有邀请

	//RtwComboBox*    m_UIInputProject;		//界面输入设置方案
	//RtwComboBox*    m_UIShowProject;		//界面显示设置方案

	/////////////////////////
	//all new setting: gao //
	/////////////////////////
	// form
	RtwWidget*			m_frmThis;
	RtwWidget*			m_ExpertForm;
	// video
	CUiCheckButton*		m_checkWindows;
	CUiCheckButton*		m_checkFullScreen;
	RtwListBox*			m_lstResolution;
	RtwButton*			m_expertButton;		//定制选项按钮
	RtwHScrollBar*		m_hsBarVideo;
	CUiCheckButton*		m_checkLight;
	CUiCheckButton*		m_checkFullScreenLight;
    // Audio
	RtwHScrollBar*		m_musicVolume;		//音乐大小
	RtwHScrollBar*		m_soundVolume;		//音效大小
	CUiCheckButton*		m_checkMusicSilence;
	CUiCheckButton*		m_checkSoundSilence;
	CUiCheckButton*		m_3dsound;			//是否3D音效
	// setting
	CUiCheckButton*		m_checkShowSelfName;
	CUiCheckButton*		m_checkShowBlood;
	CUiCheckButton*		m_checkShowDegree;
	CUiCheckButton*		m_checkShowDamage;
	CUiCheckButton*		m_checkShowUnionName;
	CUiCheckButton*		m_checkShowEXP;
	CUiCheckButton*		m_checkShowOtherName;
	CUiCheckButton*		m_checkShowNPCName;
	CUiCheckButton*		m_checkShowOtherSpeak;
	CUiCheckButton*		m_checkShowTradeName;
	CUiCheckButton*		m_checkShowSelfSpeak;
	// opreatesetting
	CUiCheckButton*		m_lockcamera;		//是否锁定视角
	RtwButton*			m_btnResDefault;
	// mutualopreate
	CUiCheckButton*		m_checkRefuseTeam;
	CUiCheckButton*		m_checkRefuseAction;
	CUiCheckButton*		m_checkRefuseMail;
	CUiCheckButton*		m_checkRefuseDuel;
	CUiCheckButton*		m_checkRefuseFriend;
	CUiCheckButton*		m_checkRefuseMonster;
	// textuersetting
	CUiCheckButton*		m_checkTextureQuality1;
	CUiCheckButton*		m_checkTextureQuality2;
	CUiCheckButton*		m_checkTextureQuality3;
	// effectsetting
	CUiCheckButton*		m_checkEffectQuality1;
	CUiCheckButton*		m_checkEffectQuality2;
	CUiCheckButton*		m_checkEffectQuality3;
	// shadowsetting
	CUiCheckButton*		m_checkShadowQuality1;
	CUiCheckButton*		m_checkShadowQuality2;
	CUiCheckButton*		m_checkShadowQuality3;
	// skillshotcut
	CUiCheckButton*		m_checkSkillShotcut;
	////////////////////////////////////////

	std::vector<Resolution> m_resolutionList;
	RTGViewWindowMode	m_bkViewMode;
	int					m_bkResWidth;
	int					m_bkResHeight;
	int					m_bkColorDepth;
	int					m_confirmVideoTimeStamp;
	int					m_confirmVideoTimer;
	bool				m_bChanged;
};

#endif // _INC_UI_FORM_SETTING_H_
