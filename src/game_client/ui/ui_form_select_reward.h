#ifndef _INC_UI_FORM_SELECT_REWARD_H_
#define _INC_UI_FORM_SELECT_REWARD_H_

#define UI_FORM_SELECT_REWARD_MAX_COUNT		8

class UIFormSelectReward
{
public:
	void Show();
	void Hide();
	bool IsVisible();
	void Refresh();

	void ResetRewardIDs(bool bShowOnly, DWORD taskID, std::vector<DWORD>& rewards)
	{
		m_bShowOnly = bShowOnly;
		m_TaskID = taskID;
		m_RewardIDs = rewards;
		m_SelectIndex = -1;
	}
	DWORD GetSelectRewardID()
	{
		DWORD RewardID = 0;
		if (m_SelectIndex>=0 && m_SelectIndex<(int)m_RewardIDs.size())
			RewardID = m_RewardIDs[m_SelectIndex];
		return RewardID;
	}

private:
	RtwWidget *m_pFrmThis;		// Õû¸ö´°¿Ú
	CUiCheckButton *m_pRewards[UI_FORM_SELECT_REWARD_MAX_COUNT];
	RtwButton *m_pOK;
	RtwButton *m_pCancel;
	RtwWidget *m_pTitle;

private:
	void OnPreDraw();
	void OnOK(RtwWidget* sender, void*);
	void OnCancel(RtwWidget* sender, void*);
	void OnGetHintText(RtwWidget* sender, void*);
	void OnLClickReward(RtwWidget* sender, void*);

	RtwImage* m_ptrHoverImage;

	SItemID m_TmpItems[UI_FORM_SELECT_REWARD_MAX_COUNT];

	bool m_bShowOnly;
	DWORD m_TaskID;
	std::vector<DWORD> m_RewardIDs;

	int m_SelectIndex;

public:
	UIFormSelectReward();
	~UIFormSelectReward();
};

#endif // _INC_UI_FORM_SELECT_REWARD_H_
