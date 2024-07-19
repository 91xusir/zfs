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
	CEdit m_SerialNum;    //序号
    CEdit m_NpcName;      //NPC名字
	CEdit m_NpcFuncName;  //NPC功能名称
    CEdit m_ModelId;      //模型编号
    CEdit m_Posx;         //位置的x坐标
    CEdit m_PosY;         //位置的y坐标
    //CEdit m_PosZ;         //位置的z坐标
    CEdit m_Dirx;         //朝向的x坐标
    CEdit m_Diry;         //朝向的y坐标
   // CEdit m_Dirz;         //朝向的z坐标
    CEdit m_ScriptEdit;   //脚本内容
    CEdit m_DialogEdit;   //对话内容
    CEdit m_AttenEdit;    //备注内容
    CEdit m_VertionEdit;  //版本内容
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
