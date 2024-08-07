#pragma once
#include <unordered_map>
#include <nlohmann/json_fwd.hpp>
#include <string>
#include <ui/rtw_button.h>
#include <ui/rtw_textbox.h>

class UILayerSelectChar {
   private:
    //选人界面
    void OnClicked_SelectChar(RtwWidget* sender, RtwEventDelegate* e);
    void OnClicked_SelectShangOrZhou(RtwWidget* sender, RtwEventDelegate* e);
    void OnClicked_Delete(void*, void*);  //删除人物
    void OnConfirm_Delete(void*, void*);  //确认删除
    //创人界面
    void OnClicked_EngterCreateChar(void*, void*);
    void OnClicked_RandName(void*, void*);
    void OnSetRandName(const nlohmann::json& json_response);
    //进入游戏
    void OnClicked_Enter(void*, void*);
    //返回按钮
    void OnClicked_Back(void*, void*);
    //人物左旋按钮
    void OnClicked_LeftRotation(void*, void*);
    void OnClicked_LMouseDown(void*, void*);
    void OnClicked_LMouseUp(void*, void*);
    //人物右旋按钮
    void OnClicked_RightRotation(void*, void*);
    void OnClicked_ChangeHide(RtwWidget* sender, RtwEventDelegate* e);

   public:
    UILayerSelectChar();
    ~UILayerSelectChar();
    void OnReceivePasswordConfirm(char lRet);  //角色删除密码确认回调
    void OnReceiveSetPassword(char lRet);
    void OnReceiveDelPassword(char lRet);
    void OnUpdateText(RtwWidget* sender, RtwEventDelegate*);

   public:
    constexpr static int                   MaxUserCharBtn = 3;
    std::unordered_map<short, std::string> m_faction = {{0, "周"}, {1, "商"}, {3, "中立"}};
    CUiCheckButton*                        m_charBtnArray[MaxUserCharBtn];
    RtwTextBox*                            m_usrRoleName;  //创建角色名称
};
