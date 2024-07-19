// DlgLink.cpp : implementation file
//

#include "stdafx.h"
#include "tool_character.h"
#include "DlgLink.h"
#include "MainFrm.h"

// CDlgLink dialog
IMPLEMENT_DYNAMIC(CDlgLink, CDialog)
CDlgLink::CDlgLink(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgLink::IDD, pParent)
	, m_bRotate(FALSE)
{

}

CDlgLink::~CDlgLink()
{
}

void CDlgLink::DoDataExchange(CDataExchange* pDX)
{
    CDialog::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_WEAPON_SLOT, m_weaList);
    DDX_Control(pDX, IDC_POSE, m_pose);
    DDX_Check(pDX, IDC_CHECK1, m_bRotate);
    DDX_Control(pDX, IDC_LIST, m_LisetImported);
}


BEGIN_MESSAGE_MAP(CDlgLink, CDialog)
	ON_BN_CLICKED(IDOK, OnBnClickedOk)
	ON_BN_CLICKED(IDC_DEL_WEAPON, OnBnClickedDelWeapon)
	ON_BN_CLICKED(IDC_LOAD_WEAPON, OnBnClickedLoadWeapon)
    ON_LBN_SELCHANGE(IDC_WEAPON_SLOT, &CDlgLink::OnLbnSelchangeWeaponSlot)
    ON_LBN_SELCHANGE(IDC_POSE, &CDlgLink::OnLbnSelchangePose)
    ON_LBN_SELCHANGE(IDC_LIST, &CDlgLink::OnLbnSelchangeList)
END_MESSAGE_MAP()

// CDlgLink message handlers
void CDlgLink::OnBnClickedOk()
{
	UpdateData(TRUE);
	// g_activeActor->m_bWeaRotate = m_bRotate;

	CString tmp;
	int i;
	i = m_weaList.GetCurSel();
	if(i != LB_ERR)
	{
		m_weaList.GetText(i,tmp);
		m_weaSlot = tmp;
	}

	i = m_pose.GetCurSel();
	if(i != LB_ERR)
	{
		m_pose.GetText(i,tmp);
		m_poseName = tmp;
	}
	CDialog::OnOK();
}

