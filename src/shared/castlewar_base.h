//-----------------------------------------------------------------------------
//created by tony 05.06.01
//-----------------------------------------------------------------------------
#ifndef CASTLEWAR_BASE_H
#define CASTLEWAR_BASE_H

#include <time.h>
#include <string>

using namespace std;

typedef time_t		TIME;
typedef struct tm	ST_TIME;

//--------------------------------------------
//���޸�...
//--------------------------------------------
#define ITEM_FLAG_ID			570		//�������ID
#define FLAG_RANGE				200

#define FLAG_NPC_ID_SHANG		719
#define FLAG_NPC_ID_ZHOU		725

#define XIQI_ANIMAL_NPC_ID		723
#define ZHAOGE_ANIMAL_NPC_ID	722
#define CHUANYUN_ANIMAL_NPC_ID	724

//op mode
const BYTE ADD			= 0;
const BYTE MINUS		= 1;
const BYTE ALLOWED		= 2;
const BYTE NOTALLOWED	= 3;

//------------------------------------
//����״̬(�޸�ֻ�������׷��)
//------------------------------------
enum EState
{
	NPC_OCCUPY_STATE			= 0, //NPCռ��	����ע�ᣩ
	UNION_OCCUPY_STATE			= 1, //���ռ��	����ע�ᣩ
	WAR_PREPARE_STATE			= 2, //��ս׼��	����ս��ʼǰ5���ӣ�������/����ע�ᣩ
	WARFARE_STATE				= 3, //��ս��	������/����ע�ᣩ
	WAR_FIVE_MIN_PAUSE_STATE	= 4	 //��ǰ����ս���������׼���������� ����ע�ᣩ
};

struct STAttackerInfo
{
	DWORD	union_id;		//����id
	string	union_name;		//��������
	string	vassal_name;	//�������
	DWORD	flag_id;		//��������id��Ӧ���촴�����ʱm_oId
							/* 0x0000: δ��������
							 * 0xffff: �Ѵ����������
							 * ����ֵ: �Ѵ���δ�����
							 */
	
	STAttackerInfo()
	{ 
		union_id	= 0;
		union_name	= "";
		vassal_name	= "";
		flag_id		= 0;
	}

	bool Serialize(CG_CmdPacket *cmd);
};

struct STDefenderInfo
{
	DWORD	union_id;		//����id
	string	union_name;		//��������
	string	vassal_name;	//�������
	bool	is_allowed;		//�Ƿ��س������׼
	DWORD	flag_id;		//��������id��Ӧ���촴�����ʱm_oId
							/* 0x0000: δ��������
							 * 0xffff: �Ѵ����������
							 * ����ֵ: �Ѵ���δ�����
							 */

	STDefenderInfo()
	{ 
		union_id	= 0; 
		union_name	= "";
		vassal_name	= "";
		flag_id		= 0;
		is_allowed	= false; 
	}

	bool Serialize(CG_CmdPacket *cmd);
};

struct STWarInfo
{
	string					feod_name;				//��������
	DWORD					master_union_id;		//��������id
	string					master_union_name;		//������������
	string					master_union_logo;		//����������־
	string					vassal_name;			//�����������
	DWORD					flag_id;				//��������id��ͬ�ϣ�
	EState					state;					//����Ŀǰ״̬
	DWORD					start_time;				//�´γ�ս��ʼʱ��
	DWORD					end_time;				//�´γ�ս����ʱ��
	DWORD					last_end_time;			//�ϴγ�ս����ʱ��
	bool					guard_animal_is_dead;	//�����ػ����Ƿ�����
	vector<STAttackerInfo>	v_attacker_union_info;	//���Ƿ���Ϣ�б�
	vector<STDefenderInfo>	v_defender_union_info;	//���ط���Ϣ�б�(����δ����׼��)
	
	STWarInfo() :
	feod_name(""), master_union_name(""), master_union_logo(""), vassal_name("")
	{
		master_union_id	= 0;
		state			= NPC_OCCUPY_STATE;
		start_time		= 0;
		end_time		= 0;
		flag_id			= 0;
		last_end_time	= 0;
		guard_animal_is_dead	= false;

		v_attacker_union_info.clear();
		v_defender_union_info.clear();
	}
	
	bool Serialize(CG_CmdPacket *cmd);

	const STWarInfo& operator=(const STWarInfo& rhs);
};

//-------------------------------------
//���в�����Ϣ�������ݿⱣ��һ�£�
//-------------------------------------
//const int FEOD_COUNT = 3;
//const string FeodNameInDB[FEOD_COUNT] = { "XiQi", "ZhaoGe", "ChuanYun" };
//char* CastleWarAreaName[3] = {"��᪳�","�����","���ƹ�"};


#endif //CASTLEWAR_BASE_H

