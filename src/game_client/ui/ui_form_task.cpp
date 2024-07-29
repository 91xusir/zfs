
#include "gc_include.h"
#include "ui_form_task.h"
#include "ui_form_msg.h"
#include "ui_form_char.h"

UIFormTask::UIFormTask()
{
	m_pFrmThis  = LOAD_UI_T(RtwForm,"fmmaintask");
	m_pFrmThis->SetMovable(true);

	m_pTree1     = LOAD_UI_T(RtwTree,"fmmaintask.fmtask.tabitem.fmcurrenttask.treetask");
	m_pTree1->SetBorderpadding(false);
	m_pTree1->EvSelect	+= RTW_CALLBACK(this, UIFormTask, OnTreeNodeSelect1);
	m_pTree1->InitItemImage(2, 1, SSize(16, 16), SSize(16, 16));

	m_pTree2     = LOAD_UI_T(RtwTree,"fmmaintask.fmtask.tabitem.fmaccepttask.treetask");
	m_pTree2->SetBorderpadding(false);
	m_pTree2->EvSelect	+= RTW_CALLBACK(this, UIFormTask, OnTreeNodeSelect2);

	m_pTree3     = LOAD_UI_T(RtwTree,"fmmaintask.fmtask.tabitem.fmsharetask.treetask");
	m_pTree3->SetBorderpadding(false);
	m_pTree3->EvSelect	+= RTW_CALLBACK(this, UIFormTask, OnTreeNodeSelect3);

	m_tab = LOAD_UI_T(RtwTab,"fmmaintask.fmtask.tabitem");
	m_tab->EvChangeSelected += RTW_CALLBACK(this, UIFormTask, ChangeSelectTab);
	m_taskDesc1 = LOAD_UI_T(RtwHtmlView,"fmmaintask.fmtask.tabitem.fmcurrenttask.fmtask1.fmhtmldesc.htmldesc");
	LOAD_UI("fmmaintask.fmtask.tabitem.fmcurrenttask.fmtask1")->SetAutoScrollV(true);

	m_taskDesc1->SetAutoSizeV(true);
	m_taskDesc1->ModifyFlags(wfGrabMouse,0);
	m_taskDesc1->SetEnableInput(false);
	m_taskDesc1->EvLClick += RTW_CALLBACK(this , UILayerMain, OnHtmlTask_HtmlHelpView);
	//奖励LABLE
	m_LableReward = LOAD_UI_T(RtwLabel, "fmmaintask.fmtask.tabitem.fmcurrenttask.fmtask1.lbprizetext");
	//固定奖励
	m_formFix = LOAD_UI_T(RtwForm, "fmmaintask.fmtask.tabitem.fmcurrenttask.fmtask1.fmfixreward");
	m_FixLable1 = LOAD_UI_T(RtwLabel, "fmmaintask.fmtask.tabitem.fmcurrenttask.fmtask1.fmfixreward.lbreward1");
	m_LableImage1 = LOAD_UI_T(RtwLabel, "fmmaintask.fmtask.tabitem.fmcurrenttask.fmtask1.fmfixreward.lbreward1.lbicon1");
	m_LableImage1->ModifyFlag(wfGrabMouse|wfMouseMove|wfHint,0);
	m_LableImage1->EvHint += RTW_CALLBACK(this, UIFormTask, OnShowHintText);
	m_LableText1 = LOAD_UI_T(RtwLabel, "fmmaintask.fmtask.tabitem.fmcurrenttask.fmtask1.fmfixreward.lbreward1.lbcenter.lbtext1");
	m_FixLable2 = LOAD_UI_T(RtwLabel, "fmmaintask.fmtask.tabitem.fmcurrenttask.fmtask1.fmfixreward.lbreward2");
	m_LableImage2 = LOAD_UI_T(RtwLabel, "fmmaintask.fmtask.tabitem.fmcurrenttask.fmtask1.fmfixreward.lbreward2.lbicon2");
	m_LableText2 = LOAD_UI_T(RtwLabel, "fmmaintask.fmtask.tabitem.fmcurrenttask.fmtask1.fmfixreward.lbreward2.lbcenter.lbtext2");
	m_FixLable3 = LOAD_UI_T(RtwLabel, "fmmaintask.fmtask.tabitem.fmcurrenttask.fmtask1.fmfixreward.lbreward3");
	m_LableImage3 = LOAD_UI_T(RtwLabel, "fmmaintask.fmtask.tabitem.fmcurrenttask.fmtask1.fmfixreward.lbreward3.lbicon3");
	m_LableText3 = LOAD_UI_T(RtwLabel, "fmmaintask.fmtask.tabitem.fmcurrenttask.fmtask1.fmfixreward.lbreward3.lbcenter.lbtext3");
	m_FixLable4 = LOAD_UI_T(RtwLabel, "fmmaintask.fmtask.tabitem.fmcurrenttask.fmtask1.fmfixreward.lbreward4");
	m_LableImage4 = LOAD_UI_T(RtwLabel, "fmmaintask.fmtask.tabitem.fmcurrenttask.fmtask1.fmfixreward.lbreward4.lbicon4");
	m_LableText4 = LOAD_UI_T(RtwLabel, "fmmaintask.fmtask.tabitem.fmcurrenttask.fmtask1.fmfixreward.lbreward4.lbcenter.lbtext4");
	//固定奖励道具
	m_formFixItem = LOAD_UI_T(RtwLabel, "fmmaintask.fmtask.tabitem.fmcurrenttask.fmtask1.fmfixreward.fmitemall");
	m_FixItem1 = LOAD_UI_T(RtwLabel, "fmmaintask.fmtask.tabitem.fmcurrenttask.fmtask1.fmfixreward.fmitemall.fmitem1");
	m_ItemImage1 = LOAD_UI_T(RtwButton, "fmmaintask.fmtask.tabitem.fmcurrenttask.fmtask1.fmfixreward.fmitemall.fmitem1.fmicon.btnicon");
	m_ItemImage1->ModifyFlag(wfGrabMouse|wfMouseMove|wfHint,0);
	m_ItemImage1->EvHint += RTW_CALLBACK(this, UIFormTask, OnShowHintText);
	m_ItemText1 = LOAD_UI_T(RtwButton, "fmmaintask.fmtask.tabitem.fmcurrenttask.fmtask1.fmfixreward.fmitemall.fmitem1.lbtext");
	m_FixItem2 = LOAD_UI_T(RtwLabel, "fmmaintask.fmtask.tabitem.fmcurrenttask.fmtask1.fmfixreward.fmitemall.fmitem2");
	m_ItemImage2 = LOAD_UI_T(RtwButton, "fmmaintask.fmtask.tabitem.fmcurrenttask.fmtask1.fmfixreward.fmitemall.fmitem2.fmicon.btnicon");
	m_ItemText2 = LOAD_UI_T(RtwButton, "fmmaintask.fmtask.tabitem.fmcurrenttask.fmtask1.fmfixreward.fmitemall.fmitem2.lbtext");
	m_FixItem3 = LOAD_UI_T(RtwLabel, "fmmaintask.fmtask.tabitem.fmcurrenttask.fmtask1.fmfixreward.fmitemall.fmitem3");
	m_ItemImage3 = LOAD_UI_T(RtwButton, "fmmaintask.fmtask.tabitem.fmcurrenttask.fmtask1.fmfixreward.fmitemall.fmitem3.fmicon.btnicon");
	m_ItemText3 = LOAD_UI_T(RtwButton, "fmmaintask.fmtask.tabitem.fmcurrenttask.fmtask1.fmfixreward.fmitemall.fmitem3.lbtext");
	m_FixItem4 = LOAD_UI_T(RtwLabel, "fmmaintask.fmtask.tabitem.fmcurrenttask.fmtask1.fmfixreward.fmitemall.fmitem4");
	m_ItemImage4 = LOAD_UI_T(RtwButton, "fmmaintask.fmtask.tabitem.fmcurrenttask.fmtask1.fmfixreward.fmitemall.fmitem4.fmicon.btnicon");
	m_ItemText4 = LOAD_UI_T(RtwButton, "fmmaintask.fmtask.tabitem.fmcurrenttask.fmtask1.fmfixreward.fmitemall.fmitem4.lbtext");
	//随机奖励道具
	m_formRandom = LOAD_UI_T(RtwForm, "fmmaintask.fmtask.tabitem.fmcurrenttask.fmtask1.fmrandomreward");
	m_RandomItem1 = LOAD_UI_T(RtwLabel, "fmmaintask.fmtask.tabitem.fmcurrenttask.fmtask1.fmrandomreward.fmitem1");
	m_RandomImage1 = LOAD_UI_T(RtwButton, "fmmaintask.fmtask.tabitem.fmcurrenttask.fmtask1.fmrandomreward.fmitem1.fmicon.btnicon");
 	m_RandomImage1->ModifyFlag(wfGrabMouse|wfMouseMove|wfHint,0);
	m_RandomImage1->EvHint += RTW_CALLBACK(this, UIFormTask, OnShowHintText);
	m_RandomImage1->EvLClick += RTW_CALLBACK(this, UIFormTask, OnShowHintText);
	m_RandomText1 = LOAD_UI_T(RtwLabel, "fmmaintask.fmtask.tabitem.fmcurrenttask.fmtask1.fmrandomreward.fmitem1.lbtext");
	m_RandomItem2 = LOAD_UI_T(RtwLabel, "fmmaintask.fmtask.tabitem.fmcurrenttask.fmtask1.fmrandomreward.fmitem2");
	m_RandomImage2 = LOAD_UI_T(RtwButton, "fmmaintask.fmtask.tabitem.fmcurrenttask.fmtask1.fmrandomreward.fmitem2.fmicon.btnicon");
	m_RandomImage2->ModifyFlag(wfGrabMouse|wfMouseMove|wfHint,0);
	m_RandomImage2->EvHint += RTW_CALLBACK(this, UIFormTask, OnShowHintText);
	m_RandomImage2->EvLClick += RTW_CALLBACK(this, UIFormTask, OnShowHintText);
	m_RandomText2 = LOAD_UI_T(RtwLabel, "fmmaintask.fmtask.tabitem.fmcurrenttask.fmtask1.fmrandomreward.fmitem2.lbtext");
	m_RandomItem3 = LOAD_UI_T(RtwLabel, "fmmaintask.fmtask.tabitem.fmcurrenttask.fmtask1.fmrandomreward.fmitem3");
	m_RandomImage3 = LOAD_UI_T(RtwButton, "fmmaintask.fmtask.tabitem.fmcurrenttask.fmtask1.fmrandomreward.fmitem3.fmicon.btnicon");
	m_RandomText3 = LOAD_UI_T(RtwLabel, "fmmaintask.fmtask.tabitem.fmcurrenttask.fmtask1.fmrandomreward.fmitem3.lbtext");
	m_RandomItem4 = LOAD_UI_T(RtwLabel, "fmmaintask.fmtask.tabitem.fmcurrenttask.fmtask1.fmrandomreward.fmitem4");
	m_RandomImage4 = LOAD_UI_T(RtwButton, "fmmaintask.fmtask.tabitem.fmcurrenttask.fmtask1.fmrandomreward.fmitem4.fmicon.btnicon");
	m_RandomText4 = LOAD_UI_T(RtwLabel, "fmmaintask.fmtask.tabitem.fmcurrenttask.fmtask1.fmrandomreward.fmitem4.lbtext");

	m_taskDesc2 = LOAD_UI_T(RtwHtmlView,"fmmaintask.fmtask.tabitem.fmaccepttask.fmtask2.fmhtmlaccept.htmlaccept");
	m_taskDesc2->ModifyFlags(wfGrabMouse,0);
	m_taskDesc2->SetEnableInput(false);
	m_taskDesc2->EvLClick += RTW_CALLBACK(this , UILayerMain, OnHtmlTask_HtmlHelpView);
	m_taskDesc3 = LOAD_UI_T(RtwHtmlView,"fmmaintask.fmtask.tabitem.fmsharetask.fmtask3.fmhtmlshare.htmlshare");
	m_taskDesc3->ModifyFlags(wfGrabMouse,0);
	m_taskDesc3->SetEnableInput(false);
	m_taskDesc3->EvLClick += RTW_CALLBACK(this , UILayerMain, OnHtmlTask_HtmlHelpView);

	m_Close		= LOAD_UI_T(RtwButton, "fmmaintask.$hide");
	m_Close->EvLClick	  += RTW_CALLBACK(this, UIFormTask, OnHide);

	//任务追踪
	LOAD_UI("fmmaintask.fmtaskbtn1.cktasktrack")->EvLClick += RTW_CALLBACK(this, UIFormTask, OnClickTaskTrack);
	LOAD_UI_T(CUiCheckButton, "fmmaintask.fmtaskbtn1.cktasktrack")->SetChecked(true);

	m_pTaskTrack = LOAD_UI_T(RtwButton, "fmmaintask.fmtaskbtn1.btntasktrack");
	m_pTaskTrack->EvLClick += RTW_CALLBACK(this, UIFormTask, OnClick_TrackTask);

	//任务共享
	m_pTaskShared = LOAD_UI_T(RtwButton, "fmmaintask.fmtaskbtn1.btnsharetask");
	m_pTaskShared->EvLClick += RTW_CALLBACK(this, UIFormTask, OnClicked_ShareTask);

	m_pTaskCancel = LOAD_UI_T(RtwButton, "fmmaintask.fmtaskbtn1.btncanceltask");
	m_pTaskCancel->EvLClick += RTW_CALLBACK(this, UIFormTask, OnClickCancelTask);

	//任务共享面板接受取消
	m_pTaskAccept = LOAD_UI_T(RtwButton, "fmmaintask.fmtaskbtn3.btnaccept");
	m_pTaskAccept->EvLClick += RTW_CALLBACK(this, UIFormTask, OnClicked_AcceptShareTask);

	m_pSharedCancel = LOAD_UI_T(RtwButton, "fmmaintask.fmtaskbtn3.btncancel");

	//关闭按钮
	LOAD_UI("fmmaintask.fmtaskbtn1.btnclose")->EvLClick += RTW_CALLBACK(this, UIFormTask, OnTaskClose);
	LOAD_UI("fmmaintask.fmtaskbtn2.btnclose")->EvLClick += RTW_CALLBACK(this, UIFormTask, OnTaskClose);
	LOAD_UI("fmmaintask.fmtaskbtn3.btnclose")->EvLClick += RTW_CALLBACK(this, UIFormTask, OnTaskClose);

	LOAD_UI("fmmaintask.fmtaskbtn1")->Show();

	m_LastTaskId1 = 0;
	m_LastTaskId2 = 0;
	m_LastTaskId3 = 0;
 	m_pFrmThis->Offset(SSize(-1, -1),	false, false);
 	m_pFrmThis->Offset(SSize(1, -1),	false, false);
}

