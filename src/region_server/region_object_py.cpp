//********************************************************************
//	created:	24:12:2009   12:58
//	filename: 	region_object_py.cpp
//	author:		tiamo
//	purpose:	python interface for region object
//********************************************************************

#include "region.h"
#include "script_engine_py.h"
#include "region_object_py.h"

#define _CHECK_OBJECT_TYPE(ret)                                                                    \
    REGION_OBJECT_PYTHON_CHECK_TYPE(self, CRegionObject*, selfObject, ret)

//
// get object id
//
PyObject* CPythonRegionObject::GetObjectId(region_object_python* self, void* unused) {
    _CHECK_OBJECT_TYPE(0);
    return Py_BuildValue("l", selfObject->m_oId);
}

//
// get object type
//
PyObject* CPythonRegionObject::GetObjectType(region_object_python* self, void* unused) {
    _CHECK_OBJECT_TYPE(0);
    return Py_BuildValue("l", selfObject->m_type);
}

//
// get grid x
//
PyObject* CPythonRegionObject::GetGridX(region_object_python* self, void* unused) {
    _CHECK_OBJECT_TYPE(0);
    return Py_BuildValue("l", selfObject->m_gridX);
}

//
// get grid x
//
PyObject* CPythonRegionObject::GetGridY(region_object_python* self, void* unused) {
    _CHECK_OBJECT_TYPE(0);
    return Py_BuildValue("l", selfObject->m_gridY);
}

//
// get grid x
//
PyObject* CPythonRegionObject::GetGrid(region_object_python* self, void* unused) {
    _CHECK_OBJECT_TYPE(0);
    return Py_BuildValue("(ll)", selfObject->m_gridX, selfObject->m_gridY);
}

//
// set timer
//
PyObject* CPythonRegionObject::SetTimer(region_object_python* self, PyObject* args, PyObject* kw) {
    //
    // get param
    //
    long               id         = 0;
    long               interval   = 0;
    long               lParam[4]  = {-1};
    static char const* keywords[] = {"id", "interval", "p1", "p2", "p3", "p4", 0};
    if (!PyArg_ParseTupleAndKeywords(args, kw, "l|lllll:SetTimer", (char**)keywords, &id, &interval,
                                     &lParam[0], &lParam[1], &lParam[2], &lParam[3]))
        return PyErr_Format(PyExc_TypeError, "[SetTimer] invalid params");

    //
    // get creature
    //
    _CHECK_OBJECT_TYPE(0);
    selfObject->SetTimer(id + OBJECT_SCRIPT_TIMER_BASE, interval, (DWORD*)lParam);
    return CScriptEngine::ReturnNone();
}

PyObject* CPythonRegionObject::GetState(region_object_python* self, PyObject* args) {
    long lIndex = PyInt_AsLong(args);
    _CHECK_OBJECT_TYPE(0);
    return Py_BuildValue("i", selfObject->GetScriptStateValue(lIndex));
}

PyObject* CPythonRegionObject::SetState(region_object_python* self, PyObject* args) {
    long lIndex = 0;
    long lValue = 0;
    if (!PyArg_ParseTuple(args, "ll", &lIndex, &lValue))
        return PyErr_Format(PyExc_TypeError, "[object SetState] invalid params");
    _CHECK_OBJECT_TYPE(0);
    selfObject->SetScriptStateValue(lIndex, lValue);
    return CScriptEngine::ReturnNone();
}

PyObject* CPythonRegionObject::AddState(region_object_python* self, PyObject* args) {
    long lIndex = 0;
    long lValue = 0;
    if (!PyArg_ParseTuple(args, "ll", &lIndex, &lValue))
        return PyErr_Format(PyExc_TypeError, "[object AddState] invalid params");
    _CHECK_OBJECT_TYPE(0);
    selfObject->AddScriptStateValue(lIndex, lValue);
    return CScriptEngine::ReturnNone();
}

