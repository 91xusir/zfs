
#ifndef GAME_TEAM_SHARE_CONST_H
#define GAME_TEAM_SHARE_CONST_H

//�ͻ��˵�Region
const unsigned char C2R_INVITE_OTHER=0;				// �Լ�������˼������[DWORD vOtherRegionID]
const unsigned char C2R_BANISH_OTHER=1;				// �Լ�������˴Ӷ�����[DWORD vOtherDBID]
const unsigned char C2R_SELF_LEAVE=2;				// �Լ��뿪����
const unsigned char C2R_DISMISS=3;					// �Լ���ɢ����
const unsigned char C2R_AGREE_OTHER_INVITE=4;		// �Լ�ͬ����˵�����[DWORD vOtherDBID]
const unsigned char C2R_REFUSE_OTHER_INVITE=5;		// �Լ��ܾ����˵�����[DWORD vOtherDBID]
const unsigned char C2R_BANISH_OTHER_BY_REGION_ID=6;// ���������RegionID[DWORD vOtherRegionID]
const unsigned char C2R_TEAM_ONOFF=7;				// ��ӿ���
const unsigned char C2R_APPLY_TEAM=8;				// �������ĳ������
const unsigned char C2R_AGREE_APPLY=9;				// ͬ���������
const unsigned char C2R_REFUSE_APPLY=10;			// �ܾ���������
const unsigned char C2R_TRANSFER_CAPTAIN=11;		// �ӳ�ת��
const unsigned char C2R_SELECTED_BY_DBID=12;		// ��DBID��ѡ�ж�Ա
const unsigned char C2R_INVITE_OTHER_BY_NAME = 13;
const unsigned char C2R_APPLY_TEAM_BY_NAME   = 14;

const unsigned char C2R_SET_PICK_TYPE = 14;//�ӳ�����ʰȡģʽ

//Region���ͻ���
const unsigned char R2C_ADD_TEAM_MEMBER=0;			//����һ������[DWORD vMemberDBID,char* vpMemberName]
const unsigned char R2C_DELETE_TEAM_MEMBER=1;		//����һ������[DWORD vMemberDBID]
const unsigned char R2C_UPDATE_ONE_MEMBER_DATA=2;	//ˢ��ĳ����Ա������[DWORD vMemberDBID,int vHp,int vMaxHp,int vMp,int vMaxMp]
const unsigned char R2C_UPDATE_ALL_MEMBER_DATA=3;	//ˢ�����г�Ա������[char* vpTeamData]string�д��TeamData��Ϣ
const unsigned char R2C_TEAM_MESSAGE=4;				//ˢһ����Ϣ[int vMessageID]
const unsigned char R2C_TEAM_MESSAGE_WITH_ID=5;		//ˢһ����Ϣ[DWORD vDBIDInMessage,int vMessageID]


//-------����-----------
const unsigned char R2C_OTHER_AGREE_INVITE=6;		//����ͬ��������[DWORD vOtherDBID,char* vpOtherName]
const unsigned char R2C_OTHER_REFUSE_INVITE=7;		//���˾ܾ��������[DWORD vOtherDBID,char* vpOtherName]
const unsigned char R2C_OTHER_INVITE=8;				//���������Լ��������[DWORD vOtherDBID,char* vpOtherName]
const unsigned char R2C_OTHER_BANISH=9;				//���������Լ�������[DWORD vOtherDBID]
const unsigned char R2C_OTHER_LEAVE=10;				//�����뿪����[DWORD vOtherDBID]
const unsigned char R2C_OTHER_DISMISS=11;			//���˽�ɢ����[DWORD vOtherDBID]
const unsigned char R2C_OTHER_APPLY=12;				//������������Լ�����
const unsigned char R2C_OTHER_AGREE_APPLY=13;		//����ͬ���Լ�����
const unsigned char R2C_OTHER_REFUSE_APPLY=14;		//���˾ܾ��Լ�����
const unsigned char R2C_TRANSFER_CAPTAIN = 15;		//�ӳ�ת��
const unsigned char R2C_SELECTED_BY_DBID=16;		// ��DBID��ѡ�ж�Ա

const unsigned char R2C_SET_PICK_TYPE = 17;//�ӳ�����ʰȡģʽ