UIFormTask::~UIFormTask()
{
}

bool UIFormTask::IsVisible()
{
	guard;
	if ( m_pFrmThis && m_pFrmThis->GetFlags(wfVisible) )	return true;
	else	return false;
	unguard;
}

void UIFormTask::Update()
{
	guard;
	m_LastTaskId1 = 0;
	if(IsVisible())
	{
		m_pTree1->Clear();
		m_pTree2->Clear();
		m_pTree3->Clear();
		BuildTaskTree1();
		BuildTaskTree2();
		BuildTaskTree3();
	}
	unguard;
}

void UIFormTask::ResetRewardText()
{
	m_LableText1->SetText("");
	m_LableText2->SetText("");
	m_LableText3->SetText("");
	m_LableText4->SetText("");

	m_ItemText1->SetText("");
	m_ItemText2->SetText("");
	m_ItemText3->SetText("");
	m_ItemText4->SetText("");

	m_RandomText1->SetText("");
	m_RandomText2->SetText("");
	m_RandomText3->SetText("");
	m_RandomText4->SetText("");
}

void UIFormTask::ResetRewardImage()
{
	m_FixItem1->SetBackgroundImage(NULL);
	m_FixItem2->SetBackgroundImage(NULL);
	m_FixItem3->SetBackgroundImage(NULL);
	m_FixItem4->SetBackgroundImage(NULL);

	m_RandomItem1->SetBackgroundImage(NULL);
	m_RandomItem2->SetBackgroundImage(NULL);
	m_RandomItem3->SetBackgroundImage(NULL);
	m_RandomItem4->SetBackgroundImage(NULL);

}

