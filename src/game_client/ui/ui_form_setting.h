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
	//RtwComboBox*	m_resolution;		//�ֱ���
	//CUiCheckButton*		m_fullscreen;		//�Ƿ�ȫ��Ļ
	//RtwComboBox*	m_graphQuality;		//ͼ����ʾ����

	//RtwComboBox*	m_textureQuality;	//��ͼ����
	//RtwComboBox*	m_effectQuality;	//��Ч����
	//RtwComboBox*	m_shadowQuality;	//��Ӱ����
	//CUiCheckButton*		m_sawtooth;			//ȫ�������
	//CUiCheckButton*		m_reflect;			//��������
	//CUiCheckButton*		m_reflectpeople;	//���ﷴ��
	//CUiCheckButton*		m_effectPost;       //���ڴ���
 //   CUiCheckButton*		m_effectObject;     //�����������


    // Game
 //   RtwHScrollBar*   m_usersInScreen;	//ͬ������
	//CUiCheckButton*		m_displayhud;		//�Ƿ���ʾHUD
	//CUiCheckButton*		m_hudSay;			//�Ƿ���ʾ˵������
	//CUiCheckButton*		m_hudName;			//�Ƿ���ʾ����
	//CUiCheckButton*		m_hudGuild;			//�Ƿ���ʾ����
	//CUiCheckButton*		m_hudNick;			//�Ƿ���ʾ�ƺ�
	//CUiCheckButton*		m_hudCountry;		//�Ƿ���ʾ����

	//CUiCheckButton*		m_UIRefuse;			//�ܾ���������

	//RtwComboBox*    m_UIInputProject;		//�����������÷���
	//RtwComboBox*    m_UIShowProject;		//������ʾ���÷���

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
	RtwButton*			m_expertButton;		//����ѡ�ť
	RtwHScrollBar*		m_hsBarVideo;
	CUiCheckButton*		m_checkLight;
	CUiCheckButton*		m_checkFullScreenLight;
    // Audio
	RtwHScrollBar*		m_musicVolume;		//���ִ�С
	RtwHScrollBar*		m_soundVolume;		//��Ч��С
	CUiCheckButton*		m_checkMusicSilence;
	CUiCheckButton*		m_checkSoundSilence;
	CUiCheckButton*		m_3dsound;			//�Ƿ�3D��Ч
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
	CUiCheckButton*		m_lockcamera;		//�Ƿ������ӽ�
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
