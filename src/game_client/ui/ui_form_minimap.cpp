#include "gc_include.h"
#include "ui_form_minimap.h"
#include "ui_form_maintask.h"
#include "ui_form_warfield.h"
#include "game_assistant.h"
#include "UIForm_MiddleMap.h"
#include "ui_form_task.h"
#include "ui_form_setting.h"
/* gao 2010.1.29 分线定义头文件 */
#include "../shared/linesys_net_define.h"
/* end */

const int UPDATE_PALYER_LIST_TIME=10; //单位:秒//更新周围玩家列表的时间
const float FSTEP = 6.f;

UIFormMiniMap::UIFormMiniMap()
{
    m_texArrayTexture		= GetDevice()->GetTextMgr()->CreateTextureFromFile("minimap_array.dds", D3DX_DEFAULT, D3DX_DEFAULT, 1);
	m_texMonster			= GetDevice()->GetTextMgr()->CreateTextureFromFile("minimap_monster.dds", D3DX_DEFAULT, D3DX_DEFAULT, 1);
	m_texNPC				= GetDevice()->GetTextMgr()->CreateTextureFromFile("minimap_npc.dds", D3DX_DEFAULT, D3DX_DEFAULT, 1);
	m_texPet				= GetDevice()->GetTextMgr()->CreateTextureFromFile("minimap_pet.dds", D3DX_DEFAULT, D3DX_DEFAULT, 1);
	m_texTeam				= GetDevice()->GetTextMgr()->CreateTextureFromFile("minimap_team.dds", D3DX_DEFAULT, D3DX_DEFAULT, 1);
	m_texPlayer				= GetDevice()->GetTextMgr()->CreateTextureFromFile("minimap_user.dds", D3DX_DEFAULT, D3DX_DEFAULT, 1);
	m_texDesFar				= GetDevice()->GetTextMgr()->CreateTextureFromFile("minimap_desfar.dds", D3DX_DEFAULT, D3DX_DEFAULT, 1);
	m_texDesNear			= GetDevice()->GetTextMgr()->CreateTextureFromFile("minimap_npc.dds", D3DX_DEFAULT, D3DX_DEFAULT, 1);
	m_texCamera				= GetDevice()->GetTextMgr()->CreateTextureFromFile("minimap_Camera.dds", D3DX_DEFAULT, D3DX_DEFAULT, 1);
	m_texMine				= GetDevice()->GetTextMgr()->CreateTextureFromFile("pic_d_icon11.dds", D3DX_DEFAULT, D3DX_DEFAULT, 1);
	m_texHerbs				= GetDevice()->GetTextMgr()->CreateTextureFromFile("pic_d_icon12.dds", D3DX_DEFAULT, D3DX_DEFAULT, 1);
	m_texSeekingpoint       = GetDevice()->GetTextMgr()->CreateTextureFromFile("seekingpoint.dds", D3DX_DEFAULT, D3DX_DEFAULT, 1);
	m_fScale = 1.f;
	m_iAreaX=0;
	m_iAreaY=0;
    m_dwColorPlayer  = 0xFF000000;
    m_dwColorPet     = 0xFF000000;
    m_dwColorTeam    = 0xFF000000;
    m_dwColorUser    = 0xFF000000;
    m_dwColorMonster = 0xFF000000;
    m_dwColorNpc     = 0xFF000000;
	//--------add start by tony 05.07.13--------------//
	m_dwColorAttacker = 0xA0A000FF;
	m_dwColorDefender = 0xA060FFFF;
	//--------add end   by tony 05.07.13--------------//

    m_iNpcCount = 0;
    m_lastNpcHint.id = 0;

    m_ShaderPoint.iNumStage = 1;
    m_ShaderPoint.bEnableBlendFunc = true;
    m_ShaderPoint.Shaders[0].eColorOp   = RTGTOP_MODULATE;//RTGTOP_BLENDTEXTUREALPHA;
    m_ShaderPoint.Shaders[0].eAlphaOp   = RTGTOP_MODULATE;
    m_ShaderPoint.Shaders[0].eAlphaArg1 = RTGTA_DIFFUSE;
    m_ShaderPoint.Shaders[0].eAlphaArg2 = RTGTA_TEXTURE;
	LOAD_UI("fmminimap")->ModifyFlags(wfInput | wfHint,0);
	LOAD_UI("fmminimap.panmap")->ModifyFlags(wfInput | wfHint,0);
	LOAD_UI("fmminimap.panallmap")->ModifyFlags(0,wfPick);
    LOAD_UI("fmminimap.btnmapadd")->EvLClick   += RTW_CALLBACK(this, UIFormMiniMap, OnButtonZoomOutClick);
    LOAD_UI("fmminimap.btnmapdec")->EvLClick   += RTW_CALLBACK(this, UIFormMiniMap, OnButtonZoomInClick);
	LOAD_UI("fmminimap.btnsubline")->EvLClick  += RTW_CALLBACK(this, UIFormMiniMap, OnSelectChildThread);
	LOAD_UI("fmminimap.btnhelp")->EvLClick	  += RTW_CALLBACK(this, UILayerMain, OnClicked_SysSetting_Help);
	//主线任务
	LOAD_UI("fmminimap.btnmaintask")->EvLClick += RTW_CALLBACK(this, UIFormMiniMap, OnClicked_MainTask);
	// 分线UI选择事件
	LOAD_UI("fmsubline.btninto")->EvLClick			 += RTW_CALLBACK(this, UIFormMiniMap, OnChildThreadButtonDown);
	LOAD_UI("fmsubline.btncancel")->EvLClick			 += RTW_CALLBACK(this, UIFormMiniMap, OnChildThreadButtonDown);
	m_plstLineContent									 = LOAD_UI_T(RtwListBox,"fmsubline.ltbservers");
	m_pwdtMiniMap										 = LOAD_UI("fmminimap");
	m_pwdtMapTitle										 = LOAD_UI("fmmaptitle");
	/* end */
    LOAD_UI("fmminimap.panmap")->EvDraw        += RTW_CALLBACK(this, UIFormMiniMap, OnDrawMiniMap);
    //LOAD_UI("fmminimap.panmap")->EvMouseMove   += RTW_CALLBACK(this, UIFormMiniMap, OnMapMouseMoveEvent);
    //LOAD_UI("fmminimap.panmap")->EvHint        += RTW_CALLBACK(this, UIFormMiniMap, OnGetNpcHintText);
	LOAD_UI("fmminimap.panmap")->EvLClick	  += RTW_CALLBACK(this,UIFormMiniMap,OnMoveTo);
    LOAD_UI("fmminimap.btnmapworld")->EvLClick      += RTW_CALLBACK(this, UIFormMiniMap, OnButtonShowWorldMap);
	LOAD_UI("fmminimap.btnmapworld")->Hide();

	// 标题按钮事件
	LOAD_UI("fmmaptitle.btnmaphide")->EvLClick      += RTW_CALLBACK(this, UIFormMiniMap, OnButtonShowHideClick);
	LOAD_UI("fmmaptitle.btnmapshow")->EvLClick      += RTW_CALLBACK(this, UIFormMiniMap, OnButtonShowHideClick);
	LOAD_UI("fmmaptitle.btnmaphide")->Show();
	LOAD_UI("fmmaptitle.btnmapshow")->Hide();
	m_pBtnWarView		= LOAD_UI_T(RtwButton, "fmminimap.btnwar");
	//保存列表的指针
	/*m_pUIPlayerList		= LOAD_UI_T(RtwListBox,"fmteam.fmcontent.tbcteam.fmprep.lbxmembers");*/
	m_pImage			= g_workspace.getImageFactory()->createImage("ui\\textures\\10.tga");
	m_pLockCamera		= LOAD_UI_T(RtwButton, "fmminimap.btnperspective");

	m_pLockCamera->EvLClick			+= RTW_CALLBACK(this,UIFormMiniMap,OnButtonLockCamera);

	//change by yz   开启关闭内挂界面
	////开关
	//LOAD_UI("fmminimap.btnwho")->EvLClick	
	//	+= RTW_CALLBACK(LOAD_UI("fmteam"), RtwWidget,DefaultChange);//打开或者关闭玩家列表

	////通知界面改变
	//LOAD_UI("fmminimap.btnwho")->EvLClick	
	//	+= RTW_CALLBACK(this, UIFormMiniMap,PlayerListUIChange);//通知列表改变

	LOAD_UI("fmminimap.btnwho")->EvLClick
		+= RTW_CALLBACK(this, UIFormMiniMap, SysBattleSetting);
	//end change

	//右键单击就弹出pop菜单
	/*m_pUIPlayerList->ModifyFlags(wfClick,0);
	m_pUIPlayerList->EvRClickItem+=RTW_CALLBACK(this,UIFormMiniMap,PopMenu);
	m_pUIPlayerList->EvLClick+=RTW_CALLBACK(this,UIFormMiniMap,PopMenuClose);*/
	//改变list的选择模式
//	m_pUIPlayerList->SetSelectedMode(RtwListBox::MOUSE_MOVE_MODE);// 注释 [3/17/2009 tooth.shi]

	GetTimer()->AddTimer(this,UPDATE_PALYER_LIST_TIME);						//将时间消息关联到该类上

    m_pWidgetSceneName = LOAD_UI("fmmaptitle.lbmapname");
	m_pWidgetSceneName->EvHint += RTW_CALLBACK(this, UIFormMiniMap, GetMiniMapHint);
	m_pWidgetSceneName->ModifyFlag(wfHint|wfGrabMouse,0);
    m_pWidgetPosition  = LOAD_UI("fmmaptitle.lbaddress");
	m_strMapName = "";
	mPopMenuOpen=false;//pop菜单没有打开
	m_bIsRunning= false;
	m_fStep =0.f;
	m_bShow =false;
}

