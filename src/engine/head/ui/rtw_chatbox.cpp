#include "ui/rtw_ui.h"

//********************************************************************
//	created:	2010.04.07 10:00
//	filename: 	rtw_chatbox.cpp
//	author:		ldr123
//	purpose:	聊天框
//  other:		局部重构添加注释
//********************************************************************

RtwChatBox::RtwChatBox():
							m_curItem(NULL), m_curPiece(NULL), m_position(0), m_range(0),
							m_ClearSystime(0), m_currentTime(0), m_Scrollnum(0)
{
	CM_MEMPROTECTOR(m_buf, 128)

	m_WidgetType	= wtChatBox;
}

RtwChatBox::RtwChatBox(const RtwChatBox &object)
{
	memcpy(this, &object, sizeof(RtwChatBox));
	CM_MEMPROTECTOR(m_buf, 128)
}

RtwChatBox::~RtwChatBox(){CM_MEMUNPROTECTOR(m_buf)}

int RtwChatBox::GetScrollnum()
{
	return m_Scrollnum;
}
int RtwChatBox::GetScrollPosition()
{
	return m_position;
}

int RtwChatBox::GetScrollRange()
{
	return m_range;
}
void RtwChatBox::SetScrollnum(int num)
{
	m_Scrollnum=num;
}

void RtwChatBox::SetScrollPosition(int pos)
{
    if (pos == GetScrollnum())
    {
        return;
    }

    SLine *pFirstLine = getLine(0);
    UI_ENSURE(pFirstLine);

    SLine *pLastLine =  getLine((int)m_Lines.size() - 1);
    UI_ENSURE(pLastLine);

    int nValueOff = 0;

  	int nHeight = m_ViewRect.getHeight();
  	int nChildHeight = pLastLine->rect.bottom - pFirstLine->rect.top;
  
  	if (nHeight >= nChildHeight)
  	{
  		return;
  	}
  	float step = (float)(nChildHeight-nHeight)/100;
 	int curPos = (pLastLine->rect.bottom-m_ViewRect.bottom)/step;
	if (curPos<=0 && pos>97)
	{
		curPos = 100;
	}

	nValueOff = ((100-pos)-curPos)*step;
	if (pos > GetScrollnum())
	{
		//滚动条往下
		if (pos==100 && curPos<pos)
		{
			nValueOff = m_ViewRect.bottom - pLastLine->rect.bottom;
		}

	}
	else 
	{
		//滚动条往上
		if (pos==0 && curPos>pos)
		{
			nValueOff = m_ViewRect.top - pFirstLine->rect.top;
		}
	}



	OffsetItemPosition(SSize(0, nValueOff), 0);
    SetScrollnum(pos);
}
 
RtwChatBox::Item* RtwChatBox::GetSelectItem()
 {
 	return m_curItem;
 }
 
RtwChatBox::Piece* RtwChatBox::GetSelectPiece()
 {
 	return m_curPiece;
 }

void RtwChatBox::ClearByTime()
{
	++m_currentTime;
	if (m_currentTime >= m_ClearSystime)
	{
		RemoveAllLines();
		m_currentTime = 0;
	}
}

void RtwChatBox::SetFilter(int filter)
{

}

void RtwChatBox::SetClearTime(DWORD time)
{
	m_ClearSystime = time;
}

