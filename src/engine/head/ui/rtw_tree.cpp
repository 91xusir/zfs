#include "ui/rtw_ui.h"

//********************************************************************
//	created:	2010.04.07 10:00
//	filename: 	rtw_tree.cpp
//	author:		ldr123
//	purpose:	树形结构
//  other:		局部重构添加注释
//********************************************************************

RtwTree::Item::Item()
{
	TextColor		= RtwPixel(0xffffffff);
	extTextColor	= RtwPixel(0xffffffff);
	Param1			= NULL;
	Param2			= NULL;
	bDraw			= false;
	bExpanded		= false;
	pNext			= NULL;
	pParent			= NULL;
	pFirstChild		= NULL;
	bTrack			= false;
	index			= 0;
	extText			= "";
	id				= 0;

	m_nBeforeNum    = 0;
	m_nAfterNum     = 0;

	for (int i=0; i<MAXIMAGENUM; ++i)
	{
		m_pBeforeImg[i] = NULL;
		m_pAfterImg[i] = NULL;
	}
}

RtwTree::Item::~Item()
{
	for (int i=0; i<MAXIMAGENUM; ++i)
	{
		DROP_TRUI_IMAGE(m_pBeforeImg[i]);
		DROP_TRUI_IMAGE(m_pAfterImg[i]);
	}
}

int RtwTree::Item::PickItemImageIndex(int x, int y)
{
	for (int i=0; i<m_nBeforeNum; ++i)
	{
		if (rcBeforeImage[i].IsContain(x, y))
		{
			return i;
		}
	}

	for (int j=0; j<m_nAfterNum; ++j)
	{
		if (rcAfterImage[j].IsContain(x, y))
		{
			return j+m_nBeforeNum;
		}
	}

	return -1;
}

//根据鼠标获取hint文本，这个hint属于图片的
bool RtwTree::Item::getHintFromPos(int x, int y, string &strResult)
{
	strResult = "";

	int nIndex = PickItemImageIndex(x, y);
	if (nIndex == -1)
	{
		return false;
	}

	if (nIndex < m_nBeforeNum)
	{
		//before
		strResult = m_strBeforeHint[nIndex];
	}
	else
	{
		//after
		strResult = m_strAfterHint[nIndex-m_nBeforeNum];
	}

	//
	return true;
}

//更新item状态图及hint
void RtwTree::Item::UpdateImage(int index, string strFileImg, const string &strHint)
{
	int maxSize = m_nAfterNum+m_nBeforeNum;
	if (index >= maxSize)
	{
		return;
	}

	RtwImage *pImg = g_workspace.getImageFactory()->createImage(strFileImg);
	if (!pImg)
	{
		return;
	}

	if (index < m_nBeforeNum)
	{
		DROP_TRUI_IMAGE(m_pBeforeImg[index]);
		m_pBeforeImg[index] = pImg;
		GRAB_TRUI_IMAGE(m_pBeforeImg[index]);

		if (strHint != "")
		{
			m_strBeforeHint[index] = strHint;
		}
	}
	else
	{
		DROP_TRUI_IMAGE(m_pAfterImg[m_nAfterNum-(maxSize-index)]);
		m_pAfterImg[m_nAfterNum-(maxSize-index)] = pImg;
		GRAB_TRUI_IMAGE(m_pAfterImg[m_nAfterNum-(maxSize-index)]);

		if (strHint != "")
		{
			m_strAfterHint[m_nAfterNum-(maxSize-index)] = strHint;
		}
	}
}

RT_IMPLEMENT_DYNAMIC(RtwTree, RtwWidget, 0, "ui")

RtwTree::RtwTree()
:m_pRootItem(NULL),		//根节点
m_pExpandImage(NULL),	//展开时图片
m_pShrinkImage(NULL),	//合并时图片
m_pSelectItem(NULL)	//被选中的item
{

}

RtwTree::~RtwTree()
{
	if( m_pRootItem )
	{
		DEL_ONE(m_pRootItem);
		m_pRootItem = NULL;
	}
}

//释放
void RtwTree::OnBeforeDestroy()
{
	_ResetSelf();

	DeleteAllItems();

	DROP_TRUI_IMAGE(m_pExpandImage);
	DROP_TRUI_IMAGE(m_pShrinkImage);
	DROP_TRUI_IMAGE(m_pCoverImage);

	RtwWidget::OnBeforeDestroy();
}

