//********************************************************************
//	created:	2010.03.24 17:50
//	filename: 	tool_files_packetView.cpp
//	author:		ldr123
//	purpose:	
//********************************************************************
#include "stdafx.h"
#include "tool_files_packet.h"
#include "tool_files_packetDoc.h"
#include "tool_files_packetView.h"
#include <string>
#include <list>
#include <io.h>
#include "game_string_assistant.h"

using std::string;
using std::list;

#ifndef _countof
#define _countof(array) (sizeof(array)/sizeof(array[0]))
#endif

//�б��е�ͼ��
const int  FILE_IMAGE_INDEX=0;
const int  FOLDER_IMAGE_INDEX=1;
const int  DRIVE_FIXED_IMAGE_INDEX=2;
const int  PACK_IMAGE_INDEX=3;
const int  UNKNOW_IMAGE_INDEX=4;


const char FOLDER_TYPE[]="�ļ���";	//�ļ������ͱ�־
const char FILE_TYPE[]="�ļ�";		//�ļ����ͱ�־
const char PACK_TYPE[]="�ļ���";		//�ļ������ͱ�־
const char FILE_IN_PACK_TYPE[]="�����ļ�";//�����ļ�����
const char FOLDER_IN_PACK_TYPE[]="�����ļ���";	//�ļ������ͱ�־

const char DRIVE_REMOVABLE_TYPE[]="���ƶ�������";
const char DRIVE_FIXED_TYPE[]="����������";
const char DRIVE_REMOTE_TYPE[]="����������";
const char DRIVE_CDROM_TYPE[]="CDROM������";

IMPLEMENT_DYNCREATE(Ctool_files_packetView, CListView)

BEGIN_MESSAGE_MAP(Ctool_files_packetView, CListView)
	ON_WM_LBUTTONDBLCLK()
	ON_COMMAND(ID_FILE_NEW,Ctool_files_packetView::OnAddFilesToPack)
	ON_COMMAND(ID_FILE_RELEASE,Ctool_files_packetView::OnReleaseFiles)
	
	ON_COMMAND(ID_NULL_COMPRESS,OnNullCompress)
	ON_COMMAND(ID_LZW_COMPRESS,OnLzwCompress)
	ON_COMMAND(ID_ZLIB_COMPRESS,OnZlibCompress)

	ON_UPDATE_COMMAND_UI(ID_NULL_COMPRESS,OnUpdateNullCompress)
	ON_UPDATE_COMMAND_UI(ID_LZW_COMPRESS,OnUpdateLzwCompress)
	ON_UPDATE_COMMAND_UI(ID_ZLIB_COMPRESS,OnUpdateZlibCompress)

	ON_COMMAND(ID_ADVANVE_VERSION,Ctool_files_packetView::OnAdvanceVersion)		//ac.ma
	ON_COMMAND(ID_COMPARE,Ctool_files_packetView::OnCompareMD5)		//ac.ma
	ON_COMMAND(ID_BUTTON_OVERLAP,Ctool_files_packetView::OnCheckOverlap)		//ac.ma
END_MESSAGE_MAP()

Ctool_files_packetView::Ctool_files_packetView()
:mCompress(NULL_COMPRESS)
{
	// TODO: �ڴ˴���ӹ������

}

Ctool_files_packetView::~Ctool_files_packetView()
{
}

BOOL Ctool_files_packetView::PreCreateWindow(CREATESTRUCT& cs)
{
	cs.style |= LVS_REPORT|LVS_AUTOARRANGE;
	return CListView::PreCreateWindow(cs);
}

void Ctool_files_packetView::OnDraw(CDC* /*pDC*/)
{
	Ctool_files_packetDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;
}

BOOL Ctool_files_packetView::OnPreparePrinting(CPrintInfo* pInfo)
{
	return DoPreparePrinting(pInfo);
}

void Ctool_files_packetView::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/){}
void Ctool_files_packetView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/){}

//��ʾ�õ�����
struct SDisplayData 
{ 
	LPCTSTR name; 
	int width; 
	int fmt;
};

static SDisplayData columns[] =
{
	{ _T("����"),       170,	LVCFMT_LEFT},
	{ _T("ԭʼ��С"),   70,		LVCFMT_RIGHT},
	{ _T("���ڴ�С"),   70,		LVCFMT_LEFT},
	{ _T("����"),       80,		LVCFMT_LEFT},
	{ _T("ѹ����ʽ"),   60,		LVCFMT_CENTER},
	{ _T("�汾"),       40,		LVCFMT_CENTER},
	{ _T("MD5"),        230,	LVCFMT_LEFT},
	{ _T("�޸�����"),   660,	LVCFMT_LEFT},
};

