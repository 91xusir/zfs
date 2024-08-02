#include "gc_include.h"
#include "ui_form_msg.h"

unordered_map<string, RtwRefPtr<UIFormMsg>> UIFormMsg::s_forms;

UIFormMsg::UIFormMsg(FormType t, int layer)  //modify by fox for bug
    : m_autoDelete(false), m_formType(t), mDefaultLayer(true) {
    guard;

    switch (t) {
        case TYPE_OK:
            m_pFrmThis = LOAD_UI("ConfirmBox1");
            m_pFrmThis->CenterScreen();
            m_pFrmThis->Hide();
            (m_pFrmThis->GetChildWidget("layworld.ConfirmBox1.btnconfirm"))
                ->EvLClick.ClearAndSetDelegate(RTW_CALLBACK(this, UIFormMsg, OnLClick_OK));
            break;

        case TYPE_OK_CANCEL:
            m_pFrmThis = LOAD_UI("ConfirmBox2");
            m_pFrmThis->CenterScreen();
            m_pFrmThis->Hide();
            (m_pFrmThis->GetChildWidget("layworld.ConfirmBox2.btnconfirm"))
                ->EvLClick.ClearAndSetDelegate(RTW_CALLBACK(this, UIFormMsg, OnLClick_OK));

            (m_pFrmThis->GetChildWidget("layworld.ConfirmBox2.btncancel"))
                ->EvLClick.ClearAndSetDelegate(RTW_CALLBACK(this, UIFormMsg, OnLClick_Cancel));
            break;
    }
    unguard;
}

UIFormMsg::~UIFormMsg() {
    guard;

    //rem by fox for bug
    //if (m_pFrmThis != NULL && m_pFrmThis->IsValid())
    //{
    //	m_pFrmThis->Close();
    //}

    unguard;
}

RtwButton* UIFormMsg::GetButtonOK(FormType t) {
    switch (t) {
        case TYPE_OK:
            return (RtwButton*)m_pFrmThis->GetChildWidget("layworld.ConfirmBox1.btnconfirm");
            break;
        case TYPE_OK_CANCEL:
            return (RtwButton*)m_pFrmThis->GetChildWidget("layworld.ConfirmBox2.btnconfirm");
            break;
        default:
            break;
    }
    return (RtwButton*)m_pFrmThis->GetChildWidget("layworld.ConfirmBox1.btnconfirm");
}

//lyytodo  以下为脑瘫设计....公用一个ConfirmBox  后面修改
UIFormMsg* UIFormMsg::ShowStatic(const RtString& text, FormType t /* = TYPE_OK*/,
                                 bool modal /* = true*/, const string& id /*= ""*/,
                                 bool vDefaultLayer /*= true*/) {
    guard;
    //一个补丁//当不同类型的UIFormMsg，的ID是不应该一样的//add by fox for bug
    string FixID;
    switch (t) {
        case TYPE_OK:
            FixID = id + "type_ok";
            break;
        case TYPE_OK_CANCEL:
            FixID = id + "type_ok_cancel";
            break;
        default:
            FixID = id + "unknow";
            break;
    }

    unordered_map<string, RtwRefPtr<UIFormMsg>>::iterator iter = s_forms.find(FixID);

    if (iter != s_forms.end()) {  //已存在
        //需要重新初始化
        //<add by fox for bug>
        iter->second->EvOK.Clear();
        iter->second->EvCancel.Clear();
        iter->second->SetUserData(NULL);
        LOAD_UI("layworld.ConfirmBox2.btnconfirm")->SetParam1(NULL);
        LOAD_UI("layworld.ConfirmBox2.btncancel")->SetParam1(NULL);
        //</add by fox for bug>
        iter->second->Show(text, modal);
        iter->second->m_pFrmThis->CenterScreen();
        LOAD_UI("layworld.ConfirmBox2.btnconfirm")->SetParam1(iter->second);
        LOAD_UI("layworld.ConfirmBox2.btncancel")->SetParam1(iter->second);
        return iter->second;
    }

    UIFormMsg* form =
        RT_NEW UIFormMsg(t, vDefaultLayer ? -1 : 3);  //modify by fox for bug//-1就是1，会被清除掉

    form->m_id = FixID;

    s_forms[FixID] = form;

    form->Show(text, modal);

    form->mDefaultLayer = vDefaultLayer;
    LOAD_UI("layworld.ConfirmBox2.btnconfirm")->SetParam1(form);
    LOAD_UI("layworld.ConfirmBox2.btncancel")->SetParam1(form);

    return form;

    unguard;
}

void UIFormMsg::Show(const RtString& text, bool modal) {
    guard;

    std::string parentName = m_pFrmThis->getName() + ".lbinfoexit";
    m_pFrmThis->GetChildWidget(parentName)->SetText((char*)text);

    if (modal)
        m_pFrmThis->ShowModal();
    else
        m_pFrmThis->Show();

    m_pFrmThis->BringToTop();

    unguard;
}

void UIFormMsg::Hide() {
    guard;

    m_pFrmThis->Hide();

    unguard;
}

bool UIFormMsg::IsVisible() {
    guard;

    return m_pFrmThis != NULL && m_pFrmThis->IsVisible();

    unguard;
}

void UIFormMsg::OnLClick_OK(RtwWidget* sender, void*) {
    guard;

    EvOK(sender, weMouseClick);

    /*rem by fox for bug
	if (m_autoDelete)
		m_pFrmThis->Close();
	else
		m_pFrmThis->Hide();
*/
    m_pFrmThis->Hide();
    unguard;
}

void UIFormMsg::OnLClick_Cancel(RtwWidget* sender, void*) {
    guard;

    EvCancel(sender, weMouseClick);

    /*rem by fox for bug
	if (m_autoDelete)
		m_pFrmThis->Close();
	else
		m_pFrmThis->Hide();
*/
    m_pFrmThis->Hide();
    unguard;
}

/*rem by fox for bug
void UIFormMsg::OnClose(void*, void*)
{
	guard;

	s_forms.erase(m_id);

	if (m_autoDelete)
		delete this;

	unguard;
}
*/

/*rem by fox for bug
void UIFormMsg::Close()
{
	guard;
	m_pFrmThis->Close();
	unguard;
}
*/

void UIFormMsg::SetUserData(void* data) {
    guard;
    m_pFrmThis->SetUserData(data);
    unguard;
}

void* UIFormMsg::GetUserData() {
    guard;
    return const_cast<void*>(m_pFrmThis->GetUserData());
    unguard;
}

void UIFormMsg::OnKeyDown(RtwWidget* sender, RtwEventDelegate* e) {
    if (e->type == etKeyChar) {
        if (e->key.code == VK_ESCAPE) {
            if (m_formType == TYPE_OK)
                OnLClick_OK(sender, e);
            else if (m_formType == TYPE_OK_CANCEL)
                OnLClick_Cancel(sender, e);
        }
        if (e->key.code == VK_RETURN) {
            OnLClick_OK(sender, e);
        }
    }
};
