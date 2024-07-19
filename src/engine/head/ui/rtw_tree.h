#ifndef _RTUI_TREE_H
#define _RTUI_TREE_H

//********************************************************************
//	created:	2010.04.07 10:00
//	filename: 	rtw_tree.h
//	author:		ldr123
//	purpose:	树形结构
//  other:		局部重构添加注释
//********************************************************************

namespace ui 
{
#define MAXIMAGENUM 4
	class RtwTree : public RtwWidget
	{
		RT_DECLARE_DYNCREATE(RtwTree, RtwWidget, 0, "ui")

		friend class CUiWidgetFactory;
		friend class CUiWidgetContainer;
		friend class CUiLayer;
		friend class CUiDesktop;
	public:
		//tree节点
		struct Item
		{
			DWORD		id;				//item的id
			std::string	text;			//文本
			std::string	link;			//链接
			std::string	extText;		//扩展文字(比如:完成之类)
			RtwPixel	TextColor;		//文本颜色
			RtwPixel	extTextColor;	//扩展文本的颜色
			void*		Param1;
			void*		Param2;

			bool		bDraw;			//绘制
			bool		bExpanded;		//是否展开
			bool		bTrack;			//跟踪(已完成?)
			RtwRect		rcText;			//文本矩形
			RtwRect		rcFunctionImage;//图片矩形

			Item*      pNext;			//下一个item
			Item*      pParent;			//父节点
			Item*      pFirstChild;		//第一个子节点

			int			index;			//item索引

			//begin
			int			m_nBeforeNum;
			int			m_nAfterNum;
			RtwImage*	m_pBeforeImg[MAXIMAGENUM];	//树内容前面图片
			RtwImage*	m_pAfterImg[MAXIMAGENUM];		//树后面的图片列表
			string		m_strBeforeHint[MAXIMAGENUM];
			string      m_strAfterHint[MAXIMAGENUM];

			RtwRect		rcBeforeImage[MAXIMAGENUM];//图片矩形
			RtwRect		rcAfterImage[MAXIMAGENUM];

            SSize		m_sizeBefore;
            SSize		m_sizeAfter;
			//end

			Item();
            ~Item();

			int PickItemImageIndex(int x, int y);

			//根据鼠标获取hint文本，这个hint属于图片的
			bool getHintFromPos(int x, int y, string &strResult);

			//更新item状态图及hint
			void UpdateImage(int index, string strFileImg, const string &strHint="");



			void SetIndex(int pindex)
			{
				index = pindex;
			}

			int	 GetIndex() const 
			{	
				return index; 
			}

			//根据索引查找item
			Item* GetItemByIndex(int pindex) 
			{
				//第一个子节点
				Item* pChildItem = pFirstChild;
				while(pChildItem)
				{
					//
					if (pChildItem->index == pindex)
					{
						return pChildItem;
					}

					Item *pTemp = pChildItem->GetItemByIndex(pindex);
					if (pTemp)
					{
						return pTemp;
					}

					pChildItem = pChildItem->pNext;
				}

				return NULL;
			}

			//是不是另一个item的子item
			bool IsChildOf(const Item* pTreeItem) const 
			{
				Item* pParentItem = pParent;
				while (pParentItem)
				{
					if (pParentItem == pTreeItem)
					{
						return true;
					}

					pParentItem = pParentItem->pParent;
				}

				return false;
			}

			//根据名字查找item
			Item* FindChildByText(const char* findtext)
			{
				Item* pChildItem = pFirstChild;

				while(pChildItem)
				{
					if (pChildItem->text == findtext)
					{
						return pChildItem;
					}

					Item* ptmp = pChildItem->FindChildByText(findtext);
					if(ptmp)
					{
						return ptmp;
					}

					pChildItem = pChildItem->pNext;
				}

				return NULL;
			}

			Item* FindChildById(DWORD id)
			{
				Item *pChildItem = pFirstChild;

				while (pChildItem)
				{
					if (pChildItem->id == id)
					{
						return pChildItem;
					}

					Item *pTmp = pChildItem->FindChildById(id);
					if (pTmp)
					{
						return pTmp;
					}

					pChildItem = pChildItem->pNext;
				}
				return NULL;
			}

			//上一个
			Item* getPrevious()
			{
				UI_ENSURE_P(pParent);
				Item* pTmp = pParent->pFirstChild;
				if (pTmp == this)
				{
					return NULL;
				}

				while (pTmp)
				{
					if (pTmp->pNext == this)
					{
						return pTmp;
					}

					pTmp = pTmp->pNext;
				}

				return NULL;
			}
		};

