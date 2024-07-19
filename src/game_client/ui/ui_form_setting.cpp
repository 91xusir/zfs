#include "gc_include.h"
#include "ui_layer.h"
#include "ui_form_trade.h"
#include "ui_form_union.h"

short UIFormSetting::sUIInputProject = 0;
short UIFormSetting::sUIShowProject = 0;

UIFormSetting::UIFormSetting()
: m_confirmVideoTimer(0)
{
	guard;

	//声音

	//m_soundVolume = RTW_WIDGET_T(RtwHScrollBar, "fmnewsetting.fmsetting.tabsystem.fmaudio.scrollaudio");


	//m_usersInScreen= RTW_WIDGET_T(RtwHScrollBar, "fmnewsetting.fmsetting.tabsystem.fmgame.scrollnumber");
	//m_usersInScreen->SetRange(0, 100);	
	//m_usersInScreen->EvScroll += RTW_CALLBACK(this, UIFormSetting, OnScroll_UsersInScreen);

 //   m_resolution = RTW_WIDGET_T(RtwComboBox, "fmnewsetting.fmsetting.tabsystem.fmphoto.cbsize");

	//m_resolution->EvSelect			+= RTW_CALLBACK(this, UIFormSetting, OnSelect_Resolution);
	//m_resolution->EvUpdateText		+= RTW_CALLBACK(this, UIFormSetting, OnSelect_Resolution);

	//m_fullscreen		= RTW_WIDGET_T(CUiCheckButton, "fmnewsetting.fmsetting.tabsystem.fmphoto.ckscreen");
	//m_3dsound			= RTW_WIDGET_T(CUiCheckButton, "fmnewsetting.fmsetting.tabsystem.fmaudio.ckscreen");

	//m_lockcamera		= RTW_WIDGET_T(CUiCheckButton, "fmnewsetting.fmsetting.tabsystem.fmgame.ckcamera");

	//m_displayhud		= RTW_WIDGET_T(CUiCheckButton, "fmnewsetting.fmsetting.tabsystem.fmgame.ckhud");
	//m_hudSay			= RTW_WIDGET_T(CUiCheckButton, "fmnewsetting.fmsetting.tabsystem.fmgame.cksay");
	//m_hudName			= RTW_WIDGET_T(CUiCheckButton, "fmnewsetting.fmsetting.tabsystem.fmgame.ckname");
	//m_hudGuild			= RTW_WIDGET_T(CUiCheckButton, "fmnewsetting.fmsetting.tabsystem.fmgame.ckguild");
	//m_hudNick			= RTW_WIDGET_T(CUiCheckButton, "fmnewsetting.fmsetting.tabsystem.fmgame.cknick");
	//m_hudCountry		= RTW_WIDGET_T(RtwButton, "fmnewsetting.fmsetting.tabsystem.fmgame.ckcountry");

	//m_UIRefuse=RTW_WIDGET_T(CUiCheckButton,"fmnewsetting.fmsetting.tabsystem.fmgame.ckrefuse");//拒绝邀请
	//m_oldSetting.game.bRefuse=false;



	//m_UIInputProject			= RTW_WIDGET_T(RtwComboBox,"fmnewsetting.fmsetting.tabsystem.fmgame.cbinputmode");
	//m_UIInputProject->SetSelectedItem(0);
	m_oldSetting.game.sUIInputProject=sUIInputProject;

	//m_UIShowProject				= RTW_WIDGET_T(RtwComboBox,"fmnewsetting.fmsetting.tabsystem.fmgame.cbfeedback");
	//m_UIShowProject->SetSelectedItem(0);
	m_oldSetting.game.sUIShowProject=sUIShowProject;


	//m_UIProject->EvSelect += RTW_CALLBACK(this,UIFormSetting,OnSelect_InputMode);


 //   m_displayhud->EvLClick +=RTW_CALLBACK(this,UIFormSetting,On_HudChange);
 //   m_hudSay->EvLClick +=RTW_CALLBACK(this,UIFormSetting,On_HudChange);
 //   m_hudName->EvLClick +=RTW_CALLBACK(this,UIFormSetting,On_HudChange);
 //   m_hudGuild->EvLClick +=RTW_CALLBACK(this,UIFormSetting,On_HudChange);
 //   m_hudNick->EvLClick +=RTW_CALLBACK(this,UIFormSetting,On_HudChange);
	//m_3dsound->EvLClick += RTW_CALLBACK(this,UIFormSetting,OnClick_Used3D);
    //m_hudCountry->EvLClick +=RTW_CALLBACK(this,UIFormSetting,On_HudChange);

	//m_refuseTeam->EvLClick+=RTW_CALLBACK(this,UIFormSetting,On_RefuseTeam);
	//m_refuseTrade->EvLClick+=
	//m_refuseGuild->EvLClick+=


	//m_graphQuality		= RTW_WIDGET_T(RtwComboBox, "fmnewsetting.fmsetting.tabsystem.fmphoto.cbshow");//图形显示质量
 //   m_graphQuality->EvSelect += RTW_CALLBACK(this,UIFormSetting,OnSelect_GraphQuality);
	//m_expertButton		= RTW_WIDGET_T(RtwButton, "fmnewsetting.fmsetting.tabsystem.fmphoto.btnadvance");//定制选项按钮

	//m_textureQuality	= RTW_WIDGET_T(RtwComboBox, "fmexpertsetting.fmphoto.cbtexture");//贴图质量
	//m_effectQuality		= RTW_WIDGET_T(RtwComboBox, "fmexpertsetting.fmphoto.cbeffect");//特效质量
	//m_shadowQuality		= RTW_WIDGET_T(RtwComboBox, "fmexpertsetting.fmphoto.cbshadow");//阴影质量

	//m_sawtooth			= RTW_WIDGET_T(CUiCheckButton, "fmexpertsetting.fmphoto.cksawtooth");//全屏抗锯齿
	//m_reflect			= RTW_WIDGET_T(CUiCheckButton, "fmexpertsetting.fmphoto.ckreflect");//环境反射
	//m_reflectpeople		= RTW_WIDGET_T(CUiCheckButton, "fmexpertsetting.fmphoto.ckreflectp");//人物反射
	//m_effectPost		= RTW_WIDGET_T(CUiCheckButton, "fmexpertsetting.fmphoto.ckupper");//后期处理
	//m_effectObject      = RTW_WIDGET_T(CUiCheckButton, "fmexpertsetting.fmphoto.ckeffectobj");//后期加强

	m_frmThis						= RTW_WIDGET("fmnewsetting");
	m_ExpertForm					= RTW_WIDGET("fmexpertsetting");//定制窗口
	//m_pFormThis->EvKey += RTW_CALLBACK(m_pFormThis, RtwWidget, DefaultKeyDown);
	// new setting
	m_checkWindows					= RTW_WIDGET_T(CUiCheckButton,"fmnewsetting.fmsetting.tabsystem.fmphoto.fmhemline1.btnwindow");
	m_checkFullScreen				= RTW_WIDGET_T(CUiCheckButton,"fmnewsetting.fmsetting.tabsystem.fmphoto.fmhemline1.btnscreen");
	m_lstResolution					= RTW_WIDGET_T(RtwListBox,"fmnewsetting.fmsetting.tabsystem.fmphoto.fmhemline1.lbresolution");
	m_expertButton					= RTW_WIDGET_T(RtwButton,"fmnewsetting.fmsetting.tabsystem.fmphoto.fmhemline1.btncustomize");
	m_hsBarVideo					= RTW_WIDGET_T(RtwHScrollBar,"fmnewsetting.fmsetting.tabsystem.fmphoto.fmhemline1.Scroll1");
	m_checkLight					= RTW_WIDGET_T(CUiCheckButton,"fmnewsetting.fmsetting.tabsystem.fmphoto.fmhemline1.btnlighting1");
	m_checkFullScreenLight			= RTW_WIDGET_T(CUiCheckButton,"fmnewsetting.fmsetting.tabsystem.fmphoto.fmhemline1.btnlighting2");
	m_musicVolume					= RTW_WIDGET_T(RtwHScrollBar,"fmnewsetting.fmsetting.tabsystem.fmphoto.fmhemline2.Scroll1");
	m_soundVolume					= RTW_WIDGET_T(RtwHScrollBar,"fmnewsetting.fmsetting.tabsystem.fmphoto.fmhemline2.Scroll2");
	m_checkMusicSilence				= RTW_WIDGET_T(CUiCheckButton,"fmnewsetting.fmsetting.tabsystem.fmphoto.fmhemline2.btnaudio1");
	m_checkSoundSilence				= RTW_WIDGET_T(CUiCheckButton,"fmnewsetting.fmsetting.tabsystem.fmphoto.fmhemline2.btnaudio2");
	m_3dsound						= RTW_WIDGET_T(CUiCheckButton,"fmnewsetting.fmsetting.tabsystem.fmphoto.fmhemline2.btnaudio3");
	m_checkShowSelfName				= RTW_WIDGET_T(CUiCheckButton,"fmnewsetting.fmsetting.tabsystem.fmaudio.fmhemline1.btnshow1");
	m_checkShowBlood				= RTW_WIDGET_T(CUiCheckButton,"fmnewsetting.fmsetting.tabsystem.fmaudio.fmhemline1.btnshow7");
	m_checkShowDegree				= RTW_WIDGET_T(CUiCheckButton,"fmnewsetting.fmsetting.tabsystem.fmaudio.fmhemline1.btnshow2");
	m_checkShowDamage				= RTW_WIDGET_T(CUiCheckButton,"fmnewsetting.fmsetting.tabsystem.fmaudio.fmhemline1.btnshow8");
	m_checkShowUnionName			= RTW_WIDGET_T(CUiCheckButton,"fmnewsetting.fmsetting.tabsystem.fmaudio.fmhemline1.btnshow3");
	m_checkShowEXP					= RTW_WIDGET_T(CUiCheckButton,"fmnewsetting.fmsetting.tabsystem.fmaudio.fmhemline1.btnshow9");
	m_checkShowOtherName			= RTW_WIDGET_T(CUiCheckButton,"fmnewsetting.fmsetting.tabsystem.fmaudio.fmhemline1.btnshow4");
	m_checkShowNPCName				= RTW_WIDGET_T(CUiCheckButton,"fmnewsetting.fmsetting.tabsystem.fmaudio.fmhemline1.btnshow5");
	m_checkShowOtherSpeak			= RTW_WIDGET_T(CUiCheckButton,"fmnewsetting.fmsetting.tabsystem.fmaudio.fmhemline1.btnshow10");
	m_checkShowTradeName			= RTW_WIDGET_T(CUiCheckButton,"fmnewsetting.fmsetting.tabsystem.fmaudio.fmhemline1.btnshow6");
	m_checkShowSelfSpeak			= RTW_WIDGET_T(CUiCheckButton,"fmnewsetting.fmsetting.tabsystem.fmaudio.fmhemline1.btnshow11");
	m_lockcamera					= RTW_WIDGET_T(CUiCheckButton,"fmnewsetting.fmsetting.tabsystem.fmaudio.fmhemline2.btnoperating1");
	m_checkSkillShotcut				= RTW_WIDGET_T(CUiCheckButton,"fmnewsetting.fmsetting.tabsystem.fmaudio.fmhemline2.btnoperating2");
	m_btnResDefault					= RTW_WIDGET_T(RtwButton,"fmnewsetting.fmsetting.tabsystem.fmaudio.fmhemline2.btndefault");
	m_checkRefuseTeam				= RTW_WIDGET_T(CUiCheckButton,"fmnewsetting.fmsetting.tabsystem.fmaudio.fmhemline3.btninteraction1");
	m_checkRefuseAction				= RTW_WIDGET_T(CUiCheckButton,"fmnewsetting.fmsetting.tabsystem.fmaudio.fmhemline3.btninteraction4");
	m_checkRefuseMail				= RTW_WIDGET_T(CUiCheckButton,"fmnewsetting.fmsetting.tabsystem.fmaudio.fmhemline3.btninteraction2");
	m_checkRefuseDuel				= RTW_WIDGET_T(CUiCheckButton,"fmnewsetting.fmsetting.tabsystem.fmaudio.fmhemline3.btninteraction5");
	m_checkRefuseFriend				= RTW_WIDGET_T(CUiCheckButton,"fmnewsetting.fmsetting.tabsystem.fmaudio.fmhemline3.btninteraction3");
	m_checkRefuseMonster			= RTW_WIDGET_T(CUiCheckButton,"fmnewsetting.fmsetting.tabsystem.fmaudio.fmhemline3.btninteraction6");
	// textuersetting
	m_checkTextureQuality1			= RTW_WIDGET_T(CUiCheckButton,"fmexpertsetting.btnlow1");
	m_checkTextureQuality2			= RTW_WIDGET_T(CUiCheckButton,"fmexpertsetting.btnmedium1");
	m_checkTextureQuality3			= RTW_WIDGET_T(CUiCheckButton,"fmexpertsetting.btnhigh1");
	// effectsetting
	m_checkEffectQuality1			= RTW_WIDGET_T(CUiCheckButton,"fmexpertsetting.btnlow2");
	m_checkEffectQuality2			= RTW_WIDGET_T(CUiCheckButton,"fmexpertsetting.btnmedium2");
	m_checkEffectQuality3			= RTW_WIDGET_T(CUiCheckButton,"fmexpertsetting.btnhigh2");
	// shadowsetting
	m_checkShadowQuality1			= RTW_WIDGET_T(CUiCheckButton,"fmexpertsetting.btnlow3");
	m_checkShadowQuality2			= RTW_WIDGET_T(CUiCheckButton,"fmexpertsetting.btnmedium3");
	m_checkShadowQuality3			= RTW_WIDGET_T(CUiCheckButton,"fmexpertsetting.btnhigh3");

	m_lstResolution->EvSelect		+= RTW_CALLBACK(this, UIFormSetting, OnSelect_Resolution);
	m_hsBarVideo->SetData(0,3,0);
	m_hsBarVideo->EvScroll			+= RTW_CALLBACK(this,UIFormSetting,OnScroll_VideoVolume);
	m_musicVolume->SetRange(0, 100);	
	m_musicVolume->EvScroll			+= RTW_CALLBACK(this, UIFormSetting, OnScroll_MusicVolume);
	m_soundVolume->SetRange(0, 100);	
	m_soundVolume->EvScroll			+= RTW_CALLBACK(this,UIFormSetting,OnScroll_SoundVolume);

	m_checkFullScreen->EvLClick		+= RTW_CALLBACK(this,UIFormSetting,OnCheckButtonFullScreenOrNo);
	m_checkWindows->EvLClick		+= RTW_CALLBACK(this,UIFormSetting,OnCheckButtonFullScreenOrNo);
	m_expertButton->EvLClick		+= RTW_CALLBACK(this,UIFormSetting,OnClick_Expert);
	m_checkMusicSilence->EvLClick	+= RTW_CALLBACK(this,UIFormSetting,OnCheckButtonSilence);
	m_checkSoundSilence->EvLClick	+= RTW_CALLBACK(this,UIFormSetting,OnCheckButtonSilence);
	m_checkTextureQuality1->EvLClick	+= RTW_CALLBACK(this,UIFormSetting,OnCheckButtonTexture);
	m_checkTextureQuality2->EvLClick	+= RTW_CALLBACK(this,UIFormSetting,OnCheckButtonTexture);
	m_checkTextureQuality3->EvLClick	+= RTW_CALLBACK(this,UIFormSetting,OnCheckButtonTexture);
	m_checkEffectQuality1->EvLClick	+= RTW_CALLBACK(this,UIFormSetting,OnCheckButtonEffect);
	m_checkEffectQuality2->EvLClick	+= RTW_CALLBACK(this,UIFormSetting,OnCheckButtonEffect);
	m_checkEffectQuality3->EvLClick	+= RTW_CALLBACK(this,UIFormSetting,OnCheckButtonEffect);
	m_checkShadowQuality1->EvLClick	+= RTW_CALLBACK(this,UIFormSetting,OnCheckButtonShadow);
	m_checkShadowQuality2->EvLClick	+= RTW_CALLBACK(this,UIFormSetting,OnCheckButtonShadow);
	m_checkShadowQuality3->EvLClick	+= RTW_CALLBACK(this,UIFormSetting,OnCheckButtonShadow);
	m_checkSkillShotcut->EvLClick	+= RTW_CALLBACK(this,UIFormSetting,OnCheckButtonShowSkillShotcut);
	//m_checkShowSelfName->EvLClick	+= RTW_CALLBACK(this,UIFormSetting,OnCheckButtonShow);
	//m_checkShowBlood->EvLClick		+= RTW_CALLBACK(this,UIFormSetting,OnCheckButtonShow);
	//m_checkShowDegree->EvLClick		+= RTW_CALLBACK(this,UIFormSetting,OnCheckButtonShow);
	//m_checkShowDamage->EvLClick		+= RTW_CALLBACK(this,UIFormSetting,OnCheckButtonShow);
	//m_checkShowUnionName->EvLClick	+= RTW_CALLBACK(this,UIFormSetting,OnCheckButtonShow);
	//m_checkShowEXP->EvLClick		+= RTW_CALLBACK(this,UIFormSetting,OnCheckButtonShow);
	//m_checkShowOtherName->EvLClick	+= RTW_CALLBACK(this,UIFormSetting,OnCheckButtonShow);
	//m_checkShowNPCName->EvLClick	+= RTW_CALLBACK(this,UIFormSetting,OnCheckButtonShow);
	//m_checkShowOtherSpeak->EvLClick	+= RTW_CALLBACK(this,UIFormSetting,OnCheckButtonShow);
	//m_checkShowActionName->EvLClick	+= RTW_CALLBACK(this,UIFormSetting,OnCheckButtonShow);
	//m_checkShowSelfSpeak->EvLClick	+= RTW_CALLBACK(this,UIFormSetting,OnCheckButtonShow);

	m_lockcamera->EvLClick			+= RTW_CALLBACK(this,UIFormSetting,OnCheckButtonCamera);
	m_btnResDefault->EvLClick		+= RTW_CALLBACK(this,UIFormSetting,OnClick_DefaultCamera);
	//定制中的确定按钮
	RTW_WIDGET("fmexpertsetting.Yes")->EvLClick +=RTW_CALLBACK(this,UIFormSetting,OnExpertClick_OK);
	//定制中的取消按钮
	RTW_WIDGET("fmexpertsetting.No")->EvLClick +=RTW_CALLBACK(this,UIFormSetting,OnExpertClick_Cancel);

	RTW_WIDGET("fmnewsetting.fmsetting.btnok")->EvLClick += RTW_CALLBACK(this, UIFormSetting, OnClick_OK);
	RTW_WIDGET("fmnewsetting.fmsetting.btncancel")->EvLClick += RTW_CALLBACK(this, UIFormSetting, OnClick_Cancel);
	//RTW_WIDGET("fmnewsetting.fmsetting.btnapply")->EvLClick += RTW_CALLBACK(this, UIFormSetting, OnClick_Apply);
	//RTW_WIDGET("fmnewsetting.fmsetting.tabsystem.fmgame.btndefault")->EvLClick += 
	//	RTW_CALLBACK(this, UIFormSetting, OnClick_DefaultCamera);		
	//if (WorldIsInited())
	//	RTW_WIDGET("fmnewsetting.fmsetting.tabsystem.btngame")->Show();
	//else
	//	RTW_WIDGET("fmnewsetting.fmsetting.tabsystem.btngame")->Hide();
	InitResolution();
	RTW_WIDGET_T(RtwTab, "fmnewsetting.fmsetting.tabsystem")->SetSelectedItem(0);
	RTW_WIDGET("fmnewsetting")->Hide();

	RtIni iniGame;
    if (iniGame.OpenFile(R(INI_GAME)))
    {
        long lV;
        iniGame.GetEntry("graph_low", "texture", &lV);    m_oldSetting.graphLow.sTexture     = (short)lV;
        iniGame.GetEntry("graph_low", "effect", &lV);     m_oldSetting.graphLow.sEffect      = (short)lV;
        iniGame.GetEntry("graph_low", "shadow", &lV);     m_oldSetting.graphLow.sShadow      = (short)lV;
        //iniGame.GetEntry("graph_low", "fsaa", &lV);       m_oldSetting.graphLow.bFsaa        = lV!=0;
        //iniGame.GetEntry("graph_low", "envref", &lV);     m_oldSetting.graphLow.bEnvref      = lV!=0;
        //iniGame.GetEntry("graph_low", "objref", &lV);     m_oldSetting.graphLow.bObjref      = lV!=0;
        //iniGame.GetEntry("graph_low", "posteffect", &lV); m_oldSetting.graphLow.bPosteffect  = lV!=0;
        //iniGame.GetEntry("graph_low", "objeffect", &lV);  m_oldSetting.graphLow.bObjeffect   = lV!=0;

        iniGame.GetEntry("graph_middle1", "texture", &lV);    m_oldSetting.graphMiddle1.sTexture     = (short)lV;
        iniGame.GetEntry("graph_middle1", "effect", &lV);     m_oldSetting.graphMiddle1.sEffect      = (short)lV;
        iniGame.GetEntry("graph_middle1", "shadow", &lV);     m_oldSetting.graphMiddle1.sShadow      = (short)lV;
        //iniGame.GetEntry("graph_middle", "fsaa", &lV);       m_oldSetting.graphMiddle1.bFsaa        = lV!=0;
        //iniGame.GetEntry("graph_middle", "envref", &lV);     m_oldSetting.graphMiddle1.bEnvref      = lV!=0;
        //iniGame.GetEntry("graph_middle", "objref", &lV);     m_oldSetting.graphMiddle1.bObjref      = lV!=0;
        //iniGame.GetEntry("graph_middle", "posteffect", &lV); m_oldSetting.graphMiddle1.bPosteffect  = lV!=0;
        //iniGame.GetEntry("graph_middle", "objeffect", &lV);  m_oldSetting.graphMiddle1.bObjeffect   = lV!=0;

		iniGame.GetEntry("graph_middle2", "texture", &lV);    m_oldSetting.graphMiddle2.sTexture     = (short)lV;
		iniGame.GetEntry("graph_middle2", "effect", &lV);     m_oldSetting.graphMiddle2.sEffect      = (short)lV;
		iniGame.GetEntry("graph_middle2", "shadow", &lV);     m_oldSetting.graphMiddle2.sShadow      = (short)lV;

        iniGame.GetEntry("graph_high", "texture", &lV);    m_oldSetting.graphHigh.sTexture     = (short)lV;
        iniGame.GetEntry("graph_high", "effect", &lV);     m_oldSetting.graphHigh.sEffect      = (short)lV;
		iniGame.GetEntry("graph_high", "shadow", &lV);     m_oldSetting.graphHigh.sShadow      = (short)lV;
        //iniGame.GetEntry("graph_high", "fsaa", &lV);       m_oldSetting.graphHigh.bFsaa        = lV!=0;
        //iniGame.GetEntry("graph_high", "envref", &lV);     m_oldSetting.graphHigh.bEnvref      = lV!=0;
        //iniGame.GetEntry("graph_high", "objref", &lV);     m_oldSetting.graphHigh.bObjref      = lV!=0;
        //iniGame.GetEntry("graph_high", "posteffect", &lV); m_oldSetting.graphHigh.bPosteffect  = lV!=0;
        //iniGame.GetEntry("graph_high", "objeffect", &lV);  m_oldSetting.graphHigh.bObjeffect   = lV!=0;
        iniGame.CloseFile();
    }
	RtIni iniUser;
	int lV;
	if(iniUser.OpenFile(R(INI_USER)))
	{
		if(iniUser.FindSection("setting_show"))
		{
			iniUser.GetEntry("setting_show", "ShowSelfName", &lV);
			g_layerMain->m_eSetting.m_bShowSelfName = (bool)lV;
			iniUser.GetEntry("setting_show", "ShowBlood", &lV);
			g_layerMain->m_eSetting.m_bShowBlood = (bool)lV;
			iniUser.GetEntry("setting_show", "ShowDegree", &lV);
			g_layerMain->m_eSetting.m_bShowDegree = (bool)lV;
			iniUser.GetEntry("setting_show", "ShowDamage", &lV);
			g_layerMain->m_eSetting.m_bShowDamage = (bool)lV;
			iniUser.GetEntry("setting_show", "ShowUnionName", &lV);
			g_layerMain->m_eSetting.m_bShowUnionName = (bool)lV;
			iniUser.GetEntry("setting_show", "ShowEXP", &lV);
			g_layerMain->m_eSetting.m_bShowEXP = (bool)lV;
			iniUser.GetEntry("setting_show", "ShowOtherName", &lV);
			g_layerMain->m_eSetting.m_bShowOtherName = (bool)lV;
			iniUser.GetEntry("setting_show", "ShowNPCName", &lV);
			g_layerMain->m_eSetting.m_bShowNPCName = (bool)lV;
			iniUser.GetEntry("setting_show", "OtherSpeak", &lV);
			g_layerMain->m_eSetting.m_bShowOtherSpeak = (bool)lV;
			iniUser.GetEntry("setting_show", "ShowActionName", &lV);
			g_layerMain->m_eSetting.m_bShowTradeName = (bool)lV;
			iniUser.GetEntry("setting_show", "ShowSelfSpeak", &lV);
			g_layerMain->m_eSetting.m_bShowSelfSpeak = (bool)lV;
			iniUser.GetEntry("setting_show", "ShowSkillShotcut", &lV);
			g_layerMain->m_eSetting.m_bShowSkillShotcut = (bool)lV;
		}else
		{
			g_layerMain->m_eSetting.m_bShowSelfName				= true;
			g_layerMain->m_eSetting.m_bShowBlood				= false;
			g_layerMain->m_eSetting.m_bShowDegree				= true;
			g_layerMain->m_eSetting.m_bShowDamage				= true;
			g_layerMain->m_eSetting.m_bShowUnionName			= true;
			g_layerMain->m_eSetting.m_bShowEXP					= true;
			g_layerMain->m_eSetting.m_bShowOtherName			= true;
			g_layerMain->m_eSetting.m_bShowNPCName				= true;
			g_layerMain->m_eSetting.m_bShowOtherSpeak			= true;
			g_layerMain->m_eSetting.m_bShowTradeName			= true;
			g_layerMain->m_eSetting.m_bShowSelfSpeak			= true;
			g_layerMain->m_eSetting.m_bShowSkillShotcut			= false;
		}
		if(iniUser.FindSection("setting_refuse"))
		{
			iniUser.GetEntry("setting_refuse", "RefuseTeam", &lV);
			g_layerMain->m_eSetting.m_bRefuseTeam = (bool)lV;
			iniUser.GetEntry("setting_refuse", "RefuseAction", &lV);
			g_layerMain->m_eSetting.m_bRefuseAction = (bool)lV;
			iniUser.GetEntry("setting_refuse", "RefuseMail", &lV);
			g_layerMain->m_eSetting.m_bRefuseMail = (bool)lV;
			iniUser.GetEntry("setting_refuse", "RefuseDuel", &lV);
			g_layerMain->m_eSetting.m_bRefuseDuel = (bool)lV;
			iniUser.GetEntry("setting_refuse", "RefuseFriend", &lV);
			g_layerMain->m_eSetting.m_bRefuseFriend = (bool)lV;
			iniUser.GetEntry("setting_refuse", "RefuseMonster", &lV);
			g_layerMain->m_eSetting.m_bRefuseMonster = (bool)lV;
		}else
		{
			g_layerMain->m_eSetting.m_bRefuseTeam		= false;
			g_layerMain->m_eSetting.m_bRefuseAction		= false;
			g_layerMain->m_eSetting.m_bRefuseMail		= false;
			g_layerMain->m_eSetting.m_bRefuseDuel		= false;
			g_layerMain->m_eSetting.m_bRefuseFriend		= false;
			g_layerMain->m_eSetting.m_bRefuseMonster	= false;
		}
		if(iniUser.FindSection("audio"))
		{
			iniUser.GetEntry("audio", "music", &lV);
			m_oldSetting.audio.iMusicVal = lV;
			if(iniUser.GetEntry("audio", "BackGroundMusicIsSlience", &lV))
				g_layerMain->m_eSetting.m_bBackGroundMusicIsSlience = (bool)lV;
			else
				g_layerMain->m_eSetting.m_bBackGroundMusicIsSlience = false;
			iniUser.GetEntry("audio", "sound", &lV);
			m_oldSetting.audio.iSoundVal = lV;
			if(iniUser.GetEntry("audio", "SoundIsSlience", &lV))
				g_layerMain->m_eSetting.m_bSoundIsSlience = (bool)lV;
			else
				g_layerMain->m_eSetting.m_bSoundIsSlience			= false;
		}else
		{
			m_oldSetting.audio.iMusicVal						= 75;
			m_oldSetting.audio.iSoundVal						= 75;
			g_layerMain->m_eSetting.m_bBackGroundMusicIsSlience = false;
			g_layerMain->m_eSetting.m_bSoundIsSlience			= false;
		}
		iniUser.CloseFile();
	}
	RTW_WIDGET("fmshortcut_bar1")->Show();
	if(g_layerMain->m_eSetting.m_bShowSkillShotcut)
		RTW_WIDGET("fmshortcut_bar2")->Show();
	else
		RTW_WIDGET("fmshortcut_bar2")->Hide();
	RtwHUD::s_cShowItem = 0;
	RtwHUD::s_cShowItem = RtwHUD::s_cShowItem | RtwHUD::siName | RtwHUD::siSpeak | RtwHUD::siPetAbility | RtwHUD::siPetFuseLev | RtwHUD::siPetMaster;
	if(g_layerMain->m_eSetting.m_bShowUnionName)
		RtwHUD::s_cShowItem |= RtwHUD::siGuild;
	if(g_layerMain->m_eSetting.m_bShowDegree)
		RtwHUD::s_cShowItem |= RtwHUD::siNick;
	if(g_layerMain->m_eSetting.m_bShowTradeName)
		RtwHUD::s_cShowItem |= RtwHUD::esiStall;
	GetPlayer()->m_pHUD->ShowNameSeparate(g_layerMain->m_eSetting.m_bShowSelfName);
	m_bChanged = false;
	unguard;
}

UIFormSetting::~UIFormSetting()
{
	guard;
	if (m_confirmVideoTimer)
		GetTimer()->RemoveTimer(m_confirmVideoTimer);
	unguard;
}

void UIFormSetting::Change()
{
	if(m_frmThis->IsVisible())
		m_frmThis->Hide();
	else
	{
		RefreshAudioSettings();
		RefreshGameSettings();
		RefreshVideoSettings();
		RefreshShowSettings();
		RefreshRefuseSettings();
		m_frmThis->Show();
	}
}

void UIFormSetting::Show()
{
	guard;
	if(m_frmThis->IsVisible())
		m_frmThis->Hide();
	else
	{
		RefreshAudioSettings();
		RefreshGameSettings();
		RefreshVideoSettings();
		RefreshShowSettings();
		RefreshRefuseSettings();
		m_frmThis->Show();
	}
	unguard;
}

void UIFormSetting::OnExpertClick_OK(void*,void*)
{
	guard;

    //m_oldSetting.graph.sTexture = (short)m_textureQuality->GetSelectedItem();
    //m_oldSetting.graph.sEffect  = (short)m_effectQuality->GetSelectedItem();
    //m_oldSetting.graph.sShadow  = (short)m_shadowQuality->GetSelectedItem();
    //m_oldSetting.graph.bFsaa    = m_sawtooth->GetChecked();
    //m_oldSetting.graph.bEnvref  = m_reflect->GetChecked();
    //m_oldSetting.graph.bObjref  = m_reflectpeople->GetChecked();
    //m_oldSetting.graph.bPosteffect = m_effectPost->GetChecked();
    //m_oldSetting.graph.bObjeffect  = m_effectObject->GetChecked();

    //if (m_oldSetting.graph.Same(m_oldSetting.graphHigh) )
    //{
    //    m_graphQuality->SetSelectedItem(0);
    //}else if (m_oldSetting.graph.Same(m_oldSetting.graphMiddle1) )
    //{
    //    m_graphQuality->SetSelectedItem(1);
    //}else if (m_oldSetting.graph.Same(m_oldSetting.graphLow) )
    //{
    //    m_graphQuality->SetSelectedItem(2);
    //}else
    //{
    //    m_graphQuality->SetSelectedItem(3); //定制选项
    //}

	m_ExpertForm->Hide();

	unguard;
}

