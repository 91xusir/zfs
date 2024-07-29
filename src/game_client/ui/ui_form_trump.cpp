#include "gc_include.h"
#include "ui_form_trump.h"
#include "ui_form_playeritems.h"
#include "ui_form_skill.h"

CUIForm_Trump::CUIForm_Trump(void):
m_pfrmTrump(NULL),
m_plbTrumpLevel(NULL),
m_plbVigourPoint(NULL),
m_plbTrumpAttactType(NULL),
m_plbTrumpAttact(NULL),
m_pproActorSoul(NULL),
m_plbNowMoney(NULL)
{
	m_pfrmTrump				= LOAD_UI("fmborder");
	m_pbtnTrumpABut			= LOAD_UI_T(RtwButton, "fmborder.fmInterface.fmProps.btnBaby");
	m_plbTrumpLevel			= LOAD_UI("fmborder.fmInterface.fmProps.lbRank2");
	m_plbVigourPoint		= LOAD_UI("fmborder.fmInterface.fmProps.lbGenki2");
	m_plbTrumpAttactType	= LOAD_UI("fmborder.fmInterface.fmProps.lbAttack1.lbtext3");
	m_plbTrumpAttact		= LOAD_UI("fmborder.fmInterface.fmProps.lbAttack2");
	m_pproActorSoul			= LOAD_UI_T(RtwProgressBar,"fmborder.fmInterface.fmCurrently.barexp");
	m_plbNowMoney			= LOAD_UI("fmborder.fmInterface.fmCurrently.lbNow4");
	m_pproSoulRate			= LOAD_UI("fmborder.fmInterface.fmCurrently.lbtext9");

	m_pbtnTrumpABut->ModifyFlags(wfDragClient | wfDragServer | wfHint, 0);
	m_pbtnTrumpABut->EvDragEnd += RTW_CALLBACK(this, CUIForm_Trump, OnDragEndItem);
	m_pbtnTrumpABut->EvHint    += RTW_CALLBACK(this, CUIForm_Trump, OnGetHintText);

	m_pfrmTrump->EvHide	+= RTW_CALLBACK(this,CUIForm_Trump,OnClose);

	char tmpStr[255];
	int i;
	for(i = 0;i < MAX_PROGRESS_NUM; ++i)
	{
		if(i <= 5)
			rt2_sprintf(tmpStr,"fmborder.fmInterface.fmProperty.barexp%d",i+ 3);
		else
			rt2_sprintf(tmpStr,"fmborder.fmInterface.fmProperty.barexp%d",i- 5);
		m_pproTrumpPro[i] = LOAD_UI_T(RtwProgressBar,tmpStr);
		m_pproTrumpPro[i]->SetValue(0.f);
		m_pproTrumpPro[i]->SetText("");
	}

	for(i = 0;i < MAX_PROGRESS_NUM; ++i)
	{
		if(i <= 5)
			rt2_sprintf(tmpStr,"fmborder.fmInterface.fmProperty.lbtext%d",i+ 3);
		else
			rt2_sprintf(tmpStr,"fmborder.fmInterface.fmProperty.lbtext%d",i- 5);
		m_plbTrumpRate[i] = LOAD_UI_T(RtwProgressBar,tmpStr);
		m_plbTrumpRate[i]->SetText("");
	}

	for(i = 0;i < MAX_PROGRESS_NUM; ++i)
	{
		if(i <= 5)
			rt2_sprintf(tmpStr,"fmborder.fmInterface.fmProperty.lbPro%d",i+ 3);
		else
			rt2_sprintf(tmpStr,"fmborder.fmInterface.fmProperty.lbPro%d",i- 5);
			m_plbTrumpInfo[i] = LOAD_UI(tmpStr);
			m_plbTrumpInfo[i]->SetText("0%");
	}

	for(i = 0;i < MAX_PROGRESS_NUM; ++i)
	{ 
		if(i <= 5)
			rt2_sprintf(tmpStr,"fmborder.fmInterface.fmProperty.btnStart%d",i+ 3);
		else
			rt2_sprintf(tmpStr,"fmborder.fmInterface.fmProperty.btnStart%d",i- 5);
			m_pbtnTrumpButton[i] = LOAD_UI_T(RtwButton,tmpStr);
			m_pbtnTrumpButton[i]->EvLClick += RTW_CALLBACK(this,CUIForm_Trump,OnLClickRifine);


			if (i == PNEUMA_TRUMP)
			{
				continue;
			}
			m_pbtnTrumpButton[i]->Disable();
			m_pbtnTrumpButton[i]->SetTextColor(0xff808080);
	}

	m_iSpeed = 0;
	m_iRifine = -1;
	m_fGap = 0;
	m_bIsRifine = false;
	m_iLastGap = 0;
	m_lTime = 0;
	m_iValue = 0;
	m_bCanFull = false;
	m_interval = 0;
	m_iSoulNeed = 0;
}

