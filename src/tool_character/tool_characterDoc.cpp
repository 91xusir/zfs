// tool_characterDoc.cpp :  Ctool_characterDoc ���ʵ��
//

#include "stdafx.h"
#include "tool_character.h"

#include "tool_characterDoc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// Ctool_characterDoc

IMPLEMENT_DYNCREATE(Ctool_characterDoc, CDocument)

BEGIN_MESSAGE_MAP(Ctool_characterDoc, CDocument)
END_MESSAGE_MAP()


// Ctool_characterDoc ����/����

Ctool_characterDoc::Ctool_characterDoc()
{
	// TODO: �ڴ����һ���Թ������

}

Ctool_characterDoc::~Ctool_characterDoc()
{
}

BOOL Ctool_characterDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

	// TODO: �ڴ�������³�ʼ������
	// (SDI �ĵ������ø��ĵ�)

	return TRUE;
}




// Ctool_characterDoc ���л�

void Ctool_characterDoc::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{
		// TODO: �ڴ���Ӵ洢����
	}
	else
	{
		// TODO: �ڴ���Ӽ��ش���
	}
}


// Ctool_characterDoc ���

#ifdef _DEBUG
void Ctool_characterDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void Ctool_characterDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG


// Ctool_characterDoc ����
