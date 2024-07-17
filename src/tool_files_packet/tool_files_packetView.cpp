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

//列表中的图标
const int  FILE_IMAGE_INDEX=0;
const int  FOLDER_IMAGE_INDEX=1;
const int  DRIVE_FIXED_IMAGE_INDEX=2;
const int  PACK_IMAGE_INDEX=3;
const int  UNKNOW_IMAGE_INDEX=4;


const char FOLDER_TYPE[]="文件夹";	//文件夹类型标志
const char FILE_TYPE[]="文件";		//文件类型标志
const char PACK_TYPE[]="文件包";		//文件包类型标志
const char FILE_IN_PACK_TYPE[]="包内文件";//包内文件类型
const char FOLDER_IN_PACK_TYPE[]="包内文件夹";	//文件夹类型标志

const char DRIVE_REMOVABLE_TYPE[]="可移动驱动器";
const char DRIVE_FIXED_TYPE[]="本地驱动器";
const char DRIVE_REMOTE_TYPE[]="网络驱动器";
const char DRIVE_CDROM_TYPE[]="CDROM驱动器";

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
	// TODO: 在此处添加构造代码

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

//显示用的数据
struct SDisplayData 
{ 
	LPCTSTR name; 
	int width; 
	int fmt;
};

static SDisplayData columns[] =
{
	{ _T("名字"),       170,	LVCFMT_LEFT},
	{ _T("原始大小"),   70,		LVCFMT_RIGHT},
	{ _T("包内大小"),   70,		LVCFMT_LEFT},
	{ _T("类型"),       80,		LVCFMT_LEFT},
	{ _T("压缩方式"),   60,		LVCFMT_CENTER},
	{ _T("版本"),       40,		LVCFMT_CENTER},
	{ _T("MD5"),        230,	LVCFMT_LEFT},
	{ _T("修改日期"),   660,	LVCFMT_LEFT},
};

