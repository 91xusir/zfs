//********************************************************************
//	created:	24:12:2009   15:42
//	filename: 	region_scene_py.h
//	author:		tiamo
//	purpose:	python interface for region scene
//********************************************************************

#include "region.h"
#include "region_object_py.h"
#include "region_scene_py.h"
#include "script_engine_py.h"

#define _CHECK_SCENE_TYPE(ret)												REGION_OBJECT_PYTHON_CHECK_TYPE(self, CRegionScene*, selfScene, ret)

PyObject* CPythonRegionScene::GetSceneName(region_object_python* self, void* unused)
{
	_CHECK_SCENE_TYPE(0);
	return Py_BuildValue("s", selfScene->m_sceneName.c_str());
}

PyObject* CPythonRegionScene::GetAverageLV(region_object_python* self, void* unused)
{
	_CHECK_SCENE_TYPE(0);
	CRegionSceneFB *pFBScene = dynamic_cast<CRegionSceneFB*>(selfScene);
	if(pFBScene)
	{
		return Py_BuildValue("i", pFBScene->m_curUserLV);
	}
	return Py_BuildValue("i", 0);
}

//
// npc = AddNpc(int id, int x, int y, int dir)
//
PyObject* CPythonRegionScene::AddNpc(region_object_python* self, PyObject* args, PyObject* kw)
{
	//
	// get params
	//
	long id		= 0;
	long pos[2]	= {0, 0};
	long dir	= 0;
	long lv		= -1;
	PyObject *pList = 0;
	long lLifeTime = 0;
	//if(!PyArg_ParseTuple(args, "llllO", &id, pos, pos + 1, &dir, &pList))
	//	return PyErr_Format(PyExc_TypeError, "[AddNpc] invalid params");
	static char const* keywords[] = {"type", "x", "y", "dir", "lv", "att", "life", 0};
	if(!PyArg_ParseTupleAndKeywords(args, kw, "lll|llOl:AddNpc", (char**)keywords, &id, pos, pos + 1, &dir, &lv, &pList, &lLifeTime))
		return PyErr_Format(PyExc_TypeError, "[AddNpc] invalid params");

	const long ciMaxCount = 8;
	float fValue[ciMaxCount] = {-1};
	if(pList)
	{
		long lListSize = PyList_Size(pList);
		for(int ii=0; ii<lListSize && ii<ciMaxCount; ii++)
			fValue[ii] = PyFloat_AsDouble(PyList_GetItem(pList, ii));
	}

	// get scene
	_CHECK_SCENE_TYPE(0);

	SSceneNpc npcInfo;
	npcInfo.bEnable				= true;
	npcInfo.cDir				= static_cast<char>(dir);
	npcInfo.lID					= 0;
	npcInfo.pos[0]				= pos[0];
	npcInfo.pos[1]				= pos[1];
	npcInfo.sNpcId				= id;
	npcInfo.sSceneID			= selfScene->GetObjectId();
	npcInfo.sDif				= 0;

	CRegionCreature* theNpc		= selfScene->AddNpc(npcInfo, lv, fValue);
	if(!theNpc)
		return PyErr_Format(PyExc_RuntimeError, "[AddNpc] unable to create npc %d", id);
	if(lLifeTime > 1000)
		theNpc->SetTimer(CRE_TIMER_KILL_SELF, lLifeTime);
	
	return CScriptEngine::CreatePythonObject(theNpc);
}

//
// set default pos
//
PyObject* CPythonRegionScene::SetDefaultPos(region_object_python* self, PyObject* args)
{
	//
	// get params
	//
	long x																	= 0;
	long y																	= 0;
	if(!PyArg_ParseTuple(args, "ll", &x, &y))
		return PyErr_Format(PyExc_TypeError, "[SetDefaultPos] invalid params");

	//
	// get scene
	//
	_CHECK_SCENE_TYPE(0);

	selfScene->m_defPos[0]													= x;
	selfScene->m_defPos[1]													= y;

	return CScriptEngine::ReturnNone();
}

