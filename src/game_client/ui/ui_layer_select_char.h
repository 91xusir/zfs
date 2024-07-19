#ifndef _INC_UI_LAYER_SELECT_CHAR_H_
#define _INC_UI_LAYER_SELECT_CHAR_H_

#include "num_keyboard.h"

class UILayerSelectChar
{
private:
	//ѡ�˽���
	//�ĸ��½����ﰴť
	void OnClicked_CreateChar1(void*, void*);
	void OnClicked_CreateChar2(void*, void*);
	void OnClicked_CreateChar3(void*, void*);
	void OnClicked_CreateChar4(void*, void*);
	//�ָ�����
	void OnClicked_Restore(void*, void*);
	//ɾ������
	void OnClicked_Delete(void*, void*);
	//��������
	void OnClicked_SetCharPwd(void*, void*);
	//�������
	void OnClicked_ChangeCharPwd(void*, void*);
	//�������
	void OnClicked_DelCharPwd(void*, void*);
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
	//�˳���Ϸ
	void OnClicked_Exit(void*, void*);
	//����رտ�
	void OnClicked_ClosePwd(void*, void*);

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
	    RtwTextBox*		m_numpwd;                 //�����ɫ������
		RtwTextBox*		m_charname;					//��ɫ����
		RtwButton*      m_closeBtn;                 //�����رհ�ť
		CNumKeyboard    m_numKeyboard;              //���ּ���

		int             m_iConfirmCnt;
		int             m_CurBtnIdx;                //��ǰ��ť������
		std::string     m_OldPwd;
		std::string     m_NewPwd;
};

#endif // _INC_UI_LAYER_SELECT_CHAR_H_
