#pragma once
//********************************************************************
//	created:	2010.03.24 17:50
//	filename: 	tool_files_packetDoc.h
//	author:		ldr123
//	purpose:	
//********************************************************************
class Ctool_files_packetDoc : public CDocument
{
protected: // 仅从序列化创建
	Ctool_files_packetDoc();
	DECLARE_DYNCREATE(Ctool_files_packetDoc)
public:
	virtual BOOL OnNewDocument();
	virtual void Serialize(CArchive& ar);
public:
	virtual ~Ctool_files_packetDoc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif
protected:
	DECLARE_MESSAGE_MAP()
};