void UIFormTask::ParseFixReward(STaskDesc* desc)
{
	m_FixLable1->Hide();
	m_FixLable2->Hide();
	m_FixLable3->Hide();
	m_FixLable4->Hide();

	m_FixItem1->Hide();
	m_FixItem2->Hide();
	m_FixItem3->Hide();
	m_FixItem4->Hide();

	//小图标
	std::string strGoldName = "ui_texture/pic_t_jinqian1.dds";
	std::string strExpName = "ui_texture/pic_t_jingyan.dds";
	std::string strLingName = "ui_texture/pic_t_lingli.dds";
	std::string strXiuName = "ui_texture/pic_t_xiuwei.dds";
	//大图标
	std::string strMengName = "ui_texture/pic_t_mengong.dds";
	std::string strBangName = "ui_texture/pic_t_banggong.dds";
	std::string strSuiName = "ui_texture/pic_t_suming.dds";
	std::string strXianName = "ui_texture/pic_t_xianyu.dds";
	std::string strSjgfName = "ui_texture/pic_t_sjgf.dds";
	std::string strFqmzName = "ui_texture/pic_t_fqmz.dds";
	std::string strZyysName = "ui_texture/pic_t_zyys.dds";
	std::string strFsyjName = "ui_texture/pic_t_fsyj.dds";
	std::string strXmlmName = "ui_texture/pic_t_xmlm.dds";
	std::string strTaskName = "ui_texture/pic_t_renwu.dds";
	std::string strSkillName = "ui_texture/pic_t_sp.dds";
	std::string strYuanName = "ui_texture/pic_t_yuanbao.dds";
	std::string strZengName = "ui_texture/pic_t_zengdian.dds";
	std::string strJiName = "ui_texture/pic_t_jifen.dds";
	std::string strZhuanName = "ui_texture/pic_t_zhuanzhi.dds";
	std::string strJunName = "ui_texture/pic_t_jungong.dds";

	std::vector<STaskKey>& table = desc->fixReward;

	for(size_t i = 0; i < table.size(); ++i)
	{
		if (table[i].key == "gold")
		{
			ShowRewardLable(strGoldName, table[i].param[0]);
		} 
		else if(table[i].key == "exp")
		{
			ShowRewardLable(strExpName, table[i].param[0]);
		}
		else if (table[i].key == "task")
		{
			DWORD taskId = atol(table[i].param[0].c_str());
			if (taskId != 0)
			{
				STaskDesc *desc = g_TableTask.FindTask(taskId);
				if (desc)
				{
					ShowRewardItem(strTaskName, desc->name);
				}
			}
		}
		else if (table[i].key == "item")
		{
			DWORD itemId = atol(table[i].param[0].c_str());
			int itemNum = atol(table[i].param[1].c_str());
			if (itemId != 0)
			{
				SItemBase *pItemClass = GetWorld()->m_pItemManager->GetItemFromIndex(itemId);
				if(pItemClass)
				{
					std::string itemIcon = pItemClass->icon;
					std::string itemName = pItemClass->name;
					ShowRewardItem(itemIcon, itemName);
				}
			}
		} 
		else if (table[i].key == "skill")
		{
			DWORD skillId = atol(table[i].param[0].c_str());
			if (skillId != 0)
			{
				SSkill* pSkill = Skill()->FindSkill(skillId);
				if (pSkill)
				{
					std::string skillIcon = pSkill->szIcon;
					std::string skillName = pSkill->szName;
					ShowRewardItem(skillIcon, skillName);
				}
			}
		} 
		else if (table[i].key == "moureward")
		{
			std::string strGold = "金钱若干";
			ShowRewardLable(strGoldName, strGold);
			std::string strSui = "俗名若干";
			ShowRewardItem(strSuiName, strSui);
		}
		else if (table[i].key == "wenreward")
		{
			std::string str = "修为若干";
			ShowRewardLable(strXiuName, str);
		} 
		else if (table[i].key == "gongreward")
		{
			std::string str = "门贡若干";
			ShowRewardItem(strMengName, str);
		} 
		else if (table[i].key == "jobrecord")
		{
			ShowRewardItem(strMengName,table[i].param[0]);
		} 
		else if(table[i].key == "credit")
		{
			int creditNum1 = atol(table[i].param[0].c_str());
			if (creditNum1 == 0)
			{
				ShowRewardItem(strBangName, table[i].param[1]);
			} 
			else if(creditNum1 == 1)
			{
				ShowRewardItem(strSuiName, table[i].param[1]);
			}
			else if (creditNum1 == 2)
			{
				ShowRewardLable(strLingName, table[i].param[1]);
			} 
			else if(creditNum1 == 3)
			{
				ShowRewardLable(strXiuName, table[i].param[1]);
			}
			else if (creditNum1 == 4)
			{
				int creditNum2 = atol(table[i].param[1].c_str());
				if (creditNum2 == 0)
				{
					ShowRewardItem(strXianName, table[i].param[2]);
				} 
				else if(creditNum2 == 1)
				{
					ShowRewardItem(strSjgfName, table[i].param[2]);
				}
				else if (creditNum2 == 2)
				{
					ShowRewardItem(strFqmzName, table[i].param[2]);
				}
				else if (creditNum2 == 3)
				{
					ShowRewardItem(strZyysName, table[i].param[2]);
				} 
				else if(creditNum2 == 4)
				{
					ShowRewardItem(strFsyjName, table[i].param[2]);
				}
				else if (creditNum2 == 5)
				{
					ShowRewardItem(strXmlmName, table[i].param[2]);
				}
			}
		}
	}	
}

void UIFormTask::ShowRewardLable(std::string& imgName, std::string& texName)
{
	if (m_LableText1->getText().empty())
	{
		m_FixLable1->Show();
		RtwImage* pImage = g_workspace.getImageFactory()->createImage(imgName);
		pImage->SetBlend(true);
		m_LableImage1->SetBackgroundImage(pImage);
		m_LableText1->SetText(texName);
	} 
	else if(m_LableText2->getText().empty())
	{
		m_FixLable2->Show();
		RtwImage* pImage = g_workspace.getImageFactory()->createImage(imgName);
		pImage->SetBlend(true);
		m_LableImage2->SetBackgroundImage(pImage);
		m_LableText2->SetText(texName);
	}
	else if (m_LableText3->getText().empty())
	{
		m_FixLable3->Show();
		RtwImage* pImage = g_workspace.getImageFactory()->createImage(imgName);
		pImage->SetBlend(true);
		m_LableImage3->SetBackgroundImage(pImage);
		m_LableText3->SetText(texName);
	} 
	else 
	{
		m_FixLable4->Show();
		RtwImage* pImage = g_workspace.getImageFactory()->createImage(imgName);
		pImage->SetBlend(true);
		m_LableImage4->SetBackgroundImage(pImage);
		m_LableText4->SetText(texName);
	}
}

void UIFormTask::ShowRewardItem(std::string& imgName, std::string& texName)
{
	if (m_ItemText1->getText().empty())
	{
		m_FixItem1->Show();
		RtwImage* pImage = g_workspace.getImageFactory()->createImage(imgName);
		pImage->SetBlend(true);
		m_ItemImage1->SetBackgroundImage(pImage);
		m_ItemText1->SetText(texName);
	} 
	else if(m_ItemText2->getText().empty())
	{
		m_FixItem2->Show();
		RtwImage* pImage = g_workspace.getImageFactory()->createImage(imgName);
		pImage->SetBlend(true);
		m_ItemImage2->SetBackgroundImage(pImage);
		m_ItemText2->SetText(texName);
	}
	else if (m_ItemText3->getText().empty())
	{
		m_FixItem3->Show();
		RtwImage* pImage = g_workspace.getImageFactory()->createImage(imgName);
		pImage->SetBlend(true);
		m_ItemImage3->SetBackgroundImage(pImage);
		m_ItemText3->SetText(texName);
	} 
	else 
	{
		m_FixItem4->Show();
		RtwImage* pImage = g_workspace.getImageFactory()->createImage(imgName);
		pImage->SetBlend(true);
		m_ItemImage4->SetBackgroundImage(pImage);
		m_ItemText4->SetText(texName);
	}
}

void UIFormTask::ParseRandomReward(STaskDesc* desc)
{
	m_RandomItem1->Hide();
	m_RandomItem2->Hide();
	m_RandomItem3->Hide();
	m_RandomItem4->Hide();

	std::vector<STaskKey>& table = desc->randomReward;

	for(size_t i = 0; i < table.size(); ++i)
	{
		if (table[i].key == "item")
		{
			SItemBase *pItemClass = GetWorld()->m_pItemManager->GetItemFromIndex(atol(table[i].param[0].c_str()));
			if (!pItemClass)
				continue;

			std::string imgName = pItemClass->icon;
			std::string texName = pItemClass->name;
			ShowRandomRewardItem(imgName, texName);
		}
	}
}

void UIFormTask::ShowRandomRewardItem(std::string& imgName, std::string& texName)
{
	if (m_RandomText1->getText().empty())
	{
		m_RandomItem1->Show();
		RtwImage* pImage = g_workspace.getImageFactory()->createImage(imgName);
		pImage->SetBlend(true);
		m_RandomImage1->SetBackgroundImage(pImage);
		m_RandomText1->SetText(texName);
	} 
	else if(m_RandomText2->getText().empty())
	{
		m_RandomItem2->Show();
		RtwImage* pImage = g_workspace.getImageFactory()->createImage(imgName);
		pImage->SetBlend(true);
		m_RandomImage2->SetBackgroundImage(pImage);
		m_RandomText2->SetText(texName);
	}
	else if (m_RandomText3->getText().empty())
	{
		m_RandomItem3->Show();
		RtwImage* pImage = g_workspace.getImageFactory()->createImage(imgName);
		pImage->SetBlend(true);
		m_RandomImage3->SetBackgroundImage(pImage);
		m_RandomText3->SetText(texName);
	} 
	else 
	{
		m_RandomItem4->Show();
		RtwImage* pImage = g_workspace.getImageFactory()->createImage(imgName);
		pImage->SetBlend(true);
		m_RandomImage4->SetBackgroundImage(pImage);
		m_RandomText4->SetText(texName);
	}
}