//
// find creature 
//
PyObject* CPythonRegionScene::FindCreatureById(region_object_python* self, PyObject* args)
{
	//
	// get scene
	//
	_CHECK_SCENE_TYPE(0);
	CRegionCreature* theCreature											= selfScene->FindCreature(PyInt_AsLong(args));
	if(!theCreature)
		return CScriptEngine::ReturnNone();

	return CScriptEngine::CreatePythonObject(theCreature);
}

//
// find npc by type 
//
PyObject* CPythonRegionScene::FindNpcByType(region_object_python* self, PyObject* args)
{
	//
	// get scene
	//
	_CHECK_SCENE_TYPE(0);
	std::vector<CRegionCreature*> npcs;
	selfScene->FindNpcByType(PyInt_AsLong(args), npcs);
	
	int count																= static_cast<int>(npcs.size());
	PyObject* retValue														= PyTuple_New(count);
	for(int i = 0; i < count; i ++)
		PyTuple_SetItem(retValue, i, CScriptEngine::CreatePythonObject(npcs[i]));

	return retValue;
}

//
// remove create by type 
//
PyObject* CPythonRegionScene::RemoveCreatureById(region_object_python* self, PyObject* args)
{
	//
	// get scene
	//
	_CHECK_SCENE_TYPE(0);
	CRegionCreature* theCreature											= selfScene->FindCreature(PyInt_AsLong(args));
	if(theCreature && !theCreature->IsUser())
	{
		theCreature->m_scene->RemoveCreature(theCreature->m_oId);
		g_factory->DestroyObject(theCreature);
	}

	return CScriptEngine::ReturnNone();
}

//
// Activation area by name
//
PyObject* CPythonRegionScene::ActivationAreaByName(region_object_python* self, PyObject* args)
{
	char *sAreaName = 0;
	long lActivation = 1;
	if(!PyArg_ParseTuple(args, "sl", &sAreaName, &lActivation))
		return PyErr_Format(PyExc_TypeError, "[ActivationAreaByName] invalid params");
	_CHECK_SCENE_TYPE(0);
	if(sAreaName && sAreaName[0] != 0 && selfScene->m_pTerrain)
	{
		RtsSceneBlockMap *pBlockMap = selfScene->m_pTerrain->FindSceneBlockMapByAreaName(sAreaName);
		if(pBlockMap)
			pBlockMap->ActivationBlockArea(sAreaName, lActivation != 0 ? true : false);
	}
	return CScriptEngine::ReturnNone();
}

PyObject* CPythonRegionScene::BroadcastShowTimer(region_object_python* self, PyObject* args, PyObject* kw)
{
	char *sTimeText = 0;
	long lTimer = 0;
	long lPlayerID = 0;
	static char const* keywords[] = {"text", "time", "player", 0};
	if(!PyArg_ParseTupleAndKeywords(args, kw, "sl|l:BroadcastShowTimer", (char**)keywords, &sTimeText, &lTimer, &lPlayerID))
		return PyErr_Format(PyExc_TypeError, "[BroadcastShowTimer] invalid params");
	//if(!PyArg_ParseTuple(args, "sl", &sTimeText, &lTimer))
	//	return PyErr_Format(PyExc_TypeError, "[BroadcastShowTimer] invalid params");
	_CHECK_SCENE_TYPE(0);
	//if(selfScene == g_region->m_defScene)
	//	return CScriptEngine::ReturnNone();
	if(lPlayerID > 0)
	{
		CRegionCreature *pPlayer = selfScene->FindCreature(lPlayerID);
		if(pPlayer && pPlayer->m_userInfo)
		{
			g_sendCmd->BeginWrite();
			g_sendCmd->WriteShort(r2c_show_timer);
			g_sendCmd->WriteString(sTimeText);
			g_sendCmd->WriteLong(lTimer);
			pPlayer->RecvCmd(g_sendCmd);
		}
	}
	else
		g_region->BroadcastShowTimerByScene(selfScene, sTimeText, lTimer);
	return CScriptEngine::ReturnNone();
}

