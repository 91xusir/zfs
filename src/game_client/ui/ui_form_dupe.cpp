
#include "gc_include.h"
#include "ui_form_dupe.h"
#include "ui_form_select_reward.h"

UIFormDupe::UIFormDupe()
{
	m_pFrmThis  = RTW_WIDGET_T(RtwForm,"fmdupetask");
	m_pFrmThis->SetMovable(true);
	m_pHtmlView = RTW_WIDGET_T(RtwHtmlView,"fmdupetask.fmdupetaskright.viewright");
	m_pHtmlView->SetEnableInput(false);
	m_Close		= RTW_WIDGET_T(RtwButton, "fmdupetask.$hide");
	m_Close->EvLClick	  += RTW_CALLBACK(this, UIFormDupe, OnHide);
	m_pHtmlView->EvAction += RTW_CALLBACK(this , UIFormDupe, OnHtmlAction_HtmlHelpView);
	m_pHtmlView->EvLClick += RTW_CALLBACK(g_layerMain , UIFormDupe, OnHtmlAction_HtmlHelpView);
	m_pTree     = RTW_WIDGET_T(RtwTree,"fmdupetask.treelist");
	m_pTree->SetBorderpadding(false);
	m_pBtnFinishTask = RTW_WIDGET_T(RtwButton, "fmdupetask.btnfinishtask");

	m_pTree->EvSelect	+= RTW_CALLBACK(this, UIFormDupe, OnTreeNodeSelect);
	//m_pFrmThis->EvKey		+= RTW_CALLBACK(m_pFrmThis, RtwWidget, DefaultKeyDown);
	m_pBtnFinishTask->EvLClick+= RTW_CALLBACK(this, UIFormDupe, OnBtnFinishTaskClicked);

	m_strDif[0] = R(DUNGEON_NORMAL);
	m_strDif[1] = R(DUNGEON_DIFFICULT);
	m_strDif[2] = R(DUNGEON_HELL);
	m_strDif[3] = R(DUNGEON_LEGEND);
	m_strDif[4] = R(DUNGEON_MYTH);

	m_strFin[0] = R(DUNGEON_NOTFINISHED);
	m_strFin[1] = R(DUNGEON_FINISHED);
	m_strFin[2] = R(DUNGEON_CANFINISH);

	m_dwColor[0] = 0xFFFFFFFF;
	m_dwColor[1] = 0xFF00FF00;
	m_dwColor[2] = 0xFFFFFF00;
	m_dwColor[3] = 0xFFFF8000;
	m_dwColor[4] = 0xFF00FFFF;

	//m_dwColor[0] = 0xFF00FFFF;
	//m_dwColor[1] = 0xFFFF8000;
	//m_dwColor[2] = 0xFFFFFF00;
	//m_dwColor[3] = 0xFFFF0000;
	//m_dwColor[4] = 0xFFFFFFFF;

	//m_pTree->AddTreeItem();

	m_bHaveSReward = false;
}

UIFormDupe::~UIFormDupe()
{
}

bool UIFormDupe::IsVisible()
{
	guard;
	if ( m_pFrmThis && m_pFrmThis->GetFlags(wfVisible) )	return true;
	else	return false;
	unguard;
}

void UIFormDupe::Refresh()
{
	guard;

	if ( IsVisible() )
	{
		m_pTree->Clear();
		BuildTaskTree();
		m_pFrmThis->Show();
	}

	unguard;
}

void UIFormDupe::Hide()
{
	guard;
	//if (m_flags & wfVisible)
	//{
	//	m_flags &= ~wfVisible;

	//	OnHide();

	//	if (GetWorkspace()->GetModalWidget() == this)
	//	{
	//		GetWorkspace()->SetModalWidget(NULL);
	//	}
	//}
	m_pFrmThis->Hide();
	unguard;
}
void UIFormDupe::Show()
{
	guard;
	if (m_pFrmThis)
	{
		if (!IsVisible())
		{
			m_pTree->Clear();
			BuildTaskTree();
			m_pFrmThis->Show();
		}
	}
	unguard;
}

