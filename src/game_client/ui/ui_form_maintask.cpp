#include "gc_include.h"
#include "ui_form_maintask.h"

UIFormMainTask::UIFormMainTask()
{
	m_CloseTree		= LOAD_UI_T(RtwButton, "fmtaskline.$hide");
	m_CloseTree->EvLClick	  += RTW_CALLBACK(this, UIFormMainTask, HideTaskTree);
	m_CloseDesc		= LOAD_UI_T(RtwButton, "fmtasklinedesc.$hide");
	m_CloseDesc->EvLClick	  += RTW_CALLBACK(this, UIFormMainTask, HideTaskDesc);

	m_pTreeList     = LOAD_UI_T(RtwTree,"fmtaskline.treetask");
	m_pTreeList->SetBorderpadding(false);
	m_pTreeList->EvSelect	+= RTW_CALLBACK(this, UIFormMainTask, OnTreeNodeSelect);
}

UIFormMainTask::~UIFormMainTask()
{
}

bool UIFormMainTask::IsVisible()
{
	guard;
	if (LOAD_UI("fmtaskline")->IsVisible() && LOAD_UI("fmtasklinedesc")->IsVisible())	return true;
	else	return false;
	unguard;
}

void UIFormMainTask::Update()
{
	guard;
	m_pTreeList->Clear();
	unguard;
}

void UIFormMainTask::Show()
{
	guard;
	m_pTreeList->Clear();
	ShowTaskTree();
	BuildTaskTree();
	unguard;
}

void UIFormMainTask::ShowTaskTree()
{
	LOAD_UI("fmtaskline")->Show();
}

void UIFormMainTask::ShowTaskDesc()
{
	LOAD_UI("fmtasklinedesc")->Show();
}

void UIFormMainTask::Hide()
{
	guard;
	HideTaskTree(NULL, NULL);
	HideTaskDesc(NULL, NULL);
	unguard;
}

void UIFormMainTask::HideTaskTree(RtwWidget* sender,void*)
{
	LOAD_UI("fmtaskline")->Hide();
}

void UIFormMainTask::HideTaskDesc(RtwWidget* sender,void*)
{
	LOAD_UI("fmtasklinedesc")->Hide();
}

void UIFormMainTask::OnTreeNodeSelect(RtwWidget* pSender, RtwEventDelegate* pEvent)
{
	guard;
	if(!m_pTreeList)	return;
	if(!m_pTreeList->GetLastSelect())	return;

	vector<int>::iterator itGroup;
	vector<MainStoryData>::iterator itLine;
	//点中相应的章
	for (itGroup = taskChapterId.begin(); itGroup != taskChapterId.end(); itGroup++)
	{
		TaskGroupData* taskGroup = g_TaskGroup.FindTaskGroup(*itGroup);
		if(m_pTreeList->getSelectItem()->text == taskGroup->strGroupName) 
		{
			if (LOAD_UI("fmtasklinedesc")->IsVisible())
			{
				LOAD_UI("fmtasklinedesc")->Hide();
			}
			else
			{
				LOAD_UI("fmtasklinedesc")->Show();
			}
		}
	}

	//点中相应的节
	for(itLine = taskMainLine.begin(); itLine != taskMainLine.end(); itLine++)
	{
		if(m_pTreeList->getSelectItem()->text == (*itLine).strDataName)
		{
			ShowTaskDesc();
			std::string strImage = "ui_texture/";
			if ((*itLine).nDataID < section)
			{
				strImage += (*itLine).strImage1;
			} 
			else if ((*itLine).nDataID > section)
			{
				strImage += (*itLine).strImage2;
			}
			else
			{
				strImage += (*itLine).strImage1;
			}
			strImage += ".tga";

			if (!strImage.empty())
			{
				RtwImage *pImage = g_workspace.getImageFactory()->createImage(strImage);
				if (pImage)
				{
					pImage->SetBlend(true);
					LOAD_UI("fmtasklinedesc.lbtasklinedesc")->SetBackgroundImage(pImage);
				}
			}
		}
	}

	unguard;
}

void UIFormMainTask::BuildTaskTree()
{
	taskChapterId.clear();
	taskMainLine.clear();

	CTaskInfo *task = &GetWorld()->m_pPlayer->m_task;
	section = task->m_nMainTaskData;

	if (!g_TaskGroup.GetMainLineID(taskChapterId)) 
		return;
	vector<int>::iterator itGroup;
	for (itGroup = taskChapterId.begin(); itGroup != taskChapterId.end(); itGroup++)
	{
		TaskGroupData* taskGroup = g_TaskGroup.FindTaskGroup(*itGroup);
		m_pTreeList->InsertTreeItem(NULL, taskGroup->strGroupName.c_str(), "");
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
				m_pTreeList->InsertTreeItem(taskGroup->strGroupName.c_str(), 
						(*itLine).strDataName.c_str(),
						"", 0,-1, 0);
				if ((*itLine).nDataID < section)
				{
					m_pTreeList->SetItemColor((*itLine).strDataName.c_str(), 0xFFFFD700);
				} 
				else if ((*itLine).nDataID > section)
				{
					m_pTreeList->SetItemColor((*itLine).strDataName.c_str(), 0xFF939393);
				}
			}
		}
	}
	//展开树项
	if (section)
	{
		MainStoryData* data = g_TableMainStory.FindMainStory(section);
		if(data)
		{
			RtwTree::Item* pItem = m_pTreeList->FindTreeItemByText(data->strDataName.c_str());
			if (pItem)
			{
				TaskGroupData* taskGroup = g_TaskGroup.FindTaskGroup(data->nGroupID);
				RtwTree::Item* pItemGroup = m_pTreeList->FindTreeItemByText(taskGroup->strGroupName.c_str());
				m_pTreeList->Expand(pItemGroup);
				m_pTreeList->Select(pItem);
			} 
		}
	}
	else
	{
		m_pTreeList->Expand(m_pTreeList->getFirstItem());
		m_pTreeList->Select(m_pTreeList->getFirstItem()->pFirstChild);
	}

	SetSelectedSectionID(section);
}

void UIFormMainTask::SetSelectedSectionID(int sectionId)
{
	MainStoryData* data = g_TableMainStory.FindMainStory(sectionId);
	if (data)
	{
		ShowTaskDesc();
		std::string strImage = "ui_texture/";
		strImage += data->strImage1;
		strImage += ".tga";

		if (!strImage.empty())
		{
			RtwImage *pImage = g_workspace.getImageFactory()->createImage(strImage);
			if (pImage)
			{
				pImage->SetBlend(true);
				LOAD_UI("fmtasklinedesc.lbtasklinedesc")->SetBackgroundImage(pImage);
			}
		}
	}
}