//Region��GameWorld[����ǰ���DWORD vFromID]
const unsigned char R2GW_INVITE_OTHER=0;			//�Լ�������˼������[DWORD vOtherDBID]
const unsigned char R2GW_BANISH_OTHER=1;			//�Լ�������˴Ӷ�����[DWORD vOtherDBID]
const unsigned char R2GW_SELF_LEAVE=2;				//�Լ��뿪����
const unsigned char R2GW_DISMISS=3;					//�Լ���ɢ����
const unsigned char R2GW_AGREE_OTHER_INVITE=4;		//�Լ�ͬ����˵�����[DWORD vOtherDBID]
const unsigned char R2GW_REFUSE_OTHER_INVITE=5;		//�Լ��ܾ����˵�����[DWORD vOtherDBID]
const unsigned char R2GW_DATE_CHANGE = 6;				//�Լ������ݱ仯Ӱ�쵽����������Ա[int vHp,int vMaxHp,int vMp,int vMaxMp]
const unsigned char R2GW_TEAM_ONOFF = 7;
const unsigned char R2GW_APPLY_ADDTEAM = 8;
const unsigned char R2GW_OTHER_AGREE_APPLY =9;
const unsigned char R2GW_REFUSE_TEAM=10;
const unsigned char R2GW_TRANSFER_CAPTAIN = 11;		//�ӳ�ת��
const unsigned char R2GW_INVITE_OTHER_BY_NAME = 12;
const unsigned char R2GW_INVITE_OTHER_BY_DBID = 13;
const unsigned char R2GW_SETREFUSE_TEAM = 16;  //���þܾ������Ϣ

const unsigned char R2GW_SET_PICK_TYPE = 14;//�ӳ�����ʰȡģʽ
const unsigned char R2GW_TEAM_PICKER = 15;//���ʱ֪ͨ����ʰ�˶�����


//GameWorld��Region[����ǰ���DWORD vSendToDBID]
const unsigned char GW2R_ADD_TEAM_MEMBER=0;			//����һ������[DWORD vMemberDBID,char* vpMemberName]
const unsigned char GW2R_DELETE_TEAM_MEMBER=1;		//����һ������[DWORD vMemberDBID]
const unsigned char GW2R_UPDATE_ONE_MEMBER_DATA=2;	//ˢ��ĳ����Ա������[DWORD vMemberDBID,int vHp,int vMaxHp,int vMp,int vMaxMp]
const unsigned char GW2R_UPDATE_ALL_MEMBER_DATA=3;	//ˢ�����г�Ա������[char* vpTeamData]string�д��TeamData��Ϣ
const unsigned char GW2R_REQUEST_MEMBER_DATA=4;		//Ҫ��regionˢ���Լ������ݵ�GameWorld[��]
const unsigned char GW2R_TEAM_MESSAGE=5;				//ˢһ����Ϣ[int vMessageID]
const unsigned char GW2R_TEAM_MESSAGE_WITH_ID=6;		//ˢһ����Ϣ[��ID][DWORD vDBIDInMessage,int vMessageID]

//-------����-----------
const unsigned char GW2R_OTHER_AGREE_INVITE=7;		//����ͬ��������[DWORD vOtherDBID,char* vpOtherName]
const unsigned char GW2R_OTHER_REFUSE_INVITE=8;		//���˾ܾ��������[DWORD vOtherDBID,char* vpOtherName]
const unsigned char GW2R_OTHER_INVITE=9;				//���������Լ��������[DWORD vOtherDBID,char* vpOtherName]
const unsigned char GW2R_OTHER_BANISH=10;			//���������Լ�������[DWORD vOtherDBID]
const unsigned char GW2R_OTHER_LEAVE=11;				//�����뿪����[DWORD vOtherDBID]
const unsigned char GW2R_OTHER_DISMISS=12;			//���˽�ɢ����[DWORD vOtherDBID]

const unsigned char GW2R_APPLY_ADDTEAM = 13;
const unsigned char GW2R_REFUSE_TEAM=14;
const unsigned char GW2R_TRANSFER_CAPTAIN = 15;		//�ӳ�ת��
const unsigned char GW2R_INVITE_OTHER_BY_NAME = 16;		//�ӳ�ת��

const unsigned char GW2R_SET_PICK_TYPE = 17;//�ӳ�����ʰȡģʽ

const unsigned char GW2R_TEAM_PICKER = 18;//���ʱ֪ͨ����ʰ�˶�����
const unsigned char GW2R_SETREFUSE_TEAM =19/*6*/; //���������˾ܾ����

const unsigned char GW2R_TO_WHENFRIEND = 20; 
/*
//Region��Region
//-----------��������---------------
const unsigned char R2R_ADD_TEAM_MEMBER=0;			//����һ������
const unsigned char R2R_DELETE_TEAM_MEMBER=1;		//����һ������
const unsigned char R2R_UPDATE_ONE_MEMBER_DATA=2;	//ˢ��ĳ����Ա������
const unsigned char R2R_TEAM_MESSAGE=3;				//ˢһ����Ϣ
const unsigned char R2R_TEAM_MESSAGE_WITH_ID=4;		//ˢһ����Ϣ[��ID]

//-------����-----------
const unsigned char R2R_SELF_ADD_OTHER_SUCCEED=5;	//�Լ������˼��뵽�Լ�����ɹ�
const unsigned char R2R_SELF_ADD_OTHER_FAULT=6;		//�Լ������˼��뵽�Լ�����ʧ��
const unsigned char R2R_SELF_AGREE_INVITE=7;			//�Լ�ͬ�������˵Ķ���
const unsigned char R2R_SELF_REFUSE_INVITE=8;		//�Լ��ܾ�������˵Ķ���
const unsigned char R2R_SELF_INVITE=9;				//�Լ�������˼������
const unsigned char R2R_SELF_BANISH=10;				//�Լ�������˳�����
const unsigned char R2R_SELF_LEAVE=11;				//�Լ��뿪����
const unsigned char R2R_SELF_DISMISS=12;				//�Լ���ɢ����


const unsigned char R2R_OTHER_REGION_ID_NOT_EXIST=13;//���˵�RegionID������
const unsigned char R2R_OTHER_DB_ID_NOT_EXIST=14;	//���˵�DBID������
const unsigned char R2R_FORCE_UPDATE_DATA=15;		//ǿ��һ������ˢ���Լ������ݵ���һ������

const unsigned char R2R_OTHER_ADD_SELF_SUCCEED=R2R_SELF_ADD_OTHER_SUCCEED;	//���˽���ɹ��������
const unsigned char R2R_OTHER_ADD_SELF_FAULT=R2R_SELF_ADD_OTHER_FAULT;		//���˽������ʧ��
const unsigned char R2R_OTHER_AGREE_INVITE=R2R_SELF_AGREE_INVITE;			//����ͬ��������
const unsigned char R2R_OTHER_REFUSE_INVITE=R2R_SELF_REFUSE_INVITE;			//���˾ܾ��������
const unsigned char R2R_OTHER_INVITE=R2R_SELF_INVITE;						//���������Լ��������
const unsigned char R2R_OTHER_BANISH=R2R_SELF_BANISH;						//���������Լ�������
const unsigned char R2R_OTHER_LEAVE=R2R_SELF_LEAVE;							//�����뿪����
const unsigned char R2R_OTHER_DISMISS=R2R_SELF_DISMISS;						//���˽�ɢ����
*/




//������Ϣ�ٹ���һ��
//����������һ�ι���
const int NULL_MESSAGE=0;
const int TEAM_COUNT_OVER=1;
const int NOT_TEAM_CAPTAIN=2;
const int TEAM_NOT_EXIST=3;
const int BANISH_MEMBER=4;
const int MEMBER_NOT_EXIST=5;
const int SELF_LEFT_TEAM=6;
const int TEAM_DISMISS=7;
const int MEMBER_ADD_TEAM=8;
const int REFUSE_INVITE=9;
const int INVITE_ADD_TEAM=10;

const int BANISH_FROM_TEAM=11;
const int LEAVE_TEAM=12;
const int ALREADY_IN_TEAM=13;
const int CHOOSE_MEMBER_ADD_TEAM=14;
const int CHOOSE_MEMBER_BANISH_FROM_TEAM=15;
const int ADD_TEAM_SUCCEED=16; 
const int THAT_PLAY_ALREADY_ADD_OTHER_TEAM=17;
const int NOT_MEMBER_TEAM_DISMISS=18;
const int THAT_OBJECT_NOT_PLAYER=19;
const int THAT_PLAYER_NOT_INLINE=20;

const int TEAM_LOCAL_NOT_THIS_DBID_NAME=21;
const int TEAM_WAIT_PLAYER_RESPOND=22;
const int TEAM_OBJECT_NOT_IN_REGION=23;						//���岻�ڸ�����
const int THAT_PLAYER_TEAM_OFF=24;							//��ҹر���ӿ��أ�


/*

//------�Է����--------
//������Ϣ�������
struct SR2rData
{
	void BeginSend(){mOperation=0;mDBIDInMessage=0;mFromDBID=0;mpFromName=NULL;
	mParameter=0;mHp=0;mMaxHp=0;mMp=0;mMaxMp=0;mpName=NULL;}

	unsigned char	mOperation;
	DWORD	mDBIDInMessage;
	DWORD	mFromDBID;
	char*	mpFromName;

	DWORD	mParameter;
	int		mHp;
	int		mMaxHp;
	int		mMp;
	int		mMaxMp;
	char*	mpName;
};

*/


#endif//GAME_TEAM_SHARE_CONST_H