BOOL CDlgLink::OnInitDialog()
{
	CDialog::OnInitDialog();

	if(!g_activeActor || !g_activeActor->m_data) return TRUE;
	CRT_Actor *cha = g_activeActor->m_data;
	for(int i=0;i<cha->m_bones.size();i++)
	{
		SRT_Bone *bone = &cha->m_bones[i];
		m_weaList.InsertString(i,bone->Name.c_str());
	}

    for (size_t i = 0; i < g_activeActor->m_vSubActor.size(); ++i)
    {
        string t = g_activeActor->m_vSubActor[i]->GetCore()->m_diskFile;
        size_t p = t.find_last_of('\\');

        if (p != string::npos)
        {
            size_t p2 = t.find_last_of('.');
            if (p2 != string::npos)
                p2 = max(0, p2 - p - 1); 
            string name = t.substr(p + 1, p2);         
            m_LisetImported.AddString(name.c_str());
        }
    }
	
	return TRUE;  
	// return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CDlgLink::OnBnClickedDelWeapon()
{
    int i = m_LisetImported.GetCurSel();
    if (i != LB_ERR)
    {
		// Modified by Wayne Wong 2010-11-09
        //delete g_activeActor->m_vSubActor[i];
		RtcGetActorManager()->ReleaseActor(g_activeActor->m_vSubActor[i]);

        g_activeActor->m_vSubActor.erase(g_activeActor->m_vSubActor.begin() + i);
        m_LisetImported.DeleteString(i);
    }
    m_pose.ResetContent();
//	g_activeActor->DelWeapon();
}

void CDlgLink::OnBnClickedLoadWeapon()
{
	char filter[] = "Actor Files (*.act)|*.act||";
	char bak[255];

	// save path
	GetCurrentDirectory(255,bak); 
	CFileDialog dlg(TRUE,NULL,NULL,NULL,filter,NULL);
	if(dlg.DoModal() != IDOK) return;
	CString file = dlg.GetFileTitle();

	// restore path
	SetCurrentDirectory(bak);
	if(file.GetLength() <=0 ) return;

//	CRT_ActorInstance *cha = (CRT_ActorInstance*)ACTOR()->CreateInstance(ACTOR_RES_TYPE_ACT,file);
	// Modified by Wayne Wong 2010-11-09
	//CRT_ActorInstance *cha = LoadActorInstance(file);
	CRT_ActorInstance* cha = RtcGetActorManager()->CreateActor(file, true);

	if(!cha)
	{
		MessageBox("load file failed");
		// Modified by Wayne Wong 2010-11-09
		//delete cha;
	}
	else
	{
		// Modified by Wayne Wong 2010-11-09
		//cha->Create(RtGetRender());

	    //cha->LinkParent(g_activeActor->m_inst);
		//g_activeActor->m_weapon = cha;
        cha->PlayPose(m_poseName.c_str(),true);
        cha->LinkParent(g_activeActor->m_inst,m_weaSlot.c_str());

        g_activeActor->m_vSubActor.push_back(cha);
        m_LisetImported.AddString(file);
        m_LisetImported.SetCurSel(m_LisetImported.GetCount() - 1);

        m_pose.ResetContent();
		// get pose info
		TPoseMap::iterator it;
		int i=0;

		// Modified by Wayne Wong 2010-11-09
		for(it=cha->GetCore()->m_poseMap.begin();it!=cha->GetCore()->m_poseMap.end();it++)
		{
			SRT_Pose *pose = &(*it).second;
            m_pose.InsertString(i++,pose->Name.c_str());
		}
	}
}

void CDlgLink::OnLbnSelchangeWeaponSlot()
{
    CString tmp;
    int i = m_weaList.GetCurSel();
    if(i != LB_ERR)
    {
        m_weaList.GetText(i,tmp);
        m_weaSlot = tmp;

        i = m_LisetImported.GetCurSel();
        if (i != LB_ERR)
        {
            g_activeActor->m_vSubActor[i]->LinkParent(g_activeActor->m_inst,m_weaSlot.c_str());
        }
    }
}

void CDlgLink::OnLbnSelchangePose()
{
    CString tmp;
    int i = m_pose.GetCurSel();
    if(i != LB_ERR)
    {
        m_pose.GetText(i,tmp);
        m_poseName = tmp;

        i = m_LisetImported.GetCurSel();
        if (i != LB_ERR)
        {
            g_activeActor->m_vSubActor[i]->PlayPose(m_poseName.c_str(),true);
        }
    }
}

void CDlgLink::OnLbnSelchangeList()
{
    m_pose.ResetContent();

    int i = m_LisetImported.GetCurSel();
    if (i != LB_ERR)
    {
        CRT_ActorInstance* cha = g_activeActor->m_vSubActor[i];
        TPoseMap::iterator it;
        int i=0;
		// Modified by Wayne Wong 2010-11-09
        for(it=cha->GetCore()->m_poseMap.begin();it!=cha->GetCore()->m_poseMap.end();it++)
        {
            SRT_Pose *pose = &(*it).second;
            m_pose.InsertString(i++,pose->Name.c_str());
        }

        m_weaList.SetCurSel(-1);
        if (!cha->m_parentSlot.empty())
        {
            for (int j = 0; j < m_weaList.GetCount(); ++j)
            {
                char t[128];
                m_weaList.GetText(j, t);

                if (stricmp(t, cha->m_parentSlot.c_str()) == 0)
                    m_weaList.SetCurSel(j);
            }
        }

        m_pose.SetCurSel(-1);
		// Modified by Wayne Wong 2010-11-09
        //if (cha->m_curPose && !cha->m_curPose->Name.empty())
		if (cha->m_curPose.IsVaild() && !cha->m_curPose.Name.empty())
        {
            for (int j = 0; j < m_pose.GetCount(); ++j)
            {
                char t[128];
                m_pose.GetText(j, t);

                if (stricmp(t, cha->m_curPose.Name.c_str()) == 0)
                    m_pose.SetCurSel(j);
            }
        }
    }
}