UIFormMiniMap::~UIFormMiniMap()
{
    guard;
	//GetWorld()->m_ActorManager.EvChange.DeleteListener<UIFormMiniMap>(*this);
	GetTimer()->RemoveObject(this);//将时间关联取消
	GetDevice()->GetTextMgr()->ReleaseTexture(m_texPlayer);
	GetDevice()->GetTextMgr()->ReleaseTexture(m_texTeam);
	GetDevice()->GetTextMgr()->ReleaseTexture(m_texPet);
	GetDevice()->GetTextMgr()->ReleaseTexture(m_texNPC);
	GetDevice()->GetTextMgr()->ReleaseTexture(m_texMonster);
	GetDevice()->GetTextMgr()->ReleaseTexture(m_texArrayTexture);
	GetDevice()->GetTextMgr()->ReleaseTexture(m_texDesFar);
	GetDevice()->GetTextMgr()->ReleaseTexture(m_texDesNear);
	GetDevice()->GetTextMgr()->ReleaseTexture(m_texCamera);
	GetDevice()->GetTextMgr()->ReleaseTexture(m_texMine);
	GetDevice()->GetTextMgr()->ReleaseTexture(m_texHerbs);
	//DROP_TRUI_IMAGE(m_pImage);//3.6
	m_vecNPCs.clear();
	m_vectorTaskLabel.clear();
    unguard;
}

void UIFormMiniMap::PopMenuReset(ui::RtwWidget* sender, RtwEventDelegate* e)
{//pop菜单关闭或者重新设置
	mPopMenuOpen=false;
}

void UIFormMiniMap::PopMenuClose(ui::RtwWidget* sender, RtwEventDelegate* e)
{
	if(mPopMenuOpen)
	{
		LOAD_UI("UserMenu")->Hide();
	}

}

void UIFormMiniMap::WarfieldShow(ui::RtwWidget* sender, RtwEventDelegate* e)
{
	if(!g_layerMain->m_formWarfield->IsVisible())
	{
		CWarfieldNetHelper::NetSend_c2r_warfield_view();
		g_layerMain->m_formWarfield->Show();
	}else
	{
		g_layerMain->m_formWarfield->Hide();
	}
}

void UIFormMiniMap::PopMenu(ui::RtwWidget* sender, RtwEventDelegate* e)
{
	//int SelectIndex=m_pUIPlayerList->GetSelectedItem();
	//DWORD PlayerRegionID = UIIndex2PlayerRegionID(SelectIndex);

	//if(PlayerRegionID!=0)
	//{//选择中的人的RegionID
	//	GcActor* pActor=GetWorld()->m_ActorManager.FindAll(PlayerRegionID);
	//	if(pActor!=NULL)
	//	{//找到这个物件
	//		// 激活菜单
	//		RtwPopupMenu* pPopupMenu = ((RtwPopupMenu*)g_workspace.LookupWidget("UserMenu"));
	//		//pPopupMenu->EvHint+=RTW_CALLBACK(this,UIFormMiniMap,PopMenuReset);
	//		if (pPopupMenu)
	//		{
	//			GetWorld()->OnPopupMenu(pPopupMenu, pActor);
	//			mPopMenuOpen=true;//pop菜单打开
	//			//mPopMenuWithPlayer=PlayerRegionID;//菜单对应的玩家
	//			pPopupMenu->SetUserData((void*)(pActor->ID()));
	//			g_workspace.ShowPopupMenu(pPopupMenu, NULL,g_workspace.GetMousePosX(),g_workspace.GetMousePosY());
	//		}
	//	}
	//}
}

DWORD UIFormMiniMap::UIIndex2PlayerRegionID(int vUIIndex)
{
	if(vUIIndex==-1)
	{
		return 0;//没有选择
	}

	map<int,DWORD>::iterator It;
	It=mUI2PlayerID.find(vUIIndex);

	if(It==mUI2PlayerID.end())
	{
		return 0;//没有找到
	}
	else
	{
		return It->second;
	}
}

void UIFormMiniMap::OnTimerEvent(DWORD vID)//时间回调函数
{
	UpdatePlayerListUI();
}

void UIFormMiniMap::PlayerListUIChange(ui::RtwWidget* sender, RtwEventDelegate* e)
{
	if(! LOAD_UI("fmteam")->IsVisible())
	{		
		//LOAD_UI("fmteam")->Show();
	}else{
		LOAD_UI("fmteam")->Hide();
	}
	UpdatePlayerListUI();
}


//add by yz 开启内挂系统
void UIFormMiniMap::SysBattleSetting(ui::RtwWidget* sender, RtwEventDelegate* e)
{
	guard;
	if (g_layerMain->m_fmsysbattle->IsVisible())
	{
		g_layerMain->m_fmsysbattle->Hide();
	}
	else
	{
		g_layerMain->m_fmsysbattle->Show();
	}
	unguard;
}

void UIFormMiniMap::UpdatePlayerListUI()
{
//	//if(! LOAD_UI("fmteam.fmcontent.tbcteam.fmprep")->IsVisible())
//	//{//如果没有显示的情况下，直接返回
//	//	return ;
//	//}
//
////////////////////////////////////////////////////////////////////////////
//
//	EXT_SPACE::unordered_map<DWORD, GcSynchroActor*>&  rPlayerMap=GetWorld()->m_ActorManager.m_ActorMap;     // 所有角色索引
//	EXT_SPACE::unordered_map<DWORD, GcSynchroActor*>::iterator It;
//
//	list<GcActor*> TempList;	//玩家指针列表
//	list<DWORD>    TempIDList;		//玩家ID列表
//
//	for(It=rPlayerMap.begin();It!=rPlayerMap.end();++It)
//	{
//		GcSynchroActor* pActor=(It->second);
//		if (!pActor->IsActive())
//		{
//			continue;//不是一个活的对象
//		}
//
//		if( pActor->NpcType()==GcActor::ENT_USER && pActor->IsValid())
//		{//是一个玩家并且有效
//			TempList.push_back(It->second);
//			TempIDList.push_back(It->first);
//		}
//	}
//
//	if(TempIDList==mPlayerIDList)
//	{//同以前一样
//		return;
//	}
//
//	
//
//	mPlayerIDList=TempIDList;//保存
//
//	int SelectIndex=m_pUIPlayerList->GetSelectedItem();//当前的选择
//	DWORD SelectPlayer = UIIndex2PlayerRegionID(SelectIndex);
//
//	SelectIndex=-1;
//	m_pUIPlayerList->RemoveAllItems();	//清除玩家列表UI
//	mUI2PlayerID.clear();				//ui到玩家的关联，清除
//
//
//
//	list<GcActor*>::iterator ListIt;
//	int Index=0;
//	for(ListIt=TempList.begin();ListIt!=TempList.end();++ListIt)
//	{
//		GcActor* pActor=(*ListIt);
//		m_pUIPlayerList->AddItem();//空行
//		mUI2PlayerID[Index]=pActor->ID();//添加一个关联
//		if(pActor->ID()==SelectPlayer)
//		{
//			SelectIndex=Index;
//		}
//		
//		m_pUIPlayerList->SetItemText(Index,0,pActor->GetName());//0列//名字
//		
//		SUserActor* pUser = g_TableUserActor.FindUserActor(pActor->m_core.Metier, pActor->m_core.MetierLev);
//		if(pUser!=NULL)
//		{
//			m_pUIPlayerList->SetItemText(Index,2,pUser->MetierName);//1列//职业
//			char lev[10];sprintf(lev,"%d",pActor->m_lLev);
//			m_pUIPlayerList->SetItemText(Index,1,string(lev));
//		}
//		Index++;
//	}
//
//	if(SelectIndex==-1 )
//	{//当前没有选择
//		//关闭pop菜单
//		PopMenuClose(NULL,NULL);
//	}
//	else
//	{
//		bool PopMenuOpen=mPopMenuOpen;
//		mPopMenuOpen=false;
//		//这里不希望影响pop窗口//所以，骗过close
//		m_pUIPlayerList->SetSelectedItem(SelectIndex);
//		mPopMenuOpen=PopMenuOpen;//恢复
//	}
}


void UIFormMiniMap::OnDrawMiniMap(ui::RtwWidget* sender, RtwEventDelegate* e)
{
    guard;
	if (g_workspace.GetFocusWidget()!=LOAD_UI("UserMenu"))
	{
		LOAD_UI("UserMenu")->Hide();
	}
    RtwRect mapRect = sender->GetClientRect();
    GetDevice()->SetRenderState(RTGRS_LIGHTING, FALSE);
    GetDevice()->SetFogEnable(FALSE);

	//if (sender->getName()=="layworld.fmminimap.panmap")
	{
		DrawMiniMap(mapRect.left, mapRect.top, mapRect.getWidth(), mapRect.getHeight(), g_fSceneBlockWidth*0.5f/m_fScale);
	}
	
    unguard;
}