void UIFormTask::ResizeTaskReward()
{
	//调整任务奖励FORM
	static RtwRect rcLable = m_LableReward->getFrameRect();
	static RtwRect rcFix = m_formFix->GetFrameRect();
	static RtwRect rcFixItem = m_formFixItem->getFrameRect();
	static RtwRect rcFixLable1 = m_FixLable1->getFrameRect();
	static RtwRect rcFixLable3 = m_FixLable3->getFrameRect();
	static RtwRect rcFixItem1 = m_FixItem1->getFrameRect();
	static RtwRect rcFixItem3 = m_FixItem3->getFrameRect();
	static RtwRect rcRandom = m_formRandom->getFrameRect();

	if (m_LableReward)
	{
		RtwRect tmpRect = m_LableReward->GetClientRect();
		RtwRect rect = m_taskDesc1->GetClientRect();

		tmpRect.top = rect.bottom;
		tmpRect.bottom = tmpRect.top + rcLable.getHeight();
		tmpRect.left = rect.left;
		tmpRect.right = rect.left + rcLable.getWidth();
		m_LableReward->MoveResize(tmpRect, true);
	}
	if (m_formFix)
	{
		RtwRect tmpRect = m_formFix->GetClientRect();
		RtwRect rect = m_LableReward->GetClientRect();

		tmpRect.top = rect.bottom;
		tmpRect.bottom = tmpRect.top + rcFix.getHeight();
		tmpRect.left = rect.left;
		tmpRect.right = rect.left + rcFix.getWidth();
		m_formFix->MoveResize(tmpRect, true);
	}
	if (m_LableText3->getText().empty())
	{
		if (m_ItemText1->getText().empty())
		{
			//移动随机FORM
			RtwRect tmpRect = m_formRandom->GetClientRect();
			RtwRect rect = m_FixLable1->GetClientRect();

			tmpRect.top = rect.bottom + 4;
			tmpRect.bottom = tmpRect.top + rcRandom.getHeight();
			tmpRect.left = rect.left - 14;
			tmpRect.right = rect.left + rcRandom.getWidth();
			m_formRandom->MoveResize(tmpRect, true);
		} 
		else
		{
			if(m_formFixItem)
			{
				RtwRect tmpRect = m_formFixItem->GetClientRect();
				RtwRect rect = m_LableReward->GetClientRect();

				tmpRect.top = rect.bottom;
				tmpRect.bottom = tmpRect.top + rcFixItem.getHeight();
				tmpRect.left = rect.left - 2;
				tmpRect.right = rect.left + rcFixItem.getWidth();
				m_formFixItem->MoveResize(tmpRect, true);
			}
			if (m_ItemText3->getText().empty())
			{
				//移动随机FORM
				if(m_formRandom)
				{
					RtwRect tmpRect = m_formRandom->GetClientRect();
					RtwRect rect = m_FixItem1->GetClientRect();

					tmpRect.top = rect.bottom + 2;
					tmpRect.bottom = tmpRect.top + rcRandom.getHeight();
					tmpRect.left = rect.left - 6;
					tmpRect.right = rect.left + rcRandom.getWidth();
					m_formRandom->MoveResize(tmpRect, true);
				}
			}
			else
			{
				//移动随机FORM
				if(m_formRandom)
				{
					RtwRect tmpRect = m_formRandom->GetClientRect();
					RtwRect rect = m_formFixItem->GetClientRect();

					tmpRect.top = rect.bottom;
					tmpRect.bottom = tmpRect.top + rcRandom.getHeight();
					tmpRect.left = rect.left - 4;
					tmpRect.right = rect.left + rcRandom.getWidth();
					m_formRandom->MoveResize(tmpRect, true);
				}
			}
		}
	}
	else
	{
		if (m_ItemText1->getText().empty())
		{
			RtwRect tmpRect = m_formRandom->GetClientRect();
			RtwRect rect = m_FixLable3->GetClientRect();

			tmpRect.top = rect.bottom + 4;
			tmpRect.bottom = tmpRect.top + rcRandom.getHeight();
			tmpRect.left = rect.left - 14;
			tmpRect.right = rect.left + rcRandom.getWidth();
			m_formRandom->MoveResize(tmpRect, true);
		} 
		else
		{
			if (m_ItemText3->getText().empty())
			{
				if(m_formFixItem)
				{
					RtwRect tmpRect = m_formFixItem->GetClientRect();
					RtwRect rect = m_FixLable3->GetClientRect();

					tmpRect.top = rect.bottom;
					tmpRect.bottom = tmpRect.top + rcFixItem.getHeight();
					tmpRect.left = rect.left - 10;
					tmpRect.right = rect.left + rcFixItem.getWidth();
					m_formFixItem->MoveResize(tmpRect, true);
				}
				if(m_formRandom)
				{
					RtwRect tmpRect = m_formRandom->GetClientRect();
					RtwRect rect = m_FixItem1->GetClientRect();

					tmpRect.top = rect.bottom;
					tmpRect.bottom = tmpRect.top + rcRandom.getHeight();
					tmpRect.left = rect.left - 12;
					tmpRect.right = rect.left + rcRandom.getWidth();
					m_formRandom->MoveResize(tmpRect, true);
				}
			} 
			else
			{
				if(m_formFixItem)
				{
					RtwRect tmpRect = m_formFixItem->GetClientRect();
					RtwRect rect = m_FixLable3->GetClientRect();

					tmpRect.top = rect.bottom;
					tmpRect.bottom = tmpRect.top + rcFixItem.getHeight();
					tmpRect.left = rect.left - 10;
					tmpRect.right = rect.left + rcFixItem.getWidth();
					m_formFixItem->MoveResize(tmpRect, true);
				}
				if(m_formRandom)
				{
					RtwRect tmpRect = m_formRandom->GetClientRect();
					RtwRect rect = m_FixItem3->GetClientRect();

					tmpRect.top = rect.bottom;
					tmpRect.bottom = tmpRect.top + rcRandom.getHeight();
					tmpRect.left = rect.left - 12;
					tmpRect.right = rect.left + rcRandom.getWidth();
					m_formRandom->MoveResize(tmpRect, true);
				}
			}
		}
	}
	//if (m_LableText3->getText().empty())
	//{
	//	RtwRect tmpRect = m_formRandom->GetClientRect();
	//	RtwRect rect = m_FixLable1->GetClientRect();

	//	tmpRect.top = rect.bottom;
	//	tmpRect.bottom = tmpRect.top + rcRandom.getHeight();
	//	tmpRect.left = rect.left;
	//	tmpRect.right = rect.left + rcRandom.getWidth();
	//	m_formRandom->MoveResize(tmpRect, true);
	//}

	//if (m_ItemText1->getText().empty())
	//{
	//	RtwRect tmpRect = m_formRandom->GetClientRect();
	//	RtwRect rect = m_FixLable3->GetClientRect();

	//	tmpRect.top = rect.bottom;
	//	tmpRect.bottom = tmpRect.top + rcRandom.getHeight();
	//	tmpRect.left = rect.left;
	//	tmpRect.right = rect.left + rcRandom.getWidth();
	//	m_formRandom->MoveResize(tmpRect, true);
	//}
	//if (m_ItemText3->getText().empty())
	//{
 //		RtwRect tmpRect = m_formRandom->GetClientRect();
 //		RtwRect rect = m_FixItem1->GetClientRect();
 //
 //		tmpRect.top = rect.bottom;
 //		tmpRect.bottom = rect.bottom + rcRandom.getHeight();
 //		tmpRect.left = rect.left;
 //		tmpRect.right = rect.left + rcRandom.getWidth() - 5;
 //		m_formRandom->MoveResize(tmpRect,true);
	//}
}

void UIFormTask::OnShowHintText(RtwWidget* sender, void*)
{
	int nLines;
	SItemID item = GetWorld()->m_pItemManager->CreateItem(6501);
	RtString str = GenerateItemHtmlHint(nLines, item, GetWorld()->m_pPlayer);
	g_workspace.SetHintText((char*)str, true);
	g_workspace.SetHintSize(ITEM_HINT_WIDTH, 30);
	g_workspace.AutoHintSize();
	g_workspace.AutoHintPosition();
}