void RtwTree::Reset()
{
	RtwWidget::Reset();
	_ResetSelf();
}

//创建根节点
void RtwTree::_CreateRootItem()
{
	if (!m_pRootItem)
	{
		m_pRootItem			=	RT_NEW Item();
		m_pRootItem->bDraw	=	false;
		m_pRootItem->text	=	"$Root";
	}
}

bool RtwTree::Create(DWORD Id)
{
	if (!RtwWidget::Create(Id))
		return false;

	_ResetSelf();
	_CreateRootItem();

	EvMouseLDown	+= RTW_CALLBACK(this, RtwTree, OnMouseLDown_This);
	EvMouseRDown	+= RTW_CALLBACK(this, RtwTree, OnMouseRDown_This);
	//EvMouseMove		+= RTW_CALLBACK(this, RtwTree, OnMouseMove_This);

	m_pExpandImage = g_workspace.getImageFactory()->createImage("ui/textures/tree_icon2.tga");
	m_pExpandImage->SetSourceRect(RtwRect(0,0,16,16)); 
	m_pExpandImage->SetBlend(true);

	m_pShrinkImage = g_workspace.getImageFactory()->createImage("ui/textures/tree_icon1.tga");
	m_pShrinkImage->SetSourceRect(RtwRect(0,0,16,16)); 
	m_pShrinkImage->SetBlend(true);

	m_pCoverImage = g_workspace.getImageFactory()->createImage("ui/textures/ui_lightBar.tga");
	m_pCoverImage->SetBlend(true);	

	return true;
}

void RtwTree::InitItemImage(int nBeforeNum, int nAfterNum, SSize beforeSize, SSize afterSize )
{
	//设置默认hint
	SetHint(g_workspace.getDefaultHint());

	m_nBeforeNum = nBeforeNum;
	m_nAfterNum  = nAfterNum;
	m_sizeBefore = beforeSize;
	m_sizeAfter  = afterSize;
}

//更新某个item的某个位置的图片及hint
void RtwTree::UpdateItemImage(Item *pItem, int nIndex, string imgFile, string strHint /* =  */)
{
	UI_ENSURE(pItem);

	if (FindTreeItemByID(pItem->id) != pItem)
	{
		//不是自己的子节点
		return;
	}

	//找到item,更新image
	pItem->UpdateImage(nIndex, imgFile, strHint);

	//更新图片了,重新显示
	CalcClient();
}

//偏移
void RtwTree::OnOffset(const SSize& size)
{
	RtwWidget::OnOffset(size);

	//从第一个item开始移动
	Item* pItem = getFirstItem();
	while (pItem)
	{
		pItem->rcFunctionImage.Offset(size.width, size.height);
		pItem->rcText.Offset(size.width, size.height);

		//扩展图片
		if (pItem->m_nBeforeNum> 0&& pItem->m_nBeforeNum<=MAXIMAGENUM)
		{
			for (int i=0; i<pItem->m_nBeforeNum; ++i)
			{
				pItem->rcBeforeImage[i].Offset(size.width, size.height);
			}
		}

		if (pItem->m_nAfterNum> 0&& pItem->m_nAfterNum<=MAXIMAGENUM)
		{
			for (int j=0; j<pItem->m_nAfterNum; ++j)
			{
				pItem->rcAfterImage[j].Offset(size.width, size.height);
			}
		}

		pItem = getNextItem(pItem);
	}

	//移动后重新显示
	CalcClient();
}

//计算子组件矩形
//计算整个tree所占用的矩形区域
RtwRect RtwTree::CalcChildrenRect()
{
	RtwRect rc	= m_rcClient;
	Item* pItem = getFirstItem();
	while (pItem)
	{
		//可绘制
		if (pItem->bDraw)
		{
			//扩充rect

			//扩展图片
			if (pItem->m_nBeforeNum> 0&& pItem->m_nBeforeNum<=MAXIMAGENUM)
			{
				for (int i=0; i<pItem->m_nBeforeNum; ++i)
				{
					rc.Expand(pItem->rcBeforeImage[i]);
				}
			}

			if (pItem->m_nAfterNum> 0&& pItem->m_nAfterNum<=MAXIMAGENUM)
			{
				for (int j=0; j<pItem->m_nAfterNum; ++j)
				{
					rc.Expand(pItem->rcAfterImage[j]);
				}
			}

			rc.Expand(pItem->rcFunctionImage);
			rc.Expand(pItem->rcText);
		}

		pItem = getNextItem(pItem);
	}
	return rc;
}