//˳��ͬ��
const int NAME_COLUMN_INDEX=0;
const int SIZE_COLUMN_INDEX=1;
const int PACK_SIZE_COLUMN_INDEX=2;
const int TYPE_COLUMN_INDEX=3;
const int COMPRESS_COLUMN_INDEX=4;
const int VERSION_COLUMN_INDEX=5;
const int MD5_COLUMN_INDEX=6;
const int MODIFY_TIME_COLUMN_INDEX=7;


void Ctool_files_packetView::OnInitialUpdate()
{
	CListView::OnInitialUpdate();


	CListCtrl& listCtrl = GetListCtrl();
	int i;
	for (i = 0; i < _countof(columns); ++i)
	{
		listCtrl.InsertColumn(i, columns[i].name, columns[i].fmt, 
			columns[i].width);
	}

	::SendMessage(listCtrl.GetSafeHwnd(), LVM_SETEXTENDEDLISTVIEWSTYLE, 0,  LVS_EX_FULLROWSELECT );//LVS_EX_GRIDLINES| LVS_EX_HEADERDRAGDROP);

	static CImageList Image;
	static CBitmap Bmp1;
	static CBitmap Bmp2;
	static CBitmap Bmp3;
	static CBitmap Bmp4;
	static CBitmap Bmp5;

	Bmp1.LoadBitmap(IDB_BITMAP1);
	Bmp2.LoadBitmap(IDB_BITMAP2);
	Bmp3.LoadBitmap(IDB_BITMAP3);
	Bmp4.LoadBitmap(IDB_BITMAP4);
	Bmp5.LoadBitmap(IDB_BITMAP5);

	Image.Create(16,16,ILC_COLOR24,5,5);

	Image.Add(&Bmp1,RGB(0,0,0));
	Image.Add(&Bmp2,RGB(0,0,0));
	Image.Add(&Bmp3,RGB(0,0,0));
	Image.Add(&Bmp4,RGB(0,0,0));
	Image.Add(&Bmp5,RGB(0,0,0));

	listCtrl.SetImageList(&Image,LVSIL_SMALL);
	DisplayFiles("");
}

//Win32
void FilesInDirectory(const char vPath[],list<_finddata_t>& vrFiles)
{
	_finddata_t FileData; 
	int Ret=0;
	intptr_t hFile=_findfirst(vPath,&FileData);
	for(;(hFile!=-1 && Ret==0) ;Ret=_findnext(hFile,&FileData))
	{
		vrFiles.push_back(FileData);
	}
	_findclose(hFile);
}

void Ctool_files_packetView::GetFilesInDirectory(const char vPath[],list<SFileAttrib>& vrFiles)
{
	list<_finddata_t> List;
	FilesInDirectory(vPath,List);

	SFileAttrib Temp;
	list<_finddata_t>::iterator It;
	bool IsRoot=true;

	string Path=GetPathName();


	for(It=List.begin();It!=List.end();++It)
	{
		Temp.Clear();

		Temp.mName=It->name;//����

		//����
		Temp.mType = ((It->attrib&_A_SUBDIR)!=0) ?  FOLDER_TYPE : FILE_TYPE ;
		//�ļ��޸�����
		Temp.mModifyTime=fox::TimeConvert(It->time_write);


		if(Temp.mType==FILE_TYPE)
		{//��һ���ж��Ƿ���ļ�

			//�ļ���С
			Temp.mSize=fox::UnitConvert(It->size);

			if(mTool.IsPack(Path+Temp.mName))
			{
				//�汾
				Temp.mVersion=Convert<string>(mTool.PackVersion(Path+Temp.mName));
				Temp.mType=PACK_TYPE;

				//��ʱ���ر���
				long StartTime=0;
				long EndTime=0;
				string StartString;
				string EndString;
				string MainVersion;
				string SubVersion;

				mTool.GetPackTime((Path+Temp.mName),StartTime,EndTime);
				mTool.GetContentVersion((Path+Temp.mName),MainVersion,SubVersion);

				using fox::TimeConvert;
				string TempString;
				TempString="���汾[";
				TempString+=MainVersion;
				TempString+="] �ΰ汾[";
				TempString+=SubVersion;
				TempString+="]";
				
				TempString+=" (";
				TempString=TempString+TimeConvert(StartTime);
				TempString=TempString+"----";
				TempString=TempString+TimeConvert(EndTime);
				TempString=TempString+"]";

				Temp.mModifyTime=TempString;
			}
		}

		vrFiles.push_back(Temp);
		if(Temp.mName=="..")
		{//����root��".."
			IsRoot=false;
		}
	}

	//��rootĿ¼�����һ��".."Ŀ¼
	if(IsRoot)
	{//���һ��".."Ŀ¼
		Temp.Clear();
		Temp.mName="..";
		Temp.mType=FOLDER_TYPE;
		Temp.mSize="";
		vrFiles.push_front(Temp);
	}

	return;
}

