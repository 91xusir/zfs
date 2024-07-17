// DlgPose.cpp : 实现文件
//
#include "stdafx.h"
#include "tool_character.h"
#include "DlgPose.h"
using namespace std;
#include "character/actor.h"
#include ".\dlgpose.h"
#include "character/cha_basic.h"

// CDlgPose 对话框

IMPLEMENT_DYNAMIC(CDlgPose, CDialog)
CDlgPose::CDlgPose(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgPose::IDD, pParent)
	, m_action(_T(""))
	, m_param(_T(""))
	, m_keyFrame(0)
    , m_param1(_T(""))
    , m_param2(_T(""))
    , m_act(NULL)
{
    m_typeMap["动画"] = "animation";
    m_typeMap["音效"] = "sound";
    m_typeMap["普通攻击"] = "hit";
    m_typeMap["技能攻击"] = "shit";
    m_typeMap["脚步声"] = "footstep";

    for (map<string, string>::iterator i = m_typeMap.begin(); i != m_typeMap.end(); ++i)
    {
        m_typeMap2[i->second] = i->first;
    }
}

CDlgPose::~CDlgPose()
{
}



void CDlgPose::DoDataExchange(CDataExchange* pDX)
{
    CDialog::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_LIST_EVENT, m_list);
    DDX_Text(pDX, IDC_EDIT_EVENT_ACTION, m_action);
    DDX_Text(pDX, IDC_EDIT_EVENT_PARAM, m_param);
    DDX_Text(pDX, IDC_EDIT_KEY_FRAME, m_keyFrame);
    DDX_Text(pDX, IDC_EDIT_EVENT_PARAM_1, m_param1);
    DDX_Text(pDX, IDC_EDIT_EVENT_PARAM_2, m_param2);
}


BEGIN_MESSAGE_MAP(CDlgPose, CDialog)
	ON_BN_CLICKED(IDC_BUTTON_ADD_EVENT, OnBnClickedButtonAddEvent)
	ON_BN_CLICKED(IDC_BUTTON_DEL_EVENT, OnBnClickedButtonDelEvent)
	ON_LBN_SELCHANGE(IDC_LIST_EVENT, OnLbnSelchangeListEvent)
	ON_BN_CLICKED(IDC_BUTTON_UPDATE, OnBnClickedButtonUpdate)
    ON_BN_CLICKED(IDC_BUTTON_F0, &CDlgPose::OnBnClickedButtonF0)
    ON_CBN_SETFOCUS(IDC_EDIT_EVENT_PARAM_1, &CDlgPose::OnCbnSetfocusEditEventParam1)
    ON_CBN_SELCHANGE(IDC_EDIT_EVENT_ACTION, &CDlgPose::OnCbnSelchangeEditEventAction)
    ON_BN_CLICKED(IDOK, &CDlgPose::OnBnClickedOk)
END_MESSAGE_MAP()


// CDlgPose 消息处理程序
void CDlgPose::OnBnClickedButtonAddEvent()
{
	if(!m_pose) return;
	
	SRT_PoseEvent event;
	event.Action = "animation";
	event.Param	 = "";
	event.KeyFrame = m_pose->StartFrm;
	m_pose->EventList.push_back(event);

	RebuildEventList();
    m_list.SetCurSel(m_list.GetCount() - 1);
    OnLbnSelchangeListEvent();
    UpdateLayout();
}

void CDlgPose::OnBnClickedButtonDelEvent()
{
	CString tmp;
	int i,j;
	i = m_list.GetCurSel();
	if(i != LB_ERR)
    {
	    vector<SRT_PoseEvent>::iterator it;
	    for(j=0,it=m_pose->EventList.begin(); 
		    it!=m_pose->EventList.end(); it++,j++)
	    {
		    if(j==i)
		    {
			    m_pose->EventList.erase(it);
			    RebuildEventList();
			    break;
		    }
	    }
    }

    if (m_list.GetCount())
        m_list.SetCurSel(max(min(m_list.GetCount() - 1, i), 0));
    OnLbnSelchangeListEvent();
    UpdateLayout();
}

BOOL CDlgPose::OnInitDialog()
{
	CDialog::OnInitDialog();
	
	RebuildEventList();

    CComboBox* p = (CComboBox*)GetDlgItem(IDC_EDIT_EVENT_ACTION);
    p->AddString("动画");
    p->AddString("音效");
    p->AddString("普通攻击");
    p->AddString("技能攻击");
    p->AddString("脚步声");


    GetDlgItem(IDC_STATIC_1)->ShowWindow(FALSE);
    GetDlgItem(IDC_STATIC_2)->ShowWindow(FALSE);
    GetDlgItem(IDC_EDIT_EVENT_PARAM_1)->ShowWindow(FALSE);
    GetDlgItem(IDC_EDIT_EVENT_PARAM_2)->ShowWindow(FALSE);


	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}

void CDlgPose::RebuildEventList()
{
	if(!m_pose) return;

	while(m_list.GetCount())
	{
		m_list.DeleteString(0);
	}
 	for(int i=0; i<m_pose->EventList.size(); i++)
	{
		m_list.InsertString(i, m_typeMap2[m_pose->EventList[i].Action.c_str()].c_str());
	}
}

void CDlgPose::OnUpdateParam1()
{
    CComboBox* p = (CComboBox*)GetDlgItem(IDC_EDIT_EVENT_PARAM_1);

    p->ResetContent();

    if (m_action == "动画")
    {
		// Modified by Wayne Wong 2010-11-09
        //CRT_ActorInstance* act = LoadActorInstance(m_param);
		CRT_ActorInstance* act = RtcGetActorManager()->CreateActor(m_param, true);

        if (!act)
            return;

		// Modified by Wayne Wong 2010-11-09
        if (act->GetCore()->m_poseMap.size())
        {
            for (TPoseMap::iterator i = act->GetCore()->m_poseMap.begin();
                 i != act->GetCore()->m_poseMap.end(); ++i)
            {
                const string& str = i->first;
                p->AddString(str.c_str());
            }

            int pos = p->FindString(0, m_param1);
            if (pos != -1)
                p->SetCurSel(pos);
        }

		// Modified by Wayne Wong 2010-11-09
        //delete act;
		Safe_ReleaseActor(act);
    }
    else if (m_action == "音效")
    {
        for (int i = 0; i <= 10; ++i)
        {
            char str[32];
            sprintf(str, "%.2f", i / 10.f);
            p->AddString(str);
        }

        int pos = p->FindString(0, m_param1);
        if (pos != -1)
            p->SetCurSel(pos);
    }
}

void CDlgPose::OnUpdateParam2()
{
    CComboBox* p = (CComboBox*)GetDlgItem(IDC_EDIT_EVENT_PARAM_2);
    p->ResetContent();

    if (m_action == "动画")
    {
        if (!m_act)
            return;

        CRT_Actor *cha = m_act->GetCore();
        for(int i=0;i<cha->m_bones.size();i++)
        {
            SRT_Bone *bone = &cha->m_bones[i];
            p->AddString(bone->Name.c_str());
        }

        int pos = p->FindString(0, m_param2);

        if (pos != -1)
            p->SetCurSel(pos);
    }
    else if (m_action == "脚步声")
    {
        if (!m_act)
            return;

        CRT_Actor *cha = m_act->GetCore();
        for(int i=0;i<cha->m_bones.size();i++)
        {
            SRT_Bone *bone = &cha->m_bones[i];
            p->AddString(bone->Name.c_str());
        }

        int pos = p->FindString(0, m_param2);

        if (pos != -1)
            p->SetCurSel(pos);
    }
  


}

void CDlgPose::UpdateLayout()
{
    GetDlgItem(IDC_EDIT_EVENT_PARAM)->ShowWindow(FALSE);
    GetDlgItem(IDC_BUTTON_F0)->ShowWindow(FALSE);
    GetDlgItem(IDC_STATIC_0)->ShowWindow(FALSE);
    GetDlgItem(IDC_STATIC_1)->ShowWindow(FALSE);
    GetDlgItem(IDC_STATIC_2)->ShowWindow(FALSE);
    GetDlgItem(IDC_EDIT_EVENT_PARAM_1)->ShowWindow(FALSE);
    GetDlgItem(IDC_EDIT_EVENT_PARAM_2)->ShowWindow(FALSE);

    if (m_action == "动画")
    {
        GetDlgItem(IDC_EDIT_EVENT_PARAM)->ShowWindow(TRUE);
        GetDlgItem(IDC_BUTTON_F0)->ShowWindow(TRUE);
        GetDlgItem(IDC_STATIC_0)->ShowWindow(TRUE);
        GetDlgItem(IDC_STATIC_1)->ShowWindow(TRUE);
        GetDlgItem(IDC_STATIC_2)->ShowWindow(TRUE);
        GetDlgItem(IDC_EDIT_EVENT_PARAM_1)->ShowWindow(TRUE);
        GetDlgItem(IDC_EDIT_EVENT_PARAM_2)->ShowWindow(TRUE);

        SetDlgItemText(IDC_STATIC_0, "Actor");
        SetDlgItemText(IDC_STATIC_1, "Pose");
        SetDlgItemText(IDC_STATIC_2, "Link");

    }
    else if (m_action == "音效")
    {
        GetDlgItem(IDC_EDIT_EVENT_PARAM)->ShowWindow(TRUE);
        GetDlgItem(IDC_BUTTON_F0)->ShowWindow(TRUE);
        GetDlgItem(IDC_STATIC_0)->ShowWindow(TRUE);
        GetDlgItem(IDC_STATIC_1)->ShowWindow(TRUE);
        GetDlgItem(IDC_EDIT_EVENT_PARAM_1)->ShowWindow(TRUE);

        SetDlgItemText(IDC_STATIC_0, "File Name");
        SetDlgItemText(IDC_STATIC_1, "Rate");
    }
    else if (m_action == "脚步声")
    {
        GetDlgItem(IDC_STATIC_2)->ShowWindow(TRUE);
        GetDlgItem(IDC_EDIT_EVENT_PARAM_2)->ShowWindow(TRUE);
        SetDlgItemText(IDC_STATIC_2, "Link");
    }

}

