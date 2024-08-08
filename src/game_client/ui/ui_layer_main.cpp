//#include "gc_include.h"
#include "ui_project.h"
#include "net_command_union.h"
#include "ui_form_msg.h"

#include "ui_form_setting.h"
#include "ui_form_playeritems.h"
#include "ui_form_item_update.h"
#include "ui_form_char.h"
#include "ui_form_task.h"
#include "ui_form_maintask.h"
#include "ui_form_trade.h"
#include "ui_form_bank.h"
#include "ui_form_shop.h"
#include "ui_form_msg.h"
#include "ui_form_textmsg.h"
#include "ui_form_skill.h"
#include "ui_form_skill_study.h"
#include "ui_form_minimap.h"
#include "ui_form_union.h"
#include "ui_form_union_create.h"
#include "ui_form_feod_tax.h"
#include "ui_form_feod_bank.h"
#include "ui_form_feod_content.h"
#include "ui_form_herolist.h"
#include "ui_form_gm.h"
#include "ui_form_help.h"
#include "ui_form_pkmode.h"
//#include "rtw_clipboard.h"
//-------------add start by tony 05.06.06------------------------------------//
#include "ui_form_castlewar.h"
#include "ui_form_castlewar_settime.h"
#include "ui_form_timer.h"
#include "net_command_castlewar.h"
#include "net_command_auction.h"
#include "ui_form_dupe.h"
#include "ui_form_vendor.h"
#include "ui_form_bid.h"
#include "ui_form_sale.h"
//-------------add end   by tony 05.06.06------------------------------------//
#include "ui_form_itemsinfo.h"
#include "ui_form_cre_info.h"
#include "ui_form_select_reward.h"
#include "ui_form_card.h"
#include "ui_form_sell_card.h"
#include "ui_form_buy_card.h"
#include "ui_form_analyse_image.h"
#include "gc_prentice.h"
#include "ui_form_bankpass.h"
#include "ui_form_compose.h"
#include "ui_form_timebar.h"
#include "ui_form_fuse.h"
#include "ui_form_warfield.h"
#include "master_list_const.h"
#include "ui_form_peteat.h"
/* gao 2009.12.29 */
#include "UI_form_Team.h"
#include "ui_form_trump.h"
#include "UIForm_System.h"
#include "UIForm_Speaker.h"
#include "UIForm_Char.h"
#include "UIForm_MiddleMap.h"
#include "UIForm_Gem.h"
#include "ui_form_playershop.h"
#include "ui_form_lookplayershop.h"
#include "Timer_Hint.h"
/* end */
#include "UIForm_Pickup.h"
#include "ui_form_decompound.h"

//新手升级指引
#include "UIForm_NewPeopleHelp.h"

#include "game_assistant.h"
#include "game_team_share_const.h"
#include "ui_form_pet_include.h"
#include "ui_form_pet_bank.h"
#include "ui_form_shopcenter.h"

#include <strstream>
#include <io.h>

const char* g_storedIcon[] =
{
	"fmcharall.tabcharall.fmsoc.btntrade",      // 0
	"fmcharall.tabcharall.fmsoc.btnfriend",     // 1
	"fmcharall.tabcharall.fmsoc.btnguild",      // 2
	"fmcharall.tabcharall.fmsoc.btnshop",       // 3
	"fmcharall.tabcharall.fmsoc.btninvite",     // 4
	"fmcharall.tabcharall.fmsoc.btnleave",      // 5
	"fmcharall.tabcharall.fmsoc.btndivid",      // 6
	"fmcharall.tabcharall.fmsoc.btnteamclose",  // 7

    "fmcharall.tabcharall.fmsoc.btnwell",       // 8
    "fmcharall.tabcharall.fmsoc.btnsmile",      // 9
    "fmcharall.tabcharall.fmsoc.btnnod",        // 10
    "fmcharall.tabcharall.fmsoc.btnno",         // 11
    "fmcharall.tabcharall.fmsoc.btncry",        // 12
    "fmcharall.tabcharall.fmsoc.btncheer",      // 13
    "fmcharall.tabcharall.fmsoc.btnwave",       // 14
};

#define GRID_SIZE_X		32
#define GRID_SIZE_Y		32

const int g_storedIconCount = sizeof(g_storedIcon) / sizeof(g_storedIcon[0]);
void OnHtmlViewHide()
{
	guard;

    if (g_layerMain)
    {
        g_layerMain->m_viewHelp->SetUserData(0);
    }

//    GetWorld()->m_dwInteractiveNPCID = 0;

	unguard;
}

void OnSaveCircle()
{
	CG_CmdPacket *cmd = NetBeginWrite();
	cmd->WriteShort(c2r_save_circle);
	NetSend(cmd);
}

void OnActiveCircle()
{
	CG_CmdPacket *cmd = NetBeginWrite();
	cmd->WriteShort(c2r_active_circle);
	NetSend(cmd);
}

