#include "ui/rtw_ui.h"

//********************************************************************
//	created:	2010.04.07 10:00
//	filename: 	rtw_tree.cpp
//	author:		ldr123
//	purpose:	���νṹ
//  other:		�ֲ��ع����ע��
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

//��������ȡhint�ı������hint����ͼƬ��
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

//����item״̬ͼ��hint
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
:m_pRootItem(NULL),		//���ڵ�
m_pExpandImage(NULL),	//չ��ʱͼƬ
m_pShrinkImage(NULL),	//�ϲ�ʱͼƬ
m_pSelectItem(NULL)	//��ѡ�е�item
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

//�ͷ�
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

//�������ڵ�
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
	//����Ĭ��hint
	SetHint(g_workspace.getDefaultHint());

	m_nBeforeNum = nBeforeNum;
	m_nAfterNum  = nAfterNum;
	m_sizeBefore = beforeSize;
	m_sizeAfter  = afterSize;
}

//����ĳ��item��ĳ��λ�õ�ͼƬ��hint
void RtwTree::UpdateItemImage(Item *pItem, int nIndex, string imgFile, string strHint /* =  */)
{
	UI_ENSURE(pItem);

	if (FindTreeItemByID(pItem->id) != pItem)
	{
		//�����Լ����ӽڵ�
		return;
	}

	//�ҵ�item,����image
	pItem->UpdateImage(nIndex, imgFile, strHint);

	//����ͼƬ��,������ʾ
	CalcClient();
}