void UIFormTask::Hide()
{
	guard;
	mExpandGroupId1.clear();
	mExpandGroupId2.clear();
	mExpandGroupId3.clear();

	RtwTree::Item* mRootItem1 = m_pTree1->getRootItem();
	if(mRootItem1->pFirstChild == NULL) 
	{
		m_pFrmThis->Hide();
		return;
	}

	RtwTree::Item* mRootItem2 = m_pTree1->getRootItem();
	if(mRootItem2->pFirstChild == NULL) 
	{
		m_pFrmThis->Hide();
		return;
	}

	RtwTree::Item* mRootItem3 = m_pTree1->getRootItem();
	if(mRootItem3->pFirstChild == NULL)
	{
		m_pFrmThis->Hide();
		return;
	}
	//记住所有打开的根项
	vector<int>::iterator itExpand1;
	for (itExpand1 = mGroupId1.begin(); itExpand1 != mGroupId1.end(); itExpand1++)
	{
		RtwTree::Item* pItem = m_pTree1->FindTreeItemByID(*itExpand1);
		if (pItem)
		{
			if (pItem->bExpanded)
			{
				mExpandGroupId1.push_back(*itExpand1);
			}
		}
	}

	vector<int>::iterator itExpand2;
	for (itExpand2 = mGroupId2.begin(); itExpand2 != mGroupId2.end(); itExpand2++)
	{
		RtwTree::Item* pItem = m_pTree2->FindTreeItemByID(*itExpand2);
		if (pItem)
		{
			if (pItem->bExpanded)
			{
				mExpandGroupId2.push_back(*itExpand2);
			}
		}
	}

	vector<int>::iterator itExpand3;
	for (itExpand3 = mGroupId3.begin(); itExpand3 != mGroupId3.end(); itExpand3++)
	{
		RtwTree::Item* pItem = m_pTree3->FindTreeItemByID(*itExpand3);
		if (pItem)
		{
			if (pItem->bExpanded)
			{
				mExpandGroupId3.push_back(*itExpand3);
			}
		}
	}
	if (IsVisible())
	{
		m_pFrmThis->Hide();
	}
	unguard;
}
void UIFormTask::Show()
{
	guard;
	m_pTree1->Clear();
	m_pTree2->Clear();
	m_pTree3->Clear();

	m_LableReward->Hide();
	m_formFix->Hide();
	m_formRandom->Hide();

	m_pFrmThis->Show();

	BuildTaskTree1();
	BuildTaskTree2();
	BuildTaskTree3();
	unguard;
}

void UIFormTask::OnTreeNodeSelect1(RtwWidget* pSender, RtwEventDelegate* pEvent)
{
	guard;
	if(!m_pTree1)	return;
	
	m_LastTaskId1 = m_pTree1->GetLastSelect()->id;

	STaskDesc *desc = g_TableTask.FindTask(m_LastTaskId1);
	if (desc)
	{
		if (desc->m_bShare)
		{
			LOAD_UI("fmmaintask.fmtaskbtn1.btnsharetask")->Enable();
		}
		else
		{
			LOAD_UI("fmmaintask.fmtaskbtn1.btnsharetask")->Disable();
		}
	}

	SetSelectedTaskID1(m_LastTaskId1);
	unguard;
}

void UIFormTask::OnTreeNodeSelect2(RtwWidget* pSender, RtwEventDelegate* pEvent)
{
	guard;
	if(!m_pTree2)	return;

	m_LastTaskId2 = m_pTree2->GetLastSelect()->id;

	SetSelectedTaskID2(m_LastTaskId2);
	unguard;
}

void UIFormTask::OnTreeNodeSelect3(RtwWidget* pSender, RtwEventDelegate* pEvent)
{
	guard;
	if(!m_pTree3)	return;

	m_LastTaskId3 = m_pTree3->GetLastSelect()->id;

	SetSelectedTaskID3(m_LastTaskId3);
	unguard;
}

void UIFormTask::SetSelectedTaskID1(int taskId)
{	
	m_taskDesc1->SetText("");
	STaskDesc *desc = g_TableTask.FindTask(taskId);
	if(!desc)
	{
		return;
	}
	////处理可选奖励
	//std::vector<DWORD> rw;
	//std::vector<STaskKey>& table = desc->randomReward;
	//for(size_t i = 0; i < table.size(); i++)
	//{
	//	if (table[i].key == "item")
	//	{			
	//		rw.push_back(atol(table[i].param[0].c_str()));
	//	}
	//}

	//ResetRewardItemIDs(rw);

	//fox by 跑环任务//任务列表
	if(desc->circleTask!=0)
	{//跑环任务
		STaskInfo* pTaskInfo = GetPlayer()->m_task.FindTask(taskId);


		RtString tmp;
		string SourceName;
		string TargetName;
		SNpc* pSourceNpc = g_TableNpc.FindNpc(pTaskInfo->m_srcNpcID);
		SourceName = (pSourceNpc == NULL) ? "":pSourceNpc->Name;
		if(desc->circleTask == 2)
		{//找NPC
			SNpc* pNpc = g_TableNpc.FindNpc(pTaskInfo->m_findnpcID);
			TargetName= (pNpc == NULL) ? "" : pNpc->Name;
			tmp.Format(desc->desc.c_str(),
				GetPlayer()->m_task.m_currCircle,g_tcTotalCircle,SourceName.c_str(), TargetName.c_str());
		}
		else if(desc->circleTask == 3) 
		{//杀怪物
			SNpc* pNpc = g_TableNpc.FindNpc(pTaskInfo->m_killmonsterID);
			TargetName= (pNpc == NULL) ? "" : pNpc->Name;
			tmp.Format(desc->desc.c_str(),
				GetPlayer()->m_task.m_currCircle,g_tcTotalCircle,SourceName.c_str(), TargetName.c_str(),desc->dynamicNum);

		}
		else if(desc->circleTask == 1)
		{//找东西
			SItemBase* pItemClass = GetWorld()->m_pItemManager->GetItemFromIndex(pTaskInfo->m_finditemID);
			TargetName= (pItemClass== NULL) ? "" : pItemClass->name;

			tmp.Format(desc->desc.c_str(),
				GetPlayer()->m_task.m_currCircle,g_tcTotalCircle,SourceName.c_str(), TargetName.c_str(),desc->dynamicNum);
		}

		string RunTaskPrefix;
		if(!GetPlayer()->m_task.m_isActive)
		{
			RunTaskPrefix=R(MSG_CIRCLE_NOT_ACTIVE);
		}

		RunTaskPrefix +=(char*)tmp;

		if (std::string::npos != desc->desc.find("html"))
		{
			m_taskDesc1->LoadFromString(desc->desc);
		}
		else
		{
			m_taskDesc1->SetText(RunTaskPrefix);
		}
	}
	else
	{//正常任务
		if (std::string::npos != desc->desc.find("html"))
		{
			//任务描述
			m_taskDesc1->LoadFromString(desc->desc);
		}
		else
		{
			m_taskDesc1->SetText(desc->desc);
		}
	}

	if (m_formFix->IsVisible())
	{
		m_formFix->Hide();
	}
	if (m_formRandom->IsVisible())
	{
		m_formRandom->Hide();
	}
	//清空Lable文本
	ResetRewardText();

	//固定奖励显示图片
	if (!desc->fixReward.empty())
	{
		m_LableReward->Show();
		m_formFix->Show();
		ParseFixReward(desc);
	}
	else
	{
		m_formFix->Hide();
	}
	if (!desc->randomReward.empty())
	{
		m_formRandom->Show();
		ParseRandomReward(desc);
	}
	else
	{
		m_formRandom->Hide();
	}

	//重新调整奖励控件位置
	ResizeTaskReward();

	//if (desc->canCancel == 0)
	//	LOAD_UI("fmcharall.tabcharall.fmtask.btndeltask")->Disable();
	//else
	//	LOAD_UI("fmcharall.tabcharall.fmtask.btndeltask")->Enable();
}

