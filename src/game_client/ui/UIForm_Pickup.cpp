#include "gc_include.h"
#include "UIForm_Pickup.h"
#include "game_assistant.h"

CUIForm_Pickup::CUIForm_Pickup(void):
m_iNowPage(0)
{
	guard;
	m_pfrmPickup					= LOAD_UI("fmpickup");
	m_pbtnUppage					= LOAD_UI_T(RtwButton,"fmpickup.fmcontent.btnon");
	m_pbtnDownpage					= LOAD_UI_T(RtwButton,"fmpickup.fmcontent.btnnext");
	m_pbtnAllPickup					= LOAD_UI_T(RtwButton,"fmpickup.fmcontent.btnpickup");
	m_pbtnUppage->EvLClick			+= RTW_CALLBACK(this,CUIForm_Pickup,OnBtnOtherDown);
	m_pbtnDownpage->EvLClick		+= RTW_CALLBACK(this,CUIForm_Pickup,OnBtnOtherDown);
	m_pbtnAllPickup->EvLClick		+=RTW_CALLBACK(this,CUIForm_Pickup,OnBtnOtherDown);
	m_pfrmPickup->EvHide			+= RTW_CALLBACK(this,CUIForm_Pickup,InHide);
	m_ptrHoverImage					= g_workspace.getImageFactory()->createImage("ui/x_textures/highlight_button.tga");
	char name[80];
	for(int i= 0;i <4;++i)
	{
		memset(name,0,80);
		rt2_sprintf(name,"fmpickup.fmcontent.fmprops.lbprops%d",i);
		m_pfrmShowLabel[i]		= LOAD_UI(name);
		memset(name,0,80);
		rt2_sprintf(name,"fmpickup.fmcontent.fmprops.lbprops%d.btnbaby%d",i,i);
		m_pbtnItem[i]			= LOAD_UI_T(RtwButton,name);
		memset(name,0,80);
		rt2_sprintf(name,"fmpickup.fmcontent.fmprops.lbprops%d.lbbaby%d",i,i);
		m_plbItem[i]			= LOAD_UI_T(RtwLabel,name);
		m_pbtnItem[i]->ModifyFlags(wfHint | wfVisible | wfDClick | wfGrabMouse,0);
		m_pbtnItem[i]->EvLClick	+=RTW_CALLBACK(this,CUIForm_Pickup,OnBtnDown);
		m_pbtnItem[i]->EvHint	+=RTW_CALLBACK(this,CUIForm_Pickup,OnHintItem);
	}
	//SItemID tmpItem = GetWorld()->m_pItemManager->CreateItem(41);
	//OnInsertNewItem(tmpItem);
	//OnInsertNewItem(tmpItem);
	//OnInsertNewItem(tmpItem);
	//OnInsertNewItem(tmpItem);
	//OnInsertNewItem(tmpItem);
	//OnInsertNewItem(tmpItem);
	//OnInsertNewItem(tmpItem);
	//OnInsertNewItem(tmpItem);
	//OnInsertNewItem(tmpItem);
	m_bAotoClose = false;
	m_bPkPick = false;
	Hide();
	unguard;
}

CUIForm_Pickup::~CUIForm_Pickup(void)
{
	guard;
	vector<SItemID>::iterator it;
	//for(it = m_vocAllItemList.begin();it != m_vocAllItemList.end();++it)
	//	if((*it) != NULL)
	//		DEL_ONE((*it));
	m_vocAllItemList.clear();
	unguard;
}
/* gao 2010.1.22
��ǰ�����ĵĲ���˵��
1.ҳ���Ǵ�0ҳ��ʼ
2.����ʵ�ֵĹ����м�ȡһ�����ߺ���һ��������ǰ�ƶ�������Ӧ���������в�������
3.Ҳ���Ը�Ϊ��ʰȡ�󲻸ı䣬ɾ�����ڶ�Ӧ�ĵ��ߣ����������ٽ��в�������
4.���ʰȡ�󲻱䣬����ǰҳ��ȫ����Ʒʰȡ��Ҳ���Խ���ǰҳ�ĸ���Ʒȫ��ɾ��
*/
void CUIForm_Pickup::Show(bool pkpick/* = false*/)
{
	guard;
	m_bPkPick = pkpick;
	m_pfrmPickup->Show();
	int num = (int)m_vocAllItemList.size();
	int page = num/4;
	if(num % 4 == 0)
		page--;
	// ��ǰ�ǵ�0ҳ����һҳ��ť��Ч
	if(m_iNowPage == 0)
		m_pbtnUppage->Hide();
	else
		m_pbtnUppage->Show();
	// ���һҳ����һҳ��Ч
	if(m_iNowPage == page)
		m_pbtnDownpage->Hide();
	else
		m_pbtnDownpage->Show();
	// �˴�Ҫ�����Ӧ���ĸ����ߵ�Ҫ��ʾ��ҳ��ȥ
	SItemBase *pItemClass = NULL;
	for(int j = 0;j < 4;++j)
		m_pfrmShowLabel[j]->Hide();
	for(int i= 0;i <4;++i)
	{
		// �����ǰi�Ѿ�����������size˵����ǰҳ��ĵ��߲����ĸ�������ʾ���Ѿ���ʾ���
		if(i>= m_vocAllItemList.size()- m_iNowPage*4)
			break;
		// ���Ϊ�գ�˵����ǰû�е��ߣ�������ѱ�ʰȡ
		if(!ItemID_IsValid(m_vocAllItemList[m_iNowPage* 4+ i]))
		{
			RtwComplexImage hoverImg;
			m_pbtnItem[i]->SetHoverImage(hoverImg);
			continue;
		}
		pItemClass = GetWorld()->m_pItemManager->GetItem(m_vocAllItemList[m_iNowPage* 4+ i].type);
		// û�в��ҵ�����
		if(!pItemClass)
			continue;
		m_pfrmShowLabel[i]->Show();
		m_pbtnItem[i]->SetBackgroundImage( g_workspace.getImageFactory()->createImage(pItemClass->icon) );
		m_ptrHoverImage->getRenderImage()->SetTransparency(0.5);
		m_ptrHoverImage->getRenderImage()->eBlendDstFunc = RTGBLEND_ONE;
		m_ptrHoverImage->getRenderImage()->eBlendSrcFunc = RTGBLEND_SRCALPHA;
		m_ptrHoverImage->getRenderImage()->SetBlend(true);
		m_pbtnItem[i]->SetHoverImage(m_ptrHoverImage);

		if (ItemIsGreen(m_vocAllItemList[m_iNowPage* 4+ i]))
			m_plbItem[i]->SetTextColor(0xff00ff00);
		else if (ItemIsBlue(m_vocAllItemList[m_iNowPage* 4+ i]))
			m_plbItem[i]->SetTextColor(0xff0000ff);
		else if (ItemIsGolden(m_vocAllItemList[m_iNowPage* 4+ i]))
			m_plbItem[i]->SetTextColor(0xffffff00);
		else 
			m_plbItem[i]->SetTextColor(0xffffffff);
		m_plbItem[i]->SetText(pItemClass->name);
	}
	unguard;
}

void CUIForm_Pickup::Hide()
{
	guard;
	m_bPkPick = false;
	m_bAotoClose = true;
	m_pfrmPickup->Hide();
	m_bAotoClose = false;
	vector<SItemID>::iterator it;
	//for(it = m_vocAllItemList.begin();it != m_vocAllItemList.end();++it)
	//	if((*it) != NULL)
	//		DEL_ONE((*it));
	m_vocAllItemList.clear();
	for(int j = 0;j < 4;++j)
		m_pfrmShowLabel[j]->Hide();
	unguard;
}
void CUIForm_Pickup::InHide(RtwWidget* sender, void*)
{
	guard;
	if ( !m_bAotoClose && !m_bPkPick )
	{
		GcActor* pActor = FindAllActor(GetPlayer()->m_lPickActorID);
		if ( pActor && pActor->m_cIsDeadNet == 3 )
		{
			CG_CmdPacket* pPacket = NetBeginWrite();
			pPacket->WriteShort(c2r_close_pickwindow);
			pPacket->WriteLong(GetPlayer()->m_lPickActorID);
			NetSend(pPacket);
		}
	}
	m_pfrmPickup->Hide();
	vector<SItemID>::iterator it;
	//for(it = m_vocAllItemList.begin();it != m_vocAllItemList.end();++it)
	//	if((*it) != NULL)
	//		DEL_ONE((*it));
	m_vocAllItemList.clear();
	for(int j = 0;j < 4;++j)
		m_pfrmShowLabel[j]->Hide();

	unguard;
}

