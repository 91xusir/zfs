//********************************************************************
//	created:	2010.03.24 17:50
//	filename: 	console_main.h
//	author:		ldr123
//	purpose:	控制台模式
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
			cout<<"包的名称"<<endl;
			cin>>Temp;
			Tool.Create(Temp);
		}
		else if(Command=="version")
		{
			string Temp;
			string MainVersion;
			string SubVersion;

			cout<<"包内容的版本信息"<<endl;
			cout<<"[main][sub]"<<endl;
			cin>>Temp;
			if(Temp=="main")
			{
				cout<<"主版本";
				cin>>Temp;

				Tool.GetContentVersion(MainVersion,SubVersion);
				MainVersion=Temp;
				Tool.SetContentVersion(MainVersion,SubVersion);

			}
			else if(Temp=="sub")
			{
				cout<<"次版本信息";
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

			cout<<"包的名称"<<endl;
			cin>>PackName;
			cout<<"文件名称"<<endl;
			cin>>Temp;
			Tool.GetPackManager()->OpenPack(PackName,CPackManager::NORMAL_PACK,"rb");//不希望改变标志
			CPack* pPack=Tool.GetPackManager()->GetPack(PackName);


			const CPack::SFileInfo* pFile=pPack->GetFileInfo(Temp);
			if(pFile==NULL)
			{
				cout<<"没有找到该文件"<<endl;
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
			cout<<"包的名称"<<endl;
			cin>>Temp;
			Tool.Open(Temp,"rb+");
		}
		else if(Command=="exit")
		{
			break;
		}
		else if(Command=="pack")
		{
			cout<<"要压缩的路径"<<endl;
			cin>>Temp;
			cout<<"--------add to pack begin-----------"<<endl;
			Tool.Pack(Temp,Temp,true);
			cout<<"--------add to pack end------------"<<endl;
		}
		else if(Command=="add")
		{
			cout<<"要添加的文件名"<<endl;
			cin>>Temp;
			Tool.Add(Temp,Temp,true);
		}
		else if(Command=="delete")
		{
			cout<<"要删除的文件名"<<endl;
			cin>>Temp;
			Tool.Delete(Temp);
		}
		else if(Command=="test")
		{
		}

			/*
			int Count;
			int Times;
			cout<<"要多少次数:"<<endl;
			cin>>Times;
			cout<<"每次要多少个文件:"<<endl;
			cin>>Count;
			for(int j=0;j<Times;j++)
			{*/

				/*
				//不压缩的包
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
						pNoPackFile=NoPackFiles[rand()%NoPackCount];//随机得到一个文件
						pUnpackFile=pNoPack->GetUnpackFile(pNoPackFile);//得到不打包的文件
						pUnpackFile->GetBuffer((rand()%pNoPackFile->mOriginSize),Test);//随机读一些字节
						delete pUnpackFile;
					}

					cout<<"不压缩的一共用时：[" << Count << "个文件][" << (GetTickCount()-LastTime) << "毫秒]\n";
				}
				*/
				/*
				//压缩的包
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
				cout<<"压缩的一共用时：[" << (GetTickCount()-LastTime) << "毫秒]\n";
				}

				//new delete要多少时间
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
				cout<<"new delete一共用时：[" << (GetTickCount()-LastTime) << "毫秒]\n";
				}

				}
				*/
		else if(Command=="other_test")
		{
			/*

			int Count;
			int Times;
			cout<<"要多少次数:"<<endl;
			cin>>Times;
			cout<<"每次要多少个文件:"<<endl;
			cin>>Count;
			for(int j=0;j<Times;j++)
			{
				//压缩的包
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
						cout<<"压缩的一共用时：[" << Count<< "个文件][" << (GetTickCount()-LastTime) << "毫秒]\n";
					}
				}*/


				/*
				//不压缩的包
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
				pNoPackFile=NoPackFiles[rand()%NoPackCount];//随机得到一个文件
				pUnpackFile=pNoPack->GetUnpackFile(pNoPackFile);//得到不打包的文件
				pUnpackFile->GetBuffer((rand()%pNoPackFile->mOriginSize),Test);//随机读一些字节
				delete pUnpackFile;
				}
				cout<<"不压缩的一共用时：[" << (GetTickCount()-LastTime) << "毫秒]\n";
				}
				}

				//new delete要多少时间
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
				cout<<"new delete一共用时：[" << (GetTickCount()-LastTime) << "毫秒]\n";
				}
				}
				*/
		}
		else if(Command=="differ")
		{//差异包
			string Pack1;
			string Pack2;
			string Pack3;
			cout<<"要生成的差异包[包1-包2] = [包3]"<<endl;
			cin>>Pack1;
			cout<<"[ "<< Pack1 <<"]" <<" - ";
			cin>>Pack2;
			cout<< "["<< Pack2<<"]"<<endl;
			cin>>Pack3;
			Tool.Differ(Pack1,Pack2,Pack3);
		}
		else if(Command=="unite")
		{//合并
			string Pack1;
			string Pack2;
			cout<<"要合并的包[包1<-包2]"<<endl;
			cin>>Pack1;
			cout<<"[ "<< Pack1 <<"]" <<" + ";
			cin>>Pack2;
			cout<< "["<< Pack2<<"]"<<endl;
			Tool.Unite(Pack1,Pack2,true,true,true);
		}
		else if(Command=="option")
		{
			string Compress;
			cout<<"清选择压缩算法[null][lzw][zlib]";
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