void Ctool_files_packetView::GetFilesInPack(const string vPackName,list<SFileAttrib>& vrFiles)
{
	mTool.Open(vPackName,"rb");//����д��־

	SFileAttrib Temp;

	vector<CPack::SFileInfo*> Files;
	if(mTool.GetAllFilesInPack(Files))//�õ����������ļ�����Ϣ
	{
		vector<CPack::SFileInfo*>::iterator It;
		for(It=Files.begin();It!=Files.end();++It)
		{
			Temp.Clear();
			Temp.mName=(*It)->mName;//����
			Temp.mType=FILE_IN_PACK_TYPE;//����
			Temp.mSize=fox::UnitConvert((*It)->mOriginSize);//ԭʼ��С
			Temp.mSizeInPack=fox::UnitConvert((*It)->mSizeInPack);//���д�С

			//ѹ����ʽ
			switch((*It)->mCompressMode) 
			{
			case CPack::NULL_COMPRESS:
				Temp.mCompressMode="��ѹ��";
				break;
			case CPack::LZW_COMPRESS:
				Temp.mCompressMode="LZWѹ��";
				break;
			case CPack::ZLIB_COMPRESS:
				Temp.mCompressMode="ZLIBѹ����ʽ";
				break;
			default:
				Temp.mCompressMode="δ֪ѹ��";
				break;
			}

			Temp.mVersion=fox::Convert<string>((*It)->mCompressVersion);//ѹ���汾
			Temp.mModifyTime=fox::TimeConvert((*It)->mWriteTime);		//�޸�ʱ��

			//MD5
			char TempMD5[16*2+1];//MD5_LENGTH=16
			rtMD5PrintString(TempMD5,(unsigned char *)((*It)->mMD5));
			TempMD5[16*2]='\0';//��β��
			strupr(TempMD5);//��д
			Temp.mMD5=TempMD5;


			vrFiles.push_back(Temp);
		}
	}
	return ;
}

void GetDrive(list<SFileAttrib>& vrDriveList)
{
	char Drive;
	string Temp;
	SFileAttrib FileTemp;
	UINT Type;

	for(Drive='A';Drive<='Z';Drive++)
	{
		Temp=Drive;
		Temp+=":\\";

		FileTemp.mName=Drive;
		FileTemp.mName+=":";

		Type=GetDriveType(Temp.c_str());

		switch(Type)
		{
		case DRIVE_UNKNOWN:
			break;
		case DRIVE_NO_ROOT_DIR:
			break;
		case DRIVE_REMOTE:
			break;

		case DRIVE_FIXED:
			FileTemp.mType=DRIVE_FIXED_TYPE;
			vrDriveList.push_back(FileTemp);
			break;
		case DRIVE_REMOVABLE:
			FileTemp.mType=DRIVE_REMOVABLE_TYPE;
			vrDriveList.push_back(FileTemp);
			break;
		case DRIVE_CDROM:
			FileTemp.mType=DRIVE_CDROM_TYPE;
			vrDriveList.push_back(FileTemp);
			break;
		case DRIVE_RAMDISK:
			break;
		default:
			break;
		} 
	}
}

void Ctool_files_packetView::DisplayDrives()
{
	list<SFileAttrib> List;
	GetDrive(List);
	Display(List);
}

string Ctool_files_packetView::GetPathName(const string vFile)//�õ���ǰ��·����
{//�õ���ǰ·��
	string FullPath;
	list<string>::iterator It;
	for(It=mPathList.begin();It!=mPathList.end();++It)
	{
		FullPath+=(*It);
		FullPath+="\\";
	}

	return FullPath+vFile;
}