std::string RtwChatBox::GetLast(int iMask)
{
	if(m_curItem)
		return m_curItem->text;
	else
		return "";
}
//heten		滚屏操作
void RtwChatBox::WDScroll(DWORD delta, int nStartCount, int &nResultCount)
{
	SLine* pLine = getLine((int)m_Lines.size()-1);
	if (!pLine)
	{
		return;
	}	

	if (((int)m_Lines.size()-1) < 3)
	{
		for (int i = 0; i<(int)m_Lines.size(); ++i)
		{
			SLine* plastLine = getLine((int)m_Lines.size()-1-i);
			int count = 0;
			int width = plastLine->rect.getWidth();
			foreach (std::list<SItem>, iterItem, plastLine->lineitems)
			{
				count ++;
				SItem* pItem = &(*iterItem);

				iterItem->rect.left --;
				iterItem->rect.right --;
				if (plastLine->lineitems.size() == count && iterItem->rect.right <= m_rcFrame.left)
				{
					foreach (std::list<SItem>, iter, plastLine->lineitems)
					{
						int i = iter->rect.right-iter->rect.left;
						iter->rect.left += (m_rcFrame.getWidth()+width);
						iter->rect.right = iter->rect.left+i;
					}
					nResultCount = nStartCount+1;
				}
			}
		}
	}
	else
	{
		for (int i = 0; i<3; ++i)
		{
			SLine* plastLine = getLine((int)m_Lines.size()-1-i);
			int count = 0;
			int width = plastLine->rect.getWidth();
			foreach (std::list<SItem>, iterItem, plastLine->lineitems)
			{
				count ++;
				SItem* pItem = &(*iterItem);
				//if (!iterItem->showcount)
				//{
				//	break;
				//}
				iterItem->rect.left --;
				iterItem->rect.right --;
				if (plastLine->lineitems.size() == count && iterItem->rect.right <= m_rcFrame.left)
				{
					foreach (std::list<SItem>, iter, plastLine->lineitems)
					{
						int i = iter->rect.right-iter->rect.left;
						//((*iter).showcount)--;
						//if (!(*iter).showcount)
						//{
						//	iter->rect.left += width;
						//	iter->rect.right = iter->rect.left+i;
						//}
						//else
						{
							iter->rect.left += (m_rcFrame.getWidth()+width);
							iter->rect.right = iter->rect.left+i;
						}			
					}
				}
			}
		}
	}	
}
//end

bool RtwChatBox::InsertChatMessage(
										const std::string& channel, const std::string& sendname, const std::string& text,
										const RtwPixel color, int indexBefore, bool bRefresh, void* param
								)
{
 	return true;
}

void RtwChatBox::Add(int mask, DWORD color, const char* description, char* fromname, char* message, long vFromRegionID, bool ischange)
{
	static const RtwPixel itemColorList[] = 
	{
		0xffffffff, 0xff30e512, 0xff00aaff, 0xffffd851
	};	//道具颜色列表

	std::string strName			= fromname;
	std::string strMessage		= message;
	std::string strDescription	= description;

	int nWordsNum	= 0;	//字符数
	int nImageNum	= 0;	//图片数量
	int nLineWidth	= 0;	//单行长度
	int nChart		= 0;	//单字符
	int nDouble		= 0;	//双字符
	int nHight		= 0;	//chatbox的高度

	int nTaskNameNum = 0;	//任务名字长度
	int nItemNameNum = 0;	//道具名字长度

	size_t nPos		= 0;	//遍历


	bool bTwo			= false;	//是否是双字节
	bool bItemLinkBegin	= false;	//item链接开始
	bool bItemLinkEnd	= false;	//item链接结束

	bool bTaskLinkBegin	= false;	//任务链接开始
	bool bTaskLinkEnd	= false;	//任务链接结束

	bool	bClientMoveToBegin	= false;
	bool	bClientMoveToEnd	= false;
	string	strMoveToLink		= "";
	int		nMoveToNum			= 0;
	int		nMoveX				= 0;
	int		nMoveY				= 0;

	bool bLocalImageBegin = false;	//本地文件开始
	bool bLocalImageEnd   = false;	//本地文件结束

	char szLinkInfo[1024];	//超链接内容
	char szFackToken[128];

	RtwImage* pFaceImage[5][8];	//图片数组


	int		nItemColor;		//链接的颜色

	SItem	itemName;		//名字item
	SItem	itemTemp;		//临时item
	SItem	deTemp;			//description的item
	SItem	itemImage;		//图片item
	SItem	itemLink;		//超链接item

	SLine	lineTemp;		//临时line

	std::string strItemLink		= "";	//道具超链接内容
	std::string strmTaskLink	= "";	//任务超链接内容
	std::string strTempMsg		= "";	//临时用
	std::string strLocalImage	= "";	//本地文件

	itemName.type	= eit_Name;
	itemTemp.type	= eit_Text;
	itemImage.type	= eit_Text;

	switch (mask)	//掩码
	{
	case 2:
	case 4:
	case 8:
	case 32:
	case 64:
	case 256:
	case 1:
		nLineWidth	= m_rcFrame.right-m_rcFrame.left - 10;
		nHight		= m_rcFrame.bottom - 10;
		deTemp.text	= description;
		deTemp.color= RtwPixel(color);
		lineTemp.InsertItem(deTemp);

		itemName.text	=	strName;
		itemName.text	+=	" :";

		for (size_t s = 0; s < strName.size(); ++s)
		{	
			char c = strName[s];
			if (c & 0x80)
			{
				nDouble++;
			}else
			{
				nChart++;
			}
		}

		nChart+=2;
		nWordsNum = (int)itemName.text.size() -1;
		nWordsNum +=(int)deTemp.text.size() - 1;

		itemName.color	= RtwPixel(color);
		itemName.id		= vFromRegionID;
		lineTemp.InsertItem(itemName);
		break;
	case 16:
		nLineWidth		=	m_rcFrame.right-m_rcFrame.left - 10;
		nHight			=	m_rcFrame.bottom - 10;
		itemName.id		= vFromRegionID;
		itemName.color	=	RtwPixel(color);
		itemName.text	=	strName;
		itemName.text	+=	" :";
		lineTemp.InsertItem(itemName);

		for (size_t s = 0; s < strName.size(); ++s)
		{	
			char c = strName[s];
			if (c & 0x80)
			{
				nDouble++;
			}else
			{
				nChart++;
			}
		}

		nChart += 2;
		nWordsNum = (int)itemName.text.size() -1;
		break;
	case 128:
		nLineWidth	= m_rcFrame.right-m_rcFrame.left - 10;
		nHight		= m_rcFrame.bottom - 10;
		break;
	}

	InsertLine(lineTemp);	//插入临时行

	while (nPos < strMessage.size())
	{
		if (
			strMessage[nPos]	== '<'
			&&  strMessage[nPos+1]	== 'm' 
			&&  strMessage[nPos+2]	== 'o'
			&&  strMessage[nPos+3]	== 'v'
			&&  strMessage[nPos+4]	== 'e'
			&&  strMessage[nPos+5]	== 't'
			&&  strMessage[nPos+6]	== 'o'
			&&  strMessage[nPos+7]	== '>'
			&&  !bClientMoveToBegin
			)
		{
			//道具链接
			bClientMoveToBegin	= true;
			nPos += 8;
		}

		if (
			strMessage[nPos]	== '<'
			&&  strMessage[nPos+1]	== 'l' 
			&&  strMessage[nPos+2]	== 'o'
			&&  strMessage[nPos+3]	== 'c'
			&&  strMessage[nPos+4]	== 'a'
			&&  strMessage[nPos+5]	== 'l'
			&&  strMessage[nPos+6]	== 'f'
			&&  strMessage[nPos+7]	== 'i'
			&&  strMessage[nPos+8]	== 'l'
			&&  strMessage[nPos+9]	== 'e'
			&&  strMessage[nPos+10]	== ':'
			&&  strMessage[nPos+11]	== '/'
			&&  strMessage[nPos+12]	== '/'
			&&  !bLocalImageBegin
			)
		{
			//道具链接
			bLocalImageBegin	= true;
			nPos += 13;
		}

		if (
			strMessage[nPos]		== 'i'
			&&  strMessage[nPos+1]	== 't' 
			&&  strMessage[nPos+2]	== 'e'
			&&  strMessage[nPos+3]	== 'm'
			&&  strMessage[nPos+4]	== '['
			&&  !bItemLinkBegin
			)
		{
			//道具链接
			bItemLinkBegin	= true;
			nPos += 5;
		}
		else if (
			strMessage[nPos]		== 't'
			&&  strMessage[nPos+1]	== 'a' 
			&&  strMessage[nPos+2]	== 's'
			&&  strMessage[nPos+3]	== 'k'
			&&  strMessage[nPos+4]	== '['
			&&  bItemLinkBegin
			)
		{
			//任务链接
			bTaskLinkBegin = true;
			nPos += 5;
		}

		if (
			strMessage[nPos]   == '^'
			&&  strMessage[nPos+1] == '-'
			&&  strMessage[nPos+2] == 'f'
			&&  strMessage[nPos+3] == 'a'
			&&  strMessage[nPos+4] == 'c'
			&&  strMessage[nPos+5] == 'e'
			)
		{
			//表情
			nPos += 6;
			int x = 0; 
			int y = 0;
			char faceToken;

			//横纵坐标
			x = (strMessage[nPos]-'0')*10 + (strMessage[nPos+1]-'0');
			nPos += 2;

			faceToken = strMessage[nPos];
			nPos++;

			y = (strMessage[nPos]-'0')*10 + (strMessage[nPos+1]-'0');
			nPos += 2;

			if ((x==4 && y==7 && itemImage.type==eit_Text) || faceToken!='_')
			{
				//无效图片
				continue;
			}

			rt2_sprintf(szFackToken, "ui\\face\\%d.gif", x*8+y);
			pFaceImage[x][y] = g_workspace.getImageFactory()->createImage(szFackToken);

			if (!pFaceImage[x][y])
			{
				continue;
			}

			itemImage.type = eit_Image;
			itemImage.SetImage(pFaceImage[x][y]);
			itemImage.imgSize = SSize(31, 31);

			nImageNum++;

			SLine *pLastLine = getLine((int)m_Lines.size() - 1);
			if ((pLastLine->rect.getWidth() + itemImage.imgSize.width > nLineWidth) && ischange)
			{
				if (pLastLine)
				{
					SLine lineNew;
					InsertLine(lineNew); //插入空行

					pLastLine = getLine((int)m_Lines.size() - 1);
					pLastLine->InsertItem(itemImage);
					CalcLinePosition((int)m_Lines.size() - 1);

					strTempMsg = "";
					nImageNum  = 0;
					nWordsNum  = 0;
				}
			}
			else
			{
				pLastLine->InsertItem(itemImage);
				nWordsNum  = 0;
			}
			CalcLinePosition((int)m_Lines.size() - 1);
		}
		else if (bLocalImageBegin)
		{
			if (
				strMessage[nPos]	== 'l'
				&&  strMessage[nPos+1]	== 'o' 
				&&  strMessage[nPos+2]	== 'c'
				&&  strMessage[nPos+3]	== 'a'
				&&  strMessage[nPos+4]	== 'l'
				&&  strMessage[nPos+5]	== 'f'
				&&  strMessage[nPos+6]	== 'i'
				&&  strMessage[nPos+7]	== 'l'
				&&  strMessage[nPos+8]	== 'e'
				&&  strMessage[nPos+9]	== '/'
				&&  strMessage[nPos+10]	== '>'
				&&  !bLocalImageEnd
				)
			{
				//本地图片
				bLocalImageEnd	= true;
				bLocalImageBegin = false;
				nPos += 11;
			}

			if (bLocalImageBegin)
			{
				strLocalImage += strMessage[nPos];
				if (strMessage[nPos] & 0x80)
				{
					//汉字
					nPos++;
					strLocalImage += strMessage[nPos];
					nItemNameNum += 2;
				}
				else
				{
					nItemNameNum++;
				}
				nPos++;
			}

			if (bLocalImageEnd)
			{
				rt2_sprintf(szFackToken, "%s", strLocalImage.c_str());
				RtwImage *pLocalImage = g_workspace.getImageFactory()->createImage(szFackToken);

				if (!pLocalImage)
				{
					continue;
				}

				pLocalImage->SetBlend(true);
				itemImage.type = eit_Image;
				itemImage.SetImage(pLocalImage);
				itemImage.imgSize = pLocalImage->getRenderImage()->getSize();

				nImageNum++;

				SLine *pLastLine = getLine((int)m_Lines.size() - 1);
				if ((pLastLine->rect.getWidth() + itemImage.imgSize.width > nLineWidth) && ischange)
				{
					if (pLastLine)
					{
						SLine lineNew;
						InsertLine(lineNew); //插入空行

						pLastLine = getLine((int)m_Lines.size() - 1);
						pLastLine->InsertItem(itemImage);
						CalcLinePosition((int)m_Lines.size() - 1);

						strTempMsg = "";
						nImageNum  = 0;
						nWordsNum  = 0;
					}
				}
				else
				{
					pLastLine->InsertItem(itemImage);
					nWordsNum  = 0;
				}
				CalcLinePosition((int)m_Lines.size() - 1);
				bItemLinkEnd = false;
				strItemLink = "";
			}
		}
		else if (bItemLinkBegin)
		{
			//道具链接
			if (
				strMessage[nPos]		== ']'
				&&  strMessage[nPos+1]	== 'm' 
				&&  strMessage[nPos+2]	== 'e'
				&&  strMessage[nPos+3]	== 't'
				&&  strMessage[nPos+4]	== 'i'
				&&  bItemLinkBegin
				)
			{
				//道具链接结束
				bItemLinkEnd = true;
				bItemLinkBegin = false;
				nPos += 5;
			}

			if (bItemLinkBegin)
			{
				strItemLink += strMessage[nPos];
				if (strMessage[nPos] & 0x80)
				{
					//汉字
					nPos++;
					strItemLink += strMessage[nPos];
					nItemNameNum += 2;
				}
				else
				{
					nItemNameNum++;
				}
				nPos++;
			}

			if (bItemLinkEnd)
			{
				//解析道具id,颜色,道具名
				sscanf(strItemLink.c_str(), "%d^)!%s", &nItemColor, &szLinkInfo);
				if (strlen(szLinkInfo) == 0)
				{
					continue;
				}

				bool bIsName = true;
				int i = 0;

				std::string tmpItemName = "";
				std::string tmpLinkInfo = "";

				while (i < (int)string(szLinkInfo).size())
				{
					if (szLinkInfo[i]=='^' && szLinkInfo[i+1]==')' && szLinkInfo[i+2]=='!')
					{
						bIsName = false;
						i += 3;
					}

					if (bIsName)
					{
						tmpItemName += szLinkInfo[i];
					}
					else
					{
						tmpLinkInfo += szLinkInfo[i];
					}
					i++;
				}

				SHyperLink tmpLink;

				//道具链接
				tmpLink.childType = hlinkItem;
				tmpLink.Type = hlinkClient;

				rt2_sprintf(szLinkInfo, "//showItemInfo %s", tmpLinkInfo.c_str());
				tmpLink.Text = szLinkInfo;

				itemLink.type =eit_Text;
				itemLink.text = "[";
				itemLink.text += tmpItemName.c_str();
				itemLink.text += "]";
				itemLink.color = itemColorList[nItemColor%4];
				itemLink.hyperLink = tmpLink;
				nWordsNum += (int)itemLink.text.size();

				SLine *pLastLine = getLine((int)m_Lines.size() - 1);
				if ((pLastLine->rect.getWidth() + ((int)itemLink.text.size()*itemLink.nFontSize)/2 > nLineWidth) && ischange)
				{
					string strTemp = itemLink.text;
					int num		= (nLineWidth - pLastLine->rect.getWidth())/itemLink.nFontSize; //得到不需要换行的个数
					num <<= 1;

					int iHead = strTemp[num] & 0xff;  
					int iTail = strTemp[num+1] & 0xff;  
					//gb2312编码
					if(iHead>=0xa1 && iHead<=0xf7 && iTail>=0xa1 && iTail<=0xfe) 
					{
						num--;
					}

					itemLink.text = strTemp.substr(0, num);

					pLastLine->InsertItem(itemLink);
					CalcLinePosition((int)m_Lines.size() - 1);

					SLine lineNew;
					InsertLine(lineNew); //插入空行
					itemLink.text = strTemp.substr(num);
					SLine *pLastLine = getLine((int)m_Lines.size() - 1);
					pLastLine->InsertItem(itemLink);

					strTempMsg = "";
					nWordsNum  = nItemNameNum;
					nItemNameNum = 0;
				}
				else
				{
					pLastLine->InsertItem(itemLink);
					nItemNameNum = 0;
					nWordsNum  = 0;
				}

				CalcLinePosition((int)m_Lines.size() - 1);

				bItemLinkEnd = false;

				strItemLink = "";
			}
		}
		else if (bClientMoveToBegin)
		{
			//寻路链接
			if (
				strMessage[nPos]		== '<'
				&&  strMessage[nPos+1]	== '/' 
				&&  strMessage[nPos+2]	== 'o'
				&&  strMessage[nPos+3]	== 't'
				&&  strMessage[nPos+4]	== 'e'
				&&  strMessage[nPos+5]	== 'v'
				&&  strMessage[nPos+6]	== 'o'
				&&  strMessage[nPos+7]	== 'm'
				&&  strMessage[nPos+8]	== '>'
				&&  bClientMoveToBegin
				)
			{
				//寻路链接结束
				bClientMoveToEnd   = true;
				bClientMoveToBegin = false;
				nPos += 9;
			}

			if (bClientMoveToBegin)
			{
				strMoveToLink += strMessage[nPos];
				if (strMessage[nPos] & 0x80)
				{
					//汉字
					nPos++;
					strMoveToLink += strMessage[nPos];
					nMoveToNum += 2;
				}
				else
				{
					nMoveToNum++;
				}
				nPos++;
			}

			if (bClientMoveToEnd)
			{
				//解析移动的坐标
				char buf[128];
				sscanf(strMoveToLink.c_str(), "%d,%d,%s", &nMoveX, &nMoveY, buf);

				SHyperLink tmpLink;

				//道具链接
				tmpLink.childType = hlinkItem;
				tmpLink.Type = hlinkClient;

				rt2_sprintf(szLinkInfo, "//clientmoveto %d %d", nMoveX, nMoveY);
				tmpLink.Text = szLinkInfo;

				itemLink.type =eit_Text;
				itemLink.text = "[";
				itemLink.text += buf;
				itemLink.text += "]";
				itemLink.color = 0xaabbcc00;
				itemLink.bUnderLine = true;
				itemLink.hyperLink = tmpLink;

				nWordsNum += strlen(buf);

				SLine *pLastLine = getLine((int)m_Lines.size() - 1);
				if ((pLastLine->rect.getWidth() + ((int)itemLink.text.size()*itemLink.nFontSize)/2 > nLineWidth))
				{
					string strTemp = itemLink.text;
					int num		= (nLineWidth - pLastLine->rect.getWidth())/itemLink.nFontSize; //得到不需要换行的个数
					num <<= 1;

					int iHead = strTemp[num] & 0xff;  
					int iTail = strTemp[num+1] & 0xff;  
					//gb2312编码
					if(iHead>=0xa1 && iHead<=0xf7 && iTail>=0xa1 && iTail<=0xfe) 
					{
						num--;
					}

					itemLink.text = strTemp.substr(0, num);

					pLastLine->InsertItem(itemLink);
					CalcLinePosition((int)m_Lines.size() - 1);

					SLine lineNew;
					InsertLine(lineNew); //插入空行
					itemLink.text = strTemp.substr(num);
					SLine *pLastLine = getLine((int)m_Lines.size() - 1);
					pLastLine->InsertItem(itemLink);

					strTempMsg = "";
					nWordsNum  = nItemNameNum;
					nMoveToNum = 0;
				}
				else
				{
					pLastLine->InsertItem(itemLink);
					nMoveToNum = 0;
					nWordsNum  = 0;
				}

				CalcLinePosition((int)m_Lines.size() - 1);

				bClientMoveToEnd = false;

				strMoveToLink = "";

			}
		}
		else if (bTaskLinkBegin)
		{
			//任务链接开启
			bTaskLinkBegin = false; //暂未开放任务链接功能
		}
		else 
		{
			SLine *pLastLine = getLine((int)m_Lines.size() -1);

			//普通文本
			strTempMsg += strMessage[nPos];
			if (strMessage[nPos] & 0x80)
			{
				nPos++;
				nWordsNum += 2;
				strTempMsg += strMessage[nPos];
			}
			else
			{
				nWordsNum += 1;
			}
			nPos++;

			if (pLastLine->rect.getWidth() + (int)strTempMsg.size()*6+12 > nLineWidth && ischange)
			{
				SItem itemText;
				itemText.text = strTempMsg;
				itemText.type = eit_Text;
				itemText.color= RtwPixel(color);

				pLastLine->InsertItem(itemText);

				CalcLinePosition((int)m_Lines.size()-1);

				SLine lineTemp;
				InsertLine(lineTemp);

				nWordsNum = 0;
				strTempMsg = "";
			}
		}

		if (!strTempMsg.empty())
		{
			SLine *pLastLine = getLine((int)m_Lines.size() -1);

			itemTemp.text = strTempMsg;
			itemTemp.type = eit_Text;
			itemTemp.color= RtwPixel(color);
			pLastLine->InsertItem(itemTemp);
			nWordsNum = 0;

			CalcLinePosition((int)m_Lines.size() - 1);
			strTempMsg = "";
		}
	}

	CalcLinePosition((int)m_Lines.size()-1);	

	SLine* pFirstLine = getLine((int)m_Lines.size()-1);
	if (pFirstLine->rect.bottom > nHight)
	{
		int nMoveHight = 0;

		if (nHight - pFirstLine->rect.bottom >= -12)
		{
			nMoveHight = -13;
		}
		else
		{
			nMoveHight = nHight-pFirstLine->rect.bottom;
		}

		OffsetItemPosition(SSize(0, nMoveHight), 0);
	}
}
