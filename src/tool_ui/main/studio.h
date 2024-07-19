#pragma once
#define WIN32_LEAN_AND_MEAN	
#define _WIN32_DCOM 
#ifndef _WIN32_WINNT
#define _WIN32_WINNT 0x0400           // π”√CoInitializeEx
#endif
#include <ObjBase.h>

#pragma warning(disable:4819)
#include <windows.h>
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>
#include <vector>
#include <list>
#include "core/rt2_core.h"
#include "graph/rt_graph.h"
#include "audio/rt_audio.h"

#include "ui/rtw_ui.h"

#include "resource.h"
#include "main_frame.h"
#include "studio_xmlwriter.h"
#include "uimain.h"
#include "studio_process.h"
#include "cmylog.h"