UILayerMain::UILayerMain()
: m_formSetting(NULL)
, m_fromPlayerItems(NULL)
, m_hotKeyGroupNumber(0)
, strCommand("")
, bStartActivity(false)
, strActivityText("")
, lActivityTime(0)
,m_bCountDownStart(false)
, mSelectRewardId(0)
{
	guard;
	m_mapHtmlContainer.clear();
	//memset(m_hotKeyGroup, 0, sizeof(RtwButton*) * UI_HOT_KEY_COUNT);
	
	//g_workspace.Load("ui/ui_layer_main.xml");
	
	//m_hotKeyLeft = LOAD_UI_T(RtwButton, "fmhotkey.btnhotkeyleft");
	//m_hotKeyLeft->EvLClick += RTW_CALLBACK(this, UILayerMain, OnClick_HotKey);

	//m_hotKeyRight = LOAD_UI_T(RtwButton, "fmhotkey.btnhotkeyright");
	//m_hotKeyRight->EvLClick += RTW_CALLBACK(this, UILayerMain, OnClick_HotKey);

	m_btnFbTask = LOAD_UI_T(RtwButton, "fmsystem.btndupetask");
	m_btnFbTask->EvLClick += RTW_CALLBACK(this, UILayerMain, OnClick_FbTask);

// 	LOAD_UI("worldfmfeedback")->ModifyFlags(0,wfInput|wfPick);
	m_btnAutoBattle = LOAD_UI_T(CUiCheckButton, "fmreborn.ckbbattle");//for auto    ac.ma
	m_btnAutoB = LOAD_UI_T(CUiCheckButton, "fmreborn.ckbautobattle");
	m_btnAutoPick = LOAD_UI_T(CUiCheckButton, "fmreborn.ckbpick");
 	m_btnAutoEatRed = LOAD_UI_T(CUiCheckButton, "fmreborn.ckbblood");
	//m_btnAutoEatRed->SetChecked(true);	//heten
	m_btnAutoEatRed->SetChecked(false);    //change by yz : 待玩家选择
 	m_btnAutoEatBlue = LOAD_UI_T(CUiCheckButton, "fmreborn.ckbmana");//blue
	//m_btnAutoEatBlue->SetChecked(true);	//heten
	m_btnAutoEatBlue->SetChecked(false);   //change by yz : 待玩家选择
	m_btnAutoHp30 = LOAD_UI_T(CUiCheckButton, "fmreborn.ckb30hp");
	m_btnAutoHp60 = LOAD_UI_T(CUiCheckButton, "fmreborn.ckb60hp");
	m_btnAutoHp60->SetChecked(true);	//heten
	m_btnAutoHp90 = LOAD_UI_T(CUiCheckButton, "fmreborn.ckb90hp");
	m_btnAutoSellWhite = LOAD_UI_T(CUiCheckButton, "fmreborn.ckbsellwhite");
	m_btnAutoSellGreen = LOAD_UI_T(CUiCheckButton, "fmreborn.ckbsellgreen");
	m_btnAutoSellBlue = LOAD_UI_T(CUiCheckButton, "fmreborn.ckbsellblue");
	m_btnAutoSellGold = LOAD_UI_T(CUiCheckButton, "fmreborn.ckbsellgolden");
 	
	m_btnAutoStateYes = LOAD_UI_T(RtwButton, "forautostate.Yes");				//ac.ma
	m_btnAutoStateYes->EvLClick += RTW_CALLBACK(this, UILayerMain, OnBtnAutoStateYes);
	m_btnAutoStateNo = LOAD_UI_T(RtwButton, "forautostate.No");				//ac.ma
	m_btnAutoStateNo->EvLClick += RTW_CALLBACK(this, UILayerMain, OnBtnAutoStateNo);

	//m_hotKeyPage = LOAD_UI("fmhotkey.lbhotkeypage");
	LOAD_UI_T(RtwButton, "fmhp.btnPKmode")->EvLClick += RTW_CALLBACK(this, UILayerMain, OnClicked_PkMode);

	LOAD_UI("fmpk.lstPKmode")->EvActivate += RTW_CALLBACK(this, UILayerMain, OnActivateFrmPkMode);

	/*LOAD_UI("btnteam")->Hide();
	LOAD_UI("btnteam")->EvLClick+=RTW_CALLBACK(this,UILayerMain,OnClicked_TeamApply);
	LOAD_UI("btnteamoff")->SetText(string("开"));
	LOAD_UI("btnteamoff")->Hide();
	LOAD_UI("btnteamoff")->EvLClick+=RTW_CALLBACK(this,UILayerMain,OnClicked_TeamOff);*/
	char hotkey1[30];
	for (int i = 0; i < 10; ++i)
	{
		rt2_sprintf(hotkey1,"fmhotkey.lbhotkey%d",i+1);
		m_hotKeyAlias[i] = LOAD_UI_T(RtwAliasButton, hotkey1);
		m_hotKeyAlias[i]->ModifyFlag(wfDragOut | wfHint | wfVisible | wfDClick | wfDragIn | wfGrabMouse,0);

		rt2_sprintf(hotkey1,"fmshortcut_bar1.lbhotkey%d",i+1);
		m_hotKeyAlias[i+10] = LOAD_UI_T(RtwAliasButton, hotkey1);
		m_hotKeyAlias[i+10]->ModifyFlag(wfDragOut | wfHint | wfVisible | wfDClick | wfDragIn | wfGrabMouse,0);

		rt2_sprintf(hotkey1,"fmshortcut_bar2.lbhotkey%d",i+1);
		m_hotKeyAlias[i+20] = LOAD_UI_T(RtwAliasButton, hotkey1);
		m_hotKeyAlias[i+20]->ModifyFlag(wfDragOut | wfHint | wfVisible | wfDClick | wfDragIn | wfGrabMouse,0);
	}
	
//	m_hotKeyButton = LOAD_UI_T(RtwAliasButton, "fmhotkey.lbhotkeybutton");
		
    LOAD_UI("fmsystem.btnsyschar")->EvLClick		+= RTW_CALLBACK(this, UILayerMain, OnClicked_System_Char);
    LOAD_UI("fmsystem.btnsysitem")->EvLClick		+= RTW_CALLBACK(this, UILayerMain, OnClicked_PlayerItems);
    LOAD_UI("fmsystem.btnsysskill")->EvLClick	+= RTW_CALLBACK(this, UILayerMain, OnClicked_System_Skill);
    LOAD_UI("fmsystem.btnsyslist")->EvLClick		+= RTW_CALLBACK(this, UILayerMain, OnClicked_HeroList);
	LOAD_UI("fmshop.btnsysshop")->EvLClick		+= RTW_CALLBACK(this, UILayerMain, OnClicked_ShopCenter);	//heten

	LOAD_UI("fmminimap.btnsysmap")->EvLClick		+= RTW_CALLBACK(this, UILayerMain, OnClicked_System_Map);//gao 2010.1.5 内容取消

	LOAD_UI("fmsystem.btnsystask")->EvLClick		+= RTW_CALLBACK(this, UILayerMain, OnClicked_Attribute_task);

	LOAD_UI("fmsystem.btnsysbattle")->EvLClick	+= RTW_CALLBACK(this, UILayerMain, OnLClicked_System_Battle);
	LOAD_UI("fmsystem.btnpet")->EvLClick			+= RTW_CALLBACK(this, UILayerMain, OnButtonPetDown);
	LOAD_UI("fmsystem.btnbangpai")->EvLClick		+= RTW_CALLBACK(this, UILayerMain, OnClicked_Union);

	/* gao 2009.12.24
	新添按钮事件的添加
	*/
	LOAD_UI("fmsystem.btnteam")->EvLClick		+= RTW_CALLBACK(this, UILayerMain, OnClicked_TeamButton);
	/* end */

    // fmsystem.btnsysfriend 在 gc_friend_with_ui.cpp (49)
	LOAD_UI("fmsystem.btnsystem")->EvLClick		+= RTW_CALLBACK(this, UILayerMain, OnClicked_System_System);

	LOAD_UI("fmsyssetting.btnsysset")->EvLClick	+= RTW_CALLBACK(this, UILayerMain, OnClicked_SysSetting_SysSet);
	LOAD_UI("fmsyssetting.btnrestart")->EvLClick	+= RTW_CALLBACK(this, UILayerMain, OnClicked_SysSetting_Restart);
	LOAD_UI("fmsyssetting.btnexit")->EvLClick	+= RTW_CALLBACK(this, UILayerMain, OnClicked_SysSetting_Exit);
	LOAD_UI("fmexitconfirm.btnconfirm")->EvLClick += RTW_CALLBACK(this, UILayerMain, OnClicked_ExitConfirm_Confirm);
	LOAD_UI("fmexitconfirm.btncancel")->EvLClick	+= RTW_CALLBACK(this, UILayerMain, OnClicked_ExitConfirm_Cancel);
	
    rt2_snprintf(g_strStaticBuffer+100, 2048-100, R(G_VERSION), GetGameVersion());
    LOAD_UI("fmsyssetting.lbver")->SetText(std::string(g_strStaticBuffer+100));

	m_formSysSetting = LOAD_UI("fmsyssetting");
	//m_formSysSetting->EvKey += RTW_CALLBACK(m_formSysSetting, RtwWidget, DefaultKeyDown);

	// PopupMenu
	LOAD_UI_T(RtwPopupMenu, "UserMenu")->EvMenuSelect    += RTW_CALLBACK(this, UILayerMain, OnMenuUser);
	//LOAD_UI_T(RtwPopupMenu, "UserMenuByname")->EvMenuSelect    += RTW_CALLBACK(this, UILayerMain, OnMenuUserByName);
    LOAD_UI_T(RtwPopupMenu, "MonsterMenu")->EvMenuSelect += RTW_CALLBACK(this, UILayerMain, OnMenuMonster);
    LOAD_UI_T(RtwPopupMenu, "PlayerMenu")->EvMenuSelect  += RTW_CALLBACK(this, UILayerMain, OnMenuPlayer);
    LOAD_UI_T(RtwPopupMenu, "PetMenu")->EvMenuSelect     += RTW_CALLBACK(this, UILayerMain, OnMenuPet);
    LOAD_UI_T(RtwPopupMenu, "NpcMenu")->EvMenuSelect     += RTW_CALLBACK(this, UILayerMain, OnMenuNpc);
    LOAD_UI_T(RtwPopupMenu, "SystemMenu")->EvMenuSelect  += RTW_CALLBACK(this, UILayerMain, OnMenuSystem);
	//人物的界面
	//m_char=LOAD_UI("fmcharall");
	//m_char->Hide();
	m_fmsysbattle=LOAD_UI("fmreborn");
	//通用一级界面和二级界面
	m_formHelp = LOAD_UI("fmhtmlhelp");
    LOAD_UI("lbrequest")->ModifyFlag(wfInput,0);
	RtwImage *img = g_workspace.getImageFactory()->createImage("ui\\ui_texture\\Tips_request.gif");
	if(img)
		LOAD_UI("lbrequest")->SetBackgroundImage(img);
	LOAD_UI("lbrequest")->Hide();
	m_viewHelp = LOAD_UI_T(RtwHtmlView, "fmhtmlhelp.fmhtml.viewright");
	m_viewHelp->ModifyFlags(0,wfKeyboardEvent);
	m_viewHelp->SetEnableInput(false);
	m_viewHelp->EvLClick += RTW_CALLBACK(this, UILayerMain, OnHtmlAction_HtmlHelpView);
	m_viewHelp->SetScrollBarV(true);
	m_viewHelp->SetAutoScrollV(true);
	m_btnClose = LOAD_UI_T(RtwButton, "fmhtmlhelp.btnswitch");
	m_btnClose->EvLClick += RTW_CALLBACK(this, UILayerMain, OnHideHelp);

	m_formHelp2 = LOAD_UI("fmhtmlhelp2");

	m_viewHelp2 = LOAD_UI_T(RtwHtmlView, "fmhtmlhelp2.fmhtml.viewright");
	m_viewHelp2->ModifyFlags(0,wfKeyboardEvent);
	m_viewHelp2->SetEnableInput(false);
	m_viewHelp2->EvLClick += RTW_CALLBACK(this, UILayerMain, OnHtmlAction_HtmlHelpView);
	m_viewHelp2->SetScrollBarV(true);
	m_viewHelp2->SetAutoScrollV(true);
	m_btnClose2 = LOAD_UI_T(RtwButton, "fmhtmlhelp2.btnswitch");
	m_btnClose2->EvLClick += RTW_CALLBACK(this, UILayerMain, OnHideHelp2);

	m_btnAccept = LOAD_UI_T(RtwButton, "fmhtmlhelp2.btnaccept");
	m_btnAccept->EvLClick           += RTW_CALLBACK(this, UILayerMain, OnClicked_AcceptTask);
	m_btnCancel = LOAD_UI_T(RtwButton, "fmhtmlhelp2.btncancel");
	m_btnCancel->EvLClick           += RTW_CALLBACK(this, UILayerMain, OnClicked_CancelTask);

	m_formHelp3 = LOAD_UI("fmhtmlhelp3");

	m_viewHelp3 = LOAD_UI_T(RtwHtmlView, "fmhtmlhelp3.fmhtml.viewright");
	m_viewHelp3->ModifyFlags(0,wfKeyboardEvent);
	m_viewHelp3->SetEnableInput(false);
	m_viewHelp3->EvLClick += RTW_CALLBACK(this, UILayerMain, OnHtmlAction_HtmlHelpView);
	m_viewHelp3->SetScrollBarV(true);
	m_viewHelp3->SetAutoScrollV(true);
	m_btnClose3 = LOAD_UI_T(RtwButton, "fmhtmlhelp3.btnswitch");
	m_btnClose3->EvLClick += RTW_CALLBACK(this, UILayerMain, OnHideHelp3);

	m_btnFinished = LOAD_UI_T(RtwButton, "fmhtmlhelp3.btnaccept");
	m_btnFinished->EvLClick           += RTW_CALLBACK(this, UILayerMain, OnClicked_FinishTask);
	m_btnCancelFinished = LOAD_UI_T(RtwButton, "fmhtmlhelp3.btncancel");
	m_btnCancelFinished->EvLClick           += RTW_CALLBACK(this, UILayerMain, OnClicked_CancelFinishTask);
	//主线任务
	m_formMainLineTask = LOAD_UI("fmmainstory");
	m_taskTree = LOAD_UI_T(RtwTree, "fmmainstory.treetask");
	m_taskTree->ModifyFlag(0, wfGrabMouse | wfMouseMove);
	m_viewDesc = LOAD_UI_T(RtwHtmlView, "fmmainstory.lbpaper.writing");
	m_viewDesc->ModifyFlags(wfGrabMouse,0);
	m_viewDesc->SetEnableInput(false);
	m_viewDesc->EvLClick += RTW_CALLBACK(this, UILayerMain, OnHtmlAction_HtmlHelpView);
	m_formBtn1 = LOAD_UI("fmmainstory.lbpaper.fmbutton1");
	m_formBtn2 = LOAD_UI("fmmainstory.lbpaper.fmbutton2");
	m_btnMainAccept = LOAD_UI_T(RtwButton, "fmmainstory.lbpaper.fmbutton1.btnyes");
	m_btnMainAccept->EvLClick   += RTW_CALLBACK(this, UILayerMain, OnClicked_AcceptMainTask);
	m_btnMainCancel = LOAD_UI_T(RtwButton, "fmmainstory.lbpaper.fmbutton1.btnno");
	m_btnMainCancel->EvLClick   += RTW_CALLBACK(this, UILayerMain, OnClicked_CancelMainTask);
	m_btnMainFinished = LOAD_UI_T(RtwButton, "fmmainstory.lbpaper.fmbutton2.btnyes");
	m_btnMainFinished->EvLClick   += RTW_CALLBACK(this, UILayerMain, OnClicked_FinishMainTask);
	m_btnMainCancelFinished = LOAD_UI_T(RtwButton, "fmmainstory.lbpaper.fmbutton2.btnno");
	m_btnMainCancelFinished->EvLClick   += RTW_CALLBACK(this, UILayerMain, OnClicked_CancelFinishMainTask);
	//新手指引
	//气泡
	m_formGuide1 = LOAD_UI("fmguide1");
	m_viewGuide1 = LOAD_UI_T(RtwHtmlView, "fmguide1.htmlguide1");
	m_viewGuide1->ModifyFlag(0, wfGrabMouse | wfMouseMove);
	m_btnGuideClose1 = LOAD_UI_T(RtwButton, "fmguide1.btnhide");
	m_btnGuideClose1->EvLClick += RTW_CALLBACK(this, UILayerMain, HideGuideAllForm);
	//气泡上
	m_formGuide2 = LOAD_UI("fmguide2");
	m_viewGuide2 = LOAD_UI_T(RtwHtmlView, "fmguide2.htmlguide2");
	m_formGuide2->ModifyFlag(0, wfGrabMouse | wfMouseMove);
	//气泡下
	m_formGuide3 = LOAD_UI("fmguide3");
	m_viewGuide3 = LOAD_UI_T(RtwHtmlView, "fmguide3.htmlguide3");
	m_formGuide3->ModifyFlag(0, wfGrabMouse | wfMouseMove);
	//气泡右
	m_formGuide4 = LOAD_UI("fmguide4");
	m_viewGuide4 = LOAD_UI_T(RtwHtmlView, "fmguide4.htmlguide4");
	m_formGuide4->ModifyFlag(0, wfGrabMouse | wfMouseMove);
	//气泡左
	m_formGuide5 = LOAD_UI("fmguide5");
	m_viewGuide5 = LOAD_UI_T(RtwHtmlView, "fmguide5.htmlguide5");
	m_formGuide5->ModifyFlag(0, wfGrabMouse | wfMouseMove);
	//脚本模块显示消息
	m_formMessageBox1 = LOAD_UI("fmhtmlmessage1");
	m_viewMessage1 = LOAD_UI_T(RtwHtmlView, "fmhtmlmessage1.fmhtml.viewright");
	m_viewMessage1->ModifyFlags(0,wfKeyboardEvent);
	m_viewMessage1->SetEnableInput(false);
	m_viewMessage1->EvLClick += RTW_CALLBACK(this, UILayerMain, OnHtmlAction_HtmlHelpView);
	m_btnMessageOK1 = LOAD_UI_T(RtwButton, "fmhtmlmessage1.btnaccept");
	m_btnMessageOK1->EvLClick += RTW_CALLBACK(this, UILayerMain, OnClicked_ShowScript1);
	m_btnMessageCancel1 = LOAD_UI_T(RtwButton, "fmhtmlmessage1.btncancel");
	m_btnMessageCancel1->EvLClick += RTW_CALLBACK(this, UILayerMain, OnClicked_CancelScript1);

	m_formMessageBox2 = LOAD_UI("fmhtmlmessage2");
	m_viewMessage2 = LOAD_UI_T(RtwHtmlView, "fmhtmlmessage2.fmhtml.viewright");
	m_viewMessage2->ModifyFlags(0,wfKeyboardEvent);
	m_viewMessage2->SetEnableInput(false);
	m_viewMessage2->EvLClick += RTW_CALLBACK(this, UILayerMain, OnHtmlAction_HtmlHelpView);
	m_btnMessageOK2 = LOAD_UI_T(RtwButton, "fmhtmlmessage2.btnaccept");
	m_btnMessageOK2->EvLClick += RTW_CALLBACK(this, UILayerMain, OnClicked_ShowScript2);

	//活动相关
	m_LableTimer = LOAD_UI_T(RtwLabel, "lbtask");
	m_formSceneInfo = LOAD_UI_T(RtwForm, "fmact");
	m_LableSceneInfo = LOAD_UI_T(RtwLabel, "fmact.lbbattle");
	m_formSceneInfo1 = LOAD_UI_T(RtwForm, "fmact1");
	m_LableSceneInfo11 = LOAD_UI_T(RtwLabel, "fmact1.lbbattle");
	m_LableSceneInfo12 = LOAD_UI_T(RtwLabel, "fmact1.lbbattle1");
	m_formSceneInfo2 = LOAD_UI_T(RtwForm, "fmact2");
	m_LableSceneInfo21 = LOAD_UI_T(RtwLabel, "fmact2.lbbattle");
	m_LableSceneInfo22 = LOAD_UI_T(RtwLabel, "fmact2.lbbattle1");
	m_LableSceneInfo23 = LOAD_UI_T(RtwLabel, "fmact2.lbbattle2");

	m_btnAutoBattle->EvLClick		+= RTW_CALLBACK(this, UILayerMain, OnClicked_AutoBattle);		//for  auto    ac.ma
	m_btnAutoB->EvLClick			+= RTW_CALLBACK(this,UILayerMain,OnClicked_AutoFight);
	m_btnAutoPick->EvLClick			+= RTW_CALLBACK(this, UILayerMain, OnClicked_AutoPick);	
	m_btnAutoEatRed->EvLClick		+= RTW_CALLBACK(this, UILayerMain, OnClicked_AutoEatRed);
	m_btnAutoEatBlue->EvLClick		+= RTW_CALLBACK(this, UILayerMain, OnClicked_AutoEatBlue);
	m_btnAutoHp30->EvLClick			+= RTW_CALLBACK(this, UILayerMain, OnClicked_AutoHp30);
	m_btnAutoHp60->EvLClick			+= RTW_CALLBACK(this, UILayerMain, OnClicked_AutoHp60);
	m_btnAutoHp90->EvLClick			+= RTW_CALLBACK(this, UILayerMain, OnClicked_AutoHp90);
	m_btnAutoSellWhite->EvLClick	+= RTW_CALLBACK(this, UILayerMain, OnClicked_AutoSellWhite);
	m_btnAutoSellGreen->EvLClick	+= RTW_CALLBACK(this, UILayerMain, OnClicked_AutoSellGreen);
	m_btnAutoSellBlue->EvLClick		+= RTW_CALLBACK(this, UILayerMain, OnClicked_AutoSellBlue);
	m_btnAutoSellGold->EvLClick		+= RTW_CALLBACK(this, UILayerMain, OnClicked_AutoSellGold);
	
	
	//新功能
	//m_fmRage			= LOAD_UI("fmrage");
	//m_btnRage			= LOAD_UI_T(RtwButton, "fmhp.rage");  //gao 2010.1.5 游戏中未用到的变量
//	m_proRage			= LOAD_UI_T(RtwProgressBar,"fmhp.barrage");
	//m_proRage->SetDirection(2);

	m_playerForm		= LOAD_UI("fmhp");
	m_playerLevel		= LOAD_UI("fmhp.lblv");
	m_playerName		= LOAD_UI("fmhp.lbname");
	m_playerHp			= LOAD_UI("fmhp.lbhpcount");
	m_playerHpBar		= LOAD_UI_T(RtwProgressBar, "fmhp.barhp");
	m_playerMp			= LOAD_UI("fmhp.lbmpcount");
	m_playerMpBar		= LOAD_UI_T(RtwProgressBar, "fmhp.barmp");
	/* gao 2009.12.24
	修改经验条设置*/
//	m_playerExp			= LOAD_UI("fmhp.lbexpcount");
	m_playerExpBar		= LOAD_UI_T(RtwProgressBar, "fmhotkey.barexp");
	/* end */

	//m_btnRemind[0]		= LOAD_UI_T(RtwButton, "btninfo3");
	//m_btnRemind[0]->EvLClick += RTW_CALLBACK(this, UILayerMain, OnClicked_RemindIcon);
	//m_btnRemind[1]		= LOAD_UI_T(RtwButton, "btninfo2");
	//m_btnRemind[1]->EvLClick += RTW_CALLBACK(this, UILayerMain, OnClicked_RemindIcon);
	//m_btnRemind[2]		= LOAD_UI_T(RtwButton, "btninfo1");
	//m_btnRemind[2]->EvLClick += RTW_CALLBACK(this, UILayerMain, OnClicked_RemindIcon);

    // 心情动作 Begin
    LOAD_UI(g_storedIcon[ 8])->EvLClick += RTW_CALLBACK_1(this, UILayerMain, OnFunAction, (void*)1);
	LOAD_UI(g_storedIcon[ 8])->SetParam1((void *)1);
    LOAD_UI(g_storedIcon[ 9])->EvLClick += RTW_CALLBACK_1(this, UILayerMain, OnFunAction, (void*)2);
	LOAD_UI(g_storedIcon[ 9])->SetParam1((void *)2);
    LOAD_UI(g_storedIcon[10])->EvLClick += RTW_CALLBACK_1(this, UILayerMain, OnFunAction, (void*)3);
	LOAD_UI(g_storedIcon[10])->SetParam1((void *)3);
    LOAD_UI(g_storedIcon[11])->EvLClick += RTW_CALLBACK_1(this, UILayerMain, OnFunAction, (void*)4);
	LOAD_UI(g_storedIcon[11])->SetParam1((void *)4);
    LOAD_UI(g_storedIcon[12])->EvLClick += RTW_CALLBACK_1(this, UILayerMain, OnFunAction, (void*)5);
	LOAD_UI(g_storedIcon[12])->SetParam1((void *)5);
    LOAD_UI(g_storedIcon[13])->EvLClick += RTW_CALLBACK_1(this, UILayerMain, OnFunAction, (void*)6);
	LOAD_UI(g_storedIcon[13])->SetParam1((void *)6);
    LOAD_UI(g_storedIcon[14])->EvLClick += RTW_CALLBACK_1(this, UILayerMain, OnFunAction, (void*)7);
	LOAD_UI(g_storedIcon[14])->SetParam1((void *)7);

	//聊天窗中连接的悬浮窗口
 	m_fmhyperLinkInChat = LOAD_UI("fmhyperLink");
 	m_htmlviewLink		= LOAD_UI_T(RtwHtmlView, "fmhyperLink.htmlhint");
 	m_htmlviewLink->SetEnableInput(false);
 	m_htmlviewLink->SetAutoSizeV(true);
	//end

	//倒计时奖励
	//begin
	m_nCountDownMillisecond		= 0;
	m_fmCountDown				= LOAD_UI("fmTimer");
	m_lblTitle					= LOAD_UI_T(RtwLabel, "fmTimer.lbTile");
	m_lblCountDownConditionText = LOAD_UI_T(RtwLabel, "fmTimer.lbTarget1");
	m_lblCountDown				= LOAD_UI_T(RtwLabel, "fmTimer.lbTime");
	m_lblCountDownText			= LOAD_UI_T(RtwLabel, "fmTimer.lbTime1");

	m_lblTitle->SetTextColor(RtwPixel(0xFFFFEFD5));
	m_lblCountDownConditionText->SetTextColor(RtwPixel(0xffE0FFFF));
	//end

    // 心情动作 End
	m_formCharProp = RT_NEW UIFormCharProp;
	m_formCharProp->Init();
	
	//tim.yang  神兽系统  初始化系统面板
	m_formPetProp = RT_NEW UIFormPetProp;
	m_formPetProp->Init();
	//end

	m_formPetSimple = RT_NEW UIFormPetSimple;
	m_formPetSimple->Init();

	m_formCharSimple = RT_NEW UIFormCharSimple;
	m_formCharSimple->Init();

	m_formMwAddMp = RT_NEW UIFormMagicWeaponAddMp;
	m_formMwAddMp->Init();

	m_formMwAddEle = RT_NEW UIFormMagicWeaponAddEle;
	m_formMwAddEle->Init();

	m_formTrump = RT_NEW CUIForm_Trump;

	//任务系统面板
	m_formTask = RT_NEW UIFormTask;
	//主线任务面板
	m_formMainTask = RT_NEW UIFormMainTask;

	m_formRecoverPet = RT_NEW UIFormRecoverPet;
	m_formRecoverPet->Init();
	
	m_formClearSkill = RT_NEW UIFormClearSkill;
	m_formClearSkill->Init();

	m_fromPlayerItems = RT_NEW UIFormPlayerItems;
	// m_fromPlayerItems->SetPlayer( GetWorld()->m_pPlayer );
	m_formTrade = RT_NEW UIFormTrade;
	m_formShop = RT_NEW UIFormShop;
	m_formBank = RT_NEW UIFormBank;
	m_formItemUpdate = RT_NEW UIFormItemUpdate;
	m_formFeodTax = RT_NEW UIFormFeodTax;
	m_formFeodBank = RT_NEW UIFormFeodBank;
	m_formFeodContent = RT_NEW UIFormFeodContent;

	m_formUnionCreate = RT_NEW UIFormUnionCreate;
	m_formUnion = RT_NEW UIFormUnion;
	m_formHeroList = RT_NEW UIFormHeroList;
	m_formGm = RT_NEW UIFormGm;

    m_fromSkill = RT_NEW UIFormSkill;
    m_fromSkillStudy = RT_NEW UIFormSkillStudy;
    m_fromMiniMap = RT_NEW UIFormMiniMap;
	m_formPkMode = RT_NEW UIFormPkMode;

//----------------add start by tony 05.06.06---------------------------------//
	m_formCastleWarReg		= RT_NEW UIFormCastleWar;
	m_formCastleWarTimeSet	= RT_NEW UIFormCastleWarTimeSet;
	m_formTimer				= RT_NEW UIFormTimer;
	m_formDupe				= RT_NEW UIFormDupe;
// 注释 [3/18/2009 tooth.shi] 	RtwHyperlink::EvAllHyperlink += RTW_CALLBACK(m_formDupe, UIFormDupe, OnUIHyperlink);
//----------------add start by tony 05.06.06---------------------------------//

	m_formVendor			= RT_NEW UIFormVendor;
	m_formBid				= RT_NEW UIFormBid;
	m_formSale				= RT_NEW UIFormSale;
	m_formWarfield			= RT_NEW UIFormWarfield;

	m_formItemsInfo = RT_NEW UIFormItemsInfo;
	m_formCreInfo = RT_NEW UIFormCreInfo;
	m_formSelectReward = RT_NEW UIFormSelectReward;

    m_formHelpSystem = RT_NEW UIFormHelp;

// 注释 [3/18/2009 tooth.shi]    RtwHyperlink::EvAllHyperlink += RTW_CALLBACK(m_formHelpSystem, UIFormHelp, OnUIHyperlink);

    m_formCard = RT_NEW UIFormCard;
	m_formRebirth = RT_NEW UIFormRebirth;
    m_formSellCard = RT_NEW UIFormSellCard;
    m_formBuyCard = RT_NEW UIFormBuyCard;

	m_formAnalyseImage = RT_NEW UIFormAnalyseImage;
	m_formBankPass = RT_NEW UIFormBankPass;
	m_formItemCompose = RT_NEW UIFormItemCompose;
	m_formTimeBar = RT_NEW UIFormTimeBar;
	m_formItemFuse = RT_NEW UIFormItemFuse;
    m_formPetEat = RT_NEW UIFormPetEat;
	/*************************gao 2009.11.5*******************************/
	m_formMonInfo 		= RT_NEW UIFormMonInfo;
	m_formChant 		= RT_NEW CUI_Form_Chant;
	m_formHotKey 		= RT_NEW CUIForm_System;
	m_formTeam 			= RT_NEW CUI_form_Team;
	m_formSpeaker		= RT_NEW CUIForm_Speaker;
	m_formChar			= RT_NEW CUIForm_Char;
	m_formMiddleMap		= RT_NEW CUIForm_MiddleMap;
	m_formGem			= RT_NEW CUIForm_Gem;	
	m_formPickItem		= RT_NEW CUIForm_Pickup;
	m_formCollectBar	= RT_NEW CUIForm_Collect;
	m_TimerHint			= RT_NEW CTimer_Hint;
	m_formPlayerShop	= RT_NEW CUIFormPlayerShop;
	m_formLookPlayerShop= RT_NEW CUIFormLookPlayerShop;
	//
	m_bNeedSetTranForShotcut = false;
	//m_formPlayerShop->Show();
	//m_formLookPlayerShop->Show();
	/*********************************************************************/
	m_formItemDecompound = RT_NEW UIFormItemDecompound;

	//新手升级指引
	m_pNewPeopleHelp	= RT_NEW CUIForm_NewPeopleHelp;

	m_formPetMain		= RT_NEW UIFromPetMain;
	m_formPetAptitude	= RT_NEW UIFromPetAptitude;
	m_formPetCtrl		= RT_NEW UIFromPetCtrl;
	m_formPetAbility	= RT_NEW UIFormPetAbility;
	m_formPetMerge		= RT_NEW UIFormPetMerge;
	m_formPetToBeads	= RT_NEW UIFormPetToBeads;
	m_formPetMergeBeads	= RT_NEW UIFormPetMergeBeads;
	m_formPetBank		= RT_NEW UIFormBankPet;
	m_formShopCenter    = RT_NEW UIFormShopCenter;

	itemtext=0;
	// Init
    m_viewHelp->SetUserData(0);
    m_formHelp->EvHide += RTW_CALLBACK_F(OnHtmlViewHide);
    g_workspace.SetSysEditName("fmdialg.fmtxtdialog.txtdialog");// add by fancui for system edit @2005-04-27
	LOAD_UI("fmBuff")->ModifyFlags(0,wfKeyboardEvent);//人物状态buffer条是不应该被active 的窗口

	/* gao 2010.1.13
	读取游戏的聊天输入以及显示
	*/
	RtIni iniUser;
	if (iniUser.OpenFile(R(INI_USER)))
	{
		char input[20];
		char show[20];
		iniUser.GetEntry("person","input",input);
		iniUser.GetEntry("person","show",show);
		GetWorld()->m_UIProject.SetShowProject(show);
		GetWorld()->m_UIProject.SetInputProject(input);

		iniUser.CloseFile();
	}
	/* end */
	m_bCanUseSwitchWeapon = true;
	
	unguard;
}