void UIFormDupe::OnTreeNodeSelect(RtwWidget* pSender, RtwEventDelegate* pEvent)
{
	guard;
	RtwTree::Item* pLastItem = m_pTree->GetLastSelect();

	SetSelectedTaskID();

	if (pLastItem && pLastItem->link!="")
	{
		m_strURL = pLastItem->link;
	}else
	{
		m_strURL = "help/bank.html";
	}
	m_pHtmlView->LoadFromFile(m_strURL.c_str());
	unguard;
}

void UIFormDupe::OnUIHyperlink(RtwWidget* pSender, RtwEventDelegate* pEvent)
{
	guard;
	if (!IsVisible())
	{
		m_pFrmThis->Show();
	}
	m_strURL = ((RtwHyperlink*)pSender)->GetURL();
	if (m_strURL=="")
	{
		m_strURL = "help/bank.html";
	}
	m_pHtmlView->LoadFromFile(m_strURL.c_str());
	unguard;
}

void UIFormDupe::SetURL(const char* szURL)
{
	m_strURL = szURL?szURL:"";
	if (m_strURL=="")
	{
		m_strURL = "help/bank.html";
	}
	m_pHtmlView->LoadFromFile(m_strURL.c_str());
}

const char* UIFormDupe::GetURL()
{
	return m_strURL.c_str();
}

void UIFormDupe::SetSelectedTaskID()
{
	if(!m_pTree)	return;

	if(!m_pTree->GetLastSelect())	return;

	std::string text = m_pTree->GetLastSelect()->text;

	std::vector<ST_FbTask>::iterator	it_fb = m_task.begin();
	std::vector<ST_TaskInfo>::iterator	it_task = it_fb->tasklist.begin();

	for(; it_fb != m_task.end(); it_fb++)
	{
		for(it_task = it_fb->tasklist.begin(); it_task != it_fb->tasklist.end(); it_task++)
		{
			if(text == it_task->taskname + m_strFin[it_task->ifinished])
			{
				m_taskID = it_task->taskid;
				return;
			}
		}
	}
}

void UIFormDupe::OnBtnFinishTaskClicked(RtwWidget* sender, void*)
{
	//if(!m_pTree)	return;
	//
	//if(!m_pTree->GetLastSelect())	return;
	//
	//long task_id	= 0;
	//bool bfind		= false;
	//bool bFinished	= false;
	//bool bCanFinish	= false;

	//std::string text = m_pTree->GetLastSelect()->text;

	//std::vector<ST_FbTask>::iterator	it_fb = m_task.begin();
	//std::vector<ST_TaskInfo>::iterator	it_task = it_fb->tasklist.begin();

	//for(; it_fb != m_task.end(); it_fb++)
	//{
	//	for(it_task = it_fb->tasklist.begin(); it_task != it_fb->tasklist.end(); it_task++)
	//	{
	//		if(text == it_task->taskname + m_strFin[it_task->ifinished])
	//		{
	//			task_id = it_task->taskid;
	//			bfind = true;
	//			break;
	//		}
	//	}

	//	if(bfind) break;
	//}
	
	//if(bfind)
	//{
	bool bFinished	= false;
	bool bCanFinish	= false;

	GetWorld()->m_pPlayer->FbTaskState(m_taskID, bFinished, bCanFinish);

	if(!bFinished && bCanFinish)
	{		
		if(m_bHaveSReward)
		{
			g_layerMain->m_formSelectReward->ResetRewardIDs(false, m_taskID, m_rID);
			Hide();
			g_layerMain->m_formSelectReward->Show();

			m_rID.clear();
			SetHaveSReward(false);
			return;
		}
		
		CG_CmdPacket* pPacket = NetBeginWrite();
		pPacket->WriteShort(c2r_finish_fb_task);
		pPacket->WriteLong(m_taskID);
		pPacket->WriteLong(0);
		NetSend(pPacket);

		m_rID.clear();
		SetHaveSReward(false);
	}
	//}
}