//绘制前景
void RtwTree::DrawForeground(const RtwRect* pClipRect/* = NULL*/)
{
	RtwRect rcAfterClip = m_rcClient;
	if (pClipRect)
		rcAfterClip.ClipBy(*pClipRect);

	//有选中的item,item可绘制,item没有子item

	//Modify Peter 2010.12.30
	if (m_pSelectItem && m_pSelectItem->bDraw
#ifndef _DLL
		&& !m_pSelectItem->pFirstChild
#endif
		)
	{
		RtwRect rcSelection	=	m_pSelectItem->rcText;
		rcSelection.left    -=  12;
		rcSelection.right	+=  12;
		rcSelection.top		-=	11;
		rcSelection.bottom	+=	9;
        
		//选中背景矩形裁剪

#ifdef _DLL
		rcSelection.ClipBy(rcAfterClip);                                        // Add by Peter 2010.12.30 
		if (rcSelection.isValid() && m_pCoverImage)
#else
		if (m_pCoverImage)
#endif
		{
			RtwImage *p = m_pExpandImage ? m_pCoverImage->getRenderImage() : NULL;
			if (p)
			{
				p->SetCtrlTransparent(m_Transparency);
				g_workspace.getRenderDevice()->DrawImage(p, rcSelection, SUiTextAlignment(alignNear, alignNear), pClipRect);
			}
		}
	}

	//从头开始绘制
	Item* pFirstItem = getFirstItem();
	if (pFirstItem)
	{
		_DrawTreeItem(pFirstItem, &rcAfterClip);
	}

	RtwWidget::DrawForeground(pClipRect);
}

void RtwTree::DrawBackground(const RtwRect* pClipRect/* = NULL*/)
{
	RtwWidget::DrawBackground(pClipRect);
}

bool RtwTree::CloneFrom(const RtwWidget* pWidget)
{
	UI_ENSURE_B(pWidget->IsKindOf(RtwTree::GetRuntimeClass()));

	UI_ENSURE_B(RtwWidget::CloneFrom(pWidget));

	return true;
}

//矩形计算
void RtwTree::CalcClient()
{
	m_rcClient = m_rcFrame;

	if (m_pRootItem)
	{
		Item* pItem = getFirstItem();
		if (pItem)
		{
			SPoint StartPoint	= m_rcClient.getLeftTop(); //控件开始point

			StartPoint += m_AutoScrollOffset;
			StartPoint += SPoint(m_padding.left, m_padding.top);

			//所有绘制都暂时取消
			_DoBeforeDrawItem(pItem);

			//遍历重置item的坐标
			_CalcTreeItemPos(StartPoint, pItem);
		}
	}

	//计算滚动条
	if (m_bAutoScrollV || m_bAutoScrollH)
	{
		CalcAutoScroll();
	}
}