void Ctool_files_packetView::Display(list<SFileAttrib>& vrFiles)//��ʾ�б�
{
	int i;
	int j;
	CListCtrl& listCtrl = GetListCtrl();//�õ�CListCtrl�ؼ�
	listCtrl.DeleteAllItems();			//����ɵ�ѡ��

	list<SFileAttrib>::iterator It;
	for(It=vrFiles.begin(),i=0;It!=vrFiles.end();++It)
	{

		if(It->mName==".")
		{//����Ҫ��ʾ��ǰĿ¼
			continue;
		}

		//�������ֺ�ͼ��
		CString strItem1=It->mName.c_str();

		const int  FILE_IMAGE_INDEX=0;
		const int  FOLDER_IMAGE_INDEX=1;
		const int  DRIVE_FIXED_IMAGE_INDEX=2;
		const int  PACK_IMAGE_INDEX=3;
		const int  UNKNOW_IMAGE_INDEX=4;

		if(It->mType==FILE_TYPE || It->mType==FILE_IN_PACK_TYPE)
		{
			listCtrl.InsertItem(i, strItem1,FILE_IMAGE_INDEX);
		}
		else if(It->mType==FOLDER_TYPE || It->mType==FOLDER_IN_PACK_TYPE )
		{
			listCtrl.InsertItem(i, strItem1,FOLDER_IMAGE_INDEX);
		}
		else if(It->mType==DRIVE_FIXED_TYPE )
		{
			listCtrl.InsertItem(i, strItem1,DRIVE_FIXED_IMAGE_INDEX);
		}
		else if(It->mType==PACK_TYPE)
		{
			listCtrl.InsertItem(i, strItem1,PACK_IMAGE_INDEX);
		}
		else
		{
			listCtrl.InsertItem(i, strItem1,UNKNOW_IMAGE_INDEX);
		}

		for (j = 1; j < _countof(columns); ++j)
		{
			switch(j)
			{
			case SIZE_COLUMN_INDEX:
				listCtrl.SetItem(i, j, LVIF_TEXT,It->mSize.c_str(), 0, 0, 0, 0);
				break;
			case PACK_SIZE_COLUMN_INDEX:
				listCtrl.SetItem(i, j, LVIF_TEXT,It->mSizeInPack.c_str(), 0, 0, 0, 0);
				break;
			case TYPE_COLUMN_INDEX://[����]
				listCtrl.SetItem(i, j, LVIF_TEXT,It->mType.c_str(), 0, 0, 0, 0);
				break;
			case COMPRESS_COLUMN_INDEX:
				listCtrl.SetItem(i, j, LVIF_TEXT,It->mCompressMode.c_str(), 0, 0, 0, 0);
				break;
			case VERSION_COLUMN_INDEX:
				if(It->mVersion=="-1")
				{
					listCtrl.SetItem(i, j, LVIF_TEXT,"̫��", 0, 0, 0, 0);
				}
				else
				{
					listCtrl.SetItem(i, j, LVIF_TEXT,It->mVersion.c_str(), 0, 0, 0, 0);
				}
				break;
			case MD5_COLUMN_INDEX:
				listCtrl.SetItem(i, j, LVIF_TEXT,It->mMD5.c_str(), 0, 0, 0, 0);
				break;
			case MODIFY_TIME_COLUMN_INDEX://[�޸�ʱ��]
				listCtrl.SetItem(i, j, LVIF_TEXT,It->mModifyTime.c_str(), 0, 0, 0, 0);
				break;
			default:
				break;
			}
		}
		++i;
	}
}

string Ctool_files_packetView::GetPackPathName(const string vFile)//�õ���ǰ��·����
{//�õ���ǰ·��
	string FullPath;
	list<string>::iterator It;
	if(mPackPathList.size()<=1)
	{//��һ���ǰ�������
		return "";
	}

	for(It=++mPackPathList.begin();It!=mPackPathList.end();++It)
	{
		FullPath+=(*It);
		FullPath+="\\";
	}

	FullPath+=vFile;

	return FullPath;
}

void Ctool_files_packetView::DisplayPack(const string vPackName)
{
	if(vPackName=="..")
	{//��һ��Ŀ¼
		mPackPathList.pop_back();
	}
	else
	{//��ӵ���·���б���
		mPackPathList.push_back(vPackName);
	}

	if(mPackPathList.empty())
	{
		//MessageBox("��ʾĿ¼�б��е��ļ�");
		DisplayFiles("");
		return;
	}

	//��ʾ�����ļ�
	//�õ����еİ����ļ��б�
	list<SFileAttrib> List;
	list<SFileAttrib>::iterator It;
	string Temp;
	string Path;

	Path=GetPathName()+mPackPathList.front();//���ļ���ȫ��
	GetFilesInPack(Path,List);

	Path=GetPackPathName("");
	//�����е��ļ��ص�����·������
	for(It=List.begin();It!=List.end();)
	{
		if(It->mName.find(Path)==0)
		{//�����·���µ�
			It->mName=It->mName.substr(Path.size());
			++It;
		}
		else
		{//�������·���µ�
			It=List.erase(It);
		}
	}

	//�������ļ��ֳ�Ŀ¼
	set<string> Folders;
	list<SFileAttrib> FilesList;
	list<SFileAttrib> FoldersList;

	SFileAttrib TempAttrib;

	for(It=List.begin();It!=List.end();++It)
	{
		Temp= It->mName;
		Temp=Temp.substr(0,Temp.find_first_of('\\'));

		if(Temp==It->mName)
		{//���ļ�
			TempAttrib.Clear();
			TempAttrib=*It;
			TempAttrib.mName=Temp;
			TempAttrib.mType=FILE_IN_PACK_TYPE;

			FilesList.push_back(TempAttrib);
		}
		else
		{//���ļ���
			Folders.insert(Temp);
		}
	}

	set<string>::iterator It1;
	for(It1=Folders.begin();It1!=Folders.end();++It1)
	{
		TempAttrib.Clear();
		TempAttrib.mName=*It1;
		TempAttrib.mType=FOLDER_IN_PACK_TYPE;
		FoldersList.push_back(TempAttrib);
	}

	for(It=FilesList.begin();It!=FilesList.end();++It)
	{
		FoldersList.push_back(*It);
	}

	//���".."
	TempAttrib.Clear();
	TempAttrib.mName="..";
	TempAttrib.mType=FOLDER_IN_PACK_TYPE;
	FoldersList.push_front(TempAttrib);
	Display(FoldersList);
}