//顺序同上
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

		Temp.mName=It->name;//名字

		//类型
		Temp.mType = ((It->attrib&_A_SUBDIR)!=0) ?  FOLDER_TYPE : FILE_TYPE ;
		//文件修改日期
		Temp.mModifyTime=fox::TimeConvert(It->time_write);


		if(Temp.mType==FILE_TYPE)
		{//进一步判断是否包文件

			//文件大小
			Temp.mSize=fox::UnitConvert(It->size);

			if(mTool.IsPack(Path+Temp.mName))
			{
				//版本
				Temp.mVersion=Convert<string>(mTool.PackVersion(Path+Temp.mName));
				Temp.mType=PACK_TYPE;

				//包时间特别处理
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
				TempString="主版本[";
				TempString+=MainVersion;
				TempString+="] 次版本[";
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
		{//不是root有".."
			IsRoot=false;
		}
	}

	//是root目录，添加一个".."目录
	if(IsRoot)
	{//添加一个".."目录
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
	mTool.Open(vPackName,"rb");//不改写标志

	SFileAttrib Temp;

	vector<CPack::SFileInfo*> Files;
	if(mTool.GetAllFilesInPack(Files))//得到包中所有文件的信息
	{
		vector<CPack::SFileInfo*>::iterator It;
		for(It=Files.begin();It!=Files.end();++It)
		{
			Temp.Clear();
			Temp.mName=(*It)->mName;//名字
			Temp.mType=FILE_IN_PACK_TYPE;//类型
			Temp.mSize=fox::UnitConvert((*It)->mOriginSize);//原始大小
			Temp.mSizeInPack=fox::UnitConvert((*It)->mSizeInPack);//包中大小

			//压缩方式
			switch((*It)->mCompressMode) 
			{
			case CPack::NULL_COMPRESS:
				Temp.mCompressMode="无压缩";
				break;
			case CPack::LZW_COMPRESS:
				Temp.mCompressMode="LZW压缩";
				break;
			case CPack::ZLIB_COMPRESS:
				Temp.mCompressMode="ZLIB压缩方式";
				break;
			default:
				Temp.mCompressMode="未知压缩";
				break;
			}

			Temp.mVersion=fox::Convert<string>((*It)->mCompressVersion);//压缩版本
			Temp.mModifyTime=fox::TimeConvert((*It)->mWriteTime);		//修改时间

			//MD5
			char TempMD5[16*2+1];//MD5_LENGTH=16
			rtMD5PrintString(TempMD5,(unsigned char *)((*It)->mMD5));
			TempMD5[16*2]='\0';//结尾零
			strupr(TempMD5);//大写
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

string Ctool_files_packetView::GetPathName(const string vFile)//得到当前的路径名
{//得到当前路径
	string FullPath;
	list<string>::iterator It;
	for(It=mPathList.begin();It!=mPathList.end();++It)
	{
		FullPath+=(*It);
		FullPath+="\\";
	}

	return FullPath+vFile;
}

void Ctool_files_packetView::Display(list<SFileAttrib>& vrFiles)//显示列表
{
	int i;
	int j;
	CListCtrl& listCtrl = GetListCtrl();//得到CListCtrl控件
	listCtrl.DeleteAllItems();			//清除旧的选项

	list<SFileAttrib>::iterator It;
	for(It=vrFiles.begin(),i=0;It!=vrFiles.end();++It)
	{

		if(It->mName==".")
		{//不需要显示当前目录
			continue;
		}

		//加入文字和图标
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
			case TYPE_COLUMN_INDEX://[类型]
				listCtrl.SetItem(i, j, LVIF_TEXT,It->mType.c_str(), 0, 0, 0, 0);
				break;
			case COMPRESS_COLUMN_INDEX:
				listCtrl.SetItem(i, j, LVIF_TEXT,It->mCompressMode.c_str(), 0, 0, 0, 0);
				break;
			case VERSION_COLUMN_INDEX:
				if(It->mVersion=="-1")
				{
					listCtrl.SetItem(i, j, LVIF_TEXT,"太旧", 0, 0, 0, 0);
				}
				else
				{
					listCtrl.SetItem(i, j, LVIF_TEXT,It->mVersion.c_str(), 0, 0, 0, 0);
				}
				break;
			case MD5_COLUMN_INDEX:
				listCtrl.SetItem(i, j, LVIF_TEXT,It->mMD5.c_str(), 0, 0, 0, 0);
				break;
			case MODIFY_TIME_COLUMN_INDEX://[修改时间]
				listCtrl.SetItem(i, j, LVIF_TEXT,It->mModifyTime.c_str(), 0, 0, 0, 0);
				break;
			default:
				break;
			}
		}
		++i;
	}
}

string Ctool_files_packetView::GetPackPathName(const string vFile)//得到当前的路径名
{//得到当前路径
	string FullPath;
	list<string>::iterator It;
	if(mPackPathList.size()<=1)
	{//第一个是包的名字
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
	{//上一级目录
		mPackPathList.pop_back();
	}
	else
	{//添加到包路径列表中
		mPackPathList.push_back(vPackName);
	}

	if(mPackPathList.empty())
	{
		//MessageBox("显示目录列表中的文件");
		DisplayFiles("");
		return;
	}

	//显示包中文件
	//得到所有的包中文件列表
	list<SFileAttrib> List;
	list<SFileAttrib>::iterator It;
	string Temp;
	string Path;

	Path=GetPathName()+mPackPathList.front();//包文件的全名
	GetFilesInPack(Path,List);

	Path=GetPackPathName("");
	//将所有的文件截掉包中路径部分
	for(It=List.begin();It!=List.end();)
	{
		if(It->mName.find(Path)==0)
		{//是这个路径下的
			It->mName=It->mName.substr(Path.size());
			++It;
		}
		else
		{//不是这个路径下的
			It=List.erase(It);
		}
	}

	//将所有文件分成目录
	set<string> Folders;
	list<SFileAttrib> FilesList;
	list<SFileAttrib> FoldersList;

	SFileAttrib TempAttrib;

	for(It=List.begin();It!=List.end();++It)
	{
		Temp= It->mName;
		Temp=Temp.substr(0,Temp.find_first_of('\\'));

		if(Temp==It->mName)
		{//是文件
			TempAttrib.Clear();
			TempAttrib=*It;
			TempAttrib.mName=Temp;
			TempAttrib.mType=FILE_IN_PACK_TYPE;

			FilesList.push_back(TempAttrib);
		}
		else
		{//是文件夹
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

	//添加".."
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
	{//当前目录，返回
		return;
	}

	if(Path=="..")
	{//上一级目录
		mPathList.pop_back();
	}
	else if(Path=="")
	{
	}
	else 
	{//添加到路径列表中
		mPathList.push_back(Path);
	}

	string FullPath=GetPathName();//得到路径名

	if(FullPath=="")
	{//得到当前所有的驱动器
		DisplayDrives();
		return;
	}

	FullPath+="*.*";		//该路径下所有的文件

	//显示文件
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
	{//只选中了一项//并且是文件夹,或者是本地驱动器
		Item=rListCtrl.GetNextItem(Item,LVNI_SELECTED);
		Name=rListCtrl.GetItemText(Item,0);
		Type=rListCtrl.GetItemText(Item,3);
		if(Type==FOLDER_TYPE || Type==DRIVE_FIXED_TYPE)
		{//文件夹或驱动器类型
			DisplayFiles(Name);
		}
		else if(Type==PACK_TYPE)
		{
			string Temp=Name;
			m_sPackName = Name;
			DisplayPack(Temp);
		}
		else if(Type==FILE_TYPE)
		{//是文件
		}
		else if(Type==FOLDER_IN_PACK_TYPE)
		{//包中文件夹
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
		Temp.mName=rListCtrl.GetItemText(Item,0);//名字
		Temp.mType=rListCtrl.GetItemText(Item,3);//类型
		vrFileList.push_back(Temp);
	}
}
/*
lpszFilter 是最重要的一个参数，它指明可供选择的文件类型和相应的扩展名。参数格式如： 
"Chart Files (*.xlc)|*.xlc|Worksheet Files (*.xls)|*.xls|Data Files (*.xlc;*.xls)|*.xlc; *.xls|All Files (*.*)|*.*||";文件类型说明和扩展名间用 | 分隔，同种类型文件的扩展名间可以用 ; 分割，每种文件类型间用 | 分隔，末尾用 || 指明。 
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


//解压
//andy
void Ctool_files_packetView::OnNowReleaseFiles(bool isCompare)
{
	list<SFileAttrib> List;
	GetSelected(List);
	//打开“另存为”通用对话框
	CFileDialog Temp(FALSE,"pak","00",OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT,"文件　(*.*)|*.*||");
	Temp.m_ofn.lpstrTitle="解压到文件夹";
	Temp.m_ofn.lpstrInitialDir="d:\\";
	if (Temp.DoModal() == IDOK)
	{//用户确认
		CString SavePath=Temp.GetPathName();
		string SavePathString=SavePath;
		string Path=GetPathName();
		string PackName;

		list<SFileAttrib>::iterator It;

		
		CWaitThread* pWaitThread = (CWaitThread*)AfxBeginThread(RUNTIME_CLASS(CWaitThread),THREAD_PRIORITY_NORMAL,0,CREATE_SUSPENDED);
		if (pWaitThread != NULL)
		{//开对话框线程
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
			{//是pak包
				mTool.Unpack(Path+It->mName);	
			} 
			else if(It->mType == FILE_IN_PACK_TYPE)
			{//pak包中的文件
				
				mTool.UnpackFile(PackName, sPathName);
			}
			else if (It->mType == FOLDER_IN_PACK_TYPE)
			{//包内文件夹	
				
				mTool.UnpackFloder(PackName, sPathName);
				
			}
			else
			{//其他

			}
		}
		pWaitThread->PostThreadMessage(WM_QUIT, 0, 0);
		if (MessageBox("解压完成") == IDOK)
		{
			
		}
		
		mTool.Close(SavePathString);
	}

	
}
//比较MD5码
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
				MessageBox("Be Careful,compare is wrong!","(⊙＿⊙)",MB_OK);
			}
		}
		else
		{
			MessageBox("Be Careful,compare is wrong!","(⊙＿⊙)",MB_OK);
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
	CPack* pCurPack = mTool.GetPackManager()->GetPack(packname);		//得到一个包接口
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
//提升版本信息 104->105			ac.ma
void Ctool_files_packetView::OnAdvanceVersion()
{
	list<SFileAttrib> List;
	GetSelected(List);

	//打开“打开”通用对话框
	CFileDialog Temp(TRUE,"pak",NULL,OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT,"包文件　(*.pak)|*.pak||");
	Temp.m_ofn.lpstrTitle="提升.pak版本";
	Temp.m_ofn.lpstrInitialDir="d:\\";


	if(Temp.DoModal()==IDOK)
	{//用户确认
		CString SavePath=Temp.GetPathName();
		string SavePathString=SavePath;		//eeeeee.pak
		string Path=GetPathName();			//Path  d:/
		string FullName;

		if(mTool.IsPack(SavePathString))
		{//存在
			//MessageBox("不能创建，该文件包已经存在！");
			mTool.Open(SavePathString,"rb+");//不希望改动读写标志
		}
		else
		{//不存在
			if(!mTool.Create(SavePathString))
			{//无法创建文件包
				MessageBox("无法创建文件包！");
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
			{//文件夹
				mTool.Pack(FullName,It->mName,true);
			}
			else if(It->mType==PACK_TYPE)
			{//pack	
				mTool.UpdateToCipherVersion();				//ac.ma
			}
			else
			{//不加入包
			}

		}
		mTool.Close(SavePathString);

		//刷新
		string FullPath=GetPathName();//得到路径名
		if(FullPath=="")
		{//得到当前所有的驱动器
			DisplayDrives();
			return;
		}
		FullPath+="*.*";		//该路径下所有的文件
		//显示文件
		list<SFileAttrib> List;
		GetFilesInDirectory(FullPath.c_str(),List);
		Display(List);
	}
}

//添加文件到文件包[无条件的替换]
void Ctool_files_packetView::OnAddFilesToPack()
{
	list<SFileAttrib> List;
	GetSelected(List);

	//打开“打开”通用对话框
	CFileDialog Temp(TRUE,"pak",NULL,OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT,"包文件　(*.pak)|*.pak||");
	Temp.m_ofn.lpstrTitle="添加到文件包";
	Temp.m_ofn.lpstrInitialDir="d:\\";


	if(Temp.DoModal()==IDOK)
	{//用户确认
		CString SavePath=Temp.GetPathName();
		string SavePathString=SavePath;
		string Path=GetPathName();
		string FullName;

		if(mTool.IsPack(SavePathString))
		{//存在
			//MessageBox("不能创建，该文件包已经存在！");
			mTool.Open(SavePathString,"rb+");//不希望改动读写标志
		}
		else
		{//不存在
			if(!mTool.Create(SavePathString))
			{//无法创建文件包
				MessageBox("无法创建文件包！");
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
			{//文件夹
				mTool.Pack(FullName,It->mName,true);
			}
			else if(It->mType==FILE_TYPE)
			{//文件
				mTool.Add(FullName,It->mName,true);
			}
			else
			{//不加入包
			}

		}
		mTool.Close(SavePathString);

		//刷新
		string FullPath=GetPathName();//得到路径名
		if(FullPath=="")
		{//得到当前所有的驱动器
			DisplayDrives();
			return;
		}
		FullPath+="*.*";		//该路径下所有的文件
		//显示文件
		list<SFileAttrib> List;
		GetFilesInDirectory(FullPath.c_str(),List);
		Display(List);
	}
}

void Ctool_files_packetView::OnOpenPack()
{
	list<SFileAttrib> List;
	GetSelected(List);

	//打开“打开”通用对话框
	CFileDialog Temp(TRUE,"pak",NULL,OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT,"包文件　(*.pak)|*.pak||");
	Temp.m_ofn.lpstrTitle="打开文件包";
	Temp.m_ofn.lpstrInitialDir="d:\\";

	if(Temp.DoModal()==IDOK)
	{//用户确认
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

Ctool_files_packetDoc* Ctool_files_packetView::GetDocument() const // 非调试版本是内联的
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(Ctool_files_packetDoc)));
	return (Ctool_files_packetDoc*)m_pDocument;
}
#endif