void UIFormDupe::OnHtmlAction_HtmlHelpView(RtwWidget* sender, void*pParam)
{
	 guard;
	 RtwEventDelegate* pEvent = (RtwEventDelegate*)pParam;
	 const char* szHref;

	 if (pEvent->type==weMouseClick)
	 {
		  //szHref = (const char*)(pEvent->hyperLink.pHLink->Text.c_str());
		  RtwTextBox::SItem* pItem = ((RtwHtmlView*)sender)->PickItem(SPoint(pEvent->mouse.x,pEvent->mouse.y));
		  if(pItem) szHref = (const char*)(pItem->hyperLink.Text.c_str());
		  else return ;
		  if (strncmp(szHref, "//viewsreward",13) == 0)
		  {
			  // ²ì¿´Ñ¡Ôñ½±Àø
			  long taskId;
			  long reward[5];
			  for(int i=0; i<5; i++)
				  reward[i] = 0;
			  sscanf(&szHref[13],"%d,%d,%d,%d,%d,%d",&taskId,&reward[0],&reward[1],&reward[2],
				  &reward[3],&reward[4]);

			  vector<DWORD> rw;
			  for(int i=0; i<5; i++)
				  if(reward[i]!=0) rw.push_back(reward[i]); 

			  g_layerMain->m_formSelectReward->ResetRewardIDs(true,taskId,rw);
			  g_layerMain->m_formSelectReward->Show();
		  }
	 }
	 unguard;
}

void UIFormDupe::BuildTaskTree()
{
	guard;
	std::vector<SFbTaskInfo>::iterator it;
	SFuBen*	pfb = NULL;
	
	m_task.clear();
	
	it = GetWorld()->m_pPlayer->m_task.m_fbTaskList.begin();

	for(; it != GetWorld()->m_pPlayer->m_task.m_fbTaskList.end(); it++)
	{		
		pfb = g_TableFB.Find(it->fbId);

		if(pfb)
		{
			std::vector<long>			taskid;
			std::vector<long>::iterator	it_task;
			SFbTask*					pfbtask;
			ST_FbTask					fbtmp;
			bool						bFinished = false;
			bool						bCanFinish = false;
			
			g_TableTask.GetTaskByFb(pfb->id, it->curDif, taskid);

			for(it_task = taskid.begin(); it_task != taskid.end(); it_task++)
			{
				ST_TaskInfo	infotmp;

				pfbtask = g_TableTask.FindFbTask(*it_task);

				infotmp.taskid		= pfbtask->id;
				infotmp.taskname	= pfbtask->name;
				infotmp.link		= pfbtask->taskDesc[it->curDif];
				GetWorld()->m_pPlayer->FbTaskState(*it_task, bFinished, bCanFinish);
				if(bFinished)
				{
					infotmp.ifinished = 1;
				}else
				{
					infotmp.ifinished = bCanFinish ? 2 : 0;
				}

				fbtmp.tasklist.push_back(infotmp);				
			}

			fbtmp.fbname	= pfb->name;
			fbtmp.fblevel	= it->curDif;
			fbtmp.fblink	= pfb->html;

			m_task.push_back(fbtmp);
		}
	}

	std::vector<ST_FbTask>::iterator	it_fb = m_task.begin();

	for(; it_fb != m_task.end(); it_fb++)
	{
		std::string root = it_fb->fbname + m_strDif[it_fb->fblevel];
		
		m_pTree->InsertTreeItem(NULL, root.c_str(), it_fb->fblink.c_str());

		std::vector<ST_TaskInfo>::iterator it_taskinfo;
		it_taskinfo = it_fb->tasklist.begin();

		for(; it_taskinfo != it_fb->tasklist.end(); it_taskinfo++)
		{
			std::string child = it_taskinfo->taskname + m_strFin[it_taskinfo->ifinished];
			
			m_pTree->InsertTreeItem(root.c_str(), 
									child.c_str(),
									it_taskinfo->link.c_str());
		}

		m_pTree->SetItemColor(root.c_str(), m_dwColor[it_fb->fblevel]);
	}
	unguard;
}

void UIFormDupe::OnHide(RtwWidget* sender,void*)
{
	Hide();
}