void UIFormMiniMap::DrawMiniMap(int iScreenX, int iScreenY, int iScreenWidth, int iScreenHeight, float fSceneWidth, WORD wShowFlag)
{
    guard;
    if (GetWorld()==NULL)
    {
        return;
    }
    const float* pPos = &(GetWorld()->m_pPlayer->GetMatrix()->_30);
	g_pScene->SetMinimapMaskTexture("minimapmask.dds", 0, 0, 1, 1);
   
    if (wShowFlag & EMMSF_SHOW_MAP)
    {
		//RtgMatrix16 playerMatrix = *(GetWorld()->m_pPlayer->Matrix());
		//playerMatrix.NormalizeMatrix();
		//RtgOrientation orie;
		//playerMatrix.GetOrientation(orie);
		///*RtgVertex3 eyeDirection = *(RtgVertex3*)&(playerMatrix._10);
		//float r = sqrt(eyeDirection.x*eyeDirection.x + eyeDirection.y*eyeDirection.y);
		//float sina = eyeDirection.x/r;*/
		//float a = RadianToDegree(orie.ang);
		float a= 0;
		g_pScene->SetMinimapRotate(a);							
		g_pScene->DrawMiniMap(iScreenX, iScreenY, iScreenWidth, iScreenHeight, fSceneWidth,pPos[0],pPos[1]);
    }

    RtgNVertexVT p[4];
    float fX, fY;
    int iX, iY;
	int iDltx, iDlty, iDlbx, iDlby, iDrtx, iDrty, iDrbx, iDrby;
	float  fSceneHeight;
	DWORD dwColor = 0xFF303030;
	float fMapRotateCos = fcos(g_pScene->GetMinimapRotateRadian());
	float fMapRotateSin = fsin(g_pScene->GetMinimapRotateRadian());
	int iCenterX = iScreenX + iScreenWidth*0.5f;
	int iCenterY = iScreenY + iScreenHeight*0.5f;

    // 取出中心点位置(玩家位置)
    m_fPlayerX = pPos[0];
    m_fPlayerY = pPos[1];
    g_pScene->OffsetToMap(m_fPlayerX, m_fPlayerY);

    // 按照比率取出需要显示的高度
    fSceneHeight = fSceneWidth * iScreenHeight / iScreenWidth;
	
    GetDevice()->SetVertexFormat(RTG_TRANSFORMED_VT);

    m_iNpcCount = 0;
	int NPCCount = 0;
	//RtwRect imgRect = RtwRect(iScreenX,iScreenY,iScreenX+iScreenWidth,iScreenY+iScreenHeight);
	char strId[32];
	RtwLabel* lbNPC;
    if (wShowFlag & EMMSF_SHOW_PLAYER) // 玩家和兽
    {
        if (GetPlayer())
        {
            float fD = (8.f/iScreenWidth)*fSceneWidth;
            float fPlayerPos[4][2];
            RtgMatrix16 m16 = *(GetPlayer()->GetMatrix());
            m16.Translate(-fD, fD, 0.f);
            fPlayerPos[0][0] = m16._30;
            fPlayerPos[0][1] = m16._31;

            m16 = *(GetPlayer()->GetMatrix());
            m16.Translate(-fD, -fD, 0.f);
            fPlayerPos[1][0] = m16._30;
            fPlayerPos[1][1] = m16._31;

            m16 = *(GetPlayer()->GetMatrix());
            m16.Translate(fD, -fD, 0.f);
            fPlayerPos[2][0] = m16._30;
            fPlayerPos[2][1] = m16._31;

            m16 = *(GetPlayer()->GetMatrix());
            m16.Translate(fD, fD, 0.f);
            fPlayerPos[3][0] = m16._30;
            fPlayerPos[3][1] = m16._31;

            for (iX=0; iX<4; iX++)
            {
				g_pScene->OffsetToMap(fPlayerPos[iX][0], fPlayerPos[iX][1]);
				fPlayerPos[iX][0] = ((fPlayerPos[iX][0]-m_fPlayerX)/fSceneWidth +0.5f)*iScreenWidth  + iScreenX;
				fPlayerPos[iX][1] = (0.5f-(fPlayerPos[iX][1]-m_fPlayerY)/fSceneHeight)*iScreenHeight + iScreenY;
            }

            m_ShaderPoint.Shaders[0].SetTexture(m_texArrayTexture);
            GetDevice()->SetShader(&m_ShaderPoint);

            SceneMiniMapRotate2(iCenterX, iCenterY, fPlayerPos[0][0], fPlayerPos[0][1], iDltx, iDlty, fMapRotateCos, fMapRotateSin);
            SceneMiniMapRotate2(iCenterX, iCenterY, fPlayerPos[1][0], fPlayerPos[1][1], iDlbx, iDlby, fMapRotateCos, fMapRotateSin);
            SceneMiniMapRotate2(iCenterX, iCenterY, fPlayerPos[2][0], fPlayerPos[2][1], iDrtx, iDrty, fMapRotateCos, fMapRotateSin);
            SceneMiniMapRotate2(iCenterX, iCenterY, fPlayerPos[3][0], fPlayerPos[3][1], iDrbx, iDrby, fMapRotateCos, fMapRotateSin);

            p[0] = RtgNVertexVT( iDltx, iDlty,  0.f, 0.f);
            p[1] = RtgNVertexVT( iDlbx, iDlby,  0.f, 1.f);
            p[2] = RtgNVertexVT( iDrtx, iDrty,  1.f, 1.f);
            p[3] = RtgNVertexVT( iDrbx, iDrby,  1.f, 0.f);
            GetDevice()->DrawPrimitiveUP(D3DPT_TRIANGLEFAN, &p, sizeof(RtgNVertexVT), 2);
            GetDevice()->RestoreShader(&m_ShaderPoint);			
        }
        if (GetWorld() && GetWorld()->m_pPet)
        {
            pPos = &(GetWorld()->m_pPet->GetMatrix()->_30);
            fX = pPos[0];
            fY = pPos[1];
            g_pScene->OffsetToMap(fX, fY);
            if (fabs(m_fPlayerX-fX)<(fSceneWidth *0.5f) && fabs(m_fPlayerY-fY)<(fSceneHeight*0.5f))
            {
                m_ShaderPoint.Shaders[0].SetTexture(m_texPet);
                GetDevice()->SetShader(&m_ShaderPoint);
				iX = ((fX-m_fPlayerX)/fSceneWidth +0.5f)*iScreenWidth  + iScreenX;
				iY = (0.5f-(fY-m_fPlayerY)/fSceneHeight)*iScreenHeight + iScreenY;
                SceneMiniMapRotate2(iCenterX, iCenterY, iX, iY, iX, iY, fMapRotateCos, fMapRotateSin);
                p[0] = RtgNVertexVT( iX-4, iY-4, 0.f, 0.f);
                p[1] = RtgNVertexVT( iX-4, iY+4, 0.f, 1.f);
                p[2] = RtgNVertexVT( iX+4, iY+4, 1.f, 1.f);
                p[3] = RtgNVertexVT( iX+4, iY-4, 1.f, 0.f);
				GetDevice()->DrawPrimitiveUP(D3DPT_TRIANGLEFAN, &p, sizeof(RtgNVertexVT), 2);
                GetDevice()->RestoreShader(&m_ShaderPoint);

                //m_ptNpcs[m_iNpcCount].x = iX;
                //m_ptNpcs[m_iNpcCount].y = iY;
                //m_ptNpcs[m_iNpcCount].id = GetWorld()->m_pPet->ID();
				if(m_iNpcCount>= m_vecNPCs.size())
				{
					SNpcPoint NPCPoint;
					NPCPoint.x= iX;
					NPCPoint.y= iY;
					NPCPoint.id= GetWorld()->m_pPet->ID();
					m_vecNPCs.push_back(NPCPoint);
				}else
				{
					m_vecNPCs[m_iNpcCount].x= iX;
					m_vecNPCs[m_iNpcCount].y= iY;
					m_vecNPCs[m_iNpcCount].id= GetWorld()->m_pPet->ID();
				}
                m_iNpcCount ++;
            }
        }
    }

    if (wShowFlag & (EMMSF_SHOW_TEAM|EMMSF_SHOW_USER|EMMSF_SHOW_MONSTER|EMMSF_SHOW_NPC|EMMSF_SHOW_CWAR_ATTACKER|EMMSF_SHOW_CWAR_DEFENDER))
    {
        GcSynchroActor* pActor;
        EXT_SPACE::unordered_map<DWORD, GcSynchroActor*>::iterator it;
        for (it=GetWorld()->m_ActorManager.m_ActorMap.begin(); it!=GetWorld()->m_ActorManager.m_ActorMap.end(); it++)
        {
            pActor = (*it).second;
			if (!pActor) continue;
            if (!pActor->IsActive()) continue;
            if (pActor->m_pHUD==NULL) continue;
			if (pActor->IsTrigger()) continue;

            pPos = &(pActor->GetMatrix()->_30);

            fX = pPos[0];
            fY = pPos[1];
            g_pScene->OffsetToMap(fX, fY);

            if (fabs(m_fPlayerX-fX)>=(fSceneWidth *0.5f)) continue;
            if (fabs(m_fPlayerY-fY)>=(fSceneHeight*0.5f)) continue;
			iX = ((fX-m_fPlayerX)/fSceneWidth +0.5f)*iScreenWidth  + iScreenX;
			iY = (0.5f-(fY-m_fPlayerY)/fSceneHeight)*iScreenHeight + iScreenY;
			
            switch (pActor->NpcType())	
            {
            case GcActor::ENT_USER:
            case GcActor::ENT_PET:
				m_ShaderPoint.Shaders[0].SetTexture(m_texPlayer);
                if (pActor->NpcType()==GcActor::ENT_USER)
                {
                    if (GetWorld()->m_Team.mData.Exist(pActor->GetName()))
                    {
                        //dwColor = m_dwColorTeam;
						continue;
                    }else
                    {
						m_ShaderPoint.Shaders[0].SetTexture(m_texPlayer);
                    }
                }else
                {
					m_ShaderPoint.Shaders[0].SetTexture(m_texPet);
                }

				//-----------add start by tony 05.07.13--------------------------------//
				if(GetWorld()->IsCastleWarTime(pActor->m_feod_name) == true)
				{
					if(pActor->m_is_attacker == (BYTE)1)
					{
						m_ShaderPoint.Shaders[0].SetTexture(m_texMonster);
					}else
					{
						m_ShaderPoint.Shaders[0].SetTexture(m_texPlayer);
					}
				}
				//-----------add end   by tony 05.07.13--------------------------------//
                break;
            //default:
            case GcActor::ENT_NPC_SERVICE:
				m_ShaderPoint.Shaders[0].SetTexture(m_texNPC);
				dwColor = m_dwColorNpc;
				
                break;
			case GcActor::ENT_NPC_COMBATIVE:
				{
					if (!pActor->m_pNpc)	dwColor = m_dwColorNpc;
					char npcFaction = pActor->m_pNpc->Faction;
					if (npcFaction==FACTION_UNKNOWN)
					{
						// 中立阵营
						dwColor = m_dwColorNpc;
						m_ShaderPoint.Shaders[0].SetTexture(m_texNPC);
					}
					else if (npcFaction==GetWorld()->m_pPlayer->m_core.Faction)
					{
						// 同一阵营
						dwColor = m_dwColorNpc;
						m_ShaderPoint.Shaders[0].SetTexture(m_texNPC);
					}
					else 
					{
						// 不同阵营
						dwColor = m_dwColorMonster;
						m_ShaderPoint.Shaders[0].SetTexture(m_texMonster);
					}
				}
				break;
            case GcActor::ENT_MONSTER:
				if (!pActor->m_pNpc) continue;
				if(pActor->m_pNpc->Type==4)
					m_ShaderPoint.Shaders[0].SetTexture(m_texHerbs);
				else if(pActor->m_pNpc->Type==5)
					m_ShaderPoint.Shaders[0].SetTexture(m_texMine);
				else
					m_ShaderPoint.Shaders[0].SetTexture(m_texMonster);
				break;
            }

			GetDevice()->SetShader(&m_ShaderPoint);
			SceneMiniMapRotate2(iCenterX, iCenterY, iX, iY, iX, iY, fMapRotateCos, fMapRotateSin);
            p[0] = RtgNVertexVT( iX-4, iY-4, 0.f, 0.f);
            p[1] = RtgNVertexVT( iX-4, iY+4, 0.f, 1.f);
            p[2] = RtgNVertexVT( iX+4, iY+4, 1.f, 1.f);
            p[3] = RtgNVertexVT( iX+4, iY-4, 1.f, 0.f);
            GetDevice()->DrawPrimitiveUP(D3DPT_TRIANGLEFAN, &p, sizeof(RtgNVertexVT), 2);

            //m_ptNpcs[m_iNpcCount].x = iX;
            //m_ptNpcs[m_iNpcCount].y = iY;
            //m_ptNpcs[m_iNpcCount].id = pActor->ID();
			if(m_iNpcCount>= m_vecNPCs.size())
			{
				SNpcPoint NPCPoint;
				NPCPoint.x= iX;
				NPCPoint.y= iY;
				NPCPoint.id= pActor->ID();
				m_vecNPCs.push_back(NPCPoint);
			}else
			{
				m_vecNPCs[m_iNpcCount].x= iX;
				m_vecNPCs[m_iNpcCount].y= iY;
				m_vecNPCs[m_iNpcCount].id= pActor->ID();
			}
			m_iNpcCount ++;
			
			if (NPCCount >= m_vectorNPCLable.size())
			{

				lbNPC = (RtwLabel*)g_workspace.getWidgetFactory()->createWidget(wtLabel);
				rt2_sprintf(strId, "lbNPC_%d",NPCCount);
				lbNPC->SetName(strId);
				lbNPC->ModifyFlag(wfGrabMouse|wfMouseMove|wfHint,0);
				m_vectorNPCLable.push_back(lbNPC);
				LOAD_UI("layworld.fmminimap.panmap")->AddChild(lbNPC);
				if (pActor->NpcType()==GcActor::ENT_USER)
					lbNPC->SetHintText(pActor->GetName());
				else if (pActor->m_pNpc)
					lbNPC->SetHintText(pActor->m_pNpc->Name);
				
			}else
			{
				lbNPC = m_vectorNPCLable[NPCCount];
				if (pActor->NpcType()==GcActor::ENT_USER)
					lbNPC->SetHintText(pActor->GetName());
				else if (pActor->m_pNpc)
					lbNPC->SetHintText(pActor->m_pNpc->Name);
			}
			
			//if(imgRect.IsContained(iX,iY))
			//{
				lbNPC->Show();
				RtwRect newLabelRect= RtwRect(iX-4,iY-4,iX +4,iY+4);
				lbNPC->MoveResize(newLabelRect,true);
			//}else
			//{
			//	lbNPC->Hide();
			//}
			
            ++NPCCount;
        }
        GetDevice()->RestoreShader(&m_ShaderPoint);
		
    }
	// 队友的渲染
	STeamData* teamData = GetWorld()->m_Team.mData.GetTeamData();
	if (GetWorld()->m_Team.mData.Size())
	{
		int teamActor = 0;
		for (;teamActor < GetWorld()->m_Team.mData.Size();++teamActor)
		{
			fX		= teamData[teamActor].X;
			fY		= teamData[teamActor].Y;
			g_pScene->OffsetToMap(fX, fY);

			if (fabs(m_fPlayerX-fX)>=(fSceneWidth *0.5f)) continue;
			if (fabs(m_fPlayerY-fY)>=(fSceneHeight*0.5f)) continue;
			if (teamData[teamActor].mDBID== GetPlayer()->DBID()) continue;
			if (sqrt((m_fPlayerX-fX)*(m_fPlayerX-fX)+(m_fPlayerY-fY)*(m_fPlayerY-fY))>=(fSceneHeight*0.5f))
				continue;
			iX = ((fX-m_fPlayerX)/fSceneWidth +0.5f)*iScreenWidth  + iScreenX;
			iY = (0.5f-(fY-m_fPlayerY)/fSceneHeight)*iScreenHeight + iScreenY;

			m_ShaderPoint.Shaders[0].SetTexture(m_texTeam);
			GetDevice()->SetShader(&m_ShaderPoint);
			SceneMiniMapRotate2(iCenterX, iCenterY, iX, iY, iX, iY, fMapRotateCos, fMapRotateSin);
			p[0] = RtgNVertexVT( iX-4, iY-4, 0.f, 0.f);
			p[1] = RtgNVertexVT( iX-4, iY+4, 0.f, 1.f);
			p[2] = RtgNVertexVT( iX+4, iY+4, 1.f, 1.f);
			p[3] = RtgNVertexVT( iX+4, iY-4, 1.f, 0.f);
			GetDevice()->DrawPrimitiveUP(D3DPT_TRIANGLEFAN, &p, sizeof(RtgNVertexVT), 2);
			GetDevice()->RestoreShader(&m_ShaderPoint);
			if(m_iNpcCount>= m_vecNPCs.size())
			{
				SNpcPoint NPCPoint;
				NPCPoint.x= iX;
				NPCPoint.y= iY;
				NPCPoint.id= 0/*pActor->ID()*/;
				m_vecNPCs.push_back(NPCPoint);
			}else
			{
				m_vecNPCs[m_iNpcCount].x= iX;
				m_vecNPCs[m_iNpcCount].y= iY;
				m_vecNPCs[m_iNpcCount].id= 0/*pActor->ID()*/;
			}

			if (NPCCount >= m_vectorNPCLable.size())
			{

				lbNPC = (RtwLabel*)g_workspace.getWidgetFactory()->createWidget(wtLabel);
				rt2_sprintf(strId, "lbNPC_%d",NPCCount);
				lbNPC->SetName(strId);
				lbNPC->ModifyFlag(wfGrabMouse|wfMouseMove|wfHint,0);
				m_vectorNPCLable.push_back(lbNPC);
				LOAD_UI("layworld.fmminimap.panmap")->AddChild(lbNPC);
				lbNPC->SetHintText(teamData[teamActor].mName);

			}else
			{
				lbNPC = m_vectorNPCLable[NPCCount];
				lbNPC->SetHintText(teamData[teamActor].mName);
			}
			lbNPC->Show();
			RtwRect newLabelRect= RtwRect(iX-4,iY-4,iX +4,iY+4);
			lbNPC->MoveResize(newLabelRect,true);
			++NPCCount;
		}
	}
	while(NPCCount< m_vectorNPCLable.size())
	{
		m_vectorNPCLable[NPCCount]->Hide();
		m_vectorNPCLable[NPCCount]->Hide();
		++NPCCount;
	}

	vector<STaskDesc*>::iterator itTaskdesc;
	int Taskdescnum= 0;
	STaskDesc* desc= NULL;
	for(itTaskdesc= m_vectorCanAccept.begin();itTaskdesc!= m_vectorCanAccept.end();itTaskdesc++)
	{
		desc = *itTaskdesc;
		if(!desc)
			continue;
		if(desc->m_TaskSencenID != m_strMapName)
			continue;
		fX		= (*itTaskdesc)->m_posTaskBegin[0]/**0.05*/;
		fY		= (*itTaskdesc)->m_posTaskBegin[1]/**0.05*/;
		g_pScene->OffsetToMap(fX, fY);

		if (fabs(m_fPlayerX-fX)>=(fSceneWidth *0.5f)) continue;
		if (fabs(m_fPlayerY-fY)>=(fSceneHeight*0.5f)) continue;
		if (sqrt((m_fPlayerX-fX)*(m_fPlayerX-fX)+(m_fPlayerY-fY)*(m_fPlayerY-fY))>=(fSceneHeight*0.5f))
			continue;
		iX = ((fX-m_fPlayerX)/fSceneWidth +0.5f)*iScreenWidth  + iScreenX;
		iY = (0.5f-(fY-m_fPlayerY)/fSceneHeight)*iScreenHeight + iScreenY;
		// 如果 NPC的 数目超过容器内Lalel数目
		RtwLabel* lbTaskdesc;
		if(Taskdescnum >= m_vectorTaskLabel.size())
		{
			RtwImage* tmpImage = g_workspace.getImageFactory()->createImage("ui_texture\\pic_d_icon02.gif");
			lbTaskdesc = (RtwLabel*)g_workspace.getWidgetFactory()->createWidget(wtLabel);
			rt2_sprintf(strId, "lbTaskdesc_%d",Taskdescnum);
 			lbTaskdesc->SetName(strId);
 			lbTaskdesc->ModifyFlag(0,wfGrabMouse|wfMouseMove|wfHint);
			if(tmpImage)
			{
				tmpImage->SetBlend(true);
				lbTaskdesc->SetBackgroundImage(tmpImage);
			}
			m_vectorTaskLabel.push_back(lbTaskdesc);
			LOAD_UI("layworld.fmminimap.panmap")->AddChild(lbTaskdesc);
		}else
		{
			lbTaskdesc = m_vectorTaskLabel[Taskdescnum];
		}
		g_workspace.BringWidgetToTop(lbTaskdesc);
		lbTaskdesc->Show();
		RtwRect newLabelRect= RtwRect(iX-5,iY-5,iX +4,iY+4);
		lbTaskdesc->MoveResize(newLabelRect,true);
		Taskdescnum++;
	}

	while(Taskdescnum< m_vectorTaskLabel.size())
	{
		m_vectorTaskLabel[Taskdescnum]->Hide();
		Taskdescnum++;
	}
    if (wShowFlag & EMMSF_SHOW_FLAG) // 场景标记
    {
		if(GetPlayer() && GetPlayer()->mBaseActor.IsDrawPathLine())
		{
			if(GetWorld())
			{
				list<SPointScene>& allPathPoint = RtsWayPoint::GetInstance()->m_allPathPoint;
				list<SPointScene>::iterator it = allPathPoint.begin();
				for(; it != allPathPoint.end(); ++ it)
				{
					m_ShaderPoint.Shaders[0].SetTexture(m_texNPC);
					GetDevice()->SetShader(&m_ShaderPoint);
					dwColor = 0xffff00ff;

					RtgVertex3 pos;
					g_pScene->GetPosByTerrain(it->x, it->y, pos);
					fX = pos.x;
					fY = pos.y;
					g_pScene->OffsetToMap(fX, fY);

					if (fabs(m_fPlayerX-fX)>=(fSceneWidth *0.5f)) continue;
					if (fabs(m_fPlayerY-fY)>=(fSceneHeight*0.5f)) continue;
					iX = ((fX-m_fPlayerX)/fSceneWidth +0.5f)*iScreenWidth  + iScreenX;
					iY = (0.5f-(fY-m_fPlayerY)/fSceneHeight)*iScreenHeight + iScreenY;
					SceneMiniMapRotate2(iCenterX, iCenterY, iX, iY, iX, iY, fMapRotateCos, fMapRotateSin);
					p[0] = RtgNVertexVT( iX-2, iY-2, 0.f, 0.f);
					p[1] = RtgNVertexVT( iX-2, iY+2, 0.f, 1.f);
					p[2] = RtgNVertexVT( iX+2, iY+2, 1.f, 1.f);
					p[3] = RtgNVertexVT( iX+2, iY-2, 1.f, 0.f);
                    GetDevice()->DrawPrimitiveUP(D3DPT_TRIANGLEFAN, &p, sizeof(RtgNVertexVT), 2);


					GetDevice()->RestoreShader(&m_ShaderPoint);
				}
			}

			if(GetPlayer())
			{
				RtsPathPath* pathNode = GetPlayer()->mBaseActor.GetPathNode();
				if(pathNode)
				{
					m_ShaderPoint.Shaders[0].SetTexture(m_texSeekingpoint);
					GetDevice()->SetShader(&m_ShaderPoint);
					dwColor = 0xffffffff;

					int count = pathNode->PathCount();
					for(int i = 0; i < count; i ++)
					{
						RtsPathPath::PPos* theStep = pathNode->GetStep(i);
						if(!theStep)
							break;

						RtgVertex3 vPos;
						g_pScene->GetPosByTerrain(theStep->MinX, theStep->MinY, vPos);
						fX = vPos.x;
						fY = vPos.y;
						g_pScene->OffsetToMap(fX, fY);

						if (fabs(m_fPlayerX-fX)>=(fSceneWidth *0.5f)) continue;
						if (fabs(m_fPlayerY-fY)>=(fSceneHeight*0.5f)) continue;
						iX = ((fX-m_fPlayerX)/fSceneWidth +0.5f)*iScreenWidth  + iScreenX;
						iY = (0.5f-(fY-m_fPlayerY)/fSceneHeight)*iScreenHeight + iScreenY;
						SceneMiniMapRotate2(iCenterX, iCenterY, iX, iY, iX, iY, fMapRotateCos, fMapRotateSin);
						p[0] = RtgNVertexVT( iX-4, iY-4, 0.f, 0.f);
						p[1] = RtgNVertexVT( iX-4, iY+4, 0.f, 1.f);
						p[2] = RtgNVertexVT( iX+4, iY+4, 1.f, 1.f);
						p[3] = RtgNVertexVT( iX+4, iY-4, 1.f, 0.f);
                        GetDevice()->DrawPrimitiveUP(D3DPT_TRIANGLEFAN, &p, sizeof(RtgNVertexVT), 2);

					}

					GetDevice()->RestoreShader(&m_ShaderPoint);
				}
			}
		}
    }
	// 指示方向的渲染
	float* fDesPos = g_layerMain->m_formMiddleMap->GetMovetoPoint();
	pPos = &(GetWorld()->m_pPlayer->GetMatrix()->_30);
	float rectPose[4][2];
	RtgMatrix16 m16;
	float noUse = 1.f;
	float centerX = 0.5f*iScreenWidth + iScreenX;
	float centerY = 0.5f*iScreenHeight + iScreenY;
	if(g_layerMain->m_formMiddleMap->GetWetherNeedShowMovetFlag())
	{
		float playerX = pPos[0];
		float playerY = pPos[1];
		float fDesPosX = fDesPos[0];
		float fDesPosY = fDesPos[1];
		g_pScene->OffsetToMap(playerX, playerY);
		g_pScene->OffsetToMap(fDesPosX, fDesPosY);
		fDesPosX = ((fDesPosX - playerX)/fSceneWidth + 0.5f)*iScreenWidth +iScreenX;
		fDesPosY = (0.5f-(fDesPosY-m_fPlayerY)/fSceneHeight)*iScreenHeight + iScreenY;
		float rWidth = fDesPosX-centerX;
		float rHeight = fDesPosY-centerY;
		float dist = sqrt(rWidth*rWidth + rHeight*rHeight);
		float cosa = -rHeight/dist;
		float sina = rWidth/dist;
		float radius = iScreenWidth*0.5f;
		if(dist > radius)
		{
			// 当前位置不在雷达内时
			m16.Identity();
			m16._00 = cosa;
			m16._01 = sina;
			m16._02 = 0;
			m16._10 = -sina;
			m16._11 = cosa;
			m16._12 = 0;
			m16._20 = centerX;
			m16._21 = centerY;
			m16._22 = 1;

			rectPose[0][0] = -8;
			rectPose[0][1] = -radius;
			rectPose[1][0] = -8;
			rectPose[1][1] = -radius + 15;
			rectPose[2][0] = 7;
			rectPose[2][1] = -radius + 15;
			rectPose[3][0] = 7;
			rectPose[3][1] = -radius;

			m16.Translate(rectPose[0][0],rectPose[0][1],noUse);
			rectPose[0][0] = m16._30;
			rectPose[0][1] = m16._31;
			m16._30 = 0.f;
			m16._31 = 0.f;
			m16._32 = 0.f;
			m16.Translate(rectPose[1][0],rectPose[1][1],noUse);
			rectPose[1][0] = m16._30;
			rectPose[1][1] = m16._31;
			m16._30 = 0.f;
			m16._31 = 0.f;
			m16._32 = 0.f;
			m16.Translate(rectPose[2][0],rectPose[2][1],noUse);
			rectPose[2][0] = m16._30;
			rectPose[2][1] = m16._31;
			m16._30 = 0.f;
			m16._31 = 0.f;
			m16._32 = 0.f;
			m16.Translate(rectPose[3][0],rectPose[3][1],noUse);
			rectPose[3][0] = m16._30;
			rectPose[3][1] = m16._31;

			SceneMiniMapRotate2(iCenterX, iCenterY, rectPose[0][0], rectPose[0][1], iDltx, iDlty, fMapRotateCos, fMapRotateSin);
			SceneMiniMapRotate2(iCenterX, iCenterY, rectPose[1][0], rectPose[1][1], iDlbx, iDlby, fMapRotateCos, fMapRotateSin);
			SceneMiniMapRotate2(iCenterX, iCenterY, rectPose[2][0], rectPose[2][1], iDrtx, iDrty, fMapRotateCos, fMapRotateSin);
			SceneMiniMapRotate2(iCenterX, iCenterY, rectPose[3][0], rectPose[3][1], iDrbx, iDrby, fMapRotateCos, fMapRotateSin);

			m_ShaderPoint.Shaders[0].SetTexture(m_texDesFar);
			GetDevice()->SetShader(&m_ShaderPoint);
			p[0] = RtgNVertexVT( iDltx, iDlty,  0.f, 0.f);
			p[1] = RtgNVertexVT( iDlbx, iDlby,  0.f, 1.f);
			p[2] = RtgNVertexVT( iDrtx, iDrty,  1.f, 1.f);
			p[3] = RtgNVertexVT( iDrbx, iDrby,  1.f, 0.f);
			GetDevice()->DrawPrimitiveUP(D3DPT_TRIANGLEFAN, p, sizeof(RtgNVertexVT), 2);
            //GetDevice()->DrawPrimitive(RTG_TRIANGLE_FAN, &p, 4, 2);
			GetDevice()->RestoreShader(&m_ShaderPoint);
		}else
		{
			m_ShaderPoint.Shaders[0].SetTexture(m_texDesNear);
			GetDevice()->SetShader(&m_ShaderPoint);
			int desPosX;
			int desPosY;
			SceneMiniMapRotate2(iCenterX, iCenterY, fDesPosX, fDesPosY, desPosX, desPosY, fMapRotateCos, fMapRotateSin);
			p[0] = RtgNVertexVT( desPosX-4, desPosY-4, 0.f, 0.f);
			p[1] = RtgNVertexVT( desPosX-4, desPosY+4, 0.f, 1.f);
			p[2] = RtgNVertexVT( desPosX+4, desPosY+4, 1.f, 1.f);
			p[3] = RtgNVertexVT( desPosX+4, desPosY-4, 1.f, 0.f);
			//GetDevice()->DrawPrimitive(RTG_TRIANGLE_FAN, &p, 4, 2);
            GetDevice()->DrawPrimitiveUP(D3DPT_TRIANGLEFAN, p, sizeof(RtgNVertexVT), 2);
			GetDevice()->RestoreShader(&m_ShaderPoint);
		}
	}
	// 探照灯的渲染
	{
		RtgVertex3 eyeDirection = GetDevice()->m_pCamera->GetViewDir();
		float r = sqrt(eyeDirection.x*eyeDirection.x + eyeDirection.y*eyeDirection.y);
		float sina = eyeDirection.x/r;
		float cosa = eyeDirection.y/r;
		m16.Identity();
		m16._00 = cosa;
		m16._01 = sina;
		m16._02 = 0;
		m16._10 = -sina;
		m16._11 = cosa;
		m16._12 = 0;
		m16._20 = centerX;
		m16._21 = centerY;
		m16._22 = 1;
		rectPose[0][0] = -32;
		rectPose[0][1] = -63;

		rectPose[1][0] = -32;
		rectPose[1][1] = 0;

		rectPose[2][0] = 31;
		rectPose[2][1] = 0;

		rectPose[3][0] = 31;
		rectPose[3][1] = -63;

		m16.Translate(rectPose[0][0],rectPose[0][1],noUse);
		rectPose[0][0] = m16._30;
		rectPose[0][1] = m16._31;
		m16._30 = 0.f;
		m16._31 = 0.f;
		m16._32 = 0.f;
		m16.Translate(rectPose[1][0],rectPose[1][1],noUse);
		rectPose[1][0] = m16._30;
		rectPose[1][1] = m16._31;
		m16._30 = 0.f;
		m16._31 = 0.f;
		m16._32 = 0.f;
		m16.Translate(rectPose[2][0],rectPose[2][1],noUse);
		rectPose[2][0] = m16._30;
		rectPose[2][1] = m16._31;
		m16._30 = 0.f;
		m16._31 = 0.f;
		m16._32 = 0.f;
		m16.Translate(rectPose[3][0],rectPose[3][1],noUse);
		rectPose[3][0] = m16._30;
		rectPose[3][1] = m16._31;
		SceneMiniMapRotate2(iCenterX, iCenterY, rectPose[0][0], rectPose[0][1], iDltx, iDlty, fMapRotateCos, fMapRotateSin);
		SceneMiniMapRotate2(iCenterX, iCenterY, rectPose[1][0], rectPose[1][1], iDlbx, iDlby, fMapRotateCos, fMapRotateSin);
		SceneMiniMapRotate2(iCenterX, iCenterY, rectPose[2][0], rectPose[2][1], iDrtx, iDrty, fMapRotateCos, fMapRotateSin);
		SceneMiniMapRotate2(iCenterX, iCenterY, rectPose[3][0], rectPose[3][1], iDrbx, iDrby, fMapRotateCos, fMapRotateSin);

		m_ShaderPoint.Shaders[0].SetTexture(m_texCamera);
		GetDevice()->SetShader(&m_ShaderPoint);
		p[0] = RtgNVertexVT( iDltx, iDlty,  0.f, 0.f);
		p[1] = RtgNVertexVT( iDlbx, iDlby,  0.f, 1.f);
		p[2] = RtgNVertexVT( iDrtx, iDrty,  1.f, 1.f);
		p[3] = RtgNVertexVT( iDrbx, iDrby,  1.f, 0.f);
		//GetDevice()->DrawPrimitive(RTG_TRIANGLE_FAN, &p, 4, 2);
        GetDevice()->DrawPrimitiveUP(D3DPT_TRIANGLEFAN, p, sizeof(RtgNVertexVT), 2);
		GetDevice()->RestoreShader(&m_ShaderPoint);
	}
   unguard;
}

