#pragma once
#include "afxwin.h"


// CDlgNpc dialog

class CDlgNpc : public CDialog
{
	DECLARE_DYNAMIC(CDlgNpc)

public:
	CDlgNpc(CWnd* pParent = NULL);   // standard constructor
	virtual ~CDlgNpc();

// Dialog Data
	enum { IDD = IDD_DIALOG_NPC };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedOk();
	/*
	CEdit m_SerialNum;    //���
    CEdit m_NpcName;      //NPC����
	CEdit m_NpcFuncName;  //NPC��������
    CEdit m_ModelId;      //ģ�ͱ��
    CEdit m_Posx;         //λ�õ�x����
    CEdit m_PosY;         //λ�õ�y����
    //CEdit m_PosZ;         //λ�õ�z����
    CEdit m_Dirx;         //�����x����
    CEdit m_Diry;         //�����y����
   // CEdit m_Dirz;         //�����z����
    CEdit m_ScriptEdit;   //�ű�����
    CEdit m_DialogEdit;   //�Ի�����
    CEdit m_AttenEdit;    //��ע����
    CEdit m_VertionEdit;  //�汾����
	*/
	CString m_SerialNum;
	CString m_NpcName;
	CString m_NpcFuncName;
	CString m_szModelId;
	CString m_szPosx;
	CString m_Posy;
	CString m_Dirx;
	CString m_Diry;
	CString m_szScript;
	CString m_szDialog;
	CString m_szAtten;
	CString m_szVertion;
};