CUIForm_Trump::~CUIForm_Trump(void)
{
}
void CUIForm_Trump::Show()
{
	if ( !IsVisiable() )
	{
		SItemID &trump = GetPlayer()->mItem.GetEquipItem(CItemContainerBase::TRUMP, true);
		if(ItemID_IsValid(trump))
			SetItemID(trump);
	}
	OnPreDraw();
	m_pfrmTrump->Show();
}
void CUIForm_Trump::Hide()
{
	m_pfrmTrump->Hide();
}

void CUIForm_Trump::OnLClickRifine(RtwWidget* sender, RtwEventDelegate* pEvent)
{
	guard;

	if (GetPlayer()->IsShapeshifted())
	{
		PlayerFailOperate(R(MSG_CANNOTRIFINE_WHEN_SHAPESHIFT));
		return;
	}

	for (int i = 0;i<MAX_PROGRESS_NUM;i++)
	{
		if (m_pbtnTrumpButton[i] == sender)
		{
			m_iRifine = i;
			if( m_iRifine == MP_TRUMP && m_itemID.params[TRUMP_LEV] >= GetPlayer()->m_core.GradeLev ) 
			{
				PlayerFailOperate(R(MSG_FABAO_NEEDBEYONDLEVEL));
				Reset();
				return;
			}
			//m_pbtnTrumpButton[i]->SetText(string("停止"));
			if ( i < 6 && m_itemID.params[TRUMP_ATTACH_TYPE] == -1)
			{
				SetAttachType(i);
			}
			//continue;
		}
		m_pbtnTrumpButton[i]->Disable();
		m_pbtnTrumpButton[i]->SetTextColor(0xff808080);
	}
	StartRifine();

	unguard;
}

void CUIForm_Trump::Reset()
{
	guard;
	GetPlayer()->AddDoCommandPose();
	m_bIsRifine = false;
	m_fGap = 0;
	m_iLastGap = 0;
	m_lTime = 0;
	//if ( m_iRifine == 7 )
	//{
	//	m_pbtnTrumpButton[m_iRifine]->SetText(string("提升"));
	//}else if ( m_iRifine == 6 )
	//{
	//	m_pbtnTrumpButton[m_iRifine]->SetText(string("转化"));
	//}else
	//	m_pbtnTrumpButton[m_iRifine]->SetText(string("注入"));
	m_iValue = 0;
	OnPreDraw();
	unguard;
}