void UIFormMiniMap::SetScale(float fScale)
{
    m_fScale = fScale;
    if (m_fScale<0.5f)     m_fScale = 0.5f;
    else if (m_fScale>1.1f) m_fScale = 1.1f;
}

void UIFormMiniMap::OnButtonZoomInClick(ui::RtwWidget* sender, RtwEventDelegate* e)
{
    SetScale(m_fScale-0.3f);
}

void UIFormMiniMap::OnButtonZoomOutClick(ui::RtwWidget* sender, RtwEventDelegate* e)
{
    SetScale(m_fScale+0.3f);
}

void UIFormMiniMap::OnMoveTo(ui::RtwWidget* sender,RtwEventDelegate* e)
{
	g_workspace.BringWidgetToBottom(sender);
	RtwRect mapRect = sender->GetClientRect();
	//get mouse pos
	int mouseX = e->mouse.x;
	int mouseY = e->mouse.y;

	//change to terrain
	int iScreenX = mapRect.left;
	int iScreenY = mapRect.top;
	int iScreenWidth = mapRect.getWidth();
	int iScreenHeight = mapRect.getHeight();
	float fSceneWidth=0;
	fSceneWidth = g_fSceneBlockWidth*0.5f/m_fScale;
	float fSceneHeight = fSceneWidth * iScreenHeight / iScreenWidth;

	float fMapRotateCos = fcos(g_pScene->GetMinimapRotateRadian());
	float fMapRotateSin = -fsin(g_pScene->GetMinimapRotateRadian());

	int iCenterX = iScreenX + iScreenWidth*0.5f;
	int iCenterY = iScreenY + iScreenHeight*0.5f;
	float terrainX,terrainY;
	//minimap rotation
	
	SceneMiniMapRotate2(iCenterX, iCenterY, mouseX, mouseY, mouseX, mouseY, fMapRotateCos, fMapRotateSin);
	
	terrainX = (((float)mouseX - iScreenX) / iScreenWidth - 0.5f) * fSceneWidth + m_fPlayerX;
	terrainY = (0.5f - ((float)mouseY - iScreenY) / iScreenHeight) * fSceneHeight + m_fPlayerY;
	
	//change to world position
	g_pScene->OffsetToWorld(terrainX,terrainY);
	
	PushCallStack(csn_GcActor_AddCommandMoveTo);
	(((GetWorld()->m_iCheckPlayerSeed = GetWorld()->m_iCheckPlayerSeed * 203014L + 3521051L) >> 16) & 0x7fff);
	GetWorld()->m_pPlayer->AddCommandMoveTo(terrainX,terrainY,0,-1.0f,0);
	GetWorld()->m_iCheckPlayerSeedSave = GetWorld()->m_iCheckPlayerSeed;
	PopCallStack();
}
void UIFormMiniMap::OnAreamapMoveTo(ui::RtwWidget* sender,RtwEventDelegate* e)
{//max
	int mouseX = e->mouse.x;
	int mouseY = e->mouse.y;
	RtwRect mapRect = sender->GetClientRect();
	const float* pPos = &(GetWorld()->m_pPlayer->GetMatrix()->_30);
	int iScreenX = mapRect.left;
	int iScreenY = mapRect.top;
	int iScreenWidth = mapRect.getWidth();
	int iScreenHeight = mapRect. getHeight();
	float fBlockWidth = g_fSceneBlockWidth;
	float fx=pPos[0];
	float fy=pPos[1];
	g_pScene->OffsetToMap(fx,fy);
	if (m_iAreaX != -1 && m_iAreaY != -1)
	{
	
		float terrainX = ((float)mouseX - m_iAreaX)*(fBlockWidth/iScreenWidth) +fx; 
		float terrainY = (m_iAreaY- (float)mouseY )*(fBlockWidth/iScreenHeight) +fy;
		g_pScene->OffsetToWorld(terrainX,terrainY);
		PushCallStack(csn_GcActor_AddCommandMoveTo);
		(((GetWorld()->m_iCheckPlayerSeed = GetWorld()->m_iCheckPlayerSeed * 203014L + 3521051L) >> 16) & 0x7fff);
		GetWorld()->m_pPlayer->AddCommandMoveTo(terrainX,terrainY,0,-1.0f,0);
		GetWorld()->m_iCheckPlayerSeedSave = GetWorld()->m_iCheckPlayerSeed;
		PopCallStack();
	}
}
bool UIFormMiniMap::MapIsShow()
{
    return LOAD_UI("fmminimap.panallmap")->IsVisible();
}

