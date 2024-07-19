
#include "gc_include.h"
#include "gc_mail.h"

fox::CStrMap					gMailStr;//字符串索引

//const int STRING_LENGTH=1024;




void CClientMail::DeleteServerMail(string& vrDate)
{
	guard;
	CG_CmdPacket* pPacket = NetBeginWrite();
	pPacket->WriteShort(c2r_subsystem_mail);
	pPacket->WriteByte(C2R_DELETE_MAIL);
	pPacket->WriteString((char*)vrDate.c_str()); // 服务器上有检查长度
	NetSend(pPacket);
	return;
	unguard;
}

void CClientMail::Receive(CG_CmdPacket *pPacket)
{
	guard;
	BYTE Operation;
	char* MailDate;
	long  MailID;
	long FromDBID;
	char* pFromName;
	char* pTitle;
	char* pContent;
	char* pItemStr;
	SItemID item;
	long money;
	long readorno;
	CItemManager *pItemMgr = ((CGameClientFrame*)GetApp())->m_pItemManager;

	READ_byte(Operation);

	if(Operation==R2C_RECEIVE_MAIL)
	{
		READ_long(MailID);
		READ_string(MailDate);
		READ_long(FromDBID);
		READ_string(pFromName);
		READ_string(pTitle);
		READ_string(pContent);
		READ_string(pItemStr);
		item.LoadFromString(11,pItemStr,pItemMgr);
		READ_long(money);
		READ_long(readorno);

		SMailData* pMail=RT_NEW SMailData;
		/*
		pMail->mTime.mDay		=0;
		pMail->mTime.mMonth		=0;
		pMail->mTime.mYear		=0;
		pMail->mTime.mHour		=0;
		pMail->mTime.mMinute	=0;
*/
		pMail->m_lMailID = MailID;
		pMail->mTime=MailDate;
		pMail->mFromDBID=FromDBID;
		pMail->mFromName=pFromName;
		pMail->mTitle=pTitle;
		pMail->mContent=pContent;
		pMail->m_Item = item;
		pMail->m_lMoney = money;
		pMail->mAlreadyOpen=(bool)readorno;
		
		//DeleteServerMail(pMail->mTime);
		std::list<SMailData*>::iterator it;
		for(it= mReceiveBox.begin();it!= mReceiveBox.end();it++)
		{
			if(pMail->m_lMailID == (*it)->m_lMailID)
			{
				DEL_ONE(pMail);
				return;
			}
		}
		mReceiveBox.push_back(pMail);
		OnReceive(pMail);
	}
	else
	{
		mpFriend->ReceiveFromServer(Operation,pPacket);
	}
	unguard;
}


CClientMail::~CClientMail()
{
	guard;
	list<SMailData*>::iterator It;
	for(It=mReceiveBox.begin();It!=mReceiveBox.end();It++)
	{
		DEL_ONE(*It)
	}
	unguard;
}

void CClientMail::Delete(SMailData* vpMail)//删除本地邮件
{
	guard;
	if(vpMail==NULL)
	{
		return;
	}

	list<SMailData*>::iterator It;
	for(It=mReceiveBox.begin();It!=mReceiveBox.end();It++)
	{
		if(*It==vpMail)
		{
			DEL_ONE(*It)
			mReceiveBox.erase(It);
			OnDelete(vpMail);
			return;
		}
	}
	return;
	unguard;
}



void CClientMail::Send(DWORD vDBID,char* vpTitle,char* vpContent,DWORD SendItemID,long money)	//发邮件
{
	guard;
	string Title(vpTitle);
	string Content(vpContent);

	//if(Title.size()>MAIL_TITLE_LIMIT_COUNT)
	//{
	//	GetWorld()->m_Chat.SystemMessage(R(MSG_MAIL_TITLE_LIMIT ),true);
	//	Title=Title.substr(0,MAIL_TITLE_LIMIT_COUNT );
	//}

	//if(Content.size()>MAIL_CONTENT_LIMIT_COUNT)
	//{
	//	GetWorld()->m_Chat.SystemMessage(R(MSG_MAIL_CONTENT_LIMIT),true);
	//	Content=Content.substr(0,MAIL_CONTENT_LIMIT_COUNT);
	//}


	CG_CmdPacket* pPacket = NetBeginWrite();
	pPacket->WriteShort(c2r_subsystem_mail);
	pPacket->WriteByte(C2R_SEND_MAIL);
	pPacket->WriteLong(SendItemID);
	pPacket->WriteLong(money);
	pPacket->WriteLong(vDBID);
	pPacket->WriteString((char*)Title.c_str()); // 服务器上有检查长度
	pPacket->WriteString((char*)Content.c_str()); // 服务器上有检查长度
	NetSend(pPacket);
	return;
	unguard;
}


