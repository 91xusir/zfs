#pragma once
#include "ui/rtw_ui.h"
#include "region_client.h"
#include <string>
#include <map>

using std::map;
#define TEAM_MAX_MEMBER 5
//组队内容相关结构体，记录相关信息
struct ApplyIfo
{
	long ID;				// 人物ID
	string name;			// 姓名
	int level;				// 等级
	string profession;		// 职业
	int animal;				// 生肖
	int power;				// 战力评估
	string time;
	ApplyIfo(long id,string na,int lev,string prf,int ani,int pow,string tim)
	{
		ID = id;
		name = na;
		level = lev;
		profession = prf;
		animal = ani;
		power = pow;
		time = tim;
	}
};
enum APPLYLISTTYPE
{
	Teammember,
	Apply,
	Undefine,
};
struct TeamMember
{
	RtwWidget* TeamMemberForm;
	RtwWidget* TeamMemberHead;
	RtwWidget* TeamMemberAnimal;
	RtwWidget* TeamMemberName;
	RtwWidget* TeamMemberLevel;
	RtwWidget* TeamMemberCombatPower;
	RtwWidget* TeamMemberProfession;
	bool	   IsShow;
	TeamMember():TeamMemberForm(NULL),TeamMemberHead(NULL),TeamMemberAnimal(NULL),TeamMemberName(NULL),TeamMemberLevel(NULL),TeamMemberCombatPower(NULL),TeamMemberProfession(NULL),IsShow(false){}
};
//队伍UI   李泽华 2010.06.01
class CUI_form_Team
{
public:
	CUI_form_Team(void);
	~CUI_form_Team(void);
	/* 客户端相关函数 */
	void Init();//初始化加载
	void Show();//显示
	void Hide();//隐藏
	void OnRefresh();//更新
	void OnShowTeammember();//显示队伍成员
	void OnShowAnimalBuff();//组队界面BUFF显示
	void OnBtnTransfer(ui::RtwWidget* vpSender, RtwEventDelegate* vpEvent);//转让队长
	void OnBtnGetaway(ui::RtwWidget* vpSender, RtwEventDelegate* vpEvent);//离开队伍
	void OnBtnKick(ui::RtwWidget* vpSender, RtwEventDelegate* vpEvent);//请离队员
	void OnBtnDisband(ui::RtwWidget* vpSender, RtwEventDelegate* vpEvent);//解散队伍
    void OnBtnDownRefresh(ui::RtwWidget* vpSender, RtwEventDelegate* vpEvent);//按下按钮时刷新
	void OnBtnShowBuff(ui::RtwWidget* vpSender, RtwEventDelegate* vpEvent);//打开组队加成界面
	void OnBtnLookInfo(ui::RtwWidget* vpSender, RtwEventDelegate* vpEvent);//查看信息
	void OnBtnAddFriend(ui::RtwWidget* vpSender, RtwEventDelegate* vpEvent);//添加好友
	void OnBtnLookIntroduce(ui::RtwWidget* vpSender, RtwEventDelegate* vpEvent);//组队BUFF说明	
	void OnSelectMember(ui::RtwWidget* vpSender, RtwEventDelegate* vpEvent);//选择队员
	void OnCloseForm(void* ,void*);//关闭
	void OnChangePickMode(CUiCheckButton* sender,void*);//改变拾取分配
	void OnGetPickModeHide(CUiCheckButton* sender,void*);//获得拾取分配说明
	void SetPickMode(byte vbType){if(m_pPickMode) m_pPickMode->SetSelectedItem(vbType);}//设置拾取分配
	void OnApplyListInsertNew(long id,string na,int lev,string prf,int ani,int pow);//申请列表添加新的
	void OnApplyListHint(ui::RtwWidget* vpSender, RtwEventDelegate* vpEvent);//申请列表 提示
	void OnApplyListBtnAgree(ui::RtwWidget* vpSender, RtwEventDelegate* vpEvent);//请求列表 同意
	void OnApplyListBtnDisagree(ui::RtwWidget* vpSender, RtwEventDelegate* vpEvent);//请求列表 拒绝
	void OnApplyListBtnClear(ui::RtwWidget* vpSender, RtwEventDelegate* vpEvent);//请求列表 关闭
	void OnApplyListRDMenuSelect(RtwWidget* vpSender, RtwEventDelegate* vpEvent);//申请列表右键菜单选中
	void OnApplyListPopUpMenu(ui::RtwWidget* vpSender, RtwEventDelegate* vpEvent);//申请列表右键菜单 
	void OnApplyListRDMenuUnFocus(ui::RtwWidget* vpSender, RtwEventDelegate* vpEvent);//右键菜单隐藏
	void OnBtnShowApply(ui::RtwWidget* vpSender, RtwEventDelegate* vpEvent);//显示申请列表
	void SetTeamButtonNormal();//设置按钮为正常状态
	void SetTeamButtonAnimation();//设置按钮为按下状态
	bool GetTeamImage(short ModelID,int HeadImageID, vector<SCharImage>& models,ui::RtwImage** HeadImage,short Profession,ui::RtwImage** ProfessionImage,short Animal,ui::RtwImage** AnimalImage);//获得队员相应图片
	bool IsVisibale(){return m_pTeam->IsVisible();}//是否存在
	/* 消息相关函数 */
	void OnReceiveApply();//收到同意
	void OnPreDraw();//绘制
private:
	ui::RtwWidget*		m_pTeam;//组队界面指针
	ui::RtwWidget*		m_pBuff;//组队加成界面指针
	ui::RtwWidget*		m_pNearPlayer;//附近玩家界面指针
	ui::RtwWidget*		m_pApply;//申请列表界面指针
	ui::RtwButton*		m_pCloseForm;//关闭组队窗口
	ui::RtwButton*		m_pTransFer;//转让队长按钮
	ui::RtwButton*		m_pKick;//请离队员按钮
	ui::RtwButton*		m_pDisband;//解散队伍按钮
	ui::RtwButton*		m_pRequest;//离开队伍按钮
	ui::RtwButton*		m_pLookInfo;//密语
	ui::RtwButton*		m_pAddFriend;//加为好友
	ui::RtwButton*		m_pOpenBuff;//打开加成页面
	ui::RtwButton*		m_pBuffIntroduce;//组队加成界面说明	
	ui::RtwButton*		m_pBtnNearClose;//附近玩家界面关闭
	ui::RtwButton*		m_pBtnAgree;//申请列表同意
	ui::RtwButton*		m_pBtnReject;//申请列表拒绝
	ui::RtwButton*		m_pBtnApplyClose;//申请列表界面关闭
	ui::RtwLabel*		m_pTeamBuff;//组队BUFF
	ui::RtwLabel*		m_pNearTeamMember;//组队加成界面附近队友
	ui::RtwLabel*		m_pEXPUp;//组队界面经验加成
	ui::RtwLabel*		m_pIntimatePlayer;//组队界面友好度达到500的队友数量
	ui::RtwLabel*		m_pAttackUp;//组队界面攻击力加成
	ui::RtwLabel*		m_pSelectRect;//成员选中框
	ui::RtwLabel*		m_pOpenApply;//申请列表打开
	ui::RtwLabel*		m_pAnimel[12];//生肖图标
	ui::RtwLabel*		m_pPickModeHint;
	ui::RtwListBox*		m_pNearListBox;//附近玩家列表
	ui::RtwListBox*		m_pApplyListBox;//请求列表
	ui::RtwComboBox*	m_pPickMode;//拾取分配模式
	ui::RtwPopupMenu*	m_pApplyListPopMenu;//请求列表右键菜单
	

	
	TeamMember* TeamMembers[TEAM_MAX_MEMBER];
	

	RtwImage*		m_imgTeamNormal;//正常按钮image
	RtwImage*		m_imgTeamAnimation;//按下按钮image
	bool			m_bShowAnimation;//按钮状态的布尔变量
	int				m_SelectMember;//选中的队员
	//APPLYLISTTYPE m_eApplylistType;//不知道有什么用
	map<string,ApplyIfo*>	m_mapApply;//不知道有什么用
};
