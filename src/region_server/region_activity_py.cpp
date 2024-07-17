#include "region.h"
#include "region_object_py.h"
#include "script_engine_py.h"
#include "region_activity_py.h"

#define _CHECK_ACTIVITY_TYPE(ret)	REGION_OBJECT_PYTHON_CHECK_TYPE(self, CRegionActivityBase*, selfActivity, ret)

PyObject* CPythonRegionActivity::GetScene(region_object_python* self, void* unused)
{
	return CScriptEngine::CreatePythonObject(g_region->m_defScene);
}

PyObject* CPythonRegionActivity::AddLinkNPC(region_object_python* self, PyObject* args)
{
	long lNPCID = PyInt_AsLong(args);
	_CHECK_ACTIVITY_TYPE(0);
	//CRegionCreature *pCreature = g_region->m_defScene->FindCreature(lNPCID);
	//if(!pCreature || (pCreature->IsUser() || pCreature->IsUserPet()))
	//	return CScriptEngine::ReturnNone();
	selfActivity->AddLinkNPC(lNPCID);
	return CScriptEngine::ReturnNone();
}

PyObject* CPythonRegionActivity::GetLinkNPC(region_object_python* self, PyObject* args)
{
	long lIndex = PyInt_AsLong(args);
	_CHECK_ACTIVITY_TYPE(0);
	CRegionCreature *pCreature = selfActivity->GetLinkNPCByIndex(lIndex);
	if(pCreature)
		return CScriptEngine::CreatePythonObject(pCreature);
	return Py_BuildValue("i", 0);
}

PyObject* CPythonRegionActivity::GetAllLinkNPC(region_object_python* self, PyObject* args)
{
	typedef std::list<CRegionCreature*>	LISTNPC;
	LISTNPC listCreature;
	_CHECK_ACTIVITY_TYPE(0);
	selfActivity->GetAllLinkNPC(listCreature);
	if(listCreature.empty())
		return Py_BuildValue("(i)", 0);
	CScriptEngine* theEngine = CScriptEngine::GetSingleton();
	PyObject* pRet = theEngine->BuildCallArgs((int)listCreature.size());
	int iIndex = 0;
	for(LISTNPC::iterator itNPC = listCreature.begin();
		itNPC != listCreature.end();
		itNPC++)
		theEngine->BuildArg(pRet, iIndex++, *itNPC);
	return pRet;
}

PyObject* CPythonRegionActivity::BroadcastShowTimer(region_object_python* self, PyObject* args)
{
	region_object_python *pPyScene = 0;
	long lTimer = 0;
	if(!PyArg_ParseTuple(args, "Ol", &pPyScene, &lTimer))
		return PyErr_Format(PyExc_TypeError, "[BroadcastShowTimer] invalid params");
	//_CHECK_ACTIVITY_TYPE(0);
	g_region->BroadcastShowTimerByScene(dynamic_cast<CRegionScene*>(pPyScene->m_pObject), " ", lTimer);
	return CScriptEngine::ReturnNone();
}

PyObject* CPythonRegionActivity::BroadcastSceneData(region_object_python* self, PyObject* args)
{
	region_object_python *pPyScene = 0;
	PyObject *pListStr = 0;
	PyObject *pListVal = 0;
	if(!PyArg_ParseTuple(args, "OOO", &pPyScene, &pListStr, &pListVal))
		return PyErr_Format(PyExc_TypeError, "[BroadcastSceneData] invalid params");
	if(!pListStr || !pListVal || !pPyScene)
		return PyErr_Format(PyExc_TypeError, "[BroadcastSceneData] invalid params");

	//_CHECK_ACTIVITY_TYPE(0);
	long lListSize = PyList_Size(pListStr);
	lListSize = PyList_Size(pListVal) < lListSize ? PyList_Size(pListVal) : lListSize;
	char **pString = RT_NEW char*[lListSize];
	long *pValue = RT_NEW long[lListSize];
	for(int ii=0; ii<lListSize; ii++)
	{
		pString[ii] = PyString_AsString(PyList_GetItem(pListStr, ii));
		pValue[ii] = PyLong_AsLong(PyList_GetItem(pListVal, ii));
	}
	g_region->BroadcastSceneDataByScene(
		dynamic_cast<CRegionScene*>(pPyScene->m_pObject),
		lListSize, (const char**)pString, pValue);
	DEL_ARRAY(pValue);
	DEL_ARRAY(pString);
	return CScriptEngine::ReturnNone();
}