UILayerMain::~UILayerMain()
{
	guard;
    DEL_ONE(m_formBuyCard);
    DEL_ONE(m_formSellCard);
    DEL_ONE(m_formCard);
    DEL_ONE(m_fromSkill);
	//for (int n = 0; n < UI_HOT_KEY_COUNT; n++)
	//	if (m_hotKeyGroup[n])
	//		m_hotKeyGroup[n] = NULL;

	DEL_ONE(m_formHelpSystem);
	DEL_ONE(m_formSelectReward);
	DEL_ONE(m_formCreInfo);
	/********************************gao 2009.11.5    删除*******************************/
	DEL_ONE(m_formMonInfo);
	DEL_ONE(m_formChant);
	DEL_ONE(m_formHotKey);
	DEL_ONE(m_formSpeaker);
	DEL_ONE(m_formChar);
	DEL_ONE(m_formTask);        //----任务系统
	DEL_ONE(m_formMainTask);
	DEL_ONE(m_formTeam);
	DEL_ONE(m_formMiddleMap);
	DEL_ONE(m_formGem);
	DEL_ONE(m_TimerHint);
	DEL_ONE(m_formPlayerShop);
	DEL_ONE(m_formLookPlayerShop);
	/***************************************************************************************/
	DEL_ONE(m_formItemsInfo);

	//---------------add start by tony 05.06.06----------------------------------//
	DEL_ONE(m_formDupe);
	DEL_ONE(m_formTimer);
	DEL_ONE(m_formCastleWarTimeSet);
	DEL_ONE(m_formCastleWarReg);
	//---------------add start by tony 05.06.06----------------------------------//

	DEL_ONE(m_formVendor);
	DEL_ONE(m_formBid);
	DEL_ONE(m_formSale);

	DEL_ONE(m_formPkMode);
	DEL_ONE(m_fromMiniMap);
	DEL_ONE(m_fromSkillStudy);
	DEL_ONE(m_fromSkill);//<add by fox for memory leak>


	DEL_ONE(m_formGm);
	DEL_ONE(m_formHeroList);
	DEL_ONE(m_formUnion);
	DEL_ONE(m_formUnionCreate);

	DEL_ONE(m_formFeodContent);
	DEL_ONE(m_formFeodBank);
	DEL_ONE(m_formFeodTax);
	DEL_ONE(m_formItemUpdate);
	DEL_ONE(m_formBank);
	DEL_ONE(m_formShop);
	DEL_ONE(m_formTrade);
	DEL_ONE(m_fromPlayerItems);
	DEL_ONE(m_formWarfield);

	m_formClearSkill->Exit();//<add by fox for memory leak>
	DEL_ONE(m_formClearSkill);//<add by fox for memory leak>

	m_formRecoverPet->Exit();//<add by fox for memory leak>
	DEL_ONE(m_formRecoverPet);//<add by fox for memory leak>

	m_formMwAddEle->Exit();//<add by fox for memory leak>
	DEL_ONE(m_formMwAddEle);//<add by fox for memory leak>

	m_formMwAddMp->Exit();//<add by fox for memory leak>
	DEL_ONE(m_formMwAddMp);//<add by fox for memory leak>

	m_formCharSimple->Exit();//<add by fox for memory leak>
	DEL_ONE(m_formCharSimple);//<add by fox for memory leak>

	m_formPetSimple->Exit();//<add by fox for memory leak>
	DEL_ONE(m_formPetSimple);//<add by fox for memory leak>

	DEL_ONE(m_formTrump);//<add by fox for memory leak>

	//tim.yang   神兽系统
	m_formPetProp->Exit();
	DEL_ONE(m_formPetProp);
	//end
	m_formCharProp->Exit();
	DEL_ONE(m_formCharProp);

	DEL_ONE(m_formAnalyseImage);
	DEL_ONE(m_formSetting);
	DEL_ONE(m_formBankPass);
	DEL_ONE(m_formItemCompose);
	DEL_ONE(m_formTimeBar);
	DEL_ONE(m_formItemFuse);
    DEL_ONE(m_formPetEat);
	DEL_ONE(m_formTeam);
	DEL_ONE(m_formPickItem);
	DEL_ONE(m_formCollectBar);

	//新手升级指引
	DEL_ONE(m_pNewPeopleHelp);

	DEL_ONE(m_formPetMain);
	DEL_ONE(m_formPetAptitude);
	DEL_ONE(m_formPetAbility);
	DEL_ONE(m_formPetMerge);
	DEL_ONE(m_formPetToBeads);
	DEL_ONE(m_formPetMergeBeads);
	DEL_ONE(m_formPetBank);
	DEL_ONE(m_formShopCenter);

	unguard;
}

void UILayerMain::InitWidgetLabel(int Metier)
{
	m_fromSkill->SetWidgetLabelName(Metier);
	if(!m_formSetting)
		m_formSetting = RT_NEW UIFormSetting;
	CG_CmdPacket* pPacket = NetBeginWrite();
	pPacket->WriteShort(c2r_set_refusesetting);
	pPacket->WriteByte(g_layerMain->m_eSetting.m_bRefuseTeam);
	pPacket->WriteByte(g_layerMain->m_eSetting.m_bRefuseAction);
	pPacket->WriteByte(g_layerMain->m_eSetting.m_bRefuseMail);
	pPacket->WriteByte(g_layerMain->m_eSetting.m_bRefuseDuel);
	pPacket->WriteByte(g_layerMain->m_eSetting.m_bRefuseFriend);
	pPacket->WriteByte(g_layerMain->m_eSetting.m_bRefuseMonster);
	NetSend(pPacket);
}
/* gao 2009.12.24
点击队伍按钮时的配置
*/
void UILayerMain::OnClicked_TeamButton(RtwWidget* vpSender, RtwEventDelegate* e)
{
	guard;
	//if(GetWorld()->m_Team.mData.Size() == 0)
	//{
	//	//
	//	
	//	LOAD_UI_T(RtwTab,"fmteam.fmcontent.tbcteam")->SetSelectedItem(1);
	//	/*LOAD_UI("fmteam.fmcontent.tbcteam.btnmembers")->Disable();
	//	LOAD_UI("fmteam.fmcontent.tbcteam.btnrequest")->Disable();*/
	//	m_formTeam
	//}
	//else 
	if(m_formTeam->IsVisibale())
		m_formTeam->Hide();
	else
	{
		m_fromMiniMap->PlayerListUIChange(vpSender,e);
		m_formTeam->Show();
	}
	unguard;
}
/* end */

/* gao 2010.3.2
帮助窗口的显示
*/
void UILayerMain::OnShowHelp(RtwWidget* vpSender, RtwEventDelegate* e)
{
	guard;
	if(m_formHelpSystem->IsVisible())
		m_formHelpSystem->Hide();
	else 
		m_formHelpSystem->Show();
	unguard;
}
/* gao 2009.12.29
layer_main中的逻辑循环
用来实现窗口的部分移动动画
*/
void UILayerMain::DoFrame()
{
	guard;
	m_formHotKey->DoFrame();
	m_fromMiniMap->DoFrame();
	GetWorld()->m_Chat.DoFrame();
	static Real tran = 0.f;
	if(m_bNeedSetTranForShotcut)
	{
		if(tran < 0.6)
		{
			LOAD_UI("fmshortcut_bar1")->SetTransparency(tran);
			if(LOAD_UI("fmshortcut_bar2")->IsVisible())
				LOAD_UI("fmshortcut_bar2")->SetTransparency(tran);
			char hotKeyLabel[50];
			for(size_t i = 0;i < 10;++i)
			{
				rt2_sprintf(hotKeyLabel,"fmshortcut_bar1.lbf%d",i+1);
				if(m_hotKeyAlias[i+10]->GetButton())
				{
					LOAD_UI(hotKeyLabel)->SetTransparency(1.f);
					m_hotKeyAlias[i+10]->SetTransparency(1.f);
				}
				if(LOAD_UI("fmshortcut_bar2")->IsVisible())
				{
					rt2_sprintf(hotKeyLabel,"fmshortcut_bar2.lbf%d",i+1);
					if(m_hotKeyAlias[i+20]->GetButton())
					{
						LOAD_UI(hotKeyLabel)->SetTransparency(1.f);
						m_hotKeyAlias[i+20]->SetTransparency(1.f);
					}
				}
			}
			tran += 0.02f;
		}
	}else
	{
		LOAD_UI("fmshortcut_bar1")->SetTransparency(tran);
		if(LOAD_UI("fmshortcut_bar2")->IsVisible())
			LOAD_UI("fmshortcut_bar2")->SetTransparency(tran);
		char hotKeyLabel[50];
		for(size_t i = 0;i < 10;++i)
		{
			rt2_sprintf(hotKeyLabel,"fmshortcut_bar1.lbf%d",i+1);
			if(m_hotKeyAlias[i+10]->GetButton())
			{
				LOAD_UI(hotKeyLabel)->SetTransparency(1.f);
				m_hotKeyAlias[i+10]->SetTransparency(1.f);
			}
			if(LOAD_UI("fmshortcut_bar2")->IsVisible())
			{
				rt2_sprintf(hotKeyLabel,"fmshortcut_bar2.lbf%d",i+1);
				if(m_hotKeyAlias[i+20]->GetButton())
				{
					LOAD_UI(hotKeyLabel)->SetTransparency(1.f);
					m_hotKeyAlias[i+20]->SetTransparency(1.f);
				}
			}
		}
		tran -= 0.02f;
		if(tran <= 0.f)
		{
			tran = 0.f;
		}
	}
	unguard;
}
/* edn */
void UILayerMain::UpDateUIBlood()
{
	guard;
	g_layerMain->m_formMonInfo->Refresh();
	g_layerMain->m_formCreInfo->Refresh();
	unguard;
}

void UILayerMain::OnKeyChar(void*, void*)
{
	guard;
	unguard;
}

/*
void UILayerMain::OnUpdateText_Chat(RtwWidget* sender, void*)
{
	guard;

	GetWorld()->m_Chat.Send((char*)m_chatInput->GetText().c_str());
	//m_chatInput->SetFocus();

	if(mKeepFocus)
	{
		m_chatInput->SetFocus();
	}
	else
	{
		GetWorkspace()->SetFocusWidget(this);
	}

	unguard;
}
*/
bool UILayerMain::Createheadimage(GcPlayer*  m_pPlayer)
{
	guard;
	//SCreModel* pModel=g_TableCreModel.FindCreModel(m_pPlayer->m_core.ModelId);
// 	SHeadModel pModel;
// 	if(g_TableHeadModel.GetHeadModelByID(m_pPlayer->m_core.HeadModelId,pModel))
// 	{
// 		RtwButton* btnhead=LOAD_UI_T(RtwButton, "fmhp.headhp");
// 		RtwImage* pImage = g_workspace.getImageFactory()->createImage("zb_texture/"+pModel.headimage+".dds");
// 		btnhead->SetBackgroundImage(pImage);
// 	}
	return true;
	unguard;
}

void UILayerMain::OnClick_HotKey(RtwWidget* sender, void*)
{
	guard;

	//if (sender == m_hotKeyLeft)
	//{
	//	if (m_hotKeyGroupNumber == 0)
	//		return;			
	//}
	//else if (sender == m_hotKeyRight)
	//{
	//	if (m_hotKeyGroupNumber == 1)
	//		return;
	//}

	//SwitchHotKeyGroup();

	unguard;
}

void UILayerMain::OnClick_FbTask(void*, void*)
{
	guard;

	//m_formDupe->BuildTaskTree();
	m_formDupe->Show();
	unguard;
}

void UILayerMain::OnClicked_System_System(void*, void*)
{
	guard;

	RtwWidget* w = LOAD_UI("fmsyssetting");

	if (w->GetFlags(wfVisible))
		w->Hide();
	else
	{
		w->Show();
	}

	unguard;
}

void UILayerMain::OnClicked_System_Map(void*, void*)
{
	guard;
	if(m_formMiddleMap->IsVisible())
			m_formMiddleMap->Hide();
	else
	{
		m_formMiddleMap->Show();
		m_formMiddleMap->SetFirstOpenMap(true);
	}
	unguard;
}

void UILayerMain::OnClicked_System_Battle(void*, void*)
{

}

void UILayerMain::OnLClicked_System_Battle(void*, void*)
{
	guard;
	if (m_fmsysbattle->GetFlags(wfVisible))
		m_fmsysbattle->Hide();
	else
	{
		m_fmsysbattle->Show();
	}
	unguard;
}
void UILayerMain::OnClicked_System_Char(void* ,void*)
{
	guard;

	if (m_formChar->IsVisible())
		m_formChar->Hide();
	else
		m_formChar->Show(true);

	unguard;
}

void UILayerMain::OnClicked_SysSetting_SysSet(void*, void*)
{
	guard;

	LOAD_UI("fmsyssetting")->Hide();

	if (!m_formSetting)
		m_formSetting = RT_NEW UIFormSetting;

	m_formSetting->Change();

	unguard;
}

void UILayerMain::OnClicked_SysSetting_Restart(void*, void*)
{
	guard;

	LOAD_UI("fmsyssetting")->Hide();
	LOAD_UI("fmshortcut_bar1")->Hide();
	LOAD_UI("fmshortcut_bar2")->Hide();

	if (GetWorld())
	{
		GetWorld()->OnLocalSaveShortcut();  //add by yz
		GetWorld()->OnNetUploadShortcut();
	}
		

    ChangeGameFlow(false);

    unguard;
}

void UILayerMain::OnClicked_SysSetting_Exit(void*, void*)
{
	guard;

	LOAD_UI("fmsyssetting")->Hide();

	LOAD_UI("fmexitconfirm")->Show();

	unguard;
}

void UILayerMain::OnClicked_SysSetting_GM(void*, void*)
{
	guard;

	LOAD_UI("fmsyssetting")->Hide();

	if(m_formGm->IsVisible())
	{
		m_formGm->Hide();
	}
	else
	{
		m_formGm->Show();
	}
	

	unguard;
}
void UILayerMain::OnClicked_SysSetting_Help(void*,void*)
{
   	guard;

	LOAD_UI("fmsyssetting")->Hide();
	if(!LOAD_UI("fmhelpsys")->IsVisible())
		LOAD_UI("fmhelpsys")->Show();
	else
		LOAD_UI("fmhelpsys")->Hide();
	//m_formGm->Show();
	unguard;
}
void UILayerMain::OnButtonPetDown(RtwWidget* vpSender, RtwEventDelegate* e)
{
	guard;
	//宠物新手提示
	GetWorld()->ShowCallPet();

	if (m_formPetMain->IsVisible())
	{
		m_formPetMain->Hide();
		if (m_formPetAptitude->IsVisible())
		{
			m_formPetAptitude->Hide();
		}
	}
	else
	{
		m_formPetMain->Show();
	}
	unguard;
}

void UILayerMain::OnClicked_SysSetting_Close(void*, void*)
{
	guard;

	LOAD_UI("fmsyssetting")->Hide();

	unguard;
}

void UILayerMain::OnClicked_ExitConfirm_Confirm(void*, void*)
{
	guard;

	LOAD_UI("fmexitconfirm")->Hide();

	PostQuitMessage(0);

	unguard;
}

void UILayerMain::OnClicked_ExitConfirm_Cancel(void*, void*)
{
	guard;

	LOAD_UI("fmexitconfirm")->Hide();

	unguard;
}

void UILayerMain::OnItemClicked_HtmlHelpView(RtwWidget* sender, void*)
{
	guard;

	m_formHelp->Hide();

	unguard;
}

void UILayerMain::OnHtmlTask_HtmlHelpView(RtwWidget* sender, void* pParam)
{
	guard;
	if (g_layerMain->m_formShop->IsVisible())
	{
		return;
	}
	//新手键盘操作提示
	GetWorld()->ShowKeyboardGuide();

	RtwEventDelegate* pEvent = (RtwEventDelegate*)pParam;
	const char* szHref;
	RtwTextBox::SItem* pItem = ((RtwHtmlView*)sender)->PickItem(SPoint(pEvent->mouse.x, pEvent->mouse.y));
	if (!pItem) return ;
	szHref = (const char*)(pItem->hyperLink.Text.c_str()+2);
	if (pEvent->type==weMouseClick && pItem->hyperLink.Type == hlinkClient)
	{
		if (strncmp(szHref, "clientmoveto", 12) == 0 || strncmp(szHref, "clientmovetonpc", 15) == 0)
		{
			std::string taskDec = szHref;
			ClientMoveto(taskDec);
		}
	}
	unguard;
}
//tim.yang 自动寻路到某地
void UILayerMain::ClientMoveto(std::string szHref)
{
	guard;
	int nops = -1;
	std::string step = " ";
	std::string temp = szHref;
	if (strstr(szHref.c_str(),"npc"))
	{
		temp = temp.substr(strlen("clientmovetonpc")+1);
		SNpc* pNpc = g_TableNpc.FindNpc(atoi(temp.c_str()));
		if (pNpc)
		{
			GetWorld()->m_pPlayer->GotoNpcFront(pNpc);
		}
	}
	else if(strstr(szHref.c_str(), "scene1"))
	{
		temp = temp.substr(strlen("clientmovetoscene1") + 1);

		std::string sceneName1 = "";
		std::string sceneName2 = "";
		int x1 = -99999;
		int y1 = -99999;
		int x2 = -99999;
		int y2 = -99999;
		int start = 0;
		int end = temp.find(step,start);
		if (std::string::npos != end)
			sceneName1 = temp.substr(start,end);

		start = end + 1;
		end = temp.find(step,start);
		if (std::string::npos != end)
			x1 = atoi(temp.substr(start,end).c_str());
		start = end + 1;
		end = temp.find(step,start);
		if (std::string::npos != end)
		y1 = atoi(temp.substr(start, end).c_str());

		start = end + 1;
		end = temp.find(step,start);
		if (std::string::npos != end)
			sceneName2 = temp.substr(start,end);

		start = end + 1;
		end = temp.find(step,start);
		if (std::string::npos != end)
			x2 = atoi(temp.substr(start,end).c_str());
		start = end + 1;
		y2 = atoi(temp.substr(start).c_str());

		if (GetWorld()->GetSceneFileName() == sceneName1)
		{
			if (-99999 != y1 && -99999 != x1)
			{
				GetWorld()->m_pPlayer->GotoThere(x1,y1);
			}
		} 
		else
		{
			if (-99999 != y2 && -99999 != x2)
			{
				GetWorld()->m_pPlayer->GotoThere(x2,y2);
			}
		}
	}
	else if(strstr(szHref.c_str(), "scene2"))
	{
		temp = temp.substr(strlen("clientmovetoscene2") + 1);

		std::string sceneName1 = "";
		std::string sceneName2 = "";
		int npcID1 = 0;
		int npcID2 = 0;
		int start = 0;
		int end = temp.find(step,start);
		if (std::string::npos != end)
			sceneName1 = temp.substr(start,end);

		start = end + 1;
		end = temp.find(step,start);
		if (std::string::npos != end)
			npcID1 = atoi(temp.substr(start,end).c_str());

		start = end + 1;
		end = temp.find(step,start);
		if (std::string::npos != end)
			sceneName2 = temp.substr(start,end);

		start = end + 1;
		npcID2 = atoi(temp.substr(start).c_str());

		if (GetWorld()->GetSceneFileName() == sceneName1)
		{
			SNpc* pNpc = g_TableNpc.FindNpc(npcID1);
			if (pNpc)
			{
				GetWorld()->m_pPlayer->GotoNpcFront(pNpc);
			}
		} 
		else
		{
			SNpc* pNpc = g_TableNpc.FindNpc(npcID2);
			if (pNpc)
			{
				GetWorld()->m_pPlayer->GotoNpcFront(pNpc);
			}
		}
	}
	else if(strstr(szHref.c_str(), "scene3"))
	{
		temp = temp.substr(strlen("clientmovetoscene3") + 1);

		std::string sceneName1 = "";
		std::string sceneName2 = "";
		int npcID1 = 0;
		int x2 = -99999;
		int y2 = -99999;
		int start = 0;
		int end = temp.find(step,start);
		if (std::string::npos != end)
			sceneName1 = temp.substr(start,end);

		start = end + 1;
		end = temp.find(step,start);
		if (std::string::npos != end)
			npcID1 = atoi(temp.substr(start,end).c_str());

		start = end + 1;
		end = temp.find(step,start);
		if (std::string::npos != end)
			sceneName2 = temp.substr(start,end);

		start = end + 1;
		end = temp.find(step,start);
		if (std::string::npos != end)
			x2 = atoi(temp.substr(start,end).c_str());
		start = end + 1;
		y2 = atoi(temp.substr(start).c_str());

		if (GetWorld()->GetSceneFileName() == sceneName1)
		{
			SNpc* pNpc = g_TableNpc.FindNpc(npcID1);
			if (pNpc)
			{
				GetWorld()->m_pPlayer->GotoNpcFront(pNpc);
			}
		} 
		else
		{
			if (-99999 != y2 && -99999 != x2)
			{
				GetWorld()->m_pPlayer->GotoThere(x2,y2);
			}
		}
	}
	else if(strstr(szHref.c_str(), "scene4"))
	{
		temp = temp.substr(strlen("clientmovetoscene4") + 1);

		std::string sceneName1 = "";
		std::string sceneName2 = "";
		int x1 = -99999;
		int y1 = -99999;
		int npcID2 = 0;
		int start = 0;
		int end = temp.find(step,start);
		if (std::string::npos != end)
			sceneName1 = temp.substr(start,end);

		start = end + 1;
		end = temp.find(step,start);
		if (std::string::npos != end)
			x1 = atoi(temp.substr(start,end).c_str());
		start = end + 1;
		end = temp.find(step,start);
		if (std::string::npos != end)
			y1 = atoi(temp.substr(start, end).c_str());

		start = end + 1;
		end = temp.find(step,start);
		if (std::string::npos != end)
			sceneName2 = temp.substr(start,end);

		start = end + 1;
		npcID2 = atoi(temp.substr(start).c_str());

		if (GetWorld()->GetSceneFileName() == sceneName1)
		{
			if (-99999 != y1 && -99999 != x1)
			{
				GetWorld()->m_pPlayer->GotoThere(x1,y1);
			}
		} 
		else
		{
			SNpc* pNpc = g_TableNpc.FindNpc(npcID2);
			if (pNpc)
			{
				GetWorld()->m_pPlayer->GotoNpcFront(pNpc);
			}
		}
	}
	else
	{
		temp = temp.substr(strlen("clientmoveto")+1);
		int start = 0;
		int end = temp.find(step,start);
		int x = -99999;
		int y = -99999;
		if (std::string::npos != end)
			x = atoi(temp.substr(start,end).c_str());
		start = end + 1;
		y = atoi(temp.substr(start).c_str());
		if (-99999 != y && -99999 != x)
		{
			GetWorld()->m_pPlayer->GotoThere(x,y);
		}
	}
	unguard;
}