void UIFormSetting::OnExpertClick_Cancel(void*,void*)
{
	guard;
	RefreshVideoSettings();
	m_ExpertForm->Hide();
	unguard;
}


void UIFormSetting::OnClick_OK(void*, void*)
{
	guard;
	m_frmThis->Hide();
	RtIni ini;
	if (!ini.OpenFile(R(INI_USER), true))
	{
		ERR("ERROR: 不能写入配置文件 [user.ini]\n");
		return;
	}
#define INI_ADD_ENTRY(s, i, v)  if (!ini.SetEntry(s, i, v)) ini.AddEntry(s, i, v)
	// 图形高级选项
	if(m_checkLight->GetChecked())
	{
		g_pScene->m_bReflectSceneObject = true;
		m_oldSetting.light.bEnvref = true;
		g_pScene->m_bReflectOtherActor = true;
		m_oldSetting.light.bObjref = true;
	}else
	{
		g_pScene->m_bReflectSceneObject = false;
		m_oldSetting.light.bEnvref = false;
		g_pScene->m_bReflectOtherActor = false;
		m_oldSetting.light.bObjref = false;
	}

	if(m_checkFullScreenLight->GetChecked())
	{
		GetDevice()->SetPostProcessEffectOnoff(true);
		m_oldSetting.light.bPosteffect = true;
		GetDevice()->SetPostProcessObjectHighLightOnoff(true);
		m_oldSetting.light.bObjeffect = true;
	}else
	{
		GetDevice()->SetPostProcessEffectOnoff(false);
		m_oldSetting.light.bPosteffect = false;
		GetDevice()->SetPostProcessObjectHighLightOnoff(false);
		m_oldSetting.light.bObjeffect = false;
	}

	if (WorldIsInited())
	{
		m_oldSetting.game.bLockCamera = m_lockcamera->GetChecked();
		GetWorld()->SetCameraMode(m_oldSetting.game.bLockCamera?GcWorld::CAMERA_MODE_LOCK:GcWorld::CAMERA_MODE_TURN);

		OnSaveShowSettings();
		OnSaveRefuseSettings();
		/* gao 2010.1.13
		以下文件修改，以去适应保存文件
		*/
		char* strInputProject = NULL;
		{//设置界面输入模式
			switch(m_oldSetting.game.sUIInputProject)
			{
			case 0://回车模式
				if(!strInputProject)
					strInputProject = RT_NEW char[20];
				memcpy(strInputProject,"ENTER_KEY_TO_CHAT",sizeof("ENTER_KEY_TO_CHAT"));
				break;
			case 1://直接模式
				if(!strInputProject)
					strInputProject = RT_NEW char[20];
				memcpy(strInputProject,"DIRECT_CHAT",sizeof("DIRECT_CHAT"));
				break;
			default:
				// 将默认输入模式设置为 回车模式
				if(!strInputProject)
					strInputProject = RT_NEW char[20];
				memcpy(strInputProject,"ENTER_KEY_TO_CHAT",sizeof("ENTER_KEY_TO_CHAT"));
				break;
			}
			GetWorld()->m_UIProject.SetInputProject(strInputProject);
		}

		char* strShowProject = NULL;
		{//设置界面显示模式
			switch(m_oldSetting.game.sUIShowProject)
			{
			case 0://正常模式
				if(!strShowProject)
					strShowProject = RT_NEW char[20];
				memcpy(strShowProject,"NORMAL_SHOW",sizeof("NORMAL_SHOW"));
				break;
			case 1://透明模式
				memcpy(strShowProject,"TRANSPARENT_SHOW",sizeof("TRANSPARENT_SHOW"));
				break;
			default:
				// 默认
				if(!strShowProject)
					strShowProject = RT_NEW char[20];
				memcpy(strShowProject,"TRANSPARENT_SHOW",sizeof("TRANSPARENT_SHOW"));
				break;
			}
			GetWorld()->m_UIProject.SetShowProject(strShowProject);
		}
		/* end */
		/* gao 2010.1.13
		将配置输出的到文件中
		*/
		const char* strPerson = "person";
		if (!ini.FindSection(strPerson)) ini.AddSection(strPerson);
		INI_ADD_ENTRY(strPerson, "input", strInputProject);
		INI_ADD_ENTRY(strPerson, "show", strShowProject);
		if(strShowProject)
			DEL_ONE(strShowProject);
		if(strInputProject)
			DEL_ONE(strInputProject);
		/* end */
	}
	// 图形普通选项
	int curSel = m_lstResolution->GetSelectedItem();	
	Resolution& curRes = m_resolutionList[curSel];
	RTGViewWindowMode eMode;
	if (m_checkFullScreen->GetChecked())
	{
		eMode = RTGVWM_TOPWINDOW;
	}else
	{
		eMode = RTGVWM_WINDOWED;
	}
	const RtgViewWindowInfo& info = GetDevice()->GetViewWindowInfo(eMode);
	m_bkViewMode = GetDevice()->GetViewWindowMode();
	m_bkResWidth = info.dwWidth;
	m_bkResHeight = info.dwHeight;
	m_bkColorDepth = GetDevice()->m_iWndColorBits;
	if (m_bkViewMode != eMode ||m_bkResWidth != curRes.width ||m_bkResHeight != curRes.height)
	{
		GetDevice()->SetViewWindowInfo(eMode, curRes.width, curRes.height, m_bkColorDepth, 60);
		GetDevice()->SetViewMode(eMode);
		RTW_WIDGET("fmnewsetting")->CenterScreen();		
		RTW_WIDGET("ConfirmResolution")->ShowModal();
		RTW_WIDGET("ConfirmResolution")->CenterScreen();
		RTW_WIDGET("ConfirmResolution.Yes")->EvLClick = RTW_CALLBACK(this, UIFormSetting, OnClick_ConfirmResolution_Yes);
		RTW_WIDGET("ConfirmResolution.No")->EvLClick = RTW_CALLBACK(this, UIFormSetting, OnClick_ConfirmResolution_No);
		//RTW_WIDGET("ConfirmResolution.$hide")->EvLClick = RTW_CALLBACK(this, UIFormSetting, OnClick_ConfirmResolution_No);
		m_confirmVideoTimeStamp = rtGetMilliseconds();
		m_confirmVideoTimer = GetTimer()->AddTimer(this, 1);
	}
	const char* strGraph = "graph";
	if (!ini.FindSection(strGraph)) ini.AddSection(strGraph);
	INI_ADD_ENTRY(strGraph, "fullscreen", (long)(m_bkViewMode!=RTGVWM_WINDOWED));
	INI_ADD_ENTRY(strGraph, "width", (long)m_bkResWidth);
	INI_ADD_ENTRY(strGraph, "height", (long)m_bkResHeight);
	INI_ADD_ENTRY(strGraph, "color", (long)m_bkColorDepth);
	INI_ADD_ENTRY(strGraph, "texture", (long)m_oldSetting.graph.sTexture);
	INI_ADD_ENTRY(strGraph, "effect", (long)m_oldSetting.graph.sEffect);
	INI_ADD_ENTRY(strGraph, "shadow", (long)m_oldSetting.graph.sShadow);
	//INI_ADD_ENTRY(strGraph, "fsaa", (long)m_oldSetting.graph.bFsaa);
	INI_ADD_ENTRY(strGraph, "envref", (long)m_oldSetting.light.bEnvref);
	INI_ADD_ENTRY(strGraph, "objref", (long)m_oldSetting.light.bObjref);
	INI_ADD_ENTRY(strGraph, "posteffect", (long)m_oldSetting.light.bPosteffect);
	INI_ADD_ENTRY(strGraph, "objeffect", (long)m_oldSetting.light.bObjeffect);

	const char* strAudio = "audio";
	if (!ini.FindSection(strAudio)) ini.AddSection(strAudio);
	if(!g_layerMain->m_eSetting.m_bBackGroundMusicIsSlience)
		m_oldSetting.audio.iMusicVal = (int)m_musicVolume->GetValue();
	INI_ADD_ENTRY(strAudio, "music", (long)m_oldSetting.audio.iMusicVal);
	INI_ADD_ENTRY(strAudio, "BackGroundMusicIsSlience", (long)g_layerMain->m_eSetting.m_bBackGroundMusicIsSlience);
	if(!g_layerMain->m_eSetting.m_bSoundIsSlience)
		m_oldSetting.audio.iSoundVal = (int)m_soundVolume->GetValue();
	INI_ADD_ENTRY(strAudio, "sound", (long)m_oldSetting.audio.iSoundVal);
	INI_ADD_ENTRY(strAudio, "SoundIsSlience", (long)g_layerMain->m_eSetting.m_bSoundIsSlience);
	INI_ADD_ENTRY(strAudio, "use3d", (long)m_oldSetting.audio.bUse3D);

	const char* strShowSetting = "setting_show";
	if(!ini.FindSection(strShowSetting))
		ini.AddSection(strShowSetting);
	INI_ADD_ENTRY(strShowSetting, "ShowSelfName", (long)g_layerMain->m_eSetting.m_bShowSelfName);
	INI_ADD_ENTRY(strShowSetting, "ShowBlood", (long)g_layerMain->m_eSetting.m_bShowBlood);
	INI_ADD_ENTRY(strShowSetting, "ShowDegree", (long)g_layerMain->m_eSetting.m_bShowDegree);
	INI_ADD_ENTRY(strShowSetting, "ShowDamage", (long)g_layerMain->m_eSetting.m_bShowDamage);
	INI_ADD_ENTRY(strShowSetting, "ShowUnionName", (long)g_layerMain->m_eSetting.m_bShowUnionName);
	INI_ADD_ENTRY(strShowSetting, "ShowEXP", (long)g_layerMain->m_eSetting.m_bShowEXP);
	INI_ADD_ENTRY(strShowSetting, "ShowOtherName", (long)g_layerMain->m_eSetting.m_bShowOtherName);
	INI_ADD_ENTRY(strShowSetting, "ShowNPCName", (long)g_layerMain->m_eSetting.m_bShowNPCName);
	INI_ADD_ENTRY(strShowSetting, "OtherSpeak", (long)g_layerMain->m_eSetting.m_bShowOtherSpeak);
	INI_ADD_ENTRY(strShowSetting, "ShowActionName", (long)g_layerMain->m_eSetting.m_bShowTradeName);
	INI_ADD_ENTRY(strShowSetting, "ShowSelfSpeak", (long)g_layerMain->m_eSetting.m_bShowSelfSpeak);
	INI_ADD_ENTRY(strShowSetting, "ShowSkillShotcut", (long)g_layerMain->m_eSetting.m_bShowSkillShotcut);

	const char* strRefuseSetting = "setting_refuse";
	if(!ini.FindSection(strRefuseSetting))
		ini.AddSection(strRefuseSetting);
	INI_ADD_ENTRY(strRefuseSetting, "RefuseTeam", (long)g_layerMain->m_eSetting.m_bRefuseTeam);
	INI_ADD_ENTRY(strRefuseSetting, "RefuseAction", (long)g_layerMain->m_eSetting.m_bRefuseAction);
	INI_ADD_ENTRY(strRefuseSetting, "RefuseMail", (long)g_layerMain->m_eSetting.m_bRefuseMail);
	INI_ADD_ENTRY(strRefuseSetting, "RefuseDuel", (long)g_layerMain->m_eSetting.m_bRefuseDuel);
	INI_ADD_ENTRY(strRefuseSetting, "RefuseFriend", (long)g_layerMain->m_eSetting.m_bRefuseFriend);
	INI_ADD_ENTRY(strRefuseSetting, "RefuseMonster", (long)g_layerMain->m_eSetting.m_bRefuseMonster);
	ini.CloseFile();

	if (g_pScene)
	{
		g_pScene->m_camMatPrev.Unit();
	}
	unguard;
}

