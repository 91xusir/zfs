#pragma once
#include <unordered_map>
#include <nlohmann/json_fwd.hpp>
#include <string>
#include <ui/rtw_button.h>
#include <ui/rtw_textbox.h>

class UILayerSelectChar {
   private:
    //ѡ�˽���
    void OnClicked_SelectChar(RtwWidget* sender, RtwEventDelegate* e);
    void OnClicked_SelectShangOrZhou(RtwWidget* sender, RtwEventDelegate* e);
    void OnClicked_Delete(void*, void*);  //ɾ������
    void OnConfirm_Delete(void*, void*);  //ȷ��ɾ��
    //���˽���
    void OnClicked_EngterCreateChar(void*, void*);
    void OnClicked_RandName(void*, void*);
    void OnSetRandName(const nlohmann::json& json_response);
    //������Ϸ
    void OnClicked_Enter(void*, void*);
    //���ذ�ť
    void OnClicked_Back(void*, void*);
    //����������ť
    void OnClicked_LeftRotation(void*, void*);
    void OnClicked_LMouseDown(void*, void*);
    void OnClicked_LMouseUp(void*, void*);
    //����������ť
    void OnClicked_RightRotation(void*, void*);
    void OnClicked_ChangeHide(RtwWidget* sender, RtwEventDelegate* e);

   public:
    UILayerSelectChar();
    ~UILayerSelectChar();
    void OnReceivePasswordConfirm(char lRet);  //��ɫɾ������ȷ�ϻص�
    void OnReceiveSetPassword(char lRet);
    void OnReceiveDelPassword(char lRet);
    void OnUpdateText(RtwWidget* sender, RtwEventDelegate*);

   public:
    constexpr static int                   MaxUserCharBtn = 3;
    std::unordered_map<short, std::string> m_faction = {{0, "��"}, {1, "��"}, {3, "����"}};
    CUiCheckButton*                        m_charBtnArray[MaxUserCharBtn];
    RtwTextBox*                            m_usrRoleName;  //������ɫ����
};
