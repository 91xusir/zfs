#pragma once
//********************************************************************
//	created:	2010.03.24 17:50
//	filename: 	tool_files_packet.h
//	author:		ldr123
//	purpose:	
//********************************************************************
#ifndef __AFXWIN_H__
	#error �ڰ������� PCH �Ĵ��ļ�֮ǰ������stdafx.h�� 
#endif

#include "resource.h"       // ������

class Ctool_files_packetApp : public CWinApp
{
public:
	Ctool_files_packetApp();
public:
	virtual BOOL InitInstance();
	DECLARE_MESSAGE_MAP()
};
extern Ctool_files_packetApp theApp;