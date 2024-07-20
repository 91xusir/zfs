#include "gc_include.h"
#include "ui_form_skill.h"
#include "gc_cursor.h"
#include "Timer_Hint.h"

RtwRect UIFormSkill::m_imgRect[2][8]= {RtwRect(297,65,349,126),RtwRect(350,65,402,126),RtwRect(456,3,508,64),RtwRect(403,3,455,64),
									  RtwRect(350,3,402,64),RtwRect(297,3,349,64),RtwRect(403,65,455,126),RtwRect(456,65,508,126),
									  RtwRect(703,6,872,264),RtwRect(3,271,172,529),RtwRect(528,6,697,264),RtwRect(353,6,522,264),
									  RtwRect(178,6,347,264),RtwRect(3,6,172,264),RtwRect(178,271,347,529),RtwRect(353,271,522,529)};

UIFormSkill::UIFormSkill()
{
    guard;
    m_pFrmThis = NULL;
    m_iLastClass = -1;
    m_iMetier = 0;
    m_iTopPosition = 0;
    m_bUseScroll = FALSE;

    //rt2_strncpy(g_strStaticBuffer+200, "fmborder_skill.fmskillall.tabskillall.fm", 2048-200);
    int l, m, c, y, x, ox, oy;
    for (c=0; c<SKILL_GRID_PAGE; c++)
    {
        rt2_strncpy(g_strStaticBuffer, "fmborder_skill.fmskillall.tabskillall.fm", 2048);
        switch (c)
        {
        case 0: rt2_strcat(g_strStaticBuffer, "first."); break;
        case 1: rt2_strcat(g_strStaticBuffer, "second."); break;
        }
        l = strlen(g_strStaticBuffer);
        for (y=0; y<SKILL_GRID_LINE; y++)
        {
            for (x=0; x<SKILL_GRID_ROW; x++)
            {
                m = y*SKILL_GRID_ROW+x+1;
                rt2_snprintf(g_strStaticBuffer+l, 200-l,"panskill%02d", m);
                m_pPanelMap[c][y][x]  = RTW_WIDGET(g_strStaticBuffer);
				rt2_snprintf(g_strStaticBuffer+l, 200-l,"panskill%02d.btnskill", m);
				m_pSkillMap[c][y][x]  = RTW_WIDGET_T(RtwButton, g_strStaticBuffer);
                rt2_snprintf(g_strStaticBuffer+l, 200-l,"panskill%02d.lbskilllev", m);
                m_pLevelMap[c][y][x]  = RTW_WIDGET(g_strStaticBuffer);
				rt2_snprintf(g_strStaticBuffer+l, 200-l,"panskill%02d.btnstradd", m);
				m_pAddButton[c][y][x] = RTW_WIDGET_T(RtwButton, g_strStaticBuffer);
				m_pAddButton[c][y][x]->ModifyFlags(0,wfPick);
				//m_pLevelMap[c][y][x]->SetParent(NULL);
				//m_pSkillMap[c][y][x]->AddChild(m_pLevelMap[c][y][x]);
				m_pLevelMap[c][y][x]->SetBorderColor(0xff7bb29a);
				m_pLevelMap[c][y][x]->SetBorderSize(1);
				m_pLevelMap[c][y][x]->ModifyFlags(0,wfPick);
				//m_pSkillMap[c][y][x]->SetBorderColor(0xffedf32f);
				//m_pSkillMap[c][y][x]->SetBorderSize(1);
                m_pSkillMap[c][y][x]->ModifyFlags(wfDragOut|wfHint|wfDClick, 0);
                m_pSkillMap[c][y][x]->EvLDClick		+= RTW_CALLBACK(this, UIFormSkill, OnLDClickTreeItem);
				m_pSkillMap[c][y][x]->EvRClick		+= RTW_CALLBACK(this, UIFormSkill, OnRDownSkillItem);
                m_pSkillMap[c][y][x]->EvHint		+= RTW_CALLBACK(this, UIFormSkill, OnGetTreeItemSkillHint);
                m_pSkillMap[c][y][x]->EvDrag		+= RTW_CALLBACK(this, UIFormSkill, OnDragSkillItem);
                m_pSkillMap[c][y][x]->EvDragEnd		+= RTW_CALLBACK(this, UIFormSkill, OnDragEndSkillItem);
				//m_pSkillMap[c][y][x]->EvMouseMove  += RTW_CALLBACK(this, UIFormSkill, OnMouseMove);

                m_pPanelMap[c][y][x]->Hide();
            }
        }
    }
	m_lbMeter1					= RTW_WIDGET_T(RtwLabel,"fmborder_skill.fmskillall.tabskillall.fmfirst.lbshowweapon");
	m_lbMetertree1				= RTW_WIDGET_T(RtwLabel,"fmborder_skill.fmskillall.tabskillall.fmfirst.lbskilltree");
	m_lbMeter2					= RTW_WIDGET_T(RtwLabel,"fmborder_skill.fmskillall.tabskillall.fmsecond.lbshowweapon");
	m_lbMetertree2				= RTW_WIDGET_T(RtwLabel,"fmborder_skill.fmskillall.tabskillall.fmsecond.lbskilltree");

	m_imgImage1 = g_workspace.getImageFactory()->createImage("ui\\textures\\24.tga");
	if(m_imgImage1)
		m_imgImage1->SetBlend(true);
	m_lbMeter1->SetBackgroundImage(m_imgImage1);

	m_imgImage3 = g_workspace.getImageFactory()->createImage("ui\\textures\\24.tga");
	if(m_imgImage3)
		m_imgImage3->SetBlend(true);
	m_lbMeter2->SetBackgroundImage(m_imgImage3);

	m_imgImage2 = g_workspace.getImageFactory()->createImage("ui\\textures\\25.tga");
	if(m_imgImage2)
		m_imgImage2->SetBlend(true);
	m_lbMetertree1->SetBackgroundImage(m_imgImage2);

	m_imgImage4 = g_workspace.getImageFactory()->createImage("ui\\textures\\25.tga");
	if(m_imgImage4)
		m_imgImage4->SetBlend(true);
	m_lbMetertree2->SetBackgroundImage(m_imgImage4);

	m_pSP = RTW_WIDGET("fmborder_skill.fmskillall.tabskillall.lbspnum");
	m_pPP = RTW_WIDGET("fmborder_skill.fmskillall.tabskillall.lbspnum1");
	m_pPanelClass[0] = RTW_WIDGET("fmborder_skill.fmskillall.tabskillall.fmfirst");
    m_pPanelClass[1] = RTW_WIDGET("fmborder_skill.fmskillall.tabskillall.fmsecond");

    m_pPanelClass[0]->EvShow += RTW_CALLBACK(this, UIFormSkill, OnClass1Show);
    m_pPanelClass[1]->EvShow += RTW_CALLBACK(this, UIFormSkill, OnClass2Show);


    RTW_WIDGET("fmborder_skill.fmskillall.tabskillall.fmthird")->EvShow += RTW_CALLBACK(this, UIFormSkill, OnClass3Show);
	RTW_WIDGET("fmborder_skill.fmskillall.tabskillall.fmhemline")->EvShow += RTW_CALLBACK(this, UIFormSkill, OnClass4Show);

// change [3/20/2009 tooth.shi]    RtScriptCsvFile csv;
	RtCsv csv;
	if(csv.LoadTableFile(R(RES_TABLE_SKILL_TREE)))
    {
        for (m=0; m<SKILL_GRID_MAT; m++)
        {
            oy = m*(3+SKILL_GRID_LINE)+2;
            for (c=0; c<SKILL_GRID_PAGE; c++)
            {
                ox = c*(1+SKILL_GRID_ROW);
                m_szBackground[m][c] = csv[oy-1][ox+1].SafeStr();
                for (y=0; y<SKILL_GRID_LINE; y++)
                {
                    for (x=0; x<SKILL_GRID_ROW; x++)
                    {
                        m_wMap[m][c][y][x] = csv[oy+y][ox+x].Word();
                    }
                }
            }
        }
    }else
    {
        ERR("Load [skill_tree.csv] Failed.\n");
    }

    m_pFrmThis = RTW_WIDGET("fmborder_skill");
//	m_pFrmThis->EvKey += RTW_CALLBACK(m_pFrmThis, RtwWidget, DefaultKeyDown);

// change [3/20/2009 tooth.shi]    m_ptrImage = new RtwImage("passive_mask");
	m_ptrImage = g_workspace.getImageFactory()->createImage("ui_texture/passive_mask.dds");
    m_ptrImage->SetBlend(true);
	m_ptrImage->getRenderImage()->GetImage()->m_Transparency = 100;
	m_ptrImage->getRenderImage()->GetImage()->m_BackTransparency = 155;
    m_ptrImage->getRenderImage()->GetImage()->eBlendDstFunc = RTGBLEND_SRCCOLOR;
    m_ptrImage->getRenderImage()->GetImage()->eBlendSrcFunc = RTGBLEND_ZERO;

	/* gao 2010.3.1
	技能面板的第三页相关数据
	*/
	string str = "fmborder_skill.fmskillall.tabskillall.fmthird";
	for (int j=0; j<SKILL_COMMON_NUM; j++)
	{
		rt2_sprintf(g_strStaticBuffer, "%s.panskill%02d", str.c_str(), j+1);
		m_pCommonSkill[j] = RTW_WIDGET(g_strStaticBuffer);
		rt2_sprintf(g_strStaticBuffer, "%s.panskill%02d.lbskilllev", str.c_str(), j+1);
		m_pCommonLev[j] = RTW_WIDGET(g_strStaticBuffer);
		rt2_sprintf(g_strStaticBuffer, "%s.panskill%02d.btnskill",str.c_str(), j+1);
		m_pCommonBtn[j] = RTW_WIDGET_T(RtwButton, g_strStaticBuffer);
		m_pCommonBtn[j]->ModifyFlags(wfDragOut|wfHint|wfDClick|wfDClick, 0);
		//m_pCommonBtn[j]->EvLDClick		+= RTW_CALLBACK(this, UIFormSkill, OnLDClickTreeItem);
		m_pCommonBtn[j]->EvRClick		+= RTW_CALLBACK(this, UIFormSkill, OnRDownSkillItem);
		m_pCommonBtn[j]->EvHint			+= RTW_CALLBACK(this, UIFormSkill, OnGetTreeItemSkillHint);
		m_pCommonBtn[j]->EvDrag			+= RTW_CALLBACK(this, UIFormSkill, OnDragSkillItem);
		m_pCommonBtn[j]->EvDragEnd		+= RTW_CALLBACK(this, UIFormSkill, OnDragEndSkillItem);
		m_pCommonLev[j]->SetBorderColor(0xff7bb29a);
		m_pCommonLev[j]->SetBorderSize(1);
		m_pCommonLev[j]->ModifyFlags(0,wfPick);
		//m_pCommonSkill[j]->Hide();
	}
	for (int j=0; j<SKILL_EXPRESSION_NUM; j++)
	{
		rt2_sprintf(g_strStaticBuffer, "%s.panskill%02d", str.c_str(), j+1 + SKILL_COMMON_NUM);
		m_pExpressionSkill[j] = RTW_WIDGET(g_strStaticBuffer);
		rt2_sprintf(g_strStaticBuffer, "%s.panskill%02d.lbskilllev", str.c_str(), j+1 + SKILL_COMMON_NUM);
		m_pExpressionLev[j] = RTW_WIDGET(g_strStaticBuffer);
		rt2_sprintf(g_strStaticBuffer, "%s.panskill%02d.btnskill",str.c_str(), j+1 + SKILL_COMMON_NUM);
		m_pExpressionBtn[j] = RTW_WIDGET_T(RtwButton, g_strStaticBuffer);
		m_pExpressionBtn[j]->ModifyFlags(wfDragOut|wfHint|wfDClick|wfDClick, 0);
		//m_pExpressionBtn[j]->EvLDClick		+= RTW_CALLBACK(this, UIFormSkill, OnLDClickTreeItem);
		m_pExpressionBtn[j]->EvRClick		+= RTW_CALLBACK(this, UIFormSkill, OnRDownSkillItem);
		m_pExpressionBtn[j]->EvHint			+= RTW_CALLBACK(this, UIFormSkill, OnGetTreeItemSkillHint);
		m_pExpressionBtn[j]->EvDrag			+= RTW_CALLBACK(this, UIFormSkill, OnDragSkillItem);
		m_pExpressionBtn[j]->EvDragEnd		+= RTW_CALLBACK(this, UIFormSkill, OnDragEndSkillItem);
		m_pExpressionLev[j]->SetBorderColor(0xff7bb29a);
		m_pExpressionLev[j]->SetBorderSize(1);
		m_pExpressionLev[j]->ModifyFlags(0,wfPick);
		//m_pExpressionSkill[j]->Hide();
	}
	for (int j=0; j<SKILL_BOOK_NUM; j++)
	{
		rt2_sprintf(g_strStaticBuffer, "%s.panskill%02d", str.c_str(), j+1 + SKILL_COMMON_NUM + SKILL_EXPRESSION_NUM);
		m_pBookSkill[j] = RTW_WIDGET(g_strStaticBuffer);
		rt2_sprintf(g_strStaticBuffer, "%s.panskill%02d.lbskilllev", str.c_str(), j+1 + SKILL_COMMON_NUM + SKILL_EXPRESSION_NUM);
		m_pBookLev[j] = RTW_WIDGET(g_strStaticBuffer);
		rt2_sprintf(g_strStaticBuffer, "%s.panskill%02d.btnskill",str.c_str(), j+1 + SKILL_COMMON_NUM + SKILL_EXPRESSION_NUM);
		m_pBookBtn[j] = RTW_WIDGET_T(RtwButton, g_strStaticBuffer);
		m_pBookBtn[j]->ModifyFlags(wfDragOut|wfHint|wfDClick|wfDClick, 0);
		//m_pBookBtn[j]->EvLDClick		+= RTW_CALLBACK(this, UIFormSkill, OnLDClickTreeItem);
		m_pBookBtn[j]->EvRClick			+= RTW_CALLBACK(this, UIFormSkill, OnRDownSkillItem);
		m_pBookBtn[j]->EvHint			+= RTW_CALLBACK(this, UIFormSkill, OnGetTreeItemSkillHint);
		m_pBookBtn[j]->EvDrag			+= RTW_CALLBACK(this, UIFormSkill, OnDragSkillItem);
		m_pBookBtn[j]->EvDragEnd		+= RTW_CALLBACK(this, UIFormSkill, OnDragEndSkillItem);
		m_pBookLev[j]->SetBorderColor(0xff7bb29a);
		m_pBookLev[j]->SetBorderSize(1);
		m_pBookLev[j]->ModifyFlags(0,wfPick);
		//m_pBookSkill[j]->Hide();
	}
	for (int j=0; j<SKILL_TRUMP_NUM; j++)
	{
		rt2_sprintf(g_strStaticBuffer, "%s.panskill%02d", str.c_str(), j+1 + SKILL_COMMON_NUM + SKILL_EXPRESSION_NUM + SKILL_BOOK_NUM);
		m_pTrumpSkill[j] = RTW_WIDGET(g_strStaticBuffer);
		rt2_sprintf(g_strStaticBuffer, "%s.panskill%02d.lbskilllev", str.c_str(), j+1 + SKILL_COMMON_NUM + SKILL_EXPRESSION_NUM + SKILL_BOOK_NUM);
		m_pTrumpLev[j] = RTW_WIDGET(g_strStaticBuffer);
		rt2_sprintf(g_strStaticBuffer, "%s.panskill%02d.btnstradd",str.c_str(), j+1 + SKILL_COMMON_NUM + SKILL_EXPRESSION_NUM + SKILL_BOOK_NUM);
		m_pTrumpAdd[j] = RTW_WIDGET_T(RtwButton, g_strStaticBuffer);
		m_pTrumpAdd[j]->ModifyFlags(0,wfPick);
		rt2_sprintf(g_strStaticBuffer, "%s.panskill%02d.btnskill",str.c_str(), j+1 + SKILL_COMMON_NUM + SKILL_EXPRESSION_NUM + SKILL_BOOK_NUM);
		m_pTrumpBtn[j] = RTW_WIDGET_T(RtwButton, g_strStaticBuffer);
		m_pTrumpBtn[j]->ModifyFlags(wfDragOut|wfHint|wfDClick|wfDClick, 0);
		m_pTrumpBtn[j]->EvLDClick		+= RTW_CALLBACK(this, UIFormSkill, OnLDClickTreeItem);
		m_pTrumpBtn[j]->EvRClick		+= RTW_CALLBACK(this, UIFormSkill, OnRDownSkillItem);
		m_pTrumpBtn[j]->EvHint			+= RTW_CALLBACK(this, UIFormSkill, OnGetTreeItemSkillHint);
		m_pTrumpBtn[j]->EvDrag			+= RTW_CALLBACK(this, UIFormSkill, OnDragSkillItem);
		m_pTrumpBtn[j]->EvDragEnd		+= RTW_CALLBACK(this, UIFormSkill, OnDragEndSkillItem);
		m_pTrumpLev[j]->SetBorderColor(0xff7bb29a);
		m_pTrumpLev[j]->SetBorderSize(1);
		m_pTrumpLev[j]->ModifyFlags(0,wfPick);
		//m_pTrumpSkill[j]->Hide();
	}
	m_btnClose = RTW_WIDGET_T(RtwButton, "fmborder_skill.btnclose");
	m_btnClose->EvLClick += RTW_CALLBACK(this, UIFormSkill, OnHideSkillPane);
    OnResize();
    Refresh();
    m_pFrmThis->Hide();
    unguard;
}