void Ctool_files_packetView::DisplayFiles(const char vPath[])
{
	string Path(vPath);
	if(Path=="." )
	{//��ǰĿ¼������
		return;
	}

	if(Path=="..")
	{//��һ��Ŀ¼
		mPathList.pop_back();
	}
	else if(Path=="")
	{
	}
	else 
	{//��ӵ�·���б���
		mPathList.push_back(Path);
	}

	string FullPath=GetPathName();//�õ�·����

	if(FullPath=="")
	{//�õ���ǰ���е�������
		DisplayDrives();
		return;
	}

	FullPath+="*.*";		//��·�������е��ļ�

	//��ʾ�ļ�
	list<SFileAttrib> List;
	GetFilesInDirectory(FullPath.c_str(),List);
	Display(List);
}

void Ctool_files_packetView::OnLButtonDblClk(UINT nFlags, CPoint point)
{
	CListCtrl& rListCtrl = GetListCtrl();

	UINT Count=rListCtrl.GetSelectedCount();
	int Item=-1;
	CString Name;
	CString Type;

	if(Count==1)
	{//ֻѡ����һ��//�������ļ���,�����Ǳ���������
		Item=rListCtrl.GetNextItem(Item,LVNI_SELECTED);
		Name=rListCtrl.GetItemText(Item,0);
		Type=rListCtrl.GetItemText(Item,3);
		if(Type==FOLDER_TYPE || Type==DRIVE_FIXED_TYPE)
		{//�ļ��л�����������
			DisplayFiles(Name);
		}
		else if(Type==PACK_TYPE)
		{
			string Temp=Name;
			m_sPackName = Name;
			DisplayPack(Temp);
		}
		else if(Type==FILE_TYPE)
		{//���ļ�
		}
		else if(Type==FOLDER_IN_PACK_TYPE)
		{//�����ļ���
			string NameInPack=Name;
			DisplayPack(NameInPack);
		}
	}
}

void Ctool_files_packetView::GetSelected(list<SFileAttrib>& vrFileList)
{
	CListCtrl& rListCtrl = GetListCtrl();
	UINT Count=rListCtrl.GetSelectedCount();
	int Item=-1;

	SFileAttrib Temp;

	for(UINT i=0;i<Count;++i)
	{
		Item=rListCtrl.GetNextItem(Item,LVNI_SELECTED);
		Temp.mName=rListCtrl.GetItemText(Item,0);//����
		Temp.mType=rListCtrl.GetItemText(Item,3);//����
		vrFileList.push_back(Temp);
	}
}
/*
lpszFilter ������Ҫ��һ����������ָ���ɹ�ѡ����ļ����ͺ���Ӧ����չ����������ʽ�磺 
"Chart Files (*.xlc)|*.xlc|Worksheet Files (*.xls)|*.xls|Data Files (*.xlc;*.xls)|*.xlc; *.xls|All Files (*.*)|*.*||";�ļ�����˵������չ������ | �ָ���ͬ�������ļ�����չ��������� ; �ָÿ���ļ����ͼ��� | �ָ���ĩβ�� || ָ���� 
*/

void Ctool_files_packetView::OnNullCompress()
{
	mCompress=NULL_COMPRESS;
	mTool.SetOpertion(CPack::NULL_COMPRESS,CNULLCompress::LAST_VERSION);

}
void Ctool_files_packetView::OnLzwCompress()
{
	mCompress=LZW_COMPRESS;
	mTool.SetOpertion(CPack::LZW_COMPRESS,CLzwArithmetic::LAST_VERSION);

}
void Ctool_files_packetView::OnZlibCompress()
{
	mCompress=ZLIB_COMPRESS;
	mTool.SetOpertion(CPack::ZLIB_COMPRESS,CZlibArithmetic::LAST_VERSION);
}


void Ctool_files_packetView::OnUpdateLzwCompress(CCmdUI* vpCmdUI)
{
	if(mCompress==LZW_COMPRESS)
	{
		vpCmdUI->SetCheck(TRUE);
	}
	else
	{
		vpCmdUI->SetCheck(FALSE);
	}
}