void UIFormSetting::OnClick_Cancel(void*, void*)
{
	guard;

	if(!g_layerMain->m_eSetting.m_bBackGroundMusicIsSlience)
	{
		g_pBackMusic->SetVolume(m_oldSetting.audio.iMusicVal*0.01);
	}else
	{
		g_pBackMusic->SetVolume(0.f);
	}
	if(!g_layerMain->m_eSetting.m_bSoundIsSlience)
	{
		g_pSoundMgr->SetWholeVolume(m_oldSetting.audio.iSoundVal*0.01);
	}else
	{
		g_pSoundMgr->SetWholeVolume(0.f);
	}

  //  if (WorldIsInited())
  //  {
  //      GetWorld()->m_ActorManager.SetMaxDisplayChar(m_oldSetting.game.cMaxDisplay);

  //      RtwHUD::s_cShowItem = 0;
  //      if (m_displayhud->GetChecked())
  //      {
  //          if (m_hudSay->GetChecked())     RtwHUD::s_cShowItem |= RtwHUD::siSpeak;
  //          if (m_hudName->GetChecked())    RtwHUD::s_cShowItem |= RtwHUD::siName;
  //          if (m_hudGuild->GetChecked())   RtwHUD::s_cShowItem |= RtwHUD::siGuild;
  //          if (m_hudNick->GetChecked())    RtwHUD::s_cShowItem |= RtwHUD::siNick;
  //          //if (m_hudCountry->GetChecked()) RtwHUD::s_cShowItem |= RtwHUD::siCountry;

  //          RtwHUD::s_cShowItem |= RtwHUD::siMsg;
  //      }

		////
  //  }
	RTW_WIDGET("fmnewsetting")->Hide();
	unguard;
}


