#include "ui/rtw_ui.h"

//********************************************************************
//	created:	2010.04.07 10:00
//	filename: 	rtw_workspace.cpp
//	author:		ldr123
//	purpose:	ui系统主要消息及逻辑处理类
//  other:		局部重构添加注释
//********************************************************************

ui::CUiDesktop g_workspace;

CUiDesktop::CUiDesktop()
	:m_State(dsSuspend),
	m_pRenderDevice(NULL),
	m_pImageFactory(NULL),
	m_pFontManager(NULL),
	m_pWidgetFactory(NULL),
	m_pThemeManager(NULL),
	m_pSound(NULL),
	m_pClipboard(NULL),
	m_pDefaultHint(NULL),
	m_pSystemLayer(NULL),
	m_pGarbageContainer(NULL),
	m_pFocusWidget(NULL),
	m_pMouseHoverWidget(NULL),
	m_pMouseDragWidget(NULL),
	m_pHintWidget(NULL),
	m_pUiXmlLoader(NULL),
	m_pDefaultPopupMenu(NULL),
	m_pDefaultMessageBox(NULL),
	m_pDefaultInputBox(NULL),
	m_pRefreshMediaFrame(NULL),
	m_pHintTargetWidget(NULL),
	m_pDefaultFocusWidget(NULL),
	m_pMouseCaptureWidget(NULL),
	m_pModalWidgetContainer(NULL),
	m_pDefaultWidgetContainer(NULL),
	m_pTopRenderWidgetContainer(NULL),
	m_rTransparent(1.0),
	m_bDebugMode(false),
	m_bModeWnd(false),
	m_bMouseLDown(false),
	m_bMouseRDown(false),
	m_pNoFocusWidget(NULL),
	m_bFontFileOk(false),
	m_strDefaultFontName("")
{
	m_HyperLinkDefaultStyle.bUnderLine = true;
	m_HyperLinkDefaultStyle.NormalColor = RtwPixel(0xff22bb44);
	m_HyperLinkDefaultStyle.HighlightColor = RtwPixel(0xffee2233);
}

CUiDesktop::~CUiDesktop() {}

//设备无效
void CUiDesktop::OnInvalidateDevice()
{
	if (m_pRenderDevice)
		m_pRenderDevice->OnInvalidateDevice();
	if (m_pFontManager)
		m_pFontManager->OnInvalidateDevice();
}

//设备恢复
void CUiDesktop::OnRestoreDevice()
{
	if (m_pRenderDevice)
		m_pRenderDevice->OnRestoreDevice();
	if (m_pFontManager)
		m_pFontManager->OnRestoreDevice();
}

//加载xml文件
bool CUiDesktop::Load(const char* xmlName)
{
	// 清空事件移动队列
	EvMove.Clear();

	// 遍历 m_Layers 中的每个 UiLayer 对象
	foreach(UiLayerList, iterLayer, m_Layers)
	{
		// 获取当前层
		CUiLayer* pLayer = *iterLayer;
		// 如果层的名称是 "layworld"
		if (pLayer->getName() == "layworld")
		{
			// 删除该层对象
			DROP_RTUI_OBJECT(pLayer);
			// 从 m_Layers 中移除该层
			m_Layers.erase(iterLayer);
			// 退出循环
			break;
		}
	}

	// 创建 XML 解析器对象
	RtsXmlDoc XmlDoc;
	// 解析 XML 文件，如果解析失败
	if (XmlDoc.ParserFile(xmlName) != 0)
	{
		// 记录错误信息
		RtCoreLog().Error("打开xml文件%s失败，请确认文件存在并且正确\n", xmlName);
		// 返回加载失败
		return false;
	}

	// 处理 XML 文件，如果处理失败
	if (!_ProcessXmlFile(static_cast<RtsXmlDoc::NodePtr*>(XmlDoc.GetRoot())))
	{
		// 记录错误信息
		RtCoreLog().Error("解析xml文件%s失败，请确认文件格式正确\n", xmlName);
		// 返回加载失败
		return false;
	}

	// 返回加载成功
	return true;
}

//创建各种管理器
bool CUiDesktop::Create(IUiRenderDevice* pRenderer, IUiImageFactory* pImageFactory, IUiFontManager* pFontManager)
{
	UI_CHECK_B(m_State == dsSuspend);

	if (!pRenderer || !pImageFactory)
	{
		return false;
	}

	m_pRenderDevice = pRenderer;

	OnRestoreDevice();
	m_pImageFactory = pImageFactory;
	m_pFontManager = pFontManager;
	m_pWidgetFactory = RT_NEW CUiWidgetFactory();
	m_pThemeManager = RT_NEW CUiThemeManager();
	m_pUiXmlLoader = RT_NEW RtwXmlLoader();

	m_pModalWidgetContainer = RT_NEW CUiWidgetContainer();
	m_pModalWidgetContainer->grab();
	m_pDefaultWidgetContainer = RT_NEW CUiWidgetContainer();
	m_pDefaultWidgetContainer->grab();
	m_pTopRenderWidgetContainer = RT_NEW CUiWidgetContainer();
	m_pTopRenderWidgetContainer->grab();
	m_pGarbageContainer = RT_NEW CUiWidgetContainer();
	m_pGarbageContainer->grab();
	UI_ENSURE_B(m_pModalWidgetContainer && m_pDefaultWidgetContainer && m_pTopRenderWidgetContainer && m_pGarbageContainer);
	//lyymark 1.UI.Init 注册ui默认图片路径和xml路径
	RegisterTexturePath("ui/textures/");
	RegisterXmlPath("ui/");

	std::string strFontName = "宋体";
	//int nResult = 0;
 //	int Result  = AddFontResource("fonts\\兰亭黑 GBK.TTF");
 //	SendMessage(HWND_BROADCAST,WM_FONTCHANGE,0,0);
 //	if (nResult)
 //	{
	//	SetFontFileOk();
	//	strFontName = "宋体";
 //	}
	SetDefaultFontName(strFontName);

	if (!m_pFontManager->CreateFont(strFontName.c_str(), 12, RtwFont::FONT_NORMAL))
	{
		RtCoreLog().Error("[UI] 创建字体失败\n");
		return false;
	}

	// Load and Init Theme
	if (!m_pThemeManager->Init())
	{
		RtCoreLog().Error("[UI] 主题管理器初始化失败\n");
		return false;
	}

	m_pThemeManager->SelectTheme("ZfsLike");

	if (!_InitSystemWidgets())
	{
		RtCoreLog().Error("[UI] 初始化系统控件失败\n");
		return false;
	}

#ifndef GAME_CLIENT
	RtsXmlDoc XmlDoc;
	if (XmlDoc.ParserFile("ui/uistudio.xml") != 0)
	{
		RtCoreLog().Error("打开ui.xml文件失败，请确认文件存在并且正确\n");
		return false;
	}
	if (!_ProcessXmlFile((RtsXmlDoc::NodePtr*)XmlDoc.GetRoot()))
	{
		RtCoreLog().Error("解析ui.xml文件失败，请确认文件格式正确\n");
		return false;
	}
#endif

	m_State = dsRun;

	return true;
}

//初始化系统组件
bool CUiDesktop::_InitSystemWidgets()
{
	UI_ENSURE_B(FindLayer(g_SystemWidgetName[sw_SystemLayer], (CUiLayer**)&m_pSystemLayer));

	UI_ENSURE_B(FindWidget(g_SystemWidgetName[sw_DefaultHint], (RtwWidget**)&m_pDefaultHint));
	m_pDefaultHint->SetTransparency(0.7);
	AddTopRenderWidget(m_pDefaultHint);

	UI_ENSURE_B(FindWidget(g_SystemWidgetName[sw_DefaultHL], (RtwWidget**)&m_pDefaultHLWidget));
	AddTopRenderWidget(m_pDefaultHLWidget);

	UI_ENSURE_B(FindWidget(g_SystemWidgetName[sw_AutoSbv]));
	UI_ENSURE_B(FindWidget(g_SystemWidgetName[sw_AutoSbh]));

	UI_ENSURE_B(FindWidget(g_SystemWidgetName[sw_PopupMenu], (RtwWidget**)&m_pDefaultPopupMenu));
	AddTopRenderWidget(m_pDefaultPopupMenu);

	//消息框
	m_pDefaultMessageBox = RT_NEW RtwMailBox();
	UI_ENSURE_B(m_pDefaultMessageBox->Init());

	//聊天输入框
	m_pDefaultInputBox = RT_NEW RtwChatInputBox();
	UI_ENSURE_B(m_pDefaultInputBox->Init());

	return true;
}

bool CUiDesktop::Destroy()
{
	UI_CHECK_B(m_State == dsRun);

	DROP_RTUI_OBJECT(m_pTopRenderWidgetContainer);
	DROP_RTUI_OBJECT(m_pModalWidgetContainer);
	DROP_RTUI_OBJECT(m_pDefaultWidgetContainer);
	DROP_RTUI_OBJECT(m_pMouseCaptureWidget);
	DROP_RTUI_OBJECT(m_pMouseHoverWidget);
	DROP_RTUI_OBJECT(m_pMouseDragWidget);
	DROP_RTUI_OBJECT(m_pHintWidget);
	DROP_RTUI_OBJECT(m_pFocusWidget);
	DROP_RTUI_OBJECT(m_pDefaultFocusWidget);
	DROP_RTUI_OBJECT(m_pDefaultHint);
	DROP_RTUI_OBJECT(m_pDefaultPopupMenu);
	DROP_RTUI_OBJECT(m_pHintTargetWidget);
	DROP_RTUI_OBJECT(m_pSystemLayer);
	DROP_RTUI_OBJECT(m_pGarbageContainer);
	DROP_RTUI_OBJECT(m_pDefaultMessageBox);
	DROP_RTUI_OBJECT(m_pDefaultInputBox);

	//释放所有组件
	while (!m_Layers.empty())
	{
		CUiLayer* pLayer = m_Layers.front();
		DROP_RTUI_OBJECT(pLayer);
		m_Layers.pop_front();
	}

	//关闭主题管理器
	m_pThemeManager->Close();

	//释放组件工厂
	DEL_ONE(m_pWidgetFactory);

	//释放主题管理器
	DEL_ONE(m_pThemeManager);

	//xml加载器
	DEL_ONE(m_pUiXmlLoader);

	m_State = dsDestroy;

	return true;
}

void CUiDesktop::Run(DWORD dwDelta)
{
	// 每2秒做一次垃圾收集
	static DWORD lastProcessGarbageTime = 0;
	DWORD nowTime = rtMilliseconds();
	if (nowTime - lastProcessGarbageTime > 2000)
	{
		DoGarbageClear();
	}

	// Run Widgets
	m_pDefaultWidgetContainer->Run(dwDelta);
	foreach(UiLayerList, iterLayer, m_Layers)
	{
		CUiLayer* pLayer = *iterLayer;
		pLayer->Run(dwDelta);
	}

	static RtwRect lastframe;
	RtwRect nowframe;
	nowframe = getViewportRect();
	if (nowframe != lastframe)
	{
		RtwEventDelegate e(etMove);
		e.move.OrigFrame_Left = nowframe.left;
		e.move.OrigFrame_Right = nowframe.right;
		e.move.OrigFrame_Top = nowframe.top;
		e.move.OrigFrame_Bottom = nowframe.bottom;
		EvMove(NULL, e);
		lastframe = nowframe;
	}
}

void CUiDesktop::Render()
{
	tick_begin();

	m_pRenderDevice->BeginRender();

	static RtwRect lastframe;
	RtwRect nowframe;
	nowframe = getViewportRect();
	if (nowframe != lastframe)
	{
		RtwEventDelegate e(etMove);
		e.move.OrigFrame_Left = nowframe.left;
		e.move.OrigFrame_Right = nowframe.right;
		e.move.OrigFrame_Top = nowframe.top;
		e.move.OrigFrame_Bottom = nowframe.bottom;
		EvMove(NULL, e);
		lastframe = nowframe;
	}

	// 把TopRenderWidgetContainer设置为不可见
	EXT_SPACE::unordered_map<DWORD, DWORD> bakFlags;
	for (UiWidgetList::iterator iterTopW = m_pTopRenderWidgetContainer->Begin(); iterTopW != m_pTopRenderWidgetContainer->End(); ++iterTopW)
	{
		RtwWidget* pWidget = *iterTopW;
		bakFlags[pWidget->getId()] = pWidget->m_Flag;
		pWidget->m_Flag &= ~wfVisible;
	}

	m_pDefaultWidgetContainer->Render();

	foreach(UiLayerList, iterLayer, m_Layers)
	{
		CUiLayer* pLayer = *iterLayer;
		pLayer->Render();
	}

	// 恢复TopRenderWidgetContainer的Flag
	for (UiWidgetList::iterator iterTopW = m_pTopRenderWidgetContainer->Begin(); iterTopW != m_pTopRenderWidgetContainer->End(); ++iterTopW)
	{
		RtwWidget* pWidget = *iterTopW;
		pWidget->m_Flag = bakFlags[pWidget->getId()];
	}
	m_pTopRenderWidgetContainer->Render();

	//拖拽组件的render
	if (m_pMouseDragWidget)
	{
		RtwRect rcOrig = m_pMouseDragWidget->GetFrameRect();
		m_pMouseDragWidget->Offset(SSize(m_MousePos.x - rcOrig.left, m_MousePos.y - rcOrig.top));
		m_pMouseDragWidget->Render();
		m_pMouseDragWidget->Offset(SSize(rcOrig.left - m_MousePos.x, rcOrig.top - m_MousePos.y));
	}

	//提示框
	m_pDefaultHint->Render();

	if (m_pHintWidget)
	{
		m_pHintWidget->Render();
	}

	if (m_pModalWidgetContainer->getWidgetCount() > 0)
	{
		m_pModalWidgetContainer->Render();
	}

	m_pRenderDevice->EndRender();

	if (m_pMouseCaptureWidget)
	{
		//被鼠标单击的组件
		RtwEventDelegate e;
		e.type = etButtonPressed;
		e.mouse.x = m_MousePos.x;
		e.mouse.y = m_MousePos.y;
		m_pMouseCaptureWidget->OnMouseCapture(e);
	}
}

void CUiDesktop::RemoveAll(bool bIncludeSystemWidgets/* = false*/)
{
	//未实现
}

//根据id查找层
bool CUiDesktop::FindLayer(DWORD LayerId, CUiLayer** ppLayer/* = NULL*/)
{
	if (ppLayer)
		*ppLayer = NULL;

	foreach(UiLayerList, iter, m_Layers)
	{
		CUiLayer* pLayer = *iter;
		if (pLayer->getId() == LayerId)
		{
			if (ppLayer)
			{
				pLayer->grab();
				*ppLayer = pLayer;
				return true;
			}
		}
	}

	return false;
}

//根据名字查找层
bool CUiDesktop::FindLayer(const std::string& Name, CUiLayer** ppLayer/* = NULL*/)
{
	if (ppLayer)
		*ppLayer = NULL;

	foreach(UiLayerList, iter, m_Layers)
	{
		CUiLayer* pLayer = *iter;
		if (pLayer->getName() == Name)
		{
			if (ppLayer)
			{
				pLayer->grab();
				*ppLayer = pLayer;
				return true;
			}
		}
	}

	return false;
}

//添加层
bool CUiDesktop::AddLayer(CUiLayer* pLayer, bool bAddToFront/* = false*/)
{
	if (FindLayer(pLayer->getId()))
		return false;

	pLayer->grab();

	if (bAddToFront)
		m_Layers.push_front(pLayer);
	else
		m_Layers.push_back(pLayer);

	return true;
}

//通过id删除层
bool CUiDesktop::RemoveLayer(DWORD LayerId)
{
	foreach(UiLayerList, iter, m_Layers)
	{
		CUiLayer* pLayer = *iter;
		if (pLayer->getId() == LayerId)
		{
			DROP_RTUI_OBJECT(pLayer);
			m_Layers.erase(iter);
			return true;
		}
	}

	return false;
}

//通过名字删除层
bool CUiDesktop::RemoveLayer(const std::string& Name)
{
	foreach(UiLayerList, iter, m_Layers)
	{
		CUiLayer* pLayer = *iter;
		if (pLayer->getName() == Name)
		{
			DROP_RTUI_OBJECT(pLayer);
			m_Layers.erase(iter);
			return true;
		}
	}

	return false;
}

//通过id查找组件
bool CUiDesktop::FindWidget(DWORD WidgetId, bool bRecursive/* = false*/, RtwWidget** ppWidget/* = NULL*/, CUiLayer** ppInLayer/* = NULL*/)
{
	if (ppWidget)
		*ppWidget = NULL;

	if (ppInLayer)
		*ppInLayer = NULL;

	if (m_pDefaultWidgetContainer && m_pDefaultWidgetContainer->FindWidget(WidgetId, ppWidget))
	{
		return true;
	}

	//递归查找
	if (bRecursive)
	{
		foreach(UiLayerList, iterLayer, m_Layers)
		{
			CUiLayer* pLayer = *iterLayer;
			if (pLayer->FindWidget(WidgetId, ppWidget, true))
			{
				if (ppInLayer)
				{
					*ppInLayer = pLayer;
					pLayer->grab();
				}
				return true;
			}
		}
	}
	else
	{
		return false;
	}

	return false;
}

//通过名字查找组件(全名)
bool CUiDesktop::FindWidget(const std::string& FullName, RtwWidget** ppWidget/* = NULL*/)
{
	if (ppWidget)
		*ppWidget = NULL;

	// 把字符串拆分
	std::vector<std::string> arrWidgetNames;
	UI_ENSURE_B(_SplitWidgetName(FullName, arrWidgetNames));

	// 依次查找Widget
	RtwWidget* pWidget = NULL;
	int index = 0;
	foreach(UiLayerList, iterLayer, m_Layers)
	{
		CUiLayer* pLayer = *iterLayer;
		if (pLayer->getName() == arrWidgetNames[0])
		{
			if ((int)arrWidgetNames.size() > 1)
			{
				if (pLayer->FindWidget(arrWidgetNames[1], &pWidget))
				{
					pWidget->drop();
					for (index = 2; index < (int)arrWidgetNames.size(); ++index)
					{
						if (!pWidget->FindChild(arrWidgetNames[index], &pWidget))
							break;
						pWidget->drop();
					}

					if (index < (int)arrWidgetNames.size())
					{
						return false;
					}
					else
					{
						if (ppWidget)
						{
							*ppWidget = pWidget;
							pWidget->grab();
						}
						return true;
					}
				}
				else
				{
					return false;
				}
			}
			else
			{
				if (ppWidget)
				{
					*ppWidget = pLayer;
					pLayer->grab();
				}
				return true;
			}
		}
	}

	UI_ENSURE_B(m_pDefaultWidgetContainer);
	if (!m_pDefaultWidgetContainer->FindWidget(arrWidgetNames[0], &pWidget))
		return false;

	pWidget->drop();
	for (index = 1; index < (int)arrWidgetNames.size(); ++index)
	{
		if (!pWidget->FindChild(arrWidgetNames[index], &pWidget))
			break;
		pWidget->drop();
	}

	if (index < (int)arrWidgetNames.size())
	{
		return false;
	}
	else
	{
		if (ppWidget)
		{
			*ppWidget = pWidget;
			pWidget->grab();
		}
		return true;
	}
}

//拆分窗体名
bool CUiDesktop::_SplitWidgetName(const std::string& FullName, std::vector<std::string>& arrShortName)
{
	arrShortName.clear();

	int offset = 0;
	int pos = (int)FullName.find_first_of('.', offset);
	while (pos != std::string::npos)
	{
		std::string str;
		if (!arrShortName.empty())
		{
			str += arrShortName.back();
			str += ".";
		}
		str += FullName.substr(offset, pos - offset);
		arrShortName.push_back(str);
		offset = pos + 1;
		pos = (int)FullName.find_first_of('.', offset);
	}
	if (offset < (int)FullName.size())
	{
		arrShortName.push_back(FullName);
	}

	return true;
}

//添加组件
bool CUiDesktop::AddWidget(RtwWidget* pWidget)
{
	if (pWidget->getWidgetType() == wtLayer)
	{
		return AddLayer((CUiLayer*)pWidget);
	}
	else
	{
		return m_pDefaultWidgetContainer->AddWidget(pWidget);
	}
}

//根据id删除组件
bool CUiDesktop::RemoveWidget(DWORD WidgetId)
{
	if (m_pDefaultWidgetContainer && m_pDefaultWidgetContainer->RemoveWidget(WidgetId, true))
		return true;

	if (m_pGarbageContainer && m_pGarbageContainer->RemoveWidget(WidgetId, true))
		return true;

	UiLayerList::iterator iterLayer = m_Layers.begin();
	for (; iterLayer != m_Layers.end(); ++iterLayer)
	{
		CUiLayer* pLayer = *iterLayer;

		if (!pLayer)
		{
			return false;
		}

		if (pLayer->getId() == WidgetId)
			return RemoveLayer(WidgetId);
		else if (pLayer->RemoveWidget(WidgetId, true))
			return true;
	}

	return false;
}