UIFormSkill::~UIFormSkill()
{
    guard;
    if (g_layerMain)
    {
        g_layerMain->ClearShortcut();
    }
#ifdef _DEBUG
    CHECK(m_listCreateButton.size()==0);
#endif

	map<string,RtwImage*>::iterator it;
	for(it= m_mapIcon.begin();it!= m_mapIcon.end();it++)
	{
		DROP_TRUI_IMAGE((*it).second);
	}
	m_mapIcon.clear();
    unguard;
}

void UIFormSkill::SetWidgetLabelName(int Metier)
{
	/* gao 2009.12.22 
	此处保留 设置label的功能
	*/
	GcActor* pActor = GetPlayer();
	if (pActor==NULL) return;
	GcActorSkill* pActorSkill = &(pActor->m_Skill);

	//m_iMetier = pActor->m_core.Metier - 1;
	m_iMetier = Metier - 1;
	if (m_iMetier<0 || m_iMetier>3) m_iMetier = 0;
	int iBase;
	switch (m_iMetier)
	{
	default:
	case 0: iBase = 1; break;
	case 1: iBase = 4; break;
	case 2: iBase = 10; break;
	case 3: iBase = 7; break;
	}
	RtwWidget* pWidget;
	char szLev[32];
	int c;
	for (c=0; c<SKILL_GRID_PAGE; c++)
	{
		rt2_sprintf(szLev, "SMSG_CLASS_NAME_%d", iBase+c);
		switch (c)
		{
		case 0: pWidget = RTW_WIDGET("fmborder_skill.fmskillall.tabskillall.btnfirst"); break;
		case 1: pWidget = RTW_WIDGET("fmborder_skill.fmskillall.tabskillall.btnsecond"); break;
		case 2: pWidget = RTW_WIDGET("fmborder_skill.fmskillall.tabskillall.btnthird"); break;
		}
		rt2_strncpy(szLev, Rs(szLev), 32);
		pWidget->SetText(szLev);
		rt2_strcat(szLev, R(CLASS));
		pWidget->SetHintText(szLev);
	}
	/* 设置功能到此结束 */
}