PyObject* CPythonRegionScene::BroadcastSceneData(region_object_python* self, PyObject* args, PyObject* kw)
{
	PyObject *pListStr = 0;
	PyObject *pListVal = 0;
	long lPlayerID = 0;
	static char const* keywords[] = {"str", "val", "player", 0};
	if(!PyArg_ParseTupleAndKeywords(args, kw, "OO|l:BroadcastSceneData", (char**)keywords, &pListStr, &pListVal, &lPlayerID))
		return PyErr_Format(PyExc_TypeError, "[BroadcastSceneData] invalid params");
	//if(!PyArg_ParseTuple(args, "OO", &pListStr, &pListVal))
	//	return PyErr_Format(PyExc_TypeError, "[BroadcastSceneData] invalid params");
	if(!pListStr || !pListVal)
		return PyErr_Format(PyExc_TypeError, "[BroadcastSceneData] invalid params");

	_CHECK_SCENE_TYPE(0);
	//if(selfScene == g_region->m_defScene)
	//	return CScriptEngine::ReturnNone();
	long lListSize = PyList_Size(pListStr);
	lListSize = PyList_Size(pListVal) < lListSize ? PyList_Size(pListVal) : lListSize;
	char **pString = RT_NEW char*[lListSize];
	long *pValue = RT_NEW long[lListSize];
	for(int ii=0; ii<lListSize; ii++)
	{
		pString[ii] = PyString_AsString(PyList_GetItem(pListStr, ii));
		pValue[ii] = PyLong_AsLong(PyList_GetItem(pListVal, ii));
	}

	if(lPlayerID > 0)
	{
		CRegionCreature *pPlayer = selfScene->FindCreature(lPlayerID);
		if(pPlayer && pPlayer->m_userInfo)
		{
			g_sendCmd->BeginWrite();
			g_sendCmd->WriteShort(r2c_show_npc_info);
			g_sendCmd->WriteLong(lListSize);
			if(pString && pValue)
			{
				for(int ii=0; ii<lListSize; ii++)
				{
					g_sendCmd->WriteString(pString[ii]);
					char sValue[20] = {0};
					itoa(pValue[ii], sValue, 10);
					g_sendCmd->WriteString(sValue);
				}
			}
			pPlayer->m_userInfo->RecvCmd(g_sendCmd);
		}
	}
	else
		g_region->BroadcastSceneDataByScene(selfScene, lListSize, (const char**)pString, pValue);
	DEL_ARRAY(pValue);
	DEL_ARRAY(pString);
	return CScriptEngine::ReturnNone();
}

PyObject* CPythonRegionScene::CloseDungeon(region_object_python* self, PyObject* args)
{
	_CHECK_SCENE_TYPE(0);
	CRegionSceneFB *pFBScene = dynamic_cast<CRegionSceneFB*>(selfScene);
	if(pFBScene)
	{
		long lDunID = g_region->m_fbMgr->GetDungeonIDBySceneID(selfScene->GetObjectId());
		if(lDunID >= 0)
		{
			CRsDungeonMgr::SRsDungeon *pDungeon = g_region->m_fbMgr->FindDungeon(lDunID);
			if(pDungeon)
			{
				pDungeon->tLimitTime = 0;
				pDungeon->tEndTime = 0;
			}
			CG_CmdPacket &cmd = g_region->m_gws.BeginSend();
			cmd.WriteShort(r2g_destroy_dungeon);
			cmd.WriteLong(lDunID);
			cmd.WriteLong(2); // 关闭副本
			g_region->m_gws.EndSend();
		}
	}
	return CScriptEngine::ReturnNone();
}

