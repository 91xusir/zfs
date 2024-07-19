#ifndef _INC_UI_FORM_CRE_INFO_H_
#define _INC_UI_FORM_CRE_INFO_H_

class UIFormCreInfo
{
public:

	void Show();
	void Hide();
	bool IsVisible();										// 是否已经显示
	void Refresh();
	int GetActorID(){guard;return m_nActorID;unguard;}
	/********gao 2009.11.9*******/
	void SetMonTextGray();
	void SetMonTextGolden();
	/********************************/

	int	m_nActorID;
	string m_strName;
	string m_strFaceFile;
	int m_iLevel;
	int m_nHpTotal;
	int m_nHpRemain;

private:
	RtwWidget				*m_pFrmThis;		// 整个窗口
	RtwWidget				*m_pFace;
	RtwWidget				*m_pName;
	RtwWidget				*m_pLevel;
	RtwProgressBar			*m_pHPs;

private:
	void OnPreDraw();

public:
	UIFormCreInfo();
	~UIFormCreInfo();
};

/* gao 2009.11.9
管理怪物血条的类
*/
class UIFormMonInfo
{
public:
	UIFormMonInfo();
	~UIFormMonInfo();
	void Show();
	void Hide();
	bool IsVisible();										// 是否已经显示
	void Refresh();
	void SetMonTextGray();
	void SetMonTextWhite();
	int GetActorID(){guard;return m_nActorID;unguard;}

	int	m_nActorID;
	string m_strName;
	string m_strFaceFile;
	int m_iLevel;
	int m_nHpTotal;
	int m_nHpRemain;

private:
	RtwWidget					*m_pFrmThis;		// 整个窗口
	RtwWidget					*m_pName;
	RtwWidget					*m_pFace;
	RtwWidget					*m_pLevel;
	RtwProgressBar				*m_pHPs;

	void OnPreDraw();
};
/* end */

/* gao 2009.12.21
管理吟唱的UI的类
*/
class CUI_Form_Chant
{
public:
	CUI_Form_Chant();
	~CUI_Form_Chant();
	void Show();
	void Hide();
	bool IsVisible();
	void Refresh(float fPercent);
	void SetSkillName(const char* strName);
private:
	RtwWidget* m_pFrmThis;
	RtwWidget* m_pName;
	RtwProgressBar* m_pChantBar;
};

// add by zheng 10.01.28
class CUIForm_Collect
{
public:
	CUIForm_Collect();
	~CUIForm_Collect();
	void Show();
	void Hide();
	bool IsVisible();
	void SetCollectTime(long millTime);
	void OnRun();
	void Revert();
	bool IsFull();
private:
	RtwWidget* m_pFrmThis;
	RtwProgressBar* m_pCollectBar;
	bool m_bStart;
	long m_lCollectTime;
	long m_lFinishTime;
	float m_value;
};

#endif // _INC_UI_FORM_CRE_INFO_H_
