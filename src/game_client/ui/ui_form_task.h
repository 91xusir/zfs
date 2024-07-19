#ifndef UI_FORM_TASK_H
#define UI_FORM_TASK_H

#define SELECT_REWARD_MAX_COUNT     4

class UIFormTask
{
public:
	UIFormTask();
	virtual ~UIFormTask();

	void Update();
	void ResetRewardText();
	void ResetRewardImage();
	void ParseFixReward(STaskDesc* desc);
	void ShowRewardLable(std::string& imgName, std::string& texName);
	void ShowRewardItem(std::string& imgName, std::string& texName);
	void ParseRandomReward(STaskDesc* desc);
	void ShowRandomRewardItem(std::string& imgName, std::string& texName);
	void ResizeTaskReward();
	void OnShowHintText(RtwWidget* sender, void*);
	bool IsVisible();
	void Show();
	void Hide();
	void OnTreeNodeSelect1(RtwWidget* pSender, RtwEventDelegate* pEvent);
	void OnTreeNodeSelect2(RtwWidget* pSender, RtwEventDelegate* pEvent);
	void OnTreeNodeSelect3(RtwWidget* pSender, RtwEventDelegate* pEvent);
	void SetSelectedTaskID1(int taskId);
	void SetSelectedTaskID2(int taskId);
	void SetSelectedTaskID3(int taskId);
	void BuildTaskTree1();
	void BuildTaskTree2();
	void BuildTaskTree3();
	int  GetSelectedTaskID();
	//得到可接任务列表
	void GetTaskByGroup(vector<int> &taskId);
	void GetTaskByGroup(vector<STaskDesc*> &taskId);

public:
	void OnHide(RtwWidget* sender,void*);
	void ChangeSelectTab(RtwWidget* pSender, RtwEventDelegate* pEvent);
	void OnClickCancelTask(RtwWidget *sender,void *);
	void OnClickTaskTrack(RtwWidget *sender, void *);
	void OnClick_TrackTask(RtwWidget *sender, void *);      //----任务追踪按钮
	void OnClicked_ShareTask(RtwWidget *sender, void *);    //----任务共享按钮
	void OnTaskClose(RtwWidget *sender, void *);
	void OnClicked_CancelTaskTrue(void*, void*);
	void OnClicked_CancelTaskFalse(void*, void*);
	void OnClicked_AcceptShareTask(RtwWidget *sender, void *);   //----接受共享任务

private:
	RtwForm*        m_pFrmThis;
	RtwTree*        m_pTree1;
	RtwTree*        m_pTree2;
	RtwTree*        m_pTree3;
	RtwTab*         m_tab;
	RtwHtmlView*    m_taskDesc1;
	RtwWidget*      m_LableReward;
	RtwWidget*      m_FixLable1;          //----固定奖励
	RtwWidget*      m_LableImage1;
	RtwWidget*      m_LableText1;
	RtwWidget*      m_FixLable2;
	RtwWidget*      m_LableImage2;
	RtwWidget*      m_LableText2;
	RtwWidget*      m_FixLable3;
	RtwWidget*      m_LableImage3;
	RtwWidget*      m_LableText3;
	RtwWidget*      m_FixLable4;
	RtwWidget*      m_LableImage4;
	RtwWidget*      m_LableText4;
	RtwWidget*      m_formFix;           //---固定奖励Form
	RtwWidget*      m_formFixItem;
	RtwWidget*      m_FixItem1;          //---固定奖励道具
	RtwWidget*      m_ItemImage1;
	RtwWidget*      m_ItemText1;
	RtwWidget*      m_FixItem2;
	RtwWidget*      m_ItemImage2;
	RtwWidget*      m_ItemText2;
	RtwWidget*      m_FixItem3;
	RtwWidget*      m_ItemImage3;
	RtwWidget*      m_ItemText3;
	RtwWidget*      m_FixItem4;
	RtwWidget*      m_ItemImage4;
	RtwWidget*      m_ItemText4;
	RtwWidget*      m_formRandom;        //---随机奖励Form
	RtwWidget*      m_RandomItem1;       //---随机奖励道具
	RtwWidget*      m_RandomImage1;
	RtwWidget*      m_RandomText1;
	RtwWidget*      m_RandomItem2;
	RtwWidget*      m_RandomImage2;
	RtwWidget*      m_RandomText2;
	RtwWidget*      m_RandomItem3;
	RtwWidget*      m_RandomImage3;
	RtwWidget*      m_RandomText3;
	RtwWidget*      m_RandomItem4;
	RtwWidget*      m_RandomImage4;
	RtwWidget*      m_RandomText4;
	RtwHtmlView*    m_taskDesc2;
	RtwHtmlView*    m_taskDesc3;
	RtwButton*		m_pTaskTrack;          //----任务追踪
	RtwButton*		m_pTaskShared;          //----任务共享
	RtwButton*		m_pTaskCancel;          //----任务取消
	RtwButton*		m_pTaskAccept;          //----共享任务接受
	RtwButton*		m_pSharedCancel;        //----共享任务取消
	RtwButton*		m_Close;

	int				m_taskID;
	vector<int>     mGroupId1;
	vector<int>     mGroupId2;
	vector<int>     mGroupId3;
	std::vector<int>	taskid;            //---可接任务列表

	DWORD           m_LastTaskId1;           //当前任务中记住用户上次选中项的任务ID
	vector<int>     mExpandGroupId1;         //当前任务中记住用户展开项ID
	DWORD           m_LastTaskId2;           //可接任务中记住用户上次选中项的任务ID
	vector<int>     mExpandGroupId2;         //可接任务中记住用户展开项ID
	DWORD           m_LastTaskId3;           //共享任务中记住用户上次选中项的任务ID
	vector<int>     mExpandGroupId3;         //共享任务中记住用户展开项ID

	std::vector<std::string> vecFixLableHint;
	std::vector<std::string> vecFixImageHint;
	std::vector<std::string> vecRandomImageHint;
};

#endif //UI_FORM_TASK_H