//计算item的起始位置
void RtwTree::_CalcTreeItemPos(SPoint& CurrentPoint, Item* pItem)
{
	//得到item的width,height
	SSize FunctionImageSize = m_pExpandImage->getSize();
	int nFontSize = g_workspace.getFontManager()->getFontSize();

	pItem->bDraw = true;

	int height = nFontSize;


	if (pItem->pFirstChild || (pItem->m_nBeforeNum<=0||pItem->m_nBeforeNum>MAXIMAGENUM))
	{
		//正常模式
		//image矩形
		pItem->rcFunctionImage.left		= CurrentPoint.x;
		pItem->rcFunctionImage.right	= CurrentPoint.x + FunctionImageSize.width;
		pItem->rcFunctionImage.top		= CurrentPoint.y;
		pItem->rcFunctionImage.bottom	= CurrentPoint.y + FunctionImageSize.height;

		//文本矩形
		pItem->rcText.left		= pItem->rcFunctionImage.right + 4;
		pItem->rcText.right		= m_rcClient.right - 1;
		pItem->rcText.top		= CurrentPoint.y;
		pItem->rcText.bottom	= pItem->rcText.top + nFontSize;
	}
	else if ((pItem->m_nBeforeNum>0&&pItem->m_nBeforeNum<=MAXIMAGENUM) || (pItem->m_nAfterNum>0&&pItem->m_nAfterNum<=MAXIMAGENUM))
	{
		int tmpHeight = nFontSize;
		int i = 0;
		int width = pItem->m_sizeBefore.width;
		int iheight = pItem->m_sizeBefore.height;

		int nBegin = CurrentPoint.x+(i*width) + 2;
		for (; i<pItem->m_nBeforeNum; ++i)
		{
			pItem->rcBeforeImage[i] = RtwRect(nBegin, CurrentPoint.y, nBegin+width, CurrentPoint.y+iheight);
			nBegin += width;
		}

		//文本矩形
		pItem->rcText.left		= pItem->rcBeforeImage[pItem->m_nBeforeNum-1].right+1;
		pItem->rcText.top		= CurrentPoint.y;
		pItem->rcText.bottom	= pItem->rcText.top + nFontSize;

		if (iheight > height)
		{
			height = iheight;
		}

		nBegin = m_rcClient.right - g_workspace.getDefaultScrollVWidth()-2;
		width = pItem->m_sizeAfter.width;
		iheight= pItem->m_sizeAfter.height;

		for (i=0; i<pItem->m_nAfterNum; ++i)
		{
			pItem->rcAfterImage[pItem->m_nAfterNum-i-1] = RtwRect(nBegin-width, CurrentPoint.y, nBegin, CurrentPoint.y+iheight);
			nBegin -= width;
		}

		if (iheight > height)
		{
			height = iheight;
		}

		pItem->rcText.right	= nBegin-2;
	}

	CurrentPoint.y += height + 3;

	int Offset		= m_LevelOffset + FunctionImageSize.width;
	int OrigLeft	= CurrentPoint.x;

	if (pItem->pFirstChild)
	{
		if (pItem->bExpanded)
		{
			CurrentPoint.x += Offset;
			_CalcTreeItemPos(CurrentPoint, pItem->pFirstChild);
		}
	}

	if (pItem->pNext)
	{
		CurrentPoint.x = OrigLeft;
		_CalcTreeItemPos(CurrentPoint, pItem->pNext);
	}
}

void RtwTree::_DoBeforeDrawItem(Item* pItem)
{
	//所有绘制都暂时取消
	pItem->bDraw = false;

	if (pItem->pFirstChild)
		_DoBeforeDrawItem(pItem->pFirstChild);

	if (pItem->pNext)
		_DoBeforeDrawItem(pItem->pNext);
}

//绘制树
void RtwTree::_DrawTreeItem(Item* pItem, const RtwRect* pClipRect)
{
	if (pItem->bDraw)
	{
		if (pItem->pFirstChild)
		{
			RtwImage *pImage = NULL;
			if (pItem->bExpanded)
			{
				//展开
				pImage = m_pExpandImage ? m_pExpandImage->getRenderImage() : NULL;
			}
			else 
			{
				//关闭
				pImage = m_pShrinkImage ? m_pShrinkImage->getRenderImage() : NULL;
			}

			if (pImage)
			{
				pImage->SetCtrlTransparent(m_Transparency);
				g_workspace.getRenderDevice()->DrawImage(
					pImage, pItem->rcFunctionImage,
					SUiTextAlignment(alignNear, alignNear),
					pClipRect
					);
			}
		}
		else
		{
			if (pItem->m_nBeforeNum>0 && pItem->m_nBeforeNum<=MAXIMAGENUM)
			{
				RtwImage *p = NULL;
				RtwRect rcRect;
				for (int i=0; i<pItem->m_nBeforeNum; ++i)
				{
					p  = pItem->m_pBeforeImg[i];
					rcRect = pItem->rcBeforeImage[i];
					if (p)
					{
						p->SetBlend(true);
						p->SetTransparency(m_Transparency);
						g_workspace.getRenderDevice()->DrawImage(p->getRenderImage(), rcRect, SUiTextAlignment(alignNear, alignNear), pClipRect);

					}
				}
			}

			if (pItem->m_nAfterNum>0 && pItem->m_nAfterNum<=MAXIMAGENUM)
			{
				RtwImage *p = NULL;
				RtwRect rcRect;

				for (int j=0; j<pItem->m_nAfterNum; ++j)
				{
					p  = pItem->m_pAfterImg[j];
					rcRect = pItem->rcAfterImage[j];
					if (p)
					{
						p->SetBlend(true);
						p->SetTransparency(m_Transparency);
						g_workspace.getRenderDevice()->DrawImage(p->getRenderImage(), rcRect, SUiTextAlignment(alignNear, alignNear), pClipRect);

					}
				}
			}
		}

		//文字
		g_workspace.getFontManager()->DrawString(
			pItem->text, pItem->rcText, SETCOLORALPHA(pItem->TextColor, m_Transparency),
			false, SUiTextAlignment(alignNear, alignNear), false, pClipRect
			);
// 
// 		//游戏中<完成>标记
// 		if (!pItem->extText.empty())
// 		{
// 			std::string showText = "（"+pItem->extText+"）";
// 			RtwRect rc;
// 			rc.left		= pItem->rcText.left + g_workspace.getFontManager()->GetStringWidth(pItem->text) + 5;
// 			rc.right	= rc.left + g_workspace.getFontManager()->GetStringWidth(showText);
// 			rc.top		= pItem->rcText.top;
// 			rc.bottom	= pItem->rcText.bottom;
// 
// 			g_workspace.getFontManager()->DrawString(
// 				showText,rc,SETCOLORALPHA(pItem->extTextColor, m_Transparency),
// 				false, SUiTextAlignment(alignNear, alignNear), false, pClipRect
// 				);
// 		}
	}

	//遍历绘制
	if (pItem->pFirstChild)
	{
		_DrawTreeItem(pItem->pFirstChild, pClipRect);
	}

	if (pItem->pNext)
	{

		_DrawTreeItem(pItem->pNext, pClipRect);
	}
	else
	{
		m_ExpandSize = pItem->rcText.bottom;

		if(!pItem->bExpanded)
		{
			m_ExpandSize = 10;
		}
	}
}

