//********************************************************************
//	created:	24:12:2009   14:18
//	filename: 	script_engine_py.cpp
//	author:		tiamo
//	purpose:	script engine
//********************************************************************

#include "region.h"
#include "script_engine_py.h"
#include "region_object_py.h"
#include "region_creature_py.h"
#include "region_scene_py.h"
#include "region_server_py.h"
#include "region_item_py.h"
#include "region_activity_py.h"

//
// singleton
//
CScriptEngine* CScriptEngine::m_pSingleton									= 0;

//
// constructor
//
CScriptEngine::CScriptEngine()
{
}

//
// destructor
//
CScriptEngine::~CScriptEngine()
{
}

//
// get singleton
//
CScriptEngine* CScriptEngine::GetSingleton()
{
	if(!m_pSingleton)
		m_pSingleton														= RT_NEW CScriptEngine;

	return m_pSingleton;
}

//
// start
//
bool CScriptEngine::Start()
{
	//
	// init pyton
	//
	Py_Initialize();

	//
	// setup module methods
	//
	static PyMethodDef moduleMethods[] = 
	{
		{"Log",				CScriptEngine::LogMessage,						METH_O,					""},
		{"ServerTime",		CScriptEngine::ServerTime,						METH_VARARGS,			""},
		{"RandomValue",		CScriptEngine::RandomValue,						METH_O,					""},
		{"Broadcast",		CScriptEngine::BroadcastBulletin,				METH_VARARGS,			""},
		{"BroadcastHTML",	CScriptEngine::BroadcastHTML,					METH_O,					""},
		{0, 0},
	};

	//
	// init module
	//
	m_pModule																= Py_InitModule(SCRIPT_PYTHON_MODULE_NAME, moduleMethods);
	if(!m_pModule)
		return false;

	//
	// init types
	//
	CPythonRegionActivity::Initialize(m_pModule);
	CPythonRegionObject::Initialize(m_pModule);
	CPythonRegionCreature::Initialize(m_pModule);
	CPythonRegionScene::Initialize(m_pModule);
	CPythonRegionServer::Initialize(m_pModule);
	CPythonRegionItem::Initialize(m_pModule);

	//
	// setup script path
	//
	std::string scriptPath													= std::string("import sys\nsys.path.append('") + R(RES_OFFSET_PATH) + "')";
	PyRun_SimpleString(const_cast<char*>(scriptPath.c_str()));

	return true;
}

//
// stop
//
void CScriptEngine::Stop()
{
	//Py_Finalize();
	//这里Python在退出的时候会在垃圾清理的地方挂掉所以注释掉
	delete this;
	m_pSingleton		= 0;
}

//
// create object
//
PyObject* CScriptEngine::CreatePythonObject(CRegionObject* theObject)
{
	PyObject* ret		= 0;
	switch(OB_TYPE(theObject))
	{
	case OBJECT_TYPE_NPC_COMBATIVE:
	case OBJECT_TYPE_NPC_SERVICE:
	case OBJECT_TYPE_CREATURE:
		ret				= CPythonRegionCreature::CreatePythonObject(static_cast<CRegionCreature*>(theObject));
		break;

	case OBJECT_TYPE_SCENE_FB:
	case OBJECT_TYPE_SCENE:
	case OBJECT_TYPE_SCENE_WARFIELD:
		ret				= CPythonRegionScene::CreatePythonObject(static_cast<CRegionScene*>(theObject));
		break;

	case OBJECT_TYPE_ACTIVITY:
		ret				= CPythonRegionActivity::CreatePythonObject(static_cast<CRegionActivityBase*>(theObject));
		break;
	default:
	case OBJECT_TYPE_OBJECT:
		ret				= CPythonRegionObject::CreatePythonObject(theObject);
		break;
	}

	return ret;
}

//
// create object
//
PyObject* CScriptEngine::CreatePythonObject(SItemID const& item)
{
	return CPythonRegionItem::CreatePythonObject(item);
}

