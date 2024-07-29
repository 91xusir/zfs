#include "ui_project.h"
#include "ui_form_minimap.h"

RtwImage::Ptr		CUIShowProject::mpBackGround[MAX_CHANNEL_CHATBOX];
RtwImage::Ptr		CUIShowProject::mpBorder[MAX_CHANNEL_CHATBOX];


CUINormalShowProject::CUINormalShowProject()//����//��ʼ��
{
	LOAD_UI("tbfeedback")->ModifyFlags(wfGrabMouse,0);//Ҫ���������Ϣ
	RtString tmpStr;
	for(int i= 0;i< MAX_CHANNEL_CHATBOX;i++)
	{
		tmpStr.Format("tbfeedback.fmchat%d.txtFeedback",i);
		LOAD_UI(tmpStr.c_str())->ModifyFlags(wfGrabMouse,0);//Ҫ���������Ϣ2010.6.4
		LOAD_UI(tmpStr.c_str())->SetBackgroundImage(mpBackGround[i]);//Ҫ����
		LOAD_UI(tmpStr.c_str())->SetBorder(mpBorder[i]);//Ҫ����
	}
};

CUITransparentShowProject::CUITransparentShowProject()//����//��ʼ��
{
// 	LOAD_UI(UI_CHAT_FORM_ALL_WHOLE_ID)->ModifyFlags(0,wfGrabMouse);//�����������Ϣ
// 	LOAD_UI(UI_CHAT_FORM_WHOLE_ID)->ModifyFlags(0,wfGrabMouse);//�����������Ϣ

	RtString tmpStr;
	for(int i= 0;i< MAX_CHANNEL_CHATBOX;i++)
	{
		tmpStr.Format("tbfeedback.fmchat%d.txtFeedback",i);
		LOAD_UI(tmpStr.c_str())->SetBackgroundImage((RtwImage*)NULL);//��Ҫ����
		LOAD_UI(tmpStr.c_str())->SetBorder(NULL);//��Ҫ����
	}
};

//----------------------------------------------------------------------------------------------
//����һ//ֱ������
 CUIProject1::CUIProject1()//����//��ʼ��
{
	//����Ĭ�ϵļ������뽹��
	//g_workspace.SetFocusWidget(LOAD_UI(UI_INPUT_WHOLE_ID));
	g_workspace.SetDefaultFocusWidget(LOAD_UI(UI_INPUT_WHOLE_ID));//g_workspace.SetSysEditName(UI_INPUT_WHOLE_ID);// change [3/16/2009 tooth.shi]
	if(g_workspace.GetFocusWidget()==NULL)
	{
		g_workspace.SetFocusWidget(LOAD_UI(UI_INPUT_WHOLE_ID));
	}


	
	//���ÿ�ݼ�
	Shortcut(false,true,false);//ctrl��һ��
	Shortcut(true,false,false);//alt��һ��
	
	//���ý����hint
	ShortcutHint(true,false,false);
};



 bool CUIProject1::Capture(int vKey,bool vAlt,bool vCtrl,bool vShift)//��������
{//ʲô����Ҫ��
	return false;
};

 void CUIProject1::KeyChar(int vChar)//�ַ���Ϣ
 {
	 return;
 }



