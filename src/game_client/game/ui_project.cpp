#include "ui_project.h"
#include "ui_form_minimap.h"

RtwImage::Ptr		CUIShowProject::mpBackGround[MAX_CHANNEL_CHATBOX];
RtwImage::Ptr		CUIShowProject::mpBorder[MAX_CHANNEL_CHATBOX];


CUINormalShowProject::CUINormalShowProject()//构造//初始化
{
	LOAD_UI("tbfeedback")->ModifyFlags(wfGrabMouse,0);//要接收鼠标消息
	RtString tmpStr;
	for(int i= 0;i< MAX_CHANNEL_CHATBOX;i++)
	{
		tmpStr.Format("tbfeedback.fmchat%d.txtFeedback",i);
		LOAD_UI(tmpStr.c_str())->ModifyFlags(wfGrabMouse,0);//要接收鼠标消息2010.6.4
		LOAD_UI(tmpStr.c_str())->SetBackgroundImage(mpBackGround[i]);//要背景
		LOAD_UI(tmpStr.c_str())->SetBorder(mpBorder[i]);//要背景
	}
};

CUITransparentShowProject::CUITransparentShowProject()//构造//初始化
{
// 	LOAD_UI(UI_CHAT_FORM_ALL_WHOLE_ID)->ModifyFlags(0,wfGrabMouse);//不接收鼠标消息
// 	LOAD_UI(UI_CHAT_FORM_WHOLE_ID)->ModifyFlags(0,wfGrabMouse);//不接收鼠标消息

	RtString tmpStr;
	for(int i= 0;i< MAX_CHANNEL_CHATBOX;i++)
	{
		tmpStr.Format("tbfeedback.fmchat%d.txtFeedback",i);
		LOAD_UI(tmpStr.c_str())->SetBackgroundImage((RtwImage*)NULL);//不要背景
		LOAD_UI(tmpStr.c_str())->SetBorder(NULL);//不要背景
	}
};

//----------------------------------------------------------------------------------------------
//方案一//直接输入
 CUIProject1::CUIProject1()//构造//初始化
{
	//设置默认的键盘输入焦点
	//g_workspace.SetFocusWidget(LOAD_UI(UI_INPUT_WHOLE_ID));
	g_workspace.SetDefaultFocusWidget(LOAD_UI(UI_INPUT_WHOLE_ID));//g_workspace.SetSysEditName(UI_INPUT_WHOLE_ID);// change [3/16/2009 tooth.shi]
	if(g_workspace.GetFocusWidget()==NULL)
	{
		g_workspace.SetFocusWidget(LOAD_UI(UI_INPUT_WHOLE_ID));
	}


	
	//设置快捷键
	Shortcut(false,true,false);//ctrl的一套
	Shortcut(true,false,false);//alt的一套
	
	//设置界面的hint
	ShortcutHint(true,false,false);
};



 bool CUIProject1::Capture(int vKey,bool vAlt,bool vCtrl,bool vShift)//按键按下
{//什么都不要做
	return false;
};

 void CUIProject1::KeyChar(int vChar)//字符信息
 {
	 return;
 }



//方案二//回车输入
 CUIProject2::CUIProject2() //构造//初始化
{
	//没有默认的键盘输入焦点
	//g_workspace.SetFocusWidget(NULL);
	g_workspace.SetDefaultFocusWidget(NULL);//g_workspace.SetSysEditName(""); // change [3/16/2009 tooth.shi]
	if(g_workspace.GetFocusWidget()==LOAD_UI(UI_INPUT_WHOLE_ID))
	{
		g_workspace.SetFocusWidget(NULL);
	}

	//设置快捷键
	Shortcut(false,false,false);//不带组合键的一套

	//设置界面的hint
	ShortcutHint(false,false,false);
};



 void CUIProject2::KeyChar(int vChar)//字符信息
 {
	 if (vChar!=VK_RETURN)
	 {
		 return;
	 }

	 RtwWidget* pWidget = g_workspace.GetFocusWidget();
	 RtwWidget* pChatWidget = LOAD_UI(UI_INPUT_WHOLE_ID);

	 if (pWidget == NULL || (pWidget==pChatWidget && !pChatWidget->IsVisible()))
	 {
		 //无焦点或焦点在输入框但输入框隐藏
		 GetWorld()->m_Chat.SetNeedChangeBackground(true);
		 g_workspace.SetFocusWidget(pChatWidget);
		 return;
	 }

	 if (pWidget == pChatWidget)
	 {
		 g_workspace.SetFocusWidget(NULL);
		 return;
	 }

	 GetWorld()->m_Chat.SetNeedChangeBackground(true);
	 g_workspace.SetFocusWidget(pChatWidget);
 }

 bool CUIProject2::Capture(int vKey,bool vAlt,bool vCtrl,bool vShift)//按键按下
{
	RtwWidget* pWidget=	g_workspace.GetFocusWidget();
	bool bIsChatOrEditBox = false;
	if (pWidget)
	{
		bIsChatOrEditBox = (pWidget->getWidgetType() == wtChatBox || pWidget->getWidgetType() == wtEditBox);
		if (bIsChatOrEditBox)
		{
			bIsChatOrEditBox = ((RtwTextBox*)pWidget)->getEnableInput();
		}
	}

	if (!bIsChatOrEditBox)
	{//输入焦点无
		return false;
	}
	return true;
}