void UIFormSkill::OnResize()
{
    guard;
    char szSkillCtrl[] = "fmborder_skill.fmskillall.tabskillall.fmhemline";
    char strId[100];
    int i, j;

    rt2_sprintf(strId, "%s.scrolldialg", szSkillCtrl);
    m_pScrollBar = RTW_WIDGET_T(RtwScrollBar, strId);

    for (i=0; i<SKILL_LIST_SIZE; i++)
	{
	
        rt2_sprintf(strId, "%s.fmother.panskill%02d", szSkillCtrl, i+1);
        m_pWndSkillList[i] = RTW_WIDGET(strId);
        j = strlen(strId);
        rt2_strncpy(strId+j, ".btnskill", 100-j);		m_pWndSkillListBtn[i]			= RTW_WIDGET_T(RtwButton, strId);
        rt2_strncpy(strId+j, ".lbskillname", 100-j);	m_pWndSkillListName[i]			= RTW_WIDGET(strId);
		rt2_strncpy(strId+j, ".lbskillinfo1", 100-j);	m_pWndSkillListInfo1[i]			= RTW_WIDGET(strId);
		rt2_strncpy(strId+j, ".lbxmm3", 100-j);			m_pWndSkillListInfolb[i]		= RTW_WIDGET(strId);
		rt2_strncpy(strId+j, ".lbskillinfo2", 100-j);	m_pWndSkillListInfo2[i]			= RTW_WIDGET(strId);
        rt2_strncpy(strId+j, ".lbexpcount", 100-j);		m_pWndSkillListExp[i]			= RTW_WIDGET(strId);
        rt2_strncpy(strId+j, ".barexp", 100-j);			m_pWndSkillListExpProgress[i]	= RTW_WIDGET_T(RtwProgressBar, strId);
		rt2_strncpy(strId+j, ".lbok", 100-j);			m_pBtnForgetSkill[i]			= RTW_WIDGET_T(RtwButton,strId);
		rt2_strncpy(strId+j, ".lbflower", 100-j);		m_pWndSkillListExpbpf[i]		= RTW_WIDGET(strId); 
		rt2_strncpy(strId+j, ".lbxmm2", 100-j);			m_pWndSkillListExpTXT[i]		= RTW_WIDGET(strId);
		m_pWndSkillListBtn[i]->SetBorderColor(0xeec8c6aa);
		m_pWndSkillListBtn[i]->SetBorderSize(0);
        m_pWndSkillList[i]->EvLDClick       += RTW_CALLBACK(this, UIFormSkill, OnLDClickSkillItem);
        m_pWndSkillList[i]->EvHint			+= RTW_CALLBACK(this, UIFormSkill, OnGetHintTextSkillItem);
        m_pWndSkillListBtn[i]->EvHint		+= RTW_CALLBACK(this, UIFormSkill, OnGetHintTextSkillItem);
        m_pWndSkillListBtn[i]->EvDrag       += RTW_CALLBACK(this, UIFormSkill, OnDragSkillItem);
        m_pWndSkillListBtn[i]->EvDragEnd    += RTW_CALLBACK(this, UIFormSkill, OnDragEndSkillItem);
		m_pBtnForgetSkill[i]->EvLClick		+= RTW_CALLBACK(this, UIFormSkill, OnBtnForgetDown);
		m_pWndSkillListName[i]->SetText("");
		m_pWndSkillListExp[i]->SetText("");
		m_pWndSkillListInfo1[i]->SetText("");
		m_pWndSkillListInfo2[i]->SetText("");
		//m_pWndSkillListExpbpf[i]->SetText(""); 2010.1.26
		//m_pWndSkillListBtn[i]->GetParent()->Hide();
		m_pWndSkillList[i]->Hide();

    }

    m_pFrmThis->EvShow += RTW_CALLBACK(this, UIFormSkill, OnShow);
    m_pScrollBar->EvScroll += RTW_CALLBACK(this, UIFormSkill, OnScroll);

    unguard;
}
/* gao 2009.12.7
没有学会，但是能看到的技能的蒙板
*/
bool UIFormSkill::CanStudySkill(SSkill *pSkill,GcActorSkill* pActorSkill)
{
	bool bFail = false;
	if(pSkill->iSP<=GetPlayer()->m_core.SkillPoint)
	{
		SSkillNodeFilter* pNode;
		for (int i=0; i<pSkill->filterStudy.iCnt; i++)
		{
			pNode = pSkill->filterStudy.pNodes+i;
			switch (pNode->pMap->eType)
			{
			case ESFT_OI:
				{
					int iValue;
					switch (pNode->pMap->eFun)
					{
					case ESVF_G_LEVEL:          iValue = pActorSkill->GetLevel()          ; break;
					case ESVF_G_HP:             iValue = pActorSkill->GetHP()             ; break;
					case ESVF_G_MP:             iValue = pActorSkill->GetMP()             ; break;
					case ESVF_G_RIDE:           iValue = pActorSkill->IsRide()            ; break;
					case ESVF_G_WeaponType:     iValue = pActorSkill->GetWeaponType()     ; break;
					case ESVF_G_WeaponEquiped:  iValue = pActorSkill->GetWeaponEquiped()  ; break;
					case ESVF_G_TrumpEquiped:	iValue = pActorSkill->GetTrumpEquiped()  ; break;
					case ESVF_G_ModelID:        iValue = pActorSkill->GetModelID()        ; break;
					case ESVF_G_Cloth:          iValue = pActorSkill->GetCloth()          ; break;
					case ESVF_G_ClothLev:       iValue = pActorSkill->GetClothLev()       ; break;
					case ESVF_G_ArmGuard:       iValue = pActorSkill->GetArmGuard()       ; break;
					case ESVF_G_ArmGuardLev:    iValue = pActorSkill->GetArmGuardLev()    ; break;
					case ESVF_G_Shoes:          iValue = pActorSkill->GetShoes()          ; break;
					case ESVF_G_ShoesLev:       iValue = pActorSkill->GetShoesLev()       ; break;
					case ESVF_G_Weapon:         iValue = pActorSkill->GetWeapon()         ; break;
					case ESVF_G_WeaponLev:      iValue = pActorSkill->GetWeaponLev()      ; break;
					case ESVF_G_MaxMP:          iValue = pActorSkill->GetMaxMP()          ; break;
					case ESVF_G_Attack:         iValue = pActorSkill->GetAttack()         ; break;
					case ESVF_G_Dodge:          iValue = pActorSkill->GetDodge()          ; break;
					case ESVF_G_DamageMin:      iValue = pActorSkill->GetDamageMin()      ; break;
					case ESVF_G_DamageMax:      iValue = pActorSkill->GetDamageMax()      ; break;
					case ESVF_G_Armor:          iValue = pActorSkill->GetArmor()          ; break;
					case ESVF_G_Con:            iValue = pActorSkill->GetCon()            ; break;
					case ESVF_G_Str:            iValue = pActorSkill->GetStr()            ; break;
					case ESVF_G_Dex:            iValue = pActorSkill->GetDex()            ; break;
					case ESVF_G_Int:            iValue = pActorSkill->GetInt()            ; break;
					case ESVF_G_Hit:            iValue = pActorSkill->GetHit()            ; break;
					case ESVF_G_Metier:         iValue = pActorSkill->GetMetier()         ; break;
					case ESVF_G_MetierLev:      iValue = pActorSkill->GetMetierLev()      ; break;
					case ESVF_G_Faction:        iValue = pActorSkill->GetFaction()        ; break;
					case ESVF_G_Exp:            iValue = pActorSkill->GetExp()            ; break;
					case ESVF_G_AttrPoint:      iValue = pActorSkill->GetAttrPoint()      ; break;
					case ESVF_G_SkillPoint:     iValue = pActorSkill->GetSkillPoint()     ; break;
					case ESVF_G_AbsorbHp:       iValue = pActorSkill->GetAbsorbHp()       ; break;
					case ESVF_G_AbsorbMp:       iValue = pActorSkill->GetAbsorbMp()       ; break;
					case ESVF_G_RateGold:       iValue = pActorSkill->GetRateGold()       ; break;
					case ESVF_G_RateExp:        iValue = pActorSkill->GetRateExp()        ; break;
					case ESVF_G_AttSpeed:       iValue = pActorSkill->GetAttSpeed()       ; break;
					case ESVF_G_Critical:       iValue = pActorSkill->GetCritical()       ; break;
					case ESVF_G_ChangeBody:     iValue = pActorSkill->IsChangeBody()      ; break;
					case ESVF_G_IsHaveCallNpc:  iValue = pActorSkill->IsHaveCallNpc()  ; break;
						
					}
					switch (pNode->Param1.i)
					{
					case 1: // >
						if (!(iValue > pNode->Param2.i)) bFail = true;
						break;
					case 2: // <
						if (!(iValue < pNode->Param2.i)) bFail = true;
						break;
					case 3: // =
					case 4: // ==
						if (!(iValue == pNode->Param2.i)) bFail = true;
						break;
					case 5: // <=
						if (!(iValue <= pNode->Param2.i)) bFail = true;
						break;
					case 6: // >=
						if (!(iValue >= pNode->Param2.i)) bFail = true;
						break;
					case 16: // &
						if ((iValue & pNode->Param2.i)==0) bFail = true;
						break;
					}
				}
				break;
			case ESFT_OF:
				{
					float fValue;
					switch (pNode->pMap->eFun)
					{
					case ESVF_G_DamageRate:     fValue = pActorSkill->GetDamageRate()     ; break;
					}
					switch (pNode->Param1.i)
					{
					case 1: // >
						if (!(fValue > pNode->Param2.f)) bFail = true;
						break;
					case 2: // <
						if (!(fValue < pNode->Param2.f)) bFail = true;
						break;
					case 3: // =
					case 4: // ==
						if (!(fValue == pNode->Param2.f)) bFail = true;
						break;
					case 5: // <=
						if (!(fValue <= pNode->Param2.f)) bFail = true;
						break;
					case 6: // >=
						if (!(fValue >= pNode->Param2.f)) bFail = true;
						break;
					}
				}
				break;
			case ESFT_CI:
				{
					SSkill *pSkill1;
					switch (pNode->pMap->eFun)
					{
					case ESVF_G_NeedSkill:
						pSkill1 = Skill()->FindSkill(pNode->Param1.i);
						if (pSkill1)
							if (!pActorSkill->FindSkillBySubID(pSkill1->wSubID))
								bFail = true;
						break;
					}
				}
				break;
			case ESFT_CII:
				{
					switch (pNode->pMap->eFun)
					{
					case ESVF_G_NeedClassSP:
						if (pActorSkill->GetClassSP(pNode->Param1.i)<pNode->Param2.i)
							bFail = true;
						break;
					case ESVF_G_MetierNeed:
						if (pActorSkill->GetMetier()!=pNode->Param1.i || pActorSkill->GetMetierLev()<pNode->Param2.i)
							bFail = true;
						break;
					}
				}
				break;
			default:
				RtCoreLog().Info("SSkillFilter::IsSuccess 没有实现的函数类型\n");
				bFail = true;
				break;
			}
			if(bFail)
				break;
		}
	}else
		bFail = true;
	if(pSkill->iSoul > GetPlayer()->m_core.Soul)
		bFail = true;
	return bFail;
}
/* end */