bool CUiDesktop::PickWidget(
	RtwWidget** ppWidget/* = NULL*/, CUiLayer** ppLayer/* = NULL*/,
	DWORD FlagMask/* = 0x00000000*/, UiLayerList* pExcludeLayers/* = NULL*/,
	CUiWidgetContainer* pExcludeWidgets/* = NULL*/)
{
	if (ppWidget)
		*ppWidget = NULL;
	if (ppLayer)
		*ppLayer = NULL;

	// 处理顺序：
	// PopupMenu
	// 当前焦点
	// 当前焦点的所有父亲窗口
	// Modal窗口容器
	// TopRender窗口容器
	// 系统默认层
	// 其他层
	if (m_pDefaultPopupMenu && m_pDefaultPopupMenu->getVisible() && m_pDefaultPopupMenu->PickWidget(m_MousePos, ppWidget, FlagMask, pExcludeWidgets))
	{
		return true;
	}

	if (m_pFocusWidget && m_pFocusWidget->PickWidget(m_MousePos, ppWidget, FlagMask, pExcludeWidgets))
	{
		const CUiLayer* pInLayer = (*ppWidget)->getLayer();
		if (pExcludeLayers && pInLayer)
		{
			UiLayerList::const_iterator iterExLayer;
			for (iterExLayer = pExcludeLayers->begin(); iterExLayer != pExcludeLayers->end(); ++iterExLayer)
			{
				CUiLayer* pTmpLayer = *iterExLayer;
				if (pInLayer == pTmpLayer)
					break;
			}
			if (iterExLayer == pExcludeLayers->end())
				return true;
		}
		else
		{
			return true;
		}

		DROP_RTUI_OBJECT(*ppWidget);
	}

	RtwWidget* pTmpWidget1 = m_pFocusWidget;
	while (pTmpWidget1 && pTmpWidget1->GetParent() && pTmpWidget1->GetParent()->getWidgetType() != wtLayer)
	{
		pTmpWidget1 = pTmpWidget1->GetParent();
	}

	if (pTmpWidget1)
	{
		if (pTmpWidget1->PickWidget(m_MousePos, ppWidget, FlagMask, pExcludeWidgets))
		{
			const CUiLayer* pInLayer = (*ppWidget)->getLayer();
			if (pExcludeLayers && pInLayer)
			{
				UiLayerList::const_iterator iterExLayer;
				for (iterExLayer = pExcludeLayers->begin(); iterExLayer != pExcludeLayers->end(); ++iterExLayer)
				{
					CUiLayer* pTmpLayer = *iterExLayer;
					if (pInLayer == pTmpLayer)
						break;
				}
				if (iterExLayer == pExcludeLayers->end())
					return true;
			}
			else
			{
				return true;
			}

			DROP_RTUI_OBJECT(*ppWidget);
		}
	}

	if (m_pModalWidgetContainer->getWidgetCount() > 0)
	{
		if (m_pModalWidgetContainer->PickWidget(m_MousePos, ppWidget, FlagMask, pExcludeWidgets))
		{
			return true;
		}
		else
		{
			return false;
		}
	}
	else if (m_pTopRenderWidgetContainer->PickWidget(m_MousePos, ppWidget, FlagMask, pExcludeWidgets))
	{
		const CUiLayer* pInLayer = (*ppWidget)->getLayer();
		if (pExcludeLayers && pInLayer)
		{
			UiLayerList::const_iterator iterExLayer;
			for (iterExLayer = pExcludeLayers->begin(); iterExLayer != pExcludeLayers->end(); ++iterExLayer)
			{
				CUiLayer* pTmpLayer = *iterExLayer;
				if (pInLayer == pTmpLayer)
					break;
			}
			if (iterExLayer == pExcludeLayers->end())
				return true;
		}
		else
		{
			return true;
		}

		DROP_RTUI_OBJECT(*ppWidget);
	}

	if (m_pDefaultWidgetContainer->PickWidget(m_MousePos, ppWidget, FlagMask, pExcludeWidgets))
	{
		return true;
	}
	else
	{
		foreach(UiLayerList, iterLayer, m_Layers)
		{
			CUiLayer* pLayer = *iterLayer;

			if (pExcludeLayers)
			{
				UiLayerList::const_iterator iterExLayer;
				for (iterExLayer = pExcludeLayers->begin(); iterExLayer != pExcludeLayers->end(); ++iterExLayer)
				{
					if (pLayer == *iterExLayer)
						break;
				}
				if (iterExLayer != pExcludeLayers->end())
					continue;
			}

			if (pLayer->PickWidget(m_MousePos, ppWidget, FlagMask, pExcludeWidgets))
			{
				if (ppLayer)
				{
					pLayer->grab();
					*ppLayer = pLayer;
				}
				return true;
			}
		}
	}

	return false;
}

//设置焦点组件
void CUiDesktop::SetFocusWidget(RtwWidget* pWidget)
{
	if (pWidget == m_pFocusWidget)
		return;

	if (m_pFocusWidget)
	{
		RtwWidget* pParent = m_pFocusWidget;
		while (pParent && pParent->GetParent())
		{
			pParent = pParent->GetParent();
		}
		RemoveTopRenderWidget(pParent->getId());

		m_pFocusWidget->EvUnFocus(m_pFocusWidget, etUnFocus); // 指针可能会被消息处理函数给毁掉
	}

	DROP_RTUI_OBJECT(m_pFocusWidget);

	if (pWidget)
	{
		InsertFocusWidget((RtwForm*)pWidget);
		m_pFocusWidget = pWidget;
		m_pFocusWidget->grab();

		if (m_pFocusWidget->GetFlags(wfFocus))
			m_pFocusWidget->EvFocus(m_pFocusWidget, weFocusIn);

		RtwWidget* pParent = pWidget; // focus的widget不能改变父子关系，否则TopRenderWidget会多出来一些
		while (pParent && pParent->GetParent())
		{
			pParent = pParent->GetParent();
		}
		AddTopRenderWidget(pParent);
	}
}

//打印组件树结构
void CUiDesktop::PrintWidgetTree()
{
	if (m_pModalWidgetContainer)
		m_pModalWidgetContainer->PrintToConsole("ModalWidgets");

	if (m_pDefaultWidgetContainer)
		m_pDefaultWidgetContainer->PrintToConsole("Widgets");

	if (m_pTopRenderWidgetContainer)
		m_pTopRenderWidgetContainer->PrintToConsole("TopRenderWidgets");

	if (m_pGarbageContainer)
		m_pGarbageContainer->PrintToConsole("Garbages");

	foreach(UiLayerList, iterLayer, m_Layers)
	{
		CUiLayer* pLayer = *iterLayer;
		pLayer->PrintToConsole();
	}
}

//通过名字显示树结构
void CUiDesktop::PrintWidgetTree(const std::string& LayerName)
{
	foreach(UiLayerList, iterLayer, m_Layers)
	{
		CUiLayer* pLayer = *iterLayer;
		if (pLayer->getName() == LayerName)
		{
			pLayer->PrintToConsole();
			return;
		}
	}
}

//顶层渲染组件
void CUiDesktop::AddTopRenderWidget(RtwWidget* pWidget)
{
	UI_ENSURE(pWidget);
	if (m_pTopRenderWidgetContainer)
	{
		m_pTopRenderWidgetContainer->AddWidget(pWidget);
	}
}

