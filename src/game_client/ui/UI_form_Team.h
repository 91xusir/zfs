#pragma once
#include "ui/rtw_ui.h"
#include "region_client.h"
#include <string>
#include <map>

using std::map;
#define TEAM_MAX_MEMBER 5
//���������ؽṹ�壬��¼�����Ϣ
struct ApplyIfo
{
	long ID;				// ����ID
	string name;			// ����
	int level;				// �ȼ�
	string profession;		// ְҵ
	int animal;				// ��Ф
	int power;				// ս������
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
//����UI   ���� 2010.06.01
class CUI_form_Team
{
public:
	CUI_form_Team(void);
	~CUI_form_Team(void);
	/* �ͻ�����غ��� */
	void Init();//��ʼ������
	void Show();//��ʾ
	void Hide();//����
	void OnRefresh();//����
	void OnShowTeammember();//��ʾ�����Ա
	void OnShowAnimalBuff();//��ӽ���BUFF��ʾ
	void OnBtnTransfer(ui::RtwWidget* vpSender, RtwEventDelegate* vpEvent);//ת�öӳ�
	void OnBtnGetaway(ui::RtwWidget* vpSender, RtwEventDelegate* vpEvent);//�뿪����
	void OnBtnKick(ui::RtwWidget* vpSender, RtwEventDelegate* vpEvent);//�����Ա
	void OnBtnDisband(ui::RtwWidget* vpSender, RtwEventDelegate* vpEvent);//��ɢ����
    void OnBtnDownRefresh(ui::RtwWidget* vpSender, RtwEventDelegate* vpEvent);//���°�ťʱˢ��
	void OnBtnShowBuff(ui::RtwWidget* vpSender, RtwEventDelegate* vpEvent);//����Ӽӳɽ���
	void OnBtnLookInfo(ui::RtwWidget* vpSender, RtwEventDelegate* vpEvent);//�鿴��Ϣ
	void OnBtnAddFriend(ui::RtwWidget* vpSender, RtwEventDelegate* vpEvent);//��Ӻ���
	void OnBtnLookIntroduce(ui::RtwWidget* vpSender, RtwEventDelegate* vpEvent);//���BUFF˵��	
	void OnSelectMember(ui::RtwWidget* vpSender, RtwEventDelegate* vpEvent);//ѡ���Ա
	void OnCloseForm(void* ,void*);//�ر�
	void OnChangePickMode(CUiCheckButton* sender,void*);//�ı�ʰȡ����
	void OnGetPickModeHide(CUiCheckButton* sender,void*);//���ʰȡ����˵��
	void SetPickMode(byte vbType){if(m_pPickMode) m_pPickMode->SetSelectedItem(vbType);}//����ʰȡ����
	void OnApplyListInsertNew(long id,string na,int lev,string prf,int ani,int pow);//�����б�����µ�
	void OnApplyListHint(ui::RtwWidget* vpSender, RtwEventDelegate* vpEvent);//�����б� ��ʾ
	void OnApplyListBtnAgree(ui::RtwWidget* vpSender, RtwEventDelegate* vpEvent);//�����б� ͬ��
	void OnApplyListBtnDisagree(ui::RtwWidget* vpSender, RtwEventDelegate* vpEvent);//�����б� �ܾ�
	void OnApplyListBtnClear(ui::RtwWidget* vpSender, RtwEventDelegate* vpEvent);//�����б� �ر�
	void OnApplyListRDMenuSelect(RtwWidget* vpSender, RtwEventDelegate* vpEvent);//�����б��Ҽ��˵�ѡ��
	void OnApplyListPopUpMenu(ui::RtwWidget* vpSender, RtwEventDelegate* vpEvent);//�����б��Ҽ��˵� 
	void OnApplyListRDMenuUnFocus(ui::RtwWidget* vpSender, RtwEventDelegate* vpEvent);//�Ҽ��˵�����
	void OnBtnShowApply(ui::RtwWidget* vpSender, RtwEventDelegate* vpEvent);//��ʾ�����б�
	void SetTeamButtonNormal();//���ð�ťΪ����״̬
	void SetTeamButtonAnimation();//���ð�ťΪ����״̬
	bool GetTeamImage(short ModelID,int HeadImageID, vector<SCharImage>& models,ui::RtwImage** HeadImage,short Profession,ui::RtwImage** ProfessionImage,short Animal,ui::RtwImage** AnimalImage);//��ö�Ա��ӦͼƬ
	bool IsVisibale(){return m_pTeam->IsVisible();}//�Ƿ����
	/* ��Ϣ��غ��� */
	void OnReceiveApply();//�յ�ͬ��
	void OnPreDraw();//����
private:
	ui::RtwWidget*		m_pTeam;//��ӽ���ָ��
	ui::RtwWidget*		m_pBuff;//��Ӽӳɽ���ָ��
	ui::RtwWidget*		m_pNearPlayer;//������ҽ���ָ��
	ui::RtwWidget*		m_pApply;//�����б����ָ��
	ui::RtwButton*		m_pCloseForm;//�ر���Ӵ���
	ui::RtwButton*		m_pTransFer;//ת�öӳ���ť
	ui::RtwButton*		m_pKick;//�����Ա��ť
	ui::RtwButton*		m_pDisband;//��ɢ���鰴ť
	ui::RtwButton*		m_pRequest;//�뿪���鰴ť
	ui::RtwButton*		m_pLookInfo;//����
	ui::RtwButton*		m_pAddFriend;//��Ϊ����
	ui::RtwButton*		m_pOpenBuff;//�򿪼ӳ�ҳ��
	ui::RtwButton*		m_pBuffIntroduce;//��Ӽӳɽ���˵��	
	ui::RtwButton*		m_pBtnNearClose;//������ҽ���ر�
	ui::RtwButton*		m_pBtnAgree;//�����б�ͬ��
	ui::RtwButton*		m_pBtnReject;//�����б�ܾ�
	ui::RtwButton*		m_pBtnApplyClose;//�����б����ر�
	ui::RtwLabel*		m_pTeamBuff;//���BUFF
	ui::RtwLabel*		m_pNearTeamMember;//��Ӽӳɽ��渽������
	ui::RtwLabel*		m_pEXPUp;//��ӽ��澭��ӳ�
	ui::RtwLabel*		m_pIntimatePlayer;//��ӽ����Ѻöȴﵽ500�Ķ�������
	ui::RtwLabel*		m_pAttackUp;//��ӽ��湥�����ӳ�
	ui::RtwLabel*		m_pSelectRect;//��Աѡ�п�
	ui::RtwLabel*		m_pOpenApply;//�����б��
	ui::RtwLabel*		m_pAnimel[12];//��Фͼ��
	ui::RtwLabel*		m_pPickModeHint;
	ui::RtwListBox*		m_pNearListBox;//��������б�
	ui::RtwListBox*		m_pApplyListBox;//�����б�
	ui::RtwComboBox*	m_pPickMode;//ʰȡ����ģʽ
	ui::RtwPopupMenu*	m_pApplyListPopMenu;//�����б��Ҽ��˵�
	

	
	TeamMember* TeamMembers[TEAM_MAX_MEMBER];
	

	RtwImage*		m_imgTeamNormal;//������ťimage
	RtwImage*		m_imgTeamAnimation;//���°�ťimage
	bool			m_bShowAnimation;//��ť״̬�Ĳ�������
	int				m_SelectMember;//ѡ�еĶ�Ա
	//APPLYLISTTYPE m_eApplylistType;//��֪����ʲô��
	map<string,ApplyIfo*>	m_mapApply;//��֪����ʲô��
};
