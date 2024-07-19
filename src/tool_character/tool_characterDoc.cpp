// tool_characterDoc.cpp :  Ctool_characterDoc 类的实现
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


// Ctool_characterDoc 构造/析构

Ctool_characterDoc::Ctool_characterDoc()
{
	// TODO: 在此添加一次性构造代码

}

Ctool_characterDoc::~Ctool_characterDoc()
{
}

BOOL Ctool_characterDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

	// TODO: 在此添加重新初始化代码
	// (SDI 文档将重用该文档)

	return TRUE;
}




// Ctool_characterDoc 序列化

void Ctool_characterDoc::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{
		// TODO: 在此添加存储代码
	}
	else
	{
		// TODO: 在此添加加载代码
	}
}


// Ctool_characterDoc 诊断

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


// Ctool_characterDoc 命令