	protected:
		RtwTree();
		virtual ~RtwTree();
	public:
		void RtwTree::InitItemImage(int nBeforeNum, int nAfterNum, SSize beforeSize=SSize(20, 20), SSize afterSize=SSize(20, 20));
		void RtwTree::UpdateItemImage(Item *pItem, int nIndex, string imgFile, string strHint="");

		virtual void OnBeforeDestroy();
		virtual void Reset();
		virtual bool Create(DWORD Id);
		virtual void DrawForeground(const RtwRect* pClipRect = NULL);
		virtual void DrawBackground(const RtwRect* pClipRect = NULL);

		virtual bool CloneFrom(const RtwWidget* pWidget);
		virtual void OnRemoveChild(RtwWidget* pWidget);
		virtual void OnOffset(const SSize& size);
		virtual RtwRect CalcChildrenRect();

		Item* InsertItem(
							const std::string& text, const char* link = "", DWORD id=0, Item* pParent = NULL, Item* pInsertAfter = NULL, 
							RtwPixel itemcolor = 0xFFFFFFFF ,int pindex = 0, bool bTrack = false, 
							const std::string& strExt = "", RtwPixel itemcolorex = 0xFFFFFFFF
						);
		bool DeleteItem(Item** ppItem);
		void DeleteAllItems();
		void MoveItemToPrevious(Item* pItem);
		void MoveItemToNext(Item* pItem);

		void Expand(Item* pItem);
		void Shrink(Item* pItem);
		void Select(Item* pItem);
		bool PickItem(const SPoint& point, Item** ppItem, bool& bFunctionImage);
		void DoCalcClient();

		void PrintToConsole();
		int ExtendSize() {return m_ExpandSize;}

		Item* getRootItem() const { return m_pRootItem; }
		Item* getSelectItem() const { return m_pSelectItem; }
		void  setSelectItem()  {  m_pSelectItem=NULL; }

		Item* getFirstItem() const;
		Item* getNextItem(Item* pItem) const;

		Item* GetLastSelect() const {	return m_pSelectItem;	}
		void Clear() {	DeleteAllItems();	}
		void SetItemColor(const char* root,DWORD color);
		Item* InsertTreeItem(const char* root,const char* child,const char* link="", DWORD id=0, RtwPixel itemcolor = 0xFFFFFFFF, int pindex = 0,
			bool bTrack = false, const std::string& strExt = "", RtwPixel itemcolorex = 0xFFFFFFFF);
		Item* FindTreeItemByText(const char* text);
		Item* FindTreeItemByID(DWORD id);

	public:
		static const int m_DefaultFlag = RtwWidget::m_DefaultFlag | wfGrabMouse | wfMouseMove | wfInput | wfFocus;
		static const int m_LevelOffset = 2; // 真正的offset为m_pExpandImage->getSize().width + m_LevelOffset
	protected:
		virtual void CalcClient();
		virtual void _DoAutoScrollV(int OriginalValue);
		virtual void _DoAutoScrollH(int OriginalValue);
	private:
		void _ResetSelf();
		void _CreateRootItem();
		void _PrintToConsole_TreeItem(const std::string& prefix, Item* pItem);
		void _CalcTreeItemPos(SPoint& CurrentPoint, Item* pItem);
		void _DoBeforeDrawItem(Item* pItem);
		void _DrawTreeItem(Item* pItem, const RtwRect* pClipRect = NULL);
		bool _PickItem(Item* pItem, const SPoint& point, Item** ppItem, bool& bFunctionImage);

		Item*		m_pRootItem;
		RtwImage*	m_pExpandImage;
		RtwImage*	m_pShrinkImage;
		RtwImage*	m_pCoverImage;
		int			m_ExpandSize;
		Item*		m_pSelectItem;

		int			m_nBeforeNum;
		int			m_nAfterNum;
		SSize		m_sizeBefore;
		SSize		m_sizeAfter;
	public:
		void OnMouseLDown_This(RtwWidget* pWidget, RtwEventDelegate* pEvent);
		void OnMouseRDown_This(RtwWidget* pWidget, RtwEventDelegate* pEvent);

        //鼠标移动，为了实现hint
        void OnMouseMove_This(RtwWidget* pWidget, RtwEventDelegate* pEvent);

		RtwEventDispatcher  EvSelect;
		RtwEventDispatcher  EvSelectNode;
	};
} 
#endif