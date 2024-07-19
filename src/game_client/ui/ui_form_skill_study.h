#ifndef _INC_UI_FORM_SKILL_STUDY_H_
#define _INC_UI_FORM_SKILL_STUDY_H_

class UIFormSkillStudy
{
public:
    UIFormSkillStudy();
    ~UIFormSkillStudy();
    void Show();
    void Hide();
    bool IsShowing();										// 是否已经显示
    void Refresh();

private:
    enum { SKILL_LIST_SIZE = 6};
    RtwWidget*          m_pFrmThis;
	RtwButton*          m_btnClose;
    RtwButton*          m_pStudyBtn[SKILL_LIST_SIZE];

private:
    void OnShow(void *,void *);

    void OnLClickStudy(RtwWidget* sender, void*);
    void OnLClickClose(RtwWidget* sender, void*);

	short ConvertIndexToID(int index);

};

#endif // _INC_UI_FORM_SKILL_STUDY_H_