void UIFormMiniMap::OnButtonShowHideClick(ui::RtwWidget* sender, RtwEventDelegate* e)
{
    guard;
	static RtwImage* pImage = g_workspace.getImageFactory()->createImage("ui\\textures\\10.tga");
	static bool firstRun = true;
	static RtwRect showRect = RtwRect(320,297,511,332);
	static RtwRect hideRect = RtwRect(320,259,511,294);
	if(firstRun)
	{
		//if (pImage)
		//{
		//	pImage->grab();
		//}
		//else
		//{
		//	return;
		//}
		firstRun = false;
		m_pwdtMapTitle->SetBackgroundImage(pImage);
		pImage->SetBlend(true);
	}
	if(m_bShow)
	{
		m_bShow = false;
		m_bIsRunning = true;
		m_fStep = -FSTEP;
		LOAD_UI("fmmaptitle.btnmaphide")->Show();
		LOAD_UI("fmmaptitle.btnmapshow")->Hide();
		pImage->SetSourceRect(showRect);
	}else
	{
		m_bShow = true;
		m_bIsRunning = true;
		m_fStep = FSTEP;
		LOAD_UI("fmmaptitle.btnmaphide")->Hide();
		LOAD_UI("fmmaptitle.btnmapshow")->Show();
		pImage->SetSourceRect(hideRect);
	}
    unguard;
}