void UIFormSkill::Refresh()
{
	guard;
    GcActor* pActor = GetPlayer();
    if (pActor==NULL) return;
	GcActorSkill* pActorSkill = &(pActor->m_Skill);
	bool bCanNotStudy = false;
    int i, j;
    SSkill* pAttr;
    SSkill *pSkill, *pSkill2, *pSkill3;
    char szLev[32];
    const char *szIcon = NULL;
    int c, y, x;
	/* 2010.1.27 gao
	重新设计此函数
	*/
	c = RTW_WIDGET_T(RtwTab, "fmborder_skill.fmskillall.tabskillall")->GetSelectedItem();
	// 当是第一页或第二页的时候
	if(c == 0 || c == 1)
	{
		for (y=0; y<SKILL_GRID_LINE; y++)
		{
			for (x=0; x<SKILL_GRID_ROW; x++)
			{
				pSkill = NULL;
				pSkill2 = NULL;
				szIcon = NULL;
				if (m_wMap[m_iMetier][c][y][x])
				{
					pSkill = Skill()->FindSkill(m_wMap[m_iMetier][c][y][x]);
					if (pSkill)
					{
						pSkill2 = pActor->m_Skill.FindSkillBySubID(pSkill->wSubID); // 如果已经学会，就显示
						if (pSkill2)
						{
							// 已经学会
							if(pSkill2->iLevel < pSkill->iMaxLevel)
							{
								pSkill3 = Skill()->FindNextLevelSkill(pSkill2->wID);
								bCanNotStudy = CanStudySkill(pSkill3,pActorSkill);
							}else
								bCanNotStudy = true;
							szIcon = pSkill2->szIcon;
							// 将技能等级输出到对应窗口
							m_pSkillMap[c][y][x]->Enable();
							m_pPanelMap[c][y][x]->SetUserData((void*)(pSkill2->wID));
							m_pSkillMap[c][y][x]->SetUserData((void*)(pSkill2->wID));
							rt2_sprintf(szLev, "%d", pSkill2->iLevel);
							m_pLevelMap[c][y][x]->Show();
							m_pLevelMap[c][y][x]->SetText(szLev);
							m_pLevelMap[c][y][x]->SetBackgroundColor(0xff000000);
							if(bCanNotStudy)
							{
								m_pSkillMap[c][y][x]->SetMaskColor(RtwPixel(0xb5000000));    
								m_pAddButton[c][y][x]->Hide();
							}else
							{
								m_pSkillMap[c][y][x]->SetMaskValue(0);
								m_pAddButton[c][y][x]->Show();
							}
							if (pSkill2->dwType==SKILL_TYPE_PASSIVE)
							{
								m_pSkillMap[c][y][x]->ModifyFlags(0,wfDragOut);
								m_pSkillMap[c][y][x]->SetMaskImage(m_ptrImage);
							}else
							{
								m_pSkillMap[c][y][x]->ModifyFlags(wfDragOut,0);
							}
						}else
						{
							// 还没有学会，但是能看见
							if (pSkill->filterSee.IsSuccess(&(pActor->m_Skill))==NULL)
							{
								bCanNotStudy = CanStudySkill(pSkill,pActorSkill);
								szIcon = pSkill->szIcon;
								m_pLevelMap[c][y][x]->Hide();
								m_pSkillMap[c][y][x]->Enable();
								m_pPanelMap[c][y][x]->SetUserData((void*)(pSkill->wID));
								m_pSkillMap[c][y][x]->SetUserData((void*)(pSkill->wID));
								m_pSkillMap[c][y][x]->ModifyFlags(0,wfDragOut);
								m_pSkillMap[c][y][x]->SetMaskImage((RtwImage*)NULL);
								if(bCanNotStudy)
								{
									m_pSkillMap[c][y][x]->SetMaskColor(RtwPixel(0xb5000000));    
									m_pSkillMap[c][y][x]->SetMaskValue(1);
									m_pAddButton[c][y][x]->Hide();
								}else
								{
									m_pSkillMap[c][y][x]->SetMaskValue(0);
									m_pAddButton[c][y][x]->Show();
								}
							}else
							{
								szIcon = "skill_hide.dds";
								m_pLevelMap[c][y][x]->Hide();
								m_pSkillMap[c][y][x]->Disable();
								m_pPanelMap[c][y][x]->SetUserData(NULL);
								m_pSkillMap[c][y][x]->SetUserData(NULL);
								m_pSkillMap[c][y][x]->ModifyFlags(0,wfDragOut);
								m_pAddButton[c][y][x]->Hide();
							}
						}
						// 切换技能图标
						RtwImage* pImage;
						map<string,RtwImage*>::iterator it;
						it = m_mapIcon.find(szIcon);
						if(it == m_mapIcon.end())
						{
							pImage = g_workspace.getImageFactory()->createImage(szIcon);
							if(pImage)
								m_mapIcon.insert(make_pair(szIcon,pImage));
						}else
							pImage = (*it).second;
						m_pSkillMap[c][y][x]->SetBackgroundImage(pImage);
					}else
					{
						szIcon = "skill_hide.dds";
						m_pSkillMap[c][y][x]->Disable();
						m_pPanelMap[c][y][x]->SetUserData(NULL);
						m_pSkillMap[c][y][x]->SetUserData(NULL);
					}
					m_pPanelMap[c][y][x]->Show();
				}else
				{
					m_pPanelMap[c][y][x]->Hide();
				}
			}
		}
	}else if(c == 2)
	{	
		int iCommonSkillNum = 0;
		int iFaceSkillNum = 0;
		int iBookSkillNum = 0;
		int iTrumpSkillNum = 0;
		pAttr = pActor->m_Skill.FirstSkill();
		while(pAttr)
		{
			if(pAttr->wClass == 17)
			{
				// 通用技能
				m_pCommonBtn[iCommonSkillNum]->Enable();
				m_pCommonBtn[iCommonSkillNum]->SetUserData((void*)(pAttr->wID));
				if (pAttr->dwType==SKILL_TYPE_PASSIVE)
				{
					m_pCommonBtn[iCommonSkillNum]->ModifyFlags(0,wfDragOut);
					m_pCommonBtn[iCommonSkillNum]->SetMaskImage(m_ptrImage);
				}else
				{
					m_pCommonBtn[iCommonSkillNum]->ModifyFlags(wfDragOut,0);
				}
				m_pCommonBtn[iCommonSkillNum]->Show();
				if(pAttr->iMaxLevel > 1)
				{
					rt2_sprintf(szLev, "%d", pAttr->iLevel);
					m_pCommonLev[iCommonSkillNum]->Show();
					m_pCommonLev[iCommonSkillNum]->SetText(szLev);
					m_pCommonLev[iCommonSkillNum]->SetBackgroundColor(0xff000000);
				}else
					m_pCommonLev[iCommonSkillNum]->Hide();

				szIcon = pAttr->szIcon;
				if(szIcon)
				{
					// 切换技能图标
					RtwImage* pImage;
					map<string,RtwImage*>::iterator it;
					it = m_mapIcon.find(szIcon);
					if(it == m_mapIcon.end())
					{
						pImage = g_workspace.getImageFactory()->createImage(szIcon);
						if(pImage)
							m_mapIcon.insert(make_pair(szIcon,pImage));
					}else
						pImage = (*it).second;
					m_pCommonBtn[iCommonSkillNum]->SetBackgroundImage(pImage);
				}
				iCommonSkillNum++;
			}else if(pAttr->wClass == 13)
			{
				// 法宝技能
				m_pTrumpBtn[iTrumpSkillNum]->Enable();
				m_pTrumpBtn[iTrumpSkillNum]->SetUserData((void*)(pAttr->wID));
				if (pAttr->dwType==SKILL_TYPE_PASSIVE)
				{
					m_pTrumpBtn[iTrumpSkillNum]->ModifyFlags(0,wfDragOut);
					m_pTrumpBtn[iTrumpSkillNum]->SetMaskImage(m_ptrImage);
				}else
				{
					m_pTrumpBtn[iTrumpSkillNum]->ModifyFlags(wfDragOut,0);
				}
				m_pTrumpBtn[iTrumpSkillNum]->Show();
				rt2_sprintf(szLev, "%d", pAttr->iLevel);
				m_pTrumpLev[iTrumpSkillNum]->Show();
				m_pTrumpLev[iTrumpSkillNum]->SetText(szLev);
				m_pTrumpLev[iTrumpSkillNum]->SetBackgroundColor(0xff000000);

				if(pAttr->iLevel < pAttr->iMaxLevel)
				{
					pSkill3 = Skill()->FindNextLevelSkill(pAttr->wID);
					bCanNotStudy = CanStudySkill(pSkill3,pActorSkill);
				}else
					bCanNotStudy = true;
				if(bCanNotStudy)
					m_pTrumpAdd[iTrumpSkillNum]->Hide();
				else
					m_pTrumpAdd[iTrumpSkillNum]->Show();
				szIcon = pAttr->szIcon;
				if(szIcon)
				{
					// 切换技能图标
					RtwImage* pImage;
					map<string,RtwImage*>::iterator it;
					it = m_mapIcon.find(szIcon);
					if(it == m_mapIcon.end())
					{
						pImage = g_workspace.getImageFactory()->createImage(szIcon);
						if(pImage)
							m_mapIcon.insert(make_pair(szIcon,pImage));
					}else
						pImage = (*it).second;
					m_pTrumpBtn[iTrumpSkillNum]->SetBackgroundImage(pImage);
				}
				iTrumpSkillNum++;
			}else if(pAttr->wClass == 14)
			{
				// 天书技能
				m_pBookBtn[iBookSkillNum]->Enable();
				m_pBookBtn[iBookSkillNum]->SetUserData((void*)(pAttr->wID));
				if (pAttr->dwType==SKILL_TYPE_PASSIVE)
				{
					m_pBookBtn[iBookSkillNum]->ModifyFlags(0,wfDragOut);
					m_pBookBtn[iBookSkillNum]->SetMaskImage(m_ptrImage);
				}else
				{
					m_pBookBtn[iBookSkillNum]->ModifyFlags(wfDragOut,0);
				}
				m_pBookBtn[iBookSkillNum]->Show();
				rt2_sprintf(szLev, "%d", pAttr->iLevel);
				m_pBookLev[iBookSkillNum]->Show();
				m_pBookLev[iBookSkillNum]->SetText(szLev);
				m_pBookLev[iBookSkillNum]->SetBackgroundColor(0xff000000);
				szIcon = pAttr->szIcon;
				if(szIcon)
				{
					// 切换技能图标
					RtwImage* pImage;
					map<string,RtwImage*>::iterator it;
					it = m_mapIcon.find(szIcon);
					if(it == m_mapIcon.end())
					{
						pImage = g_workspace.getImageFactory()->createImage(szIcon);
						if(pImage)
							m_mapIcon.insert(make_pair(szIcon,pImage));
					}else
						pImage = (*it).second;
					m_pBookBtn[iBookSkillNum]->SetBackgroundImage(pImage);
				}
				iBookSkillNum++;
			}
			while (pAttr)
			{
				pAttr = pActor->m_Skill.NextSkill();
				if (pAttr==0 || pAttr->wClass==17 || pAttr->wClass==13 || pAttr->wClass==14)
				{
					break;
				}
			}
		}
		// 将其余技能按钮置为disable
		while(iCommonSkillNum < SKILL_COMMON_NUM)
		{
			m_pCommonBtn[iCommonSkillNum]->Disable();
			m_pCommonBtn[iCommonSkillNum]->Hide();
			m_pCommonBtn[iCommonSkillNum]->SetBackgroundImage((RtwImage*)NULL);
			m_pCommonBtn[iCommonSkillNum]->ModifyFlags(0,wfDragOut);
			m_pCommonLev[iCommonSkillNum]->Hide();
			iCommonSkillNum++;
		}
		while(iFaceSkillNum < SKILL_EXPRESSION_NUM)
		{
			m_pExpressionBtn[iFaceSkillNum]->Disable();
			m_pExpressionBtn[iFaceSkillNum]->Hide();
			m_pExpressionBtn[iFaceSkillNum]->SetBackgroundImage((RtwImage*)NULL);
			m_pExpressionBtn[iFaceSkillNum]->ModifyFlags(0,wfDragOut);
			m_pExpressionLev[iFaceSkillNum]->Hide();
			iFaceSkillNum++;
		}
		while(iTrumpSkillNum < SKILL_TRUMP_NUM)
		{
			m_pTrumpBtn[iTrumpSkillNum]->Disable();
			m_pTrumpBtn[iTrumpSkillNum]->Hide();
			m_pTrumpBtn[iTrumpSkillNum]->SetBackgroundImage((RtwImage*)NULL);
			m_pTrumpBtn[iTrumpSkillNum]->ModifyFlags(0,wfDragOut);
			m_pTrumpLev[iTrumpSkillNum]->Hide();
			m_pTrumpAdd[iTrumpSkillNum]->Hide();
			iTrumpSkillNum++;
		}
		while(iBookSkillNum < SKILL_BOOK_NUM)
		{
			m_pBookBtn[iBookSkillNum]->Disable();
			m_pBookBtn[iBookSkillNum]->Hide();
			m_pBookBtn[iBookSkillNum]->SetBackgroundImage((RtwImage*)NULL);
			m_pBookBtn[iBookSkillNum]->ModifyFlags(0,wfDragOut);
			m_pBookLev[iBookSkillNum]->Hide();
			iBookSkillNum++;
		}
	}else if(c == 3)
	{
		// 第四页的处理
		// 跳过 m_iTopPosition 个技能
		pAttr = pActor->m_Skill.FirstSkill();

		int iCount = 0;
		j = 0;
		while (j<m_iTopPosition && pAttr)
		{
			pAttr = pActor->m_Skill.NextSkill();
			if (pAttr==NULL)
			{
				break;
			}
			if (pAttr->wClass==15 || pAttr->wClass == 18 || pAttr->wClass == 19)
			{
				j ++;
				iCount ++;
			}
		}
		if (j<m_iTopPosition)
		{
			j = 0;
			iCount = 0;
			m_iTopPosition = 0;
			pAttr = pActor->m_Skill.FirstSkill();
		}

		while (pAttr!=0 && pAttr->wClass!=15 && pAttr->wClass != 18 && pAttr->wClass != 19)
		{
			pAttr = pActor->m_Skill.NextSkill();
		}

		i = 0;
		while (i<SKILL_LIST_SIZE && pAttr)
		{	

			m_pWndSkillList[i]->Show();
			m_pWndSkillList[i]->SetUserData((void*)pAttr->wID);
			m_pWndSkillListBtn[i]->SetUserData((void*)pAttr->wID);

			// Icon

			// change [3/20/2009 tooth.shi]        m_pWndSkillListBtn[i]->SetBackgroundImage(new RtwImage(pAttr->szIcon));
			m_pWndSkillListBtn[i]->SetBackgroundImage(g_workspace.getImageFactory()->createImage(pAttr->szIcon));

			// 是否可以拖动(被动技能不能拖动)
			if (pAttr->dwType==SKILL_TYPE_PASSIVE)
			{
				m_pWndSkillListBtn[i]->SetMaskImage(m_ptrImage);
				m_pWndSkillListBtn[i]->ModifyFlags(0,wfDragOut);
				m_pWndSkillListName[i]->SetTextColor(RtwPixel(0x80808080));
				m_pWndSkillListInfo1[i]->SetTextColor(RtwPixel(0x80808080));
				m_pWndSkillListInfo2[i]->SetTextColor(RtwPixel(0x80808080));
				m_pWndSkillListInfolb[i]->Hide();
				rt2_sprintf(g_strStaticBuffer, "%d", pAttr->iLevel);
			}else
			{
				RtwComplexImage complexImage;
				m_pWndSkillListBtn[i]->SetMaskImage(complexImage);
				m_pWndSkillListBtn[i]->ModifyFlags(wfDragServer|wfDragOut|wfHint|wfDClick,0);
				m_pWndSkillListName[i]->SetTextColor(RtwPixel(0xFFFFFFFF));
				m_pWndSkillListInfo1[i]->SetTextColor(RtwPixel(0xFFFFFFFF));
				m_pWndSkillListInfo2[i]->SetTextColor(RtwPixel(0xFFFFFFFF));
				m_pWndSkillListInfolb[i]->Show();
				rt2_sprintf(g_strStaticBuffer, "%d", pAttr->iMP);
				m_pWndSkillListInfo2[i]->SetText(g_strStaticBuffer);
				rt2_sprintf(g_strStaticBuffer, "%d", pAttr->iLevel);
			}

			// Name & Info
			m_pWndSkillListName[i]->SetText(pAttr->szName);
			m_pWndSkillListInfo1[i]->SetText(g_strStaticBuffer);
			rt2_sprintf(g_strStaticBuffer, "%d", pAttr->iLevel);
			//m_pWndSkillListExpbpf[i]->SetText("Exp");//exp重新赋值 2010.1.26

			CSkillInstance* pSkillInst = pActor->m_Skill.FindActiveSkill(pAttr->wID);
			if (pSkillInst && pAttr->wClass != 18)
			{
				int iExp;
				char szExp[20];

				if (pAttr->iLevelUpExp)
				{
					iExp = pSkillInst->m_wExp*100/pAttr->iLevelUpExp;
				}else
				{
					iExp = 100;
				}
				rt2_sprintf(szExp, "%d%%", iExp);
				m_pWndSkillListExp[i]->Show();
				m_pWndSkillListExp[i]->SetText(szExp);
				m_pWndSkillListExpProgress[i]->Show();
				m_pWndSkillListExpProgress[i]->SetValue(iExp/100.f);
				m_pWndSkillListExpbpf[i]->Show();
				m_pWndSkillListExpTXT[i]->Show();
			}else
			{
				m_pWndSkillListExp[i]->Hide();
				m_pWndSkillListExpProgress[i]->Hide();
				m_pWndSkillListExpbpf[i]->Hide();
				m_pWndSkillListExpTXT[i]->Hide();
			}
			if(pAttr->wClass != 18 && pAttr->wClass != 19)
			{
				m_pBtnForgetSkill[i]->Show();
			}else
				m_pBtnForgetSkill[i]->Hide();

			iCount ++;
			i ++;
			while (pAttr)
			{
				pAttr = pActor->m_Skill.NextSkill();
				if (pAttr==0 || pAttr->wClass==15 || pAttr->wClass == 18 || pAttr->wClass == 19)
				{
					break;
				}
			}
		}

		for (; i<SKILL_LIST_SIZE; i++)
		{
			m_pWndSkillList[i]->Hide();
		}

	}
	// 剩余sp点
    rt2_sprintf(szLev, "%d", pActor->m_core.SkillPoint);
    m_pSP->SetText(szLev);
	rt2_sprintf(szLev,"%d",pActor->m_core.VigourPoint);
	m_pPP->SetText(szLev);

	unguard;
}

