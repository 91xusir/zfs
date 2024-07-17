// MergeActorDlg.cpp : implementation file
//

#include "stdafx.h"
#include "tool_character.h"
#include "MergeActorDlg.h"


// CMergeActorDlg dialog

IMPLEMENT_DYNAMIC(CMergeActorDlg, CDialog)

CMergeActorDlg::CMergeActorDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CMergeActorDlg::IDD, pParent)
    , m_StcActorName(_T(""))
{

}

CMergeActorDlg::~CMergeActorDlg()
{
}

void CMergeActorDlg::DoDataExchange(CDataExchange* pDX)
{
    CDialog::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_LIST_ACTOR, m_ListActor);
    DDX_Control(pDX, IDC_PROGRESS, m_Progress);
    DDX_Text(pDX, IDC_ACTOR1, m_StcActorName);
}


BEGIN_MESSAGE_MAP(CMergeActorDlg, CDialog)
    ON_BN_CLICKED(IDC_BUTTON_ADD, &CMergeActorDlg::OnBnClickedButtonAdd)
    ON_BN_CLICKED(IDOK, &CMergeActorDlg::OnBnClickedMerge)
    ON_BN_CLICKED(IDC_BUTTON_CLR, &CMergeActorDlg::OnBnClickedButtonClr)
    ON_BN_CLICKED(IDC_ACTOR_DST, &CMergeActorDlg::OnBnClickedActorDst)
END_MESSAGE_MAP()


// CMergeActorDlg message handlers

void CMergeActorDlg::OnBnClickedButtonAdd()
{
    char bak[255];
    GetCurrentDirectory(255,bak); 

    const int MaxOpenFile = 512;
    CString filePath;  
    CFileDialog dlg(
        TRUE,  
        NULL,  
        NULL,  
        OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT | OFN_ALLOWMULTISELECT | OFN_EXPLORER,    
        "Actor File(.act)|*.act|All Files(*.*)|*.*||",  
        NULL);    

    TCHAR* pBuffer = new TCHAR[MAX_PATH * MaxOpenFile];
    *pBuffer = 0;
    dlg.m_ofn.lpstrFile = pBuffer;  
    dlg.m_ofn.nMaxFile  = MaxOpenFile * MAX_PATH;  
    dlg.m_ofn.lStructSize = 88;

    if (dlg.DoModal() == IDOK)  
    {  
        POSITION pos = dlg.GetStartPosition();  

        while (pos != NULL)  
        {  
            filePath = dlg.GetNextPathName(pos);   
            m_ListActor.AddString(filePath);
        }  
    }   

    delete[] pBuffer;

    m_Progress.SetPos(0);
    SetCurrentDirectory(bak);
}

void CMergeActorDlg::OnBnClickedMerge()
{
    RtArchive *ar = RtCoreFile().CreateFileReader(m_StcActorName.GetBuffer());
    m_StcActorName.ReleaseBuffer();

    if (!ar)
        return;

    RtObject* o = ar->ReadObject(NULL);
    delete ar;
    
    if (!o || !o->IsKindOf(RT_RUNTIME_CLASS(CRT_Actor)))
    {
        delete o;
        return;
    }

    CRT_Actor* act = (CRT_Actor*)o;

    int nNumMerge = 0;
    bool bOnlyPose = !!IsDlgButtonChecked(IDC_REPLACE);

    m_Progress.SetRange32(0, m_ListActor.GetCount() - 1);
    m_Progress.SetPos(0);

    for (int i = 0; i < m_ListActor.GetCount(); ++i)
    {
        TCHAR fileName[MAX_PATH];
        m_ListActor.GetText(i, fileName);

        m_Progress.SetPos(i);

        RtArchive *ar = RtCoreFile().CreateFileReader(fileName);

        if (!ar)
            continue;

        RtObject* o = ar->ReadObject(NULL);

        if (!o || !o->IsKindOf(RT_RUNTIME_CLASS(CRT_Actor)))
        {
            delete o;
            delete ar;
            continue;
        }

        CRT_Actor* a = (CRT_Actor*)o;

        bool bRes = false;

        if (bOnlyPose)
            bRes = act->MergeEvent(a);
        else
            bRes = act->Merge(a);

        if (bRes)
            ++nNumMerge;

        delete a;
        delete ar;
    }


    ar = RtCoreFile().CreateFileWriter(NULL, m_StcActorName.GetBuffer());
    m_StcActorName.ReleaseBuffer();

    ASSERT(ar);
    ar->WriteObject(act);
    ar->Close();

    delete ar;
    delete act;

    CString strInfo;
    strInfo.Format(TEXT("合并完成,共合并Actor:%d"), nNumMerge + 1);
    MessageBox(strInfo.GetBuffer(), TEXT("合并完成"), MB_OK);
    m_Progress.SetPos(0);
}

void CMergeActorDlg::OnBnClickedButtonClr()
{
    m_ListActor.ResetContent();
    m_Progress.SetPos(0);
}

void CMergeActorDlg::OnBnClickedActorDst()
{
    char bak[255];
    GetCurrentDirectory(255,bak); 

    CFileDialog dlg(
        TRUE,  
        NULL,  
        NULL,  
        OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT | OFN_EXPLORER,    
        "Actor File(.act)|*.act|All Files(*.*)|*.*||",  
        NULL);    

    if (dlg.DoModal() == IDOK)
    {
        m_StcActorName = dlg.GetPathName();
        UpdateData(FALSE);
    }

    SetCurrentDirectory(bak);
}
