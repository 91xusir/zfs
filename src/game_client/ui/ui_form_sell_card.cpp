#include "gc_include.h"
#include "ui_form_sell_card.h"
#include "ui_form_msg.h"
#include "business.h"

UIFormSellCard::UIFormSellCard()
{
	guard;
    m_pFrmThis = LOAD_UI("fmsellcard");

    m_pTextCardID  = LOAD_UI_T(RtwTextBox, "fmsellcard.txtcard");
    m_pTextCardPwd = LOAD_UI_T(RtwTextBox, "fmsellcard.txtpass");
    m_pTextPrice   = LOAD_UI_T(RtwTextBox, "fmsellcard.txtprice");
    m_pBtnOK       = LOAD_UI_T(RtwButton,  "fmsellcard.btnok");

    RtwWidget* pLable3 = LOAD_UI("fmsellcard.lbrule3");
    rt2_sprintf(g_strStaticBuffer, "3.寄售价格只能在%d万～%d万的范围之内", CCardTradeList::e_lMinPrice/10000, CCardTradeList::e_lMaxPrice/10000);
    pLable3->SetText(g_strStaticBuffer);

    RtwButton* pBtnCancel = LOAD_UI_T(RtwButton, "fmsellcard.btncancel");

    m_pTextCardID->SetAlphaNumberOnly(true);
    m_pTextCardID->SetCapsLock(1);

    m_pTextCardPwd->SetAlphaNumberOnly(true);
    m_pTextCardPwd->SetCapsLock(1);

    m_pTextPrice->SetNumberOnly(true);

	m_pBtnOK->EvLClick                              += RTW_CALLBACK(this, UIFormSellCard, OnLClick_OK);
	m_pFrmThis->EvClose                             += RTW_CALLBACK(this, UIFormSellCard, OnClose);
	pBtnCancel->EvLClick                            += RTW_CALLBACK(this, UIFormSellCard, OnLClick_Cancel);
//	(m_pFrmThis->GetChildWidget("$hide"))->EvLClick += RTW_CALLBACK(this, UIFormSellCard, OnLClick_Cancel);

	m_pFrmThis->Hide();
	unguard;
}

UIFormSellCard::~UIFormSellCard()
{
	guard;	
	unguard;
}

void UIFormSellCard::Show(bool bModal)
{
	guard;

    m_pTextCardID->SetText("");
    m_pTextCardPwd->SetText("");
    m_pTextPrice->SetText("");

	if (bModal)
		m_pFrmThis->ShowModal();
	else
		m_pFrmThis->Show();

    m_pTextCardID->SetFocus();

	m_pFrmThis->BringToTop();

	unguard;
}

void UIFormSellCard::SetCardID(const char* szContent)
{
	guard;
    if (m_pTextCardID) m_pTextCardID->SetText(szContent);
	unguard;
}

void UIFormSellCard::SetUserData(long lNpcID, long lType)
{
    m_lNpcID = lNpcID;
    m_lType = lType;
}

void UIFormSellCard::Hide()
{
	guard;
	m_pFrmThis->Hide();
	unguard;
}

bool UIFormSellCard::IsVisible()
{
	guard;
	return m_pFrmThis != NULL && m_pFrmThis->IsVisible();
	unguard;
}

void UIFormSellCard::OnLClick_OK(RtwWidget* sender, void*)
{
	guard;

    int iLen = (int)m_pTextCardID->GetText().length();
    if (iLen<8 || iLen>20)
    {
        // 卡号长度错误
        UIFormMsg::ShowStatic(R(MSG_CARD_ID_LEN_ERR), UIFormMsg::TYPE_OK);
        m_pTextCardID->SetFocus();
        return;
    }

    iLen = (int)m_pTextCardPwd->GetText().length();
    if (iLen<6 || iLen>20)
    {
        // 密码长度错误
        UIFormMsg::ShowStatic(R(MSG_CARD_PWD_LEN_ERR), UIFormMsg::TYPE_OK);
        m_pTextCardPwd->SetFocus();
        return;
    }

    long lPrice = atol(m_pTextPrice->GetText().c_str());
    if (lPrice<CCardTradeList::e_lMinPrice || lPrice>CCardTradeList::e_lMaxPrice)
    {
        // 价格范围错误
        rt2_sprintf(g_strStaticBuffer, R(MSG_CARD_PRICE_ERR), CCardTradeList::e_lMinPrice, CCardTradeList::e_lMaxPrice);
        UIFormMsg::ShowStatic(g_strStaticBuffer, UIFormMsg::TYPE_OK);
        m_pTextPrice->SetFocus();
        return;
    }

    char szMD5[40];
    rtMD5StringString((char*)(m_pTextCardPwd->GetText().c_str()), szMD5);
    m_pTextCardPwd->SetText("");

    CG_CmdPacket* pPacket = NetBeginWrite();
    pPacket->WriteShort(c2r_consign_card);
    pPacket->WriteLong(m_lNpcID);
    pPacket->WriteByte((char)m_lType);
    pPacket->WriteString((char*)m_pTextCardID->GetText().c_str());
    pPacket->WriteString(szMD5);
    pPacket->WriteLong(lPrice);
    NetSend(pPacket);

	m_pFrmThis->Hide();
	unguard;
}

void UIFormSellCard::OnLClick_Cancel(RtwWidget* sender, void*)
{
	guard;
    m_pTextCardID->SetText("");
    m_pTextCardPwd->SetText("");
    m_pTextPrice->SetText("");
	m_pFrmThis->Hide();
	unguard;
}

void UIFormSellCard::OnClose(void*, void*)
{
}