PyObject* CPythonRegionScene::FBSceneUser(region_object_python* self, PyObject* args)
{
	_CHECK_SCENE_TYPE(0);
	CRegionSceneFB *pFBScene = dynamic_cast<CRegionSceneFB*>(selfScene);
	if(pFBScene)
	{
		long lDunID = g_region->m_fbMgr->GetDungeonIDBySceneID(selfScene->GetObjectId());
		CRsDungeonMgr::SRsDungeon *pDungeon = g_region->m_fbMgr->FindDungeon(lDunID);
		if(pDungeon)
		{
			PyObject* ret = PyTuple_New(static_cast<int>(pDungeon->listUser.size()));
			long lIndex = 0;
			for(std::list<DWORD>::iterator itUserID = pDungeon->listUser.begin();
				itUserID != pDungeon->listUser.end();
				lIndex++, itUserID++)
			{
				CRegionUser *pUser = g_region->FindUser(*itUserID);
				if(!pUser || !pUser->m_dummy)
					return Py_BuildValue("i", 0);
				PyTuple_SetItem(ret, lIndex, CScriptEngine::CreatePythonObject(pUser->m_dummy));
			}
			return ret;
		}
	}
	return Py_BuildValue("i", 0);
}

PyObject* CPythonRegionScene::FBSceneBroadcast(region_object_python* self, PyObject* args)
{
	char *pText = 0;
	long lColorR = -1;
	long lColorG = -1;
	long lColorB = -1;
	if(!PyArg_ParseTuple(args, "slll", &pText, &lColorR, &lColorG, &lColorB))
		return PyErr_Format(PyExc_TypeError, "[FBSceneBroadcast] invalid params");
	if(!pText || pText[0] == 0)
		return CScriptEngine::ReturnNone();
	if(lColorR > 255)
		lColorR = 255;
	if(lColorG > 255)
		lColorG = 255;
	if(lColorB > 255)
		lColorB = 255;
	if(lColorR < 0)
		lColorR = -1;
	if(lColorG < 0)
		lColorG = -1;
	if(lColorB < 0)
		lColorB = -1;
	lColorR = (lColorR & 0xff) << 16;
	lColorG = (lColorG & 0xff) << 8;
	lColorB = (lColorB & 0xff) << 0;
	long lRealColor = 0xff000000;
	lRealColor |= lColorR;
	lRealColor |= lColorG;
	lRealColor |= lColorB;

	_CHECK_SCENE_TYPE(0);
	CRegionSceneFB *pFBScene = dynamic_cast<CRegionSceneFB*>(selfScene);
	if(pFBScene)
	{
		long lDunID = g_region->m_fbMgr->GetDungeonIDBySceneID(selfScene->GetObjectId());
		CRsDungeonMgr::SRsDungeon *pDungeon = g_region->m_fbMgr->FindDungeon(lDunID);
		if(pDungeon)
		{
			for(std::list<DWORD>::iterator itUserID = pDungeon->listUser.begin();
				itUserID != pDungeon->listUser.end();
				itUserID++)
			{
				CRegionUser *pUser = g_region->FindUser(*itUserID);
				if(pUser && pUser->m_dummy)
				{
					g_sendCmd->BeginWrite();
					g_sendCmd->WriteShort(r2c_gm_bulletin);
					g_sendCmd->WriteLong(0);
					g_sendCmd->WriteString(pText);
					g_sendCmd->WriteLong(lRealColor);
					g_sendCmd->WriteByte(0);
					pUser->m_dummy->RecvCmd(g_sendCmd);
				}
			}
		}
	}
	return CScriptEngine::ReturnNone();
}

//
// init region object type
//
bool CPythonRegionScene::Initialize(PyObject* module)
{
	//
	// init region object type
	//
	m_pythonType.tp_base													= CPythonRegionObject::GetPythonType();
	if(PyType_Ready(&m_pythonType) < 0)
		return false;

	return true;
}

//
// create new
//
PyObject* CPythonRegionScene::CreatePythonObject(CRegionScene* theScene)
{
	region_object_python* ret												= PyObject_NEW(region_object_python, &m_pythonType);
	if(!ret)
		return 0;

	ret->m_pObject															= theScene;
	return reinterpret_cast<PyObject*>(ret);
}

