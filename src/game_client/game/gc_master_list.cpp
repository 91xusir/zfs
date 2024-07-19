

#include "gc_include.h"
#include "gc_master_list.h"
#include "ui_form_msg.h"
#include "game_string_assistant.h"
#include "multi_sort.h"




void CGCMasterList::UIClick(ui::RtwWidget* vpSender, RtwEventDelegate* )
{
	for(int i=0;i!=MASTER_LIST_COLUMN_MAX_COUNT;++i)
	{
		if(vpSender==mpUIList->GetColumnHeader(i))
		{//�����ĳ��
			mMaxToMin[i]=!mMaxToMin[i];//�任
			mSortMainIndex=i;//����������
		}
	}

	UpdateUI();

	return;
}


void CGCMasterList::UIDbClick(ui::RtwWidget* vpSender, RtwEventDelegate* )
{
	int Index=mpUIList->GetSelectedItem();
	string Name(mpUIList->GetItemText(Index,0));

	GetWorld()->m_Chat.OnMemberChoice(0,Name.c_str());//˽��
}

void CGCMasterList::Init()
{
	//�����б��ָ��
	mpUIList=RTW_WIDGET_T(RtwListBox,"fmMaster.listmail");
	mpUIList->ModifyFlags(wfDClick,0);
	//mpUIList->SetColumnFormat(RtwStringFormat(alignCenter,alignCenter));	// ע�� [3/17/2009 tooth.shi]
	mpUIList->EvLDClick+=RTW_CALLBACK(this,CGCMasterList,UIDbClick);
	

	for(int i =0 ;i != MASTER_LIST_COLUMN_MAX_COUNT ;++i)
	{
//		mpUIList->GetColumnHeader(i)->EvLClick += RTW_CALLBACK(this,CGCMasterList,UIClick);tooth.shi
	}
	
	mpUI=RTW_WIDGET("fmMaster");
	mpUI->Hide();

	//mpUI->EvKey += RTW_CALLBACK(mpUI,RtwWidget,DefaultKeyDown);
}


UIFormMsg* CGCMasterList::SysMessage(const string& vrString)
{
	UIFormMsg* pFrm = UIFormMsg::ShowStatic(vrString.c_str(), UIFormMsg::TYPE_OK,true,"master_list");
	return pFrm;
}


void CGCMasterList::ApplyOk(ui::RtwWidget* vpSender, RtwEventDelegate* )
{//Ҫ�ۿ�
	CG_CmdPacket* pPacket = NetBeginWrite();
	pPacket->WriteShort(c2r_master_list);
	pPacket->WriteByte(C2R_APPLY_VIEW_LIST);
	NetSend(pPacket);
}


void CGCMasterList::ReceiveFromServer(CG_CmdPacket *pPacket)
{
	byte Operation;
	READ_byte(Operation);

	switch(Operation)
	{
	case R2C_APPLY_SUCCESS:
		//ϵͳ��Ϣ//����ɹ�
		{
			UIFormMsg *pFrm = SysMessage(R(MSG_MASTER_LIST_APPLY_SUCCESS));
			pFrm->EvOK += RTW_CALLBACK(this,CGCMasterList,ApplyOk);
		}
		break;
	case R2C_APPLY_FAIL:
		//ϵͳ��Ϣ//����ʧ��
		{
			byte Code=0;

			READ_byte(Code);
			switch(Code)
			{
			case FAIL_LIST_OVERFLOW:			//�б���
				SysMessage(R(MSG_MASTER_LIST_FAIL_LIST_OVERFLOW));
				break;
			case FAIL_LEVEL_LOW:				//�ȼ�̫��
				SysMessage(R(MSG_MASTER_LIST_FAIL_LEVEL_LOW));
				break;
			case FAIL_STUDENT_OVERFLOW:			//��ǰѧ��̫��
				SysMessage(R(MSG_MASTER_LIST_FAIL_STUDENT_OVERFLOW));
				break;
			case FAIL_MASTER_ALREADY_EXIST:		//��ʦ���Ѿ������б���
				SysMessage(R(MSG_MASTER_LIST_FAIL_MASTER_ALREADY_EXIST));
				break;
			default:
				break;
			}
		}

		break;
	case R2C_LIST_INFO:
		{
			*pPacket >> mList;
			UpdateUI();
		}
		break;
	default:
		break;
	}

	return;
}