//根据id删除顶层渲染组件
void CUiDesktop::RemoveTopRenderWidget(DWORD WidgetId)
{
	if (m_pTopRenderWidgetContainer)
		m_pTopRenderWidgetContainer->RemoveWidget(WidgetId, false);
}

//查看是否有指定id的顶层渲染组件
bool CUiDesktop::FindInTopRenderContainer(DWORD WidgetId)
{
	return m_pTopRenderWidgetContainer->FindWidget(WidgetId);
}

//添加模态组件
void CUiDesktop::AddModalWidget(RtwWidget* pWidget)
{
	UI_ENSURE(pWidget);
	if (m_pModalWidgetContainer)
		m_pModalWidgetContainer->AddWidget(pWidget);
}

//根据id删除模态组件
void CUiDesktop::RemoveModalWidget(DWORD WidgetId)
{
	if (m_pModalWidgetContainer)
		m_pModalWidgetContainer->RemoveWidget(WidgetId, false);
}

//判断是否有指定id的模态窗体
bool CUiDesktop::FindInModalContainer(DWORD WidgetId)
{
	if (m_pModalWidgetContainer)
		return m_pModalWidgetContainer->FindWidget(WidgetId);
	return false;
}

//添加废弃组件
void CUiDesktop::AddGarbageWidget(RtwWidget* pWidget)
{
	if (m_pGarbageContainer)
		m_pGarbageContainer->AddWidget(pWidget);
}

//清理废弃的组件
void CUiDesktop::DoGarbageClear()
{
	UiWidgetList GarbageToClear;
	UiWidgetList::iterator iterWidget = m_pGarbageContainer->Begin();
	for (; iterWidget != m_pGarbageContainer->End(); ++iterWidget)
	{
		RtwWidget* pWidget = *iterWidget;
		if (pWidget->getRefCount() == 1)
			GarbageToClear.push_back(pWidget);
	}

	foreach(UiWidgetList, iterClear, GarbageToClear)
	{
		RtwWidget* pWidget = *iterClear;
		m_pGarbageContainer->RemoveWidget(pWidget->getId());
	}
}

//模态组件的鼠标事件(根据代码分析,可能是模态组件管理器中,是否有可见的组件)
bool CUiDesktop::_ProcessModalWidget_MouseEvent()
{
	for (UiWidgetList::iterator iter = m_pModalWidgetContainer->Begin(); \
		iter != m_pModalWidgetContainer->End(); ++iter)
	{
		RtwWidget* pWidget = *iter;
		if (pWidget->getVisible())
		{
			return true;
		}
	}
	return false;
}

//鼠标按下事件
bool CUiDesktop::MouseDown(int button)
{
	RtwEventDelegate e;
	e.type = weMouseDown;
	e.mouse.x = m_MousePos.x;
	e.mouse.y = m_MousePos.y;
	e.mouse.button = button;

	if (button == buttonRight)
		m_bMouseRDown = true;

	else if (button == buttonLeft)
		m_bMouseLDown = true;

	// 处理Layer
	foreach(UiLayerList, iterLayer, m_Layers)
	{
		(*iterLayer)->OnMouseDown(e);
	}

	// 处理ButtonDown
	bool bProcessed = false;
	DROP_RTUI_OBJECT(m_pMouseCaptureWidget);

	bool bPickOk = PickWidget(&m_pMouseCaptureWidget, NULL, wfGrabMouse | wfPick);
	if (bPickOk)
	{
		//如果有穿透效果,则捡取不成功
		if (m_pMouseCaptureWidget->GetFlags(wfPenetrate))
		{
			bPickOk = false;
		}
	}

	if (bPickOk)
	{
		if (m_pDefaultPopupMenu && m_pDefaultPopupMenu->getVisible() && m_pMouseCaptureWidget != m_pDefaultPopupMenu)
		{
			// 如果默认菜单显示,并且鼠标没有点中默认菜单,就关闭它
			if (!m_pDefaultPopupMenu->FindChild(m_pMouseCaptureWidget->getId(), NULL, true))
				HidePopupMenu();
		}

		if (m_pMouseCaptureWidget->getEnable())
		{
			//被点击的控件可用
			bProcessed = true;

			//让关闭按钮点击时,不区分是否有焦点
			//begin
			RtwWidget* pParent = m_pMouseCaptureWidget;
			RtwButton* pCloseButton = NULL;

			while (pParent && pParent->GetParent())
			{
				pParent = pParent->GetParent();
			}

			if (pParent->getWidgetType() == wtForm)
			{
				RtwForm* pForm = (RtwForm*)pParent;
				if (pForm->getShowCloseButton())
				{
					pCloseButton = pForm->getCloseButton();

					if (pCloseButton->getClientRect().IsContained(m_MousePos.x, m_MousePos.y))
					{
						DROP_RTUI_OBJECT(m_pMouseCaptureWidget);
						m_pMouseCaptureWidget = pCloseButton;
						m_pMouseCaptureWidget->grab();
						m_pMouseCaptureWidget->OnMouseDown(e);

						if (m_bModeWnd)
						{
							bProcessed = true;
						}

						if (_ProcessModalWidget_MouseEvent())
						{
							bProcessed = true;
						}

						return bProcessed;
					}
				}
			}
			//end

			if (m_pMouseCaptureWidget->GetFlags(wfFocus))
			{
				//可获取焦点
				SetFocusWidget(m_pMouseCaptureWidget);
			}

			if (m_pMouseCaptureWidget->GetFlags(wfGrabMouse))
			{
				//有鼠标事件
				m_pMouseCaptureWidget->OnMouseDown(e);
			}

			if (m_pSound)
			{
				if (!m_pMouseCaptureWidget->getSoundFile().empty())
				{
					m_pSound->PlaySound(m_pMouseCaptureWidget->getSoundFile());
				}
			}
		}
	}
	else
	{
		SetFocusWidget(NULL);
		HidePopupMenu();
	}

	if (m_bModeWnd)
	{
		bProcessed = true;
	}

	if (_ProcessModalWidget_MouseEvent())
	{
		bProcessed = true;
	}

	return bProcessed;
}

//按钮抬起事件
bool CUiDesktop::MouseUp(int button)
{
	RtwEventDelegate e;
	e.type = etMouseUp;
	e.mouse.x = m_MousePos.x;
	e.mouse.y = m_MousePos.y;
	e.mouse.button = button;

	if (button == buttonRight)
		m_bMouseRDown = false;
	else if (button == buttonLeft)
		m_bMouseLDown = false;

	// 处理Layer
	foreach(UiLayerList, iterLayer, m_Layers)
	{
		(*iterLayer)->OnMouseUp(e);
	}

	// 处理Widget
	bool bProcessed = false;
	RtwWidget* pPickWidget = NULL;
	PickWidget(&pPickWidget, NULL, wfGrabMouse | wfPick);

	// 处理Drag
	if (button == buttonRight)
	{
		//右键打断当前被拖拽的操作
		if (m_pMouseDragWidget)
		{
			RtwEventDelegate e(etDragNull);
			m_pMouseDragWidget->OnDragNull(e);
			SetMouseDragWidget(NULL);
		}
	}
	else if (button == buttonLeft)
	{
		if (m_pMouseDragWidget)
		{
			//被拖拽的组件
			if (m_pMouseDragWidget && m_pMouseDragWidget->getFlag(wfDragOut))
			{
				bProcessed = true;

				RtwEventDelegate e(etDragNull);
				m_pMouseDragWidget->OnDragNull(e);
			}

			if (m_pMouseHoverWidget && m_pMouseHoverWidget->getFlag(wfDragIn))
			{
				//拖入
				bProcessed = true;
				RtwEventDelegate e(etDragIn);
				e.dragIn.pDragOutWidget = m_pMouseDragWidget;
				m_pMouseHoverWidget->OnDragIn(e);
				SetMouseDragWidget(NULL);
			}
		}
	}

	// 处理Click
	if (m_pMouseCaptureWidget)
	{
		if (!bProcessed)
		{
			if (m_pMouseCaptureWidget->getEnable())
			{
				bProcessed = true;
				m_pMouseCaptureWidget->OnMouseUp(e);

				if (pPickWidget == m_pMouseCaptureWidget)
				{
					e.type = weMouseClick;
					m_pMouseCaptureWidget->OnMouseClick(e);
				}
				else
				{
					//如果是closebutton,则相应
					RtwButton* pCloseButton = NULL;
					RtwWidget* pParent = pPickWidget;

					while (pParent && pParent->GetParent())
					{
						pParent = pParent->GetParent();
					}

					if (pParent)
					{
						if (pParent->getWidgetType() == wtForm)
						{
							RtwForm* pForm = (RtwForm*)pParent;
							if (pForm->getShowCloseButton())
							{
								pCloseButton = pForm->getCloseButton();

								if (pCloseButton == m_pMouseHoverWidget)
								{
									e.type = weMouseClick;
									m_pMouseCaptureWidget->OnMouseClick(e);
								}
							}
						}
					}
				}
			}
		}
	}
	DROP_RTUI_OBJECT(m_pMouseCaptureWidget);

	// 处理超链接
	if (m_pRefreshMediaFrame)
	{
		m_pRefreshMediaFrame->ProcessNextHyperLink();
		SetRefreshMediaFrame(NULL);
	}
	DROP_RTUI_OBJECT(pPickWidget);

	return bProcessed;
}

//双击事件
bool CUiDesktop::MouseDClick(int button)
{
	RtwEventDelegate e(etMouseDClick);
	e.mouse.button = button;
	e.mouse.x = m_MousePos.x;
	e.mouse.y = m_MousePos.y;

	// 处理Layer
	foreach(UiLayerList, iterLayer, m_Layers)
	{
		(*iterLayer)->OnMouseDClick(e);
	}

	// 处理Widget
	bool bProcessed = false;
	if (m_pMouseHoverWidget)
	{
		if (m_pMouseHoverWidget->GetFlags(wfDClick))
		{
			//可以双击
			bProcessed = true;
			m_pMouseHoverWidget->OnMouseDClick(e);
		}
		else if (m_pMouseHoverWidget->GetFlags(wfGrabMouse))
		{
			// 有鼠标事件
			bProcessed = true;
			e.type = weMouseDown;
			m_pMouseHoverWidget->OnMouseDown(e);
		}
	}

	return bProcessed;
}

//鼠标移动
bool CUiDesktop::MouseMove(int x, int y)
{
	m_MouseDelta.width = x - m_MousePos.x;
	m_MouseDelta.height = y - m_MousePos.y;
	m_MousePos.x = x;
	m_MousePos.y = y;

	RtwEventDelegate e;
	e.type = etMouseMove;
	e.mouse.x = m_MousePos.x;
	e.mouse.y = m_MousePos.y;

	if (m_bMouseLDown)
		e.mouse.button = buttonLeft;
	else if (m_bMouseRDown)
		e.mouse.button = buttonRight;

	// 处理Layer
	foreach(UiLayerList, iterLayer, m_Layers)
	{
		(*iterLayer)->OnMouseMove(e);
	}

	// 处理Widget
	RtwWidget* pWidget = NULL;
	PickWidget(&pWidget, NULL, wfGrabMouse | wfMouseMove | wfPick);

	//让关闭按钮总是在移动到其位置时高亮,不区分是否有焦点
	//begin
	if (pWidget)
	{
		RtwWidget* pParent = pWidget;
		RtwButton* pCloseButton = NULL;

		while (pParent && pParent->GetParent())
		{
			pParent = pParent->GetParent();
		}

		if (pParent->getWidgetType() == wtForm)
		{
			RtwForm* pForm = (RtwForm*)pParent;
			if (pForm->getShowCloseButton())
			{
				pCloseButton = pForm->getCloseButton();

				if (pCloseButton->getClientRect().IsContained(x, y))
				{
					m_pMouseHoverWidget = pCloseButton;
					m_pMouseHoverWidget->grab();

					if (m_pMouseHoverWidget && m_pMouseHoverWidget->GetFlags(wfMouseMove))
					{
						m_pMouseHoverWidget->OnMouseMove(e);
					}

					if (pWidget)
						DROP_RTUI_OBJECT(pWidget);

					return true;	//直接返回
				}
			}
		}
	}
	//end

	if (m_pMouseHoverWidget != pWidget)
	{
		if (m_pMouseHoverWidget)
		{
			DROP_RTUI_OBJECT(m_pMouseHoverWidget);
		}

		if (pWidget)
		{
			m_pMouseHoverWidget = pWidget;
			m_pMouseHoverWidget->grab();
		}
	}

	if (pWidget)
		DROP_RTUI_OBJECT(pWidget);

	if (m_pMouseCaptureWidget)
	{
		if (m_pMouseCaptureWidget->GetFlags(wfMouseMove))
			m_pMouseCaptureWidget->OnMouseMove(e);
	}
	else
	{
		if (m_pMouseHoverWidget && m_pMouseHoverWidget->GetFlags(wfMouseMove))
			m_pMouseHoverWidget->OnMouseMove(e);
	}

	if (m_pMouseHoverWidget != m_pHintTargetWidget)
	{
		//鼠标划过的窗体不是hint窗体?
		if (m_pMouseHoverWidget)
		{
			if (m_pHintTargetWidget)
			{
				if (m_pHintTargetWidget->getHint())
					m_pHintTargetWidget->getHint()->Hide();
				DROP_RTUI_OBJECT(m_pHintTargetWidget);
			}

			if (m_pMouseHoverWidget->getFlag(wfHint, false))
			{
				m_pHintTargetWidget = m_pMouseHoverWidget;
				m_pHintTargetWidget->grab();

				m_pHintTargetWidget->OnHint();
			}
		}
		else
		{
			if (m_pHintTargetWidget)
			{
				if (m_pHintTargetWidget->getHint())
					m_pHintTargetWidget->getHint()->Hide();
				DROP_RTUI_OBJECT(m_pHintTargetWidget);
			}
		}
	}

	//拖拽
	if (m_pMouseCaptureWidget && m_pMouseCaptureWidget->getFlag(wfDragOut) && !m_pMouseDragWidget && !m_bMouseRDown)
	{
		if (m_pMouseDragWidget != m_pMouseCaptureWidget)
		{
			SetMouseDragWidget(m_pMouseCaptureWidget);
			m_pMouseCaptureWidget->OnDragOut(e);
		}
	}

	if (!m_pHintTargetWidget && m_pHintWidget)
	{
		m_pHintWidget->Hide();
		DROP_RTUI_OBJECT(m_pHintWidget);
	}

	return true;
}

//鼠标滚轮事件
bool CUiDesktop::MouseWheel(int delta, int x, int y)
{
	RtwEventDelegate e(etMouseWheel);
	e.mouseWheel.delta = delta;
	e.mouse.x = x;
	e.mouse.y = y;

	// 处理Layer
	foreach(UiLayerList, iterLayer, m_Layers)
	{
		(*iterLayer)->OnMouseWheel(e);
	}

	// 处理Widget
	bool bProcessed = false;
	if (m_pFocusWidget)
	{
		bProcessed = true;
		m_pFocusWidget->OnMouseWheel(e);
	}

	return bProcessed;
}

//字符输入
bool CUiDesktop::KeyChar(int ch)
{
	RtwEventDelegate e(etKeyChar);
	e.key.code = ch;

	// 处理Layer
	foreach(UiLayerList, iterLayer, m_Layers)
	{
		(*iterLayer)->OnKeyChar(e);
	}

	// 处理Widget
	bool bProcessed = false;
	if (m_pFocusWidget && m_pFocusWidget->GetFlags(wfKeyboardEvent))
	{
		//有焦点,且有键盘事件
		bProcessed = true;
		m_pFocusWidget->OnKeyChar(e);
	}

	return bProcessed;
}

bool CUiDesktop::KeyDown(int key)
{
	RtwEventDelegate e(etKeyDown);
	e.key.code = key;

	// 处理Layer
	foreach(UiLayerList, iterLayer, m_Layers)
	{
		(*iterLayer)->OnKeyDown(e);
	}

	// 处理Widget
	bool bProcessed = false;
	if (m_pFocusWidget && m_pFocusWidget->GetFlags(wfKeyboardEvent))
	{
		//有焦点,且有键盘事件
		bProcessed = true;
		m_pFocusWidget->OnKeyDown(e);
	}

	return bProcessed;
}

//键盘抬起
bool CUiDesktop::KeyUp(int key)
{
	RtwEventDelegate e(etKeyUp);
	e.key.code = key;

	if (key == VK_ESCAPE)
	{
		// 		RtCoreLog().Error("esc begin\n");
		if (!m_lstFocusWidgetList.empty())
		{
			// 			RtCoreLog().Error("!m_lstFocusWidgetList.empty()\n");
			RtwForm* pWidget = m_lstFocusWidgetList.front();
			//不是form或者是form并且可以用esc关闭
			if (pWidget->GetEscapeHide())
			{
				// 				RtCoreLog().Error("!m_lstFocusWidgetList.empty() Hide\n");
				pWidget->Hide();
				if (!m_lstFocusWidgetList.empty())
				{
					// 					RtCoreLog().Error("!m_lstFocusWidgetList.empty() setFocus\n");
										//SetFocusWidget(m_lstFocusWidgetList.front());
				}
			}
		}
		else
		{
			RtCoreLog().Error("m_lstFocusWidgetList. is empty()\n");
			if (m_pNoFocusWidget)
			{
				RtCoreLog().Error("m_pNoFocusWidget is not null\n");
				m_pNoFocusWidget->Show();
				// 				SetFocusWidget(m_pNoFocusWidget);
			}
		}
	}

	// 处理Layer
	foreach(UiLayerList, iterLayer, m_Layers)
	{
		(*iterLayer)->OnKeyUp(e);
	}

	// 处理Widget
	bool bProcessed = false;
	if (m_pFocusWidget && m_pFocusWidget->GetFlags(wfKeyboardEvent))
	{
		//有焦点,且有键盘事件
		bProcessed = true;
		m_pFocusWidget->OnKeyUp(e);
	}

	return bProcessed;
}

//插入
void CUiDesktop::InsertFocusWidget(RtwForm* pWidget)
{
	//如果查找到了,说明已经存在了,则是切换focus状态
	//先删除,后添加
	if (pWidget && pWidget->getWidgetType() == wtForm && pWidget->GetEscapeHide())
	{
		DeleteFocusWidget(pWidget);

		m_lstFocusWidgetList.push_front(pWidget);
	}
}

//删除
void CUiDesktop::DeleteFocusWidget(RtwForm* pWidget)
{
	std::list<RtwForm*>::iterator vri = find(
		m_lstFocusWidgetList.begin(),
		m_lstFocusWidgetList.end(),
		pWidget
	);

	if (vri == m_lstFocusWidgetList.end())
	{
		return;
	}

	m_lstFocusWidgetList.erase(vri);
	m_lstFocusWidgetList.swap(m_lstFocusWidgetList);
}

//设置鼠标拖拽组件
void CUiDesktop::SetMouseDragWidget(RtwWidget* pWidget)
{
	DROP_RTUI_OBJECT(m_pMouseDragWidget);

	m_pMouseDragWidget = pWidget;

	if (m_pMouseDragWidget)
		m_pMouseDragWidget->grab();
}

//设置鼠标滑过组件
void CUiDesktop::SetMouseHoverWidget(RtwWidget* pWidget)
{
	DROP_RTUI_OBJECT(m_pMouseHoverWidget);

	m_pMouseHoverWidget = pWidget;

	if (m_pMouseHoverWidget)
		m_pMouseHoverWidget->grab();
}

//设置提示框组件
void CUiDesktop::SetHintWidget(RtwWidget* pWidget)
{
	DROP_RTUI_OBJECT(m_pHintWidget);

	m_pHintWidget = pWidget;

	if (m_pHintWidget)
		m_pHintWidget->grab();
}

//通过组件名获取组件类型
EWidgetType CUiDesktop::getWidgetTypeFromTypeName(const std::string& TypeName)
{
	for (int i = 0; i < wtCount; ++i)
	{
		if (g_WidgetTypeName[i] == TypeName)
			return (EWidgetType)i;
	}
	return wtUnknown;
}

//通过短名获取组件类型
EWidgetType CUiDesktop::getWidgetTypeFromTypeShortName(const std::string& TypeShortName)
{
	for (int i = 0; i < wtCount; ++i)
	{
		if (g_WidgetTypeShortName[i] == TypeShortName)
			return (EWidgetType)i;
	}
	return wtUnknown;
}

//处理xml节点
bool CUiDesktop::_ProcessXmlFile(RtsXmlDoc::NodePtr* pRoot)
{
	const std::string& TagName = pRoot->strName;
	UI_ENSURE_B(TagName == "Ui");

	RtsXmlDoc::NodePtr* pChildNode = pRoot->pChildren;
	while (pChildNode)
	{
		if (pChildNode->strName == "Loader")
		{
			if (!_ProcessXml_Loader(pChildNode))
			{
				return false;
			}
		}
		else if (pChildNode->strName == "Config")
		{
			UI_ENSURE_B(_ProcessXml_Config(pChildNode))
		}

		pChildNode = pChildNode->pNext;
	}
	return true;
}

