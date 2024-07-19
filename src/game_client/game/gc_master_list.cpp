

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
		{//如果是某列
			mMaxToMin[i]=!mMaxToMin[i];//变换
			mSortMainIndex=i;//设置主索引
		}
	}

	UpdateUI();

	return;
}


void CGCMasterList::UIDbClick(ui::RtwWidget* vpSender, RtwEventDelegate* )
{
	int Index=mpUIList->GetSelectedItem();
	string Name(mpUIList->GetItemText(Index,0));

	GetWorld()->m_Chat.OnMemberChoice(0,Name.c_str());//私聊
}

void CGCMasterList::Init()
{
	//保存列表的指针
	mpUIList=RTW_WIDGET_T(RtwListBox,"fmMaster.listmail");
	mpUIList->ModifyFlags(wfDClick,0);
	//mpUIList->SetColumnFormat(RtwStringFormat(alignCenter,alignCenter));	// 注释 [3/17/2009 tooth.shi]
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
{//要观看
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
		//系统信息//加入成功
		{
			UIFormMsg *pFrm = SysMessage(R(MSG_MASTER_LIST_APPLY_SUCCESS));
			pFrm->EvOK += RTW_CALLBACK(this,CGCMasterList,ApplyOk);
		}
		break;
	case R2C_APPLY_FAIL:
		//系统信息//加入失败
		{
			byte Code=0;

			READ_byte(Code);
			switch(Code)
			{
			case FAIL_LIST_OVERFLOW:			//列表满
				SysMessage(R(MSG_MASTER_LIST_FAIL_LIST_OVERFLOW));
				break;
			case FAIL_LEVEL_LOW:				//等级太低
				SysMessage(R(MSG_MASTER_LIST_FAIL_LEVEL_LOW));
				break;
			case FAIL_STUDENT_OVERFLOW:			//当前学生太多
				SysMessage(R(MSG_MASTER_LIST_FAIL_STUDENT_OVERFLOW));
				break;
			case FAIL_MASTER_ALREADY_EXIST:		//该师傅已经存在列表中
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

//给多级排序用
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

	mpUIList->RemoveAllItems();	//清除玩家列表UI

	//存入专门排序的列表中
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

	//排序对象
	CMultiSort			Sort;

	//按各种情况排序
	for(int i=0;i!=MASTER_LIST_COLUMN_MAX_COUNT;++i)
	{
		if(i==0 || i==1)//string
		{
			mMaxToMin[i] ? (Sort += CGreater<string>()) : (Sort += CLess<string>() );//用那种排序方式
		}
		else if(i==2 || i==3)//short int
		{
			mMaxToMin[i] ? ( Sort += CGreater<short int>()) :( Sort += CLess<short int>() );//用那种排序方式
		}
	}

	//设主索引
	Sort.SetMainIndex(mSortMainIndex);
	
	//排序
	FixList.sort( Sort );


	//将排好的list输入到UIList
	int Index;
	string Level;
	string PrenticeSize;

	list<CKey<SMasterInfo> >::iterator It2;
	for(It2=FixList.begin();It2!=FixList.end();++It2)
	{//所有的师傅
		Level.clear();
		PrenticeSize.clear();

		Level= fox::Convert<string>(It2->mpT->mLevel)+R(MSG_MASTER_LIST_LEVEL_FLAG);
		PrenticeSize= fox::Convert<string>(It2->mpT->mPrenticeAmount)+R(MSG_MASTER_LIST_COUNT_FLAG);

		Index=mpUIList->AddItem();//空行
		mpUIList->SetItemText(Index,0,It2->mpT->mName);		//0列//名字
		mpUIList->SetItemText(Index,1,It2->mpT->mMetier);	//1列//职业
		mpUIList->SetItemText(Index,2,Level);				//2列//等级
		mpUIList->SetItemText(Index,3,PrenticeSize);		//3列//出师徒弟
	}

	RTW_WIDGET("fmMaster")->Show();
}
