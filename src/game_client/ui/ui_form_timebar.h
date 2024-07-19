#ifndef _INC_UI_FORM_TIMEBAR_H_
#define _INC_UI_FORM_TIMEBAR_H_

class UIFormTimeBar
{
public:
	enum ETimeBarUseType
	{
		tbut_ItemCompose			= 1,
		tbut_CallPet				= 2,
		tbut_CallTaskItem           = 3,
		tbut_ItemDecompound			= 4,
		tbus_fuse					= 5,
		tbus_fuse_pet				= 6,
		tbus_catch_pet				= 7
	};

	UIFormTimeBar();
	~UIFormTimeBar();
	void ClearData();

	void Begin(ETimeBarUseType UseType, DWORD TimeProgress);
	void End(bool bNotify);
	void Hide(bool bNotify);
	bool IsVisible();										// 是否已经显示
	void Refresh();
	void Tick(DWORD Delta);

	RtwEventDispatcher EvTimeOver;

private:
	RtwWidget *m_pFrmThis;		// 整个窗口
	RtwProgressBar *m_pProgressBar;

private:
	void OnPreDraw();

	ETimeBarUseType m_UseType;
	DWORD m_BeginTime;
	DWORD m_ProgressTime;
};

#endif // _INC_UI_FORM_TIMEBAR_H_