void CUIForm_Trump::Tick()
{
	guard;

	// 控制速度为1秒
	float fPassTime = 1000.f;
	if ( m_lTime == 0 )
	{
		m_lTime = rtGetMilliseconds();
	}else
	{
		int now = rtGetMilliseconds();
		if (  now - m_lTime < 1000 )
		{
			return;
		}else
		{
			fPassTime = now - m_lTime;
			m_lTime = now;
		}
	}

	fPassTime /= 1000.f;
	ERR2("%f\n",fPassTime);
	float iSpeed = m_iSoulNeed / m_interval * fPassTime;

	char str[20];
	m_core = GetPlayer()->m_core;

	if ( m_fGap < m_iSoulNeed - iSpeed )
	{
		m_fGap += iSpeed;
	}
	float tic = ((float)m_core.Soul - m_fGap) / (float)m_core.GetSoulValueMax();
	if ( tic > 1.f || tic < 0.f)
	{
		Reset();
		return;
	}
	m_pproActorSoul->SetValue(tic);
	rt2_sprintf(str,"%d/%d",m_core.Soul - (int)m_fGap,m_core.GetSoulValueMax());
	m_pproSoulRate->SetText(str);

	iSpeed = m_iLastGap / m_interval * fPassTime;
	if ( m_iValue < m_iLastGap - iSpeed )
	{
		m_iValue += iSpeed;
	}
	tic = m_iValue / 100.f;
	m_pproTrumpPro[m_iRifine]->SetValue(tic);
	if (m_iValue > 100.f)
	{
		m_iValue = 100.f;
	}
	rt2_sprintf(str,"%d/100",(int)m_iValue);
	//m_plbTrumpRate[m_iRifine]->SetText(str);

	unguard;
}
void CUIForm_Trump::SetItemID(SItemID id) 
{ 
	guard;
	if (!ItemID_IsValid(id) || !ItemIsTrump(id.type))
	{
		return;
	}

	m_itemID = id; 
	GetPlayer()->m_RifineTrump = m_itemID;

	OnPreDraw();
	
	unguard;
}

char *CUIForm_Trump::GetRifineName(int attach)
{
	string attachType;
	switch (attach)
	{
	case ATTACK_FIRE:
		attachType = "火伤";
		break;
	case DEF_FIRE:
		attachType = "火防御";
		break;
	case ATTACK_WATER:
		attachType = "水伤";
		break;
	case DEF_WATER:
		attachType = "水防御";
		break;
	case ATTACK_POISON:
		attachType = "毒伤";
		break;
	case DEF_POISON:
		attachType = "毒防御";
		break;
	default:
		attachType = "";
	}
	size_t length = strlen(attachType.c_str())+1;
	char *str = RT_NEW char[length];
	rt2_strncpy(str,attachType.c_str(), length);
	return str;
}
void CUIForm_Trump::SetAttachType(int attach)
{
	string attachType;
	switch (attach)
	{
	case ATTACK_FIRE:
		attachType = "火伤";
		break;
	case DEF_FIRE:
		attachType = "火防御";
		break;
	case ATTACK_WATER:
		attachType = "水伤";
		break;
	case DEF_WATER:
		attachType = "水防御";
		break;
	case ATTACK_POISON:
		attachType = "毒伤";
		break;
	case DEF_POISON:
		attachType = "毒防御";
		break;
	default:
		attachType = "";
	}
	m_plbTrumpAttactType->SetText(attachType);
}