//返回可视范围矩形
RtwRect CUiDesktop::getViewportRect()
{
	if (!m_pRenderDevice)
	{
		return RtwRect(0, 0, 0, 0);
	}

	return m_pRenderDevice->getViewportRect();
}

bool CUiDesktop::_ProcessXml_Loader(RtsXmlDoc::NodePtr* pNode)
{
	RtsXmlDoc::NodePtr* pChildNode = pNode->pChildren;
	while (pChildNode)
	{
		if (pChildNode->strName == "File")
		{
			std::string FileName = pChildNode->GetProp_s("Name");
			UI_ENSURE_B(m_pUiXmlLoader->LoadFromFile(FileName));
		}
		pChildNode = pChildNode->pNext;
	}
	return true;
}

bool CUiDesktop::_ProcessXml_Config(RtsXmlDoc::NodePtr* pNode)
{
	return true;
}

//显示弹出式菜单
void CUiDesktop::ShowPopupMenu()
{
	if (m_pDefaultPopupMenu)
		ShowPopupMenu(m_pDefaultPopupMenu, NULL, m_MousePos.x, m_MousePos.y);
}

//显示指定的弹出式菜单
void CUiDesktop::ShowPopupMenu(RtwPopupMenu* menu, void* xx, int x, int y)
{
	RtwRect rc;
	RtwRect rcHint = menu->GetFrameRect();

	rc.left = x;

	if (rc.left < 0)
		rc.left = 0;

	rc.right = rc.left + rcHint.getWidth() - 1;

	rc.top = y - 10 - rcHint.getHeight();

	if (rc.top < 0)
		rc.top = 0;

	rc.bottom = rc.top + rcHint.getHeight() - 1;

	if (rc.right > g_workspace.getViewportRect().right)
	{
		rc.Offset(g_workspace.getViewportRect().right - rc.right - 2, 0);
	}

	if (rc.bottom > g_workspace.getViewportRect().bottom)
	{
		rc.Offset(0, g_workspace.getViewportRect().bottom - rc.bottom - 2);
	}

	menu->Move(SPoint(x, y));
	menu->Refresh();
	menu->Show();
	menu->SetFocus();
}

//隐藏菜单
void CUiDesktop::HidePopupMenu()
{
	if (m_pDefaultPopupMenu)
		m_pDefaultPopupMenu->Hide();
}

//设置默认焦点组件
void CUiDesktop::SetDefaultFocusWidget(RtwWidget* pDefault)
{
	if (m_pDefaultFocusWidget == pDefault)
		return;

	DROP_RTUI_OBJECT(m_pDefaultFocusWidget);

	if (pDefault)
	{
		m_pDefaultFocusWidget = pDefault;
		m_pDefaultFocusWidget->grab();
	}
}

void CUiDesktop::SetRefreshMediaFrame(CUiMediaFrame* pFrame)
{
	if (m_pRefreshMediaFrame)
	{
		DROP_RTUI_OBJECT(m_pRefreshMediaFrame);
	}

	if (pFrame)
	{
		m_pRefreshMediaFrame = pFrame;
		m_pRefreshMediaFrame->grab();
	}
}

//将指定组件置于顶层
void CUiDesktop::BringWidgetToTop(RtwWidget* pWidget)
{
	if (pWidget->getParent())
	{
		pWidget->getParent()->getChildren()->BringWidgetToTop(pWidget->getId());
	}
	else if (pWidget->getLayer())
	{
		pWidget->getLayer()->BringWidgetToTop(pWidget->getId());
	}
	else
	{
		m_pDefaultWidgetContainer->BringWidgetToTop(pWidget->getId());
	}
}

//将指定组件置于底层
void CUiDesktop::BringWidgetToBottom(RtwWidget* pWidget)
{
	if (pWidget->getParent())
	{
		pWidget->getParent()->getChildren()->BringWidgetToBottom(pWidget->getId());
	}
	else if (pWidget->getLayer())
	{
		pWidget->getLayer()->BringWidgetToBottom(pWidget->getId());
	}
	else
	{
		m_pDefaultWidgetContainer->BringWidgetToBottom(pWidget->getId());
	}
}

//注册图片路径
bool CUiDesktop::RegisterTexturePath(const std::string& TexturePath)
{
	stlforeach(std::list<std::string>, iter, m_DefaultTexturePaths)
	{
		if (*iter == TexturePath)
		{
			return true;
		}
	}

	m_DefaultTexturePaths.push_back(TexturePath);
	return true;
}

//注册xml路径
bool CUiDesktop::RegisterXmlPath(const std::string& XmlPath)
{
	stlforeach(std::list<std::string>, iter, m_DefaultXmlPaths)
	{
		if (*iter == XmlPath)
		{
			return true;
		}
	}

	m_DefaultXmlPaths.push_back(XmlPath);
	return true;
}

//重置提示框组件
void CUiDesktop::ResetHintTargetWidget()
{
	if (m_pHintTargetWidget)
	{
		if (m_pHintTargetWidget->getHint())
			m_pHintTargetWidget->getHint()->Hide();

		DROP_RTUI_OBJECT(m_pHintTargetWidget);
	}
}

//设置hint文字
void CUiDesktop::SetHintText(const std::string& HintText, bool b)
{
	if (m_pHintTargetWidget)
	{
		m_pHintTargetWidget->SetHintText(HintText);
	}
}

//hint自动大小
void CUiDesktop::AutoHintSize()
{
	if (m_pHintTargetWidget)
		m_pHintTargetWidget->AutoHintSize();
}

//设置hint的起始位置
void CUiDesktop::SetHintPosition(int x, int y)
{
	if (m_pHintTargetWidget)
		m_pHintTargetWidget->AutoHintPosition();
}

//获取鼠标位置
//begin
int CUiDesktop::GetMousePosX()
{
	return getMousePos().x;
}

int CUiDesktop::GetMousePosY()
{
	return getMousePos().y;
}
//end

void CUiDesktop::ShowHint(RtwWidget* pWidget, int x) {}
void CUiDesktop::HideHint(int x) {}
void CUiDesktop::SetActiveWidget(RtwWidget* pWidget) {}

//根据名字搜索组件
RtwWidget* CUiDesktop::LookupWidget(const std::string szName)
{
	RtwWidget* vpUI = 0;
	std::string strName = szName;
	if (strName.find("layworld") == std::string::npos)
		strName = "layworld." + strName;

	if (!g_workspace.FindWidget(strName, &vpUI))
	{
		string Temp("Can't find widget [");
		Temp = Temp + szName + "]!!!!!!!!!!!!!!!!!!!";
		RtCoreLog().Error(Temp.c_str());
		CHECKEX(Temp.c_str());
		return NULL;
	}
	return  vpUI;
}

//设置鼠标点中的组件指针
void CUiDesktop::SetMouseCapture(RtwWidget* pkWidget)
{
	m_pMouseCaptureWidget = pkWidget;
}

//设置提示框大小
void CUiDesktop::SetHintSize(int x, int y)
{
	if (m_pHintTargetWidget)
	{
		if (m_pHintTargetWidget->getHint())
		{
			m_pHintTargetWidget->getHint()->SetWidgetSize(SSize(x, y));
		}
		else
			getDefaultHint()->SetWidgetSize(SSize(x, y));
	}
}