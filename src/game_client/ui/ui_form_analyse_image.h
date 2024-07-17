#ifndef _INC_UI_FORM_ANALYSE_IMAGE_H_
#define _INC_UI_FORM_ANALYSE_IMAGE_H_

#define ANALYSE_IMAGE_SELECTION_COUNT	8

class UIFormAnalyseImage
{
public:
	void Refresh();
	void Show();
	void Hide();
	bool IsVisible();

	void Run();

	void SetData(const char* data, const long dataLen, const char* strSelections, int nSelectNum, int nSecondsLeft, int nResendLeft, int nErrorLeft);
	void ClearData();

private:
	RtwWidget *m_pFrmThis;		// 整个窗口
	RtwWidget *m_pBtnApplyNext;	
	RtwWidget *m_pText;
	RtwWidget *m_pImage;
	RtwWidget *m_pTimeLeft;
	RtwWidget *m_arrSelections[ANALYSE_IMAGE_SELECTION_COUNT];
    RtwWidget *m_pType1Hint;
    RtwWidget *m_pType2Hint;
    RtwWidget *m_pType2Text;
    RtwWidget *m_pType2Ok;

	void OnSelect(RtwWidget* sender, void*);
	void OnBtnApplyNext(RtwWidget* sender, void*);
    void OnBtnOK(RtwWidget* sender, void*);
    void OnKeyType2Text(RtwWidget* sender, void*);

	CM_MEMDEF(m_Characters, ANALYSE_IMAGE_SELECTION_COUNT)
	int m_nSelectNum;		// 需要选择第几个字符
	int m_nEndTick;			// 结束的时间
	int m_nResendLeft;		// 可重发次数
	int m_nErrorLeft;		// 可识别错误次数

	DWORD m_dwSetDataTick;

private:
	void OnPreDraw();
	void AskForAnotherString();
    void AnswerQuestion(const std::string& str);

public:
	UIFormAnalyseImage();
	~UIFormAnalyseImage();
};

#endif // _INC_UI_FORM_ANALYSE_IMAGE_H_
