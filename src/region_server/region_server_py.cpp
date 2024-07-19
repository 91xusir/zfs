//********************************************************************
//	created:	24:12:2009   15:04
//	filename: 	region_server_py.cpp
//	author:		tiamo
//	purpose:	region server interface
//********************************************************************

#include "region.h"
#include "script_engine_py.h"
#include "region_object_py.h"
#include "region_server_py.h"

//
// broadcast 
//
PyObject* CPythonRegionServer::Broadcast(region_object_python* self, PyObject* args)
{
	return CScriptEngine::ReturnNone();
}

//
// create new
//
PyObject* CPythonRegionServer::CreatePythonObject(CRegionServer* theServer)
{
	region_object_python* ret												= PyObject_NEW(region_object_python, &m_pythonType);
	if(!ret)
		return 0;

	ret->m_pObject															= theServer;
	return reinterpret_cast<PyObject*>(ret);
}

//
// init
//
bool CPythonRegionServer::Initialize(PyObject* module)
{
	//
	// setup type
	//
	m_pythonType.tp_base													= CPythonRegionObject::GetPythonType();
	if(PyType_Ready(&m_pythonType) < 0)
		return false;

	//
	// build region server
	//
	PyObject* theServer														= CPythonRegionServer::CreatePythonObject(g_region);
	if(!theServer)
		return false;

	//
	// add object
	//
	PyModule_AddObject(module, "region_server", theServer);

	return true;
}

//
// attribute
//
PyGetSetDef CPythonRegionServer::m_pAttributes[] = 
{
	{0, 0},
};

//
// method
//
PyMethodDef	CPythonRegionServer::m_pMethods[] = 
{
	{"Broadcast",					(PyCFunction)CPythonRegionServer::Broadcast,							METH_VARARGS, ""},
	{0, 0},
};

//
// type
//
PyTypeObject CPythonRegionServer::m_pythonType = 
{
	PyObject_HEAD_INIT(NULL)
	0,																	/*ob_size*/
	SCRIPT_PYTHON_MODULE_NAME ".RegionServer",							/*tp_name*/
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
	CPythonRegionServer::m_pMethods,									/*tp_methods*/
	0,																	/*tp_members*/
	CPythonRegionServer::m_pAttributes,									/*tp_getset*/
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