void CUIForm_Trump::StartRifine()
{
	guard;

	if (GetPlayer()->m_core.Soul <= 0)
	{
		if (m_iRifine == PNEUMA_TRUMP)
		{
			PlayerFailOperate(R(MSG_FABAO_NOSOUL));
		}
		else if (m_iRifine == MP_TRUMP)
		{
			PlayerFailOperate(R(MSG_FABAO_NOSOULTOLEVUP));
		}
		else
		{
			PlayerFailOperate(R(MSG_FABAO_NOTENOUGHSOUL));
		}
		Reset();
		return;
	}

	if ( !ItemID_IsValid(m_itemID) && m_iRifine != PNEUMA_TRUMP )
	{
		ItemID_SetInvalid(m_itemID);
		ItemID_SetInvalid(GetPlayer()->m_RifineTrump);
		Reset();
		return;
	}

	m_iSpeed = TRUMP_ActorSoulSec(m_itemID,m_iRifine);

	switch(m_iRifine)
	{
	case MP_TRUMP:
		m_iValue = m_itemID.params[TRUMP_MP];
		break;
	case PNEUMA_TRUMP:
		m_iValue = GetPlayer()->m_core.Vigour;
		break;
	default:
		m_iValue = m_itemID.params[TRUMP_ATTACH_MP];
		break;
	}

	float iSpeed = TRUMP_ActorSoulSec(m_itemID,m_iRifine);
	float rifinePointTime = TRUMP_CentiSoul(m_itemID,m_iRifine) / (float)iSpeed;
	float rifineLevupTime = rifinePointTime * float(100 - m_iValue);
	float actorSoulOverTime = GetPlayer()->m_core.Soul / iSpeed;

	if ( actorSoulOverTime < 0.f && rifineLevupTime < 0.f )
	{
		Reset();
		return;
	}

	if (actorSoulOverTime >= rifineLevupTime)
	{
		m_bCanFull = true;
		m_interval = rifineLevupTime;
		m_iSoulNeed = iSpeed * m_interval;
		m_iLastGap = 100 - m_iValue;
	}else{
		m_bCanFull = false;
		m_interval = actorSoulOverTime;
		m_iSoulNeed = GetPlayer()->m_core.Soul;
		m_iLastGap = GetPlayer()->m_core.Soul / TRUMP_CentiSoul(m_itemID,m_iRifine);
	}
	if( GetPlayer()->m_core.Soul < /*iSpeed*/TRUMP_CentiSoul(m_itemID,m_iRifine) * 100 )
	{
		if (m_iRifine == PNEUMA_TRUMP)
		{
			PlayerFailOperate(R(MSG_FABAO_NOSOUL));
		}
		else if (m_iRifine == MP_TRUMP)
		{
			PlayerFailOperate(R(MSG_FABAO_NOSOULTOLEVUP));
		}
		else
		{
			PlayerFailOperate(R(MSG_FABAO_NOTENOUGHSOUL));
		}
		Reset();
		return;
	}

	if (m_bIsRifine)
	{
		GetPlayer()->AddDoCommandPose();
		GetPlayer()->SendTrumpRifine(m_iRifine,true);
		Reset();
		OnPreDraw();
		return;
	}else
	{
		GetPlayer()->AddDoCommandPose(DO_COMMAND_RIFINE);
		GetPlayer()->SendTrumpRifine(m_iRifine);
	}

	m_bIsRifine = true;

	unguard;
}

void CUIForm_Trump::ReceiveCmd( int rifine, int succeed, int vigour,  int soul)
{
	guard;

	if (rifine != m_iRifine)
	{
		m_iRifine = rifine;
	}
	Reset();

	RtgVertex3 pos;

	GetPlayer()->m_core.Soul = soul;

	m_itemID = GetPlayer()->mItem.GetEquipItem(CItemContainerBase::TRUMP, true);
	if ( ItemID_IsValid(m_itemID) && ItemIsTrump(m_itemID) )
	{
		GetPlayer()->m_RifineTrump = m_itemID;
	}

	if (succeed)
	{
		if (m_iRifine == PNEUMA_TRUMP)
		{
			GetPlayer()->m_core.VigourPoint = vigour;
			GetPlayer()->m_core.Vigour = 0;
			PlayerFailOperate(R(MSG_FABAO_YUANQIUP));
			if(g_layerMain->m_fromSkill->IsShowing())
				g_layerMain->m_fromSkill->Refresh();
		}
		else if (m_iRifine < 6)
		{
			char str[64];
			char *rifineName = GetRifineName(m_iRifine);
			long tmp = m_iRifine % 2 ? 1 : 2;
			rt2_sprintf(str,R(MSG_FABAO_SPECIALLEVELUP),rifineName,tmp);
			if (rifineName)
			{
				DEL_ARRAY(rifineName);
				rifineName = 0;
			}
			PlayerFailOperate(str);
		}
		else if (m_iRifine == MP_TRUMP)
		{
			PlayerFailOperate(R(MSG_FABAO_LEVELUP));
		}
		g_pSoundMgr->PlayOnce("mw_succeed.wav", false, 0, pos);
	}
	else{
		if (m_iRifine == PNEUMA_TRUMP)
		{
			GetPlayer()->m_core.Vigour = vigour;
			PlayerFailOperate(R(MSG_FABAO_NOTCHANGE));
		}
		else if (m_iRifine < 6)
		{
			char str[64];
			char *rifineName = GetRifineName(m_iRifine);
			rt2_sprintf(str,R(MSG_FABAO_SPECIALFAIL),rifineName);
			if (rifineName)
			{
				DEL_ARRAY(rifineName);
				rifineName = 0;
			}
			PlayerFailOperate(str);
		}
		else if (m_iRifine == MP_TRUMP)
		{
			PlayerFailOperate(R(MSG_FABAO_LEVELUPFAIL));
		}
		g_pSoundMgr->PlayOnce("mw_failed.wav", false, 0, pos);
	}

	OnPreDraw();
	unguard;
}