//
// method
//
PyMethodDef CPythonRegionScene::m_pMethods[] = 
{
	{"AddNpc",						(PyCFunction)CPythonRegionScene::AddNpc,				METH_VARARGS | METH_KEYWORDS,	""},
	{"SetDefaultPos",				(PyCFunction)CPythonRegionScene::SetDefaultPos,			METH_VARARGS,					""},
	{"FindCreatureById",			(PyCFunction)CPythonRegionScene::FindCreatureById,		METH_O,							""},
	{"FindNpcByType",				(PyCFunction)CPythonRegionScene::FindNpcByType,			METH_O,							""},
	{"RemoveCreatureById",			(PyCFunction)CPythonRegionScene::RemoveCreatureById,	METH_O,							""},
	{"ActivationAreaByName",		(PyCFunction)CPythonRegionScene::ActivationAreaByName,	METH_VARARGS,					""},
	{"BroadcastShowTimer",			(PyCFunction)CPythonRegionScene::BroadcastShowTimer,	METH_VARARGS | METH_KEYWORDS,	""},
	{"BroadcastSceneData",			(PyCFunction)CPythonRegionScene::BroadcastSceneData,	METH_VARARGS | METH_KEYWORDS,	""},
	{"CloseDungeon",				(PyCFunction)CPythonRegionScene::CloseDungeon,			METH_NOARGS,					""},
	{"FBSceneUser",					(PyCFunction)CPythonRegionScene::FBSceneUser,			METH_NOARGS,					""},
	{"FBSceneBroadcast",			(PyCFunction)CPythonRegionScene::FBSceneBroadcast,		METH_VARARGS,					""},
	{0, 0},
};

//
// attribute
//
PyGetSetDef CPythonRegionScene::m_pAttributes[] = 
{
	{(char*)"name",						(getter)CPythonRegionScene::GetSceneName,					0},//场景名字
	{(char*)"averageLV",				(getter)CPythonRegionScene::GetAverageLV,					0},//FB场景用户等级
	{0, 0},
};

//
// region object type
//
PyTypeObject CPythonRegionScene::m_pythonType = 
{
	PyObject_HEAD_INIT(NULL)
	0,																	/*ob_size*/
	SCRIPT_PYTHON_MODULE_NAME ".RegionScene",							/*tp_name*/
	sizeof(region_object_python),										/*tp_basicsize*/
	0,																	/*tp_itemsize*/
	0,																	/*tp_dealloc*/
	0,																	/*tp_print*/
	0,																	/*tp_getattr*/
	0,																	/*tp_setattr*/
	0,																	/*tp_compare*/
	0,																	/*tp_repr*/
	0,																	/*tp_as_number*/
	0,																	/*tp_as_sequence*/
	0,																	/*tp_as_mapping*/
	0,																	/*tp_hash*/
	0,																	/*tp_call*/
	0,																	/*tp_str*/
	PyObject_GenericGetAttr,											/*tp_getattro*/
	PyObject_GenericSetAttr,											/*tp_setattro*/
	0,																	/*tp_as_buffer*/
	Py_TPFLAGS_DEFAULT,													/*tp_flags*/
	0,																	/*tp_doc*/
	0,																	/*tp_traverse*/
	0,																	/*tp_clear*/
	0,																	/*tp_richcompare*/
	0,																	/*tp_weaklistoffset*/
	0,																	/*tp_iter*/
	0,																	/*tp_iternext*/
	CPythonRegionScene::m_pMethods,										/*tp_methods*/
	0,																	/*tp_members*/
	CPythonRegionScene::m_pAttributes,									/*tp_getset*/
	0,																	/*tp_base*/
	0,																	/*tp_dict*/
	0,																	/*tp_descr_get*/
	0,																	/*tp_descr_set*/
	0,																	/*tp_dictoffset*/
	0,																	/*tp_init*/
	0,																	/*tp_alloc*/
	0,																	/*tp_new*/
	0,																	/*tp_free*/
	0,																	/*tp_is_gc*/
};