//
// init region object type
//
bool CPythonRegionObject::Initialize(PyObject* module) {
    //
    // init region object type
    //
    if (PyType_Ready(&m_pythonType) < 0)
        return false;

    //
    // add const
    //
    if (PyModule_AddIntConstant(module, "OBJECT_TYPE_OBJECT", OBJECT_TYPE_OBJECT) == -1)
        return false;

    if (PyModule_AddIntConstant(module, "OBJECT_TYPE_CREATURE", OBJECT_TYPE_CREATURE) == -1)
        return false;

    if (PyModule_AddIntConstant(module, "OBJECT_TYPE_ITEM", OBJECT_TYPE_ITEM) == -1)
        return false;

    if (PyModule_AddIntConstant(module, "OBJECT_TYPE_NPC_SERVICE", OBJECT_TYPE_NPC_SERVICE) == -1)
        return false;

    if (PyModule_AddIntConstant(module, "OBJECT_TYPE_NPC_COMBATIVE", OBJECT_TYPE_NPC_COMBATIVE) ==
        -1)
        return false;

    if (PyModule_AddIntConstant(module, "OBJECT_TYPE_SCENE", OBJECT_TYPE_SCENE) == -1)
        return false;

    if (PyModule_AddIntConstant(module, "OBJECT_TYPE_SCENE_FB", OBJECT_TYPE_SCENE_FB) == -1)
        return false;

    if (PyModule_AddIntConstant(module, "OBJECT_TYPE_SCENE_WARFIELD", OBJECT_TYPE_SCENE_WARFIELD) ==
        -1)
        return false;

    if (PyModule_AddIntConstant(module, "OBJECT_TYPE_MAX", OBJECT_TYPE_MAX) == -1)
        return false;

    return true;
}

//
// create new
//
PyObject* CPythonRegionObject::CreatePythonObject(CRegionObject* theObject) {
    region_object_python* ret = PyObject_NEW(region_object_python, &m_pythonType);
    if (!ret)
        return 0;

    ret->m_pObject = theObject;
    return reinterpret_cast<PyObject*>(ret);
}

//
// method
//
PyMethodDef CPythonRegionObject::m_pMethods[] = {
    {"SetTimer", (PyCFunction)CPythonRegionObject::SetTimer, METH_VARARGS | METH_KEYWORDS, ""},
    {"GetState", (PyCFunction)CPythonRegionObject::GetState, METH_O, ""},
    {"SetState", (PyCFunction)CPythonRegionObject::SetState, METH_VARARGS, ""},
    {"AddState", (PyCFunction)CPythonRegionObject::AddState, METH_VARARGS, ""},
    {0, 0},
};

//
// attribute
//
PyGetSetDef CPythonRegionObject::m_pAttributes[] = {
    {(char*)"object_id", (getter)CPythonRegionObject::GetObjectId, 0},
    {(char*)"object_type", (getter)CPythonRegionObject::GetObjectType, 0},
    {(char*)"grid_x", (getter)CPythonRegionObject::GetGridX, 0},
    {(char*)"grid_y", (getter)CPythonRegionObject::GetGridY, 0},
    {(char*)"grid", (getter)CPythonRegionObject::GetGrid, 0},

    {0, 0},
};

//
// region object type
//
PyTypeObject CPythonRegionObject::m_pythonType = {
    PyObject_HEAD_INIT(NULL) 0,                /*ob_size*/
    SCRIPT_PYTHON_MODULE_NAME ".RegionObject", /*tp_name*/
    sizeof(region_object_python),              /*tp_basicsize*/
    0,                                         /*tp_itemsize*/
    0,                                         /*tp_dealloc*/
    0,                                         /*tp_print*/
    0,                                         /*tp_getattr*/
    0,                                         /*tp_setattr*/
    0,                                         /*tp_compare*/
    0,                                         /*tp_repr*/
    0,                                         /*tp_as_number*/
    0,                                         /*tp_as_sequence*/
    0,                                         /*tp_as_mapping*/
    0,                                         /*tp_hash*/
    0,                                         /*tp_call*/
    0,                                         /*tp_str*/
    PyObject_GenericGetAttr,                   /*tp_getattro*/
    PyObject_GenericSetAttr,                   /*tp_setattro*/
    0,                                         /*tp_as_buffer*/
    Py_TPFLAGS_DEFAULT,                        /*tp_flags*/
    0,                                         /*tp_doc*/
    0,                                         /*tp_traverse*/
    0,                                         /*tp_clear*/
    0,                                         /*tp_richcompare*/
    0,                                         /*tp_weaklistoffset*/
    0,                                         /*tp_iter*/
    0,                                         /*tp_iternext*/
    CPythonRegionObject::m_pMethods,           /*tp_methods*/
    0,                                         /*tp_members*/
    CPythonRegionObject::m_pAttributes,        /*tp_getset*/
    0,                                         /*tp_base*/
    0,                                         /*tp_dict*/
    0,                                         /*tp_descr_get*/
    0,                                         /*tp_descr_set*/
    0,                                         /*tp_dictoffset*/
    0,                                         /*tp_init*/
    0,                                         /*tp_alloc*/
    0,                                         /*tp_new*/
    0,                                         /*tp_free*/
    0,                                         /*tp_is_gc*/
};
