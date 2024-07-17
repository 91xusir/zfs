#include "ui/rtw_ui.h"

RT_IMPLEMENT_DYNAMIC(RtwHtmlView, RtwWidget, 0, "ui")

RtwHtmlView::CBTranslate* RtwHtmlView::ms_cbTranslateFunc = NULL;
void RtwHtmlView::SetTranslateText(CBTranslate* cb)
{
	ms_cbTranslateFunc = cb;
}

RtwHtmlView::RtwHtmlView():m_path("")
{
	m_WidgetType = wtHtmlView;
	m_pXmlDoc = RT_NEW RtsXmlDoc();
	SetMultiLine(true);
}

RtwHtmlView::~RtwHtmlView()
{
	DEL_ONE(m_pXmlDoc);
}

void RtwHtmlView::Reset()
{
	if(m_pXmlDoc)
		m_pXmlDoc->Cleanup();
	m_Lines.clear();
	m_Linetmp.Reset();
	m_Itemtmp.Reset();
	m_ImageItempath = "";
	return;
}

bool RtwHtmlView::LoadFromFile(const std::string& FileName)
{
	Reset();
	int x = (int)FileName.rfind('/');
	m_ImageItempath = FileName.substr(0,x+1);
	std::string fileString = ReadFileToString(FileName);
	if(fileString == "") return false;
	ReplaceString(fileString);
	if(m_pXmlDoc->ParserString(fileString.c_str()))
	{
		return false;
	}
// 	if(!m_pXmlDoc->ParserFile(FileName.c_str(), 1) == 0)
// 	{
// 		RtCoreLog().Error("[RtwHtmlView::LoadFromFile] Parse file \ %s \ error\n",FileName.c_str());
// 		return false;
// 	}
	if(!_ProcessNode((RtsXmlDoc::NodePtr*)m_pXmlDoc->GetRoot()))
	{
		RtCoreLog().Error("[RtwHtmlView::LoadFromFile] Process file %s error\n",FileName.c_str());
		return false;
	}
// 	if (GetParent())
// 	{
// 		m_rcFrame = GetParent()->GetFrameRect();
// 	}
	CalcClient();
	RefreshPosition();
	return true;
}

bool RtwHtmlView::LoadFromString(const std::string& strHtml)
{
	Reset();
	std::string strHtmltmp = strHtml;
	ReplaceString(strHtmltmp);
	if(m_pXmlDoc->ParserString(strHtmltmp.c_str()))
	{
		return false;
	}
	if(!_ProcessNode((RtsXmlDoc::NodePtr*)m_pXmlDoc->GetRoot()))
	{
		return false;
	}
	//Reset();
	CalcClient();
	RefreshPosition();
	return true;
}

void RtwHtmlView::SetText(const std::string text)
{
	// 删除所有行，并重置相关标志
	//m_Lines.clear();
 	RemoveAllItems(false);
 	m_ViewOffsetLine = 0;
 	m_ViewOffsetCol = 0;
 	m_ViewLastLine = 0;
 	m_CursorPosLine = 0;
 	m_CursorPosCol = 0;
 	m_CursorPosColBak = 0;
 	m_SelStartLine = 0;
 	m_SelStartCol = 0;
 	m_SelEndLine = 0;
 	m_SelEndCol = 0;
 	m_SelText = "";
 	SLine line;
 	line.height = g_workspace.getFontManager()->getDefaultFont()->getFontSize();
 	SItem item;
 	item.type = eit_Text;
 	//item.color = 0xff000000 + rand() *10000 % 0xffffff;
	item.color = m_TextColor;
	std::string stext;
 	int dw=0;
 	int odd=0;
	int num=0;
	bool db=false;
	int textnum=(int)text.size()-1;
	int width=m_rcFrame.right-m_rcFrame.left-6;
	if (textnum>width/6)
	{
		SLine Lineone;
		InsertLine(Lineone);
 		for (size_t s = 0; s < text.size(); ++s)
 		{
			num++;
 			char c = text[s];
 			if (c & 0x80)
 			{
 				dw++;
 			}else
 			{
 				odd++;
 			}
			if (num*6>width-6)
			{
				if (dw%2!=0)
				{
					db=true;
				}
			}
			if (num*6>width||db)
			{
				
					SItem itemtext;
					itemtext.text=stext;
					itemtext.type=eit_Text;
					SLine* plastLine = getLine((int)m_Lines.size()-1);
					plastLine->InsertItem(itemtext);
					CalcLinePosition((int)m_Lines.size()-1);
					stext="";
					SLine Linetemp;
					InsertLine(Linetemp);
//  					if (db)
//  					{
//  						num-=num-1;
//  					}else
//  					{
// 						num-=num;
// 					}
					num=0;
					db=false;
					dw=0;
					odd=0;
					if (c & 0x80)
					{
						dw++;
					}else
					{
						odd++;
					}
					num++;
			}
				stext+=c;
 		}
		SLine* pFirstLine = getLine((int)m_Lines.size()-1);
		item.text = stext;
		pFirstLine->InsertItem(item);
		CalcLinePosition((int)m_Lines.size()-1);
 	}else
 	{
 		item.text = text;
 		line.lineitems.push_back(item);
 		InsertLine(line, -1);
 	}
	
}
bool RtwHtmlView::_ProcessNode(RtsXmlDoc::NodePtr* pNode)
{
	if(!pNode)
		return true;
	const std::string& TagName = pNode->strName;
	if(TagName == "html")
	{
		RtsXmlDoc::NodePtr* pChildNode = pNode->pChildren;
		while(pChildNode)
		{
			if(pChildNode->strName == "body")
				UI_ENSURE_B(_Process_UiHtmlText(pChildNode))
			pChildNode = pChildNode->pNext;
		}
	}
	else
	{

	}
	return true;
}

bool RtwHtmlView::_Process_UiHtmlText(RtsXmlDoc::NodePtr* pNode)
{
	m_Lines.clear();
	RtsXmlDoc::NodePtr* pChildNode = pNode->pChildren;
	while(pChildNode)
	{
		if(pChildNode->strName == "p")
		{
			RtwTextBox::SLine* pLine = _Process_Line(pChildNode);
// 			if(pLine)
// 				m_Lines.push_back(*pLine);

			if (pLine)
			{
				InsertLine(*pLine);
			}
// 			if(pItem)
// 			{
// 				m_Linetmp.InsertItem(pItem);
// 			}
		}
		if (pChildNode->strName == "font")
		{
			RtwTextBox::SLine pLine;
			RtwTextBox::SItem* pItem = _Process_Item(pChildNode);
			if(pItem)
			{
				m_Linetmp.InsertItem(*pItem);
				m_Lines.push_back(m_Linetmp);
			}

			{
// 				pLine.lineitems.push_back(*pItem);
// 				m_Lines.push_back(pLine);
			}
		}
		pChildNode = pChildNode->pNext;
	}
	return true;
}

RtwTextBox::SLine* RtwHtmlView::_Process_Line(RtsXmlDoc::NodePtr *pNode)
{
	m_Linetmp.Reset();
	std::string strAlign = pNode->GetProp_s("align");//位置
	// 	if(strAlign == "center")
	// 	{
	// 		m_Linetmp.
	// 	}
	RtsXmlDoc::NodePtr* pChildNode = pNode->pChildren;
	while(pChildNode)
	{
		if(pChildNode->strName == "font")
		{
			RtwTextBox::SItem* pItem = _Process_Item(pChildNode);
//  			if(pItem)
//  				m_Linetmp.lineitems.push_back(*pItem);
 			RtwPixel itemcolor = pItem->color;
 			RtsXmlDoc::NodePtr* pFontChild = pChildNode->pChildren;
 			while(pFontChild)
 			{
 				if (pFontChild->strName == "img")
 				{
 					RtwTextBox::SItem* pItem = _Process_ImageItem(pFontChild);
					if(pItem)
					{
						m_Linetmp.InsertItem(*pItem);
					}
// 						m_Linetmp.lineitems.push_back(*pItem);
 				}
 				else
 				{
//  					RtwTextBox::SItem* item;
//  					item=_Process_ChildItem(pFontChild);
// 					item->color = itemcolor;
					if(pItem)
					{
						m_Linetmp.InsertItem(*pItem);
					}
// 					m_Linetmp.lineitems.push_back(*item);
 				}
 				pFontChild = pFontChild->pNext;
 			}
		}
		else if (pChildNode->strName == "a")
		{
			RtwTextBox::SItem* pItem = _Process_Item(pChildNode);
// 			if(pItem)
// 				m_Linetmp.lineitems.push_back(*pItem);
			if(pItem)
			{
				m_Linetmp.InsertItem(*pItem);
			}
		}
		else if (pChildNode->strName == "span")
		{
			RtwTextBox::SItem* pItem = _Process_Item(pChildNode);
// 			if(pItem)
// 				m_Linetmp.lineitems.push_back(*pItem);
			if(pItem)
			{
				m_Linetmp.InsertItem(*pItem);
			}
		}
		else if (pChildNode->strName == "img")
		{
			RtwTextBox::SItem* pItem = _Process_ImageItem(pChildNode);
// 			if(pItem)
// 				m_Linetmp.lineitems.push_back(*pItem);
			if(pItem)
			{
				m_Linetmp.InsertItem(*pItem);
			}
		}
// 		else if (pChildNode->strName.find("$") != -1)
// 		{
// 			m_Itemtmp.Reset();
// 			std::string str_result;
// 			int begin = pChildNode->strName.find("$");
// 			int end	  = pChildNode->strName.find(")",begin);
// 			if (end == -1)
// 				end	  = pChildNode->strName.find("]",begin);
// 			if (end != -1 && end > begin)
// 			{
// 				std::string username = pChildNode->strName.substr(begin+2,end-2);
// 				(ms_cbTranslateFunc)(str_result,username.c_str(),NULL);
// 				pChildNode->strName.replace(begin,end-begin+1,str_result.c_str(),str_result.size());
// 				m_Itemtmp.text = pChildNode->strName;
// 				m_Itemtmp.type = RtwTextBox::eit_Text;
// 				RtwTextBox::SItem* pItem = &m_Itemtmp;
// 				if(pItem)
// 					m_Linetmp.lineitems.push_back(*pItem);
// 				m_bWordWrap = true;
// 			}
// 		}
		else
		{
			m_Itemtmp.Reset();
			std::string strText = pChildNode->strName;
			m_Itemtmp.text = strText;
			m_Itemtmp.type = RtwTextBox::eit_Text;
			RtwTextBox::SItem* pItem = &m_Itemtmp;
// 			if(pItem)
// 				m_Linetmp.lineitems.push_back(*pItem);
			if(pItem)
			{
				m_Linetmp.InsertItem(*pItem);
			}
			m_bWordWrap = true;
		}
		pChildNode = pChildNode->pNext;
	}
	return &m_Linetmp;
}

RtwTextBox::SItem* RtwHtmlView::_Process_Item(RtsXmlDoc::NodePtr *pNode)
{
	m_Itemtmp.Reset();
	RtsXmlDoc::NodePtr* pChildNode = pNode->pChildren;
	if (pChildNode)
	{
		std::string strText = pChildNode->GetName();
		int xx = (int)strText.find("$");
		if (xx != -1)
		{
			std::string str_result;
			std::string yy = strText.substr(xx+2,strText.size()-3);
		    (ms_cbTranslateFunc)(str_result,yy.c_str(),NULL);
			m_Itemtmp.text = str_result;
		}
		else
			m_Itemtmp.text = strText;
	}
	if(pNode)
	{
		std::string strColor     = pNode->GetProp_s("color");
		std::string strhyperLink = pNode->GetProp_s("href");
		std::string strClass	 = pNode->GetProp_s("class");
		std::string strFontSize = pNode->GetProp_s("size");
		std::string strFontName = pNode->GetProp_s("name");

		//.memcpy(m_Itemtmp.text,strText,m_Itemtmp.text.size());
		m_Itemtmp.type = RtwTextBox::eit_Text;
		if (!strFontName.empty())
		{
			m_Itemtmp.strFontName = strFontName;
		}

		if (!strFontSize.empty())
		{
			int i = atoi(strFontSize.c_str());
			if (i == 0)
			{
				i = 12;
			}
			m_Itemtmp.nFontSize = i;
		}
		if(strColor.size() > 1)
			m_Itemtmp.color = strtoul(strColor.c_str() + 1, NULL, 16);
		if (strhyperLink.size() > 1)
		{
			m_Itemtmp.hyperLink.FromString(strhyperLink);
			m_Itemtmp.color = 0xaabbcc00;
			m_Itemtmp.bUnderLine = true;
		}
	}
	return &m_Itemtmp;
}


RtwTextBox::SItem* RtwHtmlView::_Process_ChildItem(RtsXmlDoc::NodePtr *pNode)
{
	m_Itemtmp.Reset();
	if(pNode)
	{
		std::string strText = pNode->GetName();
		int xx = (int)strText.find("$");
		if (xx != -1)
		{
			std::string str_result;
			std::string yy = strText.substr(xx+2,strText.size()-3);
			(ms_cbTranslateFunc)(str_result,yy.c_str(),NULL);
			m_Itemtmp.text = str_result;
		}
		else
			m_Itemtmp.text = strText;
		std::string strColor = pNode->GetProp_s("color");
		std::string strhyperLink = pNode->GetProp_s("href");
		std::string strClass	 = pNode->GetProp_s("class");
		m_Itemtmp.text = pNode->strName;
		std::string strFontSize = pNode->GetProp_s("size");
		std::string strFontName = pNode->GetProp_s("name");

		//.memcpy(m_Itemtmp.text,strText,m_Itemtmp.text.size());
		m_Itemtmp.type = RtwTextBox::eit_Text;
		if (!strFontName.empty())
		{
			m_Itemtmp.strFontName = strFontName;
		}

		if (!strFontSize.empty())
		{
			int i = atoi(strFontSize.c_str());
			if (i == 0)
			{
				i = 12;
			}
			m_Itemtmp.nFontSize = i;
		}
		//.memcpy(m_Itemtmp.text,strText,m_Itemtmp.text.size());
		m_Itemtmp.type = RtwTextBox::eit_Text;
		if(strColor.size() > 1)
			m_Itemtmp.color = strtoul(strColor.c_str() + 1, NULL, 16);
		if (strhyperLink.size() > 1)
		{
			m_Itemtmp.hyperLink.FromString(strhyperLink);
			m_Itemtmp.color = 0xaa75FF78;
			m_Itemtmp.bUnderLine = true;
		}
	}
	return &m_Itemtmp;
}

RtwTextBox::SItem* RtwHtmlView::_Process_ImageItem(RtsXmlDoc::NodePtr *pNode)
{
	m_Itemtmp.Reset();
	RtsXmlDoc::NodePtr* pChildNode = pNode->pChildren;
	if (pChildNode)
	{
		std::string strText = pChildNode->GetName();
		int xx = (int)strText.find("$");
		if (xx != -1)
		{
			std::string str_result;
			std::string yy = strText.substr(xx+2,strText.size()-3);
			(ms_cbTranslateFunc)(str_result,yy.c_str(),NULL);
			m_Itemtmp.text = str_result;
		}
		else
			m_Itemtmp.text = strText;
	}
	if(pNode)
	{
		std::string strSrc			= pNode->GetProp_s("src");
		std::string strWidth		= pNode->GetProp_s("width");
		std::string	strHeight		= pNode->GetProp_s("height");
		std::string strSource		= pNode->GetProp_s("source");
		std::string strHref			= pNode->GetProp_s("href");
		std::string strHightImage	= pNode->GetProp_s("hight");
		std::string strHightRect	= pNode->GetProp_s("hightsource");
		std::string strPushedImage	= pNode->GetProp_s("pushed");
		std::string strPushedRect	= pNode->GetProp_s("pushedsource");


		//.memcpy(m_Itemtmp.text,strText,m_Itemtmp.text.size());
		m_Itemtmp.type = RtwTextBox::eit_Image;
		m_Itemtmp.text = pNode->strName;

		RtwImage *pImage, *pHightImage, *pPushedImage;
		pImage = pHightImage = pPushedImage = NULL;

		if(!strSrc.empty())
		{
			std::string imagepath = m_ImageItempath + strSrc;
			pImage = g_workspace.getImageFactory()->createImage(imagepath.c_str());
			if (pImage)
			{
				pImage->SetBlend(true);
				if (!strSource.empty())
				{
					RtwRect rc;
					rc.FromString(strSource);
					pImage->SetSourceRect(rc);
				}
			}
		}

		if(!strHightImage.empty())
		{
			std::string imagepath = m_ImageItempath + strHightImage;
			pHightImage = g_workspace.getImageFactory()->createImage(imagepath.c_str());
			if (pHightImage)
			{
				pHightImage->SetBlend(true);
				if (!strHightRect.empty())
				{
					RtwRect rc;
					rc.FromString(strHightRect);
					pHightImage->SetSourceRect(rc);
				}
			}
		}

		if(!strPushedImage.empty())
		{
			std::string imagepath = m_ImageItempath + strPushedImage;
			pPushedImage = g_workspace.getImageFactory()->createImage(imagepath.c_str());
			if (pPushedImage)
			{
				pPushedImage->SetBlend(true);
				if (!strSource.empty())
				{
					RtwRect rc;
					rc.FromString(strPushedRect);
					pPushedImage->SetSourceRect(rc);
				}
			}
		}

		m_Itemtmp.SetImage(pImage, pHightImage, pPushedImage);

		if (!strHref.empty())
		{
			m_Itemtmp.hyperLink.Type = hlinkClient;
			m_Itemtmp.hyperLink.Text =  strHref;
		}
		else
		{
			m_Itemtmp.hyperLink.Text = "";
		}

		SSize tmpSize;
		if (strWidth.size() > 1 && strHeight.size() > 1)
		{
			tmpSize = SSize(atoi(strWidth.c_str()),atoi(strHeight.c_str()));
		}
		else
		{	
			tmpSize = SSize(31, 31);
		}
		m_Itemtmp.rect.ReSize(tmpSize);
		m_Itemtmp.imgSize = tmpSize;
	}
	return &m_Itemtmp;
}

void RtwHtmlView::DrawForeground(const RtwRect* pClipRect/* = NULL*/)
{
	RtwTextBox::DrawForeground(pClipRect);
}
void RtwHtmlView::DrawBackground(const RtwRect* pClipRect/* = NULL*/)
{
	RtwTextBox::DrawBackground(pClipRect);
}

void RtwHtmlView::AddPath(char* path)
{
	if (m_path == "")
	{
		m_path = path;
	}
	else
		m_path += path;
	m_path += "/";
}

void RtwHtmlView::ReplaceString(std::string& replace)
{
	int pos = (int)replace.find("$");
	std::string str_result;
	while (pos != -1)
	{
		int endpos	  = (int)replace.find(")",pos);
		if (endpos == -1)
			endpos	  = (int)replace.find("]",pos);
		if (endpos != -1 && endpos > pos)
		{
			std::string username = replace.substr(pos+2,endpos-pos-2);
			(ms_cbTranslateFunc)(str_result,username.c_str(),NULL);
			replace.replace(pos,endpos-pos+1,str_result.c_str(),str_result.size());
		}
		pos = (int)replace.find("$");
	}
}

std::string RtwHtmlView::ReadFileToString(const std::string& FileName)
{
	RtArchive* readfile = RtCoreFile().CreateFileReader(FileName.c_str());
 	if (!readfile) return "";
	char* stringBuff = RT_NEW char[readfile->TotalSize()+1];
	readfile->Serialize(stringBuff, readfile->TotalSize());
	stringBuff[readfile->TotalSize()]='\0';
	readfile->Close();
	DEL_ONE(readfile);
	std::string outString = stringBuff;
	delete[] stringBuff;
	return outString;
}