bool CUIForm_Trump::IsVisiable()
{
	return m_pfrmTrump->IsVisible();
}

void CUIForm_Trump::DoFrame()
{
}

void CUIForm_Trump::OnClose(RtwWidget* sender, RtwEventDelegate* pEvent)
{
	guard;
	if(ItemID_IsValid(m_itemID))
	{
		m_pbtnTrumpABut->SetBackgroundImage((RtwImage*)NULL);
		m_pbtnTrumpABut->SetText("");
		ItemID_SetInvalid(m_itemID);
		ItemID_SetInvalid(GetPlayer()->m_RifineTrump);
	}
	for (int i=0;i<MAX_PROGRESS_NUM;i++)
	{
		if (i == PNEUMA_TRUMP)
		{
			continue;
		}
		m_pbtnTrumpButton[i]->Disable();
		m_pbtnTrumpButton[i]->SetTextColor(0xff808080);
		m_pproTrumpPro[i]->SetValue(0);
		//m_plbTrumpRate[i]->SetText("0/100");
		m_plbTrumpInfo[i]->SetText("0%");
	}

	if (m_bIsRifine)
	{
		GetPlayer()->SendTrumpRifine(m_iRifine,true);
	}
	Reset();
	unguard;
}

void CUIForm_Trump::OnPreDraw()
{
	guard;

	char str[100];
	rt2_sprintf(str,"%d",GetPlayer()->m_core.VigourPoint);
	m_plbVigourPoint->SetText(str);

	rt2_sprintf(str,"%d/%d",GetPlayer()->m_core.Soul, GetPlayer()->m_core.GetSoulValueMax());
	m_pproActorSoul->SetValue((float)GetPlayer()->m_core.Soul / (float)GetPlayer()->m_core.GetSoulValueMax());
	m_pproSoulRate->SetText(str);

	rt2_sprintf(str,"%d/%d",GetPlayer()->m_core.Vigour,100);
	m_pproTrumpPro[PNEUMA_TRUMP]->SetValue((float)GetPlayer()->m_core.Vigour / 100.f);
	//m_plbTrumpRate[PNEUMA_TRUMP]->SetText(str);

	SItemBase *pItemClass = NULL;
	m_itemID = GetPlayer()->mItem.GetEquipItem(CItemContainerBase::TRUMP, true);
	if (ItemID_IsValid(m_itemID))
	{
		pItemClass = GetWorld()->m_pItemManager->GetItem(m_itemID.type);
		if(!pItemClass)			return;
		m_pbtnTrumpABut->SetBackgroundImage( g_workspace.getImageFactory()->createImage(pItemClass->icon) );

		rt2_sprintf(str,"%d",m_itemID.params[TRUMP_LEV]);
		m_plbTrumpLevel->SetText(str);

		rt2_sprintf(str,"%d",m_itemID.params[TRUMP_ATTACH_POINT]);
		m_plbTrumpAttact->SetText(str);

		// 设置无效按钮
		for (int i=0;i<6;i++)
		{
			if ( i == m_itemID.params[TRUMP_ATTACH_TYPE] )
			{
				m_pbtnTrumpButton[i]->Enable();
				m_pbtnTrumpButton[i]->SetTextColor(0xffffffff);
				m_pproTrumpPro[i]->SetValue((float)m_itemID.params[TRUMP_ATTACH_MP] / 100.f);
				rt2_sprintf(str,"%d/100",m_itemID.params[TRUMP_ATTACH_MP]);
				//m_plbTrumpRate[i]->SetText(str);

				rt2_sprintf(str,"%d%%",m_itemID.params[TRUMP_ATTACH_RATE]);

				// 设置法宝附加属性炼化成功几率
				m_plbTrumpInfo[i]->SetText(str);
				SetAttachType(i);
				continue;
			}
			m_pproTrumpPro[i]->SetValue(0);
			//m_plbTrumpRate[i]->SetText(string("0/100"));
			if ( m_itemID.params[TRUMP_ATTACH_TYPE] != -1)
			{
				m_pbtnTrumpButton[i]->Disable();
				m_pbtnTrumpButton[i]->SetTextColor(0xff808080);
			}else
			{
				m_pbtnTrumpButton[i]->Enable();
				m_pbtnTrumpButton[i]->SetTextColor(0xffffffff);
				m_plbTrumpAttactType->SetText(string(""));
				m_plbTrumpAttact->SetText(string(""));
				m_plbTrumpInfo[i]->SetText(string("100%"));
			}
		}

		m_pbtnTrumpButton[MP_TRUMP]->Enable();
		m_pbtnTrumpButton[MP_TRUMP]->SetTextColor(0xffffffff);
		m_pproTrumpPro[MP_TRUMP]->SetValue((float)m_itemID.params[TRUMP_MP] / 100.f);
		rt2_sprintf(str,"%d/100",m_itemID.params[TRUMP_MP]);
		//m_plbTrumpRate[MP_TRUMP]->SetText(str);
		m_plbTrumpInfo[MP_TRUMP]->SetText(string("100%"));


	}else
	{
		m_pbtnTrumpABut->SetBackgroundImage((RtwImage*)NULL);
		m_pbtnTrumpABut->SetText("");

		m_plbTrumpLevel->SetText("");
		m_plbTrumpAttact->SetText("");
		m_plbTrumpAttactType->SetText("");
		for (int i = 0;i<MAX_PROGRESS_NUM;i++)
		{
			m_pbtnTrumpButton[i]->Disable();
			m_pbtnTrumpButton[i]->SetTextColor(0xff808080);
		}
	}

	m_pbtnTrumpButton[PNEUMA_TRUMP]->Enable();
	m_pbtnTrumpButton[PNEUMA_TRUMP]->SetTextColor(0xffffffff);
	m_plbTrumpInfo[PNEUMA_TRUMP]->SetText(string("100%"));

	int moneyTotal = GetPlayer()->mItem.m_Bag.GetMoney();
	rt2_sprintf(str,"%d",moneyTotal);
	m_plbNowMoney->SetText(str);

	unguard;
}

void CUIForm_Trump::Refresh()
{
	guard;
	if(m_pfrmTrump->IsVisible())
	{
		char str[20];
		if ( !m_bIsRifine )
		{
			rt2_sprintf(str,"%d/%d",GetPlayer()->m_core.Soul, GetPlayer()->m_core.GetSoulValueMax());
			m_pproActorSoul->SetValue((float)GetPlayer()->m_core.Soul / (float)GetPlayer()->m_core.GetSoulValueMax());
			m_pproSoulRate->SetText(str);
		}

		int moneyTotal = GetPlayer()->mItem.m_Bag.GetMoney();
		rt2_sprintf(str,"%d",moneyTotal);
		m_plbNowMoney->SetText(str);
		//OnPreDraw();
		//m_pfrmTrump->Show();
	}
	unguard;
}
void CUIForm_Trump::OnDragEndItem(RtwWidget* sender, RtwEventDelegate* pEvent)
{

}
void CUIForm_Trump::OnGetHintText(RtwWidget* sender, RtwEventDelegate* pEvent)
{
	guard;
	g_layerMain->m_fromPlayerItems->OnGetHintText_Parts(LOAD_UI_T(RtwButton, "fmitem.panItem.btnTrump"),NULL);
	unguard;
}