//
// return none
//
PyObject* CScriptEngine::ReturnNone()
{
	Py_INCREF(Py_None);
	return Py_None;
}

//
// log message
//
PyObject* CScriptEngine::LogMessage(PyObject* self, PyObject* args)
{
	RtCoreLog().Info("[script] %s\n", PyString_AsString(args));
	return ReturnNone();
}

//
// server time
//
PyObject* CScriptEngine::ServerTime(PyObject* self, PyObject* args)
{
	int iYear = 0;
	int iMonth = 0;
	int iDayOfWeek = 0;
	int iDay = 0;
	int iHour = 0;
	int iMin = 0;
	int iSec = 0;
	int iMSec = 0;
	rtSystemTime(iYear, iMonth, iDayOfWeek, iDay, iHour, iMin, iSec, iMSec);
	return Py_BuildValue("iiiiiiiil", iYear, iMonth, iDay, iDayOfWeek, iHour, iMin, iSec, iMSec, rtGetMilliseconds());
}

//
// return Random value x
//
PyObject* CScriptEngine::RandomValue(PyObject* self, PyObject* args)
{
	long lMaxValue = PyInt_AsLong(args);
	if(lMaxValue < 0)
		lMaxValue *= -1;

	//srand(rtGetMilliseconds());   ///< 随机种子
	long lValue = rand() % lMaxValue;
	return Py_BuildValue("i", lValue);
}

PyObject* CScriptEngine::BroadcastBulletin(PyObject* self, PyObject* args)
{
	int iSysMsg = 0;
	char *pMessage = 0;
	if(!PyArg_ParseTuple(args, "is", &iSysMsg, &pMessage))
		return PyErr_Format(PyExc_TypeError, "[BroadcastBulletin] invalid params");

	g_region->m_gws.BroadcastBulletin(pMessage, (char)iSysMsg);
	return CScriptEngine::ReturnNone();
}

PyObject* CScriptEngine::BroadcastHTML(PyObject* self, PyObject* args)
{
	static const char *sStartStr = "<html><head><meta http-equiv=\"Content-Type\" content=\"text/html; charset=gb2312\"/></head><body>";
	static const char *sEndStr = "</body></html>";
	static const char *sStartFag = "<p>";
	static const char *sEndFag = "</p>";
	static const char *sFlogStr = "@@";
	//static char sText[1024 * 10] = {0};
	S_MEMDEF(sText, 1024 * 10)
	S_MEMPROTECTOR(sText, 1024 * 10, bMP)

	char *sDialogText = PyString_AsString(args);
	if(!sDialogText || sDialogText[0] == 0)
		return PyErr_Format(PyExc_TypeError, "[BroadcastHTML] invalid params");
	rt2_strcpy(sText, sDialogText);
	std::string sCreateString(sStartStr);
	char *pNewSubString = strtok(sText, sFlogStr);
	if(pNewSubString)
	{
		do
		{
			sCreateString += sStartFag;
			sCreateString += pNewSubString;
			sCreateString += sEndFag;
			pNewSubString = strtok(NULL, sFlogStr);
		}while(pNewSubString);
	}
	else
	{
		sCreateString += sStartFag;
		sCreateString += sText;
		sCreateString += sEndFag;
	}
	sCreateString += sEndStr;

	g_sendCmd->BeginWrite();
	g_sendCmd->WriteShort(r2c_show_dialog);
	g_sendCmd->WriteLong(0);
	g_sendCmd->WriteShort(DIALOG_HTML);
	g_sendCmd->WriteString(sCreateString.c_str());
	g_sendCmd->WriteLong(0);
	g_sendCmd->WriteByte(0);
	g_sendCmd->WriteString("");
	g_sendCmd->WriteString("");
	g_sendCmd->WriteLong(0);
	g_region->BroadcastCmd(g_sendCmd, 0);

	return CScriptEngine::ReturnNone();
}