void UIFormSkill::UpdateSkillExp(short wID, short wExp, short wLevUpExp)
{
    int iExp;
    char szExp[20];
    int i = 0;
    while (i<SKILL_LIST_SIZE)
    {
        if ((short)m_pWndSkillList[i]->GetUserData()==wID)
        {
            if (wLevUpExp)
            {
                iExp = wExp*100/wLevUpExp;
            }else
            {
                iExp = 100;
            }
            rt2_sprintf(szExp, "%d%%", iExp);
            m_pWndSkillListExp[i]->SetText(szExp);
            m_pWndSkillListExpProgress[i]->SetValue(iExp/100.f);
            break;
        }
        i ++;
    }
}

void UIFormSkill::OnShow(void *,void *)
{
	guard;
	int c;
	c = RTW_WIDGET_T(RtwTab, "fmborder_skill.fmskillall.tabskillall")->GetSelectedItem();

	if(IsShowing())
	{
		Refresh();
	}

	
	switch(c)
	{
	case 0:
		OnClass1Show(NULL, NULL);
		break;
	case 1:
		OnClass2Show(NULL,NULL);
		break;
	default:

		break;
	}
// 李泽华 2010.4.7
/////////////////////////////////////////////////////////////
	/* gao 2009.12.17
	Refresh()中此处已经刷新，所以此处就注释了
	*/
    /*RtwTab* pTab = RTW_WIDGET_T(RtwTab, "fmborder_skill.fmskillall.tabskillall");
    switch (pTab->GetSelectedItem())
    {
    default:
        pTab->SetSelectedItem(0);
    case 0: OnClass1Show(0, 0); break;
    case 1: OnClass2Show(0, 0); break;
    case 2: OnClass3Show(0, 0); break;
    }*/
	/* end */
	unguard;
}

void UIFormSkill::OnClass1Show(void *,void *)
{
	guard; 
	Refresh();
    int iBackup = m_iLastClass;
    m_iLastClass = 0;
    if (iBackup!=m_iLastClass)
    {
		if(m_imgImage1)
			m_imgImage1->SetSourceRect(m_imgRect[0][m_iMetier*2]);
		if(m_imgImage2)
			m_imgImage2->SetSourceRect(m_imgRect[1][m_iMetier*2]);
    }
	m_pPanelClass[m_iLastClass]->Show();
	unguard;
}

void UIFormSkill::OnClass2Show(void *,void *)
{
	guard;
	Refresh();
    int iBackup = m_iLastClass;
    m_iLastClass = 1;
    if (iBackup!=m_iLastClass)
    {
		if(m_imgImage3)
			m_imgImage3->SetSourceRect(m_imgRect[0][(m_iMetier*2)+1]);
		if(m_imgImage4)
			m_imgImage4->SetSourceRect(m_imgRect[1][(m_iMetier*2)+1]);
    }
	m_pPanelClass[m_iLastClass]->Show();
	GetWorld()->ShowSkillPoint();
	unguard;
}

void UIFormSkill::OnClass3Show(void *,void *)
{
	guard;
	Refresh();
	// 2010.1.27 gao 将原第三页技能更改
//    int iBackup = m_iLastClass;
//    m_iLastClass = 2;
//    if (iBackup!=m_iLastClass)
//    {
//// change [3/20/2009 tooth.shi]        m_pPanelClass[m_iLastClass]->SetBackgroundImage(new RtwImage(m_szBackground[m_iMetier][m_iLastClass].c_str()));
//		m_pPanelClass[m_iLastClass]->SetBackgroundImage(g_workspace.getImageFactory()->createImage(m_szBackground[m_iMetier][m_iLastClass].c_str()));
//    }
//    if (m_iLastClass>=0 && m_iLastClass!=iBackup)
//    {
//        m_pPanelClass[iBackup]->SetBackgroundImage((RtwImage*)NULL);
//    }
	unguard;
}

void UIFormSkill::OnClass4Show(void *,void *)
{
	guard;
	Refresh();
	// 2010.1.27 gao 将原第四页生活技能技能更改
	int iBackup = m_iLastClass;
	//m_iLastClass = 2;
	//if (iBackup!=m_iLastClass)
	//{
	//	// change [3/20/2009 tooth.shi]        m_pPanelClass[m_iLastClass]->SetBackgroundImage(new RtwImage(m_szBackground[m_iMetier][m_iLastClass].c_str()));
	//	m_pPanelClass[m_iLastClass]->SetBackgroundImage(g_workspace.getImageFactory()->createImage(m_szBackground[m_iMetier][m_iLastClass].c_str()));
	//}
	//if (m_iLastClass>=0 && m_iLastClass!=iBackup)
	//{
	//	m_pPanelClass[iBackup]->SetBackgroundImage((RtwImage*)NULL);
	//}
	unguard;
}

void UIFormSkill::OnScroll(RtwWidget* sender,void *p2)
{
	guard;

    if (m_pScrollBar==sender)
    {
        if (m_bUseScroll)
        {
            m_iTopPosition = m_pScrollBar->GetValue();
            Refresh();
        }
    }

	unguard;
}

void UIFormSkill::Show()
{
    guard;
	//玩家首次打开技能页面的新手提示
	GetWorld()->ShowSkillPage();
    Refresh();
    m_pFrmThis->Show();
    unguard;
}

bool UIFormSkill::IsShowing()
{
    guard;
    if ( m_pFrmThis && m_pFrmThis->GetFlags(wfVisible) )	return true;
    else                                                    return false;
    unguard;
}

void UIFormSkill::Hide()
{
    guard;
    if (m_pFrmThis) m_pFrmThis->Hide();
    unguard;
}

// -1 表示没有找到，否则返回相对的索引[0 base]
int UIFormSkill::GetSkillItemIdx(const RtString& szStr)
{
	guard;

    for (int i=0; i<SKILL_LIST_SIZE; i++)
    {
        if (m_pWndSkillList[i]->GetID().c_str()==szStr)
        {
            return i;
        }
    }
    return -1;

	unguard;
}