void Ctool_files_packetView::OnUpdateZlibCompress(CCmdUI* vpCmdUI)
{
	if(mCompress==ZLIB_COMPRESS)
	{
		vpCmdUI->SetCheck(TRUE);
	}
	else
	{
		vpCmdUI->SetCheck(FALSE);
	}
}


void Ctool_files_packetView::OnUpdateNullCompress(CCmdUI* vpCmdUI)
{
	if(mCompress==NULL_COMPRESS)
	{
		vpCmdUI->SetCheck(TRUE);
	}
	else
	{
		vpCmdUI->SetCheck(FALSE);
	}
}


//��ѹ
//andy
void Ctool_files_packetView::OnNowReleaseFiles(bool isCompare)
{
	list<SFileAttrib> List;
	GetSelected(List);
	//�򿪡����Ϊ��ͨ�öԻ���
	CFileDialog Temp(FALSE,"pak","00",OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT,"�ļ���(*.*)|*.*||");
	Temp.m_ofn.lpstrTitle="��ѹ���ļ���";
	Temp.m_ofn.lpstrInitialDir="d:\\";
	if (Temp.DoModal() == IDOK)
	{//�û�ȷ��
		CString SavePath=Temp.GetPathName();
		string SavePathString=SavePath;
		string Path=GetPathName();
		string PackName;

		list<SFileAttrib>::iterator It;

		
		CWaitThread* pWaitThread = (CWaitThread*)AfxBeginThread(RUNTIME_CLASS(CWaitThread),THREAD_PRIORITY_NORMAL,0,CREATE_SUSPENDED);
		if (pWaitThread != NULL)
		{//���Ի����߳�
			pWaitThread->ResumeThread();
		}
		for(It=List.begin();It!=List.end();++It)
		{
			if(It->mName=="." || It->mName=="..")
			{
				continue;
			}
			
			PackName=Path+m_sPackName;
			string sPathName;
			list<string>::iterator it;
			for (it=mPackPathList.begin(); it!=mPackPathList.end(); ++it)
			{
				if (mPackPathList.size() == 1)
				{
					sPathName ="";
					break;
				}
				if (it == mPackPathList.begin())
				{
					++it;
				}
				sPathName+=(*it);
				sPathName+="\\";
			}
			sPathName+=(It->mName);
			if (isCompare)
			{
				std::string packPath = Path+sPathName;
				if (GetMD5ListFromPack(packPath))
				{
					m_decoPath = SavePathString;
				}
				else
				{
					return;
				}
			}
			if (It->mType == PACK_TYPE)
			{//��pak��
				mTool.Unpack(Path+It->mName);	
			} 
			else if(It->mType == FILE_IN_PACK_TYPE)
			{//pak���е��ļ�
				
				mTool.UnpackFile(PackName, sPathName);
			}
			else if (It->mType == FOLDER_IN_PACK_TYPE)
			{//�����ļ���	
				
				mTool.UnpackFloder(PackName, sPathName);
				
			}
			else
			{//����

			}
		}
		pWaitThread->PostThreadMessage(WM_QUIT, 0, 0);
		if (MessageBox("��ѹ���") == IDOK)
		{
			
		}
		
		mTool.Close(SavePathString);
	}

	
}
//�Ƚ�MD5��
void Ctool_files_packetView::OnReleaseFiles()
{
	OnNowReleaseFiles(false);
}

bool Ctool_files_packetView::CompareMD5()
{
	bool isEqual = true;
	if (m_packFileMD5.size() != m_decoFileMD5.size())
	{
		RtCoreLog().Error("file size is not equal!");
		isEqual = false;
		return isEqual;
	}
	if (m_packFileMD5.empty())
	{
		RtCoreLog().Error("packFileMD5 is empty!");
		isEqual = false;
		return isEqual;
	}
	if (m_decoFileMD5.empty())
	{
		RtCoreLog().Error("decoFileMD5 is empty!");
		isEqual = false;
		return isEqual;
	}
	std::map<std::string,std::string>::iterator it = m_packFileMD5.begin();
	for (;it!=m_packFileMD5.end();++it)
	{
		if (m_decoFileMD5[it->first] != it->second)
		{
			RtCoreLog().Error("file %s md5 is not equal!\n\r",it->first.c_str());
			RtCoreLog().Error("pachmd5 is %s\n\r",it->second.c_str());
			RtCoreLog().Error("decomd5 is %s\n\r",m_decoFileMD5[it->first].c_str());
			isEqual = false;
		}
	}
	return isEqual;
}