////added by ldr123 at 2010 04 16
////发送消息到服务器实现(超链接)
////begin
//bool UILayerMain::ItemHyperLinkSend(long itemId)
//{
//	CG_CmdPacket* pPacket = NetBeginWrite();
//	pPacket->WriteShort(c2r_hyperLink_item);
//	pPacket->WriteLong(itemId);
//	return NetSend(pPacket);
//}
////end

void UILayerMain::OnHtmlAction_HtmlHelpView(RtwWidget* sender, void* pParam)
{
    guard;
//     if (m_viewHelp->GetUserData()==0) return;
    RtwEventDelegate* pEvent = (RtwEventDelegate*)pParam;
    const char* szHref;
	
	RtwTextBox::SItem* pItem = ((RtwHtmlView*)sender)->PickItem(SPoint(pEvent->mouse.x, pEvent->mouse.y));
	if (!pItem) return ;
	szHref = (const char*)(pItem->hyperLink.Text.c_str()+2);
    if (pEvent->type==weMouseClick && pItem->hyperLink.Type == hlinkRServer)
    {
        //szHref = (const char*)(pEvent->action.descriptor);//tooth.shi
       if (szHref && szHref[0])
        {
			if (strncmp(szHref, "Script_ShowRet", 14) == 0)
			{
				char* szMsg = "ShowRet";
				CG_CmdPacket *cmd = NetBeginWrite();
				cmd->WriteShort(c2r_message_dialog_sure);
				cmd->WriteString(szMsg);
				NetSend(cmd);
				if (m_formHelp->IsVisible())
				{
					m_formHelp->Hide();
				}
			}
			else
			{
				GetWorld()->OnNetUploadInteractiveNpc((DWORD)m_viewHelp->GetUserData(), szHref);
				if (g_layerMain->m_formHelp->IsVisible() && g_layerMain->m_formMiddleMap->GetFocusParentWidget(sender) == g_layerMain->m_formHelp)
				{
					g_layerMain->m_formHelp->Hide();
					if (g_layerMain->m_formMonInfo->GetActorID())
					{
						g_layerMain->m_formMonInfo->m_nActorID = 0;
					}
				}
			}
        }
    }else if (pEvent->type==weMouseClick && pItem->hyperLink.Type == hlinkClient)
    {
		if (g_layerMain->m_formHelp->IsVisible() && g_layerMain->m_formMiddleMap->GetFocusParentWidget(sender) == g_layerMain->m_formHelp)
		{
			g_layerMain->m_formHelp->Hide();
		}
		if (strncmp(szHref, "showItemInfo", 12) == 0)
		{
			ShowHyerLinkFromChat(szHref+13);
// 			long nItemID = atol(szHref+13);
// 			ItemHyperLinkSend(nItemID);
		}
		else if (strncmp(szHref, "banksetpass", 11) == 0)
		{
			g_layerMain->m_formBankPass->Show(GetWorld()->m_dwInteractiveNPCID, UIFormBankPass::sAddPass);
		}
		else if (strncmp(szHref, "bankreleasepass", 15) == 0)
		{
			g_layerMain->m_formBankPass->Show(GetWorld()->m_dwInteractiveNPCID, UIFormBankPass::sRemovePass);
		}
		else if (strncmp(szHref, "bankchangepass", 14) == 0)
		{
			g_layerMain->m_formBankPass->Show(GetWorld()->m_dwInteractiveNPCID, UIFormBankPass::sChangePass);
		}
		else if (strncmp(szHref, "bank", 4) == 0)
		{
			//---------add start by tony 05.10.11----------//
			if(GetWorld()->m_pPlayer->m_core.Lev < 10)
			{
				ShowSystemMessage(R(TYRO_LIMIT_BANK));
				return;
			}
			//---------add end   by tony 05.10.11----------//
			if (strncmp(szHref+5, "store", 5) == 0)			// store
				g_layerMain->m_formBank->Show(1, GetWorld()->m_dwInteractiveNPCID);
			else if (strncmp(szHref+5, "withdraw", 8) == 0)	// withdraw
				g_layerMain->m_formBank->Show(0, GetWorld()->m_dwInteractiveNPCID);
		}
		else if (strncmp(szHref, "PetBank", 7) == 0)
		{
			if(GetWorld()->m_pPlayer->m_core.Lev < 10)
			{
				ShowSystemMessage(R(TYRO_LIMIT_BANK));
				return;
			}
			g_layerMain->m_formPetBank->Show(GetWorld()->m_dwInteractiveNPCID);
		}
		else if (strncmp(szHref, "shop", 4) == 0)
		{
			if (strncmp(szHref+5, "buy", 3) == 0)
				g_layerMain->m_formShop->Show(true, GetWorld()->m_dwInteractiveNPCID);
			else if (strncmp(szHref+5, "sell", 4) == 0)
				g_layerMain->m_formShop->Show(true, GetWorld()->m_dwInteractiveNPCID);
		}
		else if (strncmp(szHref, "ItemUpdate", 10) == 0)
		{
//			g_layerMain->m_formItemUpdate->Show();

			RECT winRect;
			::GetClientRect((HWND)GetDevice()->GetWndHandle(),&winRect);
			RtwRect rect1 = g_layerMain->m_formItemUpdate->OnGetFormRect();
			RtwRect rect2 = g_layerMain->m_fromPlayerItems->OnGetFormRect();

			int x = (winRect.right - winRect.left - rect1.getWidth()-rect2.getWidth())*0.5 +winRect.left;
			int y = (winRect.bottom - winRect.top - rect1.getHeight())*0.5 +winRect.top;
			g_layerMain->m_formItemUpdate->SetPos(x, y);
			g_layerMain->m_formItemUpdate->Show();
			g_workspace.SetFocusWidget(g_layerMain->m_formItemUpdate->OnGetThisForm());
			g_layerMain->m_fromPlayerItems->SetPos(x+rect1.getWidth(), y);
			g_layerMain->m_fromPlayerItems->Show();
		}
		else if (strncmp(szHref, "Gem", 3) == 0)          //宝石合成
		{
			RECT winRect;
			::GetClientRect((HWND)GetDevice()->GetWndHandle(),&winRect);
			RtwRect rect1 = g_layerMain->m_formGem->OnGetFormRect();
			RtwRect rect2 = g_layerMain->m_fromPlayerItems->OnGetFormRect();

			int x = (winRect.right - winRect.left - rect1.getWidth()-rect2.getWidth())*0.5 +winRect.left;
			int y = (winRect.bottom - winRect.top - rect1.getHeight())*0.5 +winRect.top;
			g_layerMain->m_formGem->SetPos(x, y);
			g_layerMain->m_formGem->Show();
			g_workspace.SetFocusWidget(g_layerMain->m_formGem->OnGetThisForm());
			g_layerMain->m_fromPlayerItems->SetPos(x+rect1.getWidth(), y);
			g_layerMain->m_fromPlayerItems->Show();
		}
		else if (strncmp(szHref, "PetAbility", strlen(szHref)) == 0) 
		{
			m_formPetAbility->Show();
		}
		else if (strncmp(szHref, "PetMerge", strlen(szHref)) == 0)
		{
			m_formPetMerge->Show();
		}
		else if (strncmp(szHref, "PetToBeads", strlen(szHref)) == 0) 
		{
			m_formPetToBeads->Show();
		}
		else if (strncmp(szHref, "PetMergeBeads", strlen(szHref)) == 0)
		{
			m_formPetMergeBeads->Show();
		}
		else if (strncmp(szHref, "UnionCreate", 11) == 0)	// Create Union
		{
            if (GetWorld()->m_pPlayer->GetUnionID()!=0)
            {
                PlayerFailOperate(R(MSG_UNION_CREATE_FAIL_ALREADY_HAVE_UNION));
            }
            else 
            {
				if (GetWorld()->m_pPlayer->m_core.Lev < UNION_CREATE_USE_LEVEL)
				{
					//用户等级不够
					PlayerFailOperate(R(MSG_UNION_CREATE_UNMEASURE));
					return;
				}

				CItemContainer& con = GetWorld()->m_pPlayer->mItem;
				if (con.GetMoney() < UNION_CREATE_USE_MONEY)
				{
					//钱不够
					PlayerFailOperate(R(MSG_UNION_CREATE_UNMEASURE));
					return;
				}

                g_layerMain->m_formUnionCreate->Show(GetWorld()->m_dwInteractiveNPCID);
            }
		}
		//else if (strncmp(szHref, "UnionWar", 8) == 0)		// 向另一诸侯国宣战
		//{
		//}
		//else if (strncmp(szHref, "FeodBank", 8) == 0)		// 采邑国库
		//{
		//	char cTmp256[256];
		//	GcActor* pNpc = GetWorld()->m_ActorManager.FindActive(GetWorld()->m_dwInteractiveNPCID);
		//	if (!pNpc || !pNpc->m_pNpc || pNpc->m_pNpc->Home.empty())
		//		return;
		//	SFeod *pFeod = g_GcFeodMgr.Find(pNpc->m_pNpc->Home);
		//	if (!pFeod)
		//		return;

		//	if (GetWorld()->m_pPlayer->DBID() != g_layerMain->m_formUnion->m_unionData.captainID)
		//	{
		//		sprintf(cTmp256, R(MSG_UNION_RIGHTS_UNION_CAPTION), GetFeodName(pNpc->m_pNpc->Home).c_str());
		//		PlayerFailOperate(cTmp256);
		//	}
		//	else if (pFeod->unionID!=g_layerMain->m_formUnion->m_unionData.id)
		//	{
		//		sprintf(cTmp256, R(MSG_UNION_RIGHTS_UNION_CAPTION), GetFeodName(pNpc->m_pNpc->Home).c_str());
		//		PlayerFailOperate(cTmp256);
		//	}
		//	else 
		//	{
		//		g_layerMain->m_formFeodBank->Show(GetWorld()->m_dwInteractiveNPCID);
		//	}
		//}
		//else if (strncmp(szHref, "FeodTax", 7) == 0)		// 采邑税收
		//{
		//	char cTmp256[256];
		//	GcActor* pNpc = GetWorld()->m_ActorManager.FindActive(GetWorld()->m_dwInteractiveNPCID);
		//	if (!pNpc || !pNpc->m_pNpc || pNpc->m_pNpc->Home.empty())
		//		return;
		//	SFeod *pFeod = g_GcFeodMgr.Find(pNpc->m_pNpc->Home);
		//	if (!pFeod)
		//		return;

		//	if (GetWorld()->m_pPlayer->DBID() != g_layerMain->m_formUnion->m_unionData.captainID)
		//	{
		//		sprintf(cTmp256, R(MSG_UNION_RIGHTS_UNION_CAPTION), GetFeodName(pNpc->m_pNpc->Home).c_str());
		//		PlayerFailOperate(cTmp256);
		//	}
		//	else if (pFeod->unionID!=g_layerMain->m_formUnion->m_unionData.id)
		//	{
		//		sprintf(cTmp256, R(MSG_UNION_RIGHTS_UNION_CAPTION), GetFeodName(pNpc->m_pNpc->Home).c_str());
		//		PlayerFailOperate(cTmp256);
		//	}
		//	else 
		//	{
		//		g_layerMain->m_formFeodTax->Show(GetWorld()->m_dwInteractiveNPCID);
		//	}
		//}
		//else if (strncmp(szHref, "FeodList", 8) == 0)		// 采邑收支表
		//{
		//	char cTmp256[256];
		//	GcActor* pNpc = GetWorld()->m_ActorManager.FindActive(GetWorld()->m_dwInteractiveNPCID);
		//	if (!pNpc || !pNpc->m_pNpc || pNpc->m_pNpc->Home.empty())
		//		return;
		//	SFeod *pFeod = g_GcFeodMgr.Find(pNpc->m_pNpc->Home);
		//	if (!pFeod)
		//		return;

		//	if (GetWorld()->m_pPlayer->DBID() != g_layerMain->m_formUnion->m_unionData.captainID)
		//	{
		//		sprintf(cTmp256, R(MSG_UNION_RIGHTS_UNION_CAPTION), GetFeodName(pNpc->m_pNpc->Home).c_str());
		//		PlayerFailOperate(cTmp256);
		//	}
		//	else if (pFeod->unionID!=g_layerMain->m_formUnion->m_unionData.id)
		//	{
		//		sprintf(cTmp256, R(MSG_UNION_RIGHTS_UNION_CAPTION), GetFeodName(pNpc->m_pNpc->Home).c_str());
		//		PlayerFailOperate(cTmp256);
		//	}
		//	else 
		//	{
		//		g_layerMain->m_formFeodContent->Show(GetWorld()->m_dwInteractiveNPCID);
		//	}
		//}
		else if (strncmp(szHref, "SkillStudy", 10) == 0)    // 学习技能
        {
            g_layerMain->m_fromSkillStudy->Show();
		}
		else if (strncmp(szHref, "RelivePet", 9) == 0)
		{
			g_layerMain->m_formRecoverPet->SetVisible(true);
		}
		else if (strncmp(szHref, "moveto", 6) == 0 || strncmp(szHref, "fly", 3) == 0)
		{
			// teleport
			CG_CmdPacket *cmd = NetBeginWrite();
			cmd->WriteShort(c2r_teleport);
			GetWorld()->m_iTeleport = 1;
			cmd->WriteLong(GetWorld()->m_dwInteractiveNPCID);
			cmd->WriteString((char*)szHref);
			NetSend(cmd);
		}
		else if (strncmp(szHref, "fbmoveto", 8) == 0)
		{
			// request enter fb
			CG_CmdPacket *cmd = NetBeginWrite();
			cmd->WriteShort(c2r_enter_fb);
			cmd->WriteLong(GetWorld()->m_dwInteractiveNPCID);
			cmd->WriteString((char*)szHref);
			NetSend(cmd);
		}
		else if (strcmp(szHref,"leave_fb") == 0)
		{
			// request leave fb
			CG_CmdPacket *cmd = NetBeginWrite();
			cmd->WriteShort(c2r_leave_fb);
			cmd->WriteLong(GetWorld()->m_dwInteractiveNPCID);
			cmd->WriteString((char*)szHref);
			NetSend(cmd);
		}
//------------------add start by tony 05.06.06-------------------------------//
		else if (strncmp(szHref, "WarfieldEnter", 13) == 0)
		{
			char m_cFaction = 0;
			if (strncmp(szHref, "WarfieldEnterShang", 18) == 0)
			{
				m_cFaction = 1;
			} 
			else if(strncmp(szHref, "WarfieldEnterZhou", 17) == 0)
			{
				m_cFaction = 0;
			}
			else
			{
				m_cFaction = -1;
			}
			// request enter warfield
			CG_CmdPacket *cmd = NetBeginWrite();
			cmd->WriteShort(c2r_warfield_enter);
			cmd->WriteByte(m_cFaction);
			NetSend(cmd);
		}
		else if (strncmp(szHref, "WarfieldLeave", 13) == 0)
		{
			// request enter warfield
			CG_CmdPacket *cmd = NetBeginWrite();
			cmd->WriteShort(c2r_warfield_leave);
			NetSend(cmd);
		}
		else if(strncmp(szHref, "CastleWarReg", 12) == 0)
		{
			g_layerMain->m_formCastleWarReg->Show(GetWorld()->m_dwInteractiveNPCID);
		}
		else if(strncmp(szHref, "CastleWarSetTime", 16) == 0)
		{
			g_layerMain->m_formCastleWarTimeSet->Show(GetWorld()->m_dwInteractiveNPCID);
		}else if(strncmp(szHref, "GetFlag", 7) == 0)
		{
			CCastleWarNetHelper::NetSend_c2r_castlewar_apply_flag(GetWorld()->m_dwInteractiveNPCID);
		}
		else if(strncmp(szHref, "ViewSalesroom", 13) == 0)
		{
			g_layerMain->m_formVendor->Show();
			CAuctionNetHelper::NetSend_c2r_auction_view();
		}
		else if(strncmp(szHref, "GetAuctionItem", 14) == 0)
		{
			//g_layerMain->m_formVendor->Show();
			CAuctionNetHelper::NetSend_c2r_auction_get_item();
		}
//------------------add end   by tony 05.06.06-------------------------------//
		else if(strncmp(szHref, "ClearSP", 7) == 0)
		{
			g_layerMain->m_formClearSkill->SetVisible(true);
			CG_CmdPacket *cmd = NetBeginWrite();
			cmd->WriteShort(c2r_request_clear_skill);
			NetSend(cmd);
		}
		else if (strncmp(szHref, "GetNextGift", 11) == 0)
		{
			CG_CmdPacket *cmd = NetBeginWrite();
			cmd->WriteShort(c2r_get_next_gift);
			NetSend(cmd);
        }
		else if (strncmp(szHref, "ChargeCard", 10) == 0)
		{
            g_layerMain->m_formCard->SetUserData(GetWorld()->m_dwInteractiveNPCID, 1);
            //g_layerMain->m_formCard->Show("请输入充值卡号码：", "", "充值", false);//by fox for string
			g_layerMain->m_formCard->Show(R(MSG_INPUT_CARD_NUMBER), "", R(MSG_DEPOSITED), false);

		}
		else if (strncmp(szHref, "SellCard", 8) == 0)
		{
            g_layerMain->m_formSellCard->SetUserData(GetWorld()->m_dwInteractiveNPCID, 2);
            g_layerMain->m_formSellCard->Show();
		}
		else if (strncmp(szHref, "BuyCard", 7) == 0)
		{
			g_layerMain->m_formBuyCard->SetUserData(GetWorld()->m_dwInteractiveNPCID, 2);
            g_layerMain->m_formBuyCard->Show();
		}
		else if (strncmp(szHref, "MasterToBe", 10) == 0)
		{
			DWORD dwMyUserID = GetWorld()->m_pPlayer->DBID();
			DWORD dwOtherUserID = 0;
			CTeamData& teamdata = GetWorld()->m_Team.mData;
			if (teamdata.Size() != 2)
			{
				PlayerFailOperate(R(MSG_PRENSYS_ADD_TEAM_CONDITION));
				return;
			}
			if (teamdata.GetCaptain() != dwMyUserID)
			{
				PlayerFailOperate(R(MSG_PRENSYS_ADD_TEAM_CONDITION));
				return;
			}
			int iTmp = teamdata.Find(dwMyUserID);
			if (iTmp==0)
				dwOtherUserID = teamdata[1].mDBID;
			else 
				dwOtherUserID = teamdata[0].mDBID;
			if (!g_GcPrenticeMgr.Judge_AddMasterPrentice(GetWorld()->m_pPlayer))
			{
				PlayerFailOperate(R(MSG_PRENSYS_ADD_CONDITION));
				return;
			}
			GcPrenticeCmd::NetSend_c2r_prentice_add_apply(dwOtherUserID);
		}
		else if (strncmp(szHref, "MasterFinish", 12) == 0)
		{
			DWORD dwMyUserID = GetWorld()->m_pPlayer->DBID();
			DWORD dwOtherUserID = 0;
			CTeamData& teamdata = GetWorld()->m_Team.mData;
			if (teamdata.Size() != 2)
			{
				PlayerFailOperate(R(MSG_PRENSYS_FINISH_TEAM_CONDITION));
				return;
			}
			if (teamdata.GetCaptain() != dwMyUserID)
			{
				PlayerFailOperate(R(MSG_PRENSYS_FINISH_TEAM_CONDITION));
				return;
			}
			int iTmp = teamdata.Find(dwMyUserID);
			if (iTmp==0)
				dwOtherUserID = teamdata[1].mDBID;
			else 
				dwOtherUserID = teamdata[0].mDBID;
			if (!g_GcPrenticeMgr.Judge_FinishMasterPrentice(GetWorld()->m_pPlayer))
			{
				PlayerFailOperate(R(MSG_PRENSYS_FINISH_CONDITION));
				return;
			}
			if (GetWorld()->m_pPlayer->mItem.m_Bag.IsFull())
			{
				PlayerFailOperate(R(MSG_PRENSYS_ITEMBAG_FULL));
				return;
			}
			GcPrenticeCmd::NetSend_c2r_prentice_finish_apply(dwOtherUserID);
		}
		else if (strncmp(szHref, "MasterLeave", 11) == 0)
		{
			UIFormTextMsg *pFrm = UIFormTextMsg::ShowStatic(R(MSG_PRENSYS_REMOVE), R(MSG_PRENSYS_INPUT_PRENTICE_NAME));
			g_GcPrenticeMgr.m_pFrmRemove = pFrm;
			pFrm->EvOK = RTW_CALLBACK_1(&g_GcPrenticeMgr, GcPrenticeMgr, cbOnMasterRemove, pFrm);
		}
		else if (strncmp(szHref, "PrenticeLeave", 13) == 0)
		{
			UIFormTextMsg *pFrm = UIFormTextMsg::ShowStatic(R(MSG_PRENSYS_REMOVE), R(MSG_PRENSYS_INPUT_MASTER_NAME));
			g_GcPrenticeMgr.m_pFrmRemove = pFrm;
			pFrm->EvOK = RTW_CALLBACK_1(&g_GcPrenticeMgr, GcPrenticeMgr, cbOnPrenticeRemove, pFrm);
		}
		else if (strncmp(szHref, "get1gift", 8) == 0)
		{
			if (!g_GcPrenticeMgr.Judge_WithdrawMPGift(0))
			{
				return;
			}
			GcPrenticeCmd::NetSend_c2r_prentice_gift(0);
		}
		else if (strncmp(szHref, "get2gift", 8) == 0)
		{
			if (!g_GcPrenticeMgr.Judge_WithdrawMPGift(1))
			{
				return;
			}
			GcPrenticeCmd::NetSend_c2r_prentice_gift(1);
		}
		else if (strncmp(szHref, "Check_View", 10) == 0)
		{
			CG_CmdPacket *cmd = NetBeginWrite();
			cmd->WriteShort(c2r_exp_card);
			cmd->WriteString("view");
			NetSend(cmd);
		}
		else if (strncmp(szHref, "Check_Begin", 11) == 0)
		{
			CG_CmdPacket *cmd = NetBeginWrite();
			cmd->WriteShort(c2r_exp_card);
			cmd->WriteString("begin");
			NetSend(cmd);
		}
		else if (strncmp(szHref, "Check_End", 9) == 0)
		{
			CG_CmdPacket *cmd = NetBeginWrite();
			cmd->WriteShort(c2r_exp_card);
			cmd->WriteString("end");
			NetSend(cmd);
		}
		else if (strncmp(szHref, "Master_Regist", 13) == 0)
		{
			CG_CmdPacket* pPacket = NetBeginWrite();
			pPacket->WriteShort(c2r_master_list);
			pPacket->WriteByte(C2R_APPLY_ADD_LIST);
			NetSend(pPacket);
		}
		else if (strncmp(szHref, "Regist_View", 11) == 0)
		{
			CG_CmdPacket* pPacket = NetBeginWrite();
			pPacket->WriteShort(c2r_master_list);
			pPacket->WriteByte(C2R_APPLY_VIEW_LIST);
			NetSend(pPacket);
		}
        else if (strncmp(szHref, "Pet_Eat", 7) == 0)
        {
            g_layerMain->m_formPetEat->Show();
        }
		//-------add start by tony 06.06.20------------------------------//
		//-添加跑环挂起功能
		else if (strncmp(szHref, "Save_Circle", 11) == 0)
		{
			CTaskInfo *task = &GetWorld()->m_pPlayer->m_task;

			if(task->m_isActive==0)
			{
				ShowSystemMessage(R(MSG_CIRCLE_SAVED));
				return;
			}

			list<STaskInfo>::iterator it;
			for(it=task->m_taskList.begin(); it!=task->m_taskList.end(); it++)
			{
				if(it->IsDoing == 0) continue;
				if(it->IsDoing == 2) continue;
				STaskDesc *desc = g_TableTask.FindTask((*it).Id);
				if(!desc) continue;
				if(desc->circleTask == 0) continue;

				RtString msg;
				int leftTime = desc->finishTimeLimit - (GetWorld()->m_NetSynachro.GetServerSec() - it->BeginTime);
				if(leftTime<0 ) leftTime = 0;
				msg.Format(desc->dynamicText[0].key.c_str(),
					task->m_currCircle,g_tcTotalCircle ,
					leftTime/3600/10,(leftTime/3600)%10, (leftTime%3600)/60/10,(leftTime%3600)/60%10,
					((leftTime%3600)%60)/10,((leftTime%3600)%60)%10	);

				msg += R(MSG_CIRCLE_SAVE_ASK);
				UIFormMsg* pFrm = UIFormMsg::ShowStatic(msg, UIFormMsg::TYPE_OK_CANCEL);

				pFrm->EvOK = RTW_CALLBACK_F(OnSaveCircle);
				break;				
			}
						
			if(it==task->m_taskList.end())
			{
				ShowSystemMessage(R(MSG_CIRCLE_NONE));
			}			
		}
		else if (strncmp(szHref, "Active_Circle", 13) == 0)
		{
			CTaskInfo *task = &GetWorld()->m_pPlayer->m_task;

			if(task->m_isActive==1)
			{
				ShowSystemMessage(R(MSG_CIRCLE_ACTIVE));
				return;
			}

			list<STaskInfo>::iterator it;
			for(it=task->m_taskList.begin(); it!=task->m_taskList.end(); it++)
			{
				if(it->IsDoing == 0) continue;
				if(it->IsDoing == 2) continue;
				STaskDesc *desc = g_TableTask.FindTask((*it).Id);
				if(!desc) continue;
				if(desc->circleTask == 0) continue;

				RtString msg = R(MSG_CIRCLE_ACTIVE_ASK);

				UIFormMsg* pFrm = UIFormMsg::ShowStatic(msg, UIFormMsg::TYPE_OK_CANCEL);

				pFrm->EvOK = RTW_CALLBACK_F(OnActiveCircle);
				break;				
			}

			if(it==task->m_taskList.end())
			{
				ShowSystemMessage(R(MSG_CIRCLE_NONE));
			}			
		}
		else if (strncmp(szHref, "clientmoveto", 12) == 0 || strncmp(szHref, "clientmovetonpc", 15) == 0)//tim.yang  自动
		{
			ClientMoveto(szHref);
		}
		else if (strncmp(szHref, "selectreward", 12) == 0)
		{
			mSelectRewardId = 0;
			sscanf(&szHref[12], "%d", &mSelectRewardId);
		}
		//-------add end by tony 06.06.20------------------------------//

		/*
		else if (strncmp(szHref, "viewsreward",11) == 0)
		{
			// 察看选择奖励
			long taskId;
			long reward[5];
			for(int i=0; i<5; ++i)
				reward[i] = 0;
			sscanf(&szHref[11],"%d,%d,%d,%d,%d,%d",taskId,reward[0],reward[1],reward[2],
				reward[3],reward[4]);
			
			vector<DWORD> rw;
			for(int i=0; i<5; ++i)
				if(reward[i]!=0) rw.push_back(reward[i]); 
			
			m_formSelectReward->ResetRewardIDs(true,taskId,rw);
			m_formSelectReward->Show();
		}
		*/

		// 如果在战斗，就让战斗停止
		if (GetWorld() && GetWorld()->m_pPlayer)
			GetWorld()->m_pPlayer->WantStopAttack();

// 		m_formHelp->Hide();
	}else if (pEvent->type==weMouseClick && pItem->hyperLink.Type == hlinkNpc)
	{
		std::string strPath = m_viewHelp->GetPath();
		strPath.append(pItem->hyperLink.Text);
		const char* filename = strPath.c_str();
		GetWorld()->OnNetDownloadDialogHTML((DWORD)m_viewHelp->GetUserData(),filename,"","", 1, 0);
	}
   unguard;
}