/*

	if((!vAlt) && (!vCtrl) &&(!vShift))
	{
		if(pWidget==NULL)
		{//输入焦点无，则到聊天框
			if(vKey==VK_RETURN)
			{
				//GetWorld()->m_Chat.KeepFocus(true);
				g_workspace.SetFocusWidget(LOAD_UI(UI_INPUT_WHOLE_ID));
			}
		}
		else if(pWidget==LOAD_UI(UI_INPUT_WHOLE_ID))
		{//输入焦点在聊天输入框中//截获
			if(vKey==VK_RETURN)
			{
				//g_workspace.SetFocusWidget(NULL);
				//GetWorld()->m_Chat.KeepFocus(false);
			}
			return true;
		}
	}
	
	return false;
	*/


 void CUIProject::KeyDown(int vKey,bool vAlt,bool vCtrl,bool vShift)//按键按下
{
	if(Capture(vKey,vAlt,vCtrl,vShift))
	{
		return;
	}

	DWORD dwShortcut = MAKE_SHORTCUT(vKey, vAlt, vCtrl, vShift);
	std::map<DWORD, RtwEventDispatcher>::iterator it = mShortcutList.find(dwShortcut);
	if (it!=mShortcutList.end())
	{
		(*it).second(NULL, NULL);
	}
};




 static bool HintAlt=false;
 static bool HintCtrl=false;
 static bool HintShift=false;
 void SetMakeHint(bool vAlt,bool vCtrl,bool vShift)
 {
	 HintAlt=vAlt;
	 HintCtrl=vCtrl;
	 HintShift=vShift;
 }

 string MakeHint(string vString,string vShortcutChar)
 {
	 string Temp;
	 Temp+=vString;
	 Temp+= HintAlt ? "(Alt+" + vShortcutChar + ")" :  "";
	 Temp+= HintCtrl ? "(Ctrl+" + vShortcutChar + ")" : "";
	 Temp+= HintShift ? "(Shift+" + vShortcutChar + ")" :  "";

	 if( (!HintAlt) && (!HintCtrl) && (!HintShift))
	 {//都没有//用快捷键本身
		 Temp+="(" + vShortcutChar +")";
	 }

	 return Temp;
 }


