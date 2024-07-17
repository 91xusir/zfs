#ifndef _INC_UI_LOGIN_LAYER_H_
#define _INC_UI_LOGIN_LAYER_H_

#include "ui_form_setting.h"
#include "soft_keyboard.h"
#include "UIForm_Server.h"

class UILayerLogin
{
public:
	UILayerLogin();

	~UILayerLogin();

private:
	void OnUpdateText(RtwWidget* sender, RtwEventDelegate*);

	void OnKey(RtwWidget* sender, RtwEventDelegate*);

	void OnHideKeyboard(void*, void*);

	void OnShowKeyboard(void*, void*);
	
	void ontab(RtwWidget* sender, RtwEventDelegate* e);

    void OnClicked_BackSelectServer(RtwWidget* sender, void*);

	void OnClicked_Login(void*, void*);

public:
	//强制登录按钮的响应事件
	void OnClicked_ForceLogin(void*, void*);

	void OnClicked_ForceLoginCancel(void*, void*);

	void OnClicked_EnterLogin(RtwWidget* sender, RtwEventDelegate* e);

private:
	void OnClicked_Quit(void*, void*);
	//领取密保
	void OnClicked_GetPassword(void*, void*);
	//忘记密码
	void OnClicked_ForgetPassword(void*, void*);
	//申请帐号
    void OnClicked_CreateAccount(void*, void*);

    void OnClicked_SelectServer(RtwWidget* sender, RtwEventDelegate* e);

    void OnClicked_SaveAccount(RtwWidget* sender, RtwEventDelegate* e);

	void OnClicked_Keyboard(void*, void*);

private:
    int             m_iCurrentSelectServer;
	CSoftKeyboard   m_Keyboard;

public:
    enum { MAX_BUTTON = 8 };
	RtwTextBox*		m_username;
	RtwTextBox*		m_password;
    CUiCheckButton* m_pBtnSaveAccount;
    RtwButton*      m_pServerListButton[MAX_BUTTON];
    RtwWidget*      m_pServerStatus[MAX_BUTTON];
	/* gao 2010.1.13
	服务器选择界面
	*/
	CUIForm_Server* m_formServer;
	/* end */
};

#endif // _INC_UI_LOGIN_LAYER_H_