void UIFormSetting::OnClick_Apply(void*, void*)
{
	guard;
	unguard;
}
void UIFormSetting::SetGemeFullScreenOrNo()
{
	guard;
	m_bkViewMode = GetDevice()->GetViewWindowMode();

	if(m_bkViewMode == RTGVWM_TOPWINDOW)
		GetDevice()->SetViewMode(RTGVWM_WINDOWED);
	else
		GetDevice()->SetViewMode(RTGVWM_TOPWINDOW);

	unguard;
}

void UIFormSetting::OnScroll_UsersInScreen(void* ,void*)
{
    guard;
    //if (WorldIsInited())
    //{
    //    GetWorld()->m_ActorManager.SetMaxDisplayChar(m_usersInScreen->GetValue());
    //}
    unguard;
}

void UIFormSetting::OnScroll_VideoVolume(void*, void*)
{
	guard;
	switch(m_hsBarVideo->GetValue())
	{
	case 3:
		m_oldSetting.graph = m_oldSetting.graphLow;
		break;
	case 2:
		m_oldSetting.graph = m_oldSetting.graphMiddle1;
		break;
	case 1:
		m_oldSetting.graph = m_oldSetting.graphMiddle2;
		break;
	case 0:
		m_oldSetting.graph = m_oldSetting.graphHigh;
		break;
	default:
		m_oldSetting.graph = m_oldSetting.graphLow;
		break;
	}
	switch(m_oldSetting.graph.sTexture)
	{
	case 0:
		m_checkTextureQuality1->SetChecked(false);
		m_checkTextureQuality2->SetChecked(false);
		m_checkTextureQuality3->SetChecked(true);
		break;
	case 1:
		m_checkTextureQuality1->SetChecked(false);
		m_checkTextureQuality2->SetChecked(true);
		m_checkTextureQuality3->SetChecked(false);
		break;
	case 2:
		m_checkTextureQuality1->SetChecked(true);
		m_checkTextureQuality2->SetChecked(false);
		m_checkTextureQuality3->SetChecked(false);
		break;
	default:
		m_oldSetting.graph.sTexture = 2;
		m_checkTextureQuality1->SetChecked(true);
		m_checkTextureQuality2->SetChecked(false);
		m_checkTextureQuality3->SetChecked(false);
		break;
	}
	switch(m_oldSetting.graph.sEffect)
	{
	case 0:
		m_checkEffectQuality1->SetChecked(false);
		m_checkEffectQuality2->SetChecked(false);
		m_checkEffectQuality3->SetChecked(true);
		break;
	case 1:
		m_checkEffectQuality1->SetChecked(false);
		m_checkEffectQuality2->SetChecked(true);
		m_checkEffectQuality3->SetChecked(false);
		break;
	case 2:
		m_checkEffectQuality1->SetChecked(true);
		m_checkEffectQuality2->SetChecked(false);
		m_checkEffectQuality3->SetChecked(false);
		break;
	default:
		m_oldSetting.graph.sEffect = 2;
		m_checkEffectQuality1->SetChecked(true);
		m_checkEffectQuality2->SetChecked(false);
		m_checkEffectQuality3->SetChecked(false);
		break;
	}
	switch(m_oldSetting.graph.sShadow)
	{
	case 0:
		m_checkShadowQuality1->SetChecked(false);
		m_checkShadowQuality2->SetChecked(false);
		m_checkShadowQuality3->SetChecked(true);
		break;
	case 1:
		m_checkShadowQuality1->SetChecked(false);
		m_checkShadowQuality2->SetChecked(true);
		m_checkShadowQuality3->SetChecked(false);
		break;
	case 2:
		m_checkShadowQuality1->SetChecked(true);
		m_checkShadowQuality2->SetChecked(false);
		m_checkShadowQuality3->SetChecked(false);
		break;
	default:
		m_oldSetting.graph.sShadow = 2;
		m_checkShadowQuality1->SetChecked(true);
		m_checkShadowQuality2->SetChecked(false);
		m_checkShadowQuality3->SetChecked(false);
		break;
	}
	unguard;
}

void UIFormSetting::OnScroll_MusicVolume(void*, void*)
{
	guard;
	if(!m_checkMusicSilence->GetChecked())
		g_pBackMusic->SetVolume(m_musicVolume->GetValue() / 100.0f);
	unguard;
}

void UIFormSetting::OnScroll_SoundVolume(void*, void*)
{
	guard;
	if(!m_checkSoundSilence->GetChecked())
		g_pSoundMgr->SetWholeVolume(m_soundVolume->GetValue()/100.0f);
	unguard;
}