/* gao 2009.12.19
在技能栏内右击释放技能
*/
void UIFormSkill::OnRDownSkillItem(RtwWidget* sender, void*)
{
	guard;

	GcActor* pPlayer = GetPlayer();
	if (pPlayer==NULL) return;

	RtwButton* pButton = (RtwButton*)sender;
	const void* tmpShort = NULL;
	unsigned short wSkillID = 0;
	tmpShort = pButton->GetUserData();
	if(tmpShort)
		wSkillID = (unsigned short)tmpShort;
	else
		return;

	SSkill* pAttr = Skill()->FindSkill(wSkillID);
	if (pAttr==NULL) return;

	pAttr = pPlayer->m_Skill.FindSkillBySubID(pAttr->wSubID);
	if (pAttr==NULL) return;

	if(pAttr->dwType != SKILL_TYPE_PASSIVE)
	{
		//if(GetWorld()->CanUseSkill(pPlayer,pAttr,true))
		{
			if (pAttr->dwRTarget==SKILL_TARGET_NONE) // 没有目标，这种技能是直接使用的
			{
				CSkillInstance* pSkill = pPlayer->m_Skill.FindActiveSkillByName(pAttr->szName);
				if (pSkill)
				{
					if (pSkill->CanUse(&(pPlayer->m_Skill), SKILL_TARGET_NONE, &(pPlayer->m_Skill), NULL, true))
					{
						if (pAttr->dwRTarget==SKILL_TARGET_ITEM_BAG)
						{
							((CGameClientFrame*)GetApp())->SetMouseCapture(cti_Decompose, GAME_CURSOR_DEITEM, pAttr, R(MSG_SKILL_CLICK_README));

						}else
						{
							pPlayer->UseSelfSkill(0, pAttr->wID, rtRandom());
						}
					}else
					{
						rt2_sprintf(g_strStaticBuffer, R(SMSG_CANNOT_USER_SKILL), pSkill->GetErrInfo());
						PlayerFailOperate(g_strStaticBuffer);
					}
				}
			}else
				GetWorld()->UseSkill(pAttr,g_workspace.GetMousePosX(),g_workspace.GetMousePosY());
		}
	}
	unguard;
}
/* end */

void UIFormSkill::OnLDClickSkillItem(RtwWidget* sender, void*)
{
    //int iIdx = GetSkillItemIdx(sender->GetID());
    //if (iIdx<0) return;
}

void UIFormSkill::OnLClickSkillItem(RtwWidget* sender, void*)
{
    //int iIdx = GetSkillItemIdx(sender->GetID());
    //if (iIdx<0) return;
}

void UIFormSkill::OnGetHintTextSkillItem(RtwWidget* sender, void*)
{
	guard;

	int iLine=0;
    GcActor* pActor = GetPlayer();
    if (pActor==NULL) return;
    SSkill* pSkill = Skill()->FindSkill((unsigned short)sender->GetUserData());
    const char* pTxt = GenerateSkillHtmlHint(pSkill, pSkill->wID, true, false, iLine, NULL);
    if (iLine)
    {
        g_workspace.SetHintSize(180, 100);
		g_workspace.SetHintText(pTxt, true);
        g_workspace.AutoHintSize();
		g_workspace.AutoHintPosition();
    }else
    {
		g_workspace.SetHintText("");
    }

	unguard;
}

void UIFormSkill::OnDragSkillItem(RtwWidget* sender, void*)
{
	guard;

    RtwWidget* pDragSvr = g_workspace.GetDragServer();
    if (!pDragSvr) return;

	unguard;
}

void UIFormSkill::OnDragEndSkillItem(RtwWidget* sender, void*)	// sender is the skill button
{
	guard;
    if ( g_workspace.GetDragClient() )
    {
        char strId[32];
        RtwButton* pButton = NULL;
        GcActor* pActor = GetPlayer();
        if (pActor==NULL) return;
        GcSkill* pSkill = (GcSkill*)pActor->m_Skill.FindActiveSkill((unsigned short)sender->GetUserData());
		RtwRect rcClient;
		RtwRect rcHotKey,rcHotKey1,rcHotKey2;
		RtwRect rcSmallHotKey;
		bool	bIsInHotKey= false;
		int		iInHotKey = -1;
		rcHotKey =	RTW_WIDGET("fmhotkey")->GetFrameRect();
		rcHotKey1 =	RTW_WIDGET("fmshortcut_bar1")->GetFrameRect();
		rcHotKey2 =	RTW_WIDGET("fmshortcut_bar2")->GetFrameRect();
		if(rcHotKey.IsContained(g_workspace.GetMousePosX(), g_workspace.GetMousePosY())
			|| rcHotKey1.IsContained(g_workspace.GetMousePosX(), g_workspace.GetMousePosY())
			|| rcHotKey2.IsContained(g_workspace.GetMousePosX(), g_workspace.GetMousePosY()))
		{
			for(int i= 0;i< UI_HOT_KEY_COUNT;++i)
			{
				rcSmallHotKey = g_layerMain->m_hotKeyAlias[i]->GetFrameRect();
				if(rcSmallHotKey.IsContained(g_workspace.GetMousePosX(), g_workspace.GetMousePosY()))
				{
					bIsInHotKey = true;
					iInHotKey = i;
					break;
				}
			}
		}
		if(!bIsInHotKey)
		{
			g_workspace.SetDragClient(NULL);
			return;
		}
       if (pSkill)
        {
			// 先进行遍历一下快捷键，是否已经存在此快捷键，如果有，删除原来的 // gao 2010.3.30
			for(int i= 0;i< UI_HOT_KEY_COUNT;++i)
			{
				if(!g_layerMain->m_hotKeyAlias[i]->GetButton())
					continue;
				DWORD *pDWord = (DWORD*)g_layerMain->m_hotKeyAlias[i]->GetButton()->GetUserData();
				if(pDWord[1] == pSkill->m_pAttr->wID)
				{
					if(iInHotKey == i)
						return;
					else
						g_layerMain->m_hotKeyAlias[i]->SetButton(NULL);
				}
			}
            rt2_sprintf(strId, "CommonSkillButton_%03d", pSkill->m_pAttr->wID);
            pButton = (RtwButton*)(m_pFrmThis->GetChildWidget(strId));
            if (pButton==NULL)
            {
// change [3/20/2009 tooth.shi] 				pButton = (RtwButton*)sender->Clone(NULL, strId);
// 				pButton->ModifyFlags(RtwButton::wfDefault | wfHint | wfDragClient, 0);
				pButton = (RtwButton*)g_workspace.getWidgetFactory()->createWidget(wtButton, strId);
				pButton->CloneFrom(sender);
				pButton->ModifyFlags(wfDragOut|wfDragIn|wfHint|wfDClick|wfGrabMouse, 0);
				DWORD *pDWord = RT_NEW DWORD[2];
				pDWord[0] = UIHotKey_Skill;
				pDWord[1] = pSkill->m_pAttr->wID;
				pButton->SetBorderColor(0xee000000);
				pButton->SetBorderSize(0);
				pButton->SetUserData(pDWord);
//                pButton->SetUserData((void*)pSkill->m_pAttr->wID);

                pButton->EvClose       += RTW_CALLBACK(this, UIFormSkill, OnSkillItemButtonClose);
				//pButton->EvLClick      += RTW_CALLBACK(this, UIFormSkill, OnLClickCommonButton);
				pButton->EvRClick	   += RTW_CALLBACK(this, UIFormSkill, OnLClickCommonButton);//gao 2009.12.21 此处有问题第一次移动可以，重登陆就不可以了
                pButton->EvHint		   += RTW_CALLBACK(this, UIFormSkill, OnGetHintTextCommonButton);
				
                pSkill->SetUIButton(pButton);
#ifdef _DEBUG
                m_listCreateButton.push_back(pButton);
#endif
            }
			g_workspace.SetDragServer(pButton);
			if(g_workspace.getMouseHover())
			{
				if(g_workspace.getMouseHover()->getWidgetType() == wtAliasButton)
					return;
				//此处 由于当设置一次按钮之后，此处按钮会将原快捷键盖住，特添加此处理 gao 2010.3.30
				rcClient =	g_workspace.getMouseHover()->GetFrameRect();
				if(rcHotKey.IsContained(rcClient.left, rcClient.top)
					|| rcHotKey1.IsContained(rcClient.left, rcClient.top)
					|| rcHotKey2.IsContained(rcClient.left, rcClient.top))
				{
					for(int i= 0;i< UI_HOT_KEY_COUNT;++i)
					{
						rcSmallHotKey = g_layerMain->m_hotKeyAlias[i]->GetFrameRect();
						if(rcSmallHotKey.IsContained(rcClient.left, rcClient.top))
						{
							if(g_layerMain->m_hotKeyAlias[i]->GetButton())
							{
								g_layerMain->m_hotKeyAlias[i]->SetButton(pButton);
								break;
							}
						}
					}
				}
			}
			g_workspace.SetDragClient(NULL);
		}else
        {
            CHECKEX("UIFormSkill: SetUserData() error");
        }
        return;
    }

	unguard;
}

void UIFormSkill::OnLClickCommonButton(RtwWidget* sender, void*)
{
	guard;

	if(!GetWorld()->CanUseShortcut())
		return;
    GcActor* pPlayer = GetPlayer();
    if (pPlayer==NULL) return;

    RtwButton* pButton = (RtwButton*)sender;
	DWORD *pDWord = (DWORD *)pButton->GetUserData();
	if (!pDWord) return;
	unsigned short wSkillID = pDWord[1];

    SSkill* pAttr = Skill()->FindSkill(wSkillID);
    if (pAttr==NULL) return;

	vector<unsigned short>::iterator it;
	it= find(m_vecSkillSubID.begin(),m_vecSkillSubID.end(),pAttr->wSubID);
	// 当技能subid在禁用列表里面的时候 返回
	if(it!= m_vecSkillSubID.end())
		return;
    pAttr = pPlayer->m_Skill.FindSkillBySubID(pAttr->wSubID);
    if (pAttr==NULL) return;

    if (pAttr->dwRTarget==SKILL_TARGET_NONE) // 没有目标，这种技能是直接使用的
    {
        CSkillInstance* pSkill = pPlayer->m_Skill.FindActiveSkillByName(pAttr->szName);
        if (pSkill)
        {
            if (pSkill->CanUse(&(pPlayer->m_Skill), SKILL_TARGET_NONE, &(pPlayer->m_Skill), NULL, true))
            {
                if (pAttr->dwRTarget==SKILL_TARGET_ITEM_BAG)
                {
					((CGameClientFrame*)GetApp())->SetMouseCapture(cti_Decompose, GAME_CURSOR_DEITEM, pAttr, R(MSG_SKILL_CLICK_README));

                }else
                {
                    pPlayer->UseSelfSkill(0, pAttr->wID, rtRandom());
                }
            }else
            {
                rt2_sprintf(g_strStaticBuffer, R(SMSG_CANNOT_USER_SKILL), pSkill->GetErrInfo());
                PlayerFailOperate(g_strStaticBuffer);
            }
        }
    }else
    {
		//RtwAliasButton* pActive = (RtwAliasButton*)g_workspace.LookupWidget("fmhotkey.lbhotkeyenable");
  //      if(!pButton || !pActive) return;
 	//	char strId[32];
		//RtwButton* pButtonlb = NULL;
		//GcSkill* pSkill = (GcSkill*)GetWorld()->m_pPlayer->m_Skill.FindActiveSkillBySubID(pAttr->wSubID);
//		if(pSkill)
//		{
// 			if (pAttr)
// 			{
// 				sprintf(strId, "CommonSkillButton_%03d", pAttr->wID);
// 				pButtonlb = (RtwButton*)g_workspace.getWidgetFactory()->createWidget(wtButton, strId);
//  				pButtonlb->CloneFrom(sender);
//  				pButtonlb->ModifyFlags(wfDragOut|wfDragIn|wfHint|wfDClick|wfGrabMouse, 0);
//  				DWORD *pDWord = new DWORD[2];
//  				pDWord[0] = UIHotKey_Skill;
//  				pDWord[1] = pAttr->wID;
//  				pButtonlb->SetUserData(pDWord);
//				pSkill->SetUIButton(pButtonlb);
//#ifdef _DEBUG
//				m_listCreateButton.push_back(pButtonlb);
//#endif
// 			}
//		}
  //      pActive->SetButton(pButtonlb);
		//pPlayer->m_Skill.SetActiveSkill(pAttr->wID);
		GetWorld()->UseSkill(pAttr,g_workspace.GetMousePosX(),g_workspace.GetMousePosY());
    }

	unguard;
}