//ƫ��
void RtwTree::OnOffset(const SSize& size)
{
	RtwWidget::OnOffset(size);

	//�ӵ�һ��item��ʼ�ƶ�
	Item* pItem = getFirstItem();
	while (pItem)
	{
		pItem->rcFunctionImage.Offset(size.width, size.height);
		pItem->rcText.Offset(size.width, size.height);

		//��չͼƬ
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

	//�ƶ���������ʾ
	CalcClient();
}

//�������������
//��������tree��ռ�õľ�������
RtwRect RtwTree::CalcChildrenRect()
{
	RtwRect rc	= m_rcClient;
	Item* pItem = getFirstItem();
	while (pItem)
	{
		//�ɻ���
		if (pItem->bDraw)
		{
			//����rect

			//��չͼƬ
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

//����ǰ��
void RtwTree::DrawForeground(const RtwRect* pClipRect/* = NULL*/)
{
	RtwRect rcAfterClip = m_rcClient;
	if (pClipRect)
		rcAfterClip.ClipBy(*pClipRect);

	//��ѡ�е�item,item�ɻ���,itemû����item

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
        
		//ѡ�б������βü�

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

	//��ͷ��ʼ����
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

//���μ���
void RtwTree::CalcClient()
{
	m_rcClient = m_rcFrame;

	if (m_pRootItem)
	{
		Item* pItem = getFirstItem();
		if (pItem)
		{
			SPoint StartPoint	= m_rcClient.getLeftTop(); //�ؼ���ʼpoint

			StartPoint += m_AutoScrollOffset;
			StartPoint += SPoint(m_padding.left, m_padding.top);

			//���л��ƶ���ʱȡ��
			_DoBeforeDrawItem(pItem);

			//��������item������
			_CalcTreeItemPos(StartPoint, pItem);
		}
	}

	//���������
	if (m_bAutoScrollV || m_bAutoScrollH)
	{
		CalcAutoScroll();
	}
}

//����item����ʼλ��
void RtwTree::_CalcTreeItemPos(SPoint& CurrentPoint, Item* pItem)
{
	//�õ�item��width,height
	SSize FunctionImageSize = m_pExpandImage->getSize();
	int nFontSize = g_workspace.getFontManager()->getFontSize();

	pItem->bDraw = true;

	int height = nFontSize;


	if (pItem->pFirstChild || (pItem->m_nBeforeNum<=0||pItem->m_nBeforeNum>MAXIMAGENUM))
	{
		//����ģʽ
		//image����
		pItem->rcFunctionImage.left		= CurrentPoint.x;
		pItem->rcFunctionImage.right	= CurrentPoint.x + FunctionImageSize.width;
		pItem->rcFunctionImage.top		= CurrentPoint.y;
		pItem->rcFunctionImage.bottom	= CurrentPoint.y + FunctionImageSize.height;

		//�ı�����
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

		//�ı�����
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
	//���л��ƶ���ʱȡ��
	pItem->bDraw = false;

	if (pItem->pFirstChild)
		_DoBeforeDrawItem(pItem->pFirstChild);

	if (pItem->pNext)
		_DoBeforeDrawItem(pItem->pNext);
}

//������
void RtwTree::_DrawTreeItem(Item* pItem, const RtwRect* pClipRect)
{
	if (pItem->bDraw)
	{
		if (pItem->pFirstChild)
		{
			RtwImage *pImage = NULL;
			if (pItem->bExpanded)
			{
				//չ��
				pImage = m_pExpandImage ? m_pExpandImage->getRenderImage() : NULL;
			}
			else 
			{
				//�ر�
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

		//����
		g_workspace.getFontManager()->DrawString(
			pItem->text, pItem->rcText, SETCOLORALPHA(pItem->TextColor, m_Transparency),
			false, SUiTextAlignment(alignNear, alignNear), false, pClipRect
			);
// 
// 		//��Ϸ��<���>���
// 		if (!pItem->extText.empty())
// 		{
// 			std::string showText = "��"+pItem->extText+"��";
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

	//��������
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

//��ȡitem
bool RtwTree::_PickItem(Item* pItem, const SPoint& point, Item** ppItem, bool& bFunctionImage)
{
	UI_ENSURE_B(pItem);

	//�ҵ���return
	if (pItem->pFirstChild && (pItem->rcFunctionImage.IsContain(point) || pItem->rcText.IsContain(point)))
	{
		//��������
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
			//��������
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
		//��������
		//�ӽڵ�
		if (pItem->pFirstChild && pItem->pFirstChild->bDraw)
		{
			if (_PickItem(pItem->pFirstChild, point, ppItem, bFunctionImage))
			{
				return true;
			}
		}

		//�ֵܽڵ�
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
	m_pSelectItem	= NULL;	//û����ѡ�е�item
}

//��������
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
		//��pInsertAfter�������
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
		//��pParent�Ľ�β����
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

	//���¼������
	DoCalcClient();
	return pNewItem;
}

//ɾ��item
bool RtwTree::DeleteItem(Item** ppItem)
{
	//Ҫɾ����itemΪ��
	UI_ENSURE_B(ppItem && *ppItem);

	//ɾ���ӽڵ�
	Item* pChild	= (*ppItem)->pFirstChild;
	Item *pNext		= NULL;

	while (pChild)
	{
		pNext	= pChild->pNext;
		DeleteItem(&pChild);
		pChild	= pNext;
	}
	(*ppItem)->pFirstChild = NULL;

	// ɾ��pointer
	if ((*ppItem)->pParent  &&  ((*ppItem) == (*ppItem)->pParent->pFirstChild))
	{
		(*ppItem)->pParent->pFirstChild = (*ppItem)->pNext;
	}

	// ɾ���Լ�,������һ���ڵ����һ�������Ϊ�Լ�����һ���
	Item* pPrevious = (*ppItem)->getPrevious();
	if (pPrevious)
	{
		pPrevious->pNext = (*ppItem)->pNext;
	}

	// ȡ��ѡ��
	if (m_pSelectItem == *ppItem)
	{
		m_pSelectItem = NULL;
	}

	//�ͷ�(��Ϊ��insert��ʱ����new������)
	DEL_ONE((*ppItem));

	return true;
}

//���item
void RtwTree::DeleteAllItems()
{
	if (m_pRootItem)
	{
		DeleteItem(&m_pRootItem);
	}

	_CreateRootItem();
}

//�ƶ�
//beign
//�ƶ�item��pItemǰ��
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

	//ˢ��
	Refresh();
}

//�ƶ���pItem����
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

//չ��
void RtwTree::Expand(Item* pItem)
{
	if (pItem && pItem->pFirstChild)
	{
		pItem->bExpanded = true;
		CalcClient();
	}
}

//�ر�
void RtwTree::Shrink(Item* pItem)
{
	if (pItem && pItem->pFirstChild)
	{
		pItem->bExpanded = false;
		CalcClient();
	}
}

//ѡ��
void RtwTree::Select(Item* pItem)
{
	if (pItem)
	{
		m_pSelectItem = pItem;
		CalcClient();
	}
}

//��ָ��λ�ü�ȡitem
bool RtwTree::PickItem(const SPoint& point, Item** ppItem, bool& bFunctionImage)
{
	return _PickItem(m_pRootItem->pFirstChild, point, ppItem, bFunctionImage);
}

//��ӡ�ṹ
void RtwTree::PrintToConsole()
{
	_PrintToConsole_TreeItem("", m_pRootItem->pFirstChild);
}

//��ӡָ��item�Ľṹ
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

//������ʾ����
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
			//��ȡ�ɹ�
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

//������
void RtwTree::OnMouseLDown_This(RtwWidget* pWidget, RtwEventDelegate* pEvent)
{
	Item* pTreeItem = NULL;
	bool bPickImage = false;
	if (PickItem(SPoint(pEvent->mouse.x, pEvent->mouse.y), &pTreeItem, bPickImage))
	{
		if (pTreeItem->pFirstChild)
		{
			//չ��
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

//�Ҽ�(�����һ��, ѡ��)
void RtwTree::OnMouseRDown_This(RtwWidget* pWidget, RtwEventDelegate* pEvent)
{
	OnMouseLDown_This(pWidget, pEvent);
}

//��ȡ��һ���ӽڵ�
RtwTree::Item* RtwTree::getFirstItem() const
{
	return m_pRootItem->pFirstChild;
}

//��ȡ��һ��item(Ӧ���ǻ�ȡ��һ�����õ�item)
RtwTree::Item* RtwTree::getNextItem(Item* pItem) const
{
	if (!pItem)
	{
		return NULL;
	}

	//���ӽڵ�
	if (pItem->pFirstChild)
	{
		return pItem->pFirstChild;
	}

	//û���ӽڵ�,�����ֵܽڵ�
	if (pItem->pNext)
	{
		return pItem->pNext;
	}

	//û���ӽڵ���ֵܽڵ�,���ȡ��һ�������õ�item
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

//�����ı�����tree��item
RtwTree::Item* RtwTree::FindTreeItemByText(const char* findtext)
{
	//�ı�Ϊ��
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

//��ָ������root�ڲ���item
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

//��������Ϊroot��item���ı���ɫ
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

	// Ҫȷ��Offset�����е���CalcAutoScroll��Ч
	bool _bBak = m_bCalcAutoScrollFlag;
	m_bCalcAutoScrollFlag = false;

	Item *pItem = getFirstItem();
	while (pItem)
	{
		pItem->rcText.Offset(_OffsetSize);
		pItem->rcFunctionImage.Offset(_OffsetSize);

		//��չͼƬ
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

	// Ҫȷ��Offset�����е���CalcAutoScroll��Ч
	bool _bBak = m_bCalcAutoScrollFlag;
	m_bCalcAutoScrollFlag = false;

	Item *pItem = getFirstItem();
	while (pItem)
	{
		pItem->rcText.Offset(_OffsetSize);
		pItem->rcFunctionImage.Offset(_OffsetSize);

		//��չͼƬ
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