void UIFormTask::SetSelectedTaskID2(int taskId)
{
	STaskDesc *desc = g_TableTask.FindTask(taskId);
	if(!desc)
	{
		m_taskDesc2->SetText("");
		return;
	}

	//fox by 跑环任务//任务列表
	if(desc->circleTask!=0)
	{//跑环任务
		STaskInfo* pTaskInfo = GetPlayer()->m_task.FindTask(taskId);


		RtString tmp;
		string SourceName;
		string TargetName;
		SNpc* pSourceNpc = g_TableNpc.FindNpc(pTaskInfo->m_srcNpcID);
		SourceName = (pSourceNpc == NULL) ? "":pSourceNpc->Name;
		if(desc->circleTask == 2)
		{//找NPC
			SNpc* pNpc = g_TableNpc.FindNpc(pTaskInfo->m_findnpcID);
			TargetName= (pNpc == NULL) ? "" : pNpc->Name;
			tmp.Format(desc->desc.c_str(),
				GetPlayer()->m_task.m_currCircle,g_tcTotalCircle,SourceName.c_str(), TargetName.c_str());
		}
		else if(desc->circleTask == 3) 
		{//杀怪物
			SNpc* pNpc = g_TableNpc.FindNpc(pTaskInfo->m_killmonsterID);
			TargetName= (pNpc == NULL) ? "" : pNpc->Name;
			tmp.Format(desc->desc.c_str(),
				GetPlayer()->m_task.m_currCircle,g_tcTotalCircle,SourceName.c_str(), TargetName.c_str(),desc->dynamicNum);

		}
		else if(desc->circleTask == 1)
		{//找东西
			SItemBase* pItemClass = GetWorld()->m_pItemManager->GetItemFromIndex(pTaskInfo->m_finditemID);
			TargetName= (pItemClass== NULL) ? "" : pItemClass->name;

			tmp.Format(desc->desc.c_str(),
				GetPlayer()->m_task.m_currCircle,g_tcTotalCircle,SourceName.c_str(), TargetName.c_str(),desc->dynamicNum);
		}

		string RunTaskPrefix;
		if(!GetPlayer()->m_task.m_isActive)
		{
			RunTaskPrefix=R(MSG_CIRCLE_NOT_ACTIVE);
		}

		RunTaskPrefix +=(char*)tmp;

		if (std::string::npos != desc->desc.find("html"))
		{
			m_taskDesc2->LoadFromString(desc->desc);
		}
		else
		{
			m_taskDesc2->SetText(RunTaskPrefix);
		}
	}
	else
	{//正常任务
		if (std::string::npos != desc->desc.find("html"))
		{
			//任务描述
			m_taskDesc2->LoadFromString(desc->mAcceptDesc);
		}
		else
		{
			m_taskDesc2->SetText(desc->mAcceptDesc);
		}
	}
}

void UIFormTask::SetSelectedTaskID3(int taskId)
{
	STaskDesc *desc = g_TableTask.FindTask(taskId);
	if(desc)
	{
		m_taskDesc3->LoadFromString(desc->desc);
	}

	////清空Lable文本
	//ResetRewardText();
	////固定奖励显示图片
	//if (!desc->fixReward.empty())
	//{
	//	m_formFix->Show();
	//	ParseFixReward(desc);
	//}
	//else
	//{
	//	m_formFix->Hide();
	//}
	//if (!desc->randomReward.empty())
	//{
	//	m_formRandom->Show();
	//	ParseRandomReward(desc);
	//}
	//else
	//{
	//	m_formRandom->Hide();
	//}

	////重新调整奖励控件位置
	//ResizeTaskReward();

}
void UIFormTask::BuildTaskTree1()
{
	guard;
	m_taskDesc1->SetText("");
	if (m_LableReward->IsVisible())
	{
		m_LableReward->Hide();
	}
	if (m_formFix->IsVisible())
	{
		m_formFix->Hide();
	}
	if (m_formRandom->IsVisible())
	{
		m_formRandom->Hide();
	}
	ResetRewardImage();
	ResetRewardText();
	//记录当前正在做的任务数
	int mTaskCnt = 0;
	CTaskInfo *task = &GetWorld()->m_pPlayer->m_task;

	if(task->m_taskList.empty()) return;

	std::list<STaskInfo>::iterator it;
	vector<int>::iterator itGroup1;
	mGroupId1.clear();
	for(it=task->m_taskList.begin(); it!=task->m_taskList.end(); it++)
	{
		STaskDesc *desc = g_TableTask.FindTask((*it).Id);
		if(!desc) continue;

		vector<int>::iterator result = find(mGroupId1.begin(), mGroupId1.end(), desc->m_TaskGroupID);
		if (result == mGroupId1.end())
		{
			mGroupId1.push_back(desc->m_TaskGroupID);
		}
		mTaskCnt++;
	}
	if(!mTaskCnt) return;
	//升序排列任务区块组
	std::sort(mGroupId1.begin(), mGroupId1.end(), less<int>());

	for (itGroup1=mGroupId1.begin(); itGroup1!=mGroupId1.end(); itGroup1++)
	{
		TaskGroupData* taskGroup = g_TaskGroup.FindTaskGroup(*itGroup1);
		m_pTree1->InsertTreeItem(NULL, taskGroup->strGroupName.c_str(), "", (*itGroup1));
		for(it=task->m_taskList.begin(); it!=task->m_taskList.end(); it++)
		{
			STaskDesc *desc = g_TableTask.FindTask((*it).Id);
			if(!desc) continue;
			if((*itGroup1)==desc->m_TaskGroupID)
			{
				std::string child = desc->name;
				if(it->IsDoing == 3)
				{
					m_pTree1->InsertTreeItem(taskGroup->strGroupName.c_str(), 
						child.c_str(),
						"", it->Id, -1, 0, it->IsTrack(), "完成", 0x0000ff00);

					RtwTree::Item* pItem = m_pTree1->FindTreeItemByID(it->Id);
					if (pItem)
					{
						if (desc->m_bShare)
						{
							m_pTree1->UpdateItemImage(pItem, 0, "ui_taskshare.tga");
						}
						if (it->IsTrack())
						{
							m_pTree1->UpdateItemImage(pItem, 1, "ui_tasktrack.tga");
						}						
						m_pTree1->UpdateItemImage(pItem, 2, "ui_taskdone.tga");
					}
				}else
				{
					m_pTree1->InsertTreeItem(taskGroup->strGroupName.c_str(), 
						child.c_str(),
						"", it->Id, -1, 0, it->IsTrack());
					RtwTree::Item* pItem = m_pTree1->FindTreeItemByID(it->Id);
					if (pItem)
					{
						if (desc->m_bShare)
						{
							m_pTree1->UpdateItemImage(pItem, 0, "ui_taskshare.tga");
						}
						if (it->IsTrack())
						{
							m_pTree1->UpdateItemImage(pItem, 1, "ui_tasktrack.tga");
						}						
					}
				}
				m_pTree1->SetItemColor(taskGroup->strGroupName.c_str(), 0xFFFFFFFF);
			}

		}
	}
	//展开所有打开的根项
	vector<int>::iterator itExpand1;
	for (itExpand1 = mExpandGroupId1.begin(); itExpand1 != mExpandGroupId1.end(); itExpand1++)
	{
		RtwTree::Item* pItem = m_pTree1->FindTreeItemByID(*itExpand1);
		if (pItem)
		{
			m_pTree1->Expand(pItem);
		}
	}
	//默认展开第一项
	if (m_LastTaskId1 == 0)
	{
		m_pTree1->Expand(m_pTree1->getFirstItem());
		m_pTree1->Select(m_pTree1->getFirstItem()->pFirstChild);
		m_LastTaskId1 = m_pTree1->getFirstItem()->pFirstChild->id;
	} 
	else
	{
		RtwTree::Item* pItem = m_pTree1->FindTreeItemByID(m_LastTaskId1);
		if (pItem)
		{
			m_pTree1->Expand(pItem->pParent);
			m_pTree1->Select(pItem);
		} 
	}

	SetSelectedTaskID1(m_LastTaskId1);
	unguard;
}

void UIFormTask::BuildTaskTree2()
{
	guard;	
	m_taskDesc2->SetText("");
	GetTaskByGroup(taskid);

	if(taskid.empty()) return;

	vector<int>::iterator itGroup2;
	mGroupId2.clear();
	std::vector<int>::iterator	it_task;
	for(it_task = taskid.begin(); it_task != taskid.end(); it_task++)
	{
		STaskDesc *desc = g_TableTask.FindTask(*it_task);
		if(!desc) continue;

		vector<int>::iterator result = find(mGroupId2.begin(), mGroupId2.end(), desc->m_TaskGroupID);
		if (result == mGroupId2.end())
		{
			mGroupId2.push_back(desc->m_TaskGroupID);
		}
	}
	//升序排列任务区块组
	std::sort(mGroupId2.begin(), mGroupId2.end(), less<int>());

	for (itGroup2=mGroupId2.begin(); itGroup2!=mGroupId2.end(); itGroup2++)
	{
		TaskGroupData* taskGroup = g_TaskGroup.FindTaskGroup(*itGroup2);
		m_pTree2->InsertTreeItem(NULL, taskGroup->strGroupName.c_str(), "", (*itGroup2));
		for(it_task = taskid.begin(); it_task != taskid.end(); it_task++)
		{
			STaskDesc *desc = g_TableTask.FindTask(*it_task);
			if(!desc) continue;

			if((*itGroup2)==desc->m_TaskGroupID)
			{
				std::string child = desc->name;
				m_pTree2->InsertTreeItem(taskGroup->strGroupName.c_str(), 
					child.c_str(),
					"", (*it_task));

				m_pTree2->SetItemColor(taskGroup->strGroupName.c_str(), 0xFFFFFFFF);
			}
		}
	}

	//展开所有打开的根项
	vector<int>::iterator itExpand2;
	for (itExpand2 = mExpandGroupId2.begin(); itExpand2 != mExpandGroupId2.end(); itExpand2++)
	{
		RtwTree::Item* pItem = m_pTree2->FindTreeItemByID(*itExpand2);
		if (pItem)
		{
			m_pTree2->Expand(pItem);
		}
	}
	//默认展开第一项
	if (m_LastTaskId2 == 0)
	{
		m_pTree2->Expand(m_pTree2->getFirstItem());
		m_pTree2->Select(m_pTree2->getFirstItem()->pFirstChild);
		m_LastTaskId2 = m_pTree2->getFirstItem()->pFirstChild->id;
	} 
	else
	{
		RtwTree::Item* pItem = m_pTree2->FindTreeItemByID(m_LastTaskId2);
		if (pItem)
		{
			m_pTree2->Expand(pItem->pParent);
			m_pTree2->Select(pItem);
		} 
	}

	SetSelectedTaskID2(m_LastTaskId2);
	unguard;
}

