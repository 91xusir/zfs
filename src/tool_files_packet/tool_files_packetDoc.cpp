//********************************************************************
//	created:	2010.03.24 17:50
//	filename: 	tool_files_packetDoc.cpp
//	author:		ldr123
//	purpose:	
//********************************************************************
#include "stdafx.h"
#include "tool_files_packet.h"
#include "tool_files_packetDoc.h"

IMPLEMENT_DYNCREATE(Ctool_files_packetDoc, CDocument)
BEGIN_MESSAGE_MAP(Ctool_files_packetDoc, CDocument)
END_MESSAGE_MAP()

Ctool_files_packetDoc::Ctool_files_packetDoc(){}
Ctool_files_packetDoc::~Ctool_files_packetDoc(){}

BOOL Ctool_files_packetDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;
	return TRUE;
}

void Ctool_files_packetDoc::Serialize(CArchive& ar){}

#ifdef _DEBUG
void Ctool_files_packetDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void Ctool_files_packetDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif 