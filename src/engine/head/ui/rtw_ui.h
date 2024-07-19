#ifndef _RTUI_UI_H
#define _RTUI_UI_H
//********************************************************************
//	created:	2010.04.07 10:00
//	filename: 	ui.h
//	author:		ldr123
//	purpose:	ui用到的类和头文件
//  other:		局部重构添加注释
//********************************************************************

namespace ui 
{
	class RtwWidget;
	class RtwButton;
	class RtwLabel;
	class CUiLayer;
	class RtwForm;
	class RtwPopupMenu;
	class CUiDesktop;
	class CUiWidgetFactory;
	class IUiFontManager;
	class RtwMailBox;
	class RtwChatInputBox;
	class CUiMediaFrame;
	class RtwVScrollBar;
	class RtwHScrollBar;
	class CUiRichText;
	class CUiHtmlText;
	class RtwHtmlView;
	class RtwChatBox;
	class RtwChatInputBox;
	class Rtw3DView;
}

#ifdef _DEBUG
	#define UI_CHECKEX(X) RtCoreLog().Error((X));
	#define UI_CHECK(X) {if (!(X)) return;}
	#define UI_CHECK_P(X) {if (!(X)) return NULL;}
	#define UI_CHECK_B(X) {if (!(X)) return false;}
#else
	#define UI_CHECKEX(X) return;
	#define UI_CHECK(X) {if (!(X)) return;}
#define UI_CHECK_P(X) {if (!(X)) return NULL;}
#define UI_CHECK_B(X) {if (!(X)) return false;}
#endif

// SDK
#include <string>
#include <list>
#include <vector>
#include <stdlib.h>

//core
#include "core/rt2_core.h"

// Base
#include "rtw_ptr.h"
#include "rtw_base.h"
#include "rtw_delegate.h"
#include "rtw_hyperlink.h"
#include "rtw_event.h"
#include "rtw_font.h"
#include "rtw_image.h"
#include "rtw_widget.h"
#include "rtw_layer.h"
#include "rtw_clipboard.h"
#include "rtw_workspace.h"
#include "rtw_template.h"
#include "rtw_theme.h"
#include "rtw_device2d.h"

// Control
#include "rtw_scrollbar.h"
#include "rtw_progressbar.h"
#include "rtw_label.h"
#include "rtw_button.h"
#include "rtw_form.h"
#include "rtw_textbox.h"
#include "rtw_listbox.h"
#include "rtw_combobox.h"
#include "rtw_tab.h"
#include "rtw_tree.h"
#include "rtw_popup_menu.h"
#include "rtw_xml.h"
#include "rtw_mailbox.h"
#include "rtw_chatinputbox.h"

//new
#include "rtw_hud.h"
#include "rtw_chatbox.h"
#include "rtw_htmlview.h"
#include "rtw_iconlist.h"
#include "rtw_hyperlink.h"
#include "rtw_mediaframe.h"
#include "rtw_richtext.h"
#include "rtw_htmlreflow.h"

//clipboard
#include "rtw_clipboard.h"

//sound
#include "rtw_sound.h"

// 3d view
#include "rtw_3dview.h"

using namespace ui;
using namespace std;
#endif 