void UIFormSkill::OnLDClickCommonButton(RtwWidget* sender, void*)
{
    LOG("UIFormSkill::OnLDClickCommonButton\n");
}

void UIFormSkill::OnSkillItemButtonClose(RtwWidget* sender, void*)
{
	guard;

    GcActor* pActor = GetPlayer();
    if (pActor)
    {
		DWORD *pDWord = (DWORD*)sender->GetUserData();
		if (!pDWord)
			return;
		DWORD skillID = pDWord[1];
		GcSkill* pSkill = (GcSkill*)pActor->m_Skill.FindActiveSkill(skillID);
        if (pSkill)
        {
            pSkill->SetUIButton(NULL);
			DEL_ARRAY(pDWord);
        }
    }
#ifdef _DEBUG
    m_listCreateButton.remove((RtwButton*)sender);
#endif
	unguard;
}

void UIFormSkill::OnGetHintTextCommonButton(RtwWidget* sender, void*)
{
	guard;

    int iLine=0;
    GcActor* pActor = GetPlayer();
    if (pActor==NULL) return;
	DWORD *pDWord = (DWORD*)sender->GetUserData();
	DWORD skillID = pDWord[1];
    SSkill* pAttr = Skill()->FindSkill(skillID);
    if (pAttr==NULL) return;
    SSkill* pSkill = (SSkill*)pActor->m_Skill.FindSkillBySubID(pAttr->wSubID);
	RtwHtmlView* hintWidget= (RtwHtmlView*)GetWorld()->GetHintWidget();
    if (pSkill)
    {
        const char* pTxt = GenerateSkillHtmlHint(pSkill, pSkill->wID, true, false, iLine, NULL);
		if(iLine)
			hintWidget->SetHtmlText(pTxt);
		else
			hintWidget->SetHtmlText("");
    }else
    {
		hintWidget->SetHtmlText(R(SMSG_OPSTR_CANNOT_SEE));
    }
	g_layerMain->m_TimerHint->m_eNowHintFor = WIDGET_SKILL_SHORTCUT;
	g_layerMain->m_TimerHint->m_wdtHintFor = sender;
	sender->SetHint(hintWidget);
	AutoResize(hintWidget,1);

	unguard;
}

const char* GenerateSkillHtmlHint(SSkill* pAttr, WORD wID, bool bStudied, bool bNextLev , int &iLine, const char* szAddinStr)
{
	guard;

    if (pAttr==NULL)
    {
        iLine = 1;
        return R(SMSG_HINT_SKILL_INEXISTENT);
    }

	char* pBuf = g_strStaticBuffer;
	memset(g_strStaticBuffer,0,sizeof(g_strStaticBuffer));
    const char* pTestOutput = NULL;
    int iLen, iTestLine = 0;
    SSkill* pNextLevel=NULL;

    if (bStudied)
    {
        if (bNextLev)
        {
            pNextLevel = Skill()->FindNextLevelSkill(wID);
        }
    }else
    {
        if (bNextLev)
        {
            pNextLevel = pAttr;
        }
    }
    if (pNextLevel)
    {
        pTestOutput = pNextLevel->filterStudy.IsSuccess(&(GetPlayer()->m_Skill), true, true, &iTestLine);
    }

    if (pAttr->dwType==SKILL_TYPE_PASSIVE)
    {
        // 名字 (被动技能)
        rt2_sprintf(pBuf, "<html><body><p align=\"center\"><FONT color=\"#808080\">%s (%s)</FONT></p>", pAttr->szName, R(G_SKILL_PASSIVE));
        iLine += 1;
    }else
    {
        rt2_sprintf(pBuf, "<html><body><p align=\"center\"><FONT color=\"#ffffff\">%s</FONT></p>", pAttr->szName);
        iLine += 1;
    }

	long parameter1 = 0,parameter2 = 0,parameter3 = 0,parameter4 = 0;
    if (bStudied)
    {
		// 等级
		rt2_snprintf(pBuf+strlen(pBuf), 2048-strlen(pBuf),"<P><FONT color=\"#ffffff\">%s %d / %d</FONT></P>",R(Level), pAttr->iLevel, pAttr->iMaxLevel);
		iLine += 1;
		if (pAttr->dwType!=SKILL_TYPE_PASSIVE)
        {
			if (pAttr->wClass == 13)
				rt2_snprintf(pBuf+strlen(pBuf), 2048-strlen(pBuf),"<P><FONT color=\"#A0FFFF\">%s %d</FONT></P>", R(SMSG_EXPEND_CA), (int)pAttr->ConsumeAnger);
			else
				rt2_snprintf(pBuf+strlen(pBuf), 2048-strlen(pBuf),"<P><FONT color=\"#A0FFFF\">%s %d</FONT></P>", R(SMSG_EXPEND_MP), pAttr->iMP);
            iLine += 1;
        }
		SSkill* pAttr2 = NULL;
		for(int i = 0;i< (pAttr->wManyStrSkillSub).size();i++)
		{
			pAttr2 = GetPlayer()->m_Skill.FindSkillBySubID(pAttr->wManyStrSkillSub[i]);
			if(pAttr2)
			{
				parameter1 += pAttr2->iRMaxDest;
				parameter2 += pAttr2->wARange;
				parameter3 += pAttr2->cAMaxObj;
				parameter4 += pAttr2->iRRTime;
			}
		}
		if(parameter3 > 0)
			parameter3--;
		// 简单说明
		rt2_snprintf(pBuf+strlen(pBuf),2048-strlen(pBuf),"<P><FONT color=\"#aa88dd\">%s</FONT></P>",pAttr->szSimpleDesc);
		iLine += 1;
		if(pAttr->dwType!=SKILL_TYPE_PASSIVE)
		{
			if(pAttr->iRMaxDest)
			{
				rt2_snprintf(pBuf+strlen(pBuf),2048-strlen(pBuf),"<P><FONT color=\"#aa88dd\">施放距离 %d</FONT></P>",pAttr->iRMaxDest + parameter1);
				iLine += 1;
			}
			if(pAttr->wARange)
			{
				rt2_snprintf(pBuf+strlen(pBuf),2048-strlen(pBuf),"<P><FONT color=\"#aa88dd\">作用范围 %d</FONT></P>",pAttr->wARange + parameter2);
				iLine += 1;
			}
			if(pAttr->cAMaxObj)
			{
				rt2_snprintf(pBuf+strlen(pBuf),2048-strlen(pBuf),"<P><FONT color=\"#aa88dd\">最大目标个数 %d</FONT></P>",pAttr->cAMaxObj + parameter3);
				iLine += 1;
			}
			if(pAttr->iRRTime)
			{
				rt2_snprintf(pBuf+strlen(pBuf),2048-strlen(pBuf),"<P><FONT color=\"#aa88dd\">冷却时间 %d秒</FONT></P>",parameter4 == 0 ? (int)(pAttr->iRRTime*0.001):(int)(parameter4*0.001));
				iLine += 1;
			}
		}
		rt2_snprintf(pBuf+strlen(pBuf),2048-strlen(pBuf),"%s",pAttr->szDescription);
        iLine += 2;
    }else
    {
        rt2_snprintf(pBuf+strlen(pBuf),2048-strlen(pBuf),"<P><FONT color=\"#ff0000\">%s</FONT></P>"
            "<P><FONT color=\"#ffffff\">%s%s %d</FONT></P>",
            R(SMSG_NOT_STUDY), R(G_TIPTOP),R(Level),pAttr->iMaxLevel);
        iLine += 2;
    }

    //iLen = strlen(pAttr->szSimpleDesc);
    //iLine += (iLen/29);

    //iLen = strlen(pAttr->szDescription);
    //iLine += (iLen/29);

    if (pNextLevel && pNextLevel->wClass != 14)
    {
        const char* pFormatNextLevel = R(SMSG_HINT_NEXT_LEVEL);
        if (pTestOutput && pTestOutput[0])
        {
			rt2_snprintf(pBuf+strlen(pBuf),2048-strlen(pBuf),"<P align=\"center\"><FONT color=\"#a00000\">下一等级</FONT></P>");
			iLine += 1;
			// 等级
			rt2_snprintf(pBuf+strlen(pBuf),2048-strlen(pBuf),"<P><FONT color=\"#ffffff\">%s %d / %d</FONT></P>",R(Level), pNextLevel->iLevel, pNextLevel->iMaxLevel);
			iLine += 1;
			if (pNextLevel->dwType!=SKILL_TYPE_PASSIVE)
			{
				if (pNextLevel->wClass == 13)
					rt2_snprintf(pBuf+strlen(pBuf), 2048-strlen(pBuf),"<P><FONT color=\"#A0FFFF\">%s %d</FONT></P>", R(SMSG_EXPEND_CA), (int)pNextLevel->ConsumeAnger);
				else
					rt2_snprintf(pBuf+strlen(pBuf),2048-strlen(pBuf), "<P><FONT color=\"#A0FFFF\">%s %d</FONT></P>", R(SMSG_EXPEND_MP), pNextLevel->iMP);
				iLine += 1;
			}

			// 简单说明
			rt2_snprintf(pBuf+strlen(pBuf),2048-strlen(pBuf),"<P><FONT color=\"#aa88dd\">%s</FONT></P>",pNextLevel->szSimpleDesc);
			iLine += 1;
			if(pAttr->dwType!=SKILL_TYPE_PASSIVE)
			{
				if(pNextLevel->iRMaxDest)
				{
					rt2_snprintf(pBuf+strlen(pBuf),2048-strlen(pBuf),"<P><FONT color=\"#aa88dd\">施放距离 %d</FONT></P>",pNextLevel->iRMaxDest + parameter1);
					iLine += 1;
				}
				if(pNextLevel->wARange)
				{
					rt2_snprintf(pBuf+strlen(pBuf),2048-strlen(pBuf),"<P><FONT color=\"#aa88dd\">作用范围 %d</FONT></P>",pNextLevel->wARange + parameter2);
					iLine += 1;
				}
				if(pNextLevel->cAMaxObj)
				{
					rt2_snprintf(pBuf+strlen(pBuf),2048-strlen(pBuf),"<P><FONT color=\"#aa88dd\">最大目标个数 %d</FONT></P>",pNextLevel->cAMaxObj + parameter3);
					iLine += 1;
				}
				if(pNextLevel->iRRTime)
				{
					rt2_snprintf(pBuf+strlen(pBuf),2048-strlen(pBuf),"<P><FONT color=\"#aa88dd\">冷却时间 %d秒</FONT></P>",parameter4 == 0 ? (int)(pAttr->iRRTime*0.001):(int)(parameter4*0.001));
					iLine += 1;
				}
			}
			rt2_snprintf(pBuf+strlen(pBuf),2048-strlen(pBuf),"%s",pNextLevel->szDescription);
			iLine += 2;

			rt2_snprintf(pBuf+strlen(pBuf),2048-strlen(pBuf),"<P align=\"center\"><FONT color=\"#a00000\">升级条件</FONT></P>");
			iLine += 1;
			if (pNextLevel->wClass == 13)
				rt2_snprintf(pBuf+strlen(pBuf),2048-strlen(pBuf),
				"<P><FONT color=\"#%s\">需要灵力 [%d]</FONT></P>"
				"%s",	//max
				((pNextLevel->iSoul>GetPlayer()->m_core.Soul)?"ff0000":"ffffff"), pNextLevel->iSoul,
				pTestOutput);
			else
				rt2_snprintf(pBuf+strlen(pBuf),2048-strlen(pBuf),
				"<P><FONT color=\"#%s\">需要SP [%d]</FONT></P>"
				"%s",	//max
				((pNextLevel->iSP>GetPlayer()->m_core.SkillPoint)?"ff0000":"ffffff"), pNextLevel->iSP,
				pTestOutput);
        }else
        {
            iLine += 2;
            rt2_snprintf(pBuf+strlen(pBuf),2048-strlen(pBuf),
                pFormatNextLevel,
                pNextLevel->szSimpleDesc,
                ((pNextLevel->iSP>GetPlayer()->m_core.SkillPoint)?"ff0000":"ffffff"), pNextLevel->iSP,
                "");
        }

        iLen = strlen(pNextLevel->szSimpleDesc);
        iLine += (iLen/29);
    }
    if (szAddinStr && pAttr->wClass != 14)
    {
        rt2_strcat(pBuf, szAddinStr);
    }
    rt2_strcat(pBuf, "</body></html>");

    return pBuf;

	unguard;
}

