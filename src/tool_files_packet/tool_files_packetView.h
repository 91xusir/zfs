#pragma once
//********************************************************************
//	created:	2010.03.24 17:50
//	filename: 	tool_files_packetView.h
//	author:		ldr123
//	purpose:	
//********************************************************************
#include "afxcview.h"
#include "pack_tool.h"
 
#include "WaitDlg.h"
#include "WaitThread.h"


struct SFileAttrib{
	SFileAttrib():mName(),mSize(),mSizeInPack(),mCompressMode(),mVersion(),mType(),mModifyTime(),mMD5(){};
	void Clear(){mName.clear();mSize.clear();mSizeInPack.clear();mCompressMode.clear();mVersion.clear();mType.clear();
		mModifyTime.clear(); mMD5.clear();return;};

	string		mName;
	string		mSize;
	string		mSizeInPack;
	string      mCompressMode;
	string		mVersion;
	string		mType;
	string		mModifyTime;
	string		mMD5;
};

class Ctool_files_packetView : public CListView
{
protected: // 仅从序列化创建
	Ctool_files_packetView();
	DECLARE_DYNCREATE(Ctool_files_packetView)

// 属性
public:
	Ctool_files_packetDoc* GetDocument() const;
// 重写
	public:
	virtual void OnDraw(CDC* pDC);  // 重写以绘制该视图
virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
protected:
	virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
	virtual void OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnEndPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnInitialUpdate(); // called first time after construct
public:
	virtual ~Ctool_files_packetView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif
protected:
protected:
	afx_msg void OnAddFilesToPack();
	afx_msg void OnReleaseFiles();
	afx_msg void OnAdvanceVersion();
	afx_msg void OnCompareMD5();
	afx_msg void OnCheckOverlap();
	
	afx_msg void OnUpdateNullCompress(CCmdUI* vpCmdUI);
	afx_msg void OnUpdateLzwCompress(CCmdUI* vpCmdUI);
	afx_msg void OnUpdateZlibCompress(CCmdUI* vpCmdUI);
	
	afx_msg void OnNullCompress();
	afx_msg void OnLzwCompress();
	afx_msg void OnZlibCompress();
	afx_msg void OnOpenPack();
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	DECLARE_MESSAGE_MAP()
private:
	enum ECompressType
	{
		NULL_COMPRESS,
		LZW_COMPRESS,
		ZLIB_COMPRESS,
	};
	
	void			Display(list<SFileAttrib>& vrFiles);//显示列表

	void			GetFilesInPack(const string vPackName,list<SFileAttrib>& vrFiles);
	void			GetFilesInDirectory(const char vPath[],list<SFileAttrib>& vrFiles);
	
	void			DisplayFiles(const char vPath[]);
	void			DisplayDrives();
	void			DisplayPack(const string vPackName);
	


	void			GetSelected(list<SFileAttrib>& vrFileList);
	string			GetPathName(const string vFile="");//得到当前的路径名
	string			GetPackPathName(const string vFile="");//得到当前的路径名

	list<string>	mPathList;
	list<string>	mPackPathList;
	CPackTool		mTool;

	ECompressType   mCompress;
	string m_sPackName;

	std::string m_decoPath;
	std::map<std::string,std::string> m_packFileMD5;
	std::map<std::string,std::string> m_decoFileMD5;
	bool GetMD5ListFromPack(std::string packpath);
	bool GetMD5ListFromdeco(std::string decopath);
	void OnNowReleaseFiles(bool isCompare);
	bool CompareMD5();
	std::string ASCIIToHex(char* pInput,int datalen);
	bool GetFileMd5(std::string filename);
};

#ifndef _DEBUG  // tool_files_packetView.cpp 的调试版本
inline Ctool_files_packetDoc* Ctool_files_packetView::GetDocument() const
   { return reinterpret_cast<Ctool_files_packetDoc*>(m_pDocument); }
#endif