void UIFormMiniMap::SetSceneName(const char* szName)
{
	m_strMapName = szName;
	if(GetPlayer()->GetLineID())
	{
		RtString str;
		str.Format("%s-%d线",m_strMapName.c_str(),GetPlayer()->GetLineID());
		m_pWidgetSceneName->SetText(str.c_str());
	}else
	{
		m_pWidgetSceneName->SetText(m_strMapName.c_str());
	}
}	
void UIFormMiniMap::SetSceneName()
{
	guard;
	if(GetPlayer()->GetLineID())
	{
		if(m_strMapName != "")
		{
			RtString str;
			str.Format("%s-%d线",m_strMapName.c_str(),GetPlayer()->GetLineID());
			m_pWidgetSceneName->SetText(str.c_str());
		}
	}else
	{
		m_pWidgetSceneName->SetText(m_strMapName.c_str());
	}
	unguard;
}

void UIFormMiniMap::SetPosition(float fX, float fY)
{
    rt2_sprintf(g_strStaticBuffer, "%d, %d", (int)(fX/20.f), (int)(fY/20.f));
    m_pWidgetPosition->SetText(g_strStaticBuffer);
}

void UIFormMiniMap::SetPKState(EPKAttr ePKAttr)
{
    switch (ePKAttr)
    {
    case PKA_SAFEST:
    case PKA_PKUNABLE:
        m_pWidgetSceneName->SetTextColor(RtwPixel(0xFF00FF00));
        m_pWidgetPosition->SetTextColor(RtwPixel(0xFF00FF00));
        break;
    case PKA_SAFE:
        m_pWidgetSceneName->SetTextColor(RtwPixel(0xFFFF00FF));
        m_pWidgetPosition->SetTextColor(RtwPixel(0xFFFF00FF));
        break;
    case PKA_UNSAFE:
        m_pWidgetSceneName->SetTextColor(RtwPixel(0xFFFF0000));
        m_pWidgetPosition->SetTextColor(RtwPixel(0xFFFF0000));
        break;
    default:
    case PKA_NORMAL:
        m_pWidgetSceneName->SetTextColor(RtwPixel(0xFFFFFFFF));
        m_pWidgetPosition->SetTextColor(RtwPixel(0xFFFFFFFF));
        break;
    }
}