void UIFormSkill::OnLDClickTreeItem(RtwWidget* sender, void*)
{
	GetWorld()->HideGuideForm();

    GcActor* pActor = GetPlayer();
    if (pActor==NULL) return;
    unsigned short wSkillID = (unsigned short)sender->GetUserData();
    SSkill* pSkill = Skill()->FindSkill(wSkillID);
    if (pSkill && pSkill->wClass != 14 && pSkill->wClass != 17 && pSkill->wClass != 13)
    {
        if (!pActor->m_Skill.FindAllSkill(wSkillID))
        {
            if (pActor->m_Skill.StudySkill(wSkillID, false))
            {
                Refresh();
                return;
            }
            PlayerFailOperate(pActor->m_Skill.GetLastErrInfo());
            return;
        }else
        {
            // 查找下一级的技能
            pSkill = Skill()->FindNextLevelSkill(wSkillID);
            if (pSkill)
            {
                if (pActor->m_Skill.StudySkill(pSkill->wID, false))
                {
                    Refresh();
					return;
                }
                PlayerFailOperate(pActor->m_Skill.GetLastErrInfo());
                return;
            }
        }
    }
    // 失败
    PlayerFailOperate(NULL);
}
void UIFormSkill::OnBtnAddSkillDown(RtwWidget* sender, void*)
{
	guard;
	unguard;
}

void UIFormSkill::OnGetTreeItemSkillHint(RtwWidget* sender, void*)
{
    guard;
    int iLine=0;
    GcActor* pActor = GetPlayer();
    if (pActor==NULL) return;
    unsigned short wSkillID = (unsigned short)sender->GetUserData();
    SSkill* pSkill = Skill()->FindSkill(wSkillID);
    const char* pTxt;
	std::string spTxt;
    if (pSkill)
    {
        pTxt = NULL;
        bool bStudied;
        int iState = 0;

        bStudied = GetPlayer()->m_Skill.FindAllSkill(wSkillID);

        if (pActor->m_core.SkillPoint>0)
        {
            if (bStudied==false || pSkill->iLevel<pSkill->iMaxLevel)
            {
                iState |= 0x01; // 学习
            }
        }

        if (bStudied && pSkill->dwType!=SKILL_TYPE_PASSIVE)
        {
            iState |= 0x02; // 拖动
        }

        switch (iState)
        {
        default:
        case 0: // none
            iLine = 0;
            pTxt = GenerateSkillHtmlHint(pSkill, pSkill->wID, bStudied, true, iLine, NULL);
            break;
        case 1: // 学习
            iLine = 1;
            pTxt = GenerateSkillHtmlHint(pSkill, pSkill->wID, bStudied, true, iLine, R(SMSG_OPSTR_DCLICK));
            break;
        case 2: // 拖动
            iLine = 1;
            pTxt = GenerateSkillHtmlHint(pSkill, pSkill->wID, bStudied, true, iLine, R(SMSG_OPSTR_DRAG));
            break;
        case 3: // 学习 & 拖动
            iLine = 2;
            pTxt = GenerateSkillHtmlHint(pSkill, pSkill->wID, bStudied, true, iLine, R(SMSG_OPSTR_DRAG_DCLICK));
            break;
        }
    }else
    {
        iLine = 0;
    }
	if(iLine)
	{
		FILE* file = fopen( "hintShow.html", "w+" );
		fwrite(pTxt,strlen(pTxt),1,file);
		fclose(file);
	}
	g_layerMain->m_TimerHint->m_eNowHintFor = WIDGET_SKILL_TREE;
	g_layerMain->m_TimerHint->m_wdtHintFor = sender;
	RtwHtmlView* hintWidget= (RtwHtmlView*)GetWorld()->GetHintWidget();
	if(iLine)
		hintWidget->SetHtmlText(pTxt);
	else
		hintWidget->SetHtmlText(R(SMSG_OPSTR_CANNOT_SEE));
	sender->SetHint(hintWidget);
  	AutoResize(hintWidget,1);
	unguard;
}

void UIFormSkill::OnHideSkillPane(RtwWidget* sender, void*)
{
	GetWorld()->HideGuideForm();
	m_pFrmThis->Hide();
}

void UIFormSkill::OnMouseMove(RtwWidget* sender, void*)
{
	guard;
	unguard;
}
void UIFormSkill::OnBtnForgetDown(RtwWidget* sender, void*)
{
	guard;
	for (int i= 0;i< SKILL_LIST_SIZE;++i)
	{
		if(m_pBtnForgetSkill[i] == sender)
		{
			GcActor* pActor = GetPlayer();
			if (pActor==NULL) return;
			unsigned short wSkillID = (unsigned short)m_pWndSkillListBtn[i]->GetUserData();
			SSkill* pSkill = Skill()->FindSkill(wSkillID);

			CG_CmdPacket* pPacket = NetBeginWrite();
			pPacket->WriteShort(c2r_delete_life_skill);
			pPacket->WriteShort(wSkillID);
			char Skill[128];
			rt2_sprintf(Skill,R(MSG_SKILL_SINGELFORGET),pSkill->szName);
			PlayerFailOperate(Skill);
			NetSend(pPacket);
		}
	}
	unguard;
}
void UIFormSkill::OnRefreshShortcut()
{
	guard;
	CHECK(GetWorld()->m_pPlayer);

	SSkill* pAttr;
	DWORD *pDWord;
	GcSkill* pSkill;
	unsigned short wSkillID;
	for(int i= 0;i< UI_HOT_KEY_COUNT;++i)
	{
		if(g_layerMain->m_hotKeyAlias[i]->GetButton())
		{
			pDWord = (DWORD*)g_layerMain->m_hotKeyAlias[i]->GetButton()->GetUserData();
			if (!pDWord) continue;
			wSkillID = pDWord[1];

			pAttr = Skill()->FindSkill(wSkillID);
			if (pAttr==NULL) continue;
			pSkill = (GcSkill*)GetPlayer()->m_Skill.FindActiveSkillBySubID(pAttr->wSubID);
			if(pSkill)
			{
				pDWord[0] = UIHotKey_Skill;
				pDWord[1] = pSkill->m_pAttr->wID;
				pSkill->SetUIButton(g_layerMain->m_hotKeyAlias[i]->GetButton());
			}else
			{
				pDWord[0] = 0;
				pDWord[1] = NULL;
				g_layerMain->m_hotKeyAlias[i]->SetButton(NULL);
			}
		}
		//// 检查备用快捷键
		//if(g_layerMain->m_hotKeyGroup[i])
		//{
		//	pDWord  = (DWORD*)g_layerMain->m_hotKeyGroup[i]->GetUserData();
		//	if (!pDWord) continue;
		//	wSkillID = pDWord[1];

		//	pAttr = Skill()->FindSkill(wSkillID);
		//	if (pAttr==NULL) continue;
		//	if(pAttr->wSubID == pSkill->m_pAttr->wSubID)
		//	{
		//		pDWord[0] = UIHotKey_Skill;
		//		pDWord[1] = pSkill->m_pAttr->wID;
		//		return;
		//	}
		//}
	}
	unguard;
}

bool UIFormSkill::CreateIconForGameClient(string imgStr,RtwImage** pImage)
{
	guard;
	// 切换技能图标
	map<string,RtwImage*>::iterator it;
	it = m_mapIcon.find(imgStr.c_str());
	if(it == m_mapIcon.end())
	{
		*pImage = g_workspace.getImageFactory()->createImage(imgStr);
		if(*pImage)
			m_mapIcon.insert(make_pair(imgStr.c_str(),*pImage));
		else
			return false;
	}else
		*pImage = (*it).second;
	return true;
	unguard;
}

void UIFormSkill::OnRefreshCanNotUseSkill()
{
	guard;
	CHECK(GetWorld()->m_pPlayer);

	SSkill* pAttr;
	DWORD *pDWord;
	GcSkill* pSkill;
	unsigned short wSkillID;
	for(int i= 0;i< UI_HOT_KEY_COUNT;++i)
	{
		if(g_layerMain->m_hotKeyAlias[i]->GetButton())
		{
			pDWord = (DWORD*)g_layerMain->m_hotKeyAlias[i]->GetButton()->GetUserData();
			if (!pDWord) continue;
			wSkillID = pDWord[1];

			pAttr = Skill()->FindSkill(wSkillID);
			if (pAttr==NULL) continue;
			pSkill = (GcSkill*)GetPlayer()->m_Skill.FindActiveSkillBySubID(pAttr->wSubID);
			vector<unsigned short>::iterator it;
			it= find(m_vecSkillSubID.begin(),m_vecSkillSubID.end(),pAttr->wSubID);
			if(it== m_vecSkillSubID.end())
				pSkill->OnSetCanUse(true);
			else
				pSkill->OnSetCanUse(false);
		}
		//// 检查备用快捷键
		//if(g_layerMain->m_hotKeyGroup[i])
		//{
		//	pDWord  = (DWORD*)g_layerMain->m_hotKeyGroup[i]->GetUserData();
		//	if (!pDWord) continue;
		//	wSkillID = pDWord[1];

		//	pAttr = Skill()->FindSkill(wSkillID);
		//	if (pAttr==NULL) continue;
		//	if(pAttr->wSubID == pSkill->m_pAttr->wSubID)
		//	{
		//		pDWord[0] = UIHotKey_Skill;
		//		pDWord[1] = pSkill->m_pAttr->wID;
		//		return;
		//	}
		//}
	}
	unguard;
	unguard;
}

void UIFormSkill::OnAddCanNotUseSkill(unsigned short subID)
{
	guard;
	m_vecSkillSubID.push_back(subID);
	OnRefreshCanNotUseSkill();
	unguard;
}

void UIFormSkill::OnDelCanNotUseSkill(unsigned short subID)
{
	guard;
	vector<unsigned short>::iterator it;
	it= find(m_vecSkillSubID.begin(),m_vecSkillSubID.end(),subID);
	if(it== m_vecSkillSubID.end())
		return;
	m_vecSkillSubID.erase(it);
	OnRefreshCanNotUseSkill();
	unguard;
}