void UIFormSetting::OnSelect_Resolution(void*, void*)
{
	guard;
	int cursel = m_lstResolution->GetSelectedItem();	
	if (cursel == -1)
		return;
	unguard;
}

void UIFormSetting::Hide()
{
}

void UIFormSetting::InitResolution()
{
	guard;

    int size = GetDevice()->GetDisplayMode(NULL);

	if (size)
	{		

		RtgDisplayMode* dm = RT_NEW RtgDisplayMode[size];
        GetDevice()->GetDisplayMode(dm);
		for (int n = 0; n < size; n++)
		{
			int id = (dm[n].dwWidth << 16) | dm[n].dwHeight;
			std::vector<Resolution>::iterator iter = m_resolutionList.begin();

			for(; iter != m_resolutionList.end(); ++iter)
			{
				if (iter->width ==dm[n].dwWidth && iter->height == dm[n].dwHeight)
					break;
			}

			if (iter == m_resolutionList.end())
			{
				bool bCanAdd = false;
				if (dm[n].dwWidth==1280 || dm[n].dwWidth==1024)
				{
					if (dm[n].dwWidth==1280)
					{
						if (dm[n].dwHeight==800 || dm[n].dwHeight == 1024)
						{
							bCanAdd = true;
						}
					}else if (dm[n].dwWidth==1024)
					{
						if (dm[n].dwHeight==768)
						{
							bCanAdd = true;
						}
					}
					if (bCanAdd)
					{
						if (dm[n].dwDepth!=16 && dm[n].dwDepth!=32)
						{
							bCanAdd = false;
						}
					}
				}
				if (bCanAdd)
				{
					m_resolutionList.push_back(Resolution());
					m_resolutionList.back().width = dm[n].dwWidth;
					m_resolutionList.back().height = dm[n].dwHeight;
					m_resolutionList.back().depth.push_back(SDepth());
					m_resolutionList.back().depth.back().depth = dm[n].dwDepth;
					m_resolutionList.back().depth.back().rate.push_back(dm[n].dwRefreshRate);
				}
			}
			else
			{
				std::vector<SDepth>::iterator iterDepth = iter->depth.begin();
				for(; iterDepth != iter->depth.end(); ++iterDepth)
				{
					if (iterDepth->depth==dm[n].dwDepth)
						break;
				}
				if (iterDepth == iter->depth.end())
				{
					iter->depth.push_back(SDepth());
					iter->depth.back().depth = dm[n].dwDepth;
					iter->depth.back().rate.push_back(dm[n].dwRefreshRate);
				}else
				{
					iterDepth->rate.push_back(dm[n].dwRefreshRate);
				}
			}
		}
		DEL_ONE(dm)

	}
	unguard;
}

void UIFormSetting::OnClick_Expert(ui::RtwWidget* vpSender, RtwEventDelegate* )
{
	guard;
	m_ExpertForm->Show();
	g_workspace.SetFocusWidget(m_ExpertForm);
	unguard;
}

void UIFormSetting::On_HudChange(ui::RtwWidget* vpSender, RtwEventDelegate* )
{
	guard;

    //if (vpSender==m_displayhud)
    //{
    //    m_hudSay->SetChecked(m_displayhud->GetChecked());
    //    m_hudName->SetChecked(m_displayhud->GetChecked());
    //    m_hudGuild->SetChecked(m_displayhud->GetChecked());
    //    m_hudNick->SetChecked(m_displayhud->GetChecked());
    //    //m_hudCountry->SetChecked(m_displayhud->GetChecked());
    //}

    //char cHUD = RtwHUD::s_cShowItem;
    //RtwHUD::s_cShowItem = 0;
    //if (m_hudSay->GetChecked())     RtwHUD::s_cShowItem |= RtwHUD::siSpeak;
    //if (m_hudName->GetChecked())    RtwHUD::s_cShowItem |= RtwHUD::siName;
    //if (m_hudGuild->GetChecked())   RtwHUD::s_cShowItem |= RtwHUD::siGuild;
    //if (m_hudNick->GetChecked())    RtwHUD::s_cShowItem |= RtwHUD::siNick;
    ////if (m_hudCountry->GetChecked()) RtwHUD::s_cShowItem |= RtwHUD::siCountry;
    //m_displayhud->SetChecked(RtwHUD::s_cShowItem!=0);

    //RtwHUD::s_cShowItem |= RtwHUD::siMsg;

    //if (cHUD!=RtwHUD::s_cShowItem)
    //{
    //    if (GetWorld())
    //    {
    //        GetWorld()->OnHUDConfigChanged();
    //    }
    //}
	unguard;
}

void UIFormSetting::OnClick_DefaultCamera(void*, void*)
{
	guard;
	if (GetWorld())
	{
		GetWorld()->SetDefaultCamerasetting();
	}
	RefreshGameSettings();
	unguard;
}

void SettingSaveResolutionInfo()
{
	guard;

	RtIni ini;
	if (!ini.OpenFile(R(INI_USER), true))
	{
		return;
	}

	RTGViewWindowMode eMode = GetDevice()->GetViewWindowMode();
	const RtgViewWindowInfo& info = GetDevice()->GetViewWindowInfo(eMode);

    ini.SetEntry("graph", "fullscreen", (eMode!=RTGVWM_WINDOWED)?(long)1:(long)0);
	ini.SetEntry("graph", "width", (long)info.dwWidth);
	ini.SetEntry("graph", "height", (long)info.dwHeight);
	ini.SetEntry("graph", "color", (long)info.dwDepth);

	ini.CloseFile();

	unguard;
}

void UIFormSetting::OnClick_ConfirmResolution_Yes(void*, void*)
{
	guard;
	if (m_confirmVideoTimer)
	{
		GetTimer()->RemoveTimer(m_confirmVideoTimer);
		m_confirmVideoTimer = 0;
	}

	RTW_WIDGET("ConfirmResolution")->Hide();

	SettingSaveResolutionInfo();

	unguard;
}

void UIFormSetting::OnClick_ConfirmResolution_No(void*, void*)
{
	guard;
	if (m_confirmVideoTimer)
	{
		GetTimer()->RemoveTimer(m_confirmVideoTimer);
		m_confirmVideoTimer = 0;
	}

	RTW_WIDGET("ConfirmResolution")->Hide();

	GetDevice()->SetViewWindowInfo(
		m_bkViewMode,
		m_bkResWidth,
		m_bkResHeight,
		m_bkColorDepth,
		0);

	GetDevice()->SetViewMode(m_bkViewMode);

	if (RTW_WIDGET("fmnewsetting")->GetFlags(wfVisible))
	{
		RefreshVideoSettings();
	}


	SettingSaveResolutionInfo();

	unguard;
}

void UIFormSetting::RefreshAudioSettings()
{
	guard;

    // Refresh Audio
	if(!g_layerMain->m_eSetting.m_bBackGroundMusicIsSlience)
	{
		m_musicVolume->SetValue(m_oldSetting.audio.iMusicVal);
		m_checkMusicSilence->SetChecked(false);
	}else
	{
		m_musicVolume->SetValue(0);
		m_checkMusicSilence->SetChecked(true);
	}
	if(!g_layerMain->m_eSetting.m_bSoundIsSlience)
	{
		m_soundVolume->SetValue(m_oldSetting.audio.iSoundVal);
		m_checkSoundSilence->SetChecked(false);
	}else
	{
		m_soundVolume->SetValue(0);
		m_checkSoundSilence->SetChecked(true);
	}
	m_oldSetting.audio.bUse3D    = g_pSoundMgr->GetUse3DAudio();

    m_3dsound->SetChecked(m_oldSetting.audio.bUse3D);

	unguard;
}

void UIFormSetting::RefreshGameSettings()
{
	guard;

	if(GetWorld()->GetCameraMode() == GcWorld::CAMERA_MODE_LOCK)
		m_lockcamera->SetChecked(true);
	else
		m_lockcamera->SetChecked(false);
    // Refresh Game
    //if (WorldIsInited())
    //{
        //GcWorld* pWorld = GetWorld();

        //m_oldSetting.game.cMaxDisplay = pWorld->m_ActorManager.GetMaxDisplayChar();
        //m_oldSetting.game.bLockCamera = pWorld->GetCameraMode()==GcWorld::CAMERA_MODE_LOCK;
        //m_oldSetting.game.cHUD = RtwHUD::s_cShowItem;

        //m_lockcamera->SetChecked    (m_oldSetting.game.bLockCamera);
        //m_usersInScreen->SetValue   (m_oldSetting.game.cMaxDisplay);
        //m_displayhud->SetChecked    (m_oldSetting.game.cHUD!=0);
        //m_hudSay->SetChecked        ((m_oldSetting.game.cHUD&RtwHUD::siSpeak)!=0);
        //m_hudName->SetChecked       ((m_oldSetting.game.cHUD&RtwHUD::siName)!=0);
        //m_hudGuild->SetChecked      ((m_oldSetting.game.cHUD&RtwHUD::siGuild)!=0);
        //m_hudNick->SetChecked       ((m_oldSetting.game.cHUD&RtwHUD::siNick)!=0);
        //m_hudCountry->SetChecked    ((m_oldSetting.game.cHUD&RtwHUD::siCountry)!=0);
		// gao 输入以及显示处理，当前不加入设置功能了
        /*m_oldSetting.game.sUIInputProject = UIFormSetting::sUIInputProject;
        m_oldSetting.game.sUIShowProject = UIFormSetting::sUIShowProject;
		m_UIInputProject->SetSelectedItem((short)m_oldSetting.game.sUIInputProject);
		m_UIShowProject->SetSelectedItem((short)m_oldSetting.game.sUIShowProject);*/

		//m_UIRefuse->SetChecked(m_oldSetting.game.bRefuse) ;//是否接受邀请
    //}

	unguard;
}

