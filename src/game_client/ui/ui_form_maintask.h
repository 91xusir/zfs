#ifndef UI_FORM_MAIN_TASK_H
#define UI_FORM_MAIN_TASK_H

class UIFormMainTask
{
public:
	UIFormMainTask();
	virtual ~UIFormMainTask();

	void Update();
	bool IsVisible();
	void Show();
	void ShowTaskTree();
	void ShowTaskDesc();
	void Hide();
	void HideTaskTree(RtwWidget* sender,void*);
	void HideTaskDesc(RtwWidget* sender,void*);
	void OnTreeNodeSelect(RtwWidget* pSender, RtwEventDelegate* pEvent);
	void BuildTaskTree();
	void SetSelectedSectionID(int sectionId);

private:
	RtwForm*        m_pFrmThis;
	RtwButton*		m_CloseTree;
	RtwButton*		m_CloseDesc;
	RtwTree*        m_pTreeList;
	vector<int>     taskChapterId;
	vector<MainStoryData> taskMainLine;
	int             section;
};

#endif //UI_FORM_MAIN_TASK_H