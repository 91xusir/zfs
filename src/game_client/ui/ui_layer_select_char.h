#ifndef _INC_UI_LAYER_SELECT_CHAR_H_
#define _INC_UI_LAYER_SELECT_CHAR_H_

#include "num_keyboard.h"

class UILayerSelectChar
{
private:
	//选人界面
	//四个新建人物按钮
	void OnClicked_CreateChar1(void*, void*);
	void OnClicked_CreateChar2(void*, void*);
	void OnClicked_CreateChar3(void*, void*);
	void OnClicked_CreateChar4(void*, void*);
	//恢复人物
	void OnClicked_Restore(void*, void*);
	//删除人物
	void OnClicked_Delete(void*, void*);
	//设置密码
	void OnClicked_SetCharPwd(void*, void*);
	//变更密码
	void OnClicked_ChangeCharPwd(void*, void*);
	//解除密码
	void OnClicked_DelCharPwd(void*, void*);
	//人物角色---男性
	void OnClicked_SetSexMale(void*, void*);
	//人物角色---女性
	void OnClicked_SetSexFemale(void*, void*);
	//人物左旋按钮
	void OnClicked_LeftRotation(void*, void*);
	void OnClicked_LMouseDown(void*, void*);
	void OnClicked_LMouseUp(void*, void*);
	//人物右旋按钮
	void OnClicked_RightRotation(void*, void*);
	//进入游戏
	void OnClicked_Enter(void*, void*);
	//返回按钮
	void OnClicked_Back(void*, void*);
	//退出游戏
	void OnClicked_Exit(void*, void*);
	//密码关闭框
	void OnClicked_ClosePwd(void*, void*);

	//创建人物角色界面
	//角色名输入框
	//人物生肖选择框
	void OnClicked_PrevAnimal(void*, void*);
	void OnClicked_NextAnimal(void*, void*);
	//人物角色发型
	void OnClicked_PrevHair(void*, void*);
	void OnClicked_NextHair(void*, void*);
	//人物角色脸型
	void OnClicked_PrevFace(void*, void*);
	void OnClicked_NextFace(void*, void*);
	//人物角色头像
	void OnClicked_PrevHead(void*, void*);
	void OnClicked_NextHead(void*, void*);
	//随机创建
	void OnClicked_RandomCreate(void*, void*);
	//完成创建
	void OnClicked_CreateChar(void*, void*);
	//四大门派---------明王殿,花间门,蜀山派,圣巫教
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
	    RtwTextBox*		m_numpwd;                 //人物角色密码锁
		RtwTextBox*		m_charname;					//角色名称
		RtwButton*      m_closeBtn;                 //密码框关闭按钮
		CNumKeyboard    m_numKeyboard;              //数字键盘

		int             m_iConfirmCnt;
		int             m_CurBtnIdx;                //当前按钮的索引
		std::string     m_OldPwd;
		std::string     m_NewPwd;
};

#endif // _INC_UI_LAYER_SELECT_CHAR_H_
