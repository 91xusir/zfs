
#ifndef GC_CHAT_CONST_H 
#define GC_CHAT_CONST_H

typedef unsigned long       DWORD;

const int EACH_FORFEND_TIME				=5*1000;							//ÿ��Υ�����ӵĽ���ʱ��[����]
const int CHECK_OFFEND_TIME				=5*1000;							//���Υ���ʱ��[����]
const int CHAT_MESSAGE_MAX_COUNT		=1024;								//������������[Ӣ��]

const int CHECK_TIME					=2*1000;							//���ˢ����ʱ��[����]
const int SPEAKER_CHECK_TIME			=30*1000;							//���ȵ�ʹ�ü�϶�����롿
const int TIME_BUFFER					=6;									//���������Ƶ������һ�η���ʱ��

const int COUNTY_CHECK_TIME				=30*1000;							//��ս���ˢ����ʱ��[����]
const int COUNTY_LIMIT_COUNT			=5;									//��ս�涨ʱ���ڵ����ƴ���
const int COUNTY_TIME_BUFFER			=COUNTY_LIMIT_COUNT-1;				//�����սˢ��ʱ��Ļ���

const DWORD SYSTEM_INFO_SCROLL_TIME		=300;								//ϵͳ��Ϣ����ʱ��[��λ������]
const float SYSTEM_INFO_CLEAR_TIME		=100.0f;							//ϵͳ�����Ϣʱ����[��λ����] // �˴���Ϊ��̬ˢ��������ʾ

const int MAX_HEIGHT_CHANGE_SIZE		=240;								//��������ɱ�߶�
const int MAX_HEIGHT_STATE				=5;									//��������ı䵵��//����Ҫ������
const int EACH_HEIGHT					=MAX_HEIGHT_CHANGE_SIZE/MAX_HEIGHT_STATE;	//ÿ���ĸı�߶�ֵ



//const char* LIMIT_MESSAGE;//="��˵��̫���ˣ����Ժ�";

const char UI_CHANNEL_WHOLE_ID[]		="fmchannel";						//��ѡ����ȫ��
const char UI_CHANNEL_ID[]				="fmchannel";						//��ѡ���


const char UI_ALL_LIST_WHOLE_ID[]		="fmdialg.channelall";				//����Ƶ���б�
const char UI_ALL_LIST_ID[]				="channelall";						//����Ƶ���б�
const char UI_TEAM_LIST_WHOLE_ID[]		="fmdialg.channelteam";				//����Ƶ���б�
const char UI_TEAM_LIST_ID[]			="channelteam";						//����Ƶ���б�
const char UI_GUILD_LIST_WHOLE_ID[]		="fmdialg.channelguild";			//���Ƶ���б�
const char UI_GUILD_LIST_ID[]			="channelguild";					//���Ƶ���б�
const char UI_COUNTRY_LIST_WHOLE_ID[]	="fmdialg.channelcountry";			//����Ƶ���б�
const char UI_COUNTRY_LIST_ID[]			="channelcountry";					//����Ƶ���б�


const char UI_FEEDBACK_SWITCH_WHOLE_ID[] 
										= "fmdialg.btnFeedbackclose";		//�ı�Ի�������Ŀ���
const char UI_FEEDBACK_SWITCH_ID[]		= "btnFeedbackclose";				//�ı�Ի�������Ŀ���


const char UI_CHAT_FACE_SWITCH_WHOLE_ID[]="fmdialg.btnchatface";			//��������Ŀ���
const char UI_CHAT_FACE_SWITCH_ID[]		="btnchatface";						//��������Ŀ���

const char UI_CHANNEL_BUTTON_ID[]		="btnchanel";						//��ѡ��򿪹رյİ�ť
const char UI_CHANNEL_BUTTON_WHOLE_ID[]	="fmdialg.btnchanel";				//��ѡ��򿪹رյİ�ť��ȫ��
const char UI_DOSKEY_LIST_WHOLE_ID[]	="listchater";						//Doskey��list��ȫ��
const char UI_INPUT_WHOLE_ID[]			="fmdialg.fmtxtdialog.txtdialog";	//������ȫ��

const char UI_SYSTEM_FORM_ALL_WHOLE_ID[]="panSysteminfo";					//ϵͳ��Ϣ���ȫ��
const char UI_SYSTEM_FORM_BOX_WHOLE_ID[]="panSysteminfo.txtFeedback";		//ϵͳ��Ϣ������ֲ��ֵ�ȫ��

const char UI_CHAT_FACE_FORM_WHOLE_ID[]	="fmchatface";						//��������
const char UI_CHAT_FACE_FORM_ID[]		="fmchatface";						//��������
const char UI_CHAT_FACE_ICON_LIST_WHOLE_ID[]="fmchatface.iconlist";			//���������е�ͼ���б��
const char UI_CHAT_FACE_ICON_LIST_ID[]	="iconlist";						//���������е�ͼ���б��



//����Ƶ����ѡ���UI��ʶ
const char UI_PERSONAL_CHANNEL_WHOLE_ID[]="layworld.fmchannel.ckpersoncl";
const char UI_TEAM_CHANNEL_WHOLE_ID[]	="layworld.fmchannel.ckgroupcl";
const char UI_GUILD_CHANNEL_WHOLE_ID[]	="layworld.fmchannel.ckguildcl";
const char UI_NORMAL_CHANNEL_WHOLE_ID[]	="layworld.fmchannel.cknormalcl";
const char UI_PUBLIC_CHANNEL_WHOLE_ID[]	="layworld.fmchannel.ckallcl";
const char UI_COUNTRY_CHANNEL_WHOLE_ID[]="layworld.fmchannel.ckcountrycl";
const char UI_SYSTEM_CHANNEL_ID[]		="";

//����Ƶ������ʱ��ǰ׺������
/*
const char* PERSONAL_CHANNEL_DESCRIPTION;//[]="[����]";
const char* TEAM_CHANNEL_DESCRIPTION;//[]="[����]";
const char* GUILD_CHANNEL_DESCRIPTION;//[]="[���]";
const char*	NORMAL_CHANNEL_DESCRIPTION;//[]="";
const char* PUBLIC_CHANNEL_DESCRIPTION;//[]="[����]";
const char* COUNTRY_CHANNEL_DESCRIPTION;//[]="[����]";
const char* SYSTEM_CHANNEL_DESCRIPTION;//[]="[ϵͳ]";
*/

//����Ƶ������ʱ����ɫ
const DWORD PERSONAL_CHANNEL_COLOR		=0xffDDA0DD;		//˽��Ƶ��
const DWORD TEAM_CHANNEL_COLOR			=0xff90EE90;		//����Ƶ��
const DWORD UNION_CHANNEL_COLOR			=0xffEEEE00;		//����Ƶ��
const DWORD BATTLE_CHANNEL_COLOR		=0xffCD3700;		//ս��Ƶ��
const DWORD NEAR_CHANNEL_COLOR			=0xffD1EEEE;		//����Ƶ��
const DWORD WORLD_CHANNEL_COLOR			=0xFF00E5EE;		//����Ƶ��
const DWORD SYSTEM_CHANNEL_COLOR		=0xFFEEC900;		//ϵͳƵ��

//����Ƶ����MASK[���ڽ���]
const int PERSONAL_CHANNEL_MASK			=0x01;				//˽��Ƶ��
const int TEAM_CHANNEL_MASK				=0x02;				//����Ƶ��
const int UNION_CHANNEL_MASK			=0x04;				//����Ƶ��
const int BATTLE_CHANNEL_MASK			=0x08;				//ս��Ƶ��
const int NEAR_CHANNEL_MASK				=0x10;				//����Ƶ��
const int WORLD_CHANNEL_MASK			=0x20;				//����Ƶ��
const int BATTLEINFO_CHANNEL_MASK		=0x40;				//ս����Ϣ
const int SYSTEM_CHANNEL_MASK			=0x80;				//ϵͳƵ��
const int SPEAKER_CHANNEL_MASK			=0x100;				//����Ƶ��  heten

//����Ƶ����ǰ׺
const char SPACE_FLAG1[]				=" ";				//�ո�
const char SPACE_FLAG2[]				="��";				//�ո�

const char PERSONAL_CHANNEL_FLAG1[]		="/";				//����Ƶ��
const char PERSONAL_CHANNEL_FLAG2[]		="��";				//����Ƶ��

const char TEAM_CHANNEL_FLAG1[]			="#";				//����Ƶ��
const char TEAM_CHANNEL_FLAG2[]			="��";				//����Ƶ��

const char UNION_CHANNEL_FLAG1[]		="@";				//����Ƶ��
const char UNION_CHANNEL_FLAG2[]		="��";				//����Ƶ��
const char UNION_CHANNEL_FLAG3[]		="��";				//����Ƶ��

const char BATTLE_CHANNEL_FLAG1[]		="$";				//ս��Ƶ��
const char BATTLE_CHANNEL_FLAG2[]		="��";				//ս��Ƶ��
const char BATTLE_CHANNEL_FLAG3[]		="��";				//ս��Ƶ��

const char NORMAL_CHANNEL_FLAG1[]		="";				//һ��Ƶ��

const char WORLD_CHANNEL_FLAG1[]		="!";				//����Ƶ��
const char WORLD_CHANNEL_FLAG2[]		="��";				//����Ƶ��

const char SYSTEM_CHANNEL_FLAG1[]		="";				//ϵͳƵ��


#endif//GC_CHAT_CONST_H