void UIFormSetting::RefreshVideoSettings()
{
	guard;

	m_lstResolution->RemoveAllItems();

	int n = 0;

	const RtgViewWindowInfo& curInfo = GetDevice()->GetViewWindowInfo(GetDevice()->GetViewWindowMode());

	foreach(std::vector<Resolution>, iter, m_resolutionList)
	{
		rt2_sprintf(g_strStaticBuffer, "%dx%d", iter->width, iter->height);

		m_lstResolution->AddItem(g_strStaticBuffer);

		if (iter->width == curInfo.dwWidth && iter->height == curInfo.dwHeight)
		{
			m_lstResolution->SetSelectedItem(n);				
		}

		n++;
	}
	char buf[8];
	int cursel   = m_lstResolution->GetSelectedItem();

	RTGViewWindowMode eMode = GetDevice()->GetViewWindowMode();

	if (eMode == RTGVWM_TOPWINDOW)
	{
		m_checkWindows->SetChecked(false);
		m_checkFullScreen->SetChecked(true);
    }else
	{
		m_checkWindows->SetChecked(true);
		m_checkFullScreen->SetChecked(false);
	}

	if (GetDevice()->GetCanDoWindowed())
		m_checkFullScreen->Enable();
	else
		m_checkFullScreen->Disable();

    //////////////////////////////////////////////////////////////////////////

    m_oldSetting.graph.sTexture = GetDevice()->GetTextMgr()->GetTextureQuality() / 2;
    if (m_oldSetting.graph.sTexture>2) m_oldSetting.graph.sTexture = 2;

    EActorQuality eqActor, eqSkin, eqMtl, eqEffect;
    ActorGetQuality(eqActor, eqSkin, eqMtl, eqEffect);

    switch (eqEffect)
    {
    default:
    case QUALITY_HIGH:      m_oldSetting.graph.sEffect = 0; break;
    case QUALITY_MIDDLE:    m_oldSetting.graph.sEffect = 1; break;
    case QUALITY_LOW:       m_oldSetting.graph.sEffect = 2; break;
    }

    switch (GcRenderActorBase::s_iShadowQuality)
    {
    default:
    case 3: m_oldSetting.graph.sShadow = 0; break; // 高
    case 2: m_oldSetting.graph.sShadow = 1; break; // 中
    case 1: m_oldSetting.graph.sShadow = 2; break; // 低
    }

	switch(m_oldSetting.graph.sTexture)
	{
	case 0:
		m_checkTextureQuality1->SetChecked(false);
		m_checkTextureQuality2->SetChecked(false);
		m_checkTextureQuality3->SetChecked(true);
		break;
	case 1:
		m_checkTextureQuality1->SetChecked(false);
		m_checkTextureQuality2->SetChecked(true);
		m_checkTextureQuality3->SetChecked(false);
		break;
	case 2:
		m_checkTextureQuality1->SetChecked(true);
		m_checkTextureQuality2->SetChecked(false);
		m_checkTextureQuality3->SetChecked(false);
		break;
	default:
		m_oldSetting.graph.sTexture = 2;
		m_checkTextureQuality1->SetChecked(true);
		m_checkTextureQuality2->SetChecked(false);
		m_checkTextureQuality3->SetChecked(false);
		break;
	}
	switch(m_oldSetting.graph.sEffect)
	{
	case 0:
		m_checkEffectQuality1->SetChecked(false);
		m_checkEffectQuality2->SetChecked(false);
		m_checkEffectQuality3->SetChecked(true);
		break;
	case 1:
		m_checkEffectQuality1->SetChecked(false);
		m_checkEffectQuality2->SetChecked(true);
		m_checkEffectQuality3->SetChecked(false);
		break;
	case 2:
		m_checkEffectQuality1->SetChecked(true);
		m_checkEffectQuality2->SetChecked(false);
		m_checkEffectQuality3->SetChecked(false);
		break;
	default:
		m_oldSetting.graph.sEffect = 2;
		m_checkEffectQuality1->SetChecked(true);
		m_checkEffectQuality2->SetChecked(false);
		m_checkEffectQuality3->SetChecked(false);
		break;
	}
	switch(m_oldSetting.graph.sShadow)
	{
	case 0:
		m_checkShadowQuality1->SetChecked(false);
		m_checkShadowQuality2->SetChecked(false);
		m_checkShadowQuality3->SetChecked(true);
		break;
	case 1:
		m_checkShadowQuality1->SetChecked(false);
		m_checkShadowQuality2->SetChecked(true);
		m_checkShadowQuality3->SetChecked(false);
		break;
	case 2:
		m_checkShadowQuality1->SetChecked(true);
		m_checkShadowQuality2->SetChecked(false);
		m_checkShadowQuality3->SetChecked(false);
		break;
	default:
		m_oldSetting.graph.sShadow = 2;
		m_checkShadowQuality1->SetChecked(true);
		m_checkShadowQuality2->SetChecked(false);
		m_checkShadowQuality3->SetChecked(false);
		break;
	}
	//m_textureQuality->SetSelectedItem(m_oldSetting.graph.sTexture);
	//m_effectQuality->SetSelectedItem(m_oldSetting.graph.sEffect);
	//m_shadowQuality->SetSelectedItem(m_oldSetting.graph.sShadow);

    m_oldSetting.bFsaa        = GetDevice()->GetRenderState(RTGRS_FULLSCRENE_ANTIALIASING);
    m_oldSetting.light.bEnvref      = g_pScene->m_bReflectSceneObject;
    m_oldSetting.light.bObjref      = g_pScene->m_bReflectOtherActor;
    m_oldSetting.light.bPosteffect  = GetDevice()->IsUsePostProcessEffectOn();
    m_oldSetting.light.bObjeffect   = GetDevice()->IsUsePostProcessObjectHighLightOn();

	m_checkLight->SetChecked(m_oldSetting.light.bEnvref || m_oldSetting.light.bObjref);
	m_checkFullScreenLight->SetChecked(m_oldSetting.light.bPosteffect || m_oldSetting.light.bObjeffect);
    //m_sawtooth->SetChecked(m_oldSetting.graph.bFsaa);
    //m_reflect->SetChecked(m_oldSetting.graph.bEnvref);
    //m_reflectpeople->SetChecked(m_oldSetting.graph.bObjref);
    //m_effectPost->SetChecked(m_oldSetting.graph.bPosteffect);
    //m_effectObject->SetChecked(m_oldSetting.graph.bObjeffect);

    if (m_oldSetting.graph.Same(m_oldSetting.graphHigh) )
    {
        m_hsBarVideo->SetValue(0);
    }else if (m_oldSetting.graph.Same(m_oldSetting.graphMiddle2) )
    {
		m_hsBarVideo->SetValue(1);
    }else if (m_oldSetting.graph.Same(m_oldSetting.graphMiddle1) )
    {
		m_hsBarVideo->SetValue(2);
    }else if (m_oldSetting.graph.Same(m_oldSetting.graphLow) )
    {
		m_hsBarVideo->SetValue(3);
    }
	unguard;
}
void UIFormSetting::RefreshShowSettings()
{
	guard;
	m_checkShowSelfName->SetChecked(g_layerMain->m_eSetting.m_bShowSelfName);
	m_checkShowBlood->SetChecked(g_layerMain->m_eSetting.m_bShowBlood);
	m_checkShowDegree->SetChecked(g_layerMain->m_eSetting.m_bShowDegree);
	m_checkShowDamage->SetChecked(g_layerMain->m_eSetting.m_bShowDamage);
	m_checkShowUnionName->SetChecked(g_layerMain->m_eSetting.m_bShowUnionName);
	m_checkShowEXP->SetChecked(g_layerMain->m_eSetting.m_bShowEXP);
	m_checkShowOtherName->SetChecked(g_layerMain->m_eSetting.m_bShowOtherName);
	m_checkShowNPCName->SetChecked(g_layerMain->m_eSetting.m_bShowNPCName);
	m_checkShowOtherSpeak->SetChecked(g_layerMain->m_eSetting.m_bShowOtherSpeak);
	m_checkShowTradeName->SetChecked(g_layerMain->m_eSetting.m_bShowTradeName);
	m_checkShowSelfSpeak->SetChecked(g_layerMain->m_eSetting.m_bShowSelfSpeak);
	m_checkSkillShotcut->SetChecked(g_layerMain->m_eSetting.m_bShowSkillShotcut);
	unguard;
}
void UIFormSetting::RefreshRefuseSettings()
{
	guard;
	m_checkRefuseTeam->SetChecked(g_layerMain->m_eSetting.m_bRefuseTeam);
	m_checkRefuseAction->SetChecked(g_layerMain->m_eSetting.m_bRefuseAction);
	m_checkRefuseMail->SetChecked(g_layerMain->m_eSetting.m_bRefuseMail);
	m_checkRefuseDuel->SetChecked(g_layerMain->m_eSetting.m_bRefuseDuel);
	m_checkRefuseFriend->SetChecked(g_layerMain->m_eSetting.m_bRefuseFriend);
	m_checkRefuseMonster->SetChecked(g_layerMain->m_eSetting.m_bRefuseMonster);
	unguard;
}

void UIFormSetting::OnTimerEvent(DWORD dwID)
{
	guard;
	int left = 15 - (rtGetMilliseconds() - m_confirmVideoTimeStamp) / 1000;
	if (left > 0)
	{
		rt2_sprintf(g_strStaticBuffer, R(MSG_RESUME_DISPLAY_SETTING), left);

		RTW_WIDGET("ConfirmResolution.Time")->SetText(g_strStaticBuffer);

		return;
	}
	OnClick_ConfirmResolution_No(NULL, NULL);
	unguard;
}