//
// init region object type
//
bool	CPythonRegionActivity::Initialize(PyObject* module)
{
	//
	// init region object type
	//
	m_pythonType.tp_base = CPythonRegionObject::GetPythonType();
	if(PyType_Ready(&m_pythonType) < 0)
		return false;

	return true;
}

//
// create new
//
PyObject*	CPythonRegionActivity::CreatePythonObject(CRegionActivityBase* theActivity)
{
	region_object_python* ret = PyObject_NEW(region_object_python, &m_pythonType);
	if(!ret)
		return 0;

	ret->m_pObject = theActivity;
	return reinterpret_cast<PyObject*>(ret);
}

//
// method
//
PyMethodDef CPythonRegionActivity::m_pMethods[] = 
{
	{"AddLinkNPC",					(PyCFunction)CPythonRegionActivity::AddLinkNPC,				METH_O,					""},
	{"GetLinkNPC",					(PyCFunction)CPythonRegionActivity::GetLinkNPC,				METH_O,					""},
	{"GetAllLinkNPC",				(PyCFunction)CPythonRegionActivity::GetAllLinkNPC,			METH_NOARGS,			""},
	{"BroadcastShowTimer",			(PyCFunction)CPythonRegionActivity::BroadcastShowTimer,		METH_VARARGS,			""},
	{"BroadcastSceneData",			(PyCFunction)CPythonRegionActivity::BroadcastSceneData,		METH_VARARGS,			""},
	{0, 0},
};

//
// attribute
//
PyGetSetDef CPythonRegionActivity::m_pAttributes[] = 
{
	{(char*)"scene",			(getter)CPythonRegionActivity::GetScene,				0},//≥°æ∞∂‘œÛ
	{0, 0},
};

//
// region object type
//
PyTypeObject CPythonRegionActivity::m_pythonType =
{
	PyObject_HEAD_INIT(NULL)
	0,												/*ob_size*/
	SCRIPT_PYTHON_MODULE_NAME ".RegionActivityBase",/*tp_name*/
	sizeof(region_object_python),					/*tp_basicsize*/
	0,												/*tp_itemsize*/
	0,												/*tp_dealloc*/
	0,												/*tp_print*/
	0,												/*tp_getattr*/
	0,												/*tp_setattr*/
	0,												/*tp_compare*/
	0,												/*tp_repr*/
	0,												/*tp_as_number*/
	0,												/*tp_as_sequence*/
	0,												/*tp_as_mapping*/
	0,												/*tp_hash*/
	0,												/*tp_call*/
	0,												/*tp_str*/
	PyObject_GenericGetAttr,						/*tp_getattro*/
	PyObject_GenericSetAttr,						/*tp_setattro*/
	0,												/*tp_as_buffer*/
	Py_TPFLAGS_DEFAULT,								/*tp_flags*/
	0,												/*tp_doc*/
	0,												/*tp_traverse*/
	0,												/*tp_clear*/
	0,												/*tp_richcompare*/
	0,												/*tp_weaklistoffset*/
	0,												/*tp_iter*/
	0,												/*tp_iternext*/
	CPythonRegionActivity::m_pMethods,				/*tp_methods*/
	0,												/*tp_members*/
	CPythonRegionActivity::m_pAttributes,			/*tp_getset*/
	0,												/*tp_base*/
	0,												/*tp_dict*/
	0,												/*tp_descr_get*/
	0,												/*tp_descr_set*/
	0,												/*tp_dictoffset*/
	0,												/*tp_init*/
	0,												/*tp_alloc*/
	0,												/*tp_new*/
	0,												/*tp_free*/
	0,												/*tp_is_gc*/
};