//������//�س�����
 CUIProject2::CUIProject2() //����//��ʼ��
{
	//û��Ĭ�ϵļ������뽹��
	//g_workspace.SetFocusWidget(NULL);
	g_workspace.SetDefaultFocusWidget(NULL);//g_workspace.SetSysEditName(""); // change [3/16/2009 tooth.shi]
	if(g_workspace.GetFocusWidget()==LOAD_UI(UI_INPUT_WHOLE_ID))
	{
		g_workspace.SetFocusWidget(NULL);
	}

	//���ÿ�ݼ�
	Shortcut(false,false,false);//������ϼ���һ��

	//���ý����hint
	ShortcutHint(false,false,false);
};



 void CUIProject2::KeyChar(int vChar)//�ַ���Ϣ
 {
	 if (vChar!=VK_RETURN)
	 {
		 return;
	 }

	 RtwWidget* pWidget = g_workspace.GetFocusWidget();
	 RtwWidget* pChatWidget = LOAD_UI(UI_INPUT_WHOLE_ID);

	 if (pWidget == NULL || (pWidget==pChatWidget && !pChatWidget->IsVisible()))
	 {
		 //�޽���򽹵�����������������
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

 bool CUIProject2::Capture(int vKey,bool vAlt,bool vCtrl,bool vShift)//��������
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
	{//���뽹����
		return false;
	}
	return true;
}


/*

	if((!vAlt) && (!vCtrl) &&(!vShift))
	{
		if(pWidget==NULL)
		{//���뽹���ޣ��������
			if(vKey==VK_RETURN)
			{
				//GetWorld()->m_Chat.KeepFocus(true);
				g_workspace.SetFocusWidget(LOAD_UI(UI_INPUT_WHOLE_ID));
			}
		}
		else if(pWidget==LOAD_UI(UI_INPUT_WHOLE_ID))
		{//���뽹���������������//�ػ�
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


 void CUIProject::KeyDown(int vKey,bool vAlt,bool vCtrl,bool vShift)//��������
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
	 {//��û��//�ÿ�ݼ�����
		 Temp+="(" + vShortcutChar +")";
	 }

	 return Temp;
 }


void CUIProject::ShortcutHint(bool vAlt,bool vCtrl,bool vShift)//hint������
{
	SetMakeHint(vAlt,vCtrl,vShift);

	// ����C
	LOAD_UI("fmsystem.btnsyschar")->SetHintText(MakeHint(R(HINT_CHAR_STRING),"C"));
	// ����
	LOAD_UI("fmsystem.btnpet")->SetHintText(MakeHint(R(HINT_PET_STRING),"Z"));
	// ����
	LOAD_UI("fmsystem.btnsysitem")->SetHintText(MakeHint(R(HINT_ITEM_STRING),"B"));
	// ����S
	LOAD_UI("fmsystem.btnsysskill")->SetHintText(MakeHint(R(HINT_SKILL_STRING),"R"));
	// ����Q
	LOAD_UI("fmsystem.btnsystask")->SetHintText(MakeHint(R(HINT_TASK_STRING),"Q"));
	// ���
	LOAD_UI("fmsystem.btnteam")->SetHintText(MakeHint(R(HINT_TEAM_STRING),"T"));
	// ��ȺW
	LOAD_UI("fmsystem.btnsysfriend")->SetHintText(MakeHint(R(HINT_SOC_STRING),"F"));
	// ����
	LOAD_UI("fmsystem.btnbangpai")->SetHintText(MakeHint(R(HINT_UNION_STRING),"G"));
	// �ʼ�M
	LOAD_UI("fmsystem.btnsysmail")->SetHintText(MakeHint(R(HINT_MAIL_STRING),"L"));
	// ���� 
	LOAD_UI("fmsystem.btnsyslist")->SetHintText(MakeHint(R(HINT_LIST_STRING),"P"));
	// ϵͳO 
	LOAD_UI("fmsystem.btnsystem")->SetHintText(MakeHint(R(HINT_SYSTEM_STRING),"X"));
	//// ���
	//LOAD_UI("fmcharall.tabcharall.btnguild")->SetHintText(MakeHint(R(HINT_GUILD_STRING),"U"));
	//// ����F
	//LOAD_UI("fmsystem.btnsysfriend")->SetHintText(MakeHint(R(HINT_FRIEND_STRING),"F"));
}


 void CUIProject::Shortcut(bool vAlt,bool vCtrl,bool vShift) //���ÿ��
{

	// ����
	mShortcutList[MAKE_SHORTCUT('C', vAlt, vCtrl, vShift)] = RTW_CALLBACK(g_layerMain, UILayerMain, OnClicked_System_Char);
	// ����
	mShortcutList[MAKE_SHORTCUT('B', vAlt, vCtrl, vShift)] = RTW_CALLBACK(g_layerMain, UILayerMain, OnClicked_PlayerItems);
	// ����
	mShortcutList[MAKE_SHORTCUT('R', vAlt, vCtrl, vShift)] = RTW_CALLBACK(g_layerMain, UILayerMain, OnClicked_System_Skill);
	// �е�ͼ
	mShortcutList[MAKE_SHORTCUT('M', vAlt, vCtrl, vShift)] = RTW_CALLBACK(g_layerMain, UILayerMain, OnClicked_System_Map);
	// �����ͼ
	//mShortcutList[MAKE_SHORTCUT('N', vAlt, vCtrl, vShift)] = RTW_CALLBACK(g_layerMain, UIFormMiniMap, OnButtonShowWorldMap);
	// ����
	mShortcutList[MAKE_SHORTCUT('Q', vAlt, vCtrl, vShift)] = RTW_CALLBACK(g_layerMain, UILayerMain, OnClicked_Attribute_task);
	// ���
	mShortcutList[MAKE_SHORTCUT('T', vAlt, vCtrl, vShift)] = RTW_CALLBACK(g_layerMain, UILayerMain, OnClicked_TeamApply);
	// ����
	mShortcutList[MAKE_SHORTCUT('F', vAlt, vCtrl, vShift)] = RTW_CALLBACK(&(GetWorld()->m_Friend), CFriendWithUI, OpenUI);
	// ����
	mShortcutList[MAKE_SHORTCUT('P', vAlt, vCtrl, vShift)] = RTW_CALLBACK(g_layerMain, UILayerMain, OnClicked_HeroList);
	// ����
	mShortcutList[MAKE_SHORTCUT('X', vAlt, vCtrl, vShift)] = RTW_CALLBACK(g_layerMain, UILayerMain, OnClicked_System_System);
	// ����
	mShortcutList[MAKE_SHORTCUT('G', vAlt, vCtrl, vShift)] = RTW_CALLBACK(g_layerMain, UILayerMain, OnClicked_Attribute_union);
	// �ʼ�
	mShortcutList[MAKE_SHORTCUT('L', vAlt, vCtrl, vShift)] = RTW_CALLBACK(&(GetWorld()->m_Mail), CMailWithUI, OpenMailUI);

	mShortcutList[MAKE_SHORTCUT('Z', vAlt, vCtrl, vShift)] = RTW_CALLBACK(g_layerMain, UILayerMain, OnButtonPetDown);


	// �ڹ�
	//mShortcutList[MAKE_SHORTCUT('Z', true, vCtrl, vShift)] = RTW_CALLBACK(g_layerMain, UILayerMain, OnClicked_AutoFight);
	mShortcutList[MAKE_SHORTCUT('Z', true, vCtrl, vShift)] = RTW_CALLBACK(g_layerMain, UIFormMiniMap, SysBattleSetting);  //add by yz �����ر��ڹҿ�ݼ�
	// �̳�
	mShortcutList[MAKE_SHORTCUT('V', true, vCtrl, vShift)] = RTW_CALLBACK(g_layerMain, UILayerMain, OnClicked_ShopCenter);
	// ����
	mShortcutList[MAKE_SHORTCUT('H', true, vCtrl, vShift)] = RTW_CALLBACK(g_layerMain, UILayerMain, OnShowHelp);
	// ����
	mShortcutList[MAKE_SHORTCUT('L', true, vCtrl, vShift)] = RTW_CALLBACK(g_layerMain, UIFormMiniMap, OnSelectChildThread);
	//// ��ʾ
	//mShortcutList[MAKE_SHORTCUT('A', true, vCtrl, vShift)] = RTW_CALLBACK(g_layerMain, UILayerMain, OnClicked_RemindIcon);

	mShortcutList[MAKE_SHORTCUT('E', false, vCtrl, vShift)] = RTW_CALLBACK(g_layerMain, UILayerMain, OnSwitchWeapon);

	////// ��Ⱥ
	////mShortcutList[MAKE_SHORTCUT('W', vAlt, vCtrl, vShift)] = RTW_CALLBACK(g_layerMain, UILayerMain, OnClicked_Attribute_soc);

	//// GM
	//mShortcutList[MAKE_SHORTCUT('G', vAlt, vCtrl, vShift)] = RTW_CALLBACK(g_layerMain, UILayerMain, OnClicked_SysSetting_GM);
};