void UIFormTask::BuildTaskTree3()
{
	guard;
	m_taskDesc3->SetText("");

	CTaskInfo *task = &GetWorld()->m_pPlayer->m_task;

	if(task->m_taskList.empty()) return;

	std::list<STaskInfo>::iterator it;
	vector<int>::iterator itGroup3;
	mGroupId3.clear();
	for(it=task->m_taskList.begin(); it!=task->m_taskList.end(); it++)
	{
		if(it->IsDoing != 4) continue;
		STaskDesc *desc = g_TableTask.FindTask((*it).Id);
		if(!desc) continue;

		vector<int>::iterator result = find(mGroupId3.begin(), mGroupId3.end(), desc->m_TaskGroupID);
		if (result == mGroupId3.end())
		{
			mGroupId3.push_back(desc->m_TaskGroupID);
		}
	}
	//升序排列任务区块组
	std::sort(mGroupId3.begin(), mGroupId3.end(), less<int>());

	for (itGroup3=mGroupId3.begin(); itGroup3!=mGroupId3.end(); itGroup3++)
	{
		TaskGroupData* taskGroup = g_TaskGroup.FindTaskGroup(*itGroup3);
		m_pTree3->InsertTreeItem(NULL, taskGroup->strGroupName.c_str(), "", (*itGroup3));
		for(it=task->m_taskList.begin(); it!=task->m_taskList.end(); it++)
		{
			if(it->IsDoing != 4) continue;
			STaskDesc *desc = g_TableTask.FindTask((*it).Id);
			if(!desc) continue;
			if((*itGroup3)==desc->m_TaskGroupID)
			{
				std::string child = desc->name;

				m_pTree3->InsertTreeItem(taskGroup->strGroupName.c_str(), 
					child.c_str(),
					"", it->Id, -1, 0);
				m_pTree3->SetItemColor(taskGroup->strGroupName.c_str(), 0xFFFFFFFF);
			}

		}
	}

	RtwTree::Item* mRootItem = m_pTree3->getRootItem();
	if(mRootItem->pFirstChild == NULL) return;

	//展开所有打开的根项
	vector<int>::iterator itExpand3;
	for (itExpand3 = mExpandGroupId3.begin(); itExpand3 != mExpandGroupId3.end(); itExpand3++)
	{
		RtwTree::Item* pItem = m_pTree3->FindTreeItemByID(*itExpand3);
		if (pItem)
		{
			m_pTree3->Expand(pItem);
		}
	}
	//默认展开第一项
	if (m_LastTaskId3 == 0)
	{
		m_pTree3->Expand(m_pTree3->getFirstItem());
		m_pTree3->Select(m_pTree3->getFirstItem()->pFirstChild);
		m_LastTaskId3 = m_pTree3->getFirstItem()->pFirstChild->id;
	} 
	else
	{
		RtwTree::Item* pItem = m_pTree3->FindTreeItemByID(m_LastTaskId3);
		if (pItem)
		{
			m_pTree3->Expand(pItem->pParent);
			m_pTree3->Select(pItem);
		} 
	}

	SetSelectedTaskID3(m_LastTaskId3);
	unguard;
}

int UIFormTask::GetSelectedTaskID()
{
	if(!m_pTree1) return 0;
	RtwTree::Item* pItem = m_pTree1->GetLastSelect();
	if (pItem)
	{
		return pItem->id;
	}
	return 1;
}
//得到可接任务列表
void UIFormTask::GetTaskByGroup(vector<int> &taskId)
{
	taskId.clear();

	std::vector<STaskDesc> m_table = g_TableTask.GetTaskTable();
	for (unsigned int i=0; i<m_table.size(); i++)
	{
		//是否允许直接接受
		if (!m_table[i].access) continue;
		//国家限制
		if (m_table[i].countryLimit != 2)
		{
			if(m_table[i].countryLimit != GetWorld()->m_pPlayer->m_core.Faction) continue;
		}
		//职业限制
		if (m_table[i].metierLimit != 0)
		{
			if(m_table[i].metierLimit != GetWorld()->m_pPlayer->m_core.Metier) continue;
		} 
		////官职要求
		//if ((*it).reqUnion != 0)
		//{
		//	if((*it).reqUnion != GetWorld()->m_pPlayer->m_core.)
		//}
		//等级限制
		if (GetWorld()->m_pPlayer->m_core.Lev < m_table[i].reqMinLev || GetWorld()->m_pPlayer->m_core.Lev > m_table[i].reqMaxLev) continue;

		//任务做完直接跳出
		CTaskInfo *taskInfo = &GetWorld()->m_pPlayer->m_task;
		if(taskInfo)
		{
			if(taskInfo->IsDoneTask(m_table[i].id)) continue;
		}

		if (m_table[i].acceptReq.empty())
		{
			if(taskInfo)
			{
				if(taskInfo->IsDoneTask(m_table[i].id) && !(m_table[i].repeat)) continue;
				STaskInfo *task = taskInfo->FindTask(m_table[i].id);
				if(task)
				{
					if(task->FIsDoing()) continue;					
				}
			}
		} 
		else
		{
			bool bNoAccept = false;
			bool bNoUnion = false;
			bool bLessGrade = false;
			CTaskInfo *taskInfo = &GetWorld()->m_pPlayer->m_task;
			STaskInfo *task = taskInfo->FindTask(m_table[i].id);
			if(task)
			{
				if(task->FIsDoing()) continue;
			}
			std::vector<STaskKey>& taskKey = m_table[i].acceptReq;
			for (size_t j = 0; j < taskKey.size(); j++)
			{
				STaskKey& key = taskKey[j];
				if (key.key == "taskdone")
				{
					int nTaskID = atoi(key.param[0].c_str());
					if (!taskInfo->IsDoneTask(nTaskID)) 
					{
						bNoAccept = true;
						break;
					}
				}
				if (key.key == "inunion")
				{
					if (!GetWorld()->m_pPlayer->GetUnionID())
					{
						bNoUnion = true;
					}
				}
				if (key.key == "gradelev")
				{
					int nGradeLev = atoi(key.param[0].c_str()) + 1;
					int nActorLev = GetWorld()->m_pPlayer->m_core.GradeLev;
					if (nActorLev < nGradeLev)
					{
						bLessGrade = false;
					}
				}
			}
			if(bNoAccept) continue;
			if(bNoUnion) continue;
			if(bLessGrade) continue;
		}
		taskId.push_back(m_table[i].id);
	}
}
//得到可接任务列表
void UIFormTask::GetTaskByGroup(vector<STaskDesc*> &taskId)
{
	taskId.clear();

	std::vector<STaskDesc>* m_table = &g_TableTask.GetTaskTable();
	vector<STaskDesc>::iterator it;
	for (it = m_table->begin();it != m_table->end();it++)
	{
		//是否允许直接接受
		if (!(*it).access) continue;
		//国家限制
		if ((*it).countryLimit != 2)
		{
			if((*it).countryLimit != GetWorld()->m_pPlayer->m_core.Faction) continue;
		}
		//职业限制
		if ((*it).metierLimit != 0)
		{
			if((*it).metierLimit != GetWorld()->m_pPlayer->m_core.Metier) continue;
		} 
		////官职要求
		//if ((*it).reqUnion != 0)
		//{
		//	if((*it).reqUnion != GetWorld()->m_pPlayer->m_core.)
		//}
		//等级限制
		if (GetWorld()->m_pPlayer->m_core.Lev < (*it).reqMinLev || GetWorld()->m_pPlayer->m_core.Lev > (*it).reqMaxLev) continue;

		//任务做完直接跳出
		CTaskInfo *taskInfo = &GetWorld()->m_pPlayer->m_task;
		if(taskInfo)
		{
			if(taskInfo->IsDoneTask((*it).id)) continue;
		}

		if ((*it).acceptReq.empty())
		{
			CTaskInfo *taskInfo = &GetWorld()->m_pPlayer->m_task;
			if (taskInfo)
			{
				if(taskInfo->IsDoneTask((*it).id) && !((*it).repeat)) continue;
				STaskInfo *task = taskInfo->FindTask((*it).id);
				if(task)
				{
					if(task->FIsDoing()) continue;					
				}
			}
		} 
		else
		{
			bool bNoAccept = false;
			bool bNoUnion = false;
			bool bLessGrade = false;
			CTaskInfo *taskInfo = &GetWorld()->m_pPlayer->m_task;
			STaskInfo *task = taskInfo->FindTask((*it).id);
			if(task)
			{
				if(task->FIsDoing()) continue;
			}
			std::vector<STaskKey>& taskKey = (*it).acceptReq;
			for (size_t j = 0; j < taskKey.size(); j++)
			{
				STaskKey& key = taskKey[j];
				if (key.key == "taskdone")
				{
					int nTaskID = atoi(key.param[0].c_str());
					if (!taskInfo->IsDoneTask(nTaskID)) 
					{
						bNoAccept = true;
						break;
					}
				}
				if (key.key == "inunion")
				{
					if (!GetWorld()->m_pPlayer->GetUnionID())
					{
						bNoUnion = true;
					}
				}
				if (key.key == "gradelev")
				{
					int nGradeLev = atoi(key.param[0].c_str()) + 1;
					int nActorLev = GetWorld()->m_pPlayer->m_core.GradeLev;
					if (nActorLev < nGradeLev)
					{
						bLessGrade = false;
					}
				}
			}
			if(bNoAccept) continue;
			if(bNoUnion) continue;
			if(bLessGrade) continue;
		}
		taskId.push_back(&(*it));
	}
}