//捡取item
bool RtwTree::_PickItem(Item* pItem, const SPoint& point, Item** ppItem, bool& bFunctionImage)
{
	UI_ENSURE_B(pItem);

	//找到就return
	if (pItem->pFirstChild && (pItem->rcFunctionImage.IsContain(point) || pItem->rcText.IsContain(point)))
	{
		//文字区域
		bFunctionImage	= false;
		*ppItem			= pItem;

		return true;
	}
	else if (!pItem->pFirstChild)
	{
		bool bResult = false;
		for (int i=0; i<pItem->m_nBeforeNum; ++i)
		{
			if (pItem->rcBeforeImage[i].IsContain(point))
			{
				bResult = true;
				break;
			}
		}

		for (int j=0; j<pItem->m_nAfterNum; ++j)
		{
			if (pItem->rcAfterImage[j].IsContain(point))
			{
				bResult = true;
				break;
			}
		}

		if (bResult || pItem->rcText.IsContain(point))
		{
			//文字区域
			bFunctionImage	= false;
			*ppItem			= pItem;

			return true;
		}
		else
		{
			if (_PickItem(pItem->pNext, point, ppItem, bFunctionImage))
			{
				return true;
			}
		}
	}
	else 
	{
		//遍历查找
		//子节点
		if (pItem->pFirstChild && pItem->pFirstChild->bDraw)
		{
			if (_PickItem(pItem->pFirstChild, point, ppItem, bFunctionImage))
			{
				return true;
			}
		}

		//兄弟节点
		if (pItem->pNext && pItem->pNext->bDraw)
		{
			if (_PickItem(pItem->pNext, point, ppItem, bFunctionImage))
			{
				return true;
			}
		}
	}

	return false;
}

void RtwTree::_ResetSelf()
{
	m_WidgetType	= wtLabel;
	m_Flag			= m_DefaultFlag;
	m_TextAlign		= SUiTextAlignment(alignNear, alignNear);
	m_pSelectItem	= NULL;	//没有已选中的item
}