void UILayerMain::OnClicked_PlayerItems(void*, void*)
{
	guard;

	if (!m_fromPlayerItems)
	{
		m_fromPlayerItems = RT_NEW UIFormPlayerItems;
		m_fromPlayerItems->SetPlayer( GetWorld()->m_pPlayer );
	}

	if ( m_fromPlayerItems->IsShowing() )
		m_fromPlayerItems->Hide();
	else
		m_fromPlayerItems->Show();

	unguard;
}

void UILayerMain::OnClicked_Union(void*, void*)
{
	guard;

	GcPlayer *pPlayer = GetWorld()->m_pPlayer;
 	if ( pPlayer && pPlayer->GetUnionID()!=0 )
	{
		if ( m_formUnion->IsVisible() )
			m_formUnion->Hide();
		else
		{
//  			CUnionNetHelper::NetSend_c2r_union_data_query( GetWorld()->m_pPlayer->GetUnionID() );
			m_formUnion->Show();
			m_formUnion->SetUnionButtonNormal();
		}
	}
 	else
 	{
 		PlayerFailOperate(R(MSG_UNION_HAVE_NO_UNION));
 	}

	unguard;
}

void UILayerMain::OnClicked_System_Skill(void*, void*)
{
    guard;
    if(m_fromSkill->IsShowing())
    {
        m_fromSkill->Hide();
    }else
    {
        m_fromSkill->Show();
    }
    unguard;
}

void UILayerMain::OnClicked_HeroList(void*, void*)
{
	guard;

	ShowSystemMessage(R(MSG_SYSTEM_NOTOPEN));
	//if (m_formHeroList->IsVisible())
	//	m_formHeroList->Hide();
	//else 
	//{
	//	m_formHeroList->Show();
	//	
	//}

	unguard;
}

	//heten
void UILayerMain::OnClicked_AutoFight(RtwWidget* sender, RtwEventDelegate* e)
{
	if (m_btnAutoBattle->GetChecked())
	{
		m_btnAutoBattle->SetChecked(false);
		GetWorld()->m_pPlayer->SetAutobattle(false);
	}
	else
	{
		m_btnAutoBattle->SetChecked(true);
		GetWorld()->m_pPlayer->SetAutobattle(true);
	}
}

void UILayerMain::OnClicked_ShopCenter(RtwWidget* sender, RtwEventDelegate* e)
{
	guard;
	return;
	if (m_formShopCenter->IsVisible())
	{
		m_formShopCenter->Hide();
	} 
	else
	{
		m_formShopCenter->Show();
	}
	//if (m_formShop->IsVisible())
	//{
	//	m_formShop->Hide();
	//}
	//else
	//{
	//	CItemNetHelper::NetSend_c2r_requstID();
	//}
	unguard;
}
//end
//void UILayerMain::SwitchHotKeyGroup()
//{
//    guard;
//	if (m_hotKeyGroupNumber == 0)
//	{
//		m_hotKeyGroupNumber = 1;
//		m_hotKeyPage->SetText("2");
//	}
//	else
//	{
//		m_hotKeyGroupNumber = 0;
//		m_hotKeyPage->SetText("1");
//	}
//
//	for (int n = 0; n < UI_HOT_KEY_COUNT; n++)
//	{
//		RtwButton::Ptr t = m_hotKeyAlias[n]->GetButton();
//		m_hotKeyAlias[n]->SetButton(m_hotKeyGroup[n]);	
//		m_hotKeyGroup[n] = t;
//	}
//    unguard;
//}
/*
bool UILayerMain::OnMemberChoice(DWORD vDBID,const char* vpName)
{
    guard;
	RtwWidget *pWidget = g_workspace.GetFocusWidget();
	if(pWidget==m_chatInput)
	{
		string str;
		str += "/";
		str += vpName;
		str += " ";
		m_chatInput->SetText(str);
	}
	return false;
    unguard;
}
*/

//void UILayerMain::SaveShortcut(string& str)
//{
//    guard;
//	strstream stream;
//
//	RtwButton* btn[2][UI_HOT_KEY_COUNT];
//
//	int b1;
//	int b2;
//
//	if (m_hotKeyGroupNumber == 0)
//	{
//		b1 = 0; b2 = 1;
//	}
//	else
//	{
//		b1 = 1; b2 = 0;
//	}
//	
//	for (int n = 0; n < UI_HOT_KEY_COUNT; n++)
//	{
//		btn[b1][n] = m_hotKeyAlias[n]->GetButton();
//		btn[b2][n] = m_hotKeyGroup[n];	
//	}
//
//	stream << 1; // version
//
//	string saveString;
//	
//	for (int group = 0; group < 2; group++)
//	{
//		stream << '{';
//
//		for (int index = 0; index < UI_HOT_KEY_COUNT; index++)
//		{
//			if (btn[group][index])
//			{
//				stream << index << ';';
//
//				const string& id = btn[group][index]->GetQualifiedID();
//				int i;
//				for (i= 0; i < g_storedIconCount; ++i)
//				{
//					if (id == g_storedIcon[i])
//					{
//						break;
//					}
//				}
//
//				if (i < g_storedIconCount)
//				{
//					stream << UIHotKey_Icon << '{' << i << '}';
//				}
//				else if (OnSaveSkillShortcut(btn[group][index], saveString))
//				{
//					stream << UIHotKey_Skill << '{' << saveString << '}';
//				}
//				else if (OnSaveItemShortcut(btn[group][index], saveString))
//				{
//					stream << UIHotKey_Item << '{' << saveString << '}';
//				}
//				else
//				{
//					stream << "{}";
//				}
//			}
//		}
//
//		stream << '}';
//	}
//
//	stream << ends;
//
//	str = stream.str();
//    unguard;
//}

void UILayerMain::SaveShortcut()
{
    guard;

    RtwButton* btn[UI_HOT_KEY_COUNT];
	FILE* file;
	int id1, id2;

	for (int n = 0; n < UI_HOT_KEY_COUNT; n++)
	{
		btn[n] = m_hotKeyAlias[n]->GetButton();
	}

	int num;
	char name[100];
	//ldr123
	//目录不存在会崩
	if (_access("usersetting", 0) != 0)
	{
		CreateDirectory("usersetting", NULL);
	}
	//end

	rt2_sprintf(name,"usersetting\\%s.dat",GetPlayer()->m_core.Name.c_str());
	file = fopen( name, "wb+" );
	num = UI_HOT_KEY_COUNT;
	fwrite(&num,1,sizeof(int),file);

	for (int index = 0; index < UI_HOT_KEY_COUNT; index++)
	{
		if (btn[index])
		{
			if (OnSaveSkillShortcut(btn[index], id1))
			{
				num = UIHotKey_Skill;
				fwrite(&num,1,sizeof(int),file);
				fwrite(&id1,1,sizeof(int),file);
			}else if (OnSaveItemShortcut(btn[index], id2, id1))
			{
				num = UIHotKey_Item;
				fwrite(&num,1,sizeof(int),file);
				fwrite(&id1,1,sizeof(int),file);
				fwrite(&id2,1,sizeof(int),file);
			}else
			{
				num = -1;
				fwrite(&num,1,sizeof(int),file);
			}
		}else
		{
			num = 0;
			fwrite(&num,1,sizeof(int),file);
		}
	}
	fclose(file);
    unguard;
}

//add by yz
void UILayerMain::SaveShortcutToNet(CG_CmdPacket* pPacket)
{
	guard;

	RtwButton* btn[UI_HOT_KEY_COUNT];
	int iID, iType;
	strstream stream;

	for (int n = 0; n < UI_HOT_KEY_COUNT; n++)
	{
		btn[n] = m_hotKeyAlias[n]->GetButton();
	}

	stream << 1 << ';'; //version
	for (int index = 0; index < UI_HOT_KEY_COUNT; index++)
	{
		if (btn[index])
		{
			if (OnSaveSkillShortcut(btn[index], iID))
			{
				stream << UIHotKey_Skill << ';' << iID << ';';
			}else if (OnSaveItemShortcut(btn[index], iType, iID))
			{
				stream << UIHotKey_Item << ';' << iID << ';' << iType << ';';
			}else
			{
				stream << -1 << ';';
			}
		}else
		{
			stream << 0 << ';';
		}
	}

	stream << ends;
	string str = stream.str();
	if (NULL != pPacket)
	{
		pPacket->WriteString((char*)str.c_str());
		NetSend(pPacket);
	}

	unguard;
}
//end

void UILayerMain::ClearShortcut()
{
    guard;
    for (int n = 0; n < UI_HOT_KEY_COUNT; n++)
    {
        m_hotKeyAlias[n]->SetButton(NULL);
        //if (m_hotKeyGroup[n])
        //{
        //    //m_hotKeyGroup[n]->Unref();
        //    m_hotKeyGroup[n] = NULL;
        //}
    }
#ifdef _DEBUG
	m_fromSkill->m_listCreateButton.clear();
#endif
    unguard;
}

void UILayerMain::LoadShortcut(CG_CmdPacket* pPacket)
{
    guard;
    ClearShortcut();	 
	FILE* file;
	char name[100];
	RtwButton* button = NULL;				
	char loadStr[30] = {0};
	rt2_sprintf(name,"usersetting\\%s.dat",GetPlayer()->m_core.Name.c_str());
	file = fopen(name,"rb+");
	if(!file)
	{
		//add by yz
		bool isHaveData = true;
		char* shortcut;
		pPacket->ReadString(&shortcut);
		string sShortcut = shortcut;
		string sub;
		parseString(sShortcut, sub);

		int version = atoi(sub.c_str());
		if (1 == version)
		{
			int iHotKeyType, iID, iType;
			for (int i = 0; i < UI_HOT_KEY_COUNT; i++)
			{				
				parseString(sShortcut, sub);
				iHotKeyType = atoi(sub.c_str());
				switch(iHotKeyType)
				{
				case 0:
					button = NULL;
					break;
				case UIHotKey_Item:
					parseString(sShortcut, sub);
					iID = atoi(sub.c_str());
					parseString(sShortcut, sub);
					iType = atoi(sub.c_str());
					rt2_snprintf(loadStr, 30, "%d;%d;%d", 1, iID, iType);
					button = OnLoadItemShortcut(loadStr);
					break;
				case UIHotKey_Skill:
					parseString(sShortcut, sub);
					iID = atoi(sub.c_str());
					rt2_snprintf(loadStr, 30, "%d;%d", 1, iID);
					button = OnLoadSkillShortcut(loadStr);
					break;
				default:
					button = NULL;
					break;
				}
				if(button)
				{
					m_hotKeyAlias[i]->SetButton(button);
				}
			}
		}
		else
		{
			isHaveData = false;
		}			
		//end

		if (NULL == pPacket || !isHaveData)
		{
			rt2_sprintf(loadStr,"%d;%d",1,111);
			button = OnLoadSkillShortcut(loadStr);
			m_hotKeyAlias[0]->SetButton(button);
			m_hotKeyAlias[0]->GetButton()->EvRClick += RTW_CALLBACK(this, UIFormSkill, OnLClickCommonButton);

			rt2_sprintf(loadStr,"%d;%d",1,113);
			button = OnLoadSkillShortcut(loadStr);
			m_hotKeyAlias[9]->SetButton(button);
			m_hotKeyAlias[9]->GetButton()->EvRClick += RTW_CALLBACK(this, UIFormSkill, OnLClickCommonButton);

			rt2_sprintf(loadStr,"%d;%d",1,112);
			button = OnLoadSkillShortcut(loadStr);
			m_hotKeyAlias[8]->SetButton(button);
			m_hotKeyAlias[8]->GetButton()->EvRClick += RTW_CALLBACK(this, UIFormSkill, OnLClickCommonButton);

			rt2_sprintf(loadStr,"%d;%d",1,107);
			button = OnLoadSkillShortcut(loadStr);
			m_hotKeyAlias[7]->SetButton(button);
			m_hotKeyAlias[7]->GetButton()->EvRClick += RTW_CALLBACK(this, UIFormSkill, OnLClickCommonButton);

			//sprintf(loadStr,"%d;%d",1,107);
			//button = OnLoadSkillShortcut(loadStr);
			//m_hotKeyAlias[6]->SetButton(button);
			//m_hotKeyAlias[6]->GetButton()->EvRClick += RTW_CALLBACK(this, UIFormSkill, OnLClickCommonButton);			
		}
		OnSetDefaultHotKeyState();
		return;		
	}
	int num,type,key1,key2;
	fread(&num,1,sizeof(int),file);
	if(num == UI_HOT_KEY_COUNT)
	{
		for(int i= 0;i< num;++i)
		{
			fread(&type,1,sizeof(int),file);
			switch(type)
			{
			case 0:
				button = NULL;
				break;
			case UIHotKey_Item:
				fread(&key1,1,sizeof(int),file);
				fread(&key2,1,sizeof(int),file);
				rt2_sprintf(loadStr,"%d;%d;%d",1,key1,key2);
				button = OnLoadItemShortcut(loadStr);
				break;
			case UIHotKey_Skill:
				fread(&key1,1,sizeof(int),file);
				rt2_sprintf(loadStr,"%d;%d",1,key1);
				button = OnLoadSkillShortcut(loadStr);
				break;
			default:
				button = NULL;
				break;
			}
			if(button)
			{
				m_hotKeyAlias[i]->SetButton(button);
			}
		}
	}
	OnSetDefaultHotKeyState();
	fclose(file);
	unguard;
}

bool UILayerMain::OnSaveSkillShortcut(RtwButton* button, string& str)
{
    guard;
	DWORD version = 1;
	DWORD *pDWord = (DWORD *)button->GetUserData();
	if (pDWord && pDWord[0] == UIHotKey_Skill)
	{
		char cTmp128[128];
		rt2_sprintf(cTmp128, "%d;%d", version, pDWord[1]);
		str = cTmp128;
		return true;
	}
	else 
	{
		return false;
	}
    unguard;
}

bool UILayerMain::OnSaveSkillShortcut(RtwButton* button, int& iSkillID)
{
    guard;
	DWORD version = 1;
	DWORD *pDWord = (DWORD *)button->GetUserData();
	if (pDWord && pDWord[0] == UIHotKey_Skill)
	{
        iSkillID = pDWord[1];
		return true;
	}
	else 
	{
		return false;
	}
    unguard;
}

bool UILayerMain::OnSaveItemShortcut(RtwButton* button, string& str)
{
    guard;
	DWORD version = 1;
	DWORD *pDWord = (DWORD *)button->GetUserData();
	if (pDWord && pDWord[0] == UIHotKey_Item)
	{
		char cTmp128[128];
		rt2_sprintf(cTmp128, "%d;%d;%d", version, pDWord[1], pDWord[2]);
		str = cTmp128;
		return true;
	}
	else 
	{
		return false;
	}
    unguard;
}

bool UILayerMain::OnSaveItemShortcut(RtwButton* button, int &iItemType, int &iItemID)
{
    guard;
	DWORD version = 1;
	DWORD *pDWord = (DWORD *)button->GetUserData();
	if (pDWord && pDWord[0] == UIHotKey_Item)
	{
        iItemID = pDWord[1];
        iItemType = pDWord[2];
		return true;
	}
	else 
	{
		return false;
	}
    unguard;
}

RtwButton* UILayerMain::OnLoadSkillShortcut(const char* str)
{
    guard;
	CHECK(m_fromSkill);
	CHECK(GetWorld()->m_pPlayer);

	if (*str == NULL)
		return NULL;

	RtwButton* pButton = NULL;
	DWORD version;
	DWORD skillID;
	sscanf(str, "%d;%d", &version, &skillID);
	if (version==1)
	{
		if (skillID==0)
		{
			ERR("Invalid Skill Shortcut\n");
			return NULL;
		}

        SSkill* pAttr = Skill()->FindSkill(skillID);
        if (pAttr==NULL) return NULL;
		GcSkill* pSkill = (GcSkill*)GetWorld()->m_pPlayer->m_Skill.FindActiveSkillBySubID(pAttr->wSubID);
		if (pSkill==NULL) return NULL;
        pAttr = pSkill->m_pAttr;
		char strId[32];
		RtwButton* pButton = NULL;
		rt2_sprintf(strId, "CommonSkillButton_%03d", pAttr->wID);
		pButton = (RtwButton*)(LOAD_UI("fmborder_skill.fmskillall")->GetChildWidget(strId));
		if (pButton==NULL)
		{
// change [3/18/2009 tooth.shi]			pButton = (RtwButton*)g_workspace.CreateWidget(NULL, strId, "Button",RtwButton::wfDefault | wfHint | wfDragClient | wfDragServer);
			pButton = (RtwButton*)g_workspace.getWidgetFactory()->createWidget(wtButton, strId);
			pButton->ModifyFlags(wfDragOut|wfDragIn|wfHint|wfDClick|wfGrabMouse, 0);

			pButton->LoadAttributesFromTemplate("IconBorder");
// change [3/18/2009 tooth.shi]			pButton->SetBackgroundImage(new RtwImage(pSkill->m_pAttr->szIcon));
			pButton->SetBackgroundImage(g_workspace.getImageFactory()->createImage(pSkill->m_pAttr->szIcon));
			pButton->Resize(GRID_SIZE_X, GRID_SIZE_Y);

			DWORD *pDWord = RT_NEW DWORD[2];
			pDWord[0] = UIHotKey_Skill;
			pDWord[1] = pSkill->m_pAttr->wID;
			pButton->SetUserData(pDWord);

			pButton->EvClose       += RTW_CALLBACK(m_fromSkill, UIFormSkill, OnSkillItemButtonClose);
			//pButton->EvLClick      += RTW_CALLBACK(m_fromSkill, UIFormSkill, OnLClickCommonButton);
 			pButton->EvHint		   += RTW_CALLBACK(m_fromSkill, UIFormSkill, OnGetHintTextCommonButton);
			pButton->EvRClick	   += RTW_CALLBACK(m_fromSkill, UIFormSkill, OnLClickCommonButton);
			
			
			pSkill->SetUIButton(pButton);
#ifdef _DEBUG
			m_fromSkill->m_listCreateButton.push_back(pButton);
#endif
        }

		return pButton;
	}

	return NULL;
    unguard;
}

RtwButton* UILayerMain::OnLoadItemShortcut(const char* str)
{
    guard;


	CHECK(m_fromPlayerItems && GetWorld());

	if (*str == NULL)
		return NULL;

	RtwButton* pButton = NULL;
	DWORD version;
	DWORD id, type;
	sscanf(str, "%d;%d;%d", &version, &id, &type);

	if (version==1)
	{
		if (id==0 || type==0)
		{
			ERR("Invalid Item Shortcut\n");
			return NULL;
		}

// change [3/18/2009 tooth.shi]		pButton = (RtwButton*)g_workspace.CreateWidget(NULL, str, "Button",	RtwButton::wfDefault | wfHint | wfDragClient | wfDragServer);
		pButton = (RtwButton*)g_workspace.getWidgetFactory()->createWidget(wtButton, str);
		pButton->ModifyFlags(wfDragOut|wfDragIn|wfHint|wfDClick|wfGrabMouse, 0);
		pButton->LoadAttributesFromTemplate("IconBorder");
		pButton->Resize(GRID_SIZE_X, GRID_SIZE_Y);

		SItemBase* pItemInstance = GetWorld()->m_pItemManager->GetItem(type);
		if (pItemInstance==NULL)
		{
			return NULL;
		}

// change [3/18/2009 tooth.shi]		RtwImage* pImage = new RtwImage(pItemInstance->icon);
		RtwImage* pImage = g_workspace.getImageFactory()->createImage(pItemInstance->icon);
		pButton->SetBackgroundImage(pImage);

		DWORD *pDword = RT_NEW DWORD[3];
		pDword[0] = UIHotKey_Item;
		int itemCount = 0;
        if (ItemIsMissionItem(type))
        {
            CBag *pBag = &(GetWorld()->m_pPlayer->mItem.m_MissionBag);
            int page, gridI, gridJ;
            if ( !pBag->Find(id, &page, &gridI, &gridJ) )
                return NULL;

            SItemID item = pBag->GetItem(page, gridI, gridJ);
            if (!ItemID_IsValid(item))
                return NULL;
            itemCount = item.count;
            pDword[1] = id;
            pDword[2] = type;
// 注释 [3/18/2009 tooth.shi]            pButton->SetTextFormat(RtwStringFormat(alignFar, alignFar));
            pButton->SetText(rtFormatNumber(itemCount));
			pButton->SetTextAlignment(SUiTextAlignment(alignFar, alignFar));
        }
		else
		{
            if (ItemCanPile(type))
            {
                int p1st, i1st, j1st;
                if ( !GetWorld()->m_pPlayer->mItem.m_Bag.FindType( GetWorld()->m_pItemManager->GetIndexFromType(type), ItemColor_All, &itemCount, &p1st, &i1st, &j1st) )
                    return NULL;

                SItemID item1st = GetWorld()->m_pPlayer->mItem.m_Bag.GetItem(p1st, i1st, j1st);
                pDword[1] = item1st.id;
                pDword[2] = item1st.type;
 // 注释 [3/18/2009 tooth.shi]               pButton->SetTextFormat(RtwStringFormat(alignFar, alignFar));
                pButton->SetText(rtFormatNumber(itemCount));
				pButton->SetTextAlignment(SUiTextAlignment(alignFar, alignFar));
            }
            else 
            {
			    // 如果没找到道具，就删掉此图标
			    CItemContainerBase::EEquipParts part;
			    if ( !GetWorld()->m_pPlayer->mItem.m_Bag.Find(id) )
				    if ( !GetWorld()->m_pPlayer->mItem.FindInEquipItems(id, part) )
					    return NULL;

			    pDword[1] = id;
			    pDword[2] = type;
            }
		}

		pButton->SetUserData(pDword);
		pButton->EvClose		+= RTW_CALLBACK(m_fromPlayerItems, UIFormPlayerItems, DeleteQuickStartUserData);
		//pButton->EvLClick		+= RTW_CALLBACK(m_fromPlayerItems, UIFormPlayerItems, OnLClick_QuickStart);
		pButton->EvRClick		+= RTW_CALLBACK(m_fromPlayerItems, UIFormPlayerItems, OnLClick_QuickStart);
		pButton->EvHint			+= RTW_CALLBACK(m_fromPlayerItems, UIFormPlayerItems, OnGetHintText_QuickStart);

		return pButton;
	}

	return NULL;
    unguard;
}


void UILayerMain::AddRemindIcon(UIRemindType type, const char* hint/* = ""*/, const char* icon/* = ""*/, const char* sound/* = ""*/)
{
    guard;
	SRemindIcon ri;

	ri.type = type;
	ri.sound = "ui_notify.wav";

	if (type==UIRemind_NewMail)
	{
		GetWorld()->m_Mail.SetMailButtonAnimation();
		ri.sound = "ui_getmail.wav";
	}
	else if (type==UIRemind_LevelUp)
	{
		g_layerMain->m_formChar->SetCharButtonAnimation();
		g_layerMain->m_fromMiniMap->OnRefreshCanAcceptTask();
		ri.sound = "eff_levelup.wav";
	}
	else if (type==UIRemind_GmResponse)
	{
		ri.hint = R(MSG_UI_GM_REPLY);
		ri.icon = R(MSG_UI_DEFAULT_IMAGE);
		ri.sound = "ui_tips.wav";
	}
	else 
	{
		return;
	}

	RtgVertex3 pos;
	g_pSoundMgr->PlayOnce(ri.sound.c_str(), false, 0, pos);
    unguard;
}

//void UILayerMain::RefreshRemindIcons()
//{
//    guard;
//	int listSize = (int)m_lstRemindIcon.size();
//	std::list<SRemindIcon>::iterator iter = m_lstRemindIcon.begin();
//
//	// move to right position
//	int nHeight = GetWorld()->m_Chat.GetCurrentHeight();
//	for (int i=0; i<UI_BTN_REMIND_COUNT; ++i)
//	{
//		m_btnRemind[i]->Move(10, nHeight-(3-i)*40);
//	}
//
//	// show/hide button
//	int nStart = 0;
//	if (listSize < UI_BTN_REMIND_COUNT)
//		nStart = UI_BTN_REMIND_COUNT - listSize;
//
//	for (int i=0; i<nStart; ++i)
//	{
//		m_btnRemind[i]->Hide();
//	}
//	for (int i=nStart; i<UI_BTN_REMIND_COUNT; ++i)
//	{
//		m_btnRemind[i]->SetHintText((*iter).hint);
//		if (!(*iter).icon.empty())
//		{
//			RtwImage *pImage = g_workspace.getImageFactory()->createImage((*iter).icon);
//			m_btnRemind[i]->SetNormalImage(pImage);
//			m_btnRemind[i]->SetHoverImage(pImage);
// 			//m_btnRemind[i]->SetPressedImage(pImage);
// 			//m_btnRemind[i]->SetFocusImage(pImage);
//		}
//		m_btnRemind[i]->Show();
//
//		iter++;
//	}
//    unguard;
//}

//void UILayerMain::OnClicked_RemindIcon(RtwWidget* sender, void*)
//{
//    guard;
//	std::list<SRemindIcon>::iterator iter = m_lstRemindIcon.begin();
//
//	if (sender==m_btnRemind[0])
//	{
//		CHECK(m_lstRemindIcon.size()>=3);
//	}
//	else if (sender==m_btnRemind[1])
//	{
//		CHECK(m_lstRemindIcon.size()>=2);
//
//		if (m_lstRemindIcon.size()>2)
//			iter++;
//	}
//	else if (sender==m_btnRemind[2])
//	{
//		CHECK(m_lstRemindIcon.size()>=1);
//
//		if (m_lstRemindIcon.size()==2)
//			iter++;
//		else if (m_lstRemindIcon.size()>=3)
//		{
//			iter++;
//			iter++;
//		}
//	}
//
//	switch ((*iter).type)
//	{
//	case UIRemind_NewMail:
//		if (!GetWorld()->m_Mail.IsMailUIVisible())
//		{
//			RtgVertex3 pos;
//			g_pSoundMgr->PlayOnce("ui_tips.wav", false, 0, pos);
//			GetWorld()->m_Mail.OpenMailUI(NULL, NULL);
//		}
//		break;
//
//	case UIRemind_LevelUp:
//		if (!m_formChar->IsVisible())
//		{
//			m_formChar->Show();
//		}
//		break;
//
//	case UIRemind_GmResponse:
//		if (!g_layerMain->m_formGm->IsVisible())
//		{
//			g_layerMain->m_formGm->Show();
//		}
//		break;
//
//	default:
//		break;
//	}
//
//	m_lstRemindIcon.erase(iter);
//	RefreshRemindIcons();
//    unguard;
//}

//change by yz
void UILayerMain::OnClicked_AutoBattle(RtwWidget* sender, RtwEventDelegate* e)			//ac.ma
{
	guard;
	if(m_btnAutoBattle->GetChecked())
	{
		GetWorld()->m_Chat.SystemMessage(R(MSG_TOOLON), false);
		GetWorld()->m_pPlayer->SetAutobattle(true);
	}
	else 
	{
		GetWorld()->m_Chat.SystemMessage(R(MSG_TOOLOFF), false);
		GetWorld()->m_pPlayer->SetAutobattle(false);
	}
	unguard;
}
//end

void UILayerMain::OnClicked_AutoPick(RtwWidget* sender, RtwEventDelegate* e)			//ac.ma
{
	guard;

	if(m_btnAutoPick->GetChecked())
	{		
		GetWorld()->m_pPlayer->SetAutoPick(true);
	}
	else 
	{
		GetWorld()->m_pPlayer->SetAutoPick(false);
	}
	unguard;
}

void UILayerMain::OnClicked_AutoEatRed(RtwWidget* sender, RtwEventDelegate* e)			//ac.ma
{
	guard;

	if(m_btnAutoEatRed->GetChecked())
	{		
		GetWorld()->m_pPlayer->SetAutoEatRed(true);
	}
	else 
	{
		GetWorld()->m_pPlayer->SetAutoEatRed(false);
	}
	unguard;
}

void UILayerMain::OnClicked_AutoEatBlue(RtwWidget* sender, RtwEventDelegate* e)			//ac.ma
{
	guard;

	if(m_btnAutoEatBlue->GetChecked())
	{		
		GetWorld()->m_pPlayer->SetAutoEatBlue(true);
	}
	else 
	{
		GetWorld()->m_pPlayer->SetAutoEatBlue(false);
	}
	unguard;
}

void UILayerMain::OnClicked_AutoHp30(RtwWidget* sender, RtwEventDelegate* e)			//ac.ma
{
	guard;

	if(m_btnAutoHp30->GetChecked())
	{		
		GetWorld()->m_pPlayer->SetAutoHp30(true);
		if (m_btnAutoHp90->GetChecked())
		{
			GetWorld()->m_pPlayer->SetAutoHp90(false);
			m_btnAutoHp90->SetChecked(false);
		}else
			if (m_btnAutoHp60->GetChecked())
			{
				GetWorld()->m_pPlayer->SetAutoHp60(false);
				m_btnAutoHp60->SetChecked(false);
			}
	}
	else 
	{
		GetWorld()->m_pPlayer->SetAutoHp30(false);
	}
	unguard;
}

void UILayerMain::OnClicked_AutoHp60(RtwWidget* sender, RtwEventDelegate* e)			//ac.ma
{
	guard;

	if(m_btnAutoHp60->GetChecked())
	{		
		GetWorld()->m_pPlayer->SetAutoHp60(true);
		if (m_btnAutoHp30->GetChecked())
		{
			GetWorld()->m_pPlayer->SetAutoHp30(false);
			m_btnAutoHp30->SetChecked(false);
		}else
			if (m_btnAutoHp90->GetChecked())
			{
				GetWorld()->m_pPlayer->SetAutoHp90(false);
				m_btnAutoHp90->SetChecked(false);
			}
	}
	else 
	{
		GetWorld()->m_pPlayer->SetAutoHp60(false);
	}
	unguard;
}

void UILayerMain::OnClicked_AutoHp90(RtwWidget* sender, RtwEventDelegate* e)			//ac.ma
{
	guard;

	if(m_btnAutoHp90->GetChecked())
	{		
		GetWorld()->m_pPlayer->SetAutoHp90(true);
		if (m_btnAutoHp30->GetChecked())
		{
			GetWorld()->m_pPlayer->SetAutoHp30(false);
			m_btnAutoHp30->SetChecked(false);
		}else
			if (m_btnAutoHp60->GetChecked())
			{
				GetWorld()->m_pPlayer->SetAutoHp60(false);
				m_btnAutoHp60->SetChecked(false);
			}
	}
	else 
	{
		GetWorld()->m_pPlayer->SetAutoHp90(false);
	}
	unguard;
}

void UILayerMain::OnClicked_AutoSellWhite(RtwWidget* sender, RtwEventDelegate* e)
{
	guard;

	if(m_btnAutoSellWhite->GetChecked())
	{		
		GetWorld()->m_pPlayer->SetAutoSellWhite(true);
	}
	else 
	{
		GetWorld()->m_pPlayer->SetAutoSellWhite(false);
	}
	unguard;
}

void UILayerMain::OnClicked_AutoSellGreen(RtwWidget* sender, RtwEventDelegate* e)
{
	guard;

	if(m_btnAutoSellGreen->GetChecked())
	{		
		GetWorld()->m_pPlayer->SetAutoSellGreen(true);
	}
	else 
	{
		GetWorld()->m_pPlayer->SetAutoSellGreen(false);
	}
	unguard;
}
void UILayerMain::OnClicked_AutoSellBlue(RtwWidget* sender, RtwEventDelegate* e)
{
	guard;

	if(m_btnAutoSellBlue->GetChecked())
	{		
		GetWorld()->m_pPlayer->SetAutoSellBlue(true);
	}
	else 
	{
		GetWorld()->m_pPlayer->SetAutoSellBlue(false);
	}
	unguard;
}

void UILayerMain::OnClicked_AutoSellGold(RtwWidget* sender, RtwEventDelegate* e)
{
	guard;

	if(m_btnAutoSellGold->GetChecked())
	{		
		GetWorld()->m_pPlayer->SetAutoSellGold(true);
	}
	else 
	{
		GetWorld()->m_pPlayer->SetAutoSellGold(false);
	}
	unguard;
}

void UILayerMain::OnClicked_AcceptTask(RtwWidget* sender, RtwEventDelegate* e)
{
	m_dwInteractiveNPCID = GetWorld()->GetInteractiveNpcId();
	m_upTaskId = GetWorld()->GetUploadTaskId();
	CG_CmdPacket *cmd = NetBeginWrite();
	cmd->WriteShort(c2r_upload_taskid);
	cmd->WriteLong(m_dwInteractiveNPCID);
	cmd->WriteLong(m_upTaskId);
	NetSend(cmd);
	if (m_formHelp2->IsVisible())
	{
		m_formHelp2->Hide();
	}
	if (m_formGuide3->IsVisible())
	{
		m_formGuide3->Hide();
	}
}

void UILayerMain::OnClicked_CancelTask(RtwWidget* sender, RtwEventDelegate* e)
{
	if (m_formHelp2->IsVisible())
	{
		m_formHelp2->Hide();
	}
}

void UILayerMain::OnClicked_FinishTask(RtwWidget* sender, RtwEventDelegate* e)
{
	m_dwInteractiveNPCID = GetWorld()->GetInteractiveNpcId();
	m_upTaskId = GetWorld()->GetUploadTaskId();
	CG_CmdPacket *cmd = NetBeginWrite();
	cmd->WriteShort(c2r_upload_finish_taskid);
	cmd->WriteLong(m_dwInteractiveNPCID);
	cmd->WriteLong(m_upTaskId);
	cmd->WriteLong(mSelectRewardId);
	NetSend(cmd);
	if (m_formHelp3->IsVisible())
	{
		GetWorld()->HideGuideForm();
		m_formHelp3->Hide();
	}
}

void UILayerMain::OnClicked_CancelFinishTask(RtwWidget* sender, RtwEventDelegate* e)
{
	if (m_formHelp3->IsVisible())
	{
		m_formHelp3->Hide();
	}
}
//接受主线任务的响应函数
void UILayerMain::OnClicked_AcceptMainTask(RtwWidget* sender, RtwEventDelegate* e)
{
	m_dwInteractiveNPCID = GetWorld()->GetInteractiveNpcId();
	m_upTaskId = GetWorld()->GetUploadTaskId();
	CG_CmdPacket *cmd = NetBeginWrite();
	cmd->WriteShort(c2r_upload_taskid);
	cmd->WriteLong(m_dwInteractiveNPCID);
	cmd->WriteLong(m_upTaskId);
	NetSend(cmd);
	if(g_layerMain->m_formBtn1->IsVisible()) 
	{
		g_layerMain->m_formBtn1->Hide();
	}
	if (m_formMainLineTask->IsVisible())
	{
		m_formMainLineTask->Hide();
	}
}

void UILayerMain::OnClicked_CancelMainTask(RtwWidget* sender, RtwEventDelegate* e)
{
	if(g_layerMain->m_formBtn1->IsVisible()) 
	{
		g_layerMain->m_formBtn1->Hide();
	}
	if (m_formMainLineTask->IsVisible())
	{
		m_formMainLineTask->Hide();
	}
}

void UILayerMain::OnClicked_FinishMainTask(RtwWidget* sender, RtwEventDelegate* e)
{
	m_dwInteractiveNPCID = GetWorld()->GetInteractiveNpcId();
	m_upTaskId = GetWorld()->GetUploadTaskId();
	STaskDesc *desc = g_TableTask.FindTask(m_upTaskId);
	if(!desc) return;
	if (!desc->selectReward.empty())
	{
		if (!mSelectRewardId)
		{
			ShowMessage(R(MSG_TASK_SELECTPRICEFIRST));
			return;
		}
	}

	CG_CmdPacket *cmd = NetBeginWrite();
	cmd->WriteShort(c2r_upload_finish_taskid);
	cmd->WriteLong(m_dwInteractiveNPCID);
	cmd->WriteLong(m_upTaskId);
	cmd->WriteLong(mSelectRewardId);
	NetSend(cmd);
	if(g_layerMain->m_formBtn2->IsVisible()) 
	{
		g_layerMain->m_formBtn2->Hide();
	}
	if (m_formMainLineTask->IsVisible())
	{
		m_formMainLineTask->Hide();
	}
}

void UILayerMain::OnClicked_CancelFinishMainTask(RtwWidget* sender, RtwEventDelegate* e)
{
	if(g_layerMain->m_formBtn2->IsVisible()) 
	{
		g_layerMain->m_formBtn2->Hide();
	}
	if (m_formMainLineTask->IsVisible())
	{
		m_formMainLineTask->Hide();
	}
}

void UILayerMain::OnHideHelp(RtwWidget* sender, RtwEventDelegate* e)
{
	if (m_formHelp->IsVisible())
	{
		m_formHelp->Hide();
	}
	if (m_formGuide4->IsVisible())
	{
		m_formGuide4->Hide();
	}
}

void UILayerMain::OnHideHelp2(RtwWidget* sender, RtwEventDelegate* e)
{
	if (m_formHelp2->IsVisible())
	{
		m_formHelp2->Hide();
	}
	if (m_formGuide3->IsVisible())
	{
		m_formGuide3->Hide();
	}
}

void UILayerMain::OnHideHelp3(RtwWidget* sender, RtwEventDelegate* e)
{
	if (m_formHelp3->IsVisible())
	{
		m_formHelp3->Hide();
	}
	if (m_formGuide3->IsVisible())
	{
		m_formGuide3->Hide();
	}
}
void UILayerMain::BuildMainTaskList()
{
	taskChapterId.clear();
	taskMainLine.clear();
	m_taskTree->Clear();
	CTaskInfo *task = &GetWorld()->m_pPlayer->m_task;
	int section = task->m_nMainTaskData;

	if (!g_TaskGroup.GetMainLineID(taskChapterId)) 
		return;
	vector<int>::iterator itGroup;
	for (itGroup = taskChapterId.begin(); itGroup != taskChapterId.end(); itGroup++)
	{
		TaskGroupData* taskGroup = g_TaskGroup.FindTaskGroup(*itGroup);
		m_taskTree->InsertTreeItem(NULL, taskGroup->strGroupName.c_str(), "");
		taskMainLine = g_TableMainStory.GetMainStoryDataV();
		vector<MainStoryData>::iterator itLine;
		for(itLine = taskMainLine.begin(); itLine != taskMainLine.end(); itLine++)
		{
			if((*itLine).nGroupID == (*itGroup))
			{
				//职业判断
				if ((*itLine).nJob)
				{
					if((*itLine).nJob != GetWorld()->m_pPlayer->m_core.Metier) continue;
				}
				m_taskTree->InsertTreeItem(taskGroup->strGroupName.c_str(), 
					(*itLine).strDataName.c_str(),
					"", 0, -1, 0);
				if ((*itLine).nDataID < section)
				{
					m_taskTree->SetItemColor((*itLine).strDataName.c_str(), 0xFFFFD700);
				} 
				else if ((*itLine).nDataID > section)
				{
					m_taskTree->SetItemColor((*itLine).strDataName.c_str(), 0xFF939393);
				}
			}
		}
	}
	//展开树项
	if (section)
	{
		MainStoryData* data = g_TableMainStory.FindMainStory(section);
		RtwTree::Item* pItem = m_taskTree->FindTreeItemByText(data->strDataName.c_str());
		if (pItem)
		{
			TaskGroupData* taskGroup = g_TaskGroup.FindTaskGroup(data->nGroupID);
			RtwTree::Item* pItemGroup = m_taskTree->FindTreeItemByText(taskGroup->strGroupName.c_str());
			m_taskTree->Expand(pItemGroup);
			m_taskTree->Select(pItem);
		} 
	}
}

void UILayerMain::OnClicked_ShowScript1(RtwWidget* sender, RtwEventDelegate* e)
{
	CG_CmdPacket *cmd = NetBeginWrite();
	cmd->WriteShort(c2r_message_dialog_sure);
	cmd->WriteString((char*)GetWorld()->GetCommandText().c_str());
	NetSend(cmd);
	if (m_formMessageBox1->IsVisible())
	{
		m_formMessageBox1->Hide();
	}
}

void UILayerMain::OnClicked_CancelScript1(RtwWidget* sender, RtwEventDelegate* e)
{
	if (m_formMessageBox1->IsVisible())
	{
		m_formMessageBox1->Hide();
	}
}

void UILayerMain::OnClicked_ShowScript2(RtwWidget* sender, RtwEventDelegate* e)
{
	CG_CmdPacket *cmd = NetBeginWrite();
	cmd->WriteShort(c2r_message_dialog_sure);
	cmd->WriteString((char*)GetWorld()->GetCommandText().c_str());
	NetSend(cmd);
	if (m_formMessageBox2->IsVisible())
	{
		m_formMessageBox2->Hide();
	}
}

void UILayerMain::HideGuideAllForm(void*, void*)
{
	g_layerMain->m_formGuide1->Hide();
}

static long dwSubTime = 0.f;

void UILayerMain::ShowActivityTime(float fSecond)
{
	if(!bStartActivity) return;

	if (strActivityText.empty())
	{
		bStartActivity = false;
		m_LableTimer->Hide();
	}
	char szBuf[128];

	dwSubTime += fSecond;
	long dwTime = (lActivityTime - dwSubTime)/1000;
	DWORD dwMinute	= dwTime/60;
	DWORD dwSecond	= dwTime%60;
	if (dwTime > 0)
	{
		rt2_sprintf(szBuf, "%s%2d分%2d秒", strActivityText.c_str(), dwMinute, dwSecond);
		m_LableTimer->SetText(szBuf);
		m_LableTimer->Show();
	}
	else
	{
		bStartActivity = false;
		m_LableTimer->Hide();
	}
}

void UILayerMain::ResetActivityTime()
{
	dwSubTime = 0.f;
}

void UILayerMain::ShowSceneInfo(int num, int idx, std::string &info)
{
	if(num == 1)
	{
		if(idx == 0)
		{
			m_LableSceneInfo->SetText(info);
			m_formSceneInfo->Show();
			m_formSceneInfo1->Hide();
			m_formSceneInfo2->Hide();
		}
	}
	else if (num == 2)
	{
		if(idx == 0)
		{
			m_LableSceneInfo11->SetText(info);
		}
		if (idx == 1)
		{
			m_LableSceneInfo12->SetText(info);
		}
		m_formSceneInfo->Hide();
		m_formSceneInfo1->Show();
		m_formSceneInfo2->Hide();
	} 
	else if(num == 3)
	{
		if (idx == 0)
		{
			m_LableSceneInfo21->SetText(info);
		}
		if (idx == 1)
		{
			m_LableSceneInfo22->SetText(info);
		}
		if (idx == 2)
		{
			m_LableSceneInfo23->SetText(info);
		}
		m_formSceneInfo->Hide();
		m_formSceneInfo1->Hide();
		m_formSceneInfo2->Show();
	}
}

void UILayerMain::SendScheduleMsg()
{
	CG_CmdPacket *cmd = NetBeginWrite();
	cmd->WriteShort(c2r_execute_script);
	cmd->WriteString(strCommand.c_str());
	NetSend(cmd);
}

void UILayerMain::OnBtnAutoStateYes(void*, void*)
{
	guard;

	LOAD_UI("forautostate")->Hide();
	m_btnAutoBattle->SetChecked(false);
	//m_btnAutoPick->SetChecked(false);		//heten
	GetWorld()->m_pPlayer->SetAutobattle(false);
	//GetWorld()->m_pPlayer->SetAutoPick(false);		//heten
	GetWorld()->m_pPlayer->SetAutoState(false);
	GetWorld()->m_Chat.SystemMessage(R(MSG_TOOLOFF), false);
	unguard;
}

void UILayerMain::OnBtnAutoStateNo(void*, void*)
{
	guard;

	LOAD_UI("forautostate")->Hide();
	
	unguard;
}

//void UILayerMain::OnClicked_Attribute_char(RtwWidget* sender, RtwEventDelegate* e)
//{
//	guard;
//
//    RtwTab* pTab = LOAD_UI_T(RtwTab, "fmcharall.tabcharall");
//    if (!m_char->IsVisible())
//    {
//        m_char->Show();
//    }else
//    {
//        if (pTab->GetSelectedItem()==0)
//        {
//            m_char->Hide();
//            return;
//        }
//    }
//    pTab->SetSelectedItem(0);
//
//	unguard;
//}

//void UILayerMain::OnClicked_Attribute_union(RtwWidget* sender, RtwEventDelegate* e)
//{
//	guard;
//
//    RtwTab* pTab = LOAD_UI_T(RtwTab, "fmcharall.tabcharall");
//    if (!m_char->IsVisible())
//    {
//        m_char->Show();
//    }else
//    {
//        if (pTab->GetSelectedItem()==1)
//        {
//            m_char->Hide();
//            return;
//        }
//    }
//    pTab->SetSelectedItem(1);
//
//	unguard;
//}

//void UILayerMain::OnClicked_Attribute_soc(RtwWidget* sender, RtwEventDelegate* e)
//{
//	guard;
//
//    RtwTab* pTab = LOAD_UI_T(RtwTab, "fmcharall.tabcharall");
//    if (!m_char->IsVisible())
//    {
//        m_char->Show();
//    }else
//    {
//        if (pTab->GetSelectedItem()==2)
//        {
//            m_char->Hide();
//            return;
//        }
//    }
//    pTab->SetSelectedItem(2);
//
//	unguard;
//}

void UILayerMain::OnClicked_Attribute_union(RtwWidget* sender, RtwEventDelegate* e)
{
	guard;

    //RtwTab* pTab = LOAD_UI_T(RtwTab, "fmcharall.tabcharall");
    //if (!m_char->IsVisible())
    //{
    //    m_char->Show();
    //}else
    //{
    //    if (pTab->GetSelectedItem()==1)
    //    {
    //        m_char->Hide();
    //        return;
    //    }
    //}
    //pTab->SetSelectedItem(1);
	OnClicked_Union(sender, e);

	unguard;
}

//void UILayerMain::OnClicked_Attribute_soc(RtwWidget* sender, RtwEventDelegate* e)
//{
//	guard;
//
//    RtwTab* pTab = LOAD_UI_T(RtwTab, "fmcharall.tabcharall");
//    if (!m_char->IsVisible())
//    {
//        m_char->Show();
//    }else
//    {
//        if (pTab->GetSelectedItem()==2)
//        {
//            m_char->Hide();
//            return;
//        }
//    }
//    pTab->SetSelectedItem(2);
//
//	unguard;
//}

void UILayerMain::OnClicked_Attribute_task(RtwWidget* sender, RtwEventDelegate* e)
{
	guard;
	if (m_formTask->IsVisible())
	{
		m_formTask->Hide();
	} 
	else
	{
		m_formTask->Show();
	}
    //RtwTab* pTab = LOAD_UI_T(RtwTab, "fmcharall.tabcharall");
    //if (!m_char->IsVisible())
    //{
    //    m_char->Show();
    //}else
    //{
    //    if (pTab->GetSelectedItem()==3)
    //    {
    //        m_char->Hide();
    //        return;
    //    }
    //}
    //pTab->SetSelectedItem(3);

	unguard;
}


// void UILayerMain::OnMenuUserByName(RtwWidget* sender,RtwEventDelegate* e)
// {
// 	guard;
// 	int iSelectItem = ((RtwPopupMenu*)sender)->GetLastSelectItem();
// 	if (iSelectItem>=0)
// 	{
// 		int iData = (int)((RtwPopupMenu*)sender)->GetItemData(iSelectItem);
// 		const string& rName = ((RtwPopupMenu*)sender)->GetItemUserString(iSelectItem);
// 		switch (iData)
// 		{
// 		case 1: // 密语, 把对方的名字复制到密语那里
// 			GetWorld()->m_Chat.OnMemberChoice(0, rName.c_str());
// 			break;
// 		case 2: //复制玩家的名字
// // 注释 [3/18/2009 tooth.shi]			CClipboard::Copy(rName);
// 			break;
// 		case 3: // 添加入黑名单
// 			{
// 				GetWorld()->m_Chat.AddBlackList(rName);
// 			}
// 			break;
// 		default: // 其他
// 			break;
// 		}
// 	}
// 	unguard;
// }
void UILayerMain::OnMenuUser(RtwWidget* sender, RtwEventDelegate* e)
{
	guard;

    int iSelectItem = ((RtwPopupMenu*)sender)->GetLastSelectItem();
    if (iSelectItem>=0)
    {
        GcActor* pActor = FindActor((DWORD)((RtwPopupMenu*)sender)->GetUserData());
        if (pActor)
        {
            int iData = (int)((RtwPopupMenu*)sender)->GetItemData(iSelectItem);
            switch (iData)
            {
            case 1: // 密语, 把对方的名字复制到密语那里
                GetWorld()->m_Chat.OnMemberChoice(0, pActor->GetName());
                break;
            case 2: // 交易
                g_layerMain->m_formTrade->OnCommand(pActor);
                break;
            case 3: // 队伍
                GetWorld()->m_Team.OnCommand(pActor, ((RtwPopupMenu*)sender)->GetItemUserString(iSelectItem));
                break;
            case 4: // 好友
                GetWorld()->m_Friend.OnCommand(pActor, ((RtwPopupMenu*)sender)->GetItemUserString(iSelectItem));
                break;
            case 5: // 工会
                g_layerMain->m_formUnion->OnCommand(pActor);//, ((RtwPopupMenu*)sender)->GetItemUserString(iSelectItem));
                break;
			case 6: //复制玩家的名字
				g_workspace.getClipboard()->Copy(pActor->GetName());
				break;
			case 7: // 察看道具
				g_layerMain->m_formItemsInfo->OnCommand(pActor);
				break;
			case 8://添加入黑名单
				{
					GetWorld()->m_Friend.AddOtherToBlackList(string(pActor->GetName()));
				}
				break;
			case 9:
				break;
			case 10:
				break;
            default: // 其他
                ShowMessage(((RtwPopupMenu*)sender)->GetItemText(iSelectItem).c_str());
                break;
            }
        }
    }

	unguard;
}

void UILayerMain::OnMenuMonster(RtwWidget* sender, RtwEventDelegate* e)
{
	guard;
    int iSelectItem = ((RtwPopupMenu*)sender)->GetLastSelectItem();
    if (iSelectItem>=0) ShowMessage(((RtwPopupMenu*)sender)->GetItemText(iSelectItem).c_str());
	unguard;
}

void UILayerMain::OnMenuPlayer(RtwWidget* sender, RtwEventDelegate* e)
{
	guard;
    int iSelectItem = ((RtwPopupMenu*)sender)->GetLastSelectItem();
    if (iSelectItem>=0) ShowMessage(((RtwPopupMenu*)sender)->GetItemText(iSelectItem).c_str());
	unguard;
}

void UILayerMain::OnMenuPet(RtwWidget* sender, RtwEventDelegate* e)
{
	guard;
    int iSelectItem = ((RtwPopupMenu*)sender)->GetLastSelectItem();
    if (iSelectItem>=0) ShowMessage(((RtwPopupMenu*)sender)->GetItemText(iSelectItem).c_str());
	unguard;
}

void UILayerMain::OnMenuNpc(RtwWidget* sender, RtwEventDelegate* e)
{
	guard;
    int iSelectItem = ((RtwPopupMenu*)sender)->GetLastSelectItem();
    if (iSelectItem>=0) ShowMessage(((RtwPopupMenu*)sender)->GetItemText(iSelectItem).c_str());
	unguard;
}

void UILayerMain::OnMenuSystem(RtwWidget* sender, RtwEventDelegate* e)
{
	guard;
    int iSelectItem = ((RtwPopupMenu*)sender)->GetLastSelectItem();
    if (iSelectItem>=0) ShowMessage(((RtwPopupMenu*)sender)->GetItemText(iSelectItem).c_str());
	unguard;
}

void UILayerMain::OnFunAction(RtwWidget* sender, RtwEventDelegate* e)
{
    guard;
    GcPlayer* pPlayer = GetPlayer();
    if (pPlayer)
    {
        if (pPlayer->IsIdel())
        {
            short sIdx = (short)sender->GetUser1();
            char cDir = rtgEncodeUprightDirection(*(pPlayer->GetMatrix()));
            CG_CmdPacket* pPacket = NetBeginWrite();
            pPacket->WriteShort(c2r_play_pose);
            pPacket->WriteShort(sIdx);
            pPacket->WriteByte(cDir);
            NetSend(pPacket);
        }
    }
    unguard;
}

void UILayerMain::OnClicked_TeamApply(RtwWidget* sender, void*)
{
	guard;
	if(m_formTeam->IsVisibale())
		m_formTeam->Hide();
	else
		m_formTeam->Show();
	unguard;
}
void UILayerMain::OnClicked_TeamOff(RtwWidget* sender, void*)
{
	guard;
	GetPlayer()->m_bTeamOnOff = !GetPlayer()->m_bTeamOnOff;
	if (GetPlayer()->m_bTeamOnOff)
	{
		// 队伍开关
		LOAD_UI("btnteamoff")->SetText(string("开"));
		GetWorld()->m_Team.SendToServer(7,1);
	}else{
		LOAD_UI("btnteamoff")->SetText(string("关"));
		GetWorld()->m_Team.SendToServer(7,0);
	}
	unguard;
}
void UILayerMain::OnClicked_PkMode(RtwWidget* sender, void*)
{
	guard;
	if (m_formPkMode->IsVisible())
		m_formPkMode->Hide();
	else
		m_formPkMode->Show();
	unguard;
}

void UILayerMain::OnActivateFrmPkMode(RtwWidget* sender, RtwEventDelegate* e)
{
	guard;

	if(sender == LOAD_UI("lstPKmode"))
	{
		if (e->type == weMouseClick && e->activate.widget != LOAD_UI("fmhp.btnPKmode"))
		{
			LOAD_UI("lstPKmode")->Hide();
		}
	}

	unguard;
}

void UILayerMain::UpdateBuyCardList(CG_CmdPacket* pPacket)
{
    if (m_formBuyCard)
    {
        m_formBuyCard->OnNetDataUpdate(pPacket);
    }
}

void UILayerMain::OnBuyCardResult(CG_CmdPacket* pPacket)
{
    if (m_formBuyCard)
    {
        m_formBuyCard->OnBuyCardResult(pPacket);
    }
}
void UILayerMain::SetMiddleMapMouseState(bool bIsDown)
{
	guard;
	m_formMiddleMap->SetMouseDownState(bIsDown);
	unguard;
}
bool UILayerMain::OnCloseWidget()
{
	guard;
	// 此处统计 所有的内界面，可能第一次会添加不全，以后如果有内界面 都应在这里处理一下
	bool succeed = false;
	/*if(m_popupMenu->IsVisible())
	{
		m_popupMenu->Hide();
		succeed = true;
	}*/
	if(m_formHelp->IsVisible())
	{
		m_formHelp->Hide();
		succeed = true;
	}
	if(m_formHelp2->IsVisible())
	{
		m_formHelp2->Hide();
		succeed = true;
	}
	if(m_formHelp3->IsVisible())
	{
		m_formHelp3->Hide();
		succeed = true;
	}
	return succeed;
	unguard;
}
void UILayerMain::OnDragEndAliasButton(RtwWidget* vpSender, RtwEventDelegate* e)
{
	guard;
	unguard;
}

void UILayerMain::ShowHyerLinkFromChat(const char *pPacket)
{
	string strPacket = string(pPacket);
	std::string strHtml = "";
	int nLines;

	map<string, strWidthLine>::iterator mapFindIter = m_mapHtmlContainer.find(strPacket);
	if (mapFindIter == m_mapHtmlContainer.end())
	{
		SItemID tmpItem;

		tmpItem.LoadFromString(11, pPacket, GetWorld()->m_pPlayer->mItem.GetItemManager());
		strHtml = GenerateItemHtmlHint(nLines, tmpItem, GetWorld()->m_pPlayer).c_str();

		m_mapHtmlContainer[strPacket].m_strMsg = strHtml;
		m_mapHtmlContainer[strPacket].m_nLine = nLines;
	}
	else
	{
		strHtml = m_mapHtmlContainer[strPacket].m_strMsg;;
		nLines	= m_mapHtmlContainer[strPacket].m_nLine;
	}

	m_htmlviewLink->SetHtmlText(strHtml.c_str());
	m_fmhyperLinkInChat->Show();
	m_htmlviewLink->Show();

	RtwRect fmRect = m_fmhyperLinkInChat->GetFrameRect();

	int nNewHeight = (nLines+12) * g_workspace.getFontManager()->getFontSize();
	fmRect.top = fmRect.bottom - nNewHeight;

	m_fmhyperLinkInChat->Move(fmRect);
}

//倒计时 ldr123
//begin
void UILayerMain::CountDownTimer(DWORD dwMillisecond)
{
	if (m_fmCountDown->IsVisible())
	{
		m_fmCountDown->Hide();
	}

	if (m_nCountDownMillisecond > 7200000)//超过奖励时间2小时
	{
		return;
	}

	if (!m_bCountDownStart)
	{
		return;
	}

	static const RtwPixel CountDownColor[] = 
	{
		0xffffc125,	/*倒计时颜色(两个切换)*/
		0xffff7f24
	};

	static const int nCountDownTimeList[7] = 
	{
		//分钟:		5, 10, 15, 20, 20, 25, 25
		//累加转换后5, 15, 30, 50, 70, 95, 120
		300, 900, 1800, 3000, 4200, 5700, 7200
	};

	//static char	szBuf[128]	= "\0";	//颜色变化
	S_MEMDEF(szBuf, 128)
	S_MEMPROTECTOR(szBuf, 128, bDefined)

	DWORD	dwTime = 0;		//秒
	bool	bStart = false;	//开始显示
	int		nPos	= 0;	//第几个奖品时间

	int		nColorIndex = 0;	//颜色索引

	m_nCountDownMillisecond += dwMillisecond;	//毫秒
	DWORD dwCountDownSecond = m_nCountDownMillisecond / 1000; //秒

	for (nPos=0; nPos<6; ++nPos)
	{
		if (dwCountDownSecond < nCountDownTimeList[nPos])
		{
			dwTime = nCountDownTimeList[nPos];
			bStart = true;
			break;
		}
	}


	if (bStart)	//倒计时可用
	{
		if (!m_fmCountDown->IsVisible())
		{
			m_fmCountDown->Show();
		}

		if (nPos == 0)
		{
			rt2_sprintf(szBuf, "继续在线%d分钟,有惊喜礼包!", (nCountDownTimeList[nPos])/60);
		}
		else
		{
			rt2_sprintf(szBuf, "继续在线%d分钟,有惊喜礼包!", (nCountDownTimeList[nPos]-nCountDownTimeList[nPos-1])/60);
		}

		nColorIndex = dwCountDownSecond%2;
		m_lblCountDownConditionText->SetText(szBuf);

		m_lblCountDown->SetTextColor(CountDownColor[nColorIndex]);
		m_lblCountDown->SetText("倒计时:");

		DWORD dwStep	= dwTime - dwCountDownSecond;
		DWORD dwMinute	= dwStep/60%60;
		DWORD dwSecond	= dwStep%60;
		if (dwStep > 0)
		{
			rt2_sprintf(szBuf, "%2d分%2d秒", dwMinute, dwSecond);
			m_lblCountDownText->SetTextColor(CountDownColor[nColorIndex]);
			m_lblCountDownText->SetText(szBuf);
		}
	}
	else
	{
		if (m_fmCountDown->IsVisible())
		{
			m_fmCountDown->Hide();
		}
	}
}

void UILayerMain::ShowCountDownForm()
{
	m_pNewPeopleHelp->ShowUserLev();
}

void UILayerMain::SetCountDownFormVisable(bool bVisable)
{
	m_pNewPeopleHelp->SetVisable(bVisable);
}
void UILayerMain::OnSwitchWeapon(void*,void*)
{
	guard;
	if(m_bCanUseSwitchWeapon)
	{
		GetPlayer()->UseSelfSkill(0, SKILL_SWITCH_WEAPON, rtRandom());
		m_bCanUseSwitchWeapon = false;
	}
	unguard;
}

void UILayerMain::OnHotKeyCheck(int iButton,bool vAlt,bool vCtrl,bool vShift)
{
	guard;
	DWORD now = MAKE_SHORTCUT(iButton, vAlt, vCtrl, vShift);
	for(size_t i = 0;i<UI_HOT_KEY_COUNT;++i)
	{
		if(m_hotKeySet[i] == now && !m_bHotKeyState[i])
		{
			m_hotKeyAlias[i]->CommandButton();
			m_bHotKeyState[i] = true;
		}
	}
	unguard;
}
void UILayerMain::OnResHotKeyState(int iButton)
{
	guard;
	DWORD now = MAKE_SHORTCUT(iButton, false, false, false);
	DWORD now1 = MAKE_SHORTCUT(iButton, true, false, false);
	DWORD now2 = MAKE_SHORTCUT(iButton, false, true, false);
	DWORD now3 = MAKE_SHORTCUT(iButton, false, false, true);
	for(size_t i = 0;i<UI_HOT_KEY_COUNT;++i)
	{
		if(m_hotKeySet[i] == now || m_hotKeySet[i] == now1 || m_hotKeySet[i] == now2 || m_hotKeySet[i] == now3)
		{
			m_bHotKeyState[i] = false;
		}
	}
	unguard;
}
void UILayerMain::OnSetDefaultHotKeyState()
{
	guard;
	m_hotKeySet[0] = MAKE_SHORTCUT(49,false,false,false);
	m_hotKeySet[1] = MAKE_SHORTCUT(50,false,false,false);
	m_hotKeySet[2] = MAKE_SHORTCUT(51,false,false,false);
	m_hotKeySet[3] = MAKE_SHORTCUT(52,false,false,false);
	m_hotKeySet[4] = MAKE_SHORTCUT(53,false,false,false);
	m_hotKeySet[5] = MAKE_SHORTCUT(54,false,false,false);
	m_hotKeySet[6] = MAKE_SHORTCUT(55,false,false,false);
	m_hotKeySet[7] = MAKE_SHORTCUT(56,false,false,false);
	m_hotKeySet[8] = MAKE_SHORTCUT(57,false,false,false);
	m_hotKeySet[9] = MAKE_SHORTCUT(48,false,false,false);
	m_hotKeySet[10] = MAKE_SHORTCUT(49,true,false,false);
	m_hotKeySet[11] = MAKE_SHORTCUT(50,true,false,false);
	m_hotKeySet[12] = MAKE_SHORTCUT(51,true,false,false);
	m_hotKeySet[13] = MAKE_SHORTCUT(52,true,false,false);
	m_hotKeySet[14] = MAKE_SHORTCUT(53,true,false,false);
	m_hotKeySet[15] = MAKE_SHORTCUT(54,true,false,false);
	m_hotKeySet[16] = MAKE_SHORTCUT(55,true,false,false);
	m_hotKeySet[17] = MAKE_SHORTCUT(56,true,false,false);
	m_hotKeySet[18] = MAKE_SHORTCUT(57,true,false,false);
	m_hotKeySet[19] = MAKE_SHORTCUT(48,true,false,false);
	m_hotKeySet[20] = MAKE_SHORTCUT(49,false,true,false);
	m_hotKeySet[21] = MAKE_SHORTCUT(50,false,true,false);
	m_hotKeySet[22] = MAKE_SHORTCUT(51,false,true,false);
	m_hotKeySet[23] = MAKE_SHORTCUT(52,false,true,false);
	m_hotKeySet[24] = MAKE_SHORTCUT(53,false,true,false);
	m_hotKeySet[25] = MAKE_SHORTCUT(54,false,true,false);
	m_hotKeySet[26] = MAKE_SHORTCUT(55,false,true,false);
	m_hotKeySet[27] = MAKE_SHORTCUT(56,false,true,false);
	m_hotKeySet[28] = MAKE_SHORTCUT(57,false,true,false);
	m_hotKeySet[29] = MAKE_SHORTCUT(48,false,true,false);
	for(size_t i =0;i< UI_HOT_KEY_COUNT;i++)
	{
		m_bHotKeyState[i] = false;
	}
	unguard;
}
void UILayerMain::OnSetHotKeyStateFree()
{
	guard;
	unguard;
}

void UILayerMain::OnMouseMoveOnSkillShotcut()
{
	guard;
	RtwRect rectShotcut1 = LOAD_UI("fmshortcut_bar1")->GetFrameRect();
	RtwRect rectShotcut2 = LOAD_UI("fmshortcut_bar2")->GetFrameRect();
	if(rectShotcut1.IsContain(g_workspace.GetMousePosX(),g_workspace.GetMousePosY())
		|| rectShotcut2.IsContain(g_workspace.GetMousePosX(),g_workspace.GetMousePosY()) 
		|| g_workspace.GetDragClient())
		m_bNeedSetTranForShotcut = true;
	else
		m_bNeedSetTranForShotcut = false;
	unguard;
}

void UILayerMain::OnSetNewWidget(RtwWidget* pWidget)
{
	guard;
	if(m_vecWidget.size()<= 0)
	{
		m_vecWidget.push_back(pWidget);
	}else
	{
		if(pWidget == m_vecWidget[m_vecWidget.size()-1])
			return;
		vector<RtwWidget*>::iterator it;
		for(it= m_vecWidget.begin();it!= m_vecWidget.end();it++)
		{
			if((*it)== pWidget)
			{
				m_vecWidget.erase(it);
				m_vecWidget.push_back(pWidget);
				return;
			}
		}
		m_vecWidget.push_back(pWidget);
	}
	unguard;
}

void UILayerMain::OnDeleteWidget(RtwWidget* pWidget)
{
	guard;
	vector<RtwWidget*>::iterator it;
	for(it= m_vecWidget.begin();it!= m_vecWidget.end();it++)
	{
		if((*it)== pWidget)
		{
			m_vecWidget.erase(it);
			return;
		}
	}
	unguard;
}

RtwWidget* UILayerMain::GetFirstWidget()
{
	guard;
	RtwWidget* pWidget= NULL;
	if(m_vecWidget.size()<= 0)
		pWidget= NULL;
	else
		pWidget= m_vecWidget[m_vecWidget.size()-1];
	return pWidget;
	unguard;
}

RtwWidget* UILayerMain::GetFocusParentWidget(RtwWidget* son)
{
	guard;
	RtwWidget* parent = son;
	if(son)
	{
		while(parent->GetParent() != NULL)
		{
			parent = parent->GetParent();
		}
	}else
		return NULL;
	return parent;
	unguard;
}
