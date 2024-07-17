#ifndef UI_FORM_DUPE_H
#define UI_FORM_DUPE_H
 
class UIFormDupe
{
	struct ST_TaskInfo
	{
		long		taskid;
		std::string taskname;
		int			ifinished;
		std::string link;
	};
	struct ST_FbTask
	{
		std::string fbname;
		int			fblevel;
		std::string	fblink;
		std::vector<ST_TaskInfo>	tasklist;
	};

public:
	UIFormDupe();
	virtual ~UIFormDupe();

	void Refresh();
	bool IsVisible();
	void Show();
	void Hide();
	void OnTreeNodeSelect(RtwWidget* pSender, RtwEventDelegate* pEvent);
	void OnUIHyperlink(RtwWidget* pSender, RtwEventDelegate* pEvent);
	void OnBtnFinishTaskClicked(RtwWidget* sender, void*);
	void BuildTaskTree();

	void SetURL(const char* szURL);
	const char* GetURL();

	int  GetSelectedTaskID() { return m_taskID; }
	void SetSelectedTaskID();
	void AddReward(DWORD id) { m_rID.push_back(id); }
	void ClearReward(){ m_rID.clear(); }
	void SetHaveSReward(bool b) { m_bHaveSReward = b; }

public:
	void OnHtmlAction_HtmlHelpView(RtwWidget* sender, void*);
	void OnHide(RtwWidget* sender,void*);

private:
	//void SetTreeNodeName();

private:
	RtwForm*      m_pFrmThis;
	RtwHtmlView*    m_pHtmlView;
	RtwTree*        m_pTree;
	RtwButton*		m_pBtnFinishTask;
	RtwButton*		m_Close;
	std::string				m_strURL;
	std::vector<ST_FbTask>	m_task;
	std::string				m_strDif[5];
	std::string				m_strFin[3];
	DWORD					m_dwColor[5];
	int						m_taskID;
	bool					m_bHaveSReward;
	std::vector<DWORD>		m_rID;
};

#endif //UI_FORM_DUPE_H