const char LOCAL_MAIL_DIRECTORY[]="m_data";
const char LOCAL_MAIL_PREFIX[]="m_";
const char LOCAL_MAIL_EXPORT_DIRECTORY[]="mail";
const char LOCAL_MAIL_EXPORT_PREFIX[]="mail_";

void CClientMail::Load()
{
	guard;
    if (GetWorld()==NULL)
        return;
    if (GetWorld()->m_pPlayer==NULL)
        return;
    const char* pUserName=GetWorld()->m_pPlayer->GetName();		//得到用户名[玩家数据库唯一]
    if (pUserName==NULL)
        return;
    if (pUserName[0]==0)
        return;

	mSelfName=pUserName;

	FILE* pFile;
	char Temp[STRING_LENGTH*2+1];
	char NameMD5[STRING_LENGTH];
	char FileName[STRING_LENGTH];

	rtMD5StringString((char*)mSelfName.c_str(),NameMD5);
	rt2_sprintf(FileName,"%s\\%s%s",LOCAL_MAIL_DIRECTORY,LOCAL_MAIL_PREFIX,NameMD5);
	pFile=fopen(FileName,"rb+");
	if(pFile==NULL)
	{//不存在，返回
		return;
	}

	string DecryptString;
	string NameMD5String(NameMD5);
	int		Size=0;
	SMailData* pMail;



	int Version=0;		//当前版本
	int Count=0;		//一共有多少封信



	if(fread(&Version,sizeof(Version),1,pFile)!=1)
	{//有错，//也要把该文件删除
		goto load_mail_file_fail;
	}

	fseek(pFile,0,SEEK_SET);//到头
	fwrite(&LOCATION_MAIL_NO_CLOSE,sizeof(LOCATION_MAIL_NO_CLOSE),1,pFile);//表明文件开始读出

	switch(Version)
	{
	case LOCATION_MAIL_NO_CLOSE://上一次文件没有正常退出//将该文件删除
		goto load_mail_file_fail;
		break;
	case 100://版本
		{
			if(fread(&Count,sizeof(Count),1,pFile)!=1)
			{//有错，//也要把该文件删除
				goto load_mail_file_fail;
			}

			for(int i=0;i!=Count;i++)
			{
				pMail=RT_NEW SMailData;				//得到新的邮件数据
				
				if(fread(&Size,sizeof(Size),1,pFile) != 1) //字符串长度[不含结尾零]
				{//有错，//也要把该文件删除
					goto load_mail_file_fail;
				}

				if( Size>=sizeof(Temp))
				{//有错，超过每封信的长度
					goto load_mail_file_fail;
				}

				if( fread(&Temp,sizeof(char),Size,pFile) != Size)	//从文件读出数据
				{//有错，//也要把该文件删除
					goto load_mail_file_fail;
				}

				Decrypt(Temp,Size,NameMD5String);				//解密
				Temp[Size]='\0';
				DecryptString=Temp;
				DecryptString>>(*pMail);										//读入到邮件数据中

				mReceiveBox.push_back(pMail);
				OnReceive(pMail);
			}
		}
		break;
	default:
		//有错，//也要把该文件删除
		fclose(pFile);
		RtCoreFile().Delete(FileName);
		return;
		break;
	}


	fseek(pFile,0,SEEK_SET);//到头
	fwrite(&Version,sizeof(Version),1,pFile);//文件正常结束

	fclose(pFile);
	return;

load_mail_file_fail:
	fclose(pFile);
	RtCoreFile().Delete(FileName);
	return;
	
	unguard;

}


