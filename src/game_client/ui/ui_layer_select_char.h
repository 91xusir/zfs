#include <unordered_map>
#ifndef _INC_UI_LAYER_SELECT_CHAR_H_
#define _INC_UI_LAYER_SELECT_CHAR_H_

class UILayerSelectChar {
   private:
    //ѡ�˽���
    //��ť�ص�
    void OnClicked_SelectChar(RtwWidget* sender, RtwEventDelegate* e);
    void OnClicked_SelectShangOrZhou(RtwWidget* sender, RtwEventDelegate* e);

    void OnClicked_EngterCreateChar(void*, void*);
    //ɾ������
    void OnClicked_Delete(void*, void*);

    //�����ɫ---����
    void OnClicked_SetSexMale(void*, void*);
    //�����ɫ---Ů��
    void OnClicked_SetSexFemale(void*, void*);
    //����������ť
    void OnClicked_LeftRotation(void*, void*);
    void OnClicked_LMouseDown(void*, void*);
    void OnClicked_LMouseUp(void*, void*);
    //����������ť
    void OnClicked_RightRotation(void*, void*);
    //������Ϸ
    void OnClicked_Enter(void*, void*);
    //���ذ�ť
    void OnClicked_Back(void*, void*);

    //���������ɫ����
    //��ɫ�������
    //������Фѡ���
    void OnClicked_PrevAnimal(void*, void*);
    void OnClicked_NextAnimal(void*, void*);
    //�����ɫ����
    void OnClicked_PrevHair(void*, void*);
    void OnClicked_NextHair(void*, void*);
    //�����ɫ����
    void OnClicked_PrevFace(void*, void*);
    void OnClicked_NextFace(void*, void*);
    //�����ɫͷ��
    void OnClicked_PrevHead(void*, void*);
    void OnClicked_NextHead(void*, void*);
    //�������
    void OnClicked_RandomCreate(void*, void*);
    //��ɴ���
    void OnClicked_CreateChar(void*, void*);
    //�Ĵ�����---------������,������,��ɽ��,ʥ�׽�
    void OnClicked_SelectMingwang(void*, void*);
    void OnClicked_SelectHuajian(void*, void*);
    void OnClicked_SelectShushan(void*, void*);
    void OnClicked_SelectShenwu(void*, void*);
    void OnConfirm_Delete(void*, void*);

   public:
    UILayerSelectChar();
    ~UILayerSelectChar();
    void OnReceiveSetPassword(char lRet);
    void OnReceivePasswordConfirm(char lRet);
    void OnReceiveDelPassword(char lRet);
    void OnUpdateText(RtwWidget* sender, RtwEventDelegate*);

   public:
    constexpr static int                   MaxUserCharBtn = 3;
    std::unordered_map<short, std::string> m_faction = {{1, "��"}, {2, "��"}, {3, "����"}};
    RtwTextBox*                            m_charname;  //������ɫ����
    CUiCheckButton*                        m_charBtnArray[MaxUserCharBtn];
};

#endif  // _INC_UI_LAYER_SELECT_CHAR_H_