void UIFormMiniMap::OnButtonShowWorldMap(ui::RtwWidget* sender, RtwEventDelegate* e)
{
    /*float fScale = GetDevice()->m_iWndWidth / 1024.0f;
    float fWidth = 256*fScale;
    float fHeight = 256*fScale;

    static string s_LoadingFiles[] = {"ui_texture/map-1.bmp",  "ui_texture/map-2.bmp", "ui_texture/map-3.bmp", "ui_texture/map-4.bmp", "ui_texture/map-5.bmp", "ui_texture/map-6.bmp", \
        "ui_texture/map-7.bmp", "ui_texture/map-8.bmp", "ui_texture/map-9.bmp", "ui_texture/map-10.bmp", "ui_texture/map-11.bmp", "ui_texture/map-12.bmp"};

    string *arrFiles = s_LoadingFiles;
    vector<SequenceImageItem> arrImages;
    SequenceImageItem image;
    image = SequenceImageItem(arrFiles[0].c_str(), 0, 0, fWidth, fHeight);
    arrImages.push_back(image);
    image = SequenceImageItem(arrFiles[1].c_str(), 256*fScale, 0, fWidth, fHeight);
    arrImages.push_back(image);
    image = SequenceImageItem(arrFiles[2].c_str(), 512*fScale, 0, fWidth, fHeight);
    arrImages.push_back(image);
    image = SequenceImageItem(arrFiles[3].c_str(), 768*fScale, 0, fWidth, fHeight);
    arrImages.push_back(image);
    image = SequenceImageItem(arrFiles[4].c_str(), 0, 256*fScale, fWidth, fHeight);
    arrImages.push_back(image);
    image = SequenceImageItem(arrFiles[5].c_str(), 256*fScale, 256*fScale, fWidth, fHeight);
    arrImages.push_back(image);
    image = SequenceImageItem(arrFiles[6].c_str(), 512*fScale, 256*fScale, fWidth, fHeight);
    arrImages.push_back(image);
    image = SequenceImageItem(arrFiles[7].c_str(), 768*fScale, 256*fScale, fWidth, fHeight);
    arrImages.push_back(image);
    image = SequenceImageItem(arrFiles[8].c_str(), 0, 512*fScale, fWidth, fHeight);
    arrImages.push_back(image);
    image = SequenceImageItem(arrFiles[9].c_str(), 256*fScale, 512*fScale, fWidth, fHeight);
    arrImages.push_back(image);
    image = SequenceImageItem(arrFiles[10].c_str(), 512*fScale, 512*fScale, fWidth, fHeight);
    arrImages.push_back(image);
    image = SequenceImageItem(arrFiles[11].c_str(), 768*fScale, 512*fScale, fWidth, fHeight);
    arrImages.push_back(image);
    g_WorldMapRenderer.SetImages(arrImages);
    g_WorldMapRenderer.SetRenderFlag(true);*/
	/*RtwWidget* worldLabel = LOAD_UI("lbworldmap");
	RtgViewWindowInfo WindosInfo = GetDevice()->GetViewWindowInfo(GetDevice()->GetViewWindowMode());
	RtwRect worldRect = RtwRect(WindosInfo.iPosX,WindosInfo.iPosY,WindosInfo.iPosX+WindosInfo.dwWidth,WindosInfo.iPosY+WindosInfo.dwHeight);
	worldLabel->MoveResize(worldRect);
	worldLabel->Show();
	g_workspace.SetFocusWidget(worldLabel);*/
}

const char* GenerateMapNpcHint(DWORD dwUserID)
{
    guard;
    GcActor* pActor = FindActor(dwUserID);
    if (pActor)
    {
        return pActor->GetName();
    }
    return "";
    unguard;
}

void UIFormMiniMap::OnGetNpcHintText(ui::RtwWidget* sender, RtwEventDelegate* e)
{
    const char* pTxt = GenerateMapNpcHint(m_lastNpcHint.id);
    g_workspace.SetHintText(pTxt, false);
    g_workspace.AutoHintSize();
    g_workspace.SetHintPosition(m_lastNpcHint.x, m_lastNpcHint.y);
}

void UIFormMiniMap::OnMapMouseMoveEvent(ui::RtwWidget* sender, RtwEventDelegate* e)
{
    OnMapMouseMove(e->mouse.x, e->mouse.y);
}

void UIFormMiniMap::OnMapMouseMove(int iMouseX, int iMouseY)
{
    guard;
    int i;
    for (i=0; i<m_iNpcCount; i++)
    {
        //if (abs(iMouseX-m_ptNpcs[i].x)<=2 && abs(iMouseY-m_ptNpcs[i].y)<=2)
        //{
        //    if (m_lastNpcHint.id!=m_ptNpcs[i].id)
        //    {
        //        m_lastNpcHint.id = m_ptNpcs[i].id;
        //        m_lastNpcHint.x = iMouseX;
        //        m_lastNpcHint.y = iMouseY;
        //        g_workspace.ShowHint(LOAD_UI("fmminimap.panmap"));
        //    }
        //    break;
        //}
		if (abs(iMouseX-m_vecNPCs[i].x)<=2 && abs(iMouseY-m_vecNPCs[i].y)<=2)
		{
			if (m_lastNpcHint.id!=m_vecNPCs[i].id)
			{
				m_lastNpcHint.id = m_vecNPCs[i].id;
				m_lastNpcHint.x = iMouseX;
				m_lastNpcHint.y = iMouseY;
				g_workspace.ShowHint(LOAD_UI("fmminimap.panmap"));
			}
			break;
		}
    }
    if (i>=m_iNpcCount)
    {
        m_lastNpcHint.id = 0;
        g_workspace.HideHint(0);
    }
    unguard;
}
/* 分线相关事件 gao 2010.1.30 */
void UIFormMiniMap::OnSelectChildThread(ui::RtwWidget* sender,RtwEventDelegate* e)
{
	guard;
	if(LOAD_UI("fmsubline")->IsVisible())
		LOAD_UI("fmsubline")->Hide();
	else
	{
		LOAD_UI("fmsubline")->Show();
		LOAD_UI_T(RtwListBox,"fmsubline.ltbservers")->RemoveAllItems();
		if(GetPlayer()->GetGroupID() && GetPlayer()->GetLineID())
		{
			CG_CmdPacket* pPacket = NetBeginWrite();
			pPacket->WriteShort(c2r_subsystem_linesys);
			pPacket->WriteShort(c2r_linesys_LineInfoReq);
			pPacket->WriteLong(GetPlayer()->GetGroupID());
			pPacket->WriteLong(GetPlayer()->GetLineID());
			NetSend(pPacket);
		}
	}
	unguard;
}