void UIFormTask::OnHide(RtwWidget* sender,void*)
{
	Hide();
}

void UIFormTask::ChangeSelectTab(RtwWidget* pSender, RtwEventDelegate* pEvent)
{
	int i =pEvent->type;
	switch(i)
	{
	case 0:
		LOAD_UI("fmmaintask.fmtaskbtn1")->Show();
		if (LOAD_UI("fmmaintask.fmtaskbtn2")->IsVisible())
		{
			LOAD_UI("fmmaintask.fmtaskbtn2")->Hide();
		}
		if (LOAD_UI("fmmaintask.fmtaskbtn3")->IsVisible())
		{
			LOAD_UI("fmmaintask.fmtaskbtn3")->Hide();
		}
		break;
	case 1:
		LOAD_UI("fmmaintask.fmtaskbtn2")->Show();
		if (LOAD_UI("fmmaintask.fmtaskbtn1")->IsVisible())
		{
			LOAD_UI("fmmaintask.fmtaskbtn1")->Hide();
		}
		if (LOAD_UI("fmmaintask.fmtaskbtn3")->IsVisible())
		{
			LOAD_UI("fmmaintask.fmtaskbtn3")->Hide();
		}
		break;
	case 2:
		LOAD_UI("fmmaintask.fmtaskbtn3")->Show();
		if (LOAD_UI("fmmaintask.fmtaskbtn1")->IsVisible())
		{
			LOAD_UI("fmmaintask.fmtaskbtn1")->Hide();
		}
		if (LOAD_UI("fmmaintask.fmtaskbtn2")->IsVisible())
		{
			LOAD_UI("fmmaintask.fmtaskbtn2")->Hide();
		}
		break;
	default:
		LOAD_UI("fmmaintask.fmtaskbtn1")->Show();
		if (LOAD_UI("fmmaintask.fmtaskbtn2")->IsVisible())
		{
			LOAD_UI("fmmaintask.fmtaskbtn2")->Hide();
		}
		if (LOAD_UI("fmmaintask.fmtaskbtn3")->IsVisible())
		{
			LOAD_UI("fmmaintask.fmtaskbtn3")->Hide();
		}
		break;
	}
	
}

void UIFormTask::OnClickCancelTask(RtwWidget *sender,void *)
{
	guard;
	char cTmp1024[1024];

	RtwTree::Item* mRootItem = m_pTree1->getRootItem();
	if(mRootItem->pFirstChild == NULL) return;
	RtwTree::Item* pSelectItem = m_pTree1->GetLastSelect();
	if(pSelectItem == NULL)
	{ 
		PlayerFailOperate(R(MSG_TASK_SELECTCANCELTASK));
		return;
	}

	int taskId = m_pTree1->GetLastSelect()->id;
	m_taskID = taskId;

	STaskDesc *desc = g_TableTask.FindTask(taskId);
	if (desc)
	{
		if (desc->m_TaskGroupID == 90)
		{
			rt2_sprintf(cTmp1024, R(MSG_TASK_CONFIRMCANCELMASTERTASK));
		} 
		else
		{
			rt2_sprintf(cTmp1024, R(MSG_TASK_CONFIRMCANCELTASK), desc->name.c_str());
		}
	}

	UIFormMsg *pFrm = UIFormMsg::ShowStatic(cTmp1024, UIFormMsg::TYPE_OK_CANCEL);
	pFrm->EvOK = RTW_CALLBACK_1(this, UIFormTask, OnClicked_CancelTaskTrue,pFrm);
	pFrm->EvCancel = RTW_CALLBACK_1(this, UIFormTask, OnClicked_CancelTaskFalse,pFrm);
	unguard;
}

void UIFormTask::OnClickTaskTrack(RtwWidget *sender, void *)
{
	if (LOAD_UI_T(CUiCheckButton, "fmmaintask.fmtaskbtn1.cktasktrack")->GetChecked())
	{
		LOAD_UI("fmmaintask.fmtaskbtn1.btntasktrack")->Enable();
		g_layerMain->m_formCharSimple->SetCheckTrack(true);
	} 
	else
	{
		LOAD_UI("fmmaintask.fmtaskbtn1.btntasktrack")->Disable();
		g_layerMain->m_formCharSimple->SetCheckTrack(false);
	}
}

void UIFormTask::OnClick_TrackTask(RtwWidget *sender, void *)
{
	if(!m_pTree1)	return;

	RtwTree::Item* mRootItem = m_pTree1->getRootItem();
	if(mRootItem->pFirstChild == NULL) return;
	RtwTree::Item* pSelectItem = m_pTree1->GetLastSelect();
	if(pSelectItem == NULL)
	{ 
		PlayerFailOperate(R(MSG_TASK_SELECTTRACKTASK));
		return;
	}

	g_layerMain->m_formCharSimple->TrackTaskItem(m_pTree1->GetLastSelect()->id);
}

void UIFormTask::OnClicked_ShareTask(RtwWidget *sender, void *)
{
	RtwTree::Item* mRootItem = m_pTree1->getRootItem();
	if(mRootItem->pFirstChild == NULL) return;
	RtwTree::Item* pSelectItem = m_pTree1->GetLastSelect();
	if(pSelectItem == NULL)
	{ 
		PlayerFailOperate(R(MSG_TASK_SELECTSHARETASK));
		return;
	}

	int taskId = m_pTree1->GetLastSelect()->id;

	//共享任务
	CG_CmdPacket *cmd = NetBeginWrite();
	cmd->WriteShort(c2r_shared_task);
	cmd->WriteLong(taskId);
	NetSend(cmd);

}

void UIFormTask::OnTaskClose(RtwWidget *sender, void *)
{
	Hide();
}

void UIFormTask::OnClicked_CancelTaskTrue(void*, void*)
{
	mExpandGroupId1.clear();

	//记住所有打开的根项
	vector<int>::iterator itExpand1;
	for (itExpand1 = mGroupId1.begin(); itExpand1 != mGroupId1.end(); itExpand1++)
	{
		RtwTree::Item* pItem = m_pTree1->FindTreeItemByID(*itExpand1);
		if (pItem)
		{
			if (pItem->bExpanded)
			{
				mExpandGroupId1.push_back(*itExpand1);
			}
		}
	}
	//取消任务
	CG_CmdPacket *cmd = NetBeginWrite();
	cmd->WriteShort(c2r_cancel_task);
	cmd->WriteLong(m_taskID);
	NetSend(cmd);
}

void UIFormTask::OnClicked_CancelTaskFalse(void*, void*)
{
	return;
}

void UIFormTask::OnClicked_AcceptShareTask(RtwWidget *sender, void *)
{
	RtwTree::Item* mRootItem = m_pTree3->getRootItem();
	if(mRootItem->pFirstChild == NULL) return;
	RtwTree::Item* pSelectItem = m_pTree3->GetLastSelect();
	if(pSelectItem == NULL)
	{ 
		PlayerFailOperate(R(MSG_TASK_SELECTONESHARETASK));
		return;
	}

	int taskId = m_pTree3->GetLastSelect()->id;

	//共享任务
	CG_CmdPacket *cmd = NetBeginWrite();
	cmd->WriteShort(c2r_accept_shared_task);
	cmd->WriteLong(taskId);
	NetSend(cmd);
}