void Ctool_files_packetView::OnCheckOverlap()
{
	list<SFileAttrib> List;
	GetSelected(List);

	if (List.empty())
		return;

	SFileAttrib fileAttrib = *(List.begin());
	std::string fullPath = GetPathName(fileAttrib.mName);
	std::transform(fullPath.begin(), fullPath.end(), fullPath.begin(), tolower);
	if (mTool.Open(fullPath, "rb+"))
	{
		CPack* pPack = mTool.GetPackManager()->GetPack(fullPath);
		pPack->CheckOverlap();

		mTool.Close(fullPath);
	}
}

void Ctool_files_packetView::OnCompareMD5()
{
	m_packFileMD5.clear();
	m_decoFileMD5.clear();
	OnNowReleaseFiles(true);
	if (!m_decoPath.empty())
	{
		int pos = m_decoPath.rfind("\\");
		m_decoPath = m_decoPath.substr(0,pos);
		if (GetMD5ListFromdeco(m_decoPath))
		{
			if (CompareMD5())
			{
				MessageBox("Congratulation,compare is right!","(^O^)",MB_OK);
			}
			else
			{
				MessageBox("Be Careful,compare is wrong!","(�ѣߡ�)",MB_OK);
			}
		}
		else
		{
			MessageBox("Be Careful,compare is wrong!","(�ѣߡ�)",MB_OK);
		}
		m_decoPath.clear();
	}
}

bool Ctool_files_packetView::GetMD5ListFromdeco(std::string decoPath)
{
	std::list<std::string> sFileList = RtCoreFile().FindFiles(decoPath.c_str(),1,1);
	if (sFileList.empty())
	{
		return false;
	}
	for (std::list<std::string>::iterator it = sFileList.begin();it!=sFileList.end();++it)
	{
		string name = (*it);
		std::string filepath = decoPath + "\\" + name;
		std::list<std::string> filelist = RtCoreFile().FindFiles(filepath.c_str(),1,1);
		if (!filelist.empty())
		{
			GetMD5ListFromdeco(filepath);
		}
		else
		{
			if (!GetFileMd5(filepath))
			{
				RtCoreLog().Error("EncodeFile %s is wrong!",filepath.c_str());
				return false;
			}
		}
	}
	return true;
}
bool Ctool_files_packetView::GetMD5ListFromPack(std::string path)
{
	std::string packname = path;
	mTool.GetPackManager()->OpenPack(packname,CPackManager::NORMAL_PACK,"rb+");
	CPack* pCurPack = mTool.GetPackManager()->GetPack(packname);		//�õ�һ�����ӿ�
	if(pCurPack)
	{
		std::vector<CPack::SFileInfo*> allFileInfo = pCurPack->GetAllFileInfo();
		std::string sList = "";
		for (std::vector<CPack::SFileInfo*>::iterator it = allFileInfo.begin(); it != allFileInfo.end(); ++it)
		{
			CPack::SFileInfo* pFileInfo = *it;
			std::string buff = ASCIIToHex(pFileInfo->mMD5, 16);
			m_packFileMD5[pFileInfo->mName] = buff;
		}
	}
	else
	{
		mTool.GetPackManager()->ClosePack(packname);
		return false;
	}
	mTool.GetPackManager()->ClosePack(packname);
	return true;
}

bool Ctool_files_packetView::GetFileMd5(std::string filename)
{
	RtMD5 rtMD5;
	if (rtMD5.EncodeFile((char*)filename.c_str()))
	{
		char buff[33];
		memset(buff, 0, 33);
		rtMD5.GetMD5String(buff);
		std::string md5 = buff;
		filename = filename.substr(m_decoPath.size()+1);
		m_decoFileMD5[filename] = md5;
	}
	else
	{
		return false;
	}
	return true;
}