void CUIForm_Pickup::OnInsertNewItem(SItemID ID)
{
	guard;
	SItemID itemID = ID;
	m_vocAllItemList.push_back(itemID);
	unguard;
}

void CUIForm_Pickup::OnBtnDown(RtwWidget* sender, void*)
{
	SItemID pItem;
	for(int i= 0;i< 4;++i)
		if(sender == m_pbtnItem[i])
		{
			pItem = m_vocAllItemList[m_iNowPage* 4+ i];
			break;
		}
	if(!ItemID_IsValid(pItem))
		return;
	if ( !m_bPkPick )
	{
		GetPlayer()->SendPickItemId(pItem);
	}
	else
	{
		long all = 0;
		CG_CmdPacket *cmd = NetBeginWrite();
		cmd->WriteShort(c2r_pk_pick_item);
		cmd->SerializeBit(all,1);
		cmd->WriteLong(pItem.id);
		NetSend(cmd);
		OnDelOneItem(pItem.id);
	}
}

void CUIForm_Pickup::OnBtnOtherDown(RtwWidget* sender, void*)
{
	// ������ť���µ�ʱ�������а�����һҳ����һҳ��ȫ��ʰȡ
	if(sender == m_pbtnUppage)
	{
		m_iNowPage--;
		Show(m_bPkPick);
	}
	else if(sender == m_pbtnDownpage)
	{
		m_iNowPage++;
		Show(m_bPkPick);
	}
	else if(sender == m_pbtnAllPickup)
	{
		// ȫ��ʰȡ��ť����
		if ( !m_bPkPick )
		{
			GetPlayer()->SendPickItemId(ItemID_CreateInvalid(),1);
		}
		else
		{
			long all = 1;
			CG_CmdPacket *cmd = NetBeginWrite();
			cmd->WriteShort(c2r_pk_pick_item);
			cmd->SerializeBit(all,1);
			NetSend(cmd);
			Hide();
		}
	}
}

void CUIForm_Pickup::OnHintItem(RtwWidget* sender, void*)
{
	SItemID pItem;
	for(int i= 0;i< 4;++i)
		if(sender == m_pbtnItem[i])
		{
			pItem = m_vocAllItemList[m_iNowPage* 4+ i];
			break;
		}
	// ��ǰ����Ϊ�գ�������������
	if(!ItemID_IsValid(pItem))
		return;

	int nLines;
	RtString str = GenerateItemHtmlHint(nLines, pItem, GetWorld()->m_pPlayer);

	g_workspace.SetHintText((const char*)str, true);
	g_workspace.SetHintSize(ITEM_HINT_WIDTH, 30);
	g_workspace.AutoHintSize();
	g_workspace.AutoHintPosition();
}
void CUIForm_Pickup::OnClickPet(RtwWidget* sender, void*)
{
}

void CUIForm_Pickup::OnDelOneItem(long ID)
{
	for(int i= 0;i< m_vocAllItemList.size(); ++i)
	{
		if(ID == m_vocAllItemList[i].id)
		{
			ItemID_SetInvalid(m_vocAllItemList[i]);
			if(1)
			{
				for (int j= i;j< m_vocAllItemList.size()- 1;++j)
				{
					if (j+1 < m_vocAllItemList.size())
					{
						m_vocAllItemList[j] = m_vocAllItemList[j+1];
					}
				}
				ItemID_SetInvalid(m_vocAllItemList[m_vocAllItemList.size()-1]);
			}
			Show(m_bPkPick);
		}
	}
	for(int i= 0;i< m_vocAllItemList.size(); ++i)
	{
		if ( ItemID_IsValid(m_vocAllItemList[i]) )
		{
			return;
		}
	}
	Hide();
}
void CUIForm_Pickup::Reset()
{
	guard;
	m_iNowPage = 0;
	unguard;
}