void UIFormSetting::OnClick_Used3D(void*, void*)
{
	g_pSoundMgr->SetUse3dAudio(m_3dsound->GetChecked());
}
void UIFormSetting::OnCheckButtonFullScreenOrNo(ui::RtwWidget* vpSender, RtwEventDelegate*)
{
	guard;
	if(vpSender == m_checkWindows)
	{
		m_checkFullScreen->SetChecked(false);
		m_checkWindows->SetChecked(true);
	}
	else if(vpSender == m_checkFullScreen)
	{
		m_checkFullScreen->SetChecked(true);
		m_checkWindows->SetChecked(false);
	}
	unguard;
}
void UIFormSetting::OnCheckButtonShowSkillShotcut(ui::RtwWidget* vpSender, RtwEventDelegate*)
{
	guard;
	g_layerMain->m_eSetting.m_bShowSkillShotcut = m_checkSkillShotcut->GetChecked();
	if(g_layerMain->m_eSetting.m_bShowSkillShotcut)
		RTW_WIDGET("fmshortcut_bar2")->Show();
	else
		RTW_WIDGET("fmshortcut_bar2")->Hide();
	unguard;
}
void UIFormSetting::OnCheckButtonSilence(ui::RtwWidget* vpSender, RtwEventDelegate*)
{
	guard;
	if(vpSender == m_checkMusicSilence)
	{
		if(m_checkMusicSilence->GetChecked())
		{
			g_pBackMusic->SetVolume(0.f);
			m_musicVolume->SetValue(0);
			m_musicVolume->Disable();
			g_layerMain->m_eSetting.m_bBackGroundMusicIsSlience = true;
		}else
		{
			g_pBackMusic->SetVolume(m_oldSetting.audio.iMusicVal*0.01);
			m_musicVolume->SetValue(m_oldSetting.audio.iMusicVal);
			m_musicVolume->Enable();
			g_layerMain->m_eSetting.m_bBackGroundMusicIsSlience = false;
		}
	}
	else if(vpSender == m_checkSoundSilence)
	{
		if(m_checkSoundSilence->GetChecked())
		{
			g_pSoundMgr->SetWholeVolume(0.f);
			m_soundVolume->SetValue(0);
			m_soundVolume->Disable();
			g_layerMain->m_eSetting.m_bSoundIsSlience = true;
		}else
		{
			g_pSoundMgr->SetWholeVolume(m_oldSetting.audio.iSoundVal*0.01);
			m_soundVolume->SetValue(m_oldSetting.audio.iSoundVal);
			m_soundVolume->Enable();
			g_layerMain->m_eSetting.m_bSoundIsSlience = false;
		}
	}
	unguard;
}
void UIFormSetting::OnCheckButtonTexture(ui::RtwWidget* vpSender, RtwEventDelegate*)
{
	guard;
	if(vpSender == m_checkTextureQuality1)
	{
		m_checkTextureQuality1->SetChecked(true);
		m_checkTextureQuality2->SetChecked(false);
		m_checkTextureQuality3->SetChecked(false);
	}else if(vpSender == m_checkTextureQuality2)
	{
		m_checkTextureQuality1->SetChecked(false);
		m_checkTextureQuality2->SetChecked(true);
		m_checkTextureQuality3->SetChecked(false);
	}else if(vpSender== m_checkTextureQuality3)
	{
		m_checkTextureQuality1->SetChecked(false);
		m_checkTextureQuality2->SetChecked(false);
		m_checkTextureQuality3->SetChecked(true);
	}
	unguard;
}
void UIFormSetting::OnCheckButtonEffect(ui::RtwWidget* vpSender, RtwEventDelegate*)
{
	guard;
	if(vpSender == m_checkEffectQuality1)
	{
		m_checkEffectQuality1->SetChecked(true);
		m_checkEffectQuality2->SetChecked(false);
		m_checkEffectQuality3->SetChecked(false);
	}else if(vpSender == m_checkEffectQuality2)
	{
		m_checkEffectQuality1->SetChecked(false);
		m_checkEffectQuality2->SetChecked(true);
		m_checkEffectQuality3->SetChecked(false);
	}else if(vpSender== m_checkEffectQuality3)
	{
		m_checkEffectQuality1->SetChecked(false);
		m_checkEffectQuality2->SetChecked(false);
		m_checkEffectQuality3->SetChecked(true);
	}
	unguard;
}
void UIFormSetting::OnCheckButtonShadow(ui::RtwWidget* vpSender, RtwEventDelegate*)
{
	guard;
	if(vpSender == m_checkShadowQuality1)
	{
		m_checkShadowQuality1->SetChecked(true);
		m_checkShadowQuality2->SetChecked(false);
		m_checkShadowQuality3->SetChecked(false);
	}else if(vpSender == m_checkShadowQuality2)
	{
		m_checkShadowQuality1->SetChecked(false);
		m_checkShadowQuality2->SetChecked(true);
		m_checkShadowQuality3->SetChecked(false);
	}else if(vpSender== m_checkShadowQuality3)
	{
		m_checkShadowQuality1->SetChecked(false);
		m_checkShadowQuality2->SetChecked(false);
		m_checkShadowQuality3->SetChecked(true);
	}
	unguard;
}
void UIFormSetting::OnSaveVideoSettings()
{
	guard;
	unguard;
}
void UIFormSetting::OnSaveShowSettings()
{
	guard;
	g_layerMain->m_eSetting.m_bShowSelfName		= m_checkShowSelfName->GetChecked();
	g_layerMain->m_eSetting.m_bShowBlood		= m_checkShowBlood->GetChecked();
	g_layerMain->m_eSetting.m_bShowDegree		= m_checkShowDegree->GetChecked();
	g_layerMain->m_eSetting.m_bShowDamage		= m_checkShowDamage->GetChecked();
	g_layerMain->m_eSetting.m_bShowUnionName	= m_checkShowUnionName->GetChecked();
	g_layerMain->m_eSetting.m_bShowEXP			= m_checkShowEXP->GetChecked();
	g_layerMain->m_eSetting.m_bShowOtherName	= m_checkShowOtherName->GetChecked();
	g_layerMain->m_eSetting.m_bShowNPCName		= m_checkShowNPCName->GetChecked();
	g_layerMain->m_eSetting.m_bShowOtherSpeak	= m_checkShowOtherSpeak->GetChecked();
	g_layerMain->m_eSetting.m_bShowTradeName	= m_checkShowTradeName->GetChecked();
	g_layerMain->m_eSetting.m_bShowSelfSpeak	= m_checkShowSelfSpeak->GetChecked();
	g_layerMain->m_eSetting.m_bShowSkillShotcut	= m_checkSkillShotcut->GetChecked();
	RtwHUD::s_cShowItem = 0;
	RtwHUD::s_cShowItem = RtwHUD::s_cShowItem | RtwHUD::siName | RtwHUD::siSpeak | RtwHUD::siPetAbility | RtwHUD::siPetFuseLev | RtwHUD::siPetMaster;
	if(g_layerMain->m_eSetting.m_bShowUnionName)
		RtwHUD::s_cShowItem |= RtwHUD::siGuild;
	if(g_layerMain->m_eSetting.m_bShowDegree)
		RtwHUD::s_cShowItem |= RtwHUD::siNick;
	if(g_layerMain->m_eSetting.m_bShowTradeName)
		RtwHUD::s_cShowItem |= RtwHUD::esiStall;
	GetPlayer()->m_pHUD->ShowNameSeparate(g_layerMain->m_eSetting.m_bShowSelfName);
	//if (m_hudSay->GetChecked())     RtwHUD::s_cShowItem |= RtwHUD::siSpeak;
	//if (m_hudName->GetChecked())    RtwHUD::s_cShowItem |= RtwHUD::siName;
	//if (m_hudGuild->GetChecked())   RtwHUD::s_cShowItem |= RtwHUD::siGuild;
	//if (m_hudNick->GetChecked())    RtwHUD::s_cShowItem |= RtwHUD::siNick;
	////if (m_hudCountry->GetChecked()) RtwHUD::s_cShowItem |= RtwHUD::siCountry;
	unguard;
}
void UIFormSetting::OnSaveRefuseSettings()
{
	guard;
	g_layerMain->m_eSetting.m_bRefuseTeam		= m_checkRefuseTeam->GetChecked();
	g_layerMain->m_eSetting.m_bRefuseAction		= m_checkRefuseAction->GetChecked();
	g_layerMain->m_eSetting.m_bRefuseMail		= m_checkRefuseMail->GetChecked();
	g_layerMain->m_eSetting.m_bRefuseDuel		= m_checkRefuseDuel->GetChecked();
	g_layerMain->m_eSetting.m_bRefuseFriend		= m_checkRefuseFriend->GetChecked();
	g_layerMain->m_eSetting.m_bRefuseMonster	= m_checkRefuseMonster->GetChecked();

	if(g_layerMain->m_eSetting.m_bRefuseTeam)
		GetWorld()->m_Team.SetRefuseState(true);//拒绝
	else
		GetWorld()->m_Team.SetRefuseState(false);//拒绝
	if(g_layerMain->m_eSetting.m_bRefuseFriend)
		GetWorld()->m_Friend.SetRefuseState(true);//拒绝
	else
		GetWorld()->m_Friend.SetRefuseState(false);//拒绝
	//if()
	//	g_layerMain->m_formTrade->SetRefuseState(false);
	//else
	//	g_layerMain->m_formTrade->SetRefuseState(true);
	//if()
	//	g_layerMain->m_formUnion->SetRefuseState(false);
	//else
	//	g_layerMain->m_formUnion->SetRefuseState(true);
	CG_CmdPacket* pPacket = NetBeginWrite();
	pPacket->WriteShort(c2r_set_refusesetting);
	pPacket->WriteByte(g_layerMain->m_eSetting.m_bRefuseTeam);
	pPacket->WriteByte(g_layerMain->m_eSetting.m_bRefuseAction);
	pPacket->WriteByte(g_layerMain->m_eSetting.m_bRefuseMail);
	pPacket->WriteByte(g_layerMain->m_eSetting.m_bRefuseDuel);
	pPacket->WriteByte(g_layerMain->m_eSetting.m_bRefuseFriend);
	pPacket->WriteByte(g_layerMain->m_eSetting.m_bRefuseMonster);
	NetSend(pPacket);
	unguard;
}
void UIFormSetting::OnCheckButtonCamera(ui::RtwWidget* vpSender, RtwEventDelegate*)
{
	guard;
	if(m_lockcamera->GetChecked())
	{
		GetWorld()->SetCameraMode(GcWorld::CAMERA_MODE_LOCK);
		RTW_WIDGET_T(RtwButton, "fmminimap.btnperspective")->SetHintText("镜头已锁");
	}
	else
	{
		GetWorld()->SetCameraMode(GcWorld::CAMERA_MODE_TURN);
		RTW_WIDGET_T(RtwButton, "fmminimap.btnperspective")->SetHintText("镜头未锁");
	}
	unguard;
}