std::string Ctool_files_packetView::ASCIIToHex(char* pInput,int datalen)
{
	stringstream ss;
	int iTemp = 0;
	for (int i = 0; i < datalen; ++i)
	{
		if ((i % 4) == 0)
		{
			if (i > 0)
				ss << hex << setw(8) << setfill('0') << iTemp;
			iTemp = 0x000000ff & (int)pInput[i];
		}
		else
		{
			iTemp <<= 8;
			iTemp += 0x000000ff & (int)pInput[i];
		}
	}
	ss << hex << setw(8) << setfill('0')<<iTemp;

	return ss.str();
}
//�����汾��Ϣ 104->105			ac.ma
void Ctool_files_packetView::OnAdvanceVersion()
{
	list<SFileAttrib> List;
	GetSelected(List);

	//�򿪡��򿪡�ͨ�öԻ���
	CFileDialog Temp(TRUE,"pak",NULL,OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT,"���ļ���(*.pak)|*.pak||");
	Temp.m_ofn.lpstrTitle="����.pak�汾";
	Temp.m_ofn.lpstrInitialDir="d:\\";


	if(Temp.DoModal()==IDOK)
	{//�û�ȷ��
		CString SavePath=Temp.GetPathName();
		string SavePathString=SavePath;		//eeeeee.pak
		string Path=GetPathName();			//Path  d:/
		string FullName;

		if(mTool.IsPack(SavePathString))
		{//����
			//MessageBox("���ܴ��������ļ����Ѿ����ڣ�");
			mTool.Open(SavePathString,"rb+");//��ϣ���Ķ���д��־
		}
		else
		{//������
			if(!mTool.Create(SavePathString))
			{//�޷������ļ���
				MessageBox("�޷������ļ�����");
			}
		}


		list<SFileAttrib>::iterator It;

		for(It=List.begin();It!=List.end();++It)
		{
			if(It->mName=="." || It->mName=="..")
			{
				continue;
			}

			FullName=Path+It->mName;

			if(It->mType==FOLDER_TYPE)
			{//�ļ���
				mTool.Pack(FullName,It->mName,true);
			}
			else if(It->mType==PACK_TYPE)
			{//pack	
				mTool.UpdateToCipherVersion();				//ac.ma
			}
			else
			{//�������
			}

		}
		mTool.Close(SavePathString);

		//ˢ��
		string FullPath=GetPathName();//�õ�·����
		if(FullPath=="")
		{//�õ���ǰ���е�������
			DisplayDrives();
			return;
		}
		FullPath+="*.*";		//��·�������е��ļ�
		//��ʾ�ļ�
		list<SFileAttrib> List;
		GetFilesInDirectory(FullPath.c_str(),List);
		Display(List);
	}
}

//����ļ����ļ���[���������滻]
void Ctool_files_packetView::OnAddFilesToPack()
{
	list<SFileAttrib> List;
	GetSelected(List);

	//�򿪡��򿪡�ͨ�öԻ���
	CFileDialog Temp(TRUE,"pak",NULL,OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT,"���ļ���(*.pak)|*.pak||");
	Temp.m_ofn.lpstrTitle="��ӵ��ļ���";
	Temp.m_ofn.lpstrInitialDir="d:\\";


	if(Temp.DoModal()==IDOK)
	{//�û�ȷ��
		CString SavePath=Temp.GetPathName();
		string SavePathString=SavePath;
		string Path=GetPathName();
		string FullName;

		if(mTool.IsPack(SavePathString))
		{//����
			//MessageBox("���ܴ��������ļ����Ѿ����ڣ�");
			mTool.Open(SavePathString,"rb+");//��ϣ���Ķ���д��־
		}
		else
		{//������
			if(!mTool.Create(SavePathString))
			{//�޷������ļ���
				MessageBox("�޷������ļ�����");
			}
		}


		list<SFileAttrib>::iterator It;

		for(It=List.begin();It!=List.end();++It)
		{
			if(It->mName=="." || It->mName=="..")
			{
				continue;
			}

			FullName=Path+It->mName;

			if(It->mType==FOLDER_TYPE)
			{//�ļ���
				mTool.Pack(FullName,It->mName,true);
			}
			else if(It->mType==FILE_TYPE)
			{//�ļ�
				mTool.Add(FullName,It->mName,true);
			}
			else
			{//�������
			}

		}
		mTool.Close(SavePathString);

		//ˢ��
		string FullPath=GetPathName();//�õ�·����
		if(FullPath=="")
		{//�õ���ǰ���е�������
			DisplayDrives();
			return;
		}
		FullPath+="*.*";		//��·�������е��ļ�
		//��ʾ�ļ�
		list<SFileAttrib> List;
		GetFilesInDirectory(FullPath.c_str(),List);
		Display(List);
	}
}

void Ctool_files_packetView::OnOpenPack()
{
	list<SFileAttrib> List;
	GetSelected(List);

	//�򿪡��򿪡�ͨ�öԻ���
	CFileDialog Temp(TRUE,"pak",NULL,OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT,"���ļ���(*.pak)|*.pak||");
	Temp.m_ofn.lpstrTitle="���ļ���";
	Temp.m_ofn.lpstrInitialDir="d:\\";

	if(Temp.DoModal()==IDOK)
	{//�û�ȷ��
	}

}


#ifdef _DEBUG
void Ctool_files_packetView::AssertValid() const
{
	CListView::AssertValid();
}

void Ctool_files_packetView::Dump(CDumpContext& dc) const
{
	CListView::Dump(dc);
}

Ctool_files_packetDoc* Ctool_files_packetView::GetDocument() const // �ǵ��԰汾��������
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(Ctool_files_packetDoc)));
	return (Ctool_files_packetDoc*)m_pDocument;
}
#endif