//主线任务响应函数
void UIFormMiniMap::OnClicked_MainTask(ui::RtwWidget* sender,RtwEventDelegate* e)
{
	if (g_layerMain->m_formMainTask->IsVisible())
	{
		g_layerMain->m_formMainTask->Hide();
	}
	else
	{
		g_layerMain->m_formMainTask->Show();
	}
}
void UIFormMiniMap::OnChildThreadButtonDown(ui::RtwWidget* sender,RtwEventDelegate* e)
{
	guard;
	if(sender == LOAD_UI("fmsubline.btncancel"))
		LOAD_UI("fmsubline")->Hide();
	if(sender == LOAD_UI("fmsubline.btninto"))
	{
		int iSelect = m_plstLineContent->GetSelectedItem();
		if(iSelect != -1)
		{
			DWORD newLine = atoi(m_plstLineContent->GetItemText(iSelect,0).c_str());
			if(newLine != GetPlayer()->GetLineID())
			{
				CG_CmdPacket* pPacket = NetBeginWrite();
				pPacket->WriteShort(c2r_subsystem_linesys);
				pPacket->WriteShort(c2r_linesys_SwitchLineReq);
				pPacket->WriteLong(GetPlayer()->GetGroupID());
				pPacket->WriteLong(newLine);
				pPacket->WriteLong(GetPlayer()->GetLineID());
				NetSend(pPacket);
				LOAD_UI("fmsubline")->Hide();
			}else
			{
				ShowSystemMessage(R(MSG_LINE_ERR_ALREADINLINE));
				LOAD_UI("fmsubline")->Hide();
			}
		}
	}
	unguard;
}
void UIFormMiniMap::OnReceiveLineCmd(CG_CmdPacket *pvPacket)
{
	guard;
	short type;
	pvPacket->ReadShort(&type);
	switch(type)
	{
	case r2c_linesys_SwitchLineRes:
		OnReceiveSwichLineRes(pvPacket);
		break;
	case r2c_linesys_LineInfoRes:
		OnReceiveLineInfo(pvPacket);
		break;
	case r2c_linesys_EnterLineNotify:
		OnReceiveEnterLineNotify(pvPacket);
		break;
	case r2c_linesys_LeaveLineNotify:
		OnReceiveLeaveLineNotify(pvPacket);
		break;
	default:
		break;
	}
	unguard;
}

void UIFormMiniMap::OnReceiveSwichLineRes(CG_CmdPacket *pvPacket)
{
	guard;
	DWORD	groupID;
	DWORD	lineID;
	long	userID;
	DWORD	errInfo;
	pvPacket->ReadLong((long*)&groupID);
	pvPacket->ReadLong((long*)&lineID);
	pvPacket->ReadLong((long*)&userID);
	pvPacket->ReadLong((long*)&errInfo);
	switch(errInfo)
	{
	case ERR_NULL:
		break;
	case ERR_FULL:
		ShowSystemMessage(R(MSG_LINE_ERR_FULL));
		break;
	case ERR_GENERAL:
		break;
	}
	unguard;
}

void UIFormMiniMap::OnReceiveLineInfo(CG_CmdPacket *pvPacket)
{
	guard;
	char tmpstr[50];
	RtwRect imgRc(448, 0, 501, 0);
	imgRc.left			-= 16;
	imgRc.right			+= 11;
	DWORD	groupID;
	DWORD	lineID;
	long	userID;
	DWORD	lineNum;
	pvPacket->ReadLong((long*)&groupID);
	pvPacket->ReadLong((long*)&lineID);
	pvPacket->ReadLong((long*)&userID);
	pvPacket->ReadLong((long*)&lineNum);
	m_lstLineInfo.clear();
	m_plstLineContent->RemoveAllItems();
	int i = 0;
	for(;i < lineNum; ++i)
	{
		SLineInfo linInfo;
		pvPacket->ReadData(&linInfo,sizeof(SLineInfo));
		m_lstLineInfo.push_back(linInfo);
		memset(tmpstr,0,50);
		rt2_sprintf(tmpstr,"%d线",linInfo.dwLineID);
		m_plstLineContent->AddItem();
		m_plstLineContent->SetItemText(i,0,tmpstr);
		int iGood,iGeneral,iNearFull,iFull;
		iGood			= linInfo.dwMaxUserCount * 0.2;
		iGeneral		= linInfo.dwMaxUserCount * 0.4;
		iNearFull		= linInfo.dwMaxUserCount * 0.6;
		iFull			= linInfo.dwMaxUserCount * 0.8;
		if(linInfo.dwUserCount <= iGood)
		{
			imgRc.bottom	= 44;
			imgRc.top		= 40;
		}
		else if(linInfo.dwUserCount <= iGeneral)
		{
			imgRc.bottom	= 30;
			imgRc.top		= 26;
		}
		else
		{
			imgRc.bottom	= 16;
			imgRc.top		= 12;
		}
		imgRc.top			-= 6;
		imgRc.bottom		+= 5;
		RtwImage* pImage	= g_workspace.getImageFactory()->createImage("ui\\textures\\10.tga");
		pImage->SetSourceRect(imgRc);
		pImage->SetBlend(true);
		m_plstLineContent->SetLineItemImage(i, 1, pImage);

        //lyymark y原来debug的代码 暂时注释掉
		/*if (g_bLogEnable)
		{
			memset(tmpstr,0,50);
			sprintf(tmpstr,"%d",linInfo.dwUserCount);
			m_plstLineContent->SetItemText(i,1,tmpstr);
		}*/
	}
	unguard;
}

void UIFormMiniMap::OnReceiveEnterLineNotify(CG_CmdPacket *pvPacket)
{
	guard;
	RtString strmessage;
	DWORD	groupID;
	DWORD	lineID;
	long	userID;
	DWORD	errInfo;
	pvPacket->ReadLong((long*)&groupID);
	pvPacket->ReadLong((long*)&lineID);
	pvPacket->ReadLong((long*)&userID);
	pvPacket->ReadLong((long*)&errInfo);
	switch(errInfo)
	{
	case ERR_NULL:
		strmessage.Format(R(MSG_LINE_ENTER_LINE),lineID);
		ShowSystemMessage(strmessage.c_str());
		GetPlayer()->SetGroupID(groupID);
		GetPlayer()->SetLineID(lineID);
		g_layerMain->m_fromMiniMap->SetSceneName();
		break;
	case ERR_FULL:
		break;
	case ERR_GENERAL:
		break;
	}
	unguard;
}

void UIFormMiniMap::OnReceiveLeaveLineNotify(CG_CmdPacket *pvPacket)
{
	guard;
	DWORD	groupID;
	DWORD	lineID;
	long	userID;
	DWORD	errInfo;
	pvPacket->ReadLong((long*)&groupID);
	pvPacket->ReadLong((long*)&lineID);
	pvPacket->ReadLong((long*)&userID);
	pvPacket->ReadLong((long*)&errInfo);
	switch(errInfo)
	{
	case ERR_NULL:
		GetPlayer()->SetGroupID(0);
		GetPlayer()->SetLineID(0);
		g_layerMain->m_fromMiniMap->SetSceneName();
		break;
	case ERR_FULL:
		break;
	case ERR_GENERAL:
		break;
	}
	unguard;
}
void UIFormMiniMap::RunAnimation()
{
	guard;
	if(m_bIsRunning)
	{
		static RtwRect rectBaseRect = m_pwdtMiniMap->getFrameRect();
		if(m_fStep == FSTEP)// 向上移动
		{
			RtwRect rectNowRect = m_pwdtMiniMap->getFrameRectRef();
			if(rectNowRect.bottom > rectBaseRect.top - 36)
			{
				rectNowRect.bottom -= m_fStep;
				rectNowRect.top -= m_fStep;
				m_pwdtMiniMap->Move(SPoint(rectNowRect.left,rectNowRect.top));
			}else
			{
				m_bIsRunning = false;
				m_fStep = 0;
			}
		}else if(m_fStep = -FSTEP)// 向下移动
		{
			RtwRect rectNowRect = m_pwdtMiniMap->getFrameRectRef();
			if(rectNowRect.bottom < rectBaseRect.bottom)
			{
				rectNowRect.bottom -= m_fStep;
				rectNowRect.top -= m_fStep;
				m_pwdtMiniMap->Move(SPoint(rectNowRect.left,rectNowRect.top));
			}else
			{
				m_bIsRunning = false;
				m_fStep = 0;
			}
		}
	}
	unguard;
}
void UIFormMiniMap::DoFrame()
{
	guard;
	RunAnimation();
	unguard;
}
void UIFormMiniMap::OnButtonLockCamera(ui::RtwWidget* sender, RtwEventDelegate* e)
{
	guard;
	if(GetWorld()->GetCameraMode() == GcWorld::CAMERA_MODE_TURN)
	{
		GetWorld()->SetCameraMode(GcWorld::CAMERA_MODE_LOCK);
		LOAD_UI_T(RtwButton, "fmminimap.btnperspective")->SetHintText("镜头已锁");
	}
	else
	{
		GetWorld()->SetCameraMode(GcWorld::CAMERA_MODE_TURN);
		LOAD_UI_T(RtwButton, "fmminimap.btnperspective")->SetHintText("镜头未锁");
	}
	if(g_layerMain->m_formSetting && g_layerMain->m_formSetting->IsVisible())
		g_layerMain->m_formSetting->Change();
	unguard;
}

/* end */

void UIFormMiniMap::GetMiniMapHint(ui::RtwWidget *sender, ui::RtwEventDelegate *e)
{
	guard;
	ReadHtml(R(MSG_MAP_HINT_AREAPOINT));
	unguard;
}

void UIFormMiniMap::OnRefreshCanAcceptTask()
{
	guard;
	g_layerMain->m_formTask->GetTaskByGroup(m_vectorCanAccept);
	unguard;
}