/*
//---------------------------------------------------
template<class S1,class S2,class Key=CMasterSort>
class COperation
{
public:
	COperation(bool vK1First)
		:mS1(),mS2(),mK1First(vK1First)
	{
	};

	bool operator()( Key vLeft, Key vRight) const 
	{
		if(mK1First)
		{
			if(mS1(vLeft.mK1,vRight.mK1))
			{
				return true;
			}
			else if(vLeft.mK1==vRight.mK1)
			{
				return mS2(vLeft.mK2,vRight.mK2);
			}
			else
			{
				return false;
			}
		}
		else
		{
			if(mS2(vLeft.mK2,vRight.mK2))
			{
				return true;
			}
			else if(vLeft.mK2==vRight.mK2)
			{
				return mS1(vLeft.mK1,vRight.mK1);
			}
			else
			{
				return false;
			}
		}
	}

	S1		mS1;
	S2		mS2;
	bool	mK1First;
};

//���༶������
class CMasterSort
{
public:

	CMasterSort(const int vK1,const int vK2,SMasterInfo* vp)
		:mK1(vK1),mK2(vK2),mp(vp)
	{
	};

	int				mK1;
	int				mK2;
	SMasterInfo*	mp;			
};

*/

void CGCMasterList::UpdateUI()
{

	mpUIList->RemoveAllItems();	//�������б�UI

	//����ר��������б���
	list<CKey<SMasterInfo> > FixList;

	{
		list<SMasterInfo>::iterator It;
		for(It=mList.mList.begin();It!=mList.mList.end();++It)
		{
			CKey<SMasterInfo> Temp(&(*It));
			Temp += &(It->mName);
			Temp += &(It->mMetier);
			Temp += &(It->mLevel);
			Temp += &(It->mPrenticeAmount);
			FixList.push_back(Temp);
		}
	}

	//�������
	CMultiSort			Sort;

	//�������������
	for(int i=0;i!=MASTER_LIST_COLUMN_MAX_COUNT;++i)
	{
		if(i==0 || i==1)//string
		{
			mMaxToMin[i] ? (Sort += CGreater<string>()) : (Sort += CLess<string>() );//����������ʽ
		}
		else if(i==2 || i==3)//short int
		{
			mMaxToMin[i] ? ( Sort += CGreater<short int>()) :( Sort += CLess<short int>() );//����������ʽ
		}
	}

	//��������
	Sort.SetMainIndex(mSortMainIndex);
	
	//����
	FixList.sort( Sort );


	//���źõ�list���뵽UIList
	int Index;
	string Level;
	string PrenticeSize;

	list<CKey<SMasterInfo> >::iterator It2;
	for(It2=FixList.begin();It2!=FixList.end();++It2)
	{//���е�ʦ��
		Level.clear();
		PrenticeSize.clear();

		Level= fox::Convert<string>(It2->mpT->mLevel)+R(MSG_MASTER_LIST_LEVEL_FLAG);
		PrenticeSize= fox::Convert<string>(It2->mpT->mPrenticeAmount)+R(MSG_MASTER_LIST_COUNT_FLAG);

		Index=mpUIList->AddItem();//����
		mpUIList->SetItemText(Index,0,It2->mpT->mName);		//0��//����
		mpUIList->SetItemText(Index,1,It2->mpT->mMetier);	//1��//ְҵ
		mpUIList->SetItemText(Index,2,Level);				//2��//�ȼ�
		mpUIList->SetItemText(Index,3,PrenticeSize);		//3��//��ʦͽ��
	}

	RTW_WIDGET("fmMaster")->Show();
}
