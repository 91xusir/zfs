//********************************************************************
//	created:	2010.03.24 17:50
//	filename: 	console_main.h
//	author:		ldr123
//	purpose:	����̨ģʽ
//********************************************************************
#include <iostream>
#include <string>
#include "pack_tool.h"
using namespace std;
#include <mmsystem.h>

inline void ConsoleMain( void ) 
{ 
	CPackTool Tool;
	string Command;
	string Temp;

	
	for(;;)
	{
		cout<<"[debug][test][other_test][differ][patch][version]"<<endl;
		cout<<"[option][exit][create][pack][add<replace>][open][unpack][unite]"<<endl;
		cin>>Command;

		if(Command=="create")
		{
			cout<<"��������"<<endl;
			cin>>Temp;
			Tool.Create(Temp);
		}
		else if(Command=="version")
		{
			string Temp;
			string MainVersion;
			string SubVersion;

			cout<<"�����ݵİ汾��Ϣ"<<endl;
			cout<<"[main][sub]"<<endl;
			cin>>Temp;
			if(Temp=="main")
			{
				cout<<"���汾";
				cin>>Temp;

				Tool.GetContentVersion(MainVersion,SubVersion);
				MainVersion=Temp;
				Tool.SetContentVersion(MainVersion,SubVersion);

			}
			else if(Temp=="sub")
			{
				cout<<"�ΰ汾��Ϣ";
				cin>>Temp;

				Tool.GetContentVersion(MainVersion,SubVersion);
				SubVersion=Temp;
				Tool.SetContentVersion(MainVersion,SubVersion);
			}


			Tool.SetContentVersion(MainVersion,SubVersion);
		}
		else if(Command == "patch")
		{
			string ExeName;
			string PackName;
			string PatchName;

			cout<<"[*.exe]+ [*.pak] = [*.exe]"<<endl;
			cin>>ExeName;
			cout<<"[" << ExeName << "] + ";
			cin>>PackName;
			cout<<"[ " << PackName << " ] =";
			cin>>PatchName;
			cout<<"[ " << PatchName<< "]" << endl;
			Tool.CreatePatchPack(ExeName,PackName,PatchName);
		}
		else if(Command =="debug")
		{
			string PackName;

			cout<<"��������"<<endl;
			cin>>PackName;
			cout<<"�ļ�����"<<endl;
			cin>>Temp;
			Tool.GetPackManager()->OpenPack(PackName,CPackManager::NORMAL_PACK,"rb");//��ϣ���ı��־
			CPack* pPack=Tool.GetPackManager()->GetPack(PackName);


			const CPack::SFileInfo* pFile=pPack->GetFileInfo(Temp);
			if(pFile==NULL)
			{
				cout<<"û���ҵ����ļ�"<<endl;
			}
			else
			{

				cout<<"Name["<< pFile->mName << "]    Offset [ "  << pFile->mOffset
					<<  " ]   SizeInPack[ "  << pFile->mSizeInPack <<" ]    OriginSize[ "<< pFile->mOriginSize <<" ]\n";
			}
		}
		else if(Command=="unpack")
		{
			Tool.Unpack();
		}
		else if(Command=="open")
		{
			cout<<"��������"<<endl;
			cin>>Temp;
			Tool.Open(Temp,"rb+");
		}
		else if(Command=="exit")
		{
			break;
		}
		else if(Command=="pack")
		{
			cout<<"Ҫѹ����·��"<<endl;
			cin>>Temp;
			cout<<"--------add to pack begin-----------"<<endl;
			Tool.Pack(Temp,Temp,true);
			cout<<"--------add to pack end------------"<<endl;
		}
		else if(Command=="add")
		{
			cout<<"Ҫ��ӵ��ļ���"<<endl;
			cin>>Temp;
			Tool.Add(Temp,Temp,true);
		}
		else if(Command=="delete")
		{
			cout<<"Ҫɾ�����ļ���"<<endl;
			cin>>Temp;
			Tool.Delete(Temp);
		}
		else if(Command=="test")
		{
		}

			/*
			int Count;
			int Times;
			cout<<"Ҫ���ٴ���:"<<endl;
			cin>>Times;
			cout<<"ÿ��Ҫ���ٸ��ļ�:"<<endl;
			cin>>Count;
			for(int j=0;j<Times;j++)
			{*/

				/*
				//��ѹ���İ�
				DWORD LastTime=timeGetTime();
				CPack* pNoPack;
				string NoPackName("nopack.pak");
				if(Tool.Open(NoPackName))
				{
					pNoPack=Tool.GetPackManager()->GetPack(NoPackName);
					vector<CPack::SFileInfo*> NoPackFiles=pNoPack->GetAllFileInfo();
					size_t NoPackCount=NoPackFiles.size();
					CPack::SFileInfo* pNoPackFile=NULL;
					CUnpackFile* pUnpackFile;
					for(int i=0;i<Count;i++)
					{
						pNoPackFile=NoPackFiles[rand()%NoPackCount];//����õ�һ���ļ�
						pUnpackFile=pNoPack->GetUnpackFile(pNoPackFile);//�õ���������ļ�
						pUnpackFile->GetBuffer((rand()%pNoPackFile->mOriginSize),Test);//�����һЩ�ֽ�
						delete pUnpackFile;
					}

					cout<<"��ѹ����һ����ʱ��[" << Count << "���ļ�][" << (GetTickCount()-LastTime) << "����]\n";
				}
				*/
				/*
				//ѹ���İ�
				LastTime=timeGetTime();
				CPack* pPack;
				string PackName("pack.pak");
				if(Tool.Open(PackName))
				{
				pPack=Tool.GetPackManager()->GetPack(PackName);
				vector<CPack::SFileInfo*> PackFiles=pPack->GetAllFileInfo();
				size_t PackCount=PackFiles.size();
				CPack::SFileInfo* pPackFile=NULL;
				CUnpackFile* pUnpackFile;
				for(int i=0;i<PackCount;i++)
				{
				pPackFile=PackFiles[rand()%PackCount];
				pUnpackFile=pPack->GetUnpackFile(pPackFile);
				pUnpackFile->GetBuffer((rand()%pPackFile->mOriginSize),Test);
				delete pUnpackFile;
				}
				cout<<"ѹ����һ����ʱ��[" << (GetTickCount()-LastTime) << "����]\n";
				}

				//new deleteҪ����ʱ��
				{

				LastTime=timeGetTime();
				CPack* pPack;
				string PackName("pack.pak");
				if(Tool.Open(PackName))
				{
				pPack=Tool.GetPackManager()->GetPack(PackName);
				vector<CPack::SFileInfo*> PackFiles=pPack->GetAllFileInfo();
				size_t PackCount=PackFiles.size();
				CPack::SFileInfo* pPackFile=NULL;
				CUnpackFile* pUnpackFile;
				for(int i=0;i<PackCount;i++)
				{
				pPackFile=PackFiles[rand()%PackCount];
				char* p=new char[rand()%pPackFile->mOriginSize];
				delete p;
				//pUnpackFile=pPack->GetUnpackFile(pPackFile);
				//pUnpackFile->GetBuffer((rand()%pPackFile->mOriginSize),Test);
				//delete pUnpackFile;
				}
				cout<<"new deleteһ����ʱ��[" << (GetTickCount()-LastTime) << "����]\n";
				}

				}
				*/
		else if(Command=="other_test")
		{
			/*

			int Count;
			int Times;
			cout<<"Ҫ���ٴ���:"<<endl;
			cin>>Times;
			cout<<"ÿ��Ҫ���ٸ��ļ�:"<<endl;
			cin>>Count;
			for(int j=0;j<Times;j++)
			{
				//ѹ���İ�
				{

					DWORD LastTime=timeGetTime();
					CPack* pPack;
					string PackName("pack.pak");
					if(Tool.Open(PackName))
					{
						pPack=Tool.GetPackManager()->GetPack(PackName);
						vector<CPack::SFileInfo*> PackFiles=pPack->GetAllFileInfo();
						size_t PackCount=PackFiles.size();
						CPack::SFileInfo* pPackFile=NULL;
						CUnpackFile* pUnpackFile;
						for(int i=0;i<Count;i++)
						{
							pPackFile=PackFiles[rand()%PackCount];
							pUnpackFile=pPack->GetUnpackFile(pPackFile);
							pUnpackFile->GetBuffer((rand()%pPackFile->mOriginSize),Test);
							delete pUnpackFile;
						}
						cout<<"ѹ����һ����ʱ��[" << Count<< "���ļ�][" << (GetTickCount()-LastTime) << "����]\n";
					}
				}*/


				/*
				//��ѹ���İ�
				{

				DWORD LastTime=timeGetTime();
				CPack* pNoPack;
				string NoPackName("nopack.pak");
				if(Tool.Open(NoPackName))
				{
				pNoPack=Tool.GetPackManager()->GetPack(NoPackName);
				vector<CPack::SFileInfo*> NoPackFiles=pNoPack->GetAllFileInfo();
				size_t NoPackCount=NoPackFiles.size();
				CPack::SFileInfo* pNoPackFile=NULL;
				CUnpackFile* pUnpackFile;
				for(int i=0;i<NoPackCount;i++)
				{
				pNoPackFile=NoPackFiles[rand()%NoPackCount];//����õ�һ���ļ�
				pUnpackFile=pNoPack->GetUnpackFile(pNoPackFile);//�õ���������ļ�
				pUnpackFile->GetBuffer((rand()%pNoPackFile->mOriginSize),Test);//�����һЩ�ֽ�
				delete pUnpackFile;
				}
				cout<<"��ѹ����һ����ʱ��[" << (GetTickCount()-LastTime) << "����]\n";
				}
				}

				//new deleteҪ����ʱ��
				{

				DWORD LastTime=timeGetTime();
				CPack* pPack;
				string PackName("pack.pak");
				if(Tool.Open(PackName))
				{
				pPack=Tool.GetPackManager()->GetPack(PackName);
				vector<CPack::SFileInfo*> PackFiles=pPack->GetAllFileInfo();
				size_t PackCount=PackFiles.size();
				CPack::SFileInfo* pPackFile=NULL;
				CUnpackFile* pUnpackFile;
				for(int i=0;i<PackCount;i++)
				{
				pPackFile=PackFiles[rand()%PackCount];
				char* p=new char[rand()%pPackFile->mOriginSize];
				delete p;
				//pUnpackFile=pPack->GetUnpackFile(pPackFile);
				//pUnpackFile->GetBuffer((rand()%pPackFile->mOriginSize),Test);
				//delete pUnpackFile;
				}
				cout<<"new deleteһ����ʱ��[" << (GetTickCount()-LastTime) << "����]\n";
				}
				}
				*/
		}
		else if(Command=="differ")
		{//�����
			string Pack1;
			string Pack2;
			string Pack3;
			cout<<"Ҫ���ɵĲ����[��1-��2] = [��3]"<<endl;
			cin>>Pack1;
			cout<<"[ "<< Pack1 <<"]" <<" - ";
			cin>>Pack2;
			cout<< "["<< Pack2<<"]"<<endl;
			cin>>Pack3;
			Tool.Differ(Pack1,Pack2,Pack3);
		}
		else if(Command=="unite")
		{//�ϲ�
			string Pack1;
			string Pack2;
			cout<<"Ҫ�ϲ��İ�[��1<-��2]"<<endl;
			cin>>Pack1;
			cout<<"[ "<< Pack1 <<"]" <<" + ";
			cin>>Pack2;
			cout<< "["<< Pack2<<"]"<<endl;
			Tool.Unite(Pack1,Pack2,true,true,true);
		}
		else if(Command=="option")
		{
			string Compress;
			cout<<"��ѡ��ѹ���㷨[null][lzw][zlib]";
			cin>>Compress;
			if(Compress=="null")
			{
				Tool.SetOpertion(CPack::NULL_COMPRESS,CNULLCompress::LAST_VERSION);
			}
			else if(Compress=="lzw")
			{
				Tool.SetOpertion(CPack::LZW_COMPRESS,CLzwArithmetic::LAST_VERSION);
			}
			else if(Compress=="zlib")
			{
				Tool.SetOpertion(CPack::ZLIB_COMPRESS,CZlibArithmetic::LAST_VERSION);
			}
		}
		else
		{
			cout<<"unknow command!"<<endl;
		}

	}

	Tool.Clear();
	return ;
}