void CClientMail::Save()
{
	guard;
	FILE* pFile;
	char Temp[STRING_LENGTH];
	char NameMD5[STRING_LENGTH];

	rtMD5StringString((char*)mSelfName.c_str(),NameMD5);
	rt2_sprintf(Temp,"%s\\%s%s",LOCAL_MAIL_DIRECTORY,LOCAL_MAIL_PREFIX,NameMD5);

	pFile=fopen(Temp,"wb");
	if(pFile==NULL)
	{//可能是没有目录，创建一个目录
		RtCoreFile().MakeDirectory(LOCAL_MAIL_DIRECTORY);
		pFile=fopen(Temp,"w");
	}

	if(pFile==NULL)
	{//还是没有目录，返回
		LOG("文件无法保存！\n");
		return;
	}



	//有多少信
	int Count=mReceiveBox.size();

	//fwrite(&LAST_LOCATION_MAIL_HEAD_VERSION,sizeof(int),1,pFile);		//当前版本
	fwrite(&LOCATION_MAIL_NO_CLOSE,sizeof(int),1,pFile);				//当前正在存盘
	fwrite(&Count,sizeof(int),1,pFile);									//信的总数 

	char EncryptString[STRING_LENGTH*2];
	string NameMD5String(NameMD5);
	string TempString;
	int	   Size=0;
	std::list<SMailData*>::iterator It;
	for(It=mReceiveBox.begin();It!=mReceiveBox.end();It++)
	{
		TempString="";
		TempString<<(*(*It));									//保存到字符串
		int Size=TempString.size();
		if(Size>sizeof(EncryptString))
		{//超过每封信最大值//不保存这封信
			continue;
		}
		memcpy(EncryptString,TempString.c_str(),Size);
		GSAssistantEncrypt(EncryptString,Size,NameMD5String);		//加密
		fwrite(&Size,sizeof(Size),1,pFile);									//字符串长度
		fwrite(EncryptString,sizeof(char),Size,pFile);	//字符串保存到文件
	}
	fseek(pFile,0,SEEK_SET);//到文件头
	fwrite(&LAST_LOCATION_MAIL_HEAD_VERSION,sizeof(int),1,pFile);		//当前版本//同时表示正常退出
	fclose(pFile);
	unguard;
}


void CClientMail::Export()
{
	guard;
	FILE* pFile;
	string Path;
	char Temp[STRING_LENGTH];

	char szTime[64];

	time_t dtime;
	struct tm *ttime;
	::time(&dtime);
	ttime = localtime(&dtime);
	strftime(szTime,64,"%Y-%m-%d %H.%M.%S",ttime); 
	Path=Path+LOCAL_MAIL_EXPORT_DIRECTORY+"\\"+szTime+".txt";

	pFile=fopen(Path.c_str(),"w+");

	if(pFile==NULL)
	{//可能是没有目录，创建一个目录
		RtCoreFile().MakeDirectory(LOCAL_MAIL_EXPORT_DIRECTORY);
		pFile=fopen(Path.c_str(),"w+");
	}

	if(pFile==NULL)
	{//还是没有目录，返回
		LOG1(const_cast<char*>(R(MAIL_CAN_NOT_EXPORT)),"\n");
		//LOG("文件无法保存！\n");
		return;
	}



	rt2_sprintf(Temp,R(EXPORT_TOTAL_RECEIVE_MAIL),(int)mReceiveBox.size(),"\n");//一共收到信件[%d]封\n
	fputs(Temp,pFile);

	std::list<SMailData*>::iterator It;
	for(It=mReceiveBox.begin();It!=mReceiveBox.end();It++)
	{

		rt2_sprintf(Temp,R(EXPORT_MAIL_TIME),(*It)->mTime.c_str());//时间[%s] 
		fputs(Temp,pFile);

		rt2_sprintf(Temp,R(EXPORT_MAIL_FROM_NAME),(*It)->mFromName.c_str());//作者[%s] 
		fputs(Temp,pFile);

		rt2_sprintf(Temp,R(EXPORT_MAIL_TITLE),(*It)->mTitle.c_str(),"\n");//标题[%s]\n
		fputs(Temp,pFile);

		rt2_sprintf(Temp,R(EXPORT_MAIL_FLAG),(*It)->mContent.c_str(),"\n\n");//%s\n\n
		fputs(Temp,pFile);
	}
	fclose(pFile);

	Path="";

	Path=Path+R(MAIL_EXPORT_TO_FLAG1) +LOCAL_MAIL_EXPORT_DIRECTORY +"\\"+ szTime+".txt"+R(MAIL_EXPORT_TO_FLAG3);
	Message(Path);
	unguard;
}




/*
void CClientMail::Message(DWORD vID,const std::string& vrMessage);
void CClientMail::MessageWithChoice(DWORD vDBID,const std::string& vrMessage){};
*/