void CDlgPose::OnLbnSelchangeListEvent()
{
	int i = m_list.GetCurSel();
	if(i == LB_ERR) 
    {
        m_action = "";
        m_param = "";
        m_param1 = "";
        m_param2 = "";
        m_keyFrame = 0;
    }
    else
    {
	    m_action = m_typeMap2[m_pose->EventList[i].Action].c_str();

	    m_keyFrame = m_pose->EventList[i].KeyFrame;
        
        if (m_action == "音效")
        {
            string str = m_pose->EventList[i].Param;
            size_t pos = str.find_last_of(' ');
            
            if (pos == str.npos)
                pos = str.length();

            m_param = str.substr(0, pos).c_str();
            m_param1 = str.substr(pos, str.length() - pos).c_str();

        }
        else if (m_action == "脚步声")
        {
            m_param2 = m_pose->EventList[i].Param.c_str();
        }
        else
        {
            m_param = m_pose->EventList[i].Param.c_str();	
            m_param1 = m_pose->EventList[i].Param1.c_str();
            m_param2 = m_pose->EventList[i].Param2.c_str();
        }
    }

    CComboBox* p = (CComboBox*)GetDlgItem(IDC_EDIT_EVENT_ACTION);
    int pos = p->FindString(0, m_action);

    if (pos != -1)
        p->SetCurSel(pos);
    
    UpdateLayout();
    OnUpdateParam1();
    OnUpdateParam2();

	UpdateData(FALSE);
}

void CDlgPose::OnBnClickedButtonUpdate()
{
	if(!m_pose) return;

	// update 
	UpdateData(TRUE);
	int i;
	i = m_list.GetCurSel();
	int num = m_list.GetCount();
	if(i == LB_ERR) return;
	m_pose->EventList[i].Action = m_typeMap[m_action.GetBuffer()];
    m_action.ReleaseBuffer();

    if (m_action == "音效")
    {
        m_pose->EventList[i].Param  = m_param + " " + m_param1;
    }
    else if (m_action == "脚步声")
    {
        m_pose->EventList[i].Param  = m_param2;
    }
    else
    {
        m_pose->EventList[i].Param  = m_param;
        m_pose->EventList[i].Param1  = m_param1;
        m_pose->EventList[i].Param2  = m_param2;
    }

	if(m_keyFrame < m_pose->StartFrm)
		m_keyFrame = m_pose->StartFrm;

	if(m_keyFrame > m_pose->EndFrm)
		m_keyFrame = m_pose->EndFrm;

	m_pose->EventList[i].KeyFrame = m_keyFrame;

    UpdateLayout();

    OnUpdateParam1();
    OnUpdateParam2();

	UpdateData(FALSE);
	RebuildEventList();
}

void CDlgPose::OnBnClickedButtonF0()
{
    char filter[] = "Actor Files (*.act)|*.act||";
    char bak[255];

    // save path
    GetCurrentDirectory(255,bak); 
    CFileDialog dlg(TRUE,NULL,NULL,NULL,filter,NULL);
    if(dlg.DoModal() == IDOK)
    {
        CString file = dlg.GetPathName();

        int pos = file.ReverseFind('\\');

        if (pos == -1)
            return;

        ++pos;

        m_param = file.Right(file.GetLength() - pos);
        UpdateData(FALSE);

        OnUpdateParam1();

        SetCurrentDirectory(bak);
        return;
    }	
   
}

void CDlgPose::OnCbnSetfocusEditEventParam1()
{
}

void CDlgPose::OnCbnSelchangeEditEventAction()
{
    UpdateData(TRUE);
    UpdateLayout();
}

void CDlgPose::OnBnClickedOk()
{
    OnBnClickedButtonUpdate();
    OnOK();
}
