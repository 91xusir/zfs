#ifndef _INC_UI_LAYER_H_
#define _INC_UI_LAYER_H_

#include "ui_layer_login.h"
#include "ui_layer_select_char.h"
#include "ui_layer_main.h"

class UILayer
{
public:
	static void Initialize();
	static void Reset();
	static void Clear();
    static void EnterSelectServer();
	static void EnterLogin();
	static void EnterSelectChar();
	static void EnterMain();
	static void EnterLoading();
	static void LeaveLoading();
	static void OnSetCursor(void*, void*);
};


extern UILayerLogin*		g_layerLogin;
extern UILayerSelectChar*	g_layerSelectChar;
extern UILayerMain*			g_layerMain;

#endif // _INC_UI_LAYER_H_