//插入子项
RtwTree::Item* RtwTree::InsertItem(
								   const std::string& text, const char* link/* = ""*/, DWORD id,  Item* pParent/* = NULL*/, 
								   Item* pInsertAfter/* = NULL*/, RtwPixel itemcolor/* = 0xFFFFFFFF*/, int pindex/* = 0*/, 
								   bool bTrack, const string &extText, RtwPixel itemcolorex
								   )
{
	Item* pNewItem = RT_NEW Item();
	pNewItem->text			= text;
	pNewItem->link			= link;
	pNewItem->TextColor		= itemcolor;
	pNewItem->index			= pindex;
	pNewItem->bTrack		= bTrack;
	pNewItem->extText		= extText;
	pNewItem->extTextColor	= itemcolorex;
	pNewItem->id			= id;
	pNewItem->m_nAfterNum	= m_nAfterNum;
	pNewItem->m_nBeforeNum	= m_nBeforeNum;
	pNewItem->m_sizeAfter	= m_sizeAfter;
	pNewItem->m_sizeBefore  = m_sizeBefore;

	Item *pTmpParent = (pParent) ? pParent:m_pRootItem;
	if (pInsertAfter)
	{
		//在pInsertAfter后面插入
		Item* pTmp			= pInsertAfter->pNext;
		pInsertAfter->pNext = pNewItem;
		pNewItem->pParent	= pInsertAfter->pParent;

		if (pTmp)
		{
			pNewItem->pNext = pTmp;
		}
	}
	else
	{
		//在pParent的结尾插入
		Item* pTmp = pTmpParent->pFirstChild;

		while (pTmp && pTmp->pNext)
		{
			pTmp = pTmp->pNext;
		}

		if (pTmp)
		{
			pTmp->pNext = pNewItem;
		}
		else 
		{
			pTmpParent->pFirstChild = pNewItem;
		}

		pNewItem->pParent = pTmpParent;
	}

	//重新计算矩形
	DoCalcClient();
	return pNewItem;
}

//删除item
bool RtwTree::DeleteItem(Item** ppItem)
{
	//要删除的item为空
	UI_ENSURE_B(ppItem && *ppItem);

	//删除子节点
	Item* pChild	= (*ppItem)->pFirstChild;
	Item *pNext		= NULL;

	while (pChild)
	{
		pNext	= pChild->pNext;
		DeleteItem(&pChild);
		pChild	= pNext;
	}
	(*ppItem)->pFirstChild = NULL;

	// 删除pointer
	if ((*ppItem)->pParent  &&  ((*ppItem) == (*ppItem)->pParent->pFirstChild))
	{
		(*ppItem)->pParent->pFirstChild = (*ppItem)->pNext;
	}

	// 删除自己,并将上一个节点的下一结点设置为自己的下一结点
	Item* pPrevious = (*ppItem)->getPrevious();
	if (pPrevious)
	{
		pPrevious->pNext = (*ppItem)->pNext;
	}

	// 取消选择
	if (m_pSelectItem == *ppItem)
	{
		m_pSelectItem = NULL;
	}

	//释放(因为在insert的时候是new出来的)
	DEL_ONE((*ppItem));

	return true;
}

//清空item
void RtwTree::DeleteAllItems()
{
	if (m_pRootItem)
	{
		DeleteItem(&m_pRootItem);
	}

	_CreateRootItem();
}

//移动
//beign
//移动item到pItem前面
void RtwTree::MoveItemToPrevious(Item* pItem)
{
	Item* pPreItem = pItem->getPrevious();
	UI_ENSURE(pPreItem);

	Item* pPrePreItem	= pPreItem->getPrevious();
	Item* pNextItem		= pItem->pNext;
	Item* pParentItem	= pItem->pParent;

	if (!pPrePreItem)
	{
		if (pParentItem)
		{
			pParentItem->pFirstChild = pItem;
		}
	}
	else 
	{
		pPrePreItem->pNext = pItem;
	}

	pItem->pNext	= pPreItem;
	pPreItem->pNext = pNextItem;

	//刷新
	Refresh();
}

//移动到pItem后面
void RtwTree::MoveItemToNext(Item* pItem)
{
	Item* pNextItem = pItem->pNext;
	UI_ENSURE(pNextItem);

	Item* pPreItem		= pItem->getPrevious();
	Item* pParentItem	= pItem->pParent;
	Item* pNextNextItem = pNextItem->pNext;

	if (!pPreItem)
	{
		if (pParentItem)
		{
			pParentItem->pFirstChild = pNextItem;
		}
	}
	else 
	{
		pPreItem->pNext = pNextItem;
	}

	pItem->pNext		= pNextNextItem;
	pNextItem->pNext	= pItem;

	Refresh();
}
//end

//展开
void RtwTree::Expand(Item* pItem)
{
	if (pItem && pItem->pFirstChild)
	{
		pItem->bExpanded = true;
		CalcClient();
	}
}

//关闭
void RtwTree::Shrink(Item* pItem)
{
	if (pItem && pItem->pFirstChild)
	{
		pItem->bExpanded = false;
		CalcClient();
	}
}

//选中
void RtwTree::Select(Item* pItem)
{
	if (pItem)
	{
		m_pSelectItem = pItem;
		CalcClient();
	}
}

//在指定位置捡取item
bool RtwTree::PickItem(const SPoint& point, Item** ppItem, bool& bFunctionImage)
{
	return _PickItem(m_pRootItem->pFirstChild, point, ppItem, bFunctionImage);
}

//打印结构
void RtwTree::PrintToConsole()
{
	_PrintToConsole_TreeItem("", m_pRootItem->pFirstChild);
}

//打印指定item的结构
void RtwTree::_PrintToConsole_TreeItem(const std::string& prefix, Item* pItem)
{
	UI_ENSURE(pItem);

	RtCoreLog().Info("%s%s\n", prefix.c_str(), pItem->text.c_str());

	if (pItem->pFirstChild)
	{
		std::string preChild = prefix + "    ";
		_PrintToConsole_TreeItem(preChild, pItem->pFirstChild);
	}
	if (pItem->pNext)
	{
		_PrintToConsole_TreeItem(prefix, pItem->pNext);
	}
}

//更新显示矩形
void RtwTree::DoCalcClient()
{
	CalcClient();
}

void RtwTree::OnMouseMove_This(RtwWidget* pWidget, RtwEventDelegate* pEvent)
{
	Item* pTreeItem = NULL;
	bool bPickImage = false;
	if (PickItem(SPoint(pEvent->mouse.x, pEvent->mouse.y), &pTreeItem, bPickImage))
	{
		ModifyFlag(wfHint, 0);

		string strHintText;
		if (pTreeItem->getHintFromPos(pEvent->mouse.x, pEvent->mouse.y, strHintText))
		{
			//获取成功
			g_workspace.SetHintText((const char*)strHintText.c_str(), true);
			g_workspace.SetHintSize(160, 30);
			g_workspace.AutoHintSize();
			g_workspace.AutoHintPosition();
		}
		else
		{
// 			ModifyFlag(0, wfHint);
		}
	}
	else
	{
// 		ModifyFlag(0, wfHint);
	}
}

//左键点击
void RtwTree::OnMouseLDown_This(RtwWidget* pWidget, RtwEventDelegate* pEvent)
{
	Item* pTreeItem = NULL;
	bool bPickImage = false;
	if (PickItem(SPoint(pEvent->mouse.x, pEvent->mouse.y), &pTreeItem, bPickImage))
	{
		if (pTreeItem->pFirstChild)
		{
			//展开
			pTreeItem->bExpanded = !pTreeItem->bExpanded;
			CalcClient();

			// Added by Wayne Wong 2010-12-20
			m_pSelectItem = pTreeItem;
			EvSelect(this, etSelect);
		}
		else 
		{
			m_pSelectItem = pTreeItem;
			EvSelect(this, etSelect);
		}
	}
}

//右键(与左键一样, 选中)
void RtwTree::OnMouseRDown_This(RtwWidget* pWidget, RtwEventDelegate* pEvent)
{
	OnMouseLDown_This(pWidget, pEvent);
}

//获取第一个子节点
RtwTree::Item* RtwTree::getFirstItem() const
{
	return m_pRootItem->pFirstChild;
}

//获取下一个item(应该是获取下一个可用的item)
RtwTree::Item* RtwTree::getNextItem(Item* pItem) const
{
	if (!pItem)
	{
		return NULL;
	}

	//有子节点
	if (pItem->pFirstChild)
	{
		return pItem->pFirstChild;
	}

	//没有子节点,但有兄弟节点
	if (pItem->pNext)
	{
		return pItem->pNext;
	}

	//没有子节点和兄弟节点,则获取下一个可以用的item
	Item* pParentItem = pItem->pParent;
	while (pParentItem && pParentItem != m_pRootItem)
	{
		if (pParentItem->pNext)
		{
			return pParentItem->pNext;
		}

		pParentItem = pParentItem->pParent;
	}

	return NULL;
}

RtwTree::Item* RtwTree::FindTreeItemByID(DWORD id)
{
	if (id<0)
	{
		return NULL;
	}

	if (!m_pRootItem->pFirstChild)
	{
		return NULL;
	}

	if (m_pRootItem->pFirstChild->id == id) 
	{
		return m_pRootItem->pFirstChild;
	}

	Item* pChildTmp = m_pRootItem->pFirstChild;
	if (!pChildTmp) 
	{
		return NULL;
	}

	Item *pTmp = NULL;
	while(pChildTmp && pChildTmp->id != id)
	{
		pTmp = pChildTmp->FindChildById(id);

		if (pTmp)
		{
			return pTmp;
		}
		else
		{
			pChildTmp = pChildTmp->pNext;
			if (pChildTmp && pChildTmp->id==id) return pChildTmp;
		}
	}

	return NULL;
}

//根据文本查找tree的item
RtwTree::Item* RtwTree::FindTreeItemByText(const char* findtext)
{
	//文本为空
	if (findtext==NULL || strlen(findtext)==0)
	{
		return NULL;
	}

	if (m_pRootItem->pFirstChild->text == findtext) 
	{
		return m_pRootItem->pFirstChild;
	}

	Item* pChildTmp = m_pRootItem->pFirstChild;
	if (!pChildTmp) 
	{
		return NULL;
	}

	Item* pTmp = NULL;
	while(pChildTmp && pChildTmp->text != findtext)
	{
		pTmp = pChildTmp->FindChildByText(findtext);
		if (pTmp)
			return pTmp;
		else
		{
			pChildTmp = pChildTmp->pNext;
			if (pChildTmp && pChildTmp->text == findtext) return pChildTmp;
		}
	}
	return NULL;
}

void RtwTree::OnRemoveChild(RtwWidget* pWidget){}

//在指定命名root内插入item
RtwTree::Item* RtwTree::InsertTreeItem(
									   const char *root, const char *child, const char *link, DWORD id, RtwPixel itemcolor, 
									   int pindex, bool bTrack,const string &extText, RtwPixel itemcolorex
									   )
{
	return InsertItem(
		child, link, id, FindTreeItemByText(root), NULL,
		itemcolor, pindex, bTrack, extText, itemcolorex
		);
}

//设置命名为root的item的文本颜色
void RtwTree::SetItemColor(const char* root,DWORD color)
{
	Item* item = FindTreeItemByText(root);
	if (item) 
	{
		item->TextColor = color;
	}
}

void RtwTree::_DoAutoScrollV(int OriginalValue)
{
	SSize _OffsetSize;
	_OffsetSize.height = OriginalValue - m_pAutoScrollV->getValue();

	// 要确保Offset过程中调用CalcAutoScroll无效
	bool _bBak = m_bCalcAutoScrollFlag;
	m_bCalcAutoScrollFlag = false;

	Item *pItem = getFirstItem();
	while (pItem)
	{
		pItem->rcText.Offset(_OffsetSize);
		pItem->rcFunctionImage.Offset(_OffsetSize);

		//扩展图片
		if (pItem->m_nBeforeNum>0 && pItem->m_nBeforeNum<=MAXIMAGENUM)
		{
			for (int i=0; i<pItem->m_nBeforeNum; ++i)
			{
				pItem->rcBeforeImage[i].Offset(_OffsetSize);
			}
		}

		if (pItem->m_nAfterNum>0 && pItem->m_nAfterNum<=MAXIMAGENUM)
		{
			for (int j=0; j<pItem->m_nAfterNum; ++j)
			{
				pItem->rcAfterImage[j].Offset(_OffsetSize);
			}
		}

		pItem = getNextItem(pItem);
	}

	m_bCalcAutoScrollFlag = _bBak;
}

void RtwTree::_DoAutoScrollH(int OriginalValue)
{
	SSize _OffsetSize;
	_OffsetSize.width = OriginalValue - m_pAutoScrollH->getValue();

	// 要确保Offset过程中调用CalcAutoScroll无效
	bool _bBak = m_bCalcAutoScrollFlag;
	m_bCalcAutoScrollFlag = false;

	Item *pItem = getFirstItem();
	while (pItem)
	{
		pItem->rcText.Offset(_OffsetSize);
		pItem->rcFunctionImage.Offset(_OffsetSize);

		//扩展图片
		for (int i=0; i<pItem->m_nBeforeNum; ++i)
		{
			pItem->rcBeforeImage[i].Offset(_OffsetSize);
		}

		for (int j=0; j<pItem->m_nAfterNum; ++j)
		{
			pItem->rcBeforeImage[j].Offset(_OffsetSize);
		}

		pItem = getNextItem(pItem);
	}

	m_bCalcAutoScrollFlag = _bBak;
}