void CUIProject::ShortcutHint(bool vAlt,bool vCtrl,bool vShift)//hint的设置
{
	SetMakeHint(vAlt,vCtrl,vShift);

	// 属性C
	LOAD_UI("fmsystem.btnsyschar")->SetHintText(MakeHint(R(HINT_CHAR_STRING),"C"));
	// 宠物
	LOAD_UI("fmsystem.btnpet")->SetHintText(MakeHint(R(HINT_PET_STRING),"Z"));
	// 道具
	LOAD_UI("fmsystem.btnsysitem")->SetHintText(MakeHint(R(HINT_ITEM_STRING),"B"));
	// 技能S
	LOAD_UI("fmsystem.btnsysskill")->SetHintText(MakeHint(R(HINT_SKILL_STRING),"R"));
	// 任务Q
	LOAD_UI("fmsystem.btnsystask")->SetHintText(MakeHint(R(HINT_TASK_STRING),"Q"));
	// 组队
	LOAD_UI("fmsystem.btnteam")->SetHintText(MakeHint(R(HINT_TEAM_STRING),"T"));
	// 社群W
	LOAD_UI("fmsystem.btnsysfriend")->SetHintText(MakeHint(R(HINT_SOC_STRING),"F"));
	// 帮派
	LOAD_UI("fmsystem.btnbangpai")->SetHintText(MakeHint(R(HINT_UNION_STRING),"G"));
	// 邮件M
	LOAD_UI("fmsystem.btnsysmail")->SetHintText(MakeHint(R(HINT_MAIL_STRING),"L"));
	// 排行 
	LOAD_UI("fmsystem.btnsyslist")->SetHintText(MakeHint(R(HINT_LIST_STRING),"P"));
	// 系统O 
	LOAD_UI("fmsystem.btnsystem")->SetHintText(MakeHint(R(HINT_SYSTEM_STRING),"X"));
	//// 诸侯
	//LOAD_UI("fmcharall.tabcharall.btnguild")->SetHintText(MakeHint(R(HINT_GUILD_STRING),"U"));
	//// 好友F
	//LOAD_UI("fmsystem.btnsysfriend")->SetHintText(MakeHint(R(HINT_FRIEND_STRING),"F"));
}


 void CUIProject::Shortcut(bool vAlt,bool vCtrl,bool vShift) //设置快捷
{

	// 人物
	mShortcutList[MAKE_SHORTCUT('C', vAlt, vCtrl, vShift)] = RTW_CALLBACK(g_layerMain, UILayerMain, OnClicked_System_Char);
	// 道具
	mShortcutList[MAKE_SHORTCUT('B', vAlt, vCtrl, vShift)] = RTW_CALLBACK(g_layerMain, UILayerMain, OnClicked_PlayerItems);
	// 技能
	mShortcutList[MAKE_SHORTCUT('R', vAlt, vCtrl, vShift)] = RTW_CALLBACK(g_layerMain, UILayerMain, OnClicked_System_Skill);
	// 中地图
	mShortcutList[MAKE_SHORTCUT('M', vAlt, vCtrl, vShift)] = RTW_CALLBACK(g_layerMain, UILayerMain, OnClicked_System_Map);
	// 世界地图
	//mShortcutList[MAKE_SHORTCUT('N', vAlt, vCtrl, vShift)] = RTW_CALLBACK(g_layerMain, UIFormMiniMap, OnButtonShowWorldMap);
	// 任务
	mShortcutList[MAKE_SHORTCUT('Q', vAlt, vCtrl, vShift)] = RTW_CALLBACK(g_layerMain, UILayerMain, OnClicked_Attribute_task);
	// 组队
	mShortcutList[MAKE_SHORTCUT('T', vAlt, vCtrl, vShift)] = RTW_CALLBACK(g_layerMain, UILayerMain, OnClicked_TeamApply);
	// 好友
	mShortcutList[MAKE_SHORTCUT('F', vAlt, vCtrl, vShift)] = RTW_CALLBACK(&(GetWorld()->m_Friend), CFriendWithUI, OpenUI);
	// 排行
	mShortcutList[MAKE_SHORTCUT('P', vAlt, vCtrl, vShift)] = RTW_CALLBACK(g_layerMain, UILayerMain, OnClicked_HeroList);
	// 设置
	mShortcutList[MAKE_SHORTCUT('X', vAlt, vCtrl, vShift)] = RTW_CALLBACK(g_layerMain, UILayerMain, OnClicked_System_System);
	// 帮派
	mShortcutList[MAKE_SHORTCUT('G', vAlt, vCtrl, vShift)] = RTW_CALLBACK(g_layerMain, UILayerMain, OnClicked_Attribute_union);
	// 邮件
	mShortcutList[MAKE_SHORTCUT('L', vAlt, vCtrl, vShift)] = RTW_CALLBACK(&(GetWorld()->m_Mail), CMailWithUI, OpenMailUI);

	mShortcutList[MAKE_SHORTCUT('Z', vAlt, vCtrl, vShift)] = RTW_CALLBACK(g_layerMain, UILayerMain, OnButtonPetDown);


	// 内挂
	//mShortcutList[MAKE_SHORTCUT('Z', true, vCtrl, vShift)] = RTW_CALLBACK(g_layerMain, UILayerMain, OnClicked_AutoFight);
	mShortcutList[MAKE_SHORTCUT('Z', true, vCtrl, vShift)] = RTW_CALLBACK(g_layerMain, UIFormMiniMap, SysBattleSetting);  //add by yz 开启关闭内挂快捷键
	// 商城
	mShortcutList[MAKE_SHORTCUT('V', true, vCtrl, vShift)] = RTW_CALLBACK(g_layerMain, UILayerMain, OnClicked_ShopCenter);
	// 帮助
	mShortcutList[MAKE_SHORTCUT('H', true, vCtrl, vShift)] = RTW_CALLBACK(g_layerMain, UILayerMain, OnShowHelp);
	// 换线
	mShortcutList[MAKE_SHORTCUT('L', true, vCtrl, vShift)] = RTW_CALLBACK(g_layerMain, UIFormMiniMap, OnSelectChildThread);
	//// 提示
	//mShortcutList[MAKE_SHORTCUT('A', true, vCtrl, vShift)] = RTW_CALLBACK(g_layerMain, UILayerMain, OnClicked_RemindIcon);

	mShortcutList[MAKE_SHORTCUT('E', false, vCtrl, vShift)] = RTW_CALLBACK(g_layerMain, UILayerMain, OnSwitchWeapon);

	////// 社群
	////mShortcutList[MAKE_SHORTCUT('W', vAlt, vCtrl, vShift)] = RTW_CALLBACK(g_layerMain, UILayerMain, OnClicked_Attribute_soc);

	//// GM
	//mShortcutList[MAKE_SHORTCUT('G', vAlt, vCtrl, vShift)] = RTW_CALLBACK(g_layerMain, UILayerMain, OnClicked